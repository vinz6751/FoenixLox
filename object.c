#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "string_output.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)

static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;
  object->isMarked = false;
  
  object->next = vm.objects;
  vm.objects = object;

#ifdef DEBUG_LOG_GC
  printf("%p allocate %zu for %d\n", (void*)object, size, type);
#endif

  return object;
}
ObjBoundMethod* newBoundMethod(Value receiver,
                               ObjClosure* method) {
  ObjBoundMethod* bound = ALLOCATE_OBJ(ObjBoundMethod,
                                       OBJ_BOUND_METHOD);
  bound->receiver = receiver;
  bound->method = method;
  return bound;
}
ObjClass* newClass(ObjString* name) {
  ObjClass* klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
  klass->name = name; // [klass]
  initTable(&klass->methods);
  return klass;
}
ObjClosure* newClosure(ObjFunction* function) {
  ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*,
                                   function->upvalueCount);
  for (int i = 0; i < function->upvalueCount; i++) {
    upvalues[i] = NULL;
  }

  ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
  closure->function = function;
  closure->upvalues = upvalues;
  closure->upvalueCount = function->upvalueCount;
  return closure;
}
ObjFunction* newFunction() {
  ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
  function->arity = 0;
  function->upvalueCount = 0;
  function->name = NULL;
  initChunk(&function->chunk);
  return function;
}
ObjInstance* newInstance(ObjClass* klass) {
  ObjInstance* instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
  instance->klass = klass;
  initTable(&instance->fields);
  return instance;
}
ObjNative* newNative(NativeFn function) {
  ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  return native;
}

/* Strings allocate-string < Hash Tables allocate-string
static ObjString* allocateString(char* chars, int length) {
*/
static ObjString* allocateString(char* chars, int length,
                                 uint32_t hash) {
  ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  string->hash = hash;

  push(OBJ_VAL(string));
  tableSet(&vm.strings, string, NIL_VAL);
  pop();

  return string;
}
static uint32_t hashString(const char* key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}
ObjString* takeString(char* chars, int length) {
/* Strings take-string < Hash Tables take-string-hash
  return allocateString(chars, length);
*/
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length,
                                        hash);
  if (interned != NULL) {
    FREE_ARRAY(char, chars, length + 1);
    return interned;
  }

  return allocateString(chars, length, hash);
}
ObjString* copyString(const char* chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString* interned = tableFindString(&vm.strings, chars, length,
                                        hash);
  if (interned != NULL) return interned;

  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
/* Strings object-c < Hash Tables copy-string-allocate
  return allocateString(heapChars, length);
*/
  return allocateString(heapChars, length, hash);
}
ObjUpvalue* newUpvalue(Value* slot) {
  ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
  upvalue->closed = NIL_VAL;
  upvalue->location = slot;
  upvalue->next = NULL;
  return upvalue;
}

ObjList* newList() {
  ObjList* list = ALLOCATE_OBJ(ObjList, OBJ_LIST);
  list->items = NULL;
  list->count = 0;
  list->capacity = 0;
  return list;
}

void appendToList(ObjList* list, Value value) {
  // Grow the array if necessary
  if (list->capacity < list->count + 1) {
    int oldCapacity = list->capacity;
    list->capacity = GROW_CAPACITY(oldCapacity);
    list->items = GROW_ARRAY(Value, list->items, oldCapacity, list->capacity);
  }
  list->items[list->count] = value;
  list->count++;
  return;
}

void storeToList(ObjList* list, int index, Value value) {
  int new_index = (index < 0) ? list->count + index : index;
  list->items[new_index] = value;
}

Value indexFromList(ObjList* list, int index) {
  int new_index = (index < 0) ? list->count + index : index;
  return list->items[new_index];
}

Value indexFromString(ObjString* str, int index) {
  int new_index = (index < 0) ? str->length + index : index;
  return NUMBER_VAL(str->chars[new_index]);
}

Value deleteFromList(ObjList* list, int index) {
  int new_index = (index < 0) ? list->count + index : index;
  Value result = list->items[new_index];
  for (int i = new_index; i < list->count - 1; i++) {
    list->items[i] = list->items[i+1];
  }
  list->items[list->count - 1] = NIL_VAL;
  list->count--;
  return result;
}

bool isValidListIndex(ObjList* list, int index) {
  if (index < -(list->count) || index > list->count - 1) {
    return false;
  }
  return true;
}

bool isValidStringIndex(ObjString* str, int index) {
  if (index < -(str->length) || index > str->length - 1) {
    return false;
  }
  return true;
}

static void printFunction(ObjFunction* function) {
  if (function->name == NULL) {
    print_cstring("<script>");
    return;
  }
  print_cstring("<fn ");
  print_cstring(function->name->chars);
  print_char('>');
}
void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_BOUND_METHOD:
      printFunction(AS_BOUND_METHOD(value)->method->function);
      break;
    case OBJ_CLASS:
      print_cstring(AS_CLASS(value)->name->chars);
      break;
    case OBJ_CLOSURE:
      printFunction(AS_CLOSURE(value)->function);
      break;
    case OBJ_FUNCTION:
      printFunction(AS_FUNCTION(value));
      break;
    case OBJ_INSTANCE:
      print_cstring(AS_INSTANCE(value)->klass->name->chars);
      print_cstring(" instance");
      break;
    case OBJ_NATIVE:
      print_cstring("<native fn>");
      break;
    case OBJ_STRING:
      print_string_obj((ObjString*)AS_OBJ(value));
      break;
    case OBJ_UPVALUE:
      print_cstring("upvalue");
      break;
    case OBJ_LIST: {
      ObjList *list = AS_LIST(value);
      print_char('[');
      for(int i=0; i < list->count; i++) {
        Value cur = list->items[i];
        if (i>0)
          print_cstring(", ");
        if (IS_STRING(cur)) {
          print_char('\"');
          printValue(cur);
          print_char('\"');
        } else {
          printValue(cur);
        }        
      }
      print_char(']');
      break;
    }
  }
}
