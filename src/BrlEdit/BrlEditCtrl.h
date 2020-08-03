#ifndef __BRLEDITCTRL_H_
#define __BRLEDITCTRL_H_

#include <utility>
#include <tom.h>
#include "brleditdefs.h"
#include <ATLCTRLS.H> 
#include <algorithm>
#include <vector>
__interface IBrailleTranslator;

struct DocProp
{
	long caretPosition;
	long markPosition;
	long brailleGrade2;
	long pageHeight;
	long pageWidth;
	long pageNumbering;	// 0-None, 1-Arabic, 2-Roman
	long topMargin;
	long bottomMargin;
	long leftMargin;
	long rightMargin;
#ifdef BOOKMARK_SUPPORT
	long bookmark[NUMOFBOOKMARKS];
#endif
};

const std::pair<size_t, std::pair<UINT, UINT> > UWMsg [] =
{
	std::make_pair(offsetof(DocProp, caretPosition), std::make_pair(BEM_SETCARETPOS, BEM_GETCARETPOS)),
	std::make_pair(offsetof(DocProp, markPosition), std::make_pair(BEM_SETMARK, BEM_GETMARK)),
	std::make_pair(offsetof(DocProp, brailleGrade2), std::make_pair(BEM_SETGRADE2, BEM_GETGRADE2)),
	std::make_pair(offsetof(DocProp, pageHeight), std::make_pair(BEM_SETPAGEHEIGHT, BEM_GETPAGEHEIGHT)),
	std::make_pair(offsetof(DocProp, pageWidth), std::make_pair(BEM_SETPAGEWIDTH, BEM_GETPAGEWIDTH)),
	std::make_pair(offsetof(DocProp, pageNumbering), std::make_pair(BEM_SETPAGENUMBERING, BEM_GETPAGENUMBERING)),
	std::make_pair(offsetof(DocProp, topMargin), std::make_pair(BEM_SETTOPMARGIN, BEM_GETTOPMARGIN)),
	std::make_pair(offsetof(DocProp, bottomMargin), std::make_pair(BEM_SETBOTTOMMARGIN, BEM_GETBOTTOMMARGIN)),
	std::make_pair(offsetof(DocProp, leftMargin), std::make_pair(BEM_SETLEFTMARGIN, BEM_GETLEFTMARGIN)),
	std::make_pair(offsetof(DocProp, rightMargin), std::make_pair(BEM_SETRIGHTMARGIN, BEM_GETRIGHTMARGIN))
};

#define NUM_UWMSG	ArraySize<sizeof(UWMsg),sizeof(pair<size_t, pair<UINT, UINT> >)>::Val
#define Param(p,d)	(*((long*)((BYTE*)&p + d)))

template <class TBase>
class CBrlEditCtrlT : public TBase
{
public:
	void SetMarkFromSel()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd,BEM_SETMARKFROMSEL,0,0);
	}
	void ClearMark()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd,BEM_SETMARK,0,-1);
	}
	void SelectToMark()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd,BEM_SELECTTOMARK,0,0);
	}
	void MoveToMark()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd,BEM_MOVETOMARK,0,0);
	}
	void MarkUnit(long unit)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd,BEM_MARKUNIT,0,unit);
	}
	void SelectUnit(long unit)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd,BEM_SELECTUNIT,0,unit);
	}

	BOOL OpenFile(CString& strFilePath)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		UINT nResult = static_cast<UINT>(::SendMessage(m_hWnd,BEM_OPENFILE,0,(LPARAM)(LPCTSTR)strFilePath));
		if(nResult == 2)
		{
			CString str = strFilePath;
			strFilePath = strFilePath.Left(strFilePath.GetLength()-4);
			strFilePath += _T(".brf");
			MoveFile((LPCTSTR)str, strFilePath);
		}

		return nResult;
	}

	BOOL SaveFile(LPCTSTR lpszFilePath, BOOL bNewFile)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return static_cast<int>(::SendMessage(m_hWnd, BEM_SAVEFILE, (WPARAM)bNewFile, (LPARAM)lpszFilePath));
	}
	BOOL SaveFileAs(LPCTSTR lpszFilePath, BOOL bNewFile)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return static_cast<int>(::SendMessage(m_hWnd, BEM_SAVEFILEAS, (WPARAM)bNewFile, (LPARAM)lpszFilePath));
	}

	BOOL Find(BOOL bFindOnly)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::PostMessage(m_hWnd,WM_FINDREPLACE,0,(LPARAM)bFindOnly);
	}

	BOOL InsertFileText(LPCTSTR lpszFilePath, CHARRANGE cr, int nInsertAs )
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LPARAM lp = MAKELPARAM(cr.cpMin, cr.cpMax);
		ATLASSERT(lp > -1);
		if(lp < 0)
			lp = 0;
		::SendMessage(m_hWnd, BEM_INSERTFILEPARAM, nInsertAs, 0);
		return static_cast<int>(::SendMessage(m_hWnd, BEM_INSERTFILE, (WPARAM)lpszFilePath, lp));
	}
	void SetNotify(HWND hwnd)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BEM_SETNOTIFY, 0, (LPARAM)hwnd);
	}
	BOOL GetBrlTrans(IBrailleTranslator **pp)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, BEM_GETBRLTRANS, 0, (LPARAM)pp);
	}
};

typedef CBrlEditCtrlT<CRichEditCtrl> CBrlEditCtrl;

template<class T, bool bSet = true>
class CProps
{
	T pT;
	DocProp& dp;
public:
	CProps(T p, DocProp& d) : pT(p), dp(d) {}
	void operator()(const std::pair<size_t, std::pair<UINT, UINT> > pr)
	{
		if(bSet)
			pT->SendMessage(pT->m_hWnd, pr.second.first, 0, Param(dp, pr.first));
		else
			Param(dp, pr.first) = static_cast<long>(pT->SendMessage(pT->m_hWnd, pr.second.second, 0, 0));
	}
};

template <class T>
class CBrlEditCommands : public CRichEditCommands<T>
{
public:
	BEGIN_MSG_MAP(CBrlEditCommands< T >)
	ALT_MSG_MAP(1)
		COMMAND_ID_HANDLER(ID_EDIT_SETMARK, OnEditSetMark)
		COMMAND_ID_HANDLER(ID_EDIT_CLEARMARK, OnEditClearMark)
		COMMAND_ID_HANDLER(ID_EDIT_SELECTTOMARK, OnEditSelectToMark)
		COMMAND_ID_HANDLER(ID_EDIT_MOVETOMARK, OnEditMoveToMark)
		COMMAND_ID_HANDLER(ID_MARK_WORD,OnEditMarkWord)
		COMMAND_ID_HANDLER(ID_MARK_SENTENCE,OnEditMarkSentence)
		COMMAND_ID_HANDLER(ID_MARK_PARA,OnEditMarkPara)
		COMMAND_ID_HANDLER(ID_MARK_LINE,OnEditMarkLine)
		COMMAND_ID_HANDLER(ID_MARK_DOC,OnEditMarkDoc)
		COMMAND_ID_HANDLER(ID_SELECT_WORD,OnEditSelectWord)
		COMMAND_ID_HANDLER(ID_SELECT_SENTENCE,OnEditSelectSentence)
		COMMAND_ID_HANDLER(ID_SELECT_PARA,OnEditSelectPara)
		COMMAND_ID_HANDLER(ID_SELECT_LINE,OnEditSelectLine)
		COMMAND_ID_HANDLER(ID_SELECT_DOC,OnEditSelectDoc)
		CHAIN_MSG_MAP_ALT(CRichEditCommands<T>,1)
	END_MSG_MAP()


protected:
#ifdef BOOKMARK_SUPPORT
	int SetBookMark(DWORD dwMark)
	{
		T* pT = static_cast<T*>(this);
		return pT->SendMessage(pT->m_hWnd, BEM_SETBOOKMARK, 0, (LPARAM)dwMark);
	}

	DWORD GetBookMarkData(int nIndex)
	{
		T* pT = static_cast<T*>(this);
		return pT->SendMessage(pT->m_hWnd, BEM_GETBOOKMARK, (WPARAM)nIndex, 0);
	}

	bool DeleteBookMarkByMark(int nMark)
	{
		T* pT = static_cast<T*>(this);
		return (pT->SendMessage(pT->m_hWnd, BEM_DELETEBOOKMARK_BYLINE, 0, (LPARAM)nMark) == 0);
	}

	bool DeleteBookMarkByIndex(int nIndex)
	{
		T* pT = static_cast<T*>(this);
		return (pT->SendMessage(pT->m_hWnd, BEM_DELETEBOOKMARK_BYLINE, (WPARAM)nIndex, 0) == 0);
	}

	bool IsValidBookMark(int nIndex)
	{
		T* pT = static_cast<T*>(this);
		return (pT->SendMessage(pT->m_hWnd, BEM_ISVALIDBOOKMARK, (WPARAM)nIndex, 0) == 0);
	}

	int GetBookMarkCount()
	{
		T* pT = static_cast<T*>(this);
		return pT->SendMessage(pT->m_hWnd, BEM_GETBOOKMARKCOUNT, 0, 0);
	}

	void ResetBookMark()
	{
		T* pT = static_cast<T*>(this);
		pT->SendMessage(pT->m_hWnd, BEM_RESETBOOKMARK, 0, 0);
	}

	int GetBookMarkID(int nIndex)
	{
		T* pT = static_cast<T*>(this);
		return pT->SendMessage(pT->m_hWnd, BEM_GETBOOKMARKID, 0, (LPARAM)nIndex);
	}
	
	void UpdateBookMark(int nIndex, DWORD dwParam)
	{
		T* pT = static_cast<T*>(this);
		pT->SendMessage(pT->m_hWnd, BEM_UPDATEBOOKMARK, (WPARAM)nIndex, (LPARAM)dwParam);
	}
/*
	bool MoveToBookMark(int nIndex)
	{
		T* pT = static_cast<T*>(this);
		return (pT->SendMessage(pT->m_hWnd, BEM_MOVETOBOOKMARK, 0, (LPARAM)nIndex) == 0);
	}
*/
#endif
public:
	void InitNetworkPrinters(bool bReInit = false)
	{
		T* pT = static_cast<T*>(this);
		pT->SendMessage(pT->m_hWnd, BEM_INITPRINTERS, 0, (LPARAM)bReInit);
	}

	BOOL PrintDocument()
	{
		T* pT = static_cast<T*>(this);
		return static_cast<int>(pT->SendMessage(pT->m_hWnd, BEM_PRINTDOCUMENT, 0, 0));
	}

	void SetDocumentProp(DocProp& rProp)
	{
		T* pT = static_cast<T*>(this);
		for_each(UWMsg, UWMsg+NUM_UWMSG, CProps<T*>(pT, rProp));
	}

	void GetDocumentProp(DocProp& rProp)
	{
		T* pT = static_cast<T*>(this);
		for_each(UWMsg, UWMsg+NUM_UWMSG, CProps<T*, false>(pT, rProp));
	}

	long GetCaret()
	{
		T* pT = static_cast<T*>(this);
		return static_cast<int>(pT->SendMessage(pT->m_hWnd, BEM_GETCARETPOS, 0, 0));
	}

	void SetCaret(long lCaret)
	{
		T* pT = static_cast<T*>(this);
		pT->SendMessage(pT->m_hWnd, BEM_SETCARETPOS, 0, lCaret);
	}

	long GetMarker()
	{
		T* pT = static_cast<T*>(this);
		return static_cast<int>(pT->SendMessage(pT->m_hWnd, BEM_GETMARK, 0, 0));
	}
	void FindReplaceCommand(DWORD dwIndex)
	{
		T* pT = static_cast<T*>(this);
		PostMessage(pT->m_hWnd, BEM_FINDREPLACE, 0, dwIndex);
	}

	LRESULT OnEditSetMark(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SetMarkFromSel();
		return 0;
	}
	LRESULT OnEditClearMark(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->ClearMark();
		return 0;
	}
	LRESULT OnEditSelectToMark(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SelectToMark();
		return 0;
	}
	LRESULT OnEditMoveToMark(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->MoveToMark();
		return 0;
	}
	LRESULT OnEditMarkWord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->MarkUnit(tomWord);
		return 0;
	}
	LRESULT OnEditMarkSentence(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->MarkUnit(tomSentence);
		return 0;
	}
	LRESULT OnEditMarkPara(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->MarkUnit(tomParagraph);
		return 0;
	}
	LRESULT OnEditMarkLine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->MarkUnit(tomLine);
		return 0;
	}
	LRESULT OnEditMarkDoc(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->MarkUnit(tomStory);
		return 0;
	}
	LRESULT OnEditSelectWord(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SelectUnit(tomWord);
		return 0;
	}
	LRESULT OnEditSelectSentence(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SelectUnit(tomSentence);
		return 0;
	}
	LRESULT OnEditSelectPara(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SelectUnit(tomParagraph);
		return 0;
	}
	LRESULT OnEditSelectLine(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SelectUnit(tomLine);
		return 0;
	}
	LRESULT OnEditSelectDoc(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		T* pT = static_cast<T*>(this);
		pT->SelectUnit(tomStory);
		return 0;
	}
};

inline LPCTSTR GetRichEditControlClassName(void)
{
	return(_T("richedit50w"));
}
#endif //__BRLEDITCTRL_H_
