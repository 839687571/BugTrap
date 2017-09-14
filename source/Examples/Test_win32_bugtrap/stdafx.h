// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// Include main BugTrap header.
#include <BugTrap.h>

// Link with one of BugTrap libraries.
#if defined _M_IX86
#ifdef _UNICODE
#pragma comment(lib, "BugTrapU.lib")
#else
#pragma comment(lib, "BugTrap.lib")
#endif
#elif defined _M_X64
#ifdef _UNICODE
#pragma comment(lib, "BugTrapU-x64.lib")
#else
#pragma comment(lib, "BugTrap-x64.lib")
#endif
#else
#error CPU architecture is not supported.
#endif

// TODO: reference additional headers your program requires here
