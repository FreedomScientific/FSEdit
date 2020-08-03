// Format.h

#ifndef __FORMAT_H__7494759Q904751998837
#define __FORMAT_H__7494759Q904751998837

#pragma once

#include "gtutility.h"

const CString NUMBERING [] =
{
	CString(_T("None")),
	CString(_T("1) 2) 3)")),
	CString(_T("1. 2. 3.")),
	CString(_T("(1) (2) (3)")),
	CString(_T("A) B) C)")),
	CString(_T("A. B. C.")),
	CString(_T("(A) (B) (C)")),
	CString(_T("a) b) c)")),
	CString(_T("a. b. c.")),
	CString(_T("(a) (b) (c)")),
	CString(_T("I) II) III)")),
	CString(_T("I. II. III.")),
	CString(_T("(I) (II) (III)")),
	CString(_T("i) ii) iii)")),
	CString(_T("i. ii. iii.")),
	CString(_T("(i) (ii) (iii)")),
	CString(_T("Bullets"))
};

#define NUM_NUMBERING	17

// Careful with this array. Although it deals with styles
// each entry deal with the setting differently depending on
// the structure and functions that use the information.
// Additionally the order of this array is important if you
// change it eratic behavior will result. (Possible crashes too)
// -- rpm --
const pair<DWORD, pair<DWORD, UINT> > SKStyles [] =
{
	make_pair(ID_FONT_BOLD, make_pair(CFM_BOLD, CFE_BOLD)),
	make_pair(ID_FONT_ITALIC, make_pair(CFM_ITALIC, CFE_ITALIC)),
	make_pair(ID_FONT_UNDERLINE, make_pair(CFM_UNDERLINE, CFE_UNDERLINE)),
	make_pair(ID_FONT_STRIKETHROUGH, make_pair(CFM_STRIKEOUT, CFE_STRIKEOUT)),
	make_pair(ID_FONT_MORE, make_pair(0, 0)),
	make_pair(ID_PARA_ALIGNLEFT, make_pair(PFM_ALIGNMENT, PFA_LEFT)),
	make_pair(ID_PARA_ALIGNCENTER, make_pair(PFM_ALIGNMENT, PFA_CENTER)),
	make_pair(ID_PARA_ALIGNRIGHT, make_pair(PFM_ALIGNMENT, PFA_RIGHT)),
	make_pair(ID_PARA_JUSTIFY, make_pair(PFM_ALIGNMENT, PFA_JUSTIFY)),
	make_pair(ID_PARA_INCREASEINDENT, make_pair(PFM_STARTINDENT, 0)),
	make_pair(ID_PARA_DECREASEINDENT, make_pair(PFM_RIGHTINDENT, 0))
};

const pair<DWORD, pair<DWORD, UINT> > SKParaNum [] =
{
	make_pair(ID_PARA_NUMBERING_NONE, make_pair(0, 0)),
	make_pair(ID_PARA_NUMBERING_NUMBERPAREN, make_pair(2, 0)),
	make_pair(ID_PARA_NUMBERING_NUMBERPERIOD, make_pair(2, 0x200)),
	make_pair(ID_PARA_NUMBERING_PARENNUMBERPAREN, make_pair(2, 0x100)),
	make_pair(ID_PARA_NUMBERING_UCPAREN, make_pair(4, 0)),
	make_pair(ID_PARA_NUMBERING_UCPERIOD, make_pair(4, 0x200)),
	make_pair(ID_PARA_NUMBERING_PARENUCPAREN, make_pair(4, 0x100)),
	make_pair(ID_PARA_NUMBERING_LCPAREN, make_pair(3, 0)),
	make_pair(ID_PARA_NUMBERING_LCPERIOD, make_pair(3, 0x200)),
	make_pair(ID_PARA_NUMBERING_PARENLCPAREN, make_pair(3, 0x100)),
	make_pair(ID_PARA_NUMBERING_UROMANPAREN, make_pair(6, 0)),
	make_pair(ID_PARA_NUMBERING_UROMANPERIOD, make_pair(6, 0x200)),
	make_pair(ID_PARA_NUMBERING_PARENUROMANPAREN, make_pair(6, 0x100)),
	make_pair(ID_PARA_NUMBERING_LROMANPAREN, make_pair(5, 0)),
	make_pair(ID_PARA_NUMBERING_LROMANPERIOD, make_pair(5, 0x200)),
	make_pair(ID_PARA_NUMBERING_PARENLROMANPAREN, make_pair(5, 0x100)),
	make_pair(ID_PARA_NUMBERING_BULLET, make_pair(PFN_BULLET, 0))
};
const const_mem_fun_t<BOOL, CFontDlgImpl<CFontDlg> > FontDlgFun [] =
{
	mem_fun(&CFontDlg::IsBold),
	mem_fun(&CFontDlg::IsItalic),
	mem_fun(&CFontDlg::IsUnderline),
	mem_fun(&CFontDlg::IsStrikeOut)
};

class CUpdateFontStyles
{
	CFontDlg* dlg;
	CHARFORMAT& cf;
	int nIndex;
public:
	CUpdateFontStyles(CFontDlg* d, CHARFORMAT& f) : dlg(d), cf(f), nIndex(0) {}
	void operator()(const const_mem_fun_t<BOOL, CFontDlgImpl<CFontDlg> >& fun)
	{
		if(fun(dlg))
		{
			cf.dwMask |= SKStyles[nIndex].second.first;
			cf.dwEffects |= SKStyles[nIndex].second.second;
		}
		else if(cf.dwEffects & SKStyles[nIndex].second.first)
		{
			cf.dwMask |= SKStyles[nIndex].second.first; 
			cf.dwEffects &= ~SKStyles[nIndex].second.second;
		}

		nIndex++;
	}
};

// Class Indent is a functor for operating on and evaluating state
// for Indentation of Skipperview.
class Indent
{
	UINT nID;
	CRichEditCtrl& view;
	LONG lTwips;
public:
	Indent(CRichEditCtrl& r, LONG t, UINT id) : view(r), lTwips(t), nID(id){}
	
	// Used with menu selection for Right indentation only.
	bool operator()()
	{
		return ProcessKey((nID==ID_PARA_INCREASEINDENT) ? VK_UP : VK_DOWN);
	}
	
	// operator is used in conjuction with the Ctrl+Alt and Arrow keys.
	// Used for Start and Right indentation
	bool operator()(int nKey)
	{
		if(!GetAsyncKeyState(nKey))
			return false;

		return ProcessKey(nKey, true);
	}
	
	// operator is used strictly for the evaluation of the start indentation
	// only. Do not use for Right Indentation.
	bool operator()(LONG lValue, LONG lParam)
	{
		if(nID==ID_PARA_INCREASEINDENT)
			return (Evaluate(lValue + DEFAULTINDENT, lParam) && (lValue + DEFAULTINDENT) != lParam);
		else
			return !(Evaluate(lValue - DEFAULTINDENT, lParam) && (lValue - DEFAULTINDENT) != lParam);
	}

	// Main function for implementing indentation.
	bool ProcessKey(int nKey, bool bKeyPressed = false)
	{
		PARAFORMAT2 pf;
		pf.cbSize = sizeof(PARAFORMAT2);
		view.GetParaFormat(pf);
		// Since the Menu Items only work on the Start Indent and the arrow keys implement
		// both Start and Right Indent, but use the same Menu ID's, distinction must be
		// be evaluated on the key(s) being depressed.
		pf.dwMask = (nID==ID_PARA_INCREASEINDENT || bKeyPressed) ? PFM_STARTINDENT : PFM_RIGHTINDENT;
		LONG* plIndent = (LONG*)((BYTE*)&pf + Type((nKey == VK_UP || nKey == VK_DOWN)));

		if(nKey > VK_UP)
		{
			if(Evaluate((*plIndent -= DEFAULTINDENT), 0))
				*plIndent = 0;				
		}
		else
		{
			LONG* plParam = (LONG*)((BYTE*)&pf + Type(!(nKey == VK_UP || nKey == VK_DOWN)));
			if(Evaluate(*plIndent + DEFAULTINDENT, lTwips - *plParam))
				*plIndent += DEFAULTINDENT;
		}

		view.SetParaFormat(pf);
		return true;
	}
	
	// Uitility functions
	bool Evaluate(LONG lValue, LONG lParam)	{return (lValue <= lParam) ? true : false;}
	size_t Type(bool bPara)	{return (bPara) ? offsetof(PARAFORMAT2, dxStartIndent) : offsetof(PARAFORMAT2, dxRightIndent);}
};

class CFileTypeOperationsDlg : public CDialogImpl<CFileTypeOperationsDlg>
{
	int nPwdState, nRtfState, nTxtState;

public:
	enum {IDD = IDD_FILE_OPERATIONS};


	BEGIN_MSG_MAP(CFileTypeOperationsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(ID_OPTIONS_CANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDCANCEL, OnOK)
		COMMAND_ID_HANDLER(IDOK, OnOK)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DWORD dwType = REG_SZ, dwDataSize = sizeof(TCHAR)*10;
		TCHAR TData[10] = {0};
		if(GTUtility::GetRegistryEntry(HKEY_CLASSES_ROOT, _T(".psw"), &dwType, _T(""), TData, &dwDataSize))
		{
			if(CString(TData) == CString(_T("fsdfile")))
			{
				::SendMessage(GetDlgItem(CKX_FILE_TYPE_PWD), BM_SETCHECK, 1, 0);
				nPwdState = BST_CHECKED;
			}
			else
				nPwdState = BST_UNCHECKED;
		}
		else
			nPwdState = BST_UNCHECKED;

		if(GTUtility::GetRegistryEntry(HKEY_CLASSES_ROOT, _T(".rtf"), &dwType, _T(""), TData, &dwDataSize))
		{
			if(CString(TData) == CString(_T("fsdfile")))
			{
				::SendMessage(GetDlgItem(CKX_FILE_TYPE_RTF), BM_SETCHECK, 1, 0);
				nRtfState = BST_CHECKED;
			}
			else
				nRtfState = BST_UNCHECKED;
		}
		else
			nRtfState = BST_UNCHECKED;

		if(GTUtility::GetRegistryEntry(HKEY_CLASSES_ROOT, _T(".txt"), &dwType, _T(""), TData, &dwDataSize))
		{
			if(CString(TData) == CString(_T("fsdfile")))
			{
				::SendMessage(GetDlgItem(CKX_FILE_TYPE_TXT), BM_SETCHECK, 1, 0);
				nTxtState = BST_CHECKED;
			}
			else
				nTxtState = BST_UNCHECKED;
		}
		else
			nTxtState = BST_UNCHECKED;

		::SetFocus(GetDlgItem(CKX_FILE_TYPE_TXT));
	
		return 0;
	}

	// The "Cancel" button generates a ID_OPTIONS_CANCEL so the "OK" button IDCANCEL 
	// can be used by OnOK.
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	// The Command Bar "OK" button will generate an IDCANCEL if the PACmate
	// "Close" (Escape) key is pressed.
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		int nCheck =static_cast<int>(::SendMessage(GetDlgItem(CKX_FILE_TYPE_PWD), BM_GETCHECK, 0, 0));
		if(nCheck != nPwdState)
		{
			TCHAR* TData = nCheck ? _T("fsdfile\0") : _T("pwdfile\0");
			DWORD dwDataSize = sizeof(TCHAR)*8;
			if(!GTUtility::SetRegistryEntry(HKEY_CLASSES_ROOT, _T(".psw"), REG_SZ, _T(""), TData, dwDataSize))
			{
//				ASSERT(0);
			}
		}

		nCheck = static_cast<int>(::SendMessage(GetDlgItem(CKX_FILE_TYPE_RTF), BM_GETCHECK, 0, 0));
		if(nCheck != nRtfState)
		{
			TCHAR* TData = nCheck ? _T("fsdfile\0") : _T("rtffile\0");
			DWORD dwDataSize = sizeof(TCHAR)*8;
			if(!GTUtility::SetRegistryEntry(HKEY_CLASSES_ROOT, _T(".rtf"), REG_SZ, _T(""), TData, dwDataSize))
			{
//				ASSERT(0);
			}
		}

		nCheck = static_cast<int>(::SendMessage(GetDlgItem(CKX_FILE_TYPE_TXT), BM_GETCHECK, 0, 0));
		if(nCheck != nTxtState)
		{
			TCHAR* TData = nCheck ? _T("fsdfile\0") : _T("txtfile\0");
			DWORD dwDataSize = sizeof(TCHAR)*8;
			if(!GTUtility::SetRegistryEntry(HKEY_CLASSES_ROOT, _T(".txt"), REG_SZ, _T(""), TData, dwDataSize))
			{
//				ASSERT(0);
			}
		}

		EndDialog(IDOK);
		return 0;
	}
};

const pair<CString, CString> DATEFORMAT [] =
{
	make_pair(CString(_T("M/d/yyyy")), CString(_T(""))),
	make_pair(CString(_T("dddd, MMMM dd, yyyy")), CString(_T(""))),
	make_pair(CString(_T("MMMM d, yyyy")), CString(_T(""))),
	make_pair(CString(_T("M/d/yy")), CString(_T(""))),
	make_pair(CString(_T("yyyy-MM-dd")), CString(_T(""))),
	make_pair(CString(_T("d-MMM-yy")), CString(_T(""))),
	make_pair(CString(_T("M.d.yyyy")), CString(_T(""))),
	make_pair(CString(_T("MMM. d, yy")), CString(_T(""))),
	make_pair(CString(_T("d MMMM yyyy")), CString(_T(""))),
	make_pair(CString(_T("MMMM yy")), CString(_T(""))),
	make_pair(CString(_T("MMM-yy")), CString(_T(""))),
	make_pair(CString(_T("M/d/yyyy")), CString(_T("h:mm tt"))),
	make_pair(CString(_T("M/d/yyyy")),CString(_T("h:mm:ss tt"))),
	make_pair(CString(_T("")),CString(_T("h:mm tt"))),
	make_pair(CString(_T("")),CString(_T("h:mm:ss tt"))),
	make_pair(CString(_T("")),CString(_T("hh:mm"))),
	make_pair(CString(_T("")),CString(_T("hh:mm:ss")))
};

#define NUM_DATEFORMAT	17

const pair<CString, CString> SPDIVIDER [] =
{
	make_pair(CString(_T("-----------")), CString(_T(""))),
	make_pair(CString(_T("...........")), CString(_T(""))),
	make_pair(CString(_T("----")), CString(_T("----"))),
	make_pair(CString(_T("....")), CString(_T("...."))),
	make_pair(CString(_T("")), CString(_T("")))
};

#define NUM_SPDIVIDER	5

inline CString GetFormatDateTimeString(int nIndex, SYSTEMTIME& st)
{
	if(nIndex < 0 || nIndex > NUM_DATEFORMAT)
		return CString();

	TCHAR buf[255];
	CString str;
	if(!DATEFORMAT[nIndex].first.IsEmpty())
	{
		memset(buf, 0, sizeof(buf));
		GetDateFormat(LOCALE_NEUTRAL, NULL, &st, DATEFORMAT[nIndex].first, buf, (sizeof(buf)/sizeof(TCHAR)));
		str += CString(buf);
	}

	if(!DATEFORMAT[nIndex].second.IsEmpty())
	{
		memset(buf, 0, sizeof(buf));
		GetTimeFormat(LOCALE_NEUTRAL, NULL, &st, DATEFORMAT[nIndex].second, buf, (sizeof(buf)/sizeof(TCHAR)));
		if(!str.IsEmpty())
			str += _T(' ') + CString(buf);
		else
			str += CString(buf);
	}

	return str;
}

class COptionsDlg : public CDialogImpl<COptionsDlg>
{
	CButton m_btnWordWrap;
	CButton m_btnAutosave;
//	CComboBox m_cbxParaNum;
	CComboBox m_cbxSPLDivider;
	CComboBox m_cbxDateTimeFormat;

	int nWordWrap, /* nParaNum,*/ nDivider, nDTF, nAuto;

public:
	int GetWordWrap() const {return nWordWrap;}
//	int GetParaNum() const {return nParaNum;}
	int GetScatchPadDiv() const {return nDivider;}
	int GetDateTimeFormat() const {return nDTF;}
	int GetAutoSave() const {return nAuto;}

	enum {IDD = IDD_OPTIONS};

	COptionsDlg(int nW = 1, int nP = 0, int nD = 0, int nF = 0) : nWordWrap(nW),
		/*nParaNum(nP),*/ nDivider(nD), nDTF(nF), nAuto(nP) {}

	BEGIN_MSG_MAP(COptionsDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(BTN_ADV_FILE_OP, OnFileOperations)
		COMMAND_ID_HANDLER(ID_OPTIONS_CANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnOK)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_btnWordWrap.Attach(GetDlgItem(IDXBX_WORDWRAP));
		m_btnWordWrap.SetCheck(nWordWrap);

//		m_cbxParaNum.Attach(GetDlgItem(IDCBX_PARA_NUMBERING));
		m_cbxSPLDivider.Attach(GetDlgItem(IDCBX_SCRATCH_PAD_LINE_DIVIDER));
		m_cbxDateTimeFormat.Attach(GetDlgItem(IDCBX_DATE_TIME_FORMAT));

		m_btnAutosave.Attach(GetDlgItem(IDCKB_AUTOSAVE));
		m_btnAutosave.SetCheck(nAuto);

		SYSTEMTIME st;
		GetLocalTime(&st);

		for(int i = 0; i < NUM_DATEFORMAT; i++)
		{
			CString str = GetFormatDateTimeString(i, st);
			m_cbxDateTimeFormat.AddString(str);
		}
		
		m_cbxDateTimeFormat.SetCurSel(nDTF);

		
		CString strDateTime = GetFormatDateTimeString(nDTF, st);
		for(int j = 0; j < NUM_SPDIVIDER; j++)
		{
			if(SPDIVIDER[j].first.IsEmpty() && SPDIVIDER[j].second.IsEmpty())
				m_cbxSPLDivider.AddString(strDateTime);
			else if(!SPDIVIDER[j].first.IsEmpty() && !SPDIVIDER[j].second.IsEmpty())
			{
				CString strT = SPDIVIDER[j].first +  _T(' ') + strDateTime + _T(' ') + SPDIVIDER[j].second;
				m_cbxSPLDivider.AddString(strT);
			}
			else
				m_cbxSPLDivider.AddString(SPDIVIDER[j].first);
		}

		m_cbxSPLDivider.SetCurSel(nDivider);

		// Start NUMBERING array at 1 so we don't add 'None' as a user option.
//		for(int k = 1; k < NUM_NUMBERING; k++)
//			m_cbxParaNum.AddString(NUMBERING[k]);

		// Set selection at one minus nParaNum because of the array offset.
		// Don't subtract if nParaNum is already zero.
//		if (nParaNum > 0)
//			m_cbxParaNum.SetCurSel(nParaNum - 1);
//		else
//			m_cbxParaNum.SetCurSel(nParaNum);
		m_cbxDateTimeFormat.SetFocus();

		// This "OK" button will return an IDCANCEL when the "Close" button is used

		return 0;
	}

	// The "Cancel" button generates a ID_OPTIONS_CANCEL so the "OK" button IDCANCEL 
	// can be used by OnOK.
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	// The Command Bar "OK" button will generate an IDCANCEL if the PACmate
	// "Close" (Escape) key is pressed.
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		nWordWrap = m_btnWordWrap.GetCheck();
		// Set nParaNum at one plus the current selection because of the array offset.
//		nParaNum = m_cbxParaNum.GetCurSel() + 1;
		nAuto = m_btnAutosave.GetCheck();
		nDivider = m_cbxSPLDivider.GetCurSel();
		nDTF = m_cbxDateTimeFormat.GetCurSel();
		EndDialog(IDOK);
		return 0;
	}

	LRESULT OnFileOperations(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		CFileTypeOperationsDlg dlg;
		dlg.DoModal();
		return 0;
	}
};

typedef int (COptionsDlg::*pOpt)() const;
const pOpt OPTFun [] =
{
	&COptionsDlg::GetDateTimeFormat,
//	&COptionsDlg::GetParaNum,
	&COptionsDlg::GetScatchPadDiv,
	&COptionsDlg::GetWordWrap,
	&COptionsDlg::GetAutoSave,
};

#endif