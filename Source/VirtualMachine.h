#pragma once

#include "Tyro.h"


// when updating these, make sure to update opcodes[] in Assembler.cpp
enum OPCODE
{
  // misc ops
  NOOP  = 0,    // does nothing, usually a jump target
  SYS,          // executes a system command, see SYSCODE
  PUSH,         // pushes the operand on the stack
  POP,          // pops the stack

  LOAD,
  STORE,

  LOCAL,        // reserves a new local variable array from the stack
  CALL,         // calls a native function

  // control ops

  GOTO,         // unconditional jump, the operand is the offset 
                // from the beginning of bytecode data in bytes
  
  IFT,          // jump if the current value on stack is 1 (true)
  IFF,          // jump if the current value on stack is 0 (false)

  IEQ,
  INE,
  ILT,
  ILE,
  IGT,
  IGE,

  // type conversion
  I2F,
  F2I,
  
  // boolean arithmetic
  IAND,
  IOR,

  // integer arithmetic
  IADD,
  ISUB,
  IMUL,
  IDIV,

  IMOD,

  // float arithmetic
  FADD,
  FSUB,
  FMUL,
  FDIV,

  FCMP,
};

enum SYSCODE
{
  SC_EXIT    = 0,    // stops execution
  SC_PRINTC  = 1,    // prints the char currently on stack
  SC_PRINTI  = 2,    // prints an int
  SC_PRINTF  = 3,    // prints a float
  SC_SLEEP   = 4,    // sleeps, ms specified by the int on stack
};

class Assembly;
class Function;

class VirtualMachine
{
  dword stack[256];
  dword *stackpos;

  dword vararray[256];

public:
  
  VirtualMachine();
  ~VirtualMachine();

  bool System(SYSCODE operand);
  bool Call(Function *function);

  bool Execute(Assembly& assembly);

  // prints all the values on the stack, for debugging
  void DumpStack();
};
