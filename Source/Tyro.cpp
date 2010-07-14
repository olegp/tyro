#include "Assembly.h"
#include "Compiler.h"
#include <time.h>
#include <windows.h>

#include "TyroDebug.h"


void print(dword i)
{
  printf("%d\n", i);
}

Function imports[] = {
  Function("rand", rand, 0, 1),
  Function("seed", srand, 1, 0),
  Function("time", time, 1, 1),
  Function("print", print, 1, 0),

  Function("sleep", Sleep, 1, 0, false),
};


void main()
{
  // search for memory leaks in debug mode
#ifdef _DEBUG 
#ifdef _WIN32
		int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		flag |= _CRTDBG_LEAK_CHECK_DF;                  
		_CrtSetDbgFlag(flag);                           
#endif
#endif

  ImportList importlist;
  importlist.AddList(imports, sizeof(imports)/sizeof(imports[0]));

  Assembly assembly;

  Compiler compiler;
  if(compiler.Compile("src.txt", assembly, importlist)) {
    Assembler::Disassemble("asm.txt", assembly);
    
    VirtualMachine machine;
    if(!machine.Execute(assembly)) {
      puts("Execution failed!");
      puts("Dumping stack:");
      machine.DumpStack();
    }

  } else
    puts("Compilation failed!");
}

