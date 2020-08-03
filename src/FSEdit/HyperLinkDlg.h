#ifndef __HYPERLINKDLG_H__12321RPM179458390
#define __HYPERLINKDLG_H__12321RPM179458390

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning (disable:4018)
#include "resource.h"
#include "skipperview.h"


class CHyperLinkDlg : public CDialogImpl<CHyperLinkDlg>
{
	UINT m_nLinkTo;
	CString strLinkText;
	CEdit m_edtLinkText;
public:
	CHyperLinkDlg(CString str): strLinkText(str) {}
	~CHyperLinkDlg(){}

	enum {IDD = IDD_HYPER_LINK_DLG};

	BEGIN_MSG_MAP(CHyperLinkDlg)
	COMMAND_RANGE_HANDLER(IDC_RADIO1, IDC_RADIO4, OnClickedRadio)
	COMMAND_HANDLER(IDCANCEL, BN_CLICKED, OnCancel)
	COMMAND_HANDLER(IDOK, BN_CLICKED, OnOK)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
	END_MSG_MAP()

	LRESULT OnClickedRadio(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		m_nLinkTo = wID - IDC_RADIO1;
		
		return 0;
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
		EndDialog(IDOK);
		return 0;
	}
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		// TODO : Add Code for message handler. Call DefWindowProc if necessary.
		return 0;
	}
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_nLinkTo = 0;
		CheckDlgButton(IDC_RADIO1, BST_CHECKED);
		m_edtLinkText.Attach(GetDlgItem(IDC_TEXT_TO_DISPLAY));
		m_edtLinkText.SetWindowText(strLinkText);
		return 0;
	}
};





#endif