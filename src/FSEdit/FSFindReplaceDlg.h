// FSFindReplaceDlg.h: interface for the CFSFindReplaceDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(__FSFINDREPLACEDLG_H__)
#define __FSFINDREPLACEDLG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "FSEditView.h"
#include "BEdit.h"

class CFSFindReplaceDlg : public CDialogImpl<CFSFindReplaceDlg>
{
public:
	enum {IDD = IDD_FIND_DLG};

	BEGIN_MSG_MAP(CFSFindReplaceDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_BUTTON_FIND, OnFind)
		COMMAND_ID_HANDLER(ID_BUTTON_REPLACE, OnReplace)
		COMMAND_ID_HANDLER(ID_BUTTON_REPLACEALL, OnReplaceAll)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(ID_EDIT_FIND, EN_CHANGE, OnChangeEditFind)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnFind(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnReplace(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnReplaceAll(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnChangeEditFind(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);

	CFSFindReplaceDlg(CFSEditView& rview, 
		IBrailleTranslator *pbt, // pbt will be NULL if Braille translation is not required
		BOOL bFindDialogOnly, 
		LPCTSTR lpszFindWhat,	
		LPCTSTR lpszReplaceWith=NULL,
		DWORD dwFlags=FR_DOWN, 
		HWND hWndParent=NULL) 
		: m_view(rview), m_edtFind(m_hWnd,pbt != nullptr), m_edtReplace(m_hWnd,(pbt != nullptr))
	{
		m_szFindWhat[0] = _T('\0');
		m_szReplaceWith[0] = _T('\0');
		m_nSuccess = IDCANCEL;
		m_bFind = bFindDialogOnly;
		
		// Initialize the FINDREPLACE structure
		memset(&m_fr, 0, sizeof(m_fr));
		m_fr.lStructSize = sizeof(m_fr);
		m_fr.Flags = 0;
		m_fr.lpfnHook = NULL;

		int nSize = sizeof(m_szFindWhat)/sizeof(_TCHAR); 
		if(lpszFindWhat != NULL)
			_tcsncpy_s(m_szFindWhat, lpszFindWhat, nSize);
		m_fr.lpstrFindWhat = (LPTSTR)m_szFindWhat;
		m_fr.wFindWhatLen =static_cast<WORD>(nSize);

		nSize = sizeof(m_szReplaceWith)/sizeof(_TCHAR); 
		if(lpszReplaceWith != NULL)
			_tcsncpy_s(m_szReplaceWith, lpszReplaceWith, nSize);
		m_fr.lpstrReplaceWith = (LPTSTR)m_szReplaceWith;
		m_fr.wReplaceWithLen = static_cast<WORD>(nSize);

		m_fr.Flags |= dwFlags;

		if(hWndParent == NULL)
			m_fr.hwndOwner = ::GetActiveWindow();
		else
			m_fr.hwndOwner = hWndParent;
		ATLASSERT(m_fr.hwndOwner != NULL);
	};

	// Override of CDialogImpl::DoModal() so we can determine which dialog
	// to use during runtime.
	int DoModal(HWND hWndParent=::GetActiveWindow(), LPARAM dwInitParam=NULL)
	{
		ATLASSERT(m_hWnd == NULL);
		_Module.AddCreateWndData(&m_thunk.cd, (CDialogImplBaseT<CWindow>*)this);
#ifdef _DEBUG
		m_bModal = true;
#endif // _DEBUG

		int nID = 0;
		if (m_bFind)
			nID = IDD_FIND_DLG;
		else
			nID = IDD_REPLACE_DLG;

		return static_cast<int>(::DialogBoxParam(_Module.GetResourceInstance(), MAKEINTRESOURCE(nID),
			hWndParent, (DLGPROC)(CDialogImplBaseT<CWindow>*)StartDialogProc, dwInitParam));
	}

	// Note: You must allocate the object on the heap.
	// If you do not, you must override OnFinalMessage()
	virtual void OnFinalMessage(HWND /*hWnd*/) 
	{
		if (m_edtFind.m_hWnd)
			SendMessage(m_edtFind.m_hWnd, WM_DESTROY, 0, 0);
		if (m_edtReplace.m_hWnd)
			SendMessage(m_edtReplace.m_hWnd, WM_DESTROY, 0, 0);
		delete this;
	}

	void FindText(LPFINDREPLACE lpfr);
	void ReplaceText(LPFINDREPLACE lpfr);
	void GetDialogInfo();

	// Helpers for parsing information after successful return
	LPCTSTR GetFindString()		const {return (LPCTSTR)m_fr.lpstrFindWhat;}
	LPCTSTR GetReplaceString()	const {return (LPCTSTR)m_fr.lpstrReplaceWith;}
	BOOL SearchDown()			const {return m_fr.Flags & FR_DOWN ? TRUE : FALSE;}
	BOOL FindNext()				const {return m_fr.Flags & FR_FINDNEXT ? TRUE : FALSE;}
	BOOL MatchCase()			const {return m_fr.Flags & FR_MATCHCASE ? TRUE : FALSE;}
	BOOL MatchWholeWord()		const {return m_fr.Flags & FR_WHOLEWORD ? TRUE : FALSE;}
	BOOL ReplaceCurrent()		const {return m_fr. Flags & FR_REPLACE ? TRUE : FALSE;}
	BOOL ReplaceAll()			const {return m_fr.Flags & FR_REPLACEALL ? TRUE : FALSE;}
	BOOL IsTerminating()		const {return m_fr.Flags & FR_DIALOGTERM ? TRUE : FALSE ;}
	DWORD GetFlags()			const {return m_fr.Flags;}

	// Call while handling FINDMSGSTRING registered message
	// to retreive the object
	static CFSFindReplaceDlg* PASCAL GetNotifier(LPARAM lParam)
	{
		ATLASSERT(lParam != NULL);
		CFSFindReplaceDlg* pDlg = (CFSFindReplaceDlg*)(lParam - offsetof(CFSFindReplaceDlg, m_fr));
		return pDlg;
	}

	static const UINT GetFindReplaceMsg()
	{
		static const UINT nMsgFindReplace = ::RegisterWindowMessage(FINDMSGSTRING);
		return nMsgFindReplace;
	}

	static FINDTEXTEX SetFindInfo(CFSEditView& rview, LPCTSTR strFindReplace)
	{
		ATLASSERT (rview != NULL);
		
		CHARRANGE cr;
		rview.GetSel(cr);
		int nStart = (cr.cpMax > cr.cpMin) ? cr.cpMax : cr.cpMin + 1;
		FINDTEXTEX ft;
		ft.chrg.cpMin = nStart;
		ft.chrg.cpMax = -1;
		ft.lpstrText = (LPCTSTR)strFindReplace;
		return ft;
	}

	static bool SearchForText(CFSEditView& rview, FINDTEXTEX ft, DWORD dwFlags)
	{
		ATLASSERT (rview != NULL);
		if (rview == NULL) return 0;

		long lIndex = rview.FindText(dwFlags, ft);
		if(lIndex != -1)
		{
			rview.SetSel(ft.chrgText.cpMin, ft.chrgText.cpMin);
//			rview.FindReplaceCommand(ft.chrgText.cpMin);
			return true;
		}

		return false;
	}

	FINDREPLACE m_fr;

protected:
	CBEdit m_edtFind;
	CBEdit m_edtReplace;
	CButton m_btnMatchCase;
	CButton m_btnMatchWhole;
	CButton m_btnHelp;

	BOOL m_bFind;
	int m_nSuccess;
	CFSEditView& m_view;
	_TCHAR m_szFindWhat[128];
	_TCHAR m_szReplaceWith[128];

};
#endif // !defined(__FSFINDREPLACEDLG_H___)
