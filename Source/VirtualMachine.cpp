#include "Assembly.h"

#include <windows.h>
#include <stdio.h>

#include "TyroDebug.h"



#define tofloat(x) (*((float *)x))
#define todword(x) (*((dword *)x))
#define tosigned(x) (*((long *)x))  // signed int


VirtualMachine::VirtualMachine() : stackpos(stack)
{
  memset(stack, 0, sizeof(stack));
}

VirtualMachine::~VirtualMachine()
{
}

bool VirtualMachine::Execute(Assembly& assembly)
{
  // check if there's a "SYS SC_EXIT" op at the end of the bytecode stream
  dword *lastop = assembly.GetByteCode() + assembly.GetSize() - 2;
  if(!(lastop[0] == SYS && lastop[1] == SC_EXIT))
    assembly.WriteOp(SYS, SC_EXIT); // if not, add it

  dword *localvars = null;
  dword *bytecode = assembly.GetByteCode();
  dword *cur = bytecode;
  stackpos = stack;

  dword a, b;

  // the only way we exit this loop is when we encounter "SYS SC_EXIT"
  for(;;) {
    dword opcode = *(cur ++);
    dword operand = *(cur ++);

    switch(opcode) {
      case NOOP:
        break;

      case LOCAL:
#ifdef _DEBUG
        memset(stackpos, 0xcafebabe, sizeof(dword) * operand);
#endif
        localvars = stackpos;
        stackpos += operand;
        break;

      case CALL:
        Call((Function *)assembly.GetFunctions()[operand]);
        break;

      case SYS:
        if(operand == SC_EXIT) return true;
        System((SYSCODE)operand);
        break;

      case PUSH:
        *(++ stackpos) = operand;
        break;

      case POP:
        stackpos --;
        break;

      case LOAD:
        *(++ stackpos) = localvars[operand];
        break;

      case STORE:
        localvars[operand] = *stackpos;
        break;

      case GOTO:
        cur = bytecode + operand;
        break;

      case IFT:
        if(*(stackpos --) == 1) cur = bytecode + operand;
        break;

      case IFF:
        if(*(stackpos --) == 0) cur = bytecode + operand;
        break;

      case IAND:
        b = *(stackpos --);
        a = *stackpos;
        *stackpos = (a && b) ? 1 : 0;
        break;

      case IOR:
        b = *(stackpos --);
        a = *stackpos;
        *stackpos = (a || b) ? 1 : 0;
        break;

      case IEQ:
        *stackpos = (*stackpos == 0) ? 1 : 0;
        break;

      case INE:
        *stackpos = (*stackpos != 0) ? 1 : 0;
        break;

      case ILT:
        *stackpos = (tosigned(stackpos) < 0) ? 1 : 0;
        break;

      case ILE:
        *stackpos = (tosigned(stackpos) <= 0) ? 1 : 0;
        break;

      case IGT:
        *stackpos = (tosigned(stackpos) > 0) ? 1 : 0;
        break;

      case IGE:
        *stackpos = (tosigned(stackpos) >= 0) ? 1 : 0;
        break;

      case I2F:
        *((float *)stackpos) = (float)*((long *)stackpos);
        break;

      case F2I:
        *((long *)stackpos) = (long)*((float *)stackpos);
        break;

      case IADD:
        b = *(stackpos --);
        a = *stackpos;
        *stackpos = a + b;
        break;

      case ISUB:
        b = *(stackpos --);
        a = *stackpos;
        *stackpos = a - b;
        break;

      case IMUL:
        b = *(stackpos --);
        a = *stackpos;
        *stackpos = a * b;
        break;

      case IDIV:
        b = *(stackpos --);
        a = *stackpos;
        *stackpos = a / b;
        break;

      case IMOD:
        b = *(stackpos --);
        a = *stackpos;
        *stackpos = tosigned(&a) % tosigned(&b);
        break;

      case FADD:
        b = *(stackpos --);
        a = *stackpos;
        *((float *)stackpos) = tofloat(&a) + tofloat(&b);
        break;

      case FSUB:
        b = *(stackpos --);
        a = *stackpos;
        *((float *)stackpos) = tofloat(&a) - tofloat(&b);
        break;

      case FMUL:
        b = *(stackpos --);
        a = *stackpos;
        *((float *)stackpos) = tofloat(&a) * tofloat(&b);
        break;

      case FDIV:
        b = *(stackpos --);
        a = *stackpos;
        *((float *)stackpos) = tofloat(&a) / tofloat(&b);
        break;

      case FCMP:
        b = *(stackpos --);
        a = *stackpos;
        if(tofloat(&a) < tofloat(&b)) *stackpos = -1;
        else if(tofloat(&a) > tofloat(&b)) *stackpos = 1;
        else *stackpos = 0;
        break;

      default:
        return false;
    }
  }

  return true;
}

bool VirtualMachine::System(SYSCODE operand)
{
  // all sys ops pop the values they use from the stack
  switch(operand) {
    case SC_PRINTC:
      putchar((char)*(stackpos --));
      return true;
      
    case SC_PRINTI:
      printf("%d\n", (int)*(stackpos --));
      return true;

    case SC_PRINTF:
      printf("%f", *((float *)&*(stackpos --)));
      return true;

    case SC_SLEEP:
      // note: platform dependant
      Sleep(*(stackpos --));
      return true;
  }

  return false;
}

bool VirtualMachine::Call(Function *function)
{
  dword pc = function->paramcount;
  dword r;
  void* pointer = function->pointer;

  // push the parameters on to the stack
  for(dword i = 0; i < pc; i ++) {
    dword a = *(stackpos --);
    __asm { push a }
  }

  // call the function
  // store the return value
  __asm {
    call pointer
    mov r, eax 
  }


  // do we need to pop the parameters passed (or have they been popped already)
  if(function->popparams) {
    // balance the stack frame
    for(dword i = 0; i < pc; i ++) {
      __asm { pop ebx }
    }
  }

  if(function->returncount == 0)
    r = 0; // don't push garbage on our stack, push a 0 instead
    
  // save the return value
  // note: all the functions return a value (void -> 0)
  *(++ stackpos) = r;
  
  return true;
}


void VirtualMachine::DumpStack()
{
  printf("\n");
  for(dword *j = stack, i = 0; j < stackpos; j ++, i ++) {
    printf("%08x ", *j);
    if((i + 1) % 8 == 0) printf("\n");
  }
  printf("\n");
}
