// NewFileDlg.h

#ifndef	__NEWFILEDLG_H__12893750918750918750
#define __NEWFILEDLG_H__12893750918750918750

#pragma once

class CNewFileDlg : public CDialogImpl<CNewFileDlg>
{
public:
	enum {IDD = IDD_NEWFILE_DLG};

	static int m_nExt;
	static bool m_bBraille2;
	static int GetFileExt(){return m_nExt;}

	CString m_strDirectory;
	CString m_strExt;
	CString m_strName;

	CString GetFileName(){return m_strName;}
	CString GetDirectory(){return m_strDirectory;}

	BEGIN_MSG_MAP(CNewFileDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDYES, OnYes)
		COMMAND_ID_HANDLER(IDNO, OnNo)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnYes(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnNo(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
};

class CCopiesDlg : public CDialogImpl<CCopiesDlg>
{
	int m_nCopies;
	int doubleSpace; // tracks flag
public:
	enum {IDD = IDD_COPIES_DLG};

	CCopiesDlg(int nCopies = 1) : m_nCopies(nCopies), doubleSpace(0) {}
	CButton m_btnDoubleSpace;

	int GetNumCopies() {return m_nCopies;}
	int IsDoubleSpaceRequired() { return doubleSpace == 1;}

	BEGIN_MSG_MAP(CCopiesDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnOK)
		COMMAND_ID_HANDLER(ID_COPIES_CANCEL, OnCancel)
		COMMAND_HANDLER(IDC_PRINT_DOUBLE_SPACE, BN_CLICKED, OnSetDoubleSpace)	
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_nCopies < 1)
		{
			EndDialog(IDCANCEL);
			return 0;
		}

		CEdit edit;
		edit.Attach(GetDlgItem(IDC_NUM_OF_COPIES));
		
		CString str;
		str.Format(_T("%d"), m_nCopies);
		edit.SetWindowText(str);
		edit.SetFocus();
		edit.SetSel(0, -1);

    	m_btnDoubleSpace.Attach(GetDlgItem(IDC_PRINT_DOUBLE_SPACE ));
		m_btnDoubleSpace.SetCheck( doubleSpace == 1? TRUE : FALSE );
		return 0;
	}

	// This handler gets both the IDOK and IDCANCEL messages.
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		TCHAR lpstr[10];
		memset(lpstr, 0, sizeof(TCHAR)*10);
		::GetWindowText(GetDlgItem(IDC_NUM_OF_COPIES), lpstr, 10);
		m_nCopies = _ttoi(lpstr);
		EndDialog(IDOK);
		return 0;
	}

	// This handler processes the ID_OPTIONS_CANCEL message generated by the
	// "Cancel" button so the IDCANCEL message can be directed to OnOK.
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	LRESULT OnSetDoubleSpace(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		if(m_btnDoubleSpace.GetCheck()) 
		    doubleSpace = 1;
		else 
		    doubleSpace = 0;
		return 0;
	}
};
#endif