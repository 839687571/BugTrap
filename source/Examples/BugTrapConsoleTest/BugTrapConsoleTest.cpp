// BugTrapConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
using namespace std;
INT_PTR g_iLogHandle = -1;

string get_app_path()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szPath, MAX_PATH);

	string strPath(szPath);
	for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
		TCHAR cChar = strPath[nPos];
		if (_T('\\') == cChar || _T('/') == cChar)
			return strPath.substr(0, nPos + 1);
	}
	return strPath;
}

static void SetupExceptionHandler()
{
	// Setup exception handler
	BT_SetAppName(_T("BugTrap Console Test"));
	BT_SetSupportEMail(_T("your@email.com"));
	BT_SetFlags(BTF_DETAILEDMODE | BTF_ATTACHREPORT);

	// = BugTrapServer ===========================================
	BT_SetSupportServer(_T("localhost"), 9999);
	// - or -
	//BT_SetSupportServer(_T("127.0.0.1"), 9999);

	// = BugTrapWebServer ========================================
	//BT_SetSupportServer(_T("http://localhost/BugTrapWebServer/RequestHandler.aspx"), BUGTRAP_HTTP_PORT);

	// required for VS 2005 & 2008
	BT_InstallSehFilter();

	BT_SetActivityType(BTA_SAVEREPORT);
	BT_SetReportFilePath(get_app_path().c_str());

	// Add custom log file using default name
	g_iLogHandle = BT_OpenLogFile((get_app_path() + "mylog.log").c_str(), BTLF_STREAM);
//	BT_SetLogSizeInEntries(g_iLogHandle, 100);
	BT_SetLogSizeInBytes(g_iLogHandle, 1000);
	BT_SetLogFlags(g_iLogHandle, BTLF_SHOWTIMESTAMP);
	BT_SetLogEchoMode(g_iLogHandle, BTLE_STDERR | BTLE_DBGOUT);

	PCTSTR pszLogFileName = BT_GetLogFileName(g_iLogHandle);
	BT_AddLogFile(pszLogFileName);
}

static unsigned APIENTRY ThreadFunc(void* /*args*/)
{
	BT_SetTerminate(); // set_terminate() must be called from every thread
	//BT_InsLogEntry(g_iLogHandle, BTLL_INFO, _T("Entering ThreadFunc() function"));

	int* ptr = 0;
	*ptr = 0;


	DWORD index = 0;
	while (true) { 
		index++;
		BT_AppLogEntryF(g_iLogHandle, BTLL_INFO, _T("Entering ThreadFunc() functio Entering ThreadFunc() functio Entering ThreadFunc() function Entering ThreadFunc() function index  =  %d"),index);
	///	Sleep()
	}

	// Throwing access violation
// 	int* ptr = 0;
// 	*ptr = 0;

	// throwing exception
	//throw "exception";

	BT_InsLogEntry(g_iLogHandle, BTLL_INFO, _T("Leaving ThreadFunc() function"));
	return 0;
}

void _tmain()
{
	SetupExceptionHandler();
	BT_SetTerminate(); // set_terminate() must be called from every thread
	//BT_InsLogEntry(g_iLogHandle, BTLL_INFO, _T("Entering main() function"));

	// Starting worker thread
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	int* ptr = 0;
	*ptr = 0;
	while (1) {

	}
	BT_SetLogSizeInBytes(g_iLogHandle,1000);


	BT_InsLogEntry(g_iLogHandle, BTLL_INFO, _T("Leaving main() function"));
	BT_CloseLogFile(g_iLogHandle);
}
