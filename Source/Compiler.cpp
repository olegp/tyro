#include "Assembly.h"
#include "Compiler.h"
#include "Lex.h"

#include <stdarg.h>

#include "TyroDebug.h"


//*** Compiler

// This function is called by the lexer when the end-of-file
// is reached; you can reset yyin (the input FILE*) and return 0
// if you want to process another file; otherwise just return 1.
extern "C" int yywrap() 
{
  return 1;
}

void yyerror(char *string)
{
  Compiler::GetActive()->Error(string, lineno);
}

// extern from Parse.cpp
int yyparse();

Compiler* Compiler::active = null;

Compiler::Compiler() : tree(null), filename(null), errorcount(0)
{
}

Compiler::~Compiler()
{
  safe_delete(tree);
}

Symbol* Compiler::GetVariable(const char *name)
{
  SymbolTable::iterator i = variables.find(name);
  
  // doesn't exist yet - create it
  if(i == variables.end()) {
    Symbol *symbol = new Symbol (name, lineno);
    variables.insert(SymbolTable::value_type(name, symbol));
    return symbol;
  }
   
  return (*i).second;
}

Symbol* Compiler::GetConstant(const char *name)
{
  SymbolTable::iterator i = constants.find(name);
  
  if(i == constants.end()) {
    Symbol *symbol = new Symbol (name, lineno);
    constants.insert(SymbolTable::value_type(name, symbol));
    return symbol;
  }
   
  return (*i).second;
}

Symbol* Compiler::GetFunction(const char *name)
{
  SymbolTable::iterator i = functions.find(name);
  
  if(i == functions.end()) {
    Symbol *symbol = new Symbol (name, lineno);
    functions.insert(SymbolTable::value_type(name, symbol));
    return symbol;
  }
   
  return (*i).second;
}

void Compiler::Error(const char *message, dword line)
{
  if(filename != null) { 
    printf("%s", filename);
    if(line != 0) 
      printf("(%d) : ", line);
    else 
      printf(" : ");
  }

  if(message != null)
    printf(message);
  else
    printf("unknown error");

  printf("\n");
  errorcount ++;
}

void Compiler::Error(const char *message, Node *node)
{
  Error(message, node != null ? node->line : 0);
}


#define ClearMap(type, map) { forEach(type, map, i) delete (*i).second; map.clear(); }
#define SetIndices(type, map) { dword j = 0; for(type::iterator i = map.begin(); i != map.end(); i ++, j ++) (*i).second->index = j; }

bool Compiler::Compile(const char *filename, Assembly& assembly, ImportList& importlist)
{
  yyin = fopen(filename, "rt");
  if(yyin == null) return false;

  active = this;
  errorcount = 0;
  this->filename = filename;

  // build the syntax tree from source
  yyparse();

  fclose(yyin);
  yyin = null;

  if(tree == null) return false;

  // set symbol indices
  SetIndices(SymbolTable, variables);

  // set function indices
  SetIndices(SymbolTable, functions);

  // clear the assembly (just in case it contained something)
  assembly.Clear();

  // move the functions from the symbol table to the assembly (using the list of imports)
  // we do it before checking semantics because this function sets Symbol::data which CheckSemantics needs
  MoveFunctions(assembly, importlist);

    
  // check the semantics
  CheckSemantics(tree);

  if(errorcount > 0) return false;
  
  // build op sequence
  Op *op = Build(tree);
  if(op == null) return false;

  // convert op sequence to bytecode
  Assemble(op, assembly);

  // clear the symbol tables
  ClearMap(SymbolTable, constants);
  ClearMap(SymbolTable, variables);

  ClearMap(SymbolTable, functions);

  // delete the op sequence
  safe_delete(op);
  
  active = null;
  errorcount = 0;
  filename = null;
  return true;
}

bool Compiler::CheckFunctionSemantics(Node *node)
{

  // do we even have the necessary data?
  if(node->symbol == null || node->symbol->data == null) return false;
  Function *function = (Function *)node->symbol->data;

  // count the number of parameters
  Node *n;
  dword paramcount = 0;
  // the second child (child[1]) should be an NT_PARAM node
  for(n = node->child[0]; n != null; n = n->child[1]) {
    if(n->type != NT_EMPTY) {
      paramcount ++;

      // parameter type checking should be done here

    } else {
    }

    if(n->type != NT_PARAM) break;
  }

  // does the number of parameters match?
  if(paramcount == function->paramcount) return true;
  else {
    char buffer[256];
    sprintf(buffer, "\'%s\' : function does not take %d parameters", function->name.c_str(), paramcount);
    Error(buffer, node);
    return false;
  }
}


bool Compiler::CheckSemantics(Node *node)
{
  for(int i = 0; i < sizeof(node->child)/sizeof(Node *); i ++) {
    if(node->child[i] != null) {
      CheckSemantics(node->child[i]);
    }
  }

  // set the return type
  switch (node->type)  {
    case NT_ERROR:
      break;

    // statements have no return value
    case NT_STMT: 
    case NT_EMPTY: 
    case NT_EXPR:
    case NT_WHILE:
    case NT_DOWHILE:
    case NT_IFTHEN:
    case NT_IFTHENELSE: 
      node->rettype = DT_VOID;
      break;

    case NT_EQUAL:
    case NT_NEQUAL:
    case NT_LESS:
    case NT_LEQUAL:
    case NT_GREATER:
    case NT_GEQUAL:
    case NT_BOOLAND:
    case NT_BOOLOR:
      node->rettype = DT_BOOL;
      break;

    case NT_ADD:
    case NT_SUB:
    case NT_MUL:
    case NT_DIV:
    case NT_ASSIGN:
      node->rettype = DT_INT;
      break;

    case NT_IDENT:
    case NT_INT:
      node->rettype = DT_INT;
      break;
  }


  // check semantics
  switch (node->type)  {
    case NT_IFTHEN:
    case NT_IFTHENELSE:
      break;

    case NT_WHILE:
    case NT_DOWHILE:
      break;

    case NT_EQUAL:
    case NT_NEQUAL:
    case NT_LESS:
    case NT_LEQUAL:
    case NT_GREATER:
    case NT_GEQUAL:
    case NT_BOOLAND:
    case NT_BOOLOR:
      break;

    case NT_ADD:
    case NT_SUB:
    case NT_MUL:
    case NT_DIV:
      break;

    case NT_ASSIGN: 
      break;

    case NT_CALL:
      CheckFunctionSemantics(node);
      break;
  }

  return true;
}

Op* Compiler::Build(Node *node)
{
  Op *a, *b, *c, *d, *e, *f;

  switch (node->type)  {

    case NT_ERROR:
      return new Op(NOOP);

    case NT_STMT:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      
      a->Concat(b);
      return a;

    case NT_EMPTY:
      return new Op(NOOP);

    case NT_PARAM:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      
      a->Concat(b);
      return a;

    case NT_CALL:
      a = Build(node->child[0]);
      b = new Op(CALL, node->symbol->index);

      a->Concat(b);
      return a;

    case NT_EXPR:
      a = Build(node->child[0]);
      b = new Op(POP);

      a->Concat(b);
      return a;
          
    case NT_WHILE:
      a = Build(node->child[0]);
      b = new Op(IFF);
      c = Build(node->child[1]);
      d = new Op(GOTO, a);
      e = new Op(NOOP);

      b->target = e;

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      d->Concat(e);
      return a;

    case NT_DOWHILE:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(IFT, a);

      a->Concat(b);
      b->Concat(c);
      return a;

    case NT_IFTHEN:
      a = Build(node->child[0]);
      b = new Op(IFF);
      c = Build(node->child[1]);
      d = new Op(NOOP);

      b->target = d;

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      return a;
      
    case NT_IFTHENELSE:
      a = Build(node->child[0]);
      b = new Op(IFF);
      c = Build(node->child[1]);
      d = new Op(GOTO);
      e = Build(node->child[2]);
      f = new Op(NOOP);
      
      b->target = e;
      d->target = f;

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      d->Concat(e);
      e->Concat(f);
      return a;

    case NT_EQUAL:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(ISUB);
      d = new Op(IEQ);

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      return a;
          
    case NT_NEQUAL:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(ISUB);
      d = new Op(INE);

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      return a;

    case NT_LESS:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(ISUB);
      d = new Op(ILT);

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      return a;

    case NT_LEQUAL:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(ISUB);
      d = new Op(ILE);

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      return a;

    case NT_GREATER:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(ISUB);
      d = new Op(IGT);

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      return a;

    case NT_GEQUAL:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(ISUB);
      d = new Op(IGE);

      a->Concat(b);
      b->Concat(c);
      c->Concat(d);
      return a;

    case NT_BOOLAND:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(IAND);

      a->Concat(b);
      b->Concat(c);
      return a;

    case NT_BOOLOR:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      c = new Op(IOR);

      a->Concat(b);
      b->Concat(c);
      return a;

    case NT_ASSIGN:
      a = Build(node->child[0]);
      b = new Op(STORE, node->symbol);

      a->Concat(b);
      return a;
      
    case NT_ADD:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      a->Concat(b);
      b->Concat(new Op(IADD));
      return a;

    case NT_SUB:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      a->Concat(b);
      b->Concat(new Op(ISUB));
      return a;
    
    case NT_MUL:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      a->Concat(b);
      b->Concat(new Op(IMUL));
      return a;

    case NT_DIV:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      a->Concat(b);
      b->Concat(new Op(IDIV));
      return a;

    case NT_MOD:
      a = Build(node->child[0]);
      b = Build(node->child[1]);
      a->Concat(b);
      b->Concat(new Op(IMOD));
      return a;


    case NT_IDENT:
      return new Op(LOAD, node->symbol);
    
    case NT_INT:
      return new Op(PUSH, node->symbol->ToDword());
   }

   return new Op(NOOP);
}

bool Compiler::Assemble(Op *op, Assembly& assembly)
{
  // set the local variable array size
  Op *first = new Op(LOCAL, (dword)variables.size());
  first->next = op;

  // set the offsets, we need these for jump targets
  dword coffset = 0;
  for(Op *cop = first; cop != null; cop = cop->next) {
    cop->offset = coffset;
    if(cop->opcode != NOOP)
      coffset += 2;
  }

  for(Op *cop = first; cop != null; cop = cop->next) {
    if(cop->opcode != NOOP) {
      assembly.WriteDword(cop->opcode);
      if(cop->symbol != null) 
        assembly.WriteDword(cop->symbol->index);
      else if(cop->target != null) 
        assembly.WriteDword(cop->target->offset);
      else
        assembly.WriteDword(cop->operand);
    }
  }

  // force an exit
  assembly.WriteOp(SYS, SC_EXIT);
  return true;
}

bool Compiler::MoveFunctions(Assembly& assembly, ImportList& importlist)
{
  dword pointercount = (dword)functions.size();
  Function **pointers = new Function*[pointercount];

  dword k = 0;
  forEach(SymbolTable, functions, i) {
    FunctionTable::iterator j = importlist.functions.find((*i).second->contents);
    if(j == importlist.functions.end()) {
      char buffer[256];
      sprintf(buffer, "reference to unknown function \'%s\'\n", (*i).second->contents.c_str());
      Error(buffer);
    } else {
      pointers[k] = (*j).second;
      (*i).second->data = (*j).second;
    }
    
    k ++;
  }

  assembly.SetFunctions(pointers, pointercount);

  return true;
}
