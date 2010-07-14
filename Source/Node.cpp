#include "Compiler.h"
#include "Lex.h"

#include "TyroDebug.h"

//*** Node

Node::Node(NodeType t) : type(t), rettype(DT_VOID), symbol(null), line(lineno)
{
  child[0] = child[1] = child[2] = null;
}

Node::Node(NodeType t, Node *a) : type(t), rettype(DT_VOID), symbol(null), line(lineno)
{
  child[0] = a;
  child[1] = child[2] = null;
}

Node::Node(NodeType t, Node *a, Node *b) : type(t), rettype(DT_VOID), symbol(null), line(lineno)
{
  child[0] = a;
  child[1] = b;
  child[2] = null;
}

Node::Node(NodeType t, Node *a, Node *b, Node *c) : type(t), rettype(DT_VOID), symbol(null), line(lineno)
{
  child[0] = a;
  child[1] = b;
  child[2] = c;
}



//*** Op

Op::Op() : offset(0), opcode(NOOP), operand(0), 
  target(null), symbol(null), next(null)
{
}

Op::Op(OPCODE oc, dword o) : offset(0), opcode(oc), operand(o), 
  target(null), symbol(null), next(null)
{
}

Op::Op(OPCODE oc) : offset(0), opcode(oc), operand(0), 
  target(null), symbol(null), next(null)
{
}

Op::Op(OPCODE oc, Op *t) : offset(0), opcode(oc), operand(0), 
  target(t), symbol(null), next(null)
{
}

Op::Op(OPCODE oc, Symbol *s) : offset(0), opcode(oc), operand(0), 
  target(null), symbol(s), next(null)
{
}

Op::~Op()
{
  safe_delete(next);
}

Op* Op::Concat(Op *op)
{  
  Op *cop;
  for(cop = this; cop->next != null; cop = cop->next) {
    // something wrong here, op is already part of this chain
    if(cop == op) {
      return this;
    }
  }

  cop->next = op;
  return this;
}

//*** ImportList

bool ImportList::Add(Function *function)
{
  FunctionTable::iterator i = functions.find(function->name);
  
  // doesn't exist yet - add it
  if(i == functions.end()) {
    functions.insert(FunctionTable::value_type(function->name, function));
    return true;
  }
   
  // couldn't add it, name already reserved
  return false;
}

bool ImportList::AddList(Function functions[], dword count)
{
  bool r = true;
  for(dword i = 0; i < count; i ++) {
    if(Add(&functions[i]) == false) r = false;
  }
  
  return r;
}

void ImportList::Clear()
{
  // clear the functions map
  functions.clear();
}

ImportList::~ImportList()
{
  Clear();
}
 
//*** Symbol

Symbol::Symbol(const char *c, dword l) : contents(c), line(l), index(0), data(null) 
{ 
}


dword Symbol::ToDword()
{
  int i = atoi(contents.c_str());
  return *((dword *)&i);
}


