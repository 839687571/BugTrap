
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions



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



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars











