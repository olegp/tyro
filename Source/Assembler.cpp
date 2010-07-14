#include "Assembly.h"

#include "TyroDebug.h"

// this structure contains info that describes an op
struct OpDesc
{
  dword code;       // opcode, see enum OPCODE in "VirtualMachine.h"
  const char *name; // the name of the opcode as a lowercase string
  byte paramcount;  // the number of parameters that the op takes (0 or 1)

} opcodes[] = {
  // these are stored in order of appearance i.e.: opcodes[opcode].code == opcode
  
  {NOOP, "noop", 0},
  {SYS,  "sys", 1},
  {PUSH, "push", 1},
  {POP,  "pop", 0},

  {LOAD, "load", 1},
  {STORE, "store", 1},

  {LOCAL, "local", 1},

  {CALL, "call", 1},

  {GOTO, "goto", 1},

  {IFT, "ift", 1},
  {IFF, "iff", 1},

  {IEQ, "ieq", 0},
  {INE, "ine", 0},
  {ILT, "ilt", 0},
  {ILE, "ile", 0},
  {IGT, "igt", 0},
  {IGE, "ige", 0},

  {I2F, "i2f", 0},
  {F2I, "f2i", 0},

  {IAND, "iand", 0},
  {IOR,  "ior", 0},

  {IADD, "iadd", 0},
  {ISUB, "isub", 0},
  {IMUL, "imul", 0},
  {IDIV, "idiv", 0},

  {IMOD, "imod", 0},

  {FADD, "fadd", 0},
  {FSUB, "fsub", 0},
  {FMUL, "fmul", 0},
  {FDIV, "fdiv", 0},

  {FCMP, "fcmp", 0},
};

struct Label
{
  string label;
  dword pos;

  Label(string l) : label(l), pos(-1)
  {
  }
};

int Assembler::AddLabel(string& name, LabelVector& labels)
{
  int index = 0;
  LabelVector::iterator i;
  for (i = labels.begin(); i != labels.end(); i ++, index ++) {
    if(i->label == name)
      return index;
  }

  labels.push_back(Label(name));
  return index;
}

bool Assembler::SetLabels(Assembly& assembly, LabelVector& labels)
{
  dword *bytecode = assembly.bytecode;
  dword curpos = assembly.curpos;

  size_t size = labels.size();
  for(dword *cp = bytecode; cp < bytecode + curpos; cp += 2) {
    if(IsJumpOp(cp[0])) {
      if(cp[1] < size) {
        dword pos = labels.at(cp[1]).pos;
        if(pos != -1)
          cp[1] = pos;
        else {
          // this is a common error
          printf("Label not found: %s\n", labels.at(cp[1]).label.c_str());
          labels.clear();
          return false;
        }
      } else {
        // if we get here, there's a bug in the assembler ;(
        printf("Label index not found: %d\n", cp[1]);
        labels.clear();
        return false;
      }
    }
  }
  
  labels.clear();
  return true;
}

bool Assembler::IsJumpOp(dword opcode)
{
  return (opcode >= GOTO && opcode <= IFF);
}

int Assembler::NextWord(string& s, string& w, int p)
{      
  int slength = (int)s.length();
  if(p >= slength) return -1;

  basic_string <char>::iterator i = s.begin() + p;
  int j = p; while(isspace(*i) && j < slength) { i ++; j ++; }
  int k = 0; while(!isspace(*i) && j + k < slength) { i ++; k ++; }

  if(j == slength) k = -1;
  else w = s.substr(j, k);
  return k;
}

dword Assembler::StringToOperand(string& s, OpDesc *opcode)
{
  const char *cstr = s.c_str();
  size_t slength = s.length();
  if(slength == 0) return 0;

  // chars are quoted with single quotes, e.g. "push 'A'"
  if(slength == 3) {
    if(cstr[0] == '\'' && cstr[2] == '\'')
      return (dword)cstr[1];
  } else if(slength == 4) {
    // also take into account newlines '\n' and such
    if(cstr[0] == '\'' && cstr[1] == '\\' && cstr[3] == '\'') {
      switch(cstr[2]) {
        case 'n': return (dword)'\n';
        case 'r': return (dword)'\r';
        case 'b': return (dword)'\b';
        case 't': return (dword)'\t';
        default: return 0; // also '\0'
      }
    }
  }

  // all float operands end with an 'f', e.g. "1.23f" or contain a decimal point
  if(tolower(cstr[slength - 1]) == 'f' || s.find(".") != string::npos) {
    float f = (float)atof(cstr);
    return *((dword *)&f);
  }


  // convert to integer, this also works with strings like "0x0ffff012"
  long i = strtol(cstr, null, 0);
  // check if this is a bool
  if(i == 0) return stricmp(cstr, "true") ? 0 : 1;
  return *((dword *)&i);
}

bool Assembler::ParseLine(string& line, int linecount, Assembly& assembly, LabelVector& labels)
{
  string op;
  int i = NextWord(line, op);
  
  // skip commented and empty lines
  if(i == -1 || op == "//" || op == ";") return true;

  // this is a a very slow way of doing things, but then this code will only
  // be used during development and for debugging
  OpDesc *opcode;
  int codecount = sizeof(opcodes)/sizeof(OpDesc); 
  int j = 0;
  for(; j < codecount; j ++) {
    if(stricmp(opcodes[j].name, op.c_str()) == 0) {
      opcode = &opcodes[j];      
      break;
    }
  }

  // is this a label?
  if(j == codecount) {
    size_t k = op.length() - 1;
    if(op.find_first_of(":") == k) {
      string name = op.substr(0, k);
      labels.at(AddLabel(name, labels)).pos = assembly.curpos;
      return true;
    } else
      return false; 

  } else {

    assembly.WriteDword(opcode->code);

    string param;
    i = NextWord(line, param, i);

    if(IsJumpOp(opcode->code)) {
      // write the label index, we'll later replace this with the code position
      assembly.WriteDword(AddLabel(param, labels)); 
    } else {  
      if(i == -1) {
        assembly.WriteDword(0);
        if(opcode->paramcount > 0) return false; // if we need more params, bail
      } else
        assembly.WriteDword(StringToOperand(param, opcode));
    }
  }

  return true;
}

bool Assembler::Assemble(const char *filename, Assembly& assembly)
{
  assembly.Clear();
  LabelVector labels;

  FILE *in = fopen(filename, "rt");
  if(in == null) return false;
 
  char buffer[256];

  int linecount = 0;
  while(fgets(buffer, sizeof(buffer), in)) {
    size_t lc = strlen(buffer) - 1;
    if(buffer[lc] == '\n') buffer[lc] = '\0';

    string line(buffer);
    if(ParseLine(line, linecount, assembly, labels) == false) {
      printf("Syntax error on line %d: \"%s\"\n", linecount + 1, buffer);
      fclose(in);
      return false;
    }
    linecount ++;
  }

  fclose(in);

  // replace label indices with code positions
  return SetLabels(assembly, labels);
}

bool Assembler::Disassemble(const char *filename, Assembly& assembly, bool hexops)
{
  FILE *out = fopen(filename, "wt");
  if(out == null) return false;

  dword *bytecode = assembly.bytecode;

  dword *pos = bytecode;
  dword *end = bytecode + assembly.curpos;

  while(pos < end) {
    dword opcode = *(pos ++);
    dword operand = *(pos ++);
    if(IsJumpOp(opcode)) operand = (operand / 2) + 1;

    if(opcodes[opcode].paramcount > 0)
      fprintf(out, hexops ? "%s\t0x%08x\n" : "%s\t%d\n", opcodes[opcode].name, operand);
    else
      fprintf(out, "%s\n", opcodes[opcode].name);
  }

  fclose(out);
  return true;
}



