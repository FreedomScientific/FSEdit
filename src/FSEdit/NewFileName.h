// NewFileName.h

#ifndef	__NEWFILENAME_H__12893750918750918750
#define __NEWFILENAME_H__12893750918750918750

#pragma once

class CNewFileName : public CDialogImpl<CNewFileName>
{
	CEdit m_edtNewFileName;
	CComboBox m_cbxFileType;
	CComboBox m_cbxFolders;
	CButton m_btnOK;
	CButton m_xbxBraille2;

	CString m_strDirectory;
	CString m_strExt;
	static int m_nExt;

	CString m_strName;
public:
	enum {IDD = IDD_NEW_FILENAME};

	static bool m_bBraille2;
	static int GetFileExt(){return m_nExt;}
	CString GetFileName(){return m_strName;}
	CString GetDirectory(){return m_strDirectory;}

	BEGIN_MSG_MAP(CNewFileName)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_HANDLER(IDEDT_NEW_NAME, EN_CHANGE, OnChange)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
};

#endif