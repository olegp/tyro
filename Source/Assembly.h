#pragma once

#include "Tyro.h"
#include "VirtualMachine.h"

#include <string>
#include <vector>

using namespace std;

class Function
{
public:
  string name;
  void *pointer;
  dword paramcount;
  dword returncount;
  bool popparams;

  Function(const char *n, void *p, dword pc, dword rc) : 
    name(n), pointer(p), paramcount(pc), returncount(rc), popparams(true)
  { }

  Function(const char *n, void *p, dword pc, dword rc, bool pp) : 
    name(n), pointer(p), paramcount(pc), returncount(rc), popparams(pp)
  { }

};


class Assembly
{
  friend class Assembler;

  Function **functions;
  dword functioncount;

  dword *bytecode;
  dword curpos;
  
  enum Constant { BufferSize = 1024 };

public:

  Function** GetFunctions() { return functions; }
  bool SetFunctions(Function **functions, dword functioncount);

  bool WriteDword(dword value);
  bool WriteOp(dword opcode, dword operand);
  
  dword* GetByteCode();
  dword GetSize();

  void Clear();

  // saves the bytecode to a file
  bool Save(const char *filename);

  // loads the bytecode from file
  bool Load(const char *filename);

  Assembly();
  ~Assembly();
};

struct Label;
typedef vector<Label> LabelVector;

// static class
class Assembler
{
  static int AddLabel(string& name, LabelVector& labels);

  // changes the label indices to code positions and clears the labels vector
  static bool SetLabels(Assembly& assembly, LabelVector& labels);

  // converts a string to an operand using the info in opcode
  static dword StringToOperand(string& s, struct OpDesc *opcode);

  // reads a single word (w) from a string (s) starting at position (p)
  static int NextWord(string& s, string& w, int p = 0);

  // returns true if this is a control opcode
  static bool IsJumpOp(dword opcode);

  // parses a single line of assembly source
  static bool ParseLine(string& line, int linecount, Assembly& assembly, LabelVector& labels);

public:

  // converts source assembly to bytecode
  static bool Assemble(const char *filename, Assembly& assembly);

  // converts bytecode to source assembly
  static bool Disassemble(const char *filename, Assembly& assembly, bool hexops = false);

};