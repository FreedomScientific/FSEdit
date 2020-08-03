#ifndef __SPELLCHKDLG_H__1234RPM17081947
#define __SPELLCHKDLG_H__1234RPM17081947

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning (disable:4018)

#include "resource.h"
#include "FSEditView.h"
#include "splchck.h"
#include "../include/BEdit.h"
#include "atlmisc.h"
#include <vector>
#include <IBrailleTranslator.h>
const pair<DWORD, UINT> ErrArray[] = 
{
	make_pair(SPL_ERROR_CAPITALIZATION, IDS_CAPITALIZATION_ERROR),
	make_pair(SPL_INITIAL_NUMERAL, IDS_BEGIN_WITH_NUMERAL),
	make_pair(SPL_NO_MORE_SUGGESTIONS, IDS_NO_SUGGESTIONS),
	make_pair(SPL_REPEAT_WORD, IDS_REPEAT_WORD),
	make_pair(SPL_UNKNOWN_INPUT_WORD, IDS_UNKNOWN_INPUT_WORD),
	make_pair(SPL_WORD_CONSIDERED_ABBREVIATION, IDS_ABBREVIATION),
	make_pair(SPL_RETURNING_CHANGE_ONCE, IDS_CHANGED_ONCE),
	make_pair(SPL_RETURNING_CHANGE_ALWAYS, IDS_CHANGE_ALWAYS),
	make_pair(SPL_RETURNING_AUTO_REPLACE, IDS_AUTO_REPLACE),
	make_pair(SPL_UNKNOWN_ERROR, IDS_UNKNOWN_ERRROR)
};

//#define WM_SPELLCHECK	WM_USER+1056

class CSpellDlg : public CDialogImpl<CSpellDlg>
{
public:
	// Constructors
	CSpellDlg(CFSEditView& rview, IBrailleTranslator *pbt, // pbt will be NULL if Braille translation is not required
		CSpellChecker& rspl, CSplBuffer &splBuf // contains first error to take care of
		) 
		: m_edtNotInDict(m_hWnd,pbt!= nullptr), m_edtChangeTo(m_hWnd,pbt!= nullptr),
		m_lbxSuggestions(m_hWnd), m_staErrMsg(m_hWnd), m_btnIgnore(m_hWnd),
		m_btnIgnoreAll(m_hWnd),	m_btnAdd(m_hWnd), m_btnChange(m_hWnd),
		m_btnChangeAll(m_hWnd),	m_btnClose(m_hWnd),
		m_view(rview), m_spl(rspl), m_splBuf(splBuf)
	{
		m_qipBrlTrans = pbt;
		m_nErrStart=0;
		m_nErrEnd=0;
	}
	CSpellDlg(CFSEditView& rview, IBrailleTranslator *pbt, // pbt will be NULL if Braille translation is not required
		CSpellChecker& rspl, CSplBuffer &splBuf, // contains first error to take care of
		long nErrStart,long nErrEnd // character range of splBuf.pwszIn in the view
		)
		: m_edtNotInDict(m_hWnd,pbt!= nullptr), m_edtChangeTo(m_hWnd,pbt!= nullptr),
		m_lbxSuggestions(m_hWnd), m_staErrMsg(m_hWnd), m_btnIgnore(m_hWnd),
		m_btnIgnoreAll(m_hWnd),	m_btnAdd(m_hWnd), m_btnChange(m_hWnd),
		m_btnChangeAll(m_hWnd),	m_btnClose(m_hWnd),
		m_view(rview), m_spl(rspl), m_splBuf(splBuf)
	{
		m_qipBrlTrans = pbt;
		m_nErrStart=nErrStart;
		m_nErrEnd=nErrEnd;
	}

	~CSpellDlg()
	{
		// Clear the internal dictionary.
		m_spl.ClearDict(SPL_IGNORE_LEX);
	}

	enum {IDD = IDD_SPELL_CHECKER};

	BEGIN_MSG_MAP(CSpellDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_HELP, DoHelp)
		COMMAND_ID_HANDLER(IDCANCEL, OnClose)
		COMMAND_ID_HANDLER(IDBTN_CHANGE, OnChange)
		COMMAND_ID_HANDLER(IDBTN_CHANGE_ALL, OnChangeAll)
		COMMAND_ID_HANDLER(IDBTN_IGNORE, OnIgnore)
		COMMAND_ID_HANDLER(IDBTN_IGNORE_ALL, OnIgnoreAll)
		COMMAND_ID_HANDLER(IDBTN_ADD, OnAdd)
		COMMAND_HANDLER(IDCLBX_SUGGESTIONS, LBN_SELCHANGE, OnSuggestLBSelChange)
		COMMAND_HANDLER(IDEDT_NOT_IN_DICTIONARY, EN_SETFOCUS, OnDictSetFocus)
		COMMAND_HANDLER(IDEDT_NOT_IN_DICTIONARY, EN_KILLFOCUS, OnDictKillFocus)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT DoHelp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnChangeAll(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnIgnore(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnIgnoreAll(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnAdd(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);

	LRESULT OnSuggestLBSelChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnDictSetFocus(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnDictKillFocus(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);

private:
	// Fills in the various dialog controls with the information relevant to the 
	// error in psplBuf.
	BOOL SpellInit(CSplBuffer *psplBuf);
	// Sets the error name in the static control, prompt for the edit box
	void SetErrorName(DWORD dwErr);
	// Selects the current error in the view.
	BOOL SelectErrorInView();
	// Selects the current error in the 'Not in Dictionaries' edit field.
	BOOL SelectErrorInEdit();
	// Continues checking at the view offset specified by m_nErrStart.
	void ContinueChecking();
	/* Fills in strReplace with a string consisting of three segments:
		1. The initial part of the input buffer that is correct.
		2. The content of the Change To field.
		3. The remaining part of the input buffer after the error.
		It performs Braille foward translation of the replacement string if 
		required. Also returns the beginning and end offsets of the Change To 
		string in the complete replacement string. */
	int BuildReplacementFromChangeTo(CString &strReplace,
		long &nChangeToStart,long &nChangeToEnd);
	int BuildReplacement(const CString &strChangeTo,CString &strReplace,
		long &nChangeToStart,long &nChangeToEnd);
	void UpdateView(const CString &strReplace,long nIncrementStartOffset);
	void RememberErrorAndCorrection();
	void CorrectError(const CString &strCorrection=_T(""));
	CString GetChangeToText();
	void IncrementStartOffset(long nIncrement);
	void ShowErrorInContext();
	void SetChangeToWithSuggestion();
	void BrlTranslate(CString &strText,BOOL bAtBeginningOfWord);

	// member variables
	CFSEditView& m_view;
	CComQIPtr<IBrailleTranslator> m_qipBrlTrans;
	CSpellChecker& m_spl;
	CSplBuffer m_splBuf; // Current error
	long m_nErrStart, m_nErrEnd; // character range in view of m_splBuf.pwszIn
	vector<pair<CString, CString> > vecChangeAll;
//	CRITICAL_SECTION cs;
	// Child controls
	CBEdit m_edtNotInDict, m_edtChangeTo;
	CStatic m_staErrMsg;
	CListBox m_lbxSuggestions;
	CButton m_btnIgnore, m_btnIgnoreAll, m_btnAdd, m_btnChange, m_btnChangeAll,
		m_btnClose;
};
#endif
