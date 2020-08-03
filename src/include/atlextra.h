////////////////////////////////////////////////////////////////////
// AtlExtra.h
// Property of Freedom Scientific, Inc.
// Written by Richard Martone
////////////////////////////////////////////////////////////////////
#pragma once
//#include "resource.h"
#include <atlmisc.h>
#include "AtlResExtra.h"
#include <tchar.h>
#include <vector>

using namespace std;


#define POINT_SIZE	20
#define BOOKMARK(a, b, c) ((LONG)MAKEWORD(a, b) | ((DWORD)((WORD)(c))) << 16)

template<int n, int d> struct ArraySize{enum{Val = n/d};};

template<class T, class S = T>
class FindPair_first
{
	T Value;
public:
	FindPair_first(T v) : Value(v) {}
	bool operator()(const pair<T, S>& e){return (e.first == Value);}
};

template<class T, class S = T>
class FindPair_second
{
	S Value;
public:
	FindPair_second(S v) : Value(v) {}
	bool operator()(const pair<T, S>& e){return (e.second == Value);}
};

class CTheWaitCursor
{
	HCURSOR hPrev;
public:
	CTheWaitCursor(){hPrev = ::SetCursor(::LoadCursor(_Module.GetModuleInstance(), IDC_WAIT));}
	~CTheWaitCursor(){::SetCursor(hPrev);}
	void Restore(){::SetCursor(hPrev);}
};

class CFileDlg : public CFileDialogImpl<CFileDialog>
{
public:
	CFileDlg(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		HWND hWndParent = NULL)
		: CFileDialogImpl<CFileDialog>(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent)
	{ }

	CString GetPathName() const
	{
		if ((m_ofn.Flags & OFN_EXPLORER) && m_hWnd != NULL)
		{
			ATLASSERT(::IsWindow(m_hWnd));
			CString strResult;
			if(::SendMessage(GetParent(), CDM_GETSPEC, (WPARAM)MAX_PATH,
				(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
			{
				strResult.Empty();
			}
			else
			{
				strResult.ReleaseBuffer();
			}

			if (!strResult.IsEmpty())
			{
				if(::SendMessage(GetParent(), CDM_GETFILEPATH, (WPARAM)MAX_PATH,
					(LPARAM)strResult.GetBuffer(MAX_PATH)) < 0)
					strResult.Empty();
				else
				{
					strResult.ReleaseBuffer();
					return strResult;
				}
			}
		}
		return m_ofn.lpstrFile;
	}

	// override base class map and references to handlers
	DECLARE_EMPTY_MSG_MAP()
};


namespace ATLEXT
{
	inline
	HINSTANCE FindResourceHandle(LPCTSTR lpszName, LPCTSTR lpszType)
	{
		ATLASSERT(lpszName != NULL);
		ATLASSERT(lpszType != NULL);

		HINSTANCE hInst;

		hInst = _Module.GetResourceInstance(); //AfxGetResourceHandle();
		if (::FindResource(hInst, lpszName, lpszType) == NULL)
			return NULL;

		return hInst;
	}

	inline
	HWND FindText(LPFINDREPLACE lpfr)
	{
		// Create a modeless dialog.
		HINSTANCE hInst = FindResourceHandle( _T("WCE_FINDDLG"), RT_DIALOG );
		if(!hInst)
			return (NULL);
		HWND hwnd = ::CreateDialog(hInst, _T("WCE_FINDDLG"), lpfr->hwndOwner,
						(DLGPROC)lpfr->lpfnHook );

		// Check it has been created successfully
		ATLASSERT(hwnd != NULL);

		// Set the value for the 'Find what' edit box
		if (SetWindowText(
			GetDlgItem(hwnd, ID_EDIT1),	// Handle of ID_EDIT1
			lpfr->lpstrFindWhat))
		{
		//	TRACE0("Setting ID_EDIT1 successful\n");
		}
		// Hide/Show 'Whole word only' check box
		if (lpfr->Flags & FR_HIDEWHOLEWORD)
			ShowWindow(GetDlgItem(hwnd, ID_CHECK1), SW_HIDE);
		else
		{
			// Disable/Enable 'Whole word only' check box
			if (lpfr->Flags & FR_NOWHOLEWORD)
				EnableWindow(GetDlgItem(hwnd, ID_CHECK1), FALSE);
			else
				ShowWindow(GetDlgItem(hwnd, ID_CHECK1), SW_SHOW);
		}

		// Hide/Show 'Match case' check box
		if (lpfr->Flags & FR_HIDEMATCHCASE)
			ShowWindow(GetDlgItem(hwnd, ID_CHECK2), SW_HIDE);
		else
		{
			// Disable/Enable 'Match case' check box
			if (lpfr->Flags & FR_NOMATCHCASE)
				EnableWindow(GetDlgItem(hwnd, ID_CHECK2), FALSE);
			else
				ShowWindow(GetDlgItem(hwnd, ID_CHECK2), SW_SHOW);
		}

		// Check Up/Down radio button
		if (lpfr->Flags & FR_DOWN)
		{
			SendMessage(GetDlgItem(hwnd, ID_RADIO1), BM_SETCHECK, 0, 0);
			SendMessage(GetDlgItem(hwnd, ID_RADIO2), BM_SETCHECK, 1, 0);
		}
		else
		{
			SendMessage(GetDlgItem(hwnd, ID_RADIO1), BM_SETCHECK, 1, 0);
			SendMessage(GetDlgItem(hwnd, ID_RADIO2), BM_SETCHECK, 0, 0);
		}

		// Hide/Show 'Direction' radio buttons
		if (lpfr->Flags & FR_HIDEUPDOWN)
		{
			ShowWindow(GetDlgItem(hwnd, ID_GROUP1), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, ID_RADIO1), SW_HIDE);
			ShowWindow(GetDlgItem(hwnd, ID_RADIO2), SW_HIDE);
		}
		else
		{
			// Disable/Enable 'Direction radio buttons'
			if (lpfr->Flags & FR_NOUPDOWN)
			{
				EnableWindow(GetDlgItem(hwnd, ID_GROUP1), FALSE);
				EnableWindow(GetDlgItem(hwnd, ID_RADIO1), FALSE);
				EnableWindow(GetDlgItem(hwnd, ID_RADIO2), FALSE);
			}
			else
			{
				ShowWindow(GetDlgItem(hwnd, ID_GROUP1), SW_SHOW);
				ShowWindow(GetDlgItem(hwnd, ID_RADIO1), SW_SHOW);
				ShowWindow(GetDlgItem(hwnd, ID_RADIO2), SW_SHOW);
			}
		}

		// Show/Hide Help button
		if (lpfr->Flags & FR_SHOWHELP)
			ShowWindow(GetDlgItem(hwnd, ID_BUTTON_HELP), SW_SHOW);
		else
			ShowWindow(GetDlgItem(hwnd, ID_BUTTON_HELP), SW_HIDE);

		SetFocus(GetDlgItem(hwnd, ID_EDIT1));

		// Show find dialog
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		return hwnd;
	}

	inline
	HWND ReplaceText(LPFINDREPLACE lpfr)
	{
		// Create a modeless dialog.
		HINSTANCE hInst = FindResourceHandle( _T("WCE_REPLACEDLG"), RT_DIALOG );
		if(!hInst)
			return (NULL);
		HWND hWnd = ::CreateDialog(hInst, _T("WCE_REPLACEDLG"), lpfr->hwndOwner,
						(DLGPROC)lpfr->lpfnHook );

		// Check it has been created successfully
		ATLASSERT(hWnd != NULL);

		// Set the value for the 'Find what' edit box
		if (SetWindowText(
			GetDlgItem(hWnd, ID_EDIT1),	// Handle of ID_EDIT1
			lpfr->lpstrFindWhat))
		{
				//TRACE0("Setting ID_EDIT1 successful\n");
		}

		// Set the value for the 'Replace with' edit box
		if (SetWindowText(
			GetDlgItem(hWnd, ID_EDIT2),	// Handle of ID_EDIT2
			lpfr->lpstrReplaceWith))
		{
		//		TRACE0("Setting ID_EDIT2 successful\n");
		}

		// Hide/Show 'Whole word only' check box
		if ((lpfr->Flags & FR_HIDEWHOLEWORD) == FR_HIDEWHOLEWORD)
			ShowWindow(GetDlgItem(hWnd, ID_CHECK1), SW_HIDE);
		else
		{
			// Disable/Enable 'Whole word only' check box
			if ((lpfr->Flags & FR_NOWHOLEWORD) == FR_NOWHOLEWORD)
				EnableWindow(GetDlgItem(hWnd, ID_CHECK1), FALSE);
			else
				ShowWindow(GetDlgItem(hWnd, ID_CHECK1), SW_SHOW);
		}

		// Hide/Show 'Match case' check box
		if ((lpfr->Flags & FR_HIDEMATCHCASE) == FR_HIDEMATCHCASE)
			ShowWindow(GetDlgItem(hWnd, ID_CHECK2), SW_HIDE);
		else
		{
			// Disable/Enable 'Match case' check box
			if ((lpfr->Flags & FR_NOMATCHCASE) == FR_NOMATCHCASE)
				EnableWindow(GetDlgItem(hWnd, ID_CHECK2), FALSE);
			else
				ShowWindow(GetDlgItem(hWnd, ID_CHECK2), SW_SHOW);
		}

		// Show/Hide Help button
		if ((lpfr->Flags & FR_SHOWHELP) == FR_SHOWHELP)
			ShowWindow(GetDlgItem(hWnd, ID_BUTTON_HELP), SW_SHOW);
		else
			ShowWindow(GetDlgItem(hWnd, ID_BUTTON_HELP), SW_HIDE);

		// Show find/replace dialog
		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);

		return hWnd;
	}

	inline UINT ChooseFont(LPCHOOSEFONT lpCFont)
	{
		HINSTANCE hInst = FindResourceHandle( _T("IDD_FONT_DLG"), RT_DIALOG );
		if(!hInst)
			return IDCANCEL;

		return static_cast<UINT>(::DialogBox(hInst, _T("IDD_FONT_DLG"), lpCFont->hwndOwner,
						(DLGPROC)lpCFont->lpfnHook ));
	}
};

const ULONG uCtrl [] =
{
	ID_EDIT1,
	ID_EDIT2,
	ID_CHECK1,
	ID_CHECK2,
	ID_RADIO1,
	ID_RADIO2,
	ID_BUTTON_FIND,
	ID_BUTTON1,
	ID_BUTTON2,
	IDCANCEL,
	0L
};

typedef LRESULT (CALLBACK* DLGPROCEX)(HWND, UINT, WPARAM, LPARAM);

template <class T>
class ATL_NO_VTABLE CFindReplaceDlgImpl : public CCommonDialogImplBase
{
public:
	FINDREPLACE m_fr;
	TCHAR m_szFindWhat[128];
	TCHAR m_szReplaceWith[128];
	bool m_bFind;

	// Constructors
	CFindReplaceDlgImpl();

	// Note: You must allocate the object on the heap.
	//       If you do not, you must override OnFinalMessage()
	virtual void OnFinalMessage(HWND /*hWnd*/) {delete this;}

	// TRUE for Find, FALSE for FindReplace
	HWND Create(BOOL bFindDialogOnly, LPCTSTR lpszFindWhat,	LPCTSTR lpszReplaceWith = NULL,
			DWORD dwFlags = FR_DOWN, HWND hWndParent = NULL);

	static const UINT GetFindReplaceMsg()
	{
		static const UINT nMsgFindReplace = ::RegisterWindowMessage(FINDMSGSTRING);
		return nMsgFindReplace;
	}

	// call while handling FINDMSGSTRING registered message
	// to retreive the object
	static T* PASCAL GetNotifier(LPARAM lParam)
	{
		ATLASSERT(lParam != NULL);
		T* pDlg = (T*)(lParam - offsetof(T, m_fr));
		return pDlg;
	}

// Operations
	// Helpers for parsing information after successful return
	LPCTSTR GetFindString()	   const {return (LPCTSTR)m_fr.lpstrFindWhat;}
	LPCTSTR GetReplaceString() const {return (LPCTSTR)m_fr.lpstrReplaceWith;}
	BOOL SearchDown()		   const {return m_fr.Flags & FR_DOWN ? TRUE : FALSE;}
	BOOL FindNext()			   const {return m_fr.Flags & FR_FINDNEXT ? TRUE : FALSE;}
	BOOL MatchCase()		   const {return m_fr.Flags & FR_MATCHCASE ? TRUE : FALSE;}
	BOOL MatchWholeWord()	   const {return m_fr.Flags & FR_WHOLEWORD ? TRUE : FALSE;}
	BOOL ReplaceCurrent()	   const {return m_fr. Flags & FR_REPLACE ? TRUE : FALSE;}
	BOOL ReplaceAll()		   const {return m_fr.Flags & FR_REPLACEALL ? TRUE : FALSE;}
	BOOL IsTerminating()	   const {return m_fr.Flags & FR_DIALOGTERM ? TRUE : FALSE ;}


	static UINT_PTR APIENTRY FRHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static CCommonDialogImplBase* pT = NULL;
		if(uMsg == WM_INITDIALOG)
		{
			if(pT == NULL)
			{
				pT = (CCommonDialogImplBase*)_Module.ExtractCreateWndData();
				ATLASSERT(pT != NULL);
				ATLASSERT(pT->m_hWnd == NULL);
				ATLASSERT(::IsWindow(hWnd));
			}
			
			// subclass dialog's window
			pT->SubclassWindow(hWnd);
		
			// Set 'Find what' into Focus 
			WTL::CEdit edtFindWhat(pT->GetDlgItem(ID_EDIT1));
			edtFindWhat.SetFocus();	
			
			// check message map for WM_INITDIALOG handler
			LRESULT lRes;
			if(pT->ProcessWindowMessage(pT->m_hWnd, uMsg, wParam, lParam, lRes, 0) == FALSE)
				return 0;
			
			return lRes;
		}
		else
		{
			CFindReplaceDlgImpl<T>* p = static_cast<CFindReplaceDlgImpl<T>*>(pT);
			switch(uMsg)
			{
			case WM_COMMAND:
				if(LOWORD(wParam) == ID_BUTTON1 && HIWORD(wParam) == BN_CLICKED)
				{
					p->m_fr.Flags = FR_ENABLEHOOK | FR_REPLACE | FR_DOWN;

					// Get 'Find what' value
					WTL::CEdit edtFind(p->GetDlgItem(ID_EDIT1));
					edtFind.GetWindowText(p->m_fr.lpstrFindWhat, p->m_fr.wFindWhatLen);

					//replace dialog, get 'Replace with' value
					WTL::CEdit edtReplace(p->GetDlgItem(ID_EDIT2));
						edtReplace.GetWindowText(p->m_fr.lpstrReplaceWith, p->m_fr.wReplaceWithLen);

					// Get state of 'Match whole word' check box
					WTL::CButton btnWholeWord(p->GetDlgItem(ID_CHECK1));
					if(btnWholeWord.GetCheck() > 0)
						p->m_fr.Flags |= FR_WHOLEWORD;

					// Get state of 'Match case' check box
					WTL::CButton btnMatchCase(p->GetDlgItem(ID_CHECK2));
					if(btnMatchCase.GetCheck() > 0)
						p->m_fr.Flags |= FR_MATCHCASE;

					// Notify parent window of user input to find replace dialog
					::SendMessage(p->m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING),
						0, (LPARAM)&p->m_fr);
				}
				else if(LOWORD(wParam) == ID_BUTTON2 && HIWORD(wParam) == BN_CLICKED)
				{
					// Prepare m_fr for parent notification
					p->m_fr.Flags = FR_ENABLEHOOK | FR_REPLACEALL | FR_DOWN;

					// Get 'Find what' value
					WTL::CEdit edtFind(p->GetDlgItem(ID_EDIT1));
					edtFind.GetWindowText(p->m_fr.lpstrFindWhat, p->m_fr.wFindWhatLen);

					//replace dialog, get 'Replace with' value
					WTL::CEdit edtReplace(p->GetDlgItem(ID_EDIT2));
					edtReplace.GetWindowText(p->m_fr.lpstrReplaceWith, p->m_fr.wReplaceWithLen);

					// Get state of 'Match whole word' check box
					WTL::CButton btnWholeWord(p->GetDlgItem(ID_CHECK1));
					if(btnWholeWord.GetCheck() > 0)
						p->m_fr.Flags |= FR_WHOLEWORD;

					// Get state of 'Match case' check box
					WTL::CButton btnMatchCase(p->GetDlgItem(ID_CHECK2));
					if(btnMatchCase.GetCheck() > 0)
						p->m_fr.Flags |= FR_MATCHCASE;

					// Notify parent window of user input to find replace dialog
					::SendMessage(p->m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING),
						0, (LPARAM)&p->m_fr);
				}
				else if(LOWORD(wParam) == ID_BUTTON_FIND && HIWORD(wParam) == BN_CLICKED)
				{
					// Prepare m_fr for parent notification
					p->m_fr.Flags = FR_ENABLEHOOK | FR_FINDNEXT;

					// Get 'Find what' value
					WTL::CEdit edtFind(p->GetDlgItem(ID_EDIT1));
					edtFind.GetWindowText(p->m_fr.lpstrFindWhat, p->m_fr.wFindWhatLen);

					// If find/replace dialog, get 'Replace with' value
					if(!p->m_bFind)
					{
						WTL::CEdit edtReplace(p->GetDlgItem(ID_EDIT2));
						edtReplace.GetWindowText(p->m_fr.lpstrReplaceWith, p->m_fr.wReplaceWithLen);
					}

					// Get state of 'Match whole word' check box
					WTL::CButton btnWholeWord(p->GetDlgItem(ID_CHECK1));
					if(btnWholeWord.GetCheck() > 0)
						p->m_fr.Flags |= FR_WHOLEWORD;

					// Get state of 'Match case' check box
					WTL::CButton btnMatchCase(p->GetDlgItem(ID_CHECK2));
					if(btnMatchCase.GetCheck() > 0)
						p->m_fr.Flags |= FR_MATCHCASE;

					// Get 'Direction' state
					p->m_fr.Flags |= FR_DOWN;
					if(p->m_bFind)
					{
						if(p->IsDlgButtonChecked(ID_RADIO1))
							p->m_fr.Flags &= ~FR_DOWN; // "up" direction (for Find dialog only)
					}

					// Notify parent window of user input to find replace dialog
					::SendMessage(p->m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING),
						0, (LPARAM)&p->m_fr);
				}
				else if(LOWORD(wParam) == ID_EDIT1 && HIWORD(wParam) == EN_CHANGE)
				{
					CString strFind;

					p->GetDlgItemText(ID_EDIT1, strFind.GetBuffer(255), 255);
					strFind.ReleaseBuffer();

					BOOL bEnable = !strFind.IsEmpty();

					CButton btnFind(p->GetDlgItem(ID_BUTTON_FIND));
					btnFind.EnableWindow(bEnable);

					if (!p->m_bFind)
					{
						CButton btnOne(p->GetDlgItem(ID_BUTTON1));
						btnOne.EnableWindow(bEnable);
						CButton btnTwo(p->GetDlgItem(ID_BUTTON2));
						btnTwo.EnableWindow(bEnable);
					}
				}
				else if(LOWORD(wParam) == IDCANCEL)
					::PostMessage(pT->m_hWnd, WM_CLOSE, 0, 0);
				break;
			case WM_CLOSE:
				p->m_fr.Flags = FR_DIALOGTERM;
				::SendMessage(p->m_fr.hwndOwner, RegisterWindowMessage(FINDMSGSTRING),
						0, (LPARAM)&p->m_fr);

				pT->DestroyWindow();
				pT = NULL;
				break;
			}
		}

		return 0;
	}
};

template<class T>
CFindReplaceDlgImpl<T>::CFindReplaceDlgImpl()
{
	memset(&m_fr, 0, sizeof(m_fr));
	m_szFindWhat[0] = '\0';
	m_szReplaceWith[0] = '\0';

	m_fr.lStructSize = sizeof(m_fr);
	m_fr.Flags = FR_ENABLEHOOK;
	m_fr.lpfnHook = (LPFRHOOKPROC)T::FRHookProc;
	m_fr.lpstrFindWhat = (LPTSTR)m_szFindWhat;
}

template<class T>
HWND CFindReplaceDlgImpl<T>::Create(BOOL bFindDialogOnly, LPCTSTR lpszFindWhat,	LPCTSTR lpszReplaceWith /* = NULL */,
			DWORD dwFlags /* = FR_DOWN */, HWND hWndParent /* = NULL */)
{
	ATLASSERT(m_fr.Flags & FR_ENABLEHOOK);
	ATLASSERT(m_fr.lpfnHook != NULL);

	m_bFind = (bFindDialogOnly == TRUE);

	m_fr.wFindWhatLen = sizeof(m_szFindWhat)/sizeof(TCHAR);
	m_fr.lpstrReplaceWith = (LPTSTR)m_szReplaceWith;
	m_fr.wReplaceWithLen = sizeof(m_szReplaceWith)/sizeof(TCHAR);
	m_fr.Flags |= dwFlags;


	if(hWndParent == NULL)
		m_fr.hwndOwner = ::GetActiveWindow();
	else
		m_fr.hwndOwner = hWndParent;
	ATLASSERT(m_fr.hwndOwner != NULL);

	if(lpszFindWhat != NULL)
		_tcscpy(m_szFindWhat, lpszFindWhat);

	if(lpszReplaceWith != NULL)
		_tcscpy(m_szReplaceWith, lpszReplaceWith);

	ATLASSERT(m_hWnd == NULL);
	_Module.AddCreateWndData(&m_thunk.cd, (CCommonDialogImplBase*)this);

	HWND hWnd;
	if(bFindDialogOnly)
		hWnd = ATLEXT::FindText(&m_fr);
	else
		hWnd = ATLEXT::ReplaceText(&m_fr);

	ATLASSERT(m_hWnd == hWnd);
	return hWnd;
}

class CFontDlg;

class CFindReplaceDlg : public CFindReplaceDlgImpl<CFindReplaceDlg>
{
public:
	DECLARE_EMPTY_MSG_MAP()
};


int CALLBACK FontFamilyCallback(const LOGFONT* lpft, const TEXTMETRIC* lpntm, DWORD nFontType, LPARAM lParam);

const UINT Eng = 0;
const UINT Esp = 1;
const UINT Ita = 2;
const UINT Deu = 3;
const UINT Fra = 4;
const UINT Ptg = 5;

inline void SetLocaleStyles(TCHAR* styles[])
{
TCHAR* styleRegular [] = {IDS_REGULAR_FONT, IDS_REGULAR_FONT_ES,
						 IDS_REGULAR_FONT_IT, IDS_REGULAR_FONT_DE,
						 IDS_REGULAR_FONT_FR, IDS_REGULAR_FONT_PT};

TCHAR* styleItalic [] = {IDS_ITALIC_FONT, IDS_ITALIC_FONT_ES,
						 IDS_ITALIC_FONT_IT, IDS_ITALIC_FONT_DE,
						 IDS_ITALIC_FONT_FR, IDS_ITALIC_FONT_PT};

TCHAR* styleBold[] = {IDS_BOLD_FONT, IDS_BOLD_FONT_ES,
						 IDS_BOLD_FONT_IT, IDS_BOLD_FONT_DE,
						 IDS_BOLD_FONT_FR, IDS_BOLD_FONT_PT};

TCHAR* styleBoldItalic [] = {IDS_BOLD_ITALIC_FONT, IDS_BOLD_ITALIC_FONT_ES,
						 IDS_BOLD_ITALIC_FONT_IT, IDS_BOLD_ITALIC_FONT_DE,
						 IDS_BOLD_ITALIC_FONT_FR, IDS_BOLD_ITALIC_FONT_PT};
	
	const TCHAR* szEs = _T("ES");
	const TCHAR* szIt = _T("IT");
	const TCHAR* szDe = _T("DE");
	const TCHAR* szFr = _T("FR");
	const TCHAR* szPt = _T("PT");

	TCHAR szLanguage[4];
	memset(szLanguage, 0, sizeof(szLanguage));
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SABBREVLANGNAME, szLanguage,
		sizeof(szLanguage)/sizeof(TCHAR));

	CString str(szLanguage);
	UINT uiLocale = Eng;
	if(str.Find(szEs, 0) != -1)
		uiLocale = Esp;
	else if(str.Find(szIt, 0) != -1)
		uiLocale = Ita;
	else if(str.Find(szDe, 0) != -1)
		uiLocale = Deu;
	else if(str.Find(szFr, 0) != -1)
		uiLocale = Fra;
	else if(str.Find(szPt, 0) != -1)
		uiLocale = Ptg;

	styles[0] = styleRegular[uiLocale];
	styles[1] = styleItalic[uiLocale];
	styles[2] = styleBold[uiLocale];
	styles[3] = styleBoldItalic[uiLocale];
}

template <class T>
class ATL_NO_VTABLE CFontDlgImpl : public CCommonDialogImplBase
{
	UINT m_uiResult;
public:
	CHOOSEFONT m_cf;
	TCHAR m_szStyleName[64];	// contains style name after return
	LOGFONT m_lf;			// default LOGFONT to store the info

	UINT DialogCancel(){return m_uiResult;}
	void SetDialogCancel(UINT i){m_uiResult = i;}

	CHARFORMAT m_cFormat;
// Constructors
	CFontDlgImpl(CHARFORMAT cformat, LPLOGFONT lplfInitial = NULL,
			DWORD dwFlags = 0L,
			HDC hDCPrinter = NULL,
			HWND hWndParent = NULL)
	{
		m_uiResult = IDCANCEL;

		memset(&m_cf, 0, sizeof(m_cf));
		memset(&m_lf, 0, sizeof(m_lf));
		memset(&m_szStyleName, 0, sizeof(m_szStyleName));

		m_cFormat = cformat;

		m_cf.lStructSize = sizeof(m_cf);
		m_cf.hwndOwner = hWndParent;
		m_cf.rgbColors = RGB(0, 0, 0);
		m_cf.lpszStyle = (LPTSTR)&m_szStyleName;
		m_cf.Flags = dwFlags | CF_ENABLEHOOK;
		m_cf.lpfnHook = (LPCFHOOKPROC)T::CRHookProc;

		if(lplfInitial != NULL)
		{
			m_cf.lpLogFont = lplfInitial;
			m_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
			memcpy(&m_lf, m_cf.lpLogFont, sizeof(m_lf));
		}
		else
		{
			m_cf.lpLogFont = &m_lf;
		}

		if(hDCPrinter != NULL)
		{
			m_cf.hDC = hDCPrinter;
			m_cf.Flags |= CF_PRINTERFONTS;
		}
	}

// Operations
	INT_PTR DoModal(HWND hWndParent) // = ::GetActiveWindow())
	{
		ATLASSERT(m_cf.Flags & CF_ENABLEHOOK);
		ATLASSERT(m_cf.lpfnHook != NULL);	// can still be a user hook

		if(m_cf.hwndOwner == NULL)		// set only if not specified before
			m_cf.hwndOwner = hWndParent;

		ATLASSERT(m_hWnd == NULL);
		_Module.AddCreateWndData(&m_thunk.cd, (CCommonDialogImplBase*)this);

		BOOL bRet = ATLEXT::ChooseFont(&m_cf);

		m_hWnd = NULL;

		if(bRet)	// copy logical font from user's initialization buffer (if needed)
			memcpy(&m_lf, m_cf.lpLogFont, sizeof(m_lf));

		return bRet ? IDOK : IDCANCEL;
	}

	// Get the selected font (works during DoModal displayed or after)
	void GetCurrentFont(LPLOGFONT lplf) const
	{
		ATLASSERT(lplf != NULL);

		if(m_hWnd != NULL)
			::GetObject(::GetDC(m_hWnd), sizeof(LOGFONT), lplf);
		else
			*lplf = m_lf;
	}

	// Helpers for parsing information after successful return
	LPCTSTR GetFaceName() const {return (LPCTSTR)m_cf.lpLogFont->lfFaceName;}  // return the face name of the font
	LPCTSTR GetStyleName() const {return m_cf.lpszStyle;} // return the style name of the font
	int GetSize() const {return m_cf.iPointSize;} // return the pt size of the font
	COLORREF GetColor() const {return m_cf.rgbColors;} // return the color of the font
	int GetWeight() const {return (int)m_cf.lpLogFont->lfWeight;} // return the chosen font weight
	BOOL IsStrikeOut() const {return m_cf.lpLogFont->lfStrikeOut ? TRUE : FALSE;} // return TRUE if strikeout
	BOOL IsUnderline() const {return m_cf.lpLogFont->lfUnderline ? TRUE : FALSE;} // return TRUE if underline
	BOOL IsBold() const {return m_cf.lpLogFont->lfWeight == FW_BOLD ? TRUE : FALSE;} // return TRUE if bold font
	BOOL IsItalic() const {return m_cf.lpLogFont->lfItalic ? TRUE : FALSE;}        // return TRUE if italic font

	vector<pair<HFONT, CString> > vecFonts;

	static UINT_PTR APIENTRY CRHookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static TCHAR* strStyles[4];// = {IDS_REGULAR_FONT, IDS_ITALIC_FONT, IDS_BOLD_FONT, IDS_BOLD_ITALIC_FONT};
		SetLocaleStyles(strStyles);

		static CCommonDialogImplBase* pT = NULL;

		if(uMsg == WM_INITDIALOG)
		{
			if(pT == NULL)
			{
				pT = (CCommonDialogImplBase*)_Module.ExtractCreateWndData();
				ATLASSERT(pT != NULL);
				ATLASSERT(pT->m_hWnd == NULL);
				ATLASSERT(::IsWindow(hWnd));
			}

			CFontDlgImpl<T>* p = static_cast<CFontDlgImpl<T>*>(pT);
			// subclass dialog's window
			pT->SubclassWindow(hWnd);
			if(p->vecFonts.size() == 0)
				EnumFontFamilies(::GetDC(p->m_cf.hwndOwner), (LPTSTR)NULL, FontFamilyCallback, reinterpret_cast<LPARAM>(p));

			CComboBox cbxFontNames(pT->GetDlgItem(AFX_IDC_FONTNAMES));
			cbxFontNames.AddString(p->m_cFormat.szFaceName);
			vector<pair<HFONT, CString> >::iterator it;
			for(it = p->vecFonts.begin(); it != p->vecFonts.end(); it++)
			{
				if((*it).second != p->m_cFormat.szFaceName)
					cbxFontNames.AddString((*it).second);
			}

			it = find_if(p->vecFonts.begin(), p->vecFonts.end(), FindPair_second<HFONT, CString>(p->m_cFormat.szFaceName));
			_tcscpy_s(p->m_cf.lpLogFont->lfFaceName, LF_FACESIZE,p->m_cFormat.szFaceName);

			int nIndex = cbxFontNames.FindStringExact(-1, p->m_cFormat.szFaceName);
			if(CB_ERR != nIndex)
				cbxFontNames.SetCurSel(nIndex);

			CComboBox cbxFontStyles(pT->GetDlgItem(AFX_IDC_FONTSTYLES));
			for(int i = 0; i < 4; i++)
			{
				CString str(strStyles[i]);
//				str.LoadString(strStyles[i]);
				cbxFontStyles.AddString(str);
			}

			if(p->m_cFormat.dwEffects & CFE_BOLD && p->m_cFormat.dwEffects & CFE_ITALIC)
			{
				cbxFontStyles.SetCurSel(3);
				p->m_cf.lpLogFont->lfItalic = 1;
				p->m_cf.lpLogFont->lfWeight = 700;
			}
			else if(p->m_cFormat.dwEffects & CFE_BOLD)
			{
				cbxFontStyles.SetCurSel(2);
				p->m_cf.lpLogFont->lfWeight = 700;
			}
			else if(p->m_cFormat.dwEffects & CFE_ITALIC)
			{
				cbxFontStyles.SetCurSel(1);
				p->m_cf.lpLogFont->lfItalic = 1;
			}
			else
				cbxFontStyles.SetCurSel(0);

			CComboBox cbxFontSizes(pT->GetDlgItem(AFX_IDC_FONTSIZES));
			for(int i = 8; i < 41; (i<12)?i++:(i<26)?(i+=2):(i<41)?(i+=4):(i+=0))
			{
				CString str;
				str.Format(_T("%d"), i);
				cbxFontSizes.AddString(str);
			}

			// Put in explicit cast to int to stop compiler warning
			int nSize = p->m_cFormat.yHeight / POINT_SIZE;

			CString strHeight;
			strHeight.Format(_T("%d"), nSize);
			nIndex = cbxFontSizes.FindStringExact(-1, strHeight);
			if(CB_ERR != nIndex)
			{
				cbxFontSizes.SetCurSel(nIndex);
				p->m_cf.lpLogFont->lfHeight = nSize;
			}

			CButton btnStrikeOut(pT->GetDlgItem(AFX_IDC_STRIKEOUT));
			if(p->m_cFormat.dwEffects & CFE_STRIKEOUT)
			{
				btnStrikeOut.SetCheck(1);
				p->m_cf.lpLogFont->lfStrikeOut = 1;
			}

			CButton btnUnderLine(pT->GetDlgItem(AFX_IDC_UNDERLINE));
			if(p->m_cFormat.dwEffects & CFE_UNDERLINE)
			{
				btnUnderLine.SetCheck(1);
				p->m_cf.lpLogFont->lfUnderline = 1;
			}

			T::UpdateSample(p->m_hWnd, p->m_cf, p->vecFonts);

			cbxFontNames.SetFocus();
			// check message map for WM_INITDIALOG handler
			LRESULT lRes;
			if(pT->ProcessWindowMessage(pT->m_hWnd, uMsg, wParam, lParam, lRes, 0) == FALSE)
				return 0;

			return lRes;
		}
		else
		{
			CFontDlgImpl<T>* p = static_cast<CFontDlgImpl<T>*>(pT);
			switch(uMsg)
			{
			case WM_KEYDOWN:
				if(VK_RETURN == (int)wParam)
					::SendMessage(p->m_hWnd, WM_CLOSE, 0, IDOK);
				else if(VK_ESCAPE == (int)wParam)
					::SendMessage(p->m_hWnd, WM_CLOSE, 0, IDCANCEL);
				break;
			case WM_COMMAND:
				if(LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDOK)
					::SendMessage(p->m_hWnd, WM_CLOSE, 0, (LOWORD(wParam) == IDOK ? IDOK : IDCANCEL));
				else if(LOWORD(wParam) == AFX_IDC_FONTNAMES && HIWORD(wParam) == CBN_SELCHANGE)
				{
					T::UpdateSample(p->m_hWnd, p->m_cf, p->vecFonts);
				}
				else if(LOWORD(wParam) == AFX_IDC_FONTSTYLES && HIWORD(wParam) == CBN_SELCHANGE)
				{
					T::UpdateSample(p->m_hWnd, p->m_cf, p->vecFonts);
				}
				else if(LOWORD(wParam) == AFX_IDC_FONTSIZES && HIWORD(wParam) == CBN_SELCHANGE)
				{
					T::UpdateSample(p->m_hWnd, p->m_cf, p->vecFonts);
				}
				else if(LOWORD(wParam) == AFX_IDC_STRIKEOUT && HIWORD(wParam) == BN_CLICKED)
				{
					T::UpdateSample(p->m_hWnd, p->m_cf, p->vecFonts);
				}
				else if(LOWORD(wParam) == AFX_IDC_UNDERLINE && HIWORD(wParam) == BN_CLICKED)
				{
					T::UpdateSample(p->m_hWnd, p->m_cf, p->vecFonts);
				}

				break;
			case WM_QUIT:
			case WM_CLOSE:
				vector<pair<HFONT, CString> >::iterator it;
				for(it = p->vecFonts.begin(); it != p->vecFonts.end(); it++)
					::DeleteObject(it->first);

				p->vecFonts.clear();
				p->SetDialogCancel((UINT)lParam);
				::EndDialog(p->m_hWnd, lParam);
				pT = NULL;
				break;
			}
		}

		return 0;
	}

	CFontDlgImpl<T>* operator=(HWND hWnd)
	{
		if(m_hWnd == hWnd)
			return this;
		else
			return NULL;
	}

};

class CFontDlg : public CFontDlgImpl<CFontDlg>
{
public:

	CFontDlg(CHARFORMAT cformat, LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = 0L,
		HDC hDCPrinter = NULL,
		HWND hWndParent = NULL)
		: CFontDlgImpl<CFontDlg>(cformat, lplfInitial, dwFlags, hDCPrinter, hWndParent)
	{ }

	DECLARE_EMPTY_MSG_MAP()

	static void UpdateSample(HWND hWnd, CHOOSEFONT& cf, vector<pair<HFONT, CString> >& vec)
	{
		vector<pair<HFONT, CString> >::iterator it;
		CComboBox cbxFontNames(::GetDlgItem(hWnd, AFX_IDC_FONTNAMES));
		CComBSTR bstr;
		int nSel = cbxFontNames.GetCurSel();
		cbxFontNames.GetLBTextBSTR(nSel, bstr.m_str);
		it = std::find_if(vec.begin(), vec.end(), FindPair_second<HFONT, CString>(CString(bstr.m_str)));
		ATLASSERT(it != vec.end());
		LOGFONT lf;
		::GetObject(it->first, sizeof(LOGFONT), &lf);

		_tcscpy_s(cf.lpLogFont->lfFaceName, bstr.m_str);

		CComboBox cbxFontStyles(::GetDlgItem(hWnd, AFX_IDC_FONTSTYLES));
		nSel = cbxFontStyles.GetCurSel();
		if(nSel == 0)
		{
			cf.lpLogFont->lfItalic = lf.lfItalic = 0;
			cf.lpLogFont->lfWeight = lf.lfWeight = 400;
		}
		else if(nSel == 1)
		{
			cf.lpLogFont->lfItalic = lf.lfItalic = 1;
			cf.lpLogFont->lfWeight = lf.lfWeight = 400;
		}
		else if(nSel == 2)
		{
			cf.lpLogFont->lfItalic = lf.lfItalic = 0;
			cf.lpLogFont->lfWeight = lf.lfWeight = 700;
		}
		else if(nSel == 3)
		{
			cf.lpLogFont->lfItalic = lf.lfItalic = 1;
			cf.lpLogFont->lfWeight = lf.lfWeight = 700;
		}

		bstr.Empty();
		cbxFontStyles.GetLBTextBSTR(nSel, bstr.m_str);
		_tcscpy_s(cf.lpszStyle, LF_FACESIZE,bstr.m_str);

		CComboBox cbxFontSizes(::GetDlgItem(hWnd, AFX_IDC_FONTSIZES));
		if((nSel = cbxFontSizes.GetCurSel()) == CB_ERR)
			return;

		bstr.Empty();
		if(bstr.Length() != 0)
			return;

		if(!cbxFontSizes.GetLBTextBSTR(nSel, bstr.m_str))
			return;

		lf.lfHeight = (-1 * _ttoi(bstr.m_str));
		lf.lfWidth = 0;
		cf.iPointSize = lf.lfHeight;

		CButton btnStrikeOut(::GetDlgItem(hWnd, AFX_IDC_STRIKEOUT));
		cf.lpLogFont->lfStrikeOut = lf.lfStrikeOut = static_cast<BYTE>(btnStrikeOut.GetCheck());
		CButton btnUnderLine(::GetDlgItem(hWnd, AFX_IDC_UNDERLINE));
		cf.lpLogFont->lfUnderline = lf.lfUnderline =static_cast<BYTE>(btnUnderLine.GetCheck());

		HFONT font = ::CreateFontIndirect(&lf);
		CStatic staSample(::GetDlgItem(hWnd, AFX_IDC_SAMPLEBOX));
		staSample.SetFont(font);
		::DeleteObject(font);
	}
};


inline int CALLBACK FontFamilyCallback(const LOGFONT* lpft, const TEXTMETRIC* lpntm, DWORD nFontType, LPARAM lParam)
{
	CFontDlgImpl<CFontDlg>* dlg = reinterpret_cast<CFontDlgImpl<CFontDlg>* >(lParam);
	dlg->vecFonts.push_back(make_pair(::CreateFontIndirect(lpft), CString(lpft->lfFaceName)));
	return 1;
}

