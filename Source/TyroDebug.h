// Tyro script Debug header file
// should be included in each source file _after_ all other includes

#pragma once

#ifdef WIN32
// search for memory leaks in debug mode
#ifdef _DEBUG
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define malloc _malloc_dbg(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif // _DEBUG

#endif // WIN32