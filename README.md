# FoenixLox

An implementation of the Lox language as described in the book [Crafting Interpreters](https://craftinginterpreters.com) by Robert Nystrom.


## Features

### From Original Lox language
- Classes, Functions, Garabage Collection, and Bytecode VM

### Extra Syntax
- Hex Literals, Character Literals, List Data type

### Runtime Library
- Casting functions
  - str, num

- List functions
  - push, pop, len, clear

- Hardware funtions:
  - joystick, readChar, readLine
  - cls, ticks, sleep
  - peek8, peek16, peek32
  - poke8, poke16, poke32

- Math functions
  - ceil, floor, round, abs, pow, exp
  - log, log10, log2, sqrt, sin, cos,tan
  - asin, acos, atan, sinh, cosh, tanh, rand

## Requirements

This project requires the [Calypsi cc68k compiler](https://www.calypsi.cc) and Make.
A Mac OS, Linux, or WSL for Windows environment is recommended.

## Related
- [Foenix Edit](https://github.com/daschewie/FoenixEdit)
- [Lox Documentation](https://craftinginterpreters.com/the-lox-language.html)
