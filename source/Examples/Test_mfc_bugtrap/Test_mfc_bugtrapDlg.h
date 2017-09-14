
// Test_mfc_bugtrapDlg.h : header file
//

#pragma once


// CTest_mfc_bugtrapDlg dialog
class CTest_mfc_bugtrapDlg : public CDialogEx
{
// Construction
public:
	CTest_mfc_bugtrapDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TEST_MFC_BUGTRAP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
