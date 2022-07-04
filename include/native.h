#ifndef clox_native_h
#define clox_native_h

#include "value.h"

Value ticksNative(int argc, Value* args);
Value sleepNative(int argc, Value* args);
Value joystickNative(int argCount, Value* args);
Value readCharNative(int argCount, Value* args);
Value readLineNative(int argCount, Value* args);

// Casting
Value strNative(int argCount, Value* args);
Value numNative(int argCount, Value* args);

// Array List functions
Value pushNative(int argCount, Value* args);
Value popNative(int argCount, Value* args);
Value lenNative(int argCount, Value* args);
Value clearNative(int argCount, Value* args);

// Console
Value clsNative(int argCount, Value* args);

// Memory
Value peek8Native(int argCount, Value* args);
Value peek16Native(int argCount, Value* args);
Value peek32Native(int argCount, Value* args);
Value poke8Native(int argCount, Value* args);
Value poke16Native(int argCount, Value* args);
Value poke32Native(int argCount, Value* args);

/// Math
Value ceilNative(int argCount, Value* args);
Value floorNative(int argCount, Value* args);
Value roundNative(int argCount, Value* args);
Value absNative(int argCount, Value* args);
Value powNative(int argCount, Value* args);
Value expNative(int argCount, Value* args);
Value logNative(int argCount, Value* args);
Value log10Native(int argCount, Value* args);
Value log2Native(int argCount, Value* args);
Value sqrtNative(int argCount, Value* args);
Value sinNative(int argCount, Value* args);
Value cosNative(int argCount, Value* args);
Value tanNative(int argCount, Value* args);
Value asinNative(int argCount, Value* args);
Value acosNative(int argCount, Value* args);
Value atanNative(int argCount, Value* args);
Value sinhNative(int argCount, Value* args);
Value coshNative(int argCount, Value* args);
Value tanhNative(int argCount, Value* args);
Value randNative(int argCount, Value* args);

#endif