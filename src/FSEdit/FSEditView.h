// skipperView.h : interface of the CskipperView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_skipperVIEW_H__1F0C8EA1_BFF8_4AF8_BF2D_B7EF70C93AF0__INCLUDED_)
#define AFX_skipperVIEW_H__1F0C8EA1_BFF8_4AF8_BF2D_B7EF70C93AF0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\brledit\brleditctrl.h"

#define BRL_CLASSNAME	_T("BRAILLE_EDIT")
#define WM_UPDATE_TITLE		WM_USER+1002

enum SkipperViewMode { normalMode, searchMode, gotoMode, doneWithGotoMode, markSettingMode };
class CMainFrame;

class CFSEditView : public CWindowImpl<CFSEditView, CBrlEditCtrl>,
	public CBrlEditCommands<CFSEditView>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,BRLEDIT_CLASS)
		
		CFSEditView()
#ifdef BOOKMARK_SUPPORT
		: m_nCurBookMark(-1), nStatus(0), nTemp(0) 
#endif
	{}

	HWND CreateExtended(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		GetWndClassInfo().m_wc.lpszClassName = BRL_CLASSNAME;

		return Create(hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	}

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}
	
	BEGIN_MSG_MAP(CFSEditView)
		MESSAGE_HANDLER(WM_CHAR, OnChar)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
//		MESSAGE_HANDLER(WM_KEYUP, OnKeyUp)
		/* The following notification is received when the limit of characters 
			set for the edit window is reached. Even though the limit is increased 
			in the handler for this notification, the character that triggers 
			the notification is lost. So this is here as a backup precaution. 
			Ideally we should never reach the limit. 1/21/03 AG */
		COMMAND_CODE_HANDLER(EN_MAXTEXT,OnLimitTextReached)
		CHAIN_MSG_MAP_ALT(CRichEditCommands<CFSEditView>, 1)
		CHAIN_MSG_MAP_ALT(CBrlEditCommands<CFSEditView>, 1)
	END_MSG_MAP()

	SkipperViewMode _mode;
	CMainFrame *owner;
	void startSearchMode() { _mode = searchMode; }
	void startGotoMode() { _mode = gotoMode; }
	void stopGotoMode() { _mode = doneWithGotoMode; }
	void startMarkSettingMode() { _mode = markSettingMode; }
	int getLineIndex(int line)
	{
     // Send the EM_LINEINDEX message.
     // The return value is the number of characters that
     // precede the first character in the line 
        auto rc = SendMessage(m_hWnd,EM_LINEINDEX, line, 0);
		return static_cast<int>(rc);
    }
	void moveCaretToThisCharacterIndex(int charIdx)
	{
        long pos = static_cast<long>(SendMessage(m_hWnd,EM_POSFROMCHAR, charIdx, 0));
    // get the low-order word from sel
		if (pos != -1) {
            int x = pos & 0xffff;
            int y = (pos-x)/65536; //& 0xffff0000);
            //int y2 = y/16;
	        // attempt to move caret there
			if (SetCaretPos(x, y) != 0) {
				if (x < 0 || y < 0)
                    SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0);
			}
		}
    }
	void getCaretLineColumn(int &line, int &col)
	{
     // from http://www.codeguru.com/Csharp/.NET/net_general/tipstricks/article.php/c8145/
     // save the handle reference for the ExtToolBox
     //HandleRef hr = new HandleRef(this, base.Handle );
     // Send the EM_LINEFROMCHAR message with the value of
     // -1 in wParam.
     // The return value is the zero-based line number
     // of the line containing the caret.
        int l = (int)SendMessage(m_hWnd,EM_LINEFROMCHAR,static_cast<WPARAM>(-1), 0);
     // Send the EM_GETSEL message to the ToolBox control.
     // The low-order word of the return value is the
     // character position of the caret relative to the
     // first character in the ToolBox control,
     // i.e. the absolute character index.
        int sel = (int)SendMessage(m_hWnd, EM_GETSEL,0, 0);
     // get the low-order word from sel
        int ai  = sel & 0xffff;
     // Send the EM_LINEINDEX message with the value of -1
     // in wParam.
     // The return value is the number of characters that
     // precede the first character in the line containing
     // the caret.
        int li = (int)SendMessage(m_hWnd,EM_LINEINDEX,static_cast<WPARAM>(-1), 0);
     // Subtract the li (line index) from the ai
     // (absolute character index),
     // The result is the column number of the caret position
     // in the line containing the caret.
        col = ai - li;
		line = l;
     // Remember these are zero-based.
     // Return a CharPoint with the caret current (L,C)
     // position
        //line = l+1;
		//col = col+1;
				//::SendMessage(m_hWnd, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }
	void resumeNormalMode() 
	{ 
		_mode = normalMode; 

	}
	void setMyOwner(CMainFrame *_owner) { owner = _owner; }
	void callMyOwnerSearchNext(); // implementation provided in mainfrm.cpp
	void callMyOwnerProcessGoto(unsigned int nc); // implementation provided in mainfrm.cpp

    LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		switch (_mode)
		{
			case doneWithGotoMode:
				// sort of a hack to avoid normal handling of the 
				// last key entered while in gotoMode
				resumeNormalMode();
				bHandled = FALSE;
				break;
			case gotoMode:
	            callMyOwnerProcessGoto(static_cast<unsigned int>(wParam)); 
			    bHandled = TRUE;
				break;
			default:
  				bHandled = FALSE;
				if(!GetModify())
					::PostMessage(GetParent(), WM_UPDATE_TITLE, 1, 0);
				break;
        }
		return 0;
	}

	LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (_mode == searchMode ) {
			if (wParam == 13) { // they are hitting return 
                callMyOwnerSearchNext(); 
				bHandled = TRUE;
                return 0;
			} else {
				_mode = normalMode; 
			}
		}
		if((wParam == 0x51) && (::GetKeyState(VK_CONTROL) < 0))
			::PostMessage(GetParent(), WM_COMMAND, ID_APP_EXIT, 0);
		
		bHandled = FALSE;
		return 1;
	}
#if 0
	LRESULT OnKeyUp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}
#endif
	LRESULT OnLimitTextReached(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
	{
//		UINT uLimit = GetLimitText();
		LONG nLen = GetTextLength();
		::SendMessage(m_hWnd,EM_EXLIMITTEXT,0,nLen + 102400);
//		bHandled = FALSE;
		return 0;
	}

#ifdef BOOKMARK_SUPPORT
private:
	POINT ptCaretPos;
	int m_cyChar;
	// Book mark variables.
	int m_nCurBookMark;
	int nStatus, nTemp;
	CHARRANGE crBookM;

	// Book mark functions
	void SetupSel(bool Select = true);
	void RestoreSel(bool bSelect = true);
	void BookMarkFormat(CHARFORMAT2& cf, bool bSet = true);
	void MarkCharFormat(int nValue);

	int GoToBookMark(int nIndex);

	bool IsBookMark();
	bool IsBookMark(CHARFORMAT2& cf);
public:
	int GetBookMarkPos(){return m_nCurBookMark;}
	void ClrCurrentBookMark(){m_nCurBookMark = -1;}
	void DeleteAllBookMarks(){ResetBookMark();}
	bool PutBookMark();
	bool DeleteBookMark();
	bool GotoNextBookMark();
	bool GotoPrevBookMark();
	int BookMark()
	{
		CHARRANGE cr;
		GetSel(cr); 
		return SetBookMark(MAKELPARAM(cr.cpMin, cr.cpMax));
	}
#endif
};

#ifdef BOOKMARK_SUPPORT
inline void CFSEditView::BookMarkFormat(CHARFORMAT2& cf, bool bSet /* = true */)
{
	UINT nMsg = bSet ? EM_SETCHARFORMAT: EM_GETCHARFORMAT;
	::SendMessage(m_hWnd, nMsg, SCF_SELECTION, (LPARAM)&cf);
}
// Marks the character as a bookmark(ours anyway).
inline void CFSEditView::MarkCharFormat(int nValue)
{
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_ANIMATION;
	cf.bAnimation = (BYTE)nValue;
	BookMarkFormat(cf);
}

inline bool CFSEditView::IsBookMark()
{
	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_ANIMATION;

	BookMarkFormat(cf, false);
	return (cf.bAnimation != 0);
}

inline bool CFSEditView::IsBookMark(CHARFORMAT2& cf)
{
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_ANIMATION;

	BookMarkFormat(cf, false);
	return (cf.bAnimation != 0);
}

// Function works with RestoreSel. SetupSel hi-lite's one character
// for set the character format.
inline void CFSEditView::SetupSel(bool bSelect /* = true */)
{
	GetSel(crBookM);
	if(crBookM.cpMin == crBookM.cpMax)
	{
		crBookM.cpMax = crBookM.cpMin + 1;
		nStatus = 1;
	}
	else if(crBookM.cpMax < crBookM.cpMin)
	{
		nTemp = crBookM.cpMin;
		crBookM.cpMin = crBookM.cpMax;
		crBookM.cpMax++;
		nStatus = 2;
	}
	else
	{
		nTemp = crBookM.cpMax;
		crBookM.cpMax = crBookM.cpMin + 1;
	}

	if(bSelect)
		SetSel(crBookM);
}

// Restores caret and hi-lite to original state.
inline void CFSEditView::RestoreSel(bool bSelect /* = true */)
{
	if(1 == nStatus)
		crBookM.cpMax = crBookM.cpMin;
	else if(2 == nStatus)
	{
		crBookM.cpMax--;
		crBookM.cpMin = nTemp;
	}
	else
		crBookM.cpMax = nTemp;

	if(bSelect)
		SetSel(crBookM);	

	nStatus = nTemp = 0;
}

inline bool CFSEditView::PutBookMark()
{
//	BookMark();
	SetupSel();

	int nIndex;
	if(IsBookMark() || (nIndex = BookMark()) == -1)
	{
		RestoreSel();
		return false;
	}
	else
		m_nCurBookMark = nIndex;

	// Set the animation byte for bookmark 
	MarkCharFormat(GetBookMarkID(m_nCurBookMark)); 

	RestoreSel();
	return true;
}

inline bool CFSEditView::DeleteBookMark()
{
	SetupSel();
	CHARFORMAT2 cf;
	if(!IsBookMark(cf))
	{
		RestoreSel();
		return false;
	}
	
	MarkCharFormat(0);

	if(DeleteBookMarkByMark(cf.bAnimation))
	{
		int nCount = GetBookMarkCount();
		if(nCount == 0)
			m_nCurBookMark = -1;
		else if(m_nCurBookMark != 0)
			m_nCurBookMark--;
	}

	RestoreSel();
	return true;
}

inline int CFSEditView::GoToBookMark(int nIndex)
{
	DWORD dwData = GetBookMarkData(nIndex);

	int nMark, nLen, nCharF, nCharB;
	nMark = LOBYTE(dwData);
	nCharF = nCharB = HIWORD(dwData);
	nCharB--;
	nLen = GetTextLength();
	if(nLen < nCharB)
	{
		nCharF = 0;
		nCharB = nLen - 1;
	}

	CHARFORMAT2 cf;
	memset(&cf, 0, sizeof(CHARFORMAT2));
	cf.cbSize = sizeof(CHARFORMAT2);
	cf.dwMask = CFM_ANIMATION;

	do
	{
		if(nCharF < nLen)
		{
			CHARRANGE cr = { nCharF, nCharF+1 };
			SetSel(cr);
			::SendMessage(m_hWnd, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			if(cf.bAnimation == nMark)
			{
				SetSel(nCharF, nCharF);
				if(nCharF != HIWORD(dwData))
					UpdateBookMark(nIndex, BOOKMARK(nMark, 0, (WORD)nCharF));
				return nIndex;
			}
			nCharF++;
		}
		
		if(nCharB >= 0)
		{
			CHARRANGE cr = { nCharB, nCharB+1 };
			SetSel(cr);
			::SendMessage(m_hWnd, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
			if(cf.bAnimation == nMark)
			{
				SetSel(nCharB, nCharB);
				if(nCharB != HIWORD(dwData))
					UpdateBookMark(nIndex, BOOKMARK(nMark, 0, (WORD)nCharB));
				return nIndex;
			}
			nCharB--;
		}
	}
	while((nCharB >= 0 || nCharF < nLen));

	// Book mark doesn't exist any more. Delete!!!
	DeleteBookMarkByIndex(nIndex);
	return m_nCurBookMark;
}

inline bool CFSEditView::GotoNextBookMark()
{
//	int nCount = GetBookMarkCount();

//	if((m_nCurBookMark + 1) > nCount-1)
//		m_nCurBookMark = 0;
//	else
//		m_nCurBookMark++;

//	MoveToBookMark(m_nCurBookMark);
	
	if(GetBookMarkCount() > 0)
	{
		m_nCurBookMark++;
		if(m_nCurBookMark > 25)
			m_nCurBookMark = 0;

		int nIndex =  m_nCurBookMark;
		do
		{
			if(IsValidBookMark(nIndex))
			{
				::HideCaret(m_hWnd);
				SetRedraw(FALSE);
				m_nCurBookMark = GoToBookMark(nIndex);
				SetRedraw(TRUE);
				::ShowCaret(m_hWnd);
				Invalidate();
				break;
			}
			else
				nIndex = (nIndex > 25) ? 0 : nIndex + 1;
		}while(nIndex != m_nCurBookMark);

		return true;
	}

	return false;
}

inline bool CFSEditView::GotoPrevBookMark()
{
//	int nCount = GetBookMarkCount();
//
//	if((m_nCurBookMark - 1) < 0)
//		m_nCurBookMark = nCount - 1;
//	else
//		m_nCurBookMark--;

//	MoveToBookMark(m_nCurBookMark);

	if(GetBookMarkCount() > 0)
	{
		m_nCurBookMark--;
		if(m_nCurBookMark < 0)
			m_nCurBookMark = 25;

		int nIndex =  m_nCurBookMark;
		do
		{
			if(IsValidBookMark(nIndex))
			{
				::HideCaret(m_hWnd);
				SetRedraw(FALSE);
				m_nCurBookMark = GoToBookMark(nIndex);
				SetRedraw(TRUE);
				::ShowCaret(m_hWnd);
				Invalidate();
				break;
			}
			else
				nIndex = (nIndex < 0) ? 25 : nIndex - 1;
		}while(nIndex != m_nCurBookMark);

		return true;
	}

	return false;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_skipperVIEW_H__1F0C8EA1_BFF8_4AF8_BF2D_B7EF70C93AF0__INCLUDED_)
