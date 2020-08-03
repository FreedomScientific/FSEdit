// FSFindReplaceDlg.cpp: implementation of the CFSFindReplaceDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "FSFindReplaceDlg.h"

LRESULT CFSFindReplaceDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Subclass edit fields to CBEdit class.
	m_edtFind.SubclassWindow(GetDlgItem(ID_EDIT_FIND));
	if (!m_bFind)
		m_edtReplace.SubclassWindow(GetDlgItem(ID_EDIT_REPLACE));
	
	m_btnMatchCase.Attach(GetDlgItem(ID_CHECK_MATCH_CASE));
	m_btnMatchWhole.Attach(GetDlgItem(ID_CHECK_MATCH_WHOLE));
	m_btnHelp.Attach(GetDlgItem(ID_BUTTON_HELP));

DocProp Prop;
	m_view.GetDocumentProp(Prop);
	if(Prop.brailleGrade2)
	{  // if it is a grade 2 document we want to disable the match case option
		m_btnMatchCase.SetCheck( FALSE );
		m_btnMatchCase.EnableWindow( FALSE );
	} // end if the document is grade 2
	
	// Prepare dialog interface.
	if(m_bFind)
		FindText(&m_fr);
	else
		ReplaceText(&m_fr);

	m_edtFind.SetFocus();
	m_edtFind.SetSel(0, -1);
	return 0;
}

/* 
Finds or replaces word here.
Sets mark at the end of the word and places caret at beginning
of the word. Replace All places the mark on the last word found
and the caret at the beginning of the last word found.
*/
LRESULT CFSFindReplaceDlg::OnFind(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_fr.Flags = FR_FINDNEXT;
	GetDialogInfo();

	FINDTEXTEX ft = SetFindInfo(m_view, m_fr.lpstrFindWhat);
	m_nSuccess = SearchForText(m_view, ft, m_fr.Flags);

	::SendMessage(m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING), 0, (LPARAM)&m_fr);
	PostMessage(WM_CLOSE, 0, 0);
	return 0;
}

LRESULT CFSFindReplaceDlg::OnReplace(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_fr.Flags = FR_REPLACE;
	GetDialogInfo();

	FINDTEXTEX ft = SetFindInfo(m_view, m_fr.lpstrFindWhat);
	m_nSuccess = SearchForText(m_view, ft, m_fr.Flags);
	// RG not referenced: FINDTEXTEX ft = SetFindInfo(m_view, m_fr.lpstrFindWhat);
	CHARRANGE cr;
	m_view.GetSel(cr);
//	long lMark = m_view.GetMarker();
//	if(lMark != -1)
	if(cr.cpMax != cr.cpMin)
	{	
//		m_view.SetSel(min(lMark, cr.cpMax), max(lMark, cr.cpMax)); //cr.cpMin, cr.cpMax);
		m_view.SetSel(cr.cpMin, cr.cpMax);
		m_view.ReplaceSel(m_fr.lpstrReplaceWith);
//		m_view.SetSel(cr.cpMin, cr.cpMin);
//		m_view.FindReplaceCommand(cr.cpMax);
	}

	::SendMessage(m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING), 0, (LPARAM)&m_fr);
	PostMessage(WM_CLOSE, 0, 0);
	return 0;
}

LRESULT CFSFindReplaceDlg::OnReplaceAll(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_fr.Flags = FR_REPLACEALL;
	GetDialogInfo();
	
	FINDTEXTEX ft = SetFindInfo(m_view, m_fr.lpstrFindWhat);
	CHARRANGE cr{};
	long lIndex = 0;
	ft.chrg.cpMin = 0; // Because this is ReplaceAll, start search at the beginning.
	do
	{
		lIndex = m_view.FindText(m_fr.Flags, ft);
		if(lIndex != -1)
		{
			cr = ft.chrgText;
			m_view.SetSel(ft.chrgText);
			m_view.ReplaceSel(m_fr.lpstrReplaceWith);
		}
	}while(lIndex != -1);

	m_view.SetSel(cr.cpMin, cr.cpMin);
//	m_view.FindReplaceCommand(cr.cpMax);

	::SendMessage(m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING), 0, (LPARAM)&m_fr);
	PostMessage(WM_CLOSE, 0, 0);
	return 0;
}

LRESULT CFSFindReplaceDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_fr.Flags = FR_DIALOGTERM;
	::SendMessage(m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING), 0, (LPARAM)&m_fr);
	EndDialog(m_nSuccess);
	return 0;
}

LRESULT CFSFindReplaceDlg::OnChangeEditFind(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CString strFind;
	GetDlgItemText(ID_EDIT_FIND, strFind.GetBuffer(255), 255);
	strFind.ReleaseBuffer();

	BOOL bEnable = !strFind.IsEmpty();

	// Enable/disable the Find and Replace buttons.
	CButton btnFind;
	btnFind.Attach(GetDlgItem(ID_BUTTON_FIND));
	btnFind.EnableWindow(bEnable);

	if (!m_bFind)
	{
		CButton btnReplace, btnReplaceAll;
		btnReplace.Attach(GetDlgItem(ID_BUTTON_REPLACE));
		btnReplace.EnableWindow(bEnable);
		btnReplaceAll.Attach(GetDlgItem(ID_BUTTON_REPLACEALL));
		btnReplaceAll.EnableWindow(bEnable);
	}

	return 0;
}

void CFSFindReplaceDlg::FindText(LPFINDREPLACE lpfr)
{
	// Set the value for the 'Find what' edit box
	m_edtFind.SetWindowText(lpfr->lpstrFindWhat);

	// Hide/Show 'Whole word only' check box
	if (lpfr->Flags & FR_HIDEWHOLEWORD)
		m_btnMatchWhole.ShowWindow(SW_HIDE);
	else
	{
		// Disable/Enable 'Whole word only' check box
		if (lpfr->Flags & FR_NOWHOLEWORD)
			m_btnMatchWhole.EnableWindow(FALSE);
		else
			m_btnMatchWhole.ShowWindow(SW_SHOW);
	}

	// Hide/Show 'Match case' check box
	if (lpfr->Flags & FR_HIDEMATCHCASE)
		m_btnMatchCase.ShowWindow(SW_HIDE);
	else
	{
		// Disable/Enable 'Match case' check box
		if (lpfr->Flags & FR_NOMATCHCASE)
			m_btnMatchCase.EnableWindow(FALSE);
		else
			m_btnMatchCase.ShowWindow(SW_SHOW);
	}

	CButton btnDirec, btnUp, btnDown;
	btnDirec.Attach(GetDlgItem(ID_GROUP_DIRECTION));
	btnUp.Attach(GetDlgItem(ID_BUTTON_UP));
	btnDown.Attach(GetDlgItem(ID_BUTTON_DOWN));

	// Hide/Show 'Direction' radio buttons
	if (lpfr->Flags & FR_HIDEUPDOWN)
	{
		btnDirec.ShowWindow(SW_HIDE);
		btnUp.ShowWindow(SW_HIDE);
		btnDown.ShowWindow(SW_HIDE);
	}
	else
	{
		// Disable/Enable 'Direction' radio buttons
		if (lpfr->Flags & FR_NOUPDOWN)
		{
			btnDirec.EnableWindow(FALSE);
			btnUp.EnableWindow(FALSE);
			btnDown.EnableWindow(FALSE);
		}
		else
		{
			btnDirec.ShowWindow(SW_SHOW);
			btnUp.ShowWindow(SW_SHOW);
			btnDown.ShowWindow(SW_SHOW);
		}
		// Check Up/Down radio button
		if (lpfr->Flags & FR_DOWN)
		{
			btnUp.SetCheck(FALSE);
			btnDown.SetCheck(TRUE);
		}
		else
		{
			btnUp.SetCheck(TRUE);
			btnDown.SetCheck(FALSE);
		}
	}

	// Show/Hide Help button
	if (lpfr->Flags & FR_SHOWHELP)
		m_btnHelp.ShowWindow(SW_SHOW);
	else
		m_btnHelp.ShowWindow(SW_HIDE);

	UpdateWindow();
}

void CFSFindReplaceDlg::ReplaceText(LPFINDREPLACE lpfr)
{
	// Set the value for the 'Find what' edit box
	m_edtFind.SetWindowText(lpfr->lpstrFindWhat);

	// Set the value for the 'Replace with' edit box
	m_edtReplace.SetWindowText(lpfr->lpstrReplaceWith);

	// Hide/Show 'Whole word only' check box
	if ((lpfr->Flags & FR_HIDEWHOLEWORD) == FR_HIDEWHOLEWORD)
		m_btnMatchWhole.ShowWindow(SW_HIDE);
	else
	{
		// Disable/Enable 'Whole word only' check box
		if ((lpfr->Flags & FR_NOWHOLEWORD) == FR_NOWHOLEWORD)
			m_btnMatchWhole.EnableWindow(FALSE);
		else
			m_btnMatchWhole.ShowWindow(SW_SHOW);
	}

	// Hide/Show 'Match case' check box
	if ((lpfr->Flags & FR_HIDEMATCHCASE) == FR_HIDEMATCHCASE)
		m_btnMatchCase.ShowWindow(SW_HIDE);
	else
	{
		// Disable/Enable 'Match case' check box
		if ((lpfr->Flags & FR_NOMATCHCASE) == FR_NOMATCHCASE)
			m_btnMatchCase.EnableWindow(FALSE);
		else
			m_btnMatchCase.ShowWindow(SW_SHOW);
	}

	// Show/Hide Help button
	if ((lpfr->Flags & FR_SHOWHELP) == FR_SHOWHELP)
		m_btnHelp.ShowWindow(SW_SHOW);
	else
		m_btnHelp.ShowWindow(SW_HIDE);

	UpdateWindow();
}

void CFSFindReplaceDlg::GetDialogInfo()
{
	// Get 'Find what' value
	m_edtFind.GetWindowText(m_fr.lpstrFindWhat, m_fr.wFindWhatLen);

	// Get 'Replace with' value
	if (!m_bFind)
		m_edtReplace.GetWindowText(m_fr.lpstrReplaceWith, m_fr.wReplaceWithLen);

	// Get state of 'Match whole word' check box
	if(m_btnMatchWhole.GetCheck() > 0)
		m_fr.Flags |= FR_WHOLEWORD;

	// Get state of 'Match case' check box
	if(m_btnMatchCase.GetCheck() > 0)
		m_fr.Flags |= FR_MATCHCASE;

	// Get 'Direction' state
	m_fr.Flags |= FR_DOWN;
	if(m_bFind)
	{
		if(IsDlgButtonChecked(ID_BUTTON_UP))
			m_fr.Flags &= ~FR_DOWN; // "up" direction (for Find dialog only)
	}
}
