// NewFileDlg.cpp

#include "stdafx.h"
#include "resource.h"
#include "NewFileDlg.h"

/* static */ bool CNewFileDlg::m_bBraille2 = true;
/* static */ int CNewFileDlg::m_nExt = 0;

LRESULT CNewFileDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();
	
	// Set some default info. 
	// Give the file a temporary name.  Filename is not used but I set just in case.
	m_strName = _T("temp");
	m_strExt.LoadString(IDS_SKIPPER_EXT);
	m_strName += m_strExt.Right(4);
	m_nExt = 0; 

	// Set the default directory.
	CString str;
	//str.LoadString(IDS_MY_DOCUMENTS);
	//m_strDirectory = _T('\\') + str + _T('\\');

	// Make the static text a child of the window.
	//CStatic ctrlAsk;
	//ctrlAsk.Attach(GetDlgItem(IDC_ASK_GRADE_TWO));

	//CButton ctrlYes;
	//ctrlYes.Attach(GetDlgItem(IDYES));
	//ctrlYes.SetFocus();

	return 0;
}

LRESULT CNewFileDlg::OnYes(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_bBraille2 = true;
	EndDialog(IDOK);
	return 0;
}

LRESULT CNewFileDlg::OnNo(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_bBraille2 = false;
	EndDialog(IDOK);
	return 0;
}

LRESULT CNewFileDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}