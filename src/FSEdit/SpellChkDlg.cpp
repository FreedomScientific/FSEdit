#include "stdafx.h"
#include "SpellChkDlg.h"
#include "mainfrm.h"

// Fills in the various dialog controls with the information relevant to the 
// error in psplBuf.
BOOL CSpellDlg::SpellInit(CSplBuffer *psplBuf)
{
	if (!psplBuf->HasValidError())
		return FALSE;

	// Select the misspelled word in the view
	SelectErrorInView();

	// Initialize Not in Dictionary edit box with the line containing the error.
	ShowErrorInContext();

	// Set the prompt for the Not in Dictionary edit box
	SetErrorName(psplBuf->dwErr);

	// Get the suggestions and put them in the list box.
	vector<CString> vecSuggestions;
	m_spl.GetSuggestions(psplBuf, vecSuggestions);
	m_lbxSuggestions.ResetContent();
	for(vector<CString>::iterator it = vecSuggestions.begin();
		it != vecSuggestions.end();
		it++)
		m_lbxSuggestions.AddString(*it);
	m_lbxSuggestions.SetCurSel(0);
	
	// Get the selected suggestion and put it in the Change To edit box
	SetChangeToWithSuggestion();
	
	m_edtChangeTo.SetFocus();
	m_edtChangeTo.SetSel(0, -1);
	return TRUE;
}

LRESULT CSpellDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();

	m_edtNotInDict.SubclassWindow(GetDlgItem(IDEDT_NOT_IN_DICTIONARY));
//	m_edtNotInDict.Attach(GetDlgItem(IDEDT_NOT_IN_DICTIONARY));

	m_staErrMsg.Attach(GetDlgItem(IDSTA_ERROR_MSG));

	m_edtChangeTo.SubclassWindow(GetDlgItem(IDEDT_CHANGE_TO));
//	m_edtChangeTo.Attach(GetDlgItem(IDEDT_CHANGE_TO));

	m_lbxSuggestions.Attach(GetDlgItem(IDCLBX_SUGGESTIONS));
	/* The default behavior of CListBox is to sort the elements alphabetically. 
		We don't want to sort the suggestions alphabetically because they should 
		be sorted by the spell chaecker scores: most likely replacements first.
		That's why we remove the style LBS_SORT. */
	m_lbxSuggestions.ModifyStyle(/*DWORD dwRemove*/ LBS_SORT, /*DWORD dwAdd*/0); 
	
	m_btnIgnore.Attach(GetDlgItem(IDBTN_IGNORE));
	m_btnIgnoreAll.Attach(GetDlgItem(IDBTN_IGNORE_ALL));
	m_btnAdd.Attach(GetDlgItem(IDBTN_ADD));
	m_btnChange.Attach(GetDlgItem(IDBTN_CHANGE));
	m_btnChangeAll.Attach(GetDlgItem(IDBTN_CHANGE_ALL));
	
//	InitializeCriticalSection(&cs);
	SpellInit(&m_splBuf);
	
	return 0;
}

// Sets the Error name in the static control
void CSpellDlg::SetErrorName(DWORD dwErr)
{
	const pair<DWORD, UINT>* pr =
		find_if(ErrArray, ErrArray+10, FindPair_first<DWORD, UINT>(dwErr));
	if(pr != (ErrArray+10))
	{
		CString str;
		str.LoadString((*pr).second);
		m_staErrMsg.SetWindowText(str);
	}
}

// Selects the current error in the view.
BOOL CSpellDlg::SelectErrorInView()
{
#ifdef DEBUG
//	ShowWindow(SW_HIDE);
#endif
	return m_view.SetSel(m_nErrStart, m_nErrEnd);
#ifdef DEBUG
//	ShowWindow(SW_SHOWNORMAL);
#endif
}

// Selects the current error in the 'Not in Dictionaries' edit field.
BOOL CSpellDlg::SelectErrorInEdit()
{
	int nLine = m_view.LineFromChar(m_nErrStart);
	int nLineStart = m_view.LineIndex(nLine);
	int nLineLen = m_view.LineLength(nLineStart);
	int nStart, nEnd; // Offsets of the error relative to the edit field
	nStart = m_nErrStart - nLineStart;
	nEnd = m_nErrEnd - nLineStart;
	if (nStart < 0)
		nStart = 0;
	if (nEnd > nLineLen)
		nEnd = nLineLen;

	m_edtNotInDict.SetSel(nStart,nEnd);
	return TRUE;
}

LRESULT CSpellDlg::OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CString strAdd(m_splBuf.GetError(),m_splBuf.GetErrorLength());
	m_spl.AddDict(USER_DICT1_ADD, strAdd, NULL);

	IncrementStartOffset(m_splBuf.GetErrorEnd());
	ContinueChecking();
	
	bHandled = TRUE;
	return 0;
}

LRESULT CSpellDlg::OnChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	// Replace the text in the view
	CorrectError();
	
	ContinueChecking();
	
	bHandled = TRUE;
	return 0;
}

LRESULT CSpellDlg::OnChangeAll(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	// Replace the text in the view and remember the error and the replacement 
	// in case it's found again.
	CorrectError();
	RememberErrorAndCorrection();
	
	ContinueChecking();

	bHandled = TRUE;
	return 0;
}

LRESULT CSpellDlg::OnIgnore(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	IncrementStartOffset(m_splBuf.GetErrorEnd());
	ContinueChecking();
	
	bHandled = TRUE;
	return 0;
}

LRESULT CSpellDlg::OnIgnoreAll(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CString strIgnore(m_splBuf.GetError(),m_splBuf.GetErrorLength());
	m_spl.AddDict(SPL_IGNORE_LEX, strIgnore, NULL);
	
	IncrementStartOffset(m_splBuf.GetErrorEnd());
	ContinueChecking();
	bHandled = TRUE;
	return 0;
}

LRESULT CSpellDlg::OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
//	DeleteCriticalSection(&cs);
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CSpellDlg::OnSuggestLBSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	SetChangeToWithSuggestion();
	bHandled = TRUE;
	return 0;
}

LRESULT CSpellDlg::OnDictSetFocus(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	SelectErrorInEdit();
	return 0;
}

LRESULT CSpellDlg::OnDictKillFocus(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	SelectErrorInEdit();
	return 0;
}

void CSpellDlg::ContinueChecking()
{
//	EnterCriticalSection(&cs);

	// If a stopping index point was given and we are about to check 
	// past that point, ask user if they want to continue.
	//NKDbgPrintfW( _T("CSpellDlg::ContinueChecking from %d to %d, m_nStop %d\r\n"), m_nErrStart, m_nErrEnd, m_spl.m_nStop);
	if (m_spl.m_nStop > 0)
	{
		if (m_nErrEnd > m_spl.m_nStop+1)
		{
			CString strTitle, strMsg;
			strTitle.LoadString(IDS_SPELLCHECKER);
			strMsg.LoadString(IDS_CONTINUE_SPELL_CHECK);
			if (::MessageBox(m_hWnd, strMsg, strTitle, MB_YESNO|MB_ICONQUESTION) == IDYES)
			{
				m_spl.m_nStop = 0;
			}
			else
			{
				EndDialog(SPL_UNKNOWN_ERROR);
				return;
			}
		}
	}

	if (m_nErrStart>m_nErrEnd) // replacement text was longer than original
		m_nErrEnd = m_nErrStart;
	if (m_spl.FindError(m_view,m_qipBrlTrans,m_splBuf,m_nErrStart,m_nErrEnd))
	{
		SelectErrorInView();

		CString strErr(m_splBuf.GetError(),m_splBuf.GetErrorLength());
		vector<pair<CString, CString> >::iterator it;
		if((it = find_if(vecChangeAll.begin(), vecChangeAll.end(), 
			FindPair_first<CString>(strErr))) != vecChangeAll.end())
		{
			CString strChangeTo;
			strChangeTo = (*it).second;
			
			CorrectError(strChangeTo);

			ContinueChecking();
			return;
		}
		SpellInit(&m_splBuf);
	}
	else
	{
		EndDialog(SPL_UNKNOWN_ERROR);
//		PostMessage(IDCANCEL);
	}
//	LeaveCriticalSection(&cs);
}
int CSpellDlg::BuildReplacementFromChangeTo(CString &strReplace,
											long &nChangeToStart,long &nChangeToEnd)
{
	CString strChangeTo = GetChangeToText();

	return BuildReplacement(strChangeTo,strReplace,nChangeToStart,nChangeToEnd);
}
int CSpellDlg::BuildReplacement(const CString &strChangeTo,CString &strReplace,
								long &nChangeToStart,long &nChangeToEnd)
{
	if(m_splBuf.iwcErr > 0)
	{
		// The first part of m_splBuf.pwszIn is not part of the error, 
		// so, we need to concatenate it with the Change To text.
		strReplace = CString(m_splBuf.pwszIn,m_splBuf.iwcErr);
		BrlTranslate(strReplace,TRUE);
	}

	nChangeToStart = strReplace.GetLength();
	strReplace += strChangeTo; // no need to BrlTranslate, since strChangeTo is 
								// always in the appropriate grade.
	nChangeToEnd = nChangeToStart + strChangeTo.GetLength();
	
	if(m_splBuf.GetErrorEnd() < m_splBuf.cwcIn)
	{
		// The error does not include the last part of m_splBuf.pwszIn,
		// so, we need to append it to the replacement string.
		CString strRemain = m_splBuf.pwszIn + m_splBuf.GetErrorEnd();
		BrlTranslate(strRemain,FALSE);
		strReplace += strRemain;
	}
	return strReplace.GetLength();
}
void CSpellDlg::UpdateView(const CString &strReplace,long nChangeToEnd)
{
//#ifdef DEBUG
	long nStart, nEnd;
	m_view.GetSel(nStart,nEnd);
	if (m_nErrStart != nStart || m_nErrEnd != nEnd)
	{
		ATLASSERT(FALSE);
		SelectErrorInView();
	}
//#endif
	m_view.ReplaceSel(strReplace);
		m_view.GetSel(nStart,nEnd);
		m_nErrStart = nEnd;
}
void CSpellDlg::RememberErrorAndCorrection()
{
	CString strErr(m_splBuf.GetError(),m_splBuf.GetErrorLength());
	CString strChangeTo = GetChangeToText();
	vecChangeAll.push_back(make_pair(strErr,strChangeTo));
}
void CSpellDlg::CorrectError(const CString &strCorrection)
{
	CString strReplace;
	long nChangeToStart=0, nChangeToEnd=0;

	if (strCorrection.IsEmpty())
		BuildReplacementFromChangeTo(strReplace,nChangeToStart,nChangeToEnd);
	else
		BuildReplacement(strCorrection,strReplace,nChangeToStart,nChangeToEnd);

	UpdateView(strReplace,nChangeToEnd);
}
CString CSpellDlg::GetChangeToText()
{
	int nChangeToLen = m_edtChangeTo.GetWindowTextLength();
	LPTSTR lpszChangeTo = (LPTSTR) _alloca((nChangeToLen + 1)*sizeof(TCHAR));
	nChangeToLen = m_edtChangeTo.GetWindowText(lpszChangeTo,nChangeToLen+1);
	CString strChangeTo;	
	if (nChangeToLen > 0)
		strChangeTo = lpszChangeTo;
	return strChangeTo;
}
void CSpellDlg::IncrementStartOffset(long nIncrement)
{
	if (nIncrement > 0)
		m_nErrStart += nIncrement;
	else
		m_nErrStart = m_nErrEnd;
}
void CSpellDlg::ShowErrorInContext()
{
	// Shows the current error in context
	int nLine = m_view.LineFromChar(m_nErrStart);
	int nLineStart = m_view.LineIndex(nLine);
	int nLineLen = m_view.LineLength(nLineStart);
	LPTSTR lpszLine = (LPTSTR) _alloca((nLineLen + 1)*sizeof(_TCHAR));
	if (!lpszLine)
		return;
	nLineLen = m_view.GetLine(nLine,lpszLine,nLineLen);
	lpszLine[nLineLen] = _T('\0'); // GetLine does not null-terminate the string.
	m_edtNotInDict.SetWindowText(lpszLine);

	SelectErrorInEdit();
}
void CSpellDlg::SetChangeToWithSuggestion()
{
	// Get the current suggestion in the list box.
	int nIndex = m_lbxSuggestions.GetCurSel();
	//NKDbgPrintfW( _T("CSpellDlg::SetChangeToWithSuggestion nIndex: %d\r\n"), nIndex);
	if(nIndex == LB_ERR)
	{
		m_edtChangeTo.SetWindowText(_T("")); // string is empty
		return;
	} // end if no suggestions

	int cchLen = m_lbxSuggestions.GetTextLen(nIndex);
	//NKDbgPrintfW( _T("CSpellDlg::SetChangeToWithSuggestion cchLen: %d\r\n"), cchLen);
	if (cchLen != LB_ERR)
	{
		TCHAR wszItemText[256]; // contains the replacement text

		m_lbxSuggestions.GetText(nIndex, wszItemText);
		//NKDbgPrintfW( _T("CSpellDlg::SetChangeToWithSuggestion wszItemText: '%s'\r\n"), wszItemText);
		CString strItemText( wszItemText);
		BrlTranslate(strItemText,TRUE);
		//NKDbgPrintfW( _T("CSpellDlg::SetChangeToWithSuggestion strItemText: '%s'\r\n"), strItemText);
		m_edtChangeTo.SetWindowText(strItemText);
	}
}

void CSpellDlg::BrlTranslate(CString &strText,BOOL bAtBeginningOfWord)
{
	if (!m_qipBrlTrans)
		return;
	// Special handling of quote marks. If we're at the beginning of a
	// word the quote must become a 8, otherwise it must become a 0.
	// The translator can't make this distinction because if just
	// translating a single character there is no context.
	if (strText.GetLength() == 1 && strText[0] == _T('\"'))
	{
		strText = bAtBeginningOfWord?_T('8'):_T('0');
		return;
	}
	CComBSTR bstrTranslated;
	m_qipBrlTrans->Translate(CComBSTR(strText),&bstrTranslated,0,0,-1,-1);
	if (bstrTranslated)
	strText = bstrTranslated;
}

LRESULT CSpellDlg::DoHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// needs work
	return 0;
}
