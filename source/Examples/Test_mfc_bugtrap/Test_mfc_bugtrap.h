
// Test_mfc_bugtrap.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CTest_mfc_bugtrapApp:
// See Test_mfc_bugtrap.cpp for the implementation of this class
//

class CTest_mfc_bugtrapApp : public CWinApp
{
public:
	CTest_mfc_bugtrapApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CTest_mfc_bugtrapApp theApp;