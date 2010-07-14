#include <stdio.h>
#include <stdlib.h>

#include "Assembly.h"

#include "TyroDebug.h"



Assembly::Assembly() : curpos(0), bytecode(null), functioncount(0), functions(null)
{
}

void Assembly::Clear()
{
  safe_delete_array(bytecode);
  safe_delete_array(functions);

  functioncount = 0;
  curpos = 0;
}


Assembly::~Assembly()
{
  Clear();
}


bool Assembly::WriteDword(dword value)
{
  if(bytecode == null) bytecode = new dword[BufferSize];

  bytecode[curpos ++] = value;

  // expand the buffer on demand
  if(curpos % BufferSize == 0) {
    dword *buffer = new dword[curpos + BufferSize];
    memcpy(buffer, bytecode, sizeof(dword) * curpos);
    delete[] bytecode;
    bytecode = buffer;
  }

  return true;
}

bool Assembly::WriteOp(dword opcode, dword operand)
{
  WriteDword(opcode);
  WriteDword(operand);
  return true;
}


dword* Assembly::GetByteCode()
{
  return bytecode;
}

dword Assembly::GetSize()
{
  return curpos;
}


bool Assembly::Save(const char *filename)
{
  FILE *out = fopen(filename, "wb");
  if(out == null) return false;
  fwrite(&curpos, sizeof(dword), 1, out);
  fwrite(bytecode, sizeof(dword), curpos, out);
  fclose(out);
  return true;
}

bool Assembly::Load(const char *filename)
{
  Clear();

  FILE *in = fopen(filename, "rb");
  if(in == null) return false;

  fread(&curpos, sizeof(dword), 1, in);
  bytecode = new dword[curpos];

  fread(bytecode, sizeof(dword), curpos, in);
  fclose(in);
  return true;
}

bool Assembly::SetFunctions(Function **functions, dword functioncount)
{
  safe_delete_array(this->functions);
  this->functions = functions;
  this->functioncount = functioncount;
  return true;
}