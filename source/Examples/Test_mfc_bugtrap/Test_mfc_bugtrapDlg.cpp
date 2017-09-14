
// Test_mfc_bugtrapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test_mfc_bugtrap.h"
#include "Test_mfc_bugtrapDlg.h"
#include "afxdialogex.h"
#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTest_mfc_bugtrapDlg dialog



CTest_mfc_bugtrapDlg::CTest_mfc_bugtrapDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTest_mfc_bugtrapDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest_mfc_bugtrapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTest_mfc_bugtrapDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTest_mfc_bugtrapDlg::OnBnClickedOk)
END_MESSAGE_MAP()


wstring get_app_path()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szPath, MAX_PATH);

	wstring strPath(szPath);
	for (int nPos = (int)strPath.size() - 1; nPos >= 0; --nPos) {
		TCHAR cChar = strPath[nPos];
		if (_T('\\') == cChar || _T('/') == cChar)
			return strPath.substr(0, nPos + 1);
	}
	return strPath;
}

BOOL CTest_mfc_bugtrapDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Setup exception handler
	BT_SetAppName(_T("BugTrap Test"));
	BT_SetSupportEMail(_T("your@email.com"));
	BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL | BTF_ATTACHREPORT | BTF_SCREENCAPTURE);
	BT_SetSupportURL(_T("http://www.intellesoft.net"));

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


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTest_mfc_bugtrapDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTest_mfc_bugtrapDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTest_mfc_bugtrapDlg::OnBnClickedOk()
{
	int* ptr = 0;
	*ptr = 0;
	//CDialogEx::OnOK();
}
