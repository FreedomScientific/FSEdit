#pragma once;
#include "resource.h"       // main symbols
#include "brleditdefs.h"
#include "atlmisc.h"
#include "..\ConvertRtfAndBrltxt\ConvertRtfAndBrltxt.h"
#include <list>
#include <algorithm>
#include <functional>
#include <gtutility.h>
#include "ErrorCodes.h"

using namespace std; //_STLP_STD;
#include <winnetwk.h>
#include <memory>
#include "brltab.h"
#include <IBrailleTranslator.h>
typedef int (CALLBACK* EDITWORDBREAKPROC)(LPWSTR lpch, int ichCurrent, int cch, int code);

inline BSTR T2BSTR(LPCTSTR lp, int len) {return ::SysAllocString(lp);}
inline BSTR A2BSTR(LPCSTR lp, int len) {USES_CONVERSION; return A2WBSTR(lp, len);}
inline BSTR W2BSTR(LPCWSTR lp, int len) {return ::SysAllocString(lp);}

class ConObject
{
	BYTE bytChar;
public:
	ConObject(BYTE byt) : bytChar(byt) {}
#ifdef DEBUG
	bool operator()(long x)
	{
		bool b = (bytChar == LOBYTE(x));
		return b;
	}
#else
	bool operator()(long x){return (bytChar == LOBYTE(x));}
#endif
};

struct minus1 : public unary_function<int, bool>
{
	bool operator()(int x)const {return (x == -1);}
};
#ifdef BOOKMARK_SUPPORT
extern "C" inline void DelBookMark(CComPtr<ITextRange> bm)
{
	bm.Release();
}
#endif
#define NUM_EMB_SETTINGS _countof(aEmbSettings)
#define GRADE_2_INS 1
#define TEXT_INS 0

typedef struct _EMB_SETTINGS
{

	BOOL bEmbosserGoesToNewLine;
	BOOL bEmbosserGoesToNewPage;
	BOOL bInterpointMargins;
	BOOL bInterpointPages;
	int iCellsPerLine;
	int iMaxCellsPerLine;
	int iLinesPerPage;
	int iMaxLinesPerPage;
	int iTopMargin;
	int iBottomMargin;
	int iLeftMargin;
	int iRightMargin;
	CString sEmbosserSel;
	CString sConnectionSel;
	CString sNetworkDeviceSel;
	CString sNetworkPortSel;
	BOOL bIsNetworkEmb;
	int iNumCopies;
	BOOL bGrade2Braille;
	BOOL bEmphasizedText;

} EMB_SETTINGS;

const size_t aEmbSettings[] =
{
	// PERMANENT PROPERTIES
	offsetof(EMB_SETTINGS, bEmbosserGoesToNewLine),
	offsetof(EMB_SETTINGS, bEmbosserGoesToNewPage),
	offsetof(EMB_SETTINGS, bInterpointMargins),
	offsetof(EMB_SETTINGS, bInterpointPages),
	offsetof(EMB_SETTINGS, iCellsPerLine),
	offsetof(EMB_SETTINGS, iMaxCellsPerLine),
	offsetof(EMB_SETTINGS, iLinesPerPage),
	offsetof(EMB_SETTINGS, iMaxLinesPerPage),
	offsetof(EMB_SETTINGS, iTopMargin),
	offsetof(EMB_SETTINGS, iBottomMargin),
	offsetof(EMB_SETTINGS, iLeftMargin),
	offsetof(EMB_SETTINGS, iRightMargin),
	offsetof(EMB_SETTINGS, sEmbosserSel),
	offsetof(EMB_SETTINGS, sNetworkDeviceSel),
	offsetof(EMB_SETTINGS, sConnectionSel),
	offsetof(EMB_SETTINGS, sNetworkPortSel),
	offsetof(EMB_SETTINGS, bIsNetworkEmb),
	// TEMP VALUES
	offsetof(EMB_SETTINGS, iNumCopies),
	offsetof(EMB_SETTINGS, bGrade2Braille),
	offsetof(EMB_SETTINGS, bEmphasizedText)
};

struct PrinterInfo
{
	CComBSTR bstrLocal;
	CComBSTR bstrRemote;
	CComBSTR bstrProvider;
};


typedef struct
{
	LPCTSTR lpsz;
	HANDLE handle;
	UINT uiDelay;
} WaitSound;

class CBrlEdit : public CWindowImpl<CBrlEdit, CRichEditCtrl>
{
	ITextDocument* pDoc;

	vector<PrinterInfo> vecPrinterInfo;
	bool EnumeratePrinter(LPNETRESOURCE nr, bool bDisplayErrMsg = false);

	void DisplayErrorMsg();

	void NetworkPrinters(bool bReInit = false);

	static HANDLE hWaitForIt;

	
	

public:
	size_t GetNumOfNetworkPrinters(){return vecPrinterInfo.size();}

protected:
	enum DocType
	{
		DOCTYPE_SKITXT,
		DOCTYPE_BRLTXT,
		DOCTYPE_BRFTXT,
		DOCTYPE_RTF,
		DOCTYPE_TEXT,
DOCTYPE_FILE,	// Temp file.
		DOCTYPE_BRAILLE_UNKNOWN,
		DOCTYPE_UNKNOWN
	};

	bool bNewFile;
public:
	CBrlEdit()
	{
//		m_bWindowOnly = TRUE;
		ClearMark();
#ifndef _WIN32_WCE
		m_szFindWhat[0] = _T('\0');
		m_szReplaceWith[0] = _T('\0');
		m_dwFRFlags = 0;
#endif	//  ndef _WIN32_WCE
		m_szCurrentFile[0] = _T('\0');
		m_hwndNotify = 0;
	}

	static LPCTSTR GetWndClassName()
	{
		return BRLEDIT_CLASS;
	}
	static LPCTSTR GetLibraryName()
	{
		return _T("BRLEDIT.DLL");
	}

	
	int m_nInsertAs;

	BEGIN_MSG_MAP(CBrlEdit)
		MESSAGE_HANDLER(m_RegisteredMsg.nContainsBrlText, OnContainsBrlText)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		// RichEdit messages
//		MESSAGE_HANDLER(EM_EXGETSEL, OnGetSel)
//		MESSAGE_HANDLER(EM_EXSETSEL, OnSetSel)
		// Clipboard messages
		MESSAGE_HANDLER(WM_CLEAR, OnClear)
		MESSAGE_HANDLER(WM_COPY, OnCopy)
		MESSAGE_HANDLER(WM_CUT, OnCut)
		MESSAGE_HANDLER(WM_PASTE, OnPaste)

		// BrlEdit messages
		MESSAGE_HANDLER(BEM_GETMARK, OnGetMark)
		MESSAGE_HANDLER(BEM_SETMARK, OnSetMark)
		MESSAGE_HANDLER(BEM_GETCARETPOS, OnGetCaretPos)
		MESSAGE_HANDLER(BEM_SETCARETPOS, OnSetCaretPos)
		MESSAGE_HANDLER(BEM_SETMARKFROMSEL, OnSetMarkFromSel)
		MESSAGE_HANDLER(BEM_MOVETOMARK, OnMoveToMark)
		MESSAGE_HANDLER(BEM_MARKUNIT, OnMarkUnit)
		MESSAGE_HANDLER(BEM_SELECTUNIT, OnSelectUnit)
		MESSAGE_HANDLER(BEM_OPENFILE,OnOpenFile)
		MESSAGE_HANDLER(BEM_SAVEFILEAS,OnSaveFileAs)
		MESSAGE_HANDLER(BEM_SAVEFILE,OnSaveFile)
		MESSAGE_HANDLER(BEM_FINDREPLACE, FindReplace)
		MESSAGE_HANDLER(BEM_GETGRADE2, OnGetGrade2)
		MESSAGE_HANDLER(BEM_SETGRADE2, OnSetGrade2)
		MESSAGE_HANDLER(BEM_GETPAGEWIDTH, OnGetPageWidth)
		MESSAGE_HANDLER(BEM_SETPAGEWIDTH, OnSetPageWidth)
		MESSAGE_HANDLER(BEM_GETPAGEHEIGHT, OnGetPageHeight)
		MESSAGE_HANDLER(BEM_SETPAGEHEIGHT, OnSetPageHeight)
		MESSAGE_HANDLER(BEM_GETPAGENUMBERING, OnGetPageNumbering)
		MESSAGE_HANDLER(BEM_SETPAGENUMBERING, OnSetPageNumbering)
		MESSAGE_HANDLER(BEM_GETTOPMARGIN, OnGetTopMargin)
		MESSAGE_HANDLER(BEM_SETTOPMARGIN, OnSetTopMargin)
		MESSAGE_HANDLER(BEM_GETBOTTOMMARGIN, OnGetBottomMargin)
		MESSAGE_HANDLER(BEM_SETBOTTOMMARGIN, OnSetBottomMargin)
		MESSAGE_HANDLER(BEM_GETLEFTMARGIN, OnGetLeftMargin)
		MESSAGE_HANDLER(BEM_SETLEFTMARGIN, OnSetLeftMargin)
		MESSAGE_HANDLER(BEM_GETRIGHTMARGIN, OnGetRightMargin)
		MESSAGE_HANDLER(BEM_SETRIGHTMARGIN, OnSetRightMargin)
		MESSAGE_HANDLER(BEM_PRINTDOCUMENT, OnPrintDocument)
		MESSAGE_HANDLER(BEM_INSERTFILEPARAM, OnInsertFileParam)
		MESSAGE_HANDLER(BEM_INSERTFILE, OnInsertFile)
#ifdef BOOKMARK_SUPPORT
		MESSAGE_HANDLER(BEM_SETBOOKMARK, OnSetBookMark)
		MESSAGE_HANDLER(BEM_GETBOOKMARK, OnGetBookMarkData)
		MESSAGE_HANDLER(BEM_DELETEBOOKMARK_BYLINE, OnDeleteBookMarkByMark)
		MESSAGE_HANDLER(BEM_DELETEBOOKMARK_BYINDEX, OnDeleteBookMarkByIndex)
		MESSAGE_HANDLER(BEM_ISVALIDBOOKMARK,OnIsValidBookMark)
		MESSAGE_HANDLER(BEM_GETBOOKMARKCOUNT, OnGetBookMarkCount)
		MESSAGE_HANDLER(BEM_RESETBOOKMARK, OnResetBookMark)
		MESSAGE_HANDLER(BEM_GETBOOKMARKID, OnGetBookMarkID)
		MESSAGE_HANDLER(BEM_UPDATEBOOKMARK, OnUpdateBookMark)
#endif
		MESSAGE_HANDLER(BEM_INITPRINTERS, OnInitNetworkPrinters)
		MESSAGE_HANDLER(BEM_SELECTTOMARK, OnSelectToMark)
		MESSAGE_HANDLER(BEM_SETNOTIFY, OnSetNotify)
		MESSAGE_HANDLER(BEM_GETBRLTRANS, OnGetBrlTrans)
	END_MSG_MAP()



	LRESULT OnInitNetworkPrinters(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		NetworkPrinters(lParam == 1);
		return 0;
	}

	LRESULT OnContainsBrlText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.brailleGrade2;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL& bHandled)
	{
		// Release the document object before unloading the riched20 library
		m_pDoc.Release();


		bHandled = FALSE;
		return 0;
	}

	// Clipboard support
	LRESULT OnClear(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		DeleteBlock();
		return 0;
	}
	LRESULT OnCopy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CopyBlock();
		return 0;
	}
	LRESULT OnCut(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CutBlock();
		return 0;
	}
	LRESULT OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		Paste();
		return 0;
	}

	// BrlEdit messages
#ifdef BOOKMARK_SUPPORT
	LRESULT OnSetBookMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		long* n = find(m_docProperties.bookmark, m_docProperties.bookmark+NUMOFBOOKMARKS, -1);
		int nIndex = n - m_docProperties.bookmark;
		if(n != m_docProperties.bookmark+NUMOFBOOKMARKS)
			*n = BOOKMARK(nIndex+1, 0, LOWORD(lParam));
		else
			return -1;

		return nIndex;
	}

	LRESULT OnGetBookMarkData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.bookmark[(int)wParam];
	}

	LRESULT OnDeleteBookMarkByMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if((int)lParam < 0 || (int)lParam > 25)
			return -1;

		long* n = find_if(m_docProperties.bookmark, m_docProperties.bookmark+NUMOFBOOKMARKS, ConObject(lParam));
		if(n == m_docProperties.bookmark+NUMOFBOOKMARKS)
			return -1;

		*n = -1;
		return 0;
	}

	LRESULT OnDeleteBookMarkByIndex(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(0 <= (int)wParam && (int)wParam < NUMOFBOOKMARKS)
		{
			m_docProperties.bookmark[(int)wParam] = -1;
			return 0;
		}

		return -1;
	}

	LRESULT OnIsValidBookMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if((int)wParam < 0 || (int)wParam > 25)
			return -1;

		return (m_docProperties.bookmark[(int)wParam] != -1) ? 0 : -1;
	}

	LRESULT OnGetBookMarkCount(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return count_if(m_docProperties.bookmark, m_docProperties.bookmark+NUMOFBOOKMARKS, not1(minus1()));
	}

	LRESULT OnResetBookMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		fill(m_docProperties.bookmark, m_docProperties.bookmark+NUMOFBOOKMARKS, -1);
		return 0;
	}

	LRESULT OnGetBookMarkID(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return LOBYTE(m_docProperties.bookmark[(int)lParam]);
	}

	LRESULT OnUpdateBookMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.bookmark[(int)wParam] = (DWORD)lParam;
		return 0;
	}
#endif

	LRESULT OnGetMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return GetMark();
	}

	LRESULT OnSetMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetMark(static_cast<int>(lParam));
		return 0;
	}
	LRESULT OnSetMarkFromSel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetMarkFromSelection();
		return 0;
	}
	LRESULT OnSelectToMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SelFromCursorToMark();
		return 0;
	}
	LRESULT OnMoveToMark(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MoveSelToMark();
		return 0;
	}
	LRESULT OnMarkUnit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		MarkUnit(static_cast<int>(lParam));
		return 0;
	}
	LRESULT OnSelectUnit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SelectUnit(static_cast<int>(lParam));
		return 0;
	}
	LRESULT OnGetCaretPos(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.caretPosition;
	}

	LRESULT OnSetCaretPos(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.caretPosition = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetGrade2(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.brailleGrade2;
	}

	LRESULT OnSetGrade2(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.brailleGrade2 = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetPageHeight(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.pageHeight;
	}

	LRESULT OnSetPageHeight(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.pageHeight = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetPageWidth(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.pageWidth;
	}

	LRESULT OnSetPageWidth(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.pageWidth = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetPageNumbering(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.pageNumbering;
	}

	LRESULT OnSetPageNumbering(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.pageNumbering = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetTopMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.topMargin;
	}

	LRESULT OnSetTopMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.topMargin = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetBottomMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.bottomMargin;
	}

	LRESULT OnSetBottomMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.bottomMargin = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetLeftMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.leftMargin;
	}

	LRESULT OnSetLeftMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.leftMargin = static_cast<int>(lParam);
		return 0;
	}
	LRESULT OnGetRightMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_docProperties.rightMargin;
	}

	LRESULT OnSetRightMargin(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_docProperties.rightMargin = static_cast<int>(lParam);
		return 0;
	}

	LRESULT OnPrintDocument(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return PrintDocument();;
	}

	LRESULT OnOpenFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return InsertFile((LPCTSTR)lParam);
	}

	LRESULT OnInsertFileParam(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_nInsertAs = static_cast<int>(wParam);
		return 1;
	}

	LRESULT OnInsertFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(InsertFileIntoFile((LPCTSTR)wParam, (int)lParam))
			return 1;
		else
			return 0;
	}


	LRESULT OnSaveFileAs(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		/*
		When doing a Save As, translations are done to the text and formatting
		and then the final result is saved in the destination file.  To make sure the
		user will be viewing the correct text and formatting we then Insert ( or Open)
		the destination file. .
		*/
		bNewFile = (LONG)wParam == 1;
		if (SaveFile((LPCTSTR)lParam))
		{
			InsertFile((LPCTSTR)lParam);
			return 1;
		}
		else
			return 0;
	}

	LRESULT OnSaveFile(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bNewFile = (LONG)wParam == 1;
		if (SaveFile((LPCTSTR)lParam))
			return 1;
		else
			return 0;
	}


	LRESULT OnSetNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_hwndNotify = (HWND)lParam;
		return 0;
	}
	LRESULT OnGetBrlTrans(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if (lParam && m_qipBrlTrans)
		{
			m_qipBrlTrans.CopyTo((IBrailleTranslator**)lParam);
			return 1;
		}
		return 0;
	}
	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		delete this;
	}

	CComQIPtr<ITextDocument> m_pDoc;
	// Specifies the beginning of a marked block of text
	// the end of the block of text is determined by the position of the insertion point
	// = -1 means that it has not been set

	static BrlTxtDocProperties m_docProperties;

	_TCHAR m_szCurrentFile[_MAX_PATH];
	void SetMark(long lMark) { m_docProperties.markPosition = lMark; }
	long GetMark() { return m_docProperties.markPosition; }
	HRESULT SetMarkFromSelection()
	{
		CComPtr<ITextSelection> pSel;
		if (!m_pDoc ||
			FAILED(m_pDoc->GetSelection(&pSel)) ||
			!pSel)
			return E_NOINTERFACE;
		return pSel->GetStart(&m_docProperties.markPosition);
	}
	BOOL IsMarkSet() { return (m_docProperties.markPosition >= 0); }
	BOOL IsMarkInRange(ITextRange* pRange)
	{
		ATLASSERT(pRange != NULL);
		if (!pRange)
			return FALSE;
		if (!IsMarkSet())
			return FALSE;
		long lStart,lEnd;
		if (FAILED(pRange->GetStart(&lStart)) ||
			FAILED(pRange->GetEnd(&lEnd)))
			return FALSE;
		if (lStart == lEnd)
			// the range is collapsed
			return (m_docProperties.markPosition == lStart);
		return (lStart <= m_docProperties.markPosition && m_docProperties.markPosition < lEnd);
	}
	void ClearMark() { m_docProperties.markPosition = -1; }

	HRESULT SelFromCursorToMark()
	{
		CHARRANGE cr;
		long lMark;
		if (!IsMarkSet())
			return E_FAIL;
		lMark = GetMark();
		GetSel(cr);
		if(lMark != -1 && cr.cpMax != lMark)
		{
			SetSel(cr.cpMax,lMark); //min(cr.cpMax, lMark), max(cr.cpMax, lMark));
			NotifyWindow(m_RegisteredMsg.nSelChanged);
		}
		return S_OK;
	}

	HRESULT MoveSelToMark()
	{
		if (!IsMarkSet())
			return E_FAIL;
		CComPtr<ITextRange> pRange;
		if (m_pDoc && SUCCEEDED(m_pDoc->Range(m_docProperties.markPosition,m_docProperties.markPosition,&pRange)) && pRange)
		{
			CHARRANGE cr;
			GetSel(cr);
			HRESULT hr = pRange->Select();
			SetMark(cr.cpMax);
			return hr;
		}

		return E_POINTER;
	}

	HRESULT GetBlock(ITextRange** ppRange)
	{
		CHARRANGE cr;
		GetSel(cr);
		if(!(cr.cpMax == cr.cpMin))
			return m_pDoc->Range(cr.cpMin, cr.cpMax, ppRange);

		if (!IsMarkSet())
			return E_FAIL;
		CComPtr<ITextSelection> pSel;
		long lSelStart;
		if (m_pDoc && SUCCEEDED(m_pDoc->GetSelection(&pSel)) && pSel &&
			SUCCEEDED(pSel->GetStart(&lSelStart)))
			return m_pDoc->Range(m_docProperties.markPosition,lSelStart,ppRange);
		return E_POINTER;
	}
	HRESULT CutBlock()
	{
		// ITextRange::Cut() fails in CE. So we first get the text in the marked
		// block, copy the text to the clipboard, and delete the range.
		CComPtr<ITextRange> pBlock;
		if (FAILED(GetBlock(&pBlock)) || !pBlock)
			return E_FAIL;
		// ITextRange::GetText fails in WCE, always returns E_OUTOFMEMORY (0x8007000EL)
		// So RichEdit message EM_GETTEXTRANGE is used instead.
		TEXTRANGE tr;
		if (FAILED(pBlock->GetStart(&tr.chrg.cpMin)) ||
			FAILED(pBlock->GetEnd(&tr.chrg.cpMax)) ||
			tr.chrg.cpMin >= tr.chrg.cpMax)
			return E_FAIL;
		int nSize = (tr.chrg.cpMax-tr.chrg.cpMin+1)*sizeof(TCHAR);
		tr.lpstrText = (LPTSTR) _alloca(nSize);
		if (!tr.lpstrText)
			return E_FAIL;
		if (::SendMessage(m_hWnd,EM_GETTEXTRANGE,0,(LPARAM)&tr) <= 0)
			return E_FAIL;
		if (!OpenClipboard())
			return E_FAIL;
		BOOL bResult = FALSE;
		if (EmptyClipboard())
		{
			HGLOBAL ClipData = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, nSize);
			if(ClipData)
			{
				void*   P = (void*)GlobalLock(ClipData);
//#ifdef _UNICODE
//				WideCharToMultiByte(GetACP(),0,tr.lpstrText,-1,(LPSTR) P,nSize,NULL,NULL);
//#else
				memcpy(P, tr.lpstrText, nSize);
//#endif
				GlobalUnlock(ClipData);
				if(SetClipboardData(CF_UNICODETEXT, ClipData))
					bResult = TRUE;
				else
					GlobalFree(ClipData);
			}
			CloseClipboard();
		}
		if (bResult)
		{
			ClearMark();
			pBlock->Delete(tomCharacter,0,NULL);
		}
		return bResult ? S_OK : E_FAIL;
	}
	HRESULT CopyBlock()
	{
		// ITextRange::Copy() fails in CE. So we get the text in the marked block,
		// and copy the text to the clipboard.
		CComPtr<ITextRange> pBlock;
		if (FAILED(GetBlock(&pBlock)) || !pBlock)
			return E_FAIL;
		// ITextRange::GetText fails in WCE, always returns E_OUTOFMEMORY (0x8007000EL)
		// So RichEdit message EM_GETTEXTRANGE is used instead.
		TEXTRANGE tr;
		if (FAILED(pBlock->GetStart(&tr.chrg.cpMin)) ||
			FAILED(pBlock->GetEnd(&tr.chrg.cpMax)) ||
			tr.chrg.cpMin >= tr.chrg.cpMax)
			return E_FAIL;
		int nSize = (tr.chrg.cpMax-tr.chrg.cpMin+1)*sizeof(TCHAR);
		tr.lpstrText = (LPTSTR) malloc(nSize);
		if (!tr.lpstrText)
			return E_FAIL;
		if (::SendMessage(m_hWnd,EM_GETTEXTRANGE,0,(LPARAM)&tr) <= 0)
		{
			free (tr.lpstrText);
			return E_FAIL;
		}
		if (!OpenClipboard())
		{
			free (tr.lpstrText);
			return E_FAIL;
		}
		BOOL bResult = FALSE;
		if (EmptyClipboard())
		{
			HGLOBAL ClipData = GlobalAlloc( GMEM_MOVEABLE|GMEM_DDESHARE, nSize);
			if(ClipData)
			{
				void*   P = (void*)GlobalLock(ClipData);
//#ifdef _UNICODE
//				WideCharToMultiByte(GetACP(),0,tr.lpstrText,-1,(LPSTR) P,nSize,NULL,NULL);
//#else
				memcpy(P, tr.lpstrText, nSize);
//#endif
				GlobalUnlock(ClipData);
				if(SetClipboardData(CF_UNICODETEXT, ClipData))
					bResult = TRUE;
				else
					GlobalFree(ClipData);
			}
			CloseClipboard();
		}
		if (bResult)
		{
			ClearMark();
		}
		free (tr.lpstrText);
		return bResult ? S_OK : E_FAIL;
	}
	HRESULT Paste()
	{
		if (!OpenClipboard())
			return E_FAIL;
		HGLOBAL ClipData = GetClipboardData(CF_UNICODETEXT);
		if (ClipData)
		{
			LPTSTR lpszData = (LPTSTR) GlobalLock(ClipData);
			CComBSTR bstr(lpszData);

			ReplaceSel(bstr.m_str);
			GlobalUnlock(ClipData);
		}

		CloseClipboard();
		SetModify(TRUE);

		return 0;
	}

	HRESULT DeleteBlock()
	{
		CComPtr<ITextRange> pBlock;
		if (SUCCEEDED(GetBlock(&pBlock)) && pBlock)
		{
			HRESULT hres = pBlock->Delete(tomCharacter,0,NULL);
			if (SUCCEEDED(hres))
				ClearMark();
			return hres;
		}
		return E_POINTER;
	}
	HRESULT MarkUnit(long unit)
	{
		CComPtr<ITextRange> pBlock;
		long lStart,lEnd;
		HRESULT hres;
		if (FAILED(hres = GetBlock(&pBlock)) || !pBlock)
		{
			CComPtr<ITextSelection> pSel;
			if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
				pBlock = pSel;
		}
		if (SUCCEEDED(hres) && pBlock)
		{
			hres = pBlock->Expand(unit,NULL);
			if (FAILED(hres))
				return hres;
			pBlock->GetStart(&lStart);
			pBlock->GetEnd(&lEnd);
			SetMark(lStart);
			CComPtr<ITextRange> pRange;
			m_pDoc->Range(lEnd,lEnd,&pRange);
			pRange->Select();
		}
		return hres;
	}
	HRESULT SelectUnit(long unit)
	{
		CComPtr<ITextRange> pBlock;
		long lStart,lEnd;
		HRESULT hres = E_FAIL;
		CComPtr<ITextSelection> pSel;
		if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
			pBlock = pSel;
		if (SUCCEEDED(hres) && pBlock)
		{
			if((unit == tomSentence) && (m_docProperties.brailleGrade2))
			{
				SetBrlSentenceRange(pBlock, unit);
			}
			else
			{
				hres = pBlock->Expand(unit, NULL);
				if (FAILED(hres))
					return hres;
			}

			pBlock->GetStart(&lStart);
			pBlock->GetEnd(&lEnd);
			SetSel(lStart, lEnd);
			NotifyWindow(m_RegisteredMsg.nSelChanged);
		}
		return hres;
	}

	/*
	Function: SetBrlSentenceRange
	Purpose: When searching for the end of a sentence, ITextRange default behavior
	is to search for a string of text that ends with a period, question mark, or
	exclamation mark and is followed either by one or more ASCII white space
	characters (9 through 0xd and 0x20), or the Unicode paragraph separator (0x2029).
	The trailing white space is part of the sentence.  The last sentence in a story
	does not need to have a period, question mark, or exclamation mark. The start of
	a story qualifies as the start of a tomSentence, even if the string there does
	not qualify as a sentence grammatically. Other sentences must follow a sentence
	end and cannot begin with a period, question mark, or exclamation mark.

	If the document is in Grade 2 Braille, the punctuation marks are 4 (low d),
	6 (low f), and 8 (low h), respectively.  Need to search for those characters
	instead of the computer Braille equivalent.
	*/
	HRESULT SetBrlSentenceRange(ITextRange *pBlock, long unit)
	{
		// Only find set a new range if trying to select a sentence and the
		// document is in Grade 2 Braille.
		if((unit == tomSentence) && (m_docProperties.brailleGrade2))
		{
			long lStart, lEnd, lCurrent, lChar;
			long lLine, lLineStart;
			int nLineLen, nSize, nIdx;
			LPTSTR lpszLine = 0;
			BOOL bStart, bEndPunc, bFound;
			CHARRANGE cr;

			// If there is user-selected text, we must select the all sentences that
			// are part of the pre-selected area.
			GetSel(cr);
			lStart = cr.cpMin;
			lEnd = cr.cpMax;

			/***  Find start of sentence. ***/
			bStart = TRUE;
			bEndPunc = FALSE;
			bFound = FALSE;
			lCurrent = lStart;
			lChar = 0;
			lLine = LineFromChar(lCurrent);
			while(!bFound && (lLine >= 0))
			{
				lLineStart = LineIndex(lLine);
				nLineLen = LineLength(lLineStart) + 1; // Add 1 for the terminating NULL.
				if (nLineLen == 1)
				{
					lLine--;
					bStart = FALSE;
					continue;
				}
				if (lpszLine)
				{
					delete [] lpszLine;
					lpszLine = 0;
				}
				nSize = nLineLen * sizeof(_TCHAR) + 1;
				lpszLine = new _TCHAR[nSize];
				memset(lpszLine, 0, nSize);
				GetLine(lLine, lpszLine, nLineLen); // Get the current line's text.
				lpszLine[nLineLen] = _T('\0');

				// If first time in loop, assign nIdx to the current index
				// that is relative to the line.  Else, restart search index
				// at the end of the line.
				if (bStart)
				{
					nIdx = lCurrent - lLineStart;
					bStart = FALSE;
				}
				else
				{
					nIdx = nLineLen - 1;
				}
				// Search backwards in the line of text for the start of a sentence.
				while (!bFound && (nIdx >= 0))
				{
					switch (lpszLine[nIdx])
					{
					case _T('4'): // period
					case _T('6'): // exclamation mark
					case _T('8'): // question mark
						// Check if we are at the end of a line.
						if ((nIdx+1) == nLineLen)
						{
							// If we started at the end of a line, look for the
							// next punctuation to indicate the start of a sentence.
							// Else, we have found the start of a sentence.
							if (lCurrent == (lLineStart + nIdx))
							{
								nIdx--;
								bEndPunc = TRUE;
								continue;
							}
							else
							{
								// We have found the end of the previous sentence.
								bFound = TRUE;
								continue;
							}
						}
						// Check for white space.  If this is the end of a sentence
						// and lChar = 0, then we are at the end of a sentence we
						// want as part of our selection.
						if ( (lpszLine[nIdx+1] == _T(' '))	||
							 (lpszLine[nIdx+1] == 0x0d)		||
							 (lpszLine[nIdx+1] == _T('\t')) )
						{
							if (lChar == 0)
							{
								bEndPunc = TRUE;
							}
							else
							{
								// Assign lStart to the last time we found a character.
								lStart = lChar;
								bFound = TRUE;
							}
						}
						break;

					case _T(' '):
					case 0x0d:
					case _T('\t'):
						// Ignore white space.
						break;

					default:
						// Keep track of where the last character was.
						lChar = lLineStart + nIdx;
						break;
					}
					nIdx--;
				}

				// While the start has not been found, assign lStart to the
				// beginning of the last line we checked.
				if (!bFound)
				{
					lStart = lLineStart;
				}
				lLine--;
			}

			/*** Find end of sentence. ***/
			bStart = TRUE;
			bFound = FALSE;
			lCurrent = lEnd;
			lLine = LineFromChar(cr.cpMin);
			while(!bFound && (lLine < GetLineCount()))
			{
				lLineStart = LineIndex(lLine);
				nLineLen = LineLength(lLineStart) + 1; // Add 1 for the terminating NULL.
				if (nLineLen == 1)
				{
					// If we reach an empty line, set the end point here because
					// we also want to select the white space before the next sentence.
					lEnd = lLineStart + 1;
					lLine++;
					bStart = FALSE;
					continue;
				}
				if (lpszLine)
				{
					delete [] lpszLine;
					lpszLine = 0;
				}
				nSize = nLineLen * sizeof(_TCHAR) + 1;
				lpszLine = new _TCHAR[nSize];
				memset(lpszLine, 0, nSize);
				GetLine(lLine, lpszLine, nLineLen); // Get the current line's text.
				lpszLine[nLineLen] = _T('\0');

				// If first time in loop, assign nIdx to the current start
				// index that is relative to the line.  Else, restart search index
				// at the beginning of the line.
				if (bStart)
				{
					nIdx = cr.cpMin - lLineStart;
					bStart = FALSE;
				}
				else
				{
					nIdx = 0;
				}
				// Search forward in the line of text for the end of a sentence.
				while (!bFound && (nIdx < nLineLen))
				{
					switch (lpszLine[nIdx])
					{
					case _T('4'): // period
					case _T('6'): // exclamation mark
					case _T('8'): // question mark
						// Check if we are at the end of a line.  If we are, then this is the
						// end of the current sentence.  However, we can not set this as the
						// end point because we want all the white space until the next sentence.
						if ((nIdx+1) == nLineLen)
						{
							bEndPunc = TRUE;
							nIdx++;
							continue;
						}

						// Check for white space.
						if ( (lpszLine[nIdx+1] == _T(' '))	||
							 (lpszLine[nIdx+1] == 0x0d)		||
							 (lpszLine[nIdx+1] == _T('\t')) )
						{
							bEndPunc = TRUE;
						}
						break;

					case _T(' '):
					case 0x0d:
					case _T('\t'):
						// Ignore white space.
						break;

					default:
						// If we are inside user-selected text, ignore this character
						// and clear bEndPunc just in case it was set to TRUE.
						if (lCurrent > (lLineStart + nIdx))
						{
							bEndPunc = FALSE;
							nIdx++;
							continue;
						}
						// If we have already found the end of the sentence, set
						// the end point to right before this character.
						else if (bEndPunc)
						{
							lEnd = lLineStart + nIdx;
							bFound = TRUE;
						}
						break;
					}
					nIdx++;
				}

				// While the end has not been found, assign lEnd to the
				// end of the last line we checked.
				if (!bFound)
				{
					lEnd = lLineStart + nLineLen;
				}
				lLine++;
			}

			if (lpszLine)
			{
				delete [] lpszLine;
				lpszLine = 0;
			}
			pBlock->SetRange(lStart, lEnd);
			return S_OK;
		}
		return S_OK;
	}

	HRESULT ToggleCharAttributes(DWORD dwAttribs)
	{
		CComPtr<ITextRange> pBlock;
		HRESULT hres;
		if (FAILED(hres = GetBlock(&pBlock)) || !pBlock)
		{
			CComPtr<ITextSelection> pSel;
			if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
				pBlock = pSel;
		}
		if (SUCCEEDED(hres) && pBlock)
		{
			CComPtr<ITextFont> pFont;
			if (FAILED(hres = pBlock->GetFont(&pFont)) || !pFont)
				return hres;
			if (dwAttribs & CFE_BOLD)
				pFont->SetBold(tomToggle);
			if (dwAttribs & CFE_ITALIC)
				pFont->SetItalic(tomToggle);
			if (dwAttribs & CFE_UNDERLINE)
				pFont->SetUnderline(tomToggle);
			if (dwAttribs & CFE_STRIKEOUT)
				pFont->SetStrikeThrough(tomToggle);
		}

		return hres;
	}
	HRESULT SetParaAlignment(long lAlignment)
	{
		CComPtr<ITextRange> pBlock;
		HRESULT hres;
		if (FAILED(hres = GetBlock(&pBlock)) || !pBlock)
		{
			CComPtr<ITextSelection> pSel;
			if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
				pBlock = pSel;
		}
		if (SUCCEEDED(hres) && pBlock)
		{
			CComPtr<ITextPara> pPara;
			if (FAILED(hres = pBlock->GetPara(&pPara)) || !pPara)
				return hres;
			hres = pPara->SetAlignment(lAlignment);
		}
		return hres;
	}
	HRESULT SetParaNumbering(long lNumbering)
	{
		CComPtr<ITextRange> pBlock;
		HRESULT hres;
		if (FAILED(hres = GetBlock(&pBlock)) || !pBlock)
		{
			CComPtr<ITextSelection> pSel;
			if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
				pBlock = pSel;
		}
		if (SUCCEEDED(hres) && pBlock)
		{
			CComPtr<ITextPara> pPara;
			if (FAILED(hres = pBlock->GetPara(&pPara)) || !pPara)
				return hres;
			pPara->SetListStart(1);
			hres = pPara->SetListType(lNumbering);
		}
		return hres;
	}
	HRESULT IncreaseIndents()
	{
		CComPtr<ITextRange> pBlock;
		HRESULT hres;
		if (FAILED(hres = GetBlock(&pBlock)) || !pBlock)
		{
			CComPtr<ITextSelection> pSel;
			if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
				pBlock = pSel;
		}
		if (SUCCEEDED(hres) && pBlock)
		{
			CComPtr<ITextPara> pPara;
			if (FAILED(hres = pBlock->GetPara(&pPara)) || !pPara)
				return hres;
			float fLeftIndent, fRightIndent, fFirstLineIndent, fDefTabWidth;
			m_pDoc->GetDefaultTabStop(&fDefTabWidth);
			pPara->GetLeftIndent(&fLeftIndent);
			pPara->GetRightIndent(&fRightIndent);
			pPara->GetFirstLineIndent(&fFirstLineIndent);
			fLeftIndent += fDefTabWidth;
			hres = pPara->SetIndents(fFirstLineIndent,fLeftIndent,fRightIndent);
		}
		return hres;
	}
	HRESULT DecreaseIndents()
	{
		CComPtr<ITextRange> pBlock;
		HRESULT hres;
		if (FAILED(hres = GetBlock(&pBlock)) || !pBlock)
		{
			CComPtr<ITextSelection> pSel;
			if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
				pBlock = pSel;
		}
		if (SUCCEEDED(hres) && pBlock)
		{
			CComPtr<ITextPara> pPara;
			if (FAILED(hres = pBlock->GetPara(&pPara)) || !pPara)
				return hres;
			float fLeftIndent, fRightIndent, fFirstLineIndent, fDefTabWidth;
			m_pDoc->GetDefaultTabStop(&fDefTabWidth);
			pPara->GetLeftIndent(&fLeftIndent);
			pPara->GetRightIndent(&fRightIndent);
			pPara->GetFirstLineIndent(&fFirstLineIndent);
			fLeftIndent -= fDefTabWidth;
			hres = pPara->SetIndents(fFirstLineIndent,fLeftIndent,fRightIndent);
		}
		return hres;
	}
#ifndef _WIN32_WCE
	// Used to remember the last state of the Find/Replace dialog.
	TCHAR m_szFindWhat[128];
	TCHAR m_szReplaceWith[128];
	DWORD m_dwFRFlags;
	HRESULT FindReplaceText(BOOL bFindDialogOnly)
	{
		CFindReplaceDialog* pDlg = new CFindReplaceDialog;
		if (!pDlg)
			return E_OUTOFMEMORY;
		HWND hwnd = pDlg->Create(bFindDialogOnly,
			m_szFindWhat,
			m_szReplaceWith,
			m_dwFRFlags,m_hWnd);
		::ShowWindow(hwnd, SW_SHOWNORMAL);
		::UpdateWindow(hwnd);
#if 0
		static UINT nFindReplaceMsg = ::RegisterWindowMessage(FINDMSGSTRING);
		FINDREPLACE fr;
		::ZeroMemory(&fr,sizeof(FINDREPLACE));
		fr.lStructSize = sizeof(FINDREPLACE);
		fr.hwndOwner = m_hWnd;
		TCHAR szFindWhat[128];
		TCHAR szReplaceWith[128];
		fr.lpstrFindWhat = szFindWhat;
		fr.lpstrReplaceWith = szReplaceWith;
		fr.wFindWhatLen = sizeof(szFindWhat);
		fr.wReplaceWithLen = sizeof(szReplaceWith);
		HWND hDlg = ::FindText(&fr);
		if (!hDlg)
		{
			DWORD dwErr = CommDlgExtendedError();
		}
		::ShowWindow(hDlg, SW_SHOWNORMAL);
		::UpdateWindow(hDlg);
#endif
		return S_OK;
	}
#endif


	LRESULT FindReplace(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetMark(static_cast<int>(lParam));
		return 0;
	}

	HRESULT OpenSaveAsDialog(BOOL bOpenFileDialog)
	{
		ATLASSERT(m_pDoc != NULL);
		if (!m_pDoc)
			return E_POINTER;
		HRESULT hres = S_OK;
		CFileDialog dlg(bOpenFileDialog,0,m_szCurrentFile,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,0,m_hWnd);
		BOOL bRet = dlg.DoModal() != 0;
		if (bRet == IDOK)
		{
			if (bOpenFileDialog)
				InsertFile(dlg.m_ofn.lpstrFile);
			else
				SaveFile(dlg.m_ofn.lpstrFile);
		}
		return hres;
	}
	HRESULT Save()
	{
			if (m_szCurrentFile[0]&&
				SaveFile(m_szCurrentFile))
				return S_OK;
			else
				return OpenSaveAsDialog(FALSE);
	}
	HRESULT New()
	{
		if (Save() == S_OK)
		{
			m_pDoc->New();
			// clear out old filename and force the new one to be named
			m_szCurrentFile[0] = _T('\0');
			Save();
		}
		return S_OK;
	}

	BOOL InsertFile(LPCTSTR lpszFilePath, int nPos = -1)
	{
		bool isUnicode = false;
		_ASSERTE(lpszFilePath);
		HANDLE hFile = ::CreateFile(lpszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == NULL || hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		// Make sure that the limit text value is large enough for this file.
		auto uLimit = ::SendMessage(m_hWnd,EM_GETLIMITTEXT,0,0);
		DWORD dwHighFileSize;
		DWORD nFileSize = ::GetFileSize(hFile,&dwHighFileSize);
		
		if (uLimit < nFileSize + (1024 * 4))
			::SendMessage(m_hWnd,EM_EXLIMITTEXT,0,nFileSize + (1024 * 4));
		
		EDITSTREAM es;
		es.dwError = 0;
		int nFormat = 0;

		if(nPos < 0)	// New document
			m_docProperties.Reset();
		else
			SetSel(LOWORD(nPos), HIWORD(nPos));

		UINT bSuccess = 1;
		switch(FileType(lpszFilePath))
		{
		case DOCTYPE_FILE:
			es.pfnCallback = StreamReadCallback;
			es.dwCookie = (DWORD_PTR)hFile;
			nFormat = SF_TEXT | SFF_SELECTION;
			StreamIn(nFormat, es);
			break;
		case DOCTYPE_BRLTXT:
			bSuccess = 2;
		case DOCTYPE_BRFTXT:
		{
			if(nPos < 0)
			{
				m_docProperties.brailleGrade2 = 1;
				NotifyBrailleGradeChng( m_docProperties.brailleGrade2 );
			}
			WORD wLeadin;
			DWORD dwNumRead;
			if(ReadFile(hFile, &wLeadin, 2, &dwNumRead, 0)
				&& (dwNumRead >= 2))
			{
				if (wLeadin == 0xfeff)
				{
					es.pfnCallback = StreamReadUnicodeCallback;
					isUnicode = true;
				}
				else
				{
					SetFilePointer(hFile, -2, NULL, FILE_CURRENT);
					es.pfnCallback = StreamReadCallbackBrl;
				}
			}
			else
				es.pfnCallback = StreamReadCallbackBrl;

			es.dwCookie = (DWORD_PTR)hFile;
			nFormat = (nPos < 0) ? SF_TEXT : SF_TEXT | SFF_SELECTION;
			if (isUnicode || es.pfnCallback == StreamReadCallbackBrl)
				nFormat |= SF_UNICODE;
			StreamIn(nFormat, es);
		}
			break;
		case DOCTYPE_BRAILLE_UNKNOWN:
		case DOCTYPE_TEXT:
		case DOCTYPE_UNKNOWN:
			if(nPos < 0)
			{
				m_docProperties.brailleGrade2 = 0;
				NotifyBrailleGradeChng( m_docProperties.brailleGrade2 );
			}

			{
				WORD wLeadin;
				DWORD dwNumRead = 0;
				if(ReadFile(hFile, &wLeadin, 2, &dwNumRead, 0))
				{
					if (wLeadin == 0xfeff)
					{
						es.pfnCallback = StreamReadUnicodeCallback;
						isUnicode = true;
					}
					else
					{
						SetFilePointer(hFile, -2, NULL, FILE_CURRENT);
						es.pfnCallback = StreamReadCallback;
					}
				}
				else
					es.pfnCallback = StreamReadCallback;
			}
			es.dwCookie = (DWORD_PTR)hFile;
			nFormat = (nPos < 0) ? SF_TEXT : SF_TEXT | SFF_SELECTION;
			if (isUnicode)
				nFormat |= SF_UNICODE;
			StreamIn(nFormat, es);

			break;
				case DOCTYPE_SKITXT:
		case DOCTYPE_RTF:
		default:
		{
			es.dwCookie = (DWORD_PTR)hFile;
			es.pfnCallback = StreamReadCallbackRTF;
			nFormat = (nPos < 0) ? SF_RTF : SF_RTF | SFF_SELECTION;
			StreamIn(nFormat, es);

			if (nPos < 0)
			{
				CloseHandle(hFile);
				hFile = 0;
				ImportDocPropertiesFromFile(lpszFilePath, m_docProperties);
				if (FileType(lpszFilePath) != DOCTYPE_SKITXT)
				{
					m_docProperties.brailleGrade2 = 0;
				}
				NotifyBrailleGradeChng(m_docProperties.brailleGrade2);
			}

		}
			break;
		}
		if (hFile != 0)
		::CloseHandle(hFile);

		if(bSuccess == 1 && (nPos < 0))
		{
			SHAddToRecentDocs(SHARD_PATH,lpszFilePath);
			_tcscpy_s(m_szCurrentFile,lpszFilePath);
		}

		return bSuccess;
	}

	BOOL InsertFileIntoFile(LPCTSTR lpszFilePath, int nPos = -1 )
	{
		CString strTempBuff1, strTempBuff2, strTempBuff3, strTempBuff4, strTempBuff5;
		HANDLE hFile;


		_ASSERTE(lpszFilePath);
		hFile = ::CreateFile(lpszFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == NULL || hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		EDITSTREAM es;
		es.dwError = 0;
		int nFormat = 0;

		SetSel(LOWORD(nPos), HIWORD(nPos));

		BOOL bSuccess = TRUE;
		switch(FileType(lpszFilePath))
		{
/*
BRLTXT is a file containing G2 Braille with BRL formatting codes. ( extention = .brl )
When Inserting BRLTXT as Text:
(1 and 2 = Execute BRL formatting codes to make BRF)
1. Convert BRL formatting Codes to RTF Codes.
2. Convert RTF Codes to TXT formatting Codes.
   See: When Inserting BRFTXT as Text:
3. Do Back translation (BackTranslateG2BrailleToText)
4. Stream in as Text.

When Inserting BRLTXT as Grade 2:
1. Convert BRL formatting Codes to RTF Codes.
2. Stream in as RTF.
*/
	case DOCTYPE_BRLTXT:
			{
				CopyFileToString( hFile, strTempBuff1 );
				// 1. Convert BRL formatting Codes to RTF Codes.
				BRLTXTtoRTF(strTempBuff1, strTempBuff2);
				if ( m_nInsertAs == TEXT_INS )
				{
					// 2. Convert RTF Codes to TXT formatting Codes.
					RTFtoTXT( strTempBuff2, strTempBuff3 );

					// 3. Do Back translation (BackTranslateG2BrailleToText)
					BackTranslateG2BrailleToText( strTempBuff3, strTempBuff4 );
					// Prepare for: 4. Stream in as Text.
					nFormat = SF_TEXT | SFF_SELECTION;
					es.dwCookie = (DWORD_PTR)&strTempBuff4;
				}
				else
				{
					// Prepare for: 2. Stream in as RTF.
					nFormat = SF_RTF | SFF_SELECTION;
					es.dwCookie = (DWORD_PTR)&strTempBuff2;
				}
				es.pfnCallback = StreamReadCStringCallback;
				StreamIn(nFormat| SF_UNICODE, es);
			}
			break;
/*
BRFTXT is a file containing G2 Braille with no formatting codes. ( extention = .brf )
When Inserting BRFTXT as Text:
1. Do Back translation (BackTranslateG2BrailleToText)
2. Stream in as Text.

When Inserting BRLTXT as Grade 2:
1. Stream in as Text.
*/
		case DOCTYPE_BRFTXT:
			{
				CopyFileToString( hFile, strTempBuff1 );
				if ( m_nInsertAs == TEXT_INS )
				{
					// 1. Do Back translation (BackTranslateG2BrailleToText)
					BackTranslateG2BrailleToText( strTempBuff1, strTempBuff2 );
					// Prepare for: 2. Stream in as Text.
					es.dwCookie = (DWORD_PTR)&strTempBuff2;
				}
				else
				{	//  Prepare for: 1. Stream in as Text.
					es.dwCookie = (DWORD_PTR)&strTempBuff1;
				}
				nFormat = SF_TEXT | SFF_SELECTION;
				goto Type_Unknown;
			}
/*
TEXT or UNKNOWN is a file containing Text with no formatting codes. ( extention = .txt or .*** )
When Inserting TEXT as Text:
1. Stream in as Text.

When Inserting TEXT as Grade 2:
1.  Convert TEXT formatting Codes to RTF Codes.
2.  Convert RTF Codes to BRL formatting Codes.
3.  Do Translation to Grade 2 (TranslateBRLTextToG2Braille)
4.  Convert BRL formatting Codes to RTF.
5.  Stream in as RTF.
*/
		case DOCTYPE_TEXT:
		case DOCTYPE_UNKNOWN:
			{
				CopyFileToString( hFile, strTempBuff1 );
				if ( m_nInsertAs == TEXT_INS )
				{   // Prepare for: 1. Stream in as Text.
					nFormat = SF_TEXT | SFF_SELECTION;
					es.dwCookie = (DWORD_PTR)&strTempBuff1;
				}
				else
				{	// 1.  Convert TEXT formatting Codes to RTF Codes.
					TXTtoRTF( strTempBuff1, strTempBuff2);
					// 2.  Convert RTF Codes to BRL formatting Codes.
					RTFtoBRLTXT( strTempBuff2, strTempBuff3);
					// 3.  Do Translation to Grade 2 (TranslateBRLTextToG2Braille)
					TranslateBRLTextToG2Braille( strTempBuff3, strTempBuff4 );
					// 4.  Convert BRL formatting Codes to RTF.
					BRLTXTtoRTF( strTempBuff4, strTempBuff5);
					// Prepare for: 5.  Stream in as RTF.
					nFormat = SF_RTF | SFF_SELECTION;
					es.dwCookie = (DWORD_PTR)&strTempBuff5;
				}
				Type_Unknown:
				{
					WORD wLeadin;
					DWORD dwNumRead = 0;
					if(ReadFile(hFile, &wLeadin, 2, &dwNumRead, 0))
					{
						if (wLeadin == 0xfeff)
						{
							es.pfnCallback = StreamReadUnicodeCStringCallback;
							nFormat |= SF_UNICODE;
						}
						else
						{
							SetFilePointer(hFile, -2, NULL, FILE_CURRENT);
							es.pfnCallback = StreamReadCStringCallback;
						}
					}
					else
						es.pfnCallback = StreamReadCStringCallback;
				}
				StreamIn(nFormat, es);
			}
			break;
/*
SKITEXT is a file containing G2 Braille with RTF Codes or Text with RTF Codes. ( extention = .fsd )
When Inserting SKITEXT containing Grade 2 with RTF Codes as Text:
1.  Convert RTF Codes to TXT formatting Codes.
2.  Do Back translation (BackTranslateG2BrailleToText)
3.  Stream in as Text.

When Inserting SKITEXT containing Text with RTF Codes as Grade 2:
GOTO: When Inserting RTF as Grade 2:
1.  Convert RTF codes to BRL formatting codes.
2.  Do Translation to Grade 2 (TranslateBRLTextToG2Braille)
3.  Convert BRL formatting codes to RTF.
4.  Stream in as RTF.

When Inserting SKITEXT containing Text with RTF Codes as Text:
1.  Stream in as RTF.

When Inserting SKITEXT containing Grade 2 with RTF Codes as Grade 2:
1.  Stream in as RTF.
*/
		case DOCTYPE_SKITXT:
			{
				CopyFileToString( hFile, strTempBuff1 );
				BrlTxtDocProperties m_docPropertiesTemp;
				es.dwCookie = (DWORD_PTR)hFile;
				ImportDocProperties(hFile, m_docPropertiesTemp);
				if( m_docPropertiesTemp.brailleGrade2 && m_nInsertAs == TEXT_INS )
				{	// 1.  Convert RTF Codes to TXT formatting Codes.
					RTFtoTXT( strTempBuff1, strTempBuff2 );
					// 2.  Do Back translation (BackTranslateG2BrailleToText)
					BackTranslateG2BrailleToText( strTempBuff2, strTempBuff3 );
					// Prepare for:  3.  Stream in as Text.
					es.dwCookie = (DWORD_PTR)&strTempBuff3;
					nFormat = SF_TEXT | SFF_SELECTION;
				}
				else if( !m_docPropertiesTemp.brailleGrade2 && m_nInsertAs == GRADE_2_INS )
				{   // 1.  Convert RTF codes to BRL formatting codes.
					RTFtoBRLTXT( strTempBuff1, strTempBuff2 );
					// 2.  Do Translation to Grade 2 (TranslateBRLTextToG2Braille)
					TranslateBRLTextToG2Braille(strTempBuff2, strTempBuff3);
					// 3.  Convert BRL formatting codes to RTF.
					BRLTXTtoRTF( strTempBuff3, strTempBuff4);
					// Prepare for:  4.  Stream in as RTF.
					es.dwCookie = (DWORD_PTR)&strTempBuff4;
					nFormat = SF_RTF | SFF_SELECTION;
				}
				else
				{	// Prepare for: 1.  Stream in as RTF.
					es.dwCookie = (DWORD_PTR)&strTempBuff1;
					nFormat = SF_RTF | SFF_SELECTION;
				}
				es.pfnCallback = StreamReadCStringCallback;
				StreamIn(nFormat, es);
			}
			break;

/*
RTF or default is a file containing Text with RTF. ( extention = .rtf or .*** )
When Inserting RTF as Text:
1. Stream in as RTF.

When Inserting RTF as Grade 2:
1.  Convert RTF codes to BRL formatting codes.
2.  Do Translation to Grade 2 (TranslateBRLTextToG2Braille)
3.  Convert BRL formatting codes to RTF.
4.  Stream in as RTF.
*/
		case DOCTYPE_RTF:
		default:
			{
				CopyFileToString( hFile, strTempBuff1 );
				if ( m_nInsertAs == TEXT_INS )
				{
					// Prepare for: 1. Stream in as RTF.
					es.dwCookie = (DWORD_PTR)&strTempBuff1;
					nFormat = SF_RTF | SFF_SELECTION;
				}
				else
				{	// 1.  Convert RTF codes to BRL formatting codes.
					RTFtoBRLTXT( strTempBuff1, strTempBuff2 );
					// 2.  Do Translation to Grade 2 (TranslateBRLTextToG2Braille)
					TranslateBRLTextToG2Braille( strTempBuff2, strTempBuff3 );
					// 3.  Convert BRL formatting codes to RTF.
					BRLTXTtoRTF( strTempBuff3, strTempBuff4);
					es.dwCookie = (DWORD_PTR)&strTempBuff4;
					// Prepare for:  4.  Stream in as RTF.
					nFormat = SF_RTF | SFF_SELECTION;
				}
				es.pfnCallback = StreamReadCStringCallback;
				StreamIn(nFormat, es);
			}
			break;

		}

		::CloseHandle(hFile);

		return bSuccess;
	}

	static DWORD CALLBACK StreamReadUnicodeCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		// Return 0 for success, 1 for failure.
		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);
if (::ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL))
		return 0;
return 1;
		}
	static DWORD CALLBACK StreamReadUnicodeCStringCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		// Return 0 for success, 1 for failure.
		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);

		CString* src = (CString*)dwCookie;
		if (src->IsEmpty())
		{
			*pcb = 0;
			return(0);
		}
		auto unicodeBuffer = reinterpret_cast<WCHAR *>(pbBuff);
		int maxChars = cb / 2;
		if (maxChars > src->GetLength())
			maxChars = src->GetLength();
		for (long idx = 0; idx < maxChars; idx++)
			unicodeBuffer[idx] = src->GetAt(idx);
		*src = src->Mid(maxChars);
		*pcb = maxChars * 2;
return(0);
	}

	static DWORD CALLBACK StreamReadCallbackRTF(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		// Return 0 for success, 1 for failure.
		_ASSERTE(dwCookie != 0);
  		_ASSERTE(pcb != NULL);
#ifdef DEBUG
		::SetLastError(ERROR_SUCCESS);
#endif
		BOOL bResult = ::ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);
#ifdef DEBUG
		// RG: When I run FSEdit in the debugger
		// I keep hitting the break point below
		// However, we seem to be hitting the break point even when the ReadFile is successful
		// Therefore, I am disabling this.
		//if (dwErr != ERROR_SUCCESS)
			//DebugBreak();
/*
The control calls the callback function repeatedly, transferring a portion of the data with each call. The control continues to call the callback function
until one of the following conditions occurs:

. The callback function returns a nonzero value.
. The callback function returns zero in the * pcb parameter.
. An error occurs that prevents the rich edit control from transferring data into or out of itself.
Examples are out-of-memory situations, failure of a system function, or an invalid character in the read buffer.
. For a stream-in operation, the RTF code contains data specifying the end of an RTF block.
. For a stream-in operation on a single-line edit control, the callback reads in an end-of-paragraph character (CR, LF, VT, LS, or PS).
*/
		if (!bResult || *pcb == 0)
		{
//			DebugBreak();
		}
		if (cb != *pcb)
		{
//			DebugBreak();
		}
#endif
  		return !bResult;
	}

	static DWORD CALLBACK StreamReadCallbackBrl(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
	{
		static WORD wLang = PRIMARYLANGID(GetSystemDefaultLangID());  // avoid calling this unnecessarily
		bool isWesternEuropeanLanguage = (wLang == LANG_ENGLISH || wLang == LANG_GERMAN || wLang == LANG_FRENCH);
		long bufferSize = cb / 2;
		DWORD bytesRead = 0;
		auto buffer = make_unique<BYTE[]>(bufferSize);
		// Return 0 for success, 1 for failure.
		_ASSERTE(dwCookie != 0);
  		_ASSERTE(pcb != NULL);
#ifdef DEBUG
		::SetLastError(ERROR_SUCCESS);
#endif
		if (!ReadFile((HANDLE)dwCookie, buffer.get(), bufferSize, &bytesRead, NULL) ||
			bytesRead == 0) 
			return 1;
				auto unicodeBuffer = reinterpret_cast<WCHAR *>(pbBuff);
				for (DWORD i = 0; i < bytesRead;i++)
				{
		if (isWesternEuropeanLanguage && buffer[i] >= '@' && buffer[i] <= '^')
					buffer[i] += 32;
		CBrailleTable::DotPattern brailleCell;
		if (buffer[i] >= 32)
		{
			s_BrailleTable.Translate(static_cast<WCHAR>(buffer[i]), brailleCell);
			unicodeBuffer[i] = UnicodeBrailleStart + brailleCell.dotPattern[0];
		}
		else
			unicodeBuffer[i] = buffer[i];
				}
				*pcb = bytesRead * 2;
				return 0;
	}
	static long StripFormFeeds(LPBYTE pbBuff,long nLen);

	static DWORD CALLBACK StreamReadCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		// Return 0 for success, 1 for failure.
		_ASSERTE(dwCookie != 0);
  		_ASSERTE(pcb != NULL);
#ifdef DEBUG
		::SetLastError(ERROR_SUCCESS);
#endif
		BOOL bResult = ::ReadFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);
#ifdef DEBUG
		DWORD dwErr = ::GetLastError();
		if (dwErr != ERROR_SUCCESS)
			DebugBreak();
/*
The control calls the callback function repeatedly, transferring a portion of the data with each call. The control continues to call the callback function
until one of the following conditions occurs:

. The callback function returns a nonzero value.
. The callback function returns zero in the * pcb parameter.
. An error occurs that prevents the rich edit control from transferring data into or out of itself.
Examples are out-of-memory situations, failure of a system function, or an invalid character in the read buffer.
. For a stream-in operation, the RTF code contains data specifying the end of an RTF block.
. For a stream-in operation on a single-line edit control, the callback reads in an end-of-paragraph character (CR, LF, VT, LS, or PS).
*/
		if (!bResult || *pcb == 0)
		{
//			DebugBreak();
		}
		if (cb != *pcb)
		{
//			DebugBreak();
		}
#endif
  		return !bResult;
	}

	static DWORD CALLBACK StreamReadCStringCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		// Return 0 for success, 1 for failure.
		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);
		CString* src = (CString*)dwCookie;
		if (src->IsEmpty())
		{
			*pcb = 0;
			return(0);
		}

		if (cb > src->GetLength())
			cb = src->GetLength();
		for (long idx = 0; idx < cb; idx++)
			*pbBuff++ = (BYTE)src->GetAt(idx);	// OK to lose double byte
		*src = src->Mid(cb);
		*pcb = cb;
		return(0);
	}

	
	BOOL ConvertToGrade2Braille(HANDLE hFile)
	{
		
		EDITSTREAM es;
		es.dwError = 0;

		CString rtf;
		es.dwCookie = (DWORD_PTR)&rtf;
		es.pfnCallback = StreamWriteToString;
		StreamOut(SF_RTF, es);

		CString text;
		RTFtoTXT(rtf, text);
		rtf.Empty();

		CComBSTR bstrOut;
		text.ReleaseBuffer();
		if  (bstrOut)
		{
			m_docProperties.brailleGrade2 = 1;

			text = bstrOut;
			bstrOut.Empty();
			CopyStringToFile(text, hFile);

			es.dwCookie = (DWORD_PTR)&text;
			es.pfnCallback = StreamReadCStringCallback;
			StreamIn(SF_TEXT, es);
		}


		return TRUE;
	}


	BOOL TranslateBRLTextToG2Braille( CString &strInBuff, CString &strOutBuff )
	{
		int nInLen = strInBuff.GetLength();
		// Problem: the character '$' can be used in both G2 translation and also as a tag signature for BRL files.
		// In this function we temporarily replace '$' in the incoming string so that if the 
		// G2 translator produces them we can detect them.  After G2 translation we first double up '$' that may 
		// result from translation and then convert the '\1' back to '$'.  This way the BRL $ tags survive and G2 '$' 
		// characters are converted into "$$" strings.  During conversion of BRL files to other formats we convert 
		// "$$" to single "$".
			
		strInBuff.Replace(_T("$"), _T("\1"));
		LPTSTR lptstrIn = strInBuff.GetBuffer(nInLen);
		HRESULT hr = E_FAIL;

		#if 0
		// needs work, must be reenabled
		if(nInLen > 350000)
		{
			int n = nInLen/10;
			for(int i = 0; i < 10; i++)
				iRet = m_qipBrlTrans->Translate(lptstrIn+(i*n), lptstrOut+(i*n), iOutSize/10);
		}
		else
			#endif
		{
		CComBSTR bstrOut;
		hr = m_qipBrlTrans->Translate(lptstrIn,&bstrOut,0,0,-1,-1);
		if  (bstrOut)
			strOutBuff = bstrOut;
		}

		strOutBuff.Replace(_T("$"), _T("$$"));
		strOutBuff.Replace(_T("\1"), _T("$"));

		strInBuff.ReleaseBuffer();
		return SUCCEEDED(hr);
	}


	BOOL BackTranslateG2BrailleToText( CString &strInBuff, CString &strOutBuff )
	{
		int nInLen = strInBuff.GetLength();
		LPTSTR lptstrIn = strInBuff.GetBuffer(nInLen);
		CComBSTR bstrOut;
HRESULT hr = m_qipBrlTrans->BackTranslate(lptstrIn,&bstrOut);

		strInBuff.ReleaseBuffer();
		if (bstrOut)
		strOutBuff = bstrOut;
		return SUCCEEDED(hr);
	}

/*
This function is a replacement of T2A. It does the same conversion at A2T but allows use of larger buffers.
T2A uses the stack, this function allows for use of the heap.
*/
	void TCHAR2ANSI(LPTSTR lpIn, LPSTR lpOut, int iOutBufSize)
	{ // lpIn must be NULL terminated
#ifdef _UNICODE
		WideCharToMultiByte( GetACP() , 0, lpIn, -1, lpOut, iOutBufSize, NULL, NULL);
#else
		strcpy( lpOut, lpIn );
#endif
	}
/*
This function  is a replacement of A2T.  It does the same conversion at A2T but allows use of larger buffers.
A2T uses the stack, this function allows for use of the heap.
*/
	void ANSI2TCHAR(LPSTR lpIn, LPTSTR lpOut, int iOutBufSize)
	{ // lpIn must be NULL terminated
#ifdef _UNICODE
		MultiByteToWideChar( GetACP() , 0, lpIn, -1, lpOut, iOutBufSize );
#else
		strcpy( lpOut, lpIn );
#endif
	}

	BOOL ConvertToString(HANDLE hFile)
	{
		m_docProperties.brailleGrade2 = 0;

		EDITSTREAM es;
		es.dwError = 0;
		BOOL bSuccess = TRUE;

		CString sG2wTxtFmt, sTxt, sRtf;
		es.dwCookie = (DWORD_PTR)&sG2wTxtFmt;
		es.pfnCallback = StreamWriteToStringW;
		StreamOut(SF_TEXT | SF_UNICODE, es); // we want to process unicode characters in StreamWriteToString...
		bSuccess = (es.dwError) ? FALSE : TRUE;
		if(!bSuccess)
			return FALSE;

		BackTranslateG2BrailleToText( sG2wTxtFmt, sTxt );

		TXTtoRTF( sTxt, sRtf, &m_docProperties);

		int nLen = sRtf.GetLength();
		// 2x because of Wide to multibyte conversion.
		LPSTR lpsz = new char[(nLen +32)*2 ];
		if ( !lpsz )
		{
			return FALSE;
		}		
		ZeroMemory( lpsz, (nLen +32)*2 );

		TCHAR2ANSI(sRtf.GetBuffer(nLen), lpsz, (nLen +32)*2 );

		DWORD dwWritten  = 0L;
		WriteFile(hFile, lpsz, nLen , &dwWritten, NULL);
		sRtf.ReleaseBuffer(nLen);
		
		es.dwCookie = (DWORD_PTR)&sRtf;
		es.pfnCallback = StreamReadCStringCallback;
		StreamIn(SF_RTF, es);

		delete [] lpsz;
		return TRUE;
	}



	BOOL BackTranslate_ConvertToText(LPCTSTR lpszFilePath)
	{
		m_docProperties.brailleGrade2 = 0;
		HANDLE hTFile = ::CreateFile(_T("\\temp\\temp.file"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(hTFile == NULL || hTFile == INVALID_HANDLE_VALUE)
			return FALSE;

		EDITSTREAM es;
		es.dwError = 0;
		BOOL bSuccess = TRUE;

		// Stream the rtf contents of the control to a string
		es.dwCookie = (DWORD_PTR)hTFile;
		es.pfnCallback = StreamWriteCallback;	// Use our RTF callback
		StreamOut(SF_TEXT, es);
		bSuccess = (es.dwError) ? FALSE : TRUE;
		::CloseHandle(hTFile);
		VARIANT_BOOL transResult;
		m_qipBrlTrans->BackTranslateFile( _T("\\temp\\temp.file"), CComBSTR(lpszFilePath),&transResult);
		
		DeleteFile(_T("\\temp\\temp.file"));
		return TRUE;
	}

	BOOL SaveFile(LPCTSTR lpszFilePath)
	{
		_ASSERTE(lpszFilePath != NULL);
		HANDLE hFile = ::CreateFile(lpszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(hFile == NULL || hFile == INVALID_HANDLE_VALUE)
			return FALSE;

		// Set our custom doc values
		SetDocProperties(m_docProperties);

		EDITSTREAM es;
		es.dwError = 0;
		BOOL success = TRUE;
		switch (FileType(lpszFilePath))
		{
			// A new File with extension BRF and grade 2 braille or a file opened that is not a BRF file and not grade 2 braille now with
			// the extension BRF.
			case DOCTYPE_BRFTXT:
				if(!m_docProperties.brailleGrade2)
				{
					TEXTMODE dwMode = GetTextMode();
					SetTextMode(TM_RICHTEXT);
					BOOL bRet; // = ConvertToGrade2Braille(hFile);
					TMessageBox<IDD_BRF_WAIT, BOOL, HANDLE, CBrlEdit> msg(&CBrlEdit::ConvertToGrade2Braille, bRet, this);
					msg.DoModal(hFile, IDSTA_CONVERT);
					if(!bRet)
					{
						DeleteFile(lpszFilePath);
						SetTextMode(dwMode);
						CloseHandle(hFile);
						return FALSE;
					}

					SetTextMode(dwMode);
				}
				else
				{
					es.dwCookie = (DWORD_PTR)hFile;
					es.pfnCallback = StreamWriteCallback;
					StreamOut(SF_TEXT, es);
				}
				break;
			case DOCTYPE_BRAILLE_UNKNOWN:
			case DOCTYPE_TEXT:
				if(m_docProperties.brailleGrade2)
				{
					CloseHandle(hFile);
					if(!BackTranslate_ConvertToText(lpszFilePath))
					{
						DeleteFile(lpszFilePath);
						return FALSE;
					}
				}
				else
				{
					es.dwCookie = (DWORD_PTR)hFile;
					es.pfnCallback = StreamWriteCallback;
					StreamOut(SF_TEXT, es);
				}
				break;
			case DOCTYPE_BRLTXT:
				{
					CString rtf;
					// First stream the control to a string
					es.dwCookie = (DWORD_PTR)&rtf;
//					es.pfnCallback = RTFStream_WriteToStringCallback;	// Use our RTF callback
					es.pfnCallback = StreamWriteToString;
					StreamOut(SF_RTF, es);
					success = (es.dwError) ? FALSE : TRUE;
					if (!success )
						break;
					if(m_docProperties.brailleGrade2 )
					{
						RTFtoBRLTXT(rtf, hFile);
					}
					else
					{
						CString strTemp1, strTemp2;
						RTFtoBRLTXT( rtf, strTemp1);
						TranslateBRLTextToG2Braille( strTemp1, strTemp2);
						CopyStringToFile( strTemp2, hFile);
					}
				}
				break;
			case DOCTYPE_SKITXT:
				es.dwCookie = (DWORD_PTR)hFile;
				es.pfnCallback = RTFStream_WriteToFileCallback;	// Use our RTF callback
				StreamOut(SF_RTF, es);
				break;
					case DOCTYPE_RTF:
			default:		// Unknown extension, default to RTF
				if(m_docProperties.brailleGrade2)
				{
					if(!ConvertToString(hFile))
					{
						CloseHandle(hFile);
						return FALSE;
					}
				}
				else
				{
					es.dwCookie = (DWORD_PTR)hFile;
					es.pfnCallback = RTFStream_WriteToFileCallback;	// Use our RTF callback
					StreamOut(SF_RTF, es);
					success = (es.dwError) ? FALSE : TRUE;
				}
				break;
		}

		::CloseHandle(hFile);

		if (success)
		{
			SHAddToRecentDocs(SHARD_PATH,lpszFilePath);
			_tcscpy_s(m_szCurrentFile,lpszFilePath);
		}
		return(success);
	}

	BOOL CopyFileToString( HANDLE hRFile, CString &strOutBuff )
	{
		DWORD dwd = GetFileSize( hRFile, NULL );
		LPSTR lpszBuffer = new char[ dwd + 32 ];
		if ( !lpszBuffer )
		{
			return FALSE;
		}
		ZeroMemory( lpszBuffer, (dwd + 32));
		BOOL bRSuccess = FALSE;
		int iNumberBytes= dwd;
		unsigned long ulNumberRead = 0;
		bRSuccess = ReadFile( hRFile, lpszBuffer, iNumberBytes, &ulNumberRead, NULL);
		if (ulNumberRead > 0 && bRSuccess)
		{
			strOutBuff = lpszBuffer;
		}
		else
		{
			delete [] lpszBuffer;
			return FALSE;
		}
		delete [] lpszBuffer;
		return TRUE;
	}

	BOOL CopyStringToFile( CString strInBuff ,HANDLE hWFile )
	{
		DWORD dwWritten = 0;
		int iLength = strInBuff.GetLength();
		LPSTR lpszBuffer = new char[ (iLength + 32)*2 ];
		if ( !lpszBuffer )
		{
			return FALSE;
		}
		ZeroMemory( lpszBuffer, (iLength + 32)*2);

		TCHAR2ANSI( strInBuff.GetBuffer(iLength), lpszBuffer, (iLength + 32)*2);

		BOOL bWSuccess = WriteFile(hWFile, lpszBuffer, iLength, &dwWritten, NULL);
		delete [] lpszBuffer;
		if ( !bWSuccess)
		{
			return FALSE;
		}
		return TRUE;
	}

	static DWORD CALLBACK StreamWriteToString(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR * pcb)
	{
		USES_CONVERSION;

		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);
		CString* src = (CString*)dwCookie;

		// Set the string size to be big enough to handle the new data received. This will enable
		// us to still call the += operator but will not cause realloc's by CString for every
		// call we make.
		int prevLen = src->GetLength();
		src->GetBufferSetLength(prevLen + cb);
		src->ReleaseBuffer(prevLen);

 		for (LONG i = 0; i < cb; i++)
 		{
 			*src += (char)(*pbBuff);
 			pbBuff++;
 		}

		*pcb = cb;

		return 0;
	}
   
	static DWORD CALLBACK StreamWriteToStringW(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR * pcb)
	{
		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);
		CString* src = (CString*)dwCookie;

		// Set the string size to be big enough to handle the new data received. This will enable
		// us to still call the += operator but will not cause realloc's by CString for every
		// call we make.
		int prevLen = src->GetLength();
		src->GetBufferSetLength(prevLen + cb);
		src->ReleaseBuffer(prevLen);
		WCHAR wChar;

		for (LONG i = 0; i < cb; i+=2)
 		{
			wChar = (unsigned char)*pbBuff;
			pbBuff++;
			wChar += ((unsigned char)*pbBuff)<<8; 
			pbBuff++;
 			(*src) = (*src) + wChar;
 		}

		*pcb = cb;

		return 0;
	}

	
	static DWORD CALLBACK StreamWriteCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
	{
		_ASSERTE(dwCookie != 0);
		_ASSERTE(pcb != NULL);
		return !::WriteFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)pcb, NULL);
	}

	LRESULT NotifyBrailleGradeChng(BOOL bGrade2)
	{
		return 0;
	}

	BOOL SetComPortEmbossing(HANDLE hPort, int iBaud, int iBufSize);
	int WriteToPort(LPCTSTR lpszFileName, LPTSTR lpszPort, int iBaud);
	BOOL SetLPTPortEmbossing(HANDLE hPort);

	BOOL PrintDocument(void)
	{
		#define tmpBrlFile  "$$brledit.brl"
		#define tmpEmbFile  "$$brlemb.out"
		//Braille Blazer defaults are used.
		EMB_SETTINGS eSet;
		eSet.bEmbosserGoesToNewLine = TRUE;
		eSet.bEmbosserGoesToNewPage = FALSE;
		eSet.bInterpointMargins = FALSE;
		eSet.bInterpointPages = FALSE;
		eSet.iCellsPerLine = 34;
		eSet.iMaxCellsPerLine = 34;
		eSet.iLinesPerPage = 25;
		eSet.iMaxLinesPerPage = 27;
		eSet.iTopMargin = 0;
		eSet.iBottomMargin = 0;
		eSet.iLeftMargin = 0;
		eSet.iRightMargin = 0;
		eSet.sEmbosserSel = _T("");
		eSet.sConnectionSel = _T("IRDA:");
		eSet.sNetworkDeviceSel = _T("");
		eSet.sNetworkPortSel = _T("");
		eSet.bIsNetworkEmb = FALSE;
		eSet.iNumCopies = 1;
		eSet.bGrade2Braille = TRUE;
		eSet.bEmphasizedText = FALSE;

		// Get last saved Embosser Properties from Registry
		GTUtility::RegDataType type;
		for(int x = 0; x < NUM_EMB_SETTINGS; x++)
		{
			type.dwType = Skipper::_Emb[x].second;
			// this string had to be hard coded because GTUtility::bstrFSEmb for some reason in Release versions returns an empty string
			if(GTUtility::GetRegistryValue(_T("\\Software\\Freedom Scientific\\Emboss")/*GTUtility::bstrFSEmb*/,Skipper::_Emb[x].first, type))
			{
				if ( type.dwType == REG_DWORD )
				{
					DwordRegParam( eSet, aEmbSettings[x]) = type.dw;
				}
				else
				{
					 CStringRegParam( eSet, aEmbSettings[x])= type.sz;
				}
			}
		}

		// Create a temporary file to receive the brltxt
		HANDLE hFile = ::CreateFile(_T(tmpBrlFile), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
		if(hFile == NULL || hFile == INVALID_HANDLE_VALUE)
			return ERR_GRL_UNKNOWN;

		EDITSTREAM es;
		es.dwError = 0;
		BOOL success = TRUE;

		// Set our custom doc values
		SetDocProperties(m_docProperties);

		// Stream the rtf contents of the control to a string
		CString rtf;
		es.dwCookie = (DWORD_PTR)&rtf;
		es.pfnCallback = RTFStream_WriteToStringCallback;	// Use our RTF callback
		StreamOut(SF_RTF, es);
		success = (es.dwError) ? FALSE : TRUE;

		// Now convert the rtf into brltxt (into the tmp dest file)
		if (success)
			RTFtoBRLTXT(rtf, hFile);

		::CloseHandle(hFile);

		if(!success)
		{
			return ERR_BRF_TRANSLATION;
		}

		if ( !ModifyFilePreBrfTrans( _T(tmpBrlFile), eSet, TRUE, FALSE ) )
		{
			return ERR_GRL_UNKNOWN;
		}
		// Next use BrlTrans to xlate the brl into a format that
		// the embosser can print. Use the grade 2 setting.
		// Note: function requires ANSI strings and not UNICODE
		VARIANT_BOOL transResult;
		m_qipBrlTrans->TranslateFile(_T(tmpBrlFile), _T(tmpEmbFile), eSet.bGrade2Braille?VARIANT_TRUE:VARIANT_FALSE,&transResult);

		if ( !ModifyFilePostBrfTrans( _T(tmpEmbFile), eSet, FALSE ) )
		{
			return ERR_GRL_UNKNOWN;
		}

		TCHAR szConnection[ _MAX_PATH*2];
		if ( eSet.bIsNetworkEmb )
		{
			_tcscpy_s( szConnection, eSet.sNetworkPortSel );
		}
		else
		{
			_tcscpy_s( szConnection, eSet.sConnectionSel);
		}
		// Send the embosser file to the printer
		while( eSet.iNumCopies >= 1 )
		{
			int iRet = WriteToPort(_T(tmpEmbFile), szConnection, CBR_9600);
			switch( iRet )
			{
				case -3:
					return ERR_GRL_UNKNOWN;
				case -2:
					return ERR_EMB_COMMUNICATION_RETRY;
				case -1:
					return ERR_EMB_COMMUNICATION;
				case 0:
					return ERR_EMB_PORT;
				default :
					break;
			}

			eSet.iNumCopies--;
		}
		// Cleanup our temporary files
		DeleteFile(_T(tmpEmbFile));
		DeleteFile(_T(tmpBrlFile));

		return TRUE;
	}

	HRESULT GetFormatInfo(long lpBuf,long flags);
	DocType FileType(LPCTSTR lpszFilePath)
	{
		CString ext(lpszFilePath);

		ext = ext.Right(4);

		if (ext.CompareNoCase(_T(".brl")) == 0)
			return(DOCTYPE_BRLTXT);
		if(ext.CompareNoCase(_T(".fsd")) == 0)
			return (DOCTYPE_SKITXT);
		if(ext.CompareNoCase(_T(".brf")) == 0)
			return (DOCTYPE_BRFTXT);
		if (ext.CompareNoCase(_T(".rtf")) == 0)
			return(DOCTYPE_RTF);
		if (ext.CompareNoCase(_T(".txt")) == 0)
			return(DOCTYPE_TEXT);
		
		ext = ext.Left(3);
		if(ext.CompareNoCase(_T(".br")) == 0)
			return DOCTYPE_BRAILLE_UNKNOWN;

		return(DOCTYPE_UNKNOWN);
	}

	BOOL ModifyFilePreBrfTrans( LPCTSTR lpszFile, EMB_SETTINGS eSet, BOOL bAddInFormattingCodes = TRUE, BOOL bDoParagraphFormatting = TRUE )
	{
		HANDLE hRFile;
		HANDLE hWFile;
		CString sTemp;
		hRFile = ::CreateFile(  lpszFile, GENERIC_READ, 0 , NULL , OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hRFile == NULL || hRFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		DWORD dwd = GetFileSize( hRFile, NULL );
		LPSTR lpszBuffer = new char[ dwd + 32 ];
		if ( !lpszBuffer )
		{
			CloseHandle( hRFile );
			return FALSE;
		}
		ZeroMemory( lpszBuffer, (dwd + 32));
		BOOL bWSuccess = FALSE;
		BOOL bRSuccess = FALSE;
		int iNumberBytes= dwd;
		unsigned long ulNumberToWrite = 0;
		unsigned long ulNumberWritten = 0;
		bRSuccess = ReadFile( hRFile, lpszBuffer, iNumberBytes, &ulNumberToWrite, NULL);
		if (ulNumberToWrite > 0 && bRSuccess)
		{
			sTemp = lpszBuffer;

			if ( bDoParagraphFormatting )
			{  // removes
				int iCurrentLoc = 0;
				int iCRLoc;
				while (1)
				{	// find new line char
					iCRLoc = sTemp.Find( _T('\n'), iCurrentLoc );
					iCurrentLoc = iCRLoc;
					if ( iCRLoc < 0 )
					{
						break;
					}
					if ((iCurrentLoc + 4) >= sTemp.GetLength())	{	break; } // if at end of buffer then break
					iCurrentLoc++;
					// we have found a new line char, so see if there is another one next to it
					while ( _T(' ') == sTemp.GetAt( iCurrentLoc ) || _T('\n') == sTemp.GetAt( iCurrentLoc ))
					{
						if( _T('\n') == sTemp.GetAt( iCurrentLoc ))
						{   // there is another new line char
							int iTempCurLoc = iCurrentLoc;
							if ((iCurrentLoc + 4) >= sTemp.GetLength())	{	break; }
							iCurrentLoc++;
							while ( isgraph( sTemp.GetAt( iCurrentLoc) ) == 0 )
							{
								if ( sTemp.GetAt( iCurrentLoc ) == _T('\n'))
								{  // there are more than 2 new line chars
									iCRLoc = iTempCurLoc;
								}
								if ((iCurrentLoc + 4) >= sTemp.GetLength())	{ break; }
								iCurrentLoc++;
							}
							if ((iCurrentLoc + 4) >= sTemp.GetLength())	{	break; }
							sTemp.Delete( (iCRLoc + 1), (iCurrentLoc - (iCRLoc + 1 )));
							sTemp.Insert( (iCurrentLoc - 1), _T("  "));
							break;
						}
						iCurrentLoc++;
					}
				}
			}
			if ( bAddInFormattingCodes )
			{
				CString sCode = _T("");
				char tmp[15];
				sprintf_s(tmp, " $mr%02d ", eSet.iRightMargin);
				sCode = tmp;
				sTemp.Insert( 0 , sCode );

				sprintf_s(tmp, " $ml%02d ", eSet.iLeftMargin );
				sCode = tmp;
				sTemp.Insert( 0 , sCode );

				sprintf_s(tmp, " $mb%02d ", eSet.iBottomMargin);
				sCode = tmp;
				sTemp.Insert( 0 , sCode );

				sprintf_s(tmp, " $mt%02d ", eSet.iTopMargin);
				sCode = tmp;
				sTemp.Insert( 0 , sCode );

				sprintf_s(tmp, " $pl%02d ", (eSet.iLinesPerPage + eSet.iTopMargin));
				sCode = tmp;
				sTemp.Insert( 0 , sCode );

				// for some reason brlTrans when translating does one less than given number of cells so we must
				// increment it to get the correct value for embossing...
				sprintf_s(tmp, " $pw%02d ", (eSet.iCellsPerLine + eSet.iLeftMargin + 1 ));
				sCode = tmp;
				sTemp.Insert( 0 , sCode );
			}
			ulNumberToWrite = sTemp.GetLength();
			LPSTR lpszNewBuffer = new char[ (ulNumberToWrite + 32)*2 ];
			if ( !lpszNewBuffer )
			{		
				delete lpszBuffer;
				::CloseHandle( hRFile );
				return FALSE;
			}
			ZeroMemory( lpszNewBuffer, (ulNumberToWrite + 32)*2);

			TCHAR2ANSI(sTemp.GetBuffer(ulNumberToWrite), lpszNewBuffer, (ulNumberToWrite + 32)*2 );

			LPSTR lpszNewBufferHold = lpszNewBuffer;
			::CloseHandle( hRFile );
			DeleteFile( lpszFile );
			hWFile = ::CreateFile( lpszFile, GENERIC_WRITE, 0 , NULL , CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			while( 1 )
			{
				bWSuccess = WriteFile( hWFile, lpszNewBuffer, ulNumberToWrite, &ulNumberWritten, NULL);
				if (ulNumberToWrite != ulNumberWritten)
				{
					if( ulNumberWritten == 0 )
					{
						lpszNewBuffer = lpszNewBufferHold;
						delete lpszBuffer;
						delete lpszNewBuffer;
						::CloseHandle( hWFile );
						return -1;
					}
					ulNumberToWrite = ulNumberToWrite - ulNumberWritten;
					lpszNewBuffer += ulNumberWritten;
					ulNumberWritten = 0;
				}
				else
				{
					break;
				}
			}
			::CloseHandle( hWFile );
			lpszNewBuffer = lpszNewBufferHold;
			delete lpszNewBuffer;
		}
		delete lpszBuffer;
		return bWSuccess;
	}

	BOOL ModifyFilePostBrfTrans( LPCTSTR lpszFile, EMB_SETTINGS eSet, BOOL bRemoveEndingNewLineChars )
	{
		BOOL bChangeToTxtFmtChar = TRUE;
		BOOL bPageAdded = FALSE;
		BOOL bParagraphFormatting = FALSE;
		int iIntptIndent = eSet.iMaxCellsPerLine - eSet.iCellsPerLine;

		HANDLE hRFile;
		HANDLE hWFile;
		CString sTemp;
		hRFile = ::CreateFile(  lpszFile, GENERIC_READ, 0 , NULL , OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hRFile == NULL || hRFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		DWORD dwd = GetFileSize( hRFile, NULL );
		LPSTR lpszBuffer = new char[ dwd + 32 ];
		if ( !lpszBuffer )
		{
			CloseHandle( hRFile );
			return FALSE;
		}
		ZeroMemory( lpszBuffer, (dwd + 32));
		BOOL bWSuccess = FALSE;
		BOOL bRSuccess = FALSE;
		int iNumberBytes= dwd;
		unsigned long ulNumberToWrite = 0;
		unsigned long ulNumberWritten = 0;

		bRSuccess = ReadFile( hRFile, lpszBuffer, iNumberBytes, &ulNumberToWrite, NULL);
		if (ulNumberToWrite > 0 && bRSuccess)
		{
			sTemp = lpszBuffer;
			if ( bRemoveEndingNewLineChars )
			{
				int iLastPageBrk = sTemp.ReverseFind( _T('\f'));
				int iLastNewLine = iLastPageBrk -1;
				while (sTemp.GetAt( iLastNewLine ) == _T('\r') || sTemp.GetAt( iLastNewLine ) == _T('\n'))
				{
					sTemp.Delete(iLastNewLine);
					iLastNewLine --;
				}
			}
			if ( bParagraphFormatting )
			{
				int iCurrentLoc = 0;
				int iCRLoc;
				while (1)
				{
					iCRLoc = sTemp.Find( _T('\r'), iCurrentLoc );
					iCurrentLoc = iCRLoc;
					if ( iCRLoc < 0 )
					{
						break;
					}
					if ((iCurrentLoc + 4) >= sTemp.GetLength())	{	break; }
					iCurrentLoc++;
					while ( _T(' ') == sTemp.GetAt( iCurrentLoc ) || _T('\r') == sTemp.GetAt( iCurrentLoc ))
					{
						if( _T('\r') == sTemp.GetAt( iCurrentLoc ))
						{
							int iTempCurLoc = iCurrentLoc;
							if ((iCurrentLoc + 4) >= sTemp.GetLength())	{	break; }
							iCurrentLoc++;
							while ( isgraph( sTemp.GetAt( iCurrentLoc) ) == 0 )
							{
								if ( sTemp.GetAt( iCurrentLoc ) == _T('\r'))
								{
									iCRLoc = iTempCurLoc;
								}
								if ((iCurrentLoc + 4) >= sTemp.GetLength())	{ break; }
								iCurrentLoc++;
							}
							if ((iCurrentLoc + 4) >= sTemp.GetLength())	{	break; }
							sTemp.Delete( (iCRLoc + 1), (iCurrentLoc - (iCRLoc + 1 )));
							sTemp.Insert( (iCurrentLoc - 1), _T("  "));
							break;
						}
						iCurrentLoc++;
					}
				}
			}
			if ( eSet.bEmbosserGoesToNewLine )
			{
				int iFirstLoc = -1;
				while( 1 )
				{
					int iSecondLoc = sTemp.Find( _T('\r'), iFirstLoc + 1 );
					if ( iSecondLoc < 0 )
					{
						break;
					}
					if ( iSecondLoc - (iFirstLoc + 1) == eSet.iMaxCellsPerLine )
					{
						sTemp.Delete( iSecondLoc);
						iSecondLoc--;
					}
					iFirstLoc = iSecondLoc;
					if ( iFirstLoc + 2 >= sTemp.GetLength())
					{
						break;
					}
				}
			}

			if ( eSet.bInterpointPages )
			{
				int iLastFound = 0;
				int iPageCount = 0;
				do
				{
					iLastFound = sTemp.Find( _T("\f"), iLastFound +1 );
					if( iLastFound >= 0)
					{
						iPageCount++;
					}
				}
				while( iLastFound >= 0 );
				if ( iPageCount%2 == 1 )
				{ // make even number of pages
					bPageAdded = TRUE;
					sTemp+= _T("\f");
				}
			}
			if ( eSet.bInterpointMargins )
			{
				int iCurrentLoc = 0;
				int iCount;

				while( 1 )
				{
					iCurrentLoc = sTemp.Find( _T("\f"), iCurrentLoc );
					if( iCurrentLoc < 0 || iCurrentLoc >= (sTemp.GetLength() - 1) )
					{
						break;
					}
					iCurrentLoc++;
					for( iCount = 1; iIntptIndent >= iCount; iCount++)
					{
						sTemp.Insert( iCurrentLoc, _T(" "));
					}
					while ( sTemp.Find( _T("\f"), iCurrentLoc) > sTemp.Find( _T("\r"), iCurrentLoc))
					{
						iCurrentLoc = sTemp.Find( _T("\r"), iCurrentLoc);
						if( iCurrentLoc < 0 || iCurrentLoc >= (sTemp.GetLength() - 1) )
						{
							break;
						}
						iCurrentLoc++;
						for( iCount = 1; iIntptIndent >= iCount; iCount++)
						{
							sTemp.Insert( iCurrentLoc, _T(" "));
						}
					}
					if( iCurrentLoc < 0 || iCurrentLoc >= (sTemp.GetLength() - 1) )
					{
						break;
					}
					iCurrentLoc = sTemp.Find( _T("\f"), iCurrentLoc) + 1;
					if( iCurrentLoc < 0 || iCurrentLoc >= (sTemp.GetLength() - 1) )
					{
						break;
					}
				}
			}

			if ( bChangeToTxtFmtChar )
			{
				sTemp.Replace( _T("\r"), _T("\r\n"));
			}
			if ( eSet.bEmbosserGoesToNewPage )
			{
				sTemp.Replace( _T("\f"), _T("\r\n"));
				sTemp += _T("\f");
				if( bPageAdded)
				{
					sTemp += _T("\f");
				}
			}

			ulNumberToWrite = sTemp.GetLength();
			LPSTR lpszNewBuffer = new char[ (ulNumberToWrite + 32)*2 ];
			if ( !lpszNewBuffer )
			{
				delete lpszBuffer;
				::CloseHandle( hRFile );
				return FALSE;
			}
			ZeroMemory( lpszNewBuffer, (ulNumberToWrite + 32)*2);

			TCHAR2ANSI(sTemp.GetBuffer(ulNumberToWrite), lpszNewBuffer, (ulNumberToWrite + 32)*2 );

			LPSTR lpszNewBufferHold = lpszNewBuffer;

			::CloseHandle( hRFile );
			DeleteFile( lpszFile );
			hWFile = ::CreateFile( lpszFile, GENERIC_WRITE, 0 , NULL , CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			while( 1 )
			{
				bWSuccess = WriteFile( hWFile, lpszNewBuffer, ulNumberToWrite, &ulNumberWritten, NULL);
				if (ulNumberToWrite != ulNumberWritten)
				{
					if( ulNumberWritten == 0 )
					{
						lpszNewBuffer = lpszNewBufferHold;
						delete lpszBuffer;
						delete lpszNewBuffer;
						::CloseHandle( hWFile );
						return -1;
					}
					ulNumberToWrite = ulNumberToWrite - ulNumberWritten;
					lpszNewBuffer += ulNumberWritten;
					ulNumberWritten = 0;
				}
				else
				{
					break;
				}
			}
			::CloseHandle( hWFile );
			lpszNewBuffer = lpszNewBufferHold;
			delete lpszNewBuffer;
		}
		delete lpszBuffer;
		return bWSuccess;
	}
	static int CALLBACK BrlEditWordBreakProc(LPTSTR lpch,
		int ichCurrent,int cch,int code);
protected:
	typedef struct
	{
		UINT nContainsBrlText;
		UINT nSelChanged;
	} RegisteredMessages;
	RegisteredMessages m_RegisteredMsg;
	void RegisterMessages();
	HWND m_hwndNotify; // Window to be notified of special events.
						// It's typically the JAWS window.
	void NotifyWindow(UINT nMsg)
	{
		// this fix should be tempary and will check with platform builder to see if Jaws is blocking somewhere.
			::PostMessage(m_hwndNotify,nMsg,0,0);
	}

	
	CComQIPtr<IBrailleTranslator> m_qipBrlTrans;
	static CBrailleTable s_BrailleTable;
	static const WCHAR UnicodeBrailleStart = 0x2800;
private:
#ifdef BOOKMARK_SUPPORT
	list<CComPtr<ITextRange> > lstBookMarks;
	list<CComPtr<ITextRange> >::iterator itBookm;
#endif
};
