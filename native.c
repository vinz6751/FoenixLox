#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mcp/syscalls.h"
#include "value.h"
#include "object.h"
#include "native.h"

Value ticksNative(int argc, Value* args) {
    return NUMBER_VAL(sys_time_jiffies());
}

Value sleepNative(int argc, Value* args) {
    if (argc !=1 || !IS_NUMBER(args[0])) {
        //error
    }
    int delay = AS_NUMBER(args[0]);
    long end = sys_time_jiffies() + delay;
    while(sys_time_jiffies() < end);
    return NIL_VAL;
}

// Casting
Value strNative(int argCount, Value* args) {
    char buffer[20];
    Value value = args[0];
    switch (value.type) {
        case VAL_BOOL: {
            if (AS_BOOL(value)) {
                return OBJ_VAL(copyString("true", 4));   
            } else {
                return OBJ_VAL(copyString("false", 5)); 
            }
        }
        case VAL_NIL: {
            return OBJ_VAL(copyString("nil", 3));
        }
        case VAL_NUMBER: {
            sprintf(buffer,"%g", AS_NUMBER(value));
            return OBJ_VAL(copyString(buffer, strlen(buffer)));
        }
        case VAL_OBJ: {
            return OBJ_VAL(copyString("<object>", 3));
        }
    }
}

Value numNative(int argCount, Value* args) {
    char *str = AS_CSTRING(args[0]);
    double num = 0;
    sscanf(str, "%g", &num);
    return NUMBER_VAL(num);
}

// List

Value pushNative(int argCount, Value* args) {
    // Append a value to the end of a list increasing the list's length by 1
    if (argCount != 2 || !IS_LIST(args[0])) {
        // Handle error
        return NIL_VAL;
    }
    ObjList* list = AS_LIST(args[0]);
    Value item = args[1];
    appendToList(list, item);
    return item;
}

Value popNative(int argCount, Value* args) {
    // Pop with no index, deletes last record;
    if (argCount == 1 && IS_LIST(args[0])) {
        ObjList* list = AS_LIST(args[0]);
        return deleteFromList(list, list->count - 1);
    } else if (argCount == 2 && IS_LIST(args[0]) && IS_NUMBER(args[1])) {
        ObjList* list = AS_LIST(args[0]);
        int index = AS_NUMBER(args[1]);
        if (!isValidListIndex(list, index)) {
            // Handle error
            return NIL_VAL;
        }
        return deleteFromList(list, index);
    }

    return NIL_VAL;
}

Value lenNative(int argCount, Value* args) {
    if (argCount == 1 && IS_LIST(args[0])) {
        ObjList* list = AS_LIST(args[0]);
        return NUMBER_VAL(list->count);
    } else if (argCount == 1 && IS_STRING(args[0])) {
        ObjString* str = AS_STRING(args[0]);
        return NUMBER_VAL(str->length);
    } else {
        return NUMBER_VAL(0);
    }
}

Value clearNative(int argCount, Value* args) {
    if (argCount != 1 || !IS_LIST(args[0])) {
        //error
    }
    ObjList* list = AS_LIST(args[0]);
    for (int i=0; i < list->count; i++) {
        list->items[i] = NIL_VAL;
    }
    list->count = 0;
    return NIL_VAL;
}

Value clsNative(int argCount, Value* args) {
    char *s = "\x1B[2J\x1B[H";
    sys_chan_write(0, (unsigned char *)s, strlen(s));
    return NIL_VAL;
}

Value peek8Native(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        uint32_t address = AS_NUMBER(args[0]);
        unsigned char *pointer = (unsigned char *) address;
        return NUMBER_VAL(*pointer);
    }
    return NUMBER_VAL(-1);
}

Value peek16Native(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        uint32_t address = AS_NUMBER(args[0]);
        uint16_t *pointer = (uint16_t *) address;
        return NUMBER_VAL(*pointer);
    }
    return NUMBER_VAL(-1);
}

Value peek32Native(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        uint32_t address = AS_NUMBER(args[0]);
        uint32_t *pointer = (uint32_t *) address;
        return NUMBER_VAL(*pointer);
    }
    return NUMBER_VAL(-1);
}

Value poke8Native(int argCount, Value* args) {
    if (argCount == 2 && IS_NUMBER(args[0]) && IS_NUMBER(args[1])) {
        uint32_t address = AS_NUMBER(args[0]);
        unsigned char *pointer = (unsigned char *) address;
        *pointer = AS_NUMBER(args[1]);
    }
    return NIL_VAL;
}

Value poke16Native(int argCount, Value* args) {
    if (argCount == 2 && IS_NUMBER(args[0]) && IS_NUMBER(args[1])) {
        uint32_t address = AS_NUMBER(args[0]);
        uint16_t *pointer = (uint16_t *) address;
        *pointer = AS_NUMBER(args[1]);
    }
    return NIL_VAL;
}

Value poke32Native(int argCount, Value* args) {
    if (argCount == 2 && IS_NUMBER(args[0]) && IS_NUMBER(args[1])) {
        uint32_t address = AS_NUMBER(args[0]);
        uint32_t *pointer = (uint32_t *) address;
        *pointer = AS_NUMBER(args[1]);
    }
    return NIL_VAL;
}

/// Math
Value ceilNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(ceil(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value floorNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(floor(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value roundNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(round(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value absNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(fabs(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value powNative(int argCount, Value* args) {
    if (argCount == 2 && IS_NUMBER(args[0]) && IS_NUMBER(args[1])) {
        double a = AS_NUMBER(args[0]);
        double b = AS_NUMBER(args[0]);
        return NUMBER_VAL(pow(a,b));
    }
    return NUMBER_VAL(-1); // handle error
}

Value expNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(exp(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value logNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(log(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value log10Native(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(log10(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value log2Native(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(log2(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value sqrtNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(sqrt(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value sinNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(sin(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value cosNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(cos(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value tanNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(tan(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value asinNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(asin(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value acosNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(acos(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value atanNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(atan(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value sinhNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(sinh(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value coshNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(cosh(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value tanhNative(int argCount, Value* args) {
    if (argCount == 1 && IS_NUMBER(args[0])) {
        double n = AS_NUMBER(args[0]);
        return NUMBER_VAL(tanh(n));
    }
    return NUMBER_VAL(-1); // handle error
}

Value randNative(int argCount, Value* args) {
    return NUMBER_VAL(rand());
}

Value joystickNative(int argCount, Value* args) {
    volatile unsigned int * joystick_port = (volatile unsigned int *)0xFEC00500;
    volatile unsigned int * game_ctrl_port = (volatile unsigned int *)0xFEC00504;
    unsigned int joy_state = 0;
    *game_ctrl_port = 0;
    joy_state = *joystick_port;
    return NUMBER_VAL(joy_state);
}

Value readCharNative(int argCount, Value* args) {
    return NUMBER_VAL(sys_chan_read_b(0));
}

Value readLineNative(int argCount, Value* args) {
    char buffer[255];
    short len = sys_chan_readline(0, (unsigned char *)buffer, 255);
    if (len > 0) {
        return OBJ_VAL(copyString(buffer, len));
    } else {
        return NIL_VAL;
    }
}