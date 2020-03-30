// BugTrapConsoleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <iostream>

using namespace std;
INT_PTR g_iLogHandle = -1;
INT_PTR g_iLogHandle2 = -1;

wstring get_app_path()
{
	wchar_t szPath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, szPath, MAX_PATH);

	wstring strPath(szPath);
	for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
		wchar_t cChar = strPath[nPos];
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
	g_iLogHandle = BT_OpenLogFile((get_app_path() + L"mylog.log").c_str(), BTLF_STREAM);
	g_iLogHandle2 = BT_OpenLogFile((get_app_path() + L"mylog2.log").c_str(), BTLF_STREAM);

	///BT_SetLogSizeInEntries(g_iLogHandle2, 100);
	BT_SetLogSizeInBytes(g_iLogHandle, 10000);
	BT_SetLogFlags(g_iLogHandle, BTLF_SHOWTIMESTAMP);
	BT_SetLogEchoMode(g_iLogHandle, BTLE_STDERR | BTLE_DBGOUT); 
	BT_SetLogUnixTime(g_iLogHandle, 1510131386);
		
	BT_SetLogSizeInBytes(g_iLogHandle2, 10000);
	BT_SetLogFlags(g_iLogHandle2, BTLF_SHOWTIMESTAMP);
	BT_SetLogEchoMode(g_iLogHandle2, BTLE_STDERR | BTLE_DBGOUT);

	PCTSTR pszLogFileName = BT_GetLogFileName(g_iLogHandle);
	BT_AddLogFile(pszLogFileName);

	PCTSTR pszLogFileName2 = BT_GetLogFileName(g_iLogHandle2);
	BT_AddLogFile(pszLogFileName2);

}

static unsigned APIENTRY ThreadFunc(void* /*args*/)
{
	std::cout << "in thread enter " << std::endl;
	BT_SetTerminate(); // set_terminate() must be called from every thread
	//BT_InsLogEntry(g_iLogHandle, BTLL_INFO, _T("Entering ThreadFunc() function"));


	std::cout << "in thread enter 2" << std::endl;


	int* ptr = 0;
	*ptr = 0;


	std::cout << "in thread " << std::endl;

	DWORD index = 0;
	while (true) { 
		index++;
		BT_AppLogEntryF(g_iLogHandle, BTLL_INFO, _T("Entering ThreadFunc() functio Entering ThreadFunc() functio Entering ThreadFunc() function Entering ThreadFunc() function index  =  %d"),index);
		Sleep(100);

		//log 2
		BT_AppLogEntryF(g_iLogHandle2, BTLL_INFO, _T("Entering ThreadFunc() functio Entering ThreadFunc() functio Entering ThreadFunc() function Entering ThreadFunc() function index  =  %d"), index);
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
	//SetupExceptionHandler();
	BT_SetTerminate(); // set_terminate() must be called from every thread  // 这个函数捕获C++异常.
	//BT_InsLogEntry(g_iLogHandle, BTLL_INFO, _T("Entering main() function"));

	std::cout << "start run thread" << std::endl;
	// Starting worker thread
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);


	std::cout << "thread end" << std::endl;
	//int* ptr = 0;
	//*ptr = 0;
	while (1) {

		//Sleep(1000);

	}
	BT_SetLogSizeInBytes(g_iLogHandle,1000);


	BT_InsLogEntry(g_iLogHandle, BTLL_INFO, _T("Leaving main() function"));
	BT_CloseLogFile(g_iLogHandle);
}
