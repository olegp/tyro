#pragma once

// define a lower case NULL as well (it looks nicer ;)
#define null NULL

// some often used macros
#define safe_delete(x) if(x != null) { delete x; x = null; }
#define safe_delete_array(x) if(x != null) { delete[] x; x = null; }

// some common types redefined
typedef unsigned char   byte;
typedef unsigned short  word;
typedef unsigned long   dword;


