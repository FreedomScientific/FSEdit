#include "stdafx.h"
#include "BrlEditWnd.h"
#include "SkeletonBrailleTranslator.h"


#define NUMRETYR  5                   // Maximum number of retries

typedef struct tagCOOKIE
{
   HANDLE hFile;
   LPBYTE pbStart;
    LPBYTE pbCur;
    LONG   bCount;
    DWORD  dwError;
} COOKIE, * PCOOKIE;


#define MAX_BUF	512

#define SFF_PWI      0x0800
#define SF_PWI      ( SF_RTF | SFF_PWI | 0x010000 )
extern HINSTANCE g_hInst;
CBrailleTable CBrlEdit::s_BrailleTable;
BrlTxtDocProperties CBrlEdit::m_docProperties;

//CString strRtf;
CComBSTR bstrRtf;

/*static DWORD CALLBACK BufferReadCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   PCOOKIE pCookie = (PCOOKIE)dwCookie;
   LONG   bytesLeft;
   LONG   bytesRead;
   // Calc the bytes read.
   bytesRead = pCookie->pbCur - pCookie->pbStart;
   // Calc bytes left to read.
   if (bytesRead < pCookie->bCount)
   {
      // Calc the bytes left to read.
      bytesLeft = pCookie->bCount - bytesRead;
   }
   else
   {
      bytesLeft = 0;
   }
   // Don’t read past the end of buffer.
   if (cb > bytesLeft) cb = bytesLeft;
   // Set bytes read.
   *pcb = cb;
   // Copy any bytes.
   if (cb)
   {
      memcpy(pbBuff, pCookie->pbCur, cb);
      pCookie->pbCur += cb;
   }
   // Return no error.
   return 0;
} // BufferReadCallback()
*/

static DWORD CALLBACK ReadCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   PCOOKIE pCookie = (PCOOKIE)dwCookie;
   // Zero the bytes read.
   *pcb = 0;
   BOOL bResult = ReadFile(pCookie->hFile, pbBuff, cb, (ULONG*)pcb, 0);

   return !bResult;
} // ReadCallback()

static DWORD CALLBACK WriteCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   PCOOKIE pCookie = (PCOOKIE)dwCookie;
   long lResult = WriteFile(pCookie->hFile, pbBuff, cb, (ULONG*)pcb, 0);
   return !lResult;
} // WriteCallback()

static DWORD CALLBACK BufferWriteCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	USES_CONVERSION;
	PCOOKIE pCookie = (PCOOKIE)dwCookie;
	// Do not overwrite the end of the buffer.
	// If there is no output buffer, then we’re
	// only here to determine the space required
	// for the stream out.
	if (pCookie->pbStart && (pCookie->pbCur + cb > pCookie->pbStart + pCookie->bCount))
	{
		// Writing all this data would overflow the buffer.
		// So only write what will fit.
		cb = static_cast<int>(pCookie->pbStart + pCookie->bCount - pCookie->pbCur);
	}

    *pcb = cb;
    if (pCookie->pbStart)
		memcpy(pCookie->pbCur, pbBuff, cb);

   	pCookie->pbCur += cb;

	return 0;
} // BufferWriteCallback()



void CBrlEdit::DisplayErrorMsg()
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL
	);
	// Process any inserts in lpMsgBuf.
	// ...
	// Display the string.
	::MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
	// Free the buffer.
	LocalFree( lpMsgBuf );
}

bool CBrlEdit::EnumeratePrinter(LPNETRESOURCE nrStart, bool bDisplayErrMsg /* = false */)
{
	DWORD dwResult, dwResultEnum;
	LPNETRESOURCE lpNRBuffer;
	DWORD dwBufferSize = 16384;
	DWORD dwNumEntries = 0xffffffff;
	HANDLE hEnum;

	dwResult = WNetOpenEnum(RESOURCE_GLOBALNET, RESOURCETYPE_PRINT, 0, nrStart, &hEnum);
	if(dwResult != NO_ERROR)
	{
		if(bDisplayErrMsg)
			DisplayErrorMsg();
		return false;
	}

	lpNRBuffer = (LPNETRESOURCE)LocalAlloc(LPTR, dwBufferSize);
	do
	{
		dwBufferSize = 16384;
		dwNumEntries = 0xffffffff;
		dwResultEnum = WNetEnumResource(hEnum, &dwNumEntries, lpNRBuffer, &dwBufferSize);

		if(dwResultEnum == NO_ERROR)
		{
			for(DWORD i = 0; i < dwNumEntries; i++)
			{
				if(lpNRBuffer[i].dwUsage & RESOURCEUSAGE_CONTAINER)
				{
					EnumeratePrinter(&lpNRBuffer[i]);
				}
				else
				{
					PrinterInfo pi;
					pi.bstrLocal = lpNRBuffer[i].lpLocalName;
					pi.bstrRemote = lpNRBuffer[i].lpRemoteName;
					pi.bstrProvider = lpNRBuffer[i].lpProvider;
					vecPrinterInfo.push_back(pi);
				}
			}
		}
		else if(dwResultEnum != ERROR_NO_MORE_ITEMS)
		{
			if(bDisplayErrMsg)
				DisplayErrorMsg();
			break;
		}
	}while(dwResultEnum != ERROR_NO_MORE_ITEMS);

	LocalFree(lpNRBuffer);
	dwResult = WNetCloseEnum(hEnum);
	if(dwResult != NO_ERROR)
	{
		if(bDisplayErrMsg)
			DisplayErrorMsg();
		return false;
	}

	return true;
}

void CBrlEdit::NetworkPrinters(bool bReInit /* = false */)
{
	if(vecPrinterInfo.size() == 0 || bReInit)
	{
		vecPrinterInfo.clear();
		NETRESOURCE nr;
		memset(&nr, 0, sizeof(nr));

		nr.lpRemoteName = _T("Henter");
		nr.dwUsage = RESOURCEUSAGE_CONTAINER;
		EnumeratePrinter(&nr);
	}
}
template <class T>
static
HRESULT CreateWithRef(T** ppObject)
{
	CComObject<T>* pObject;
	HRESULT hr = CComObject<T>::CreateInstance(&pObject);
	if (SUCCEEDED(hr))
	{
		pObject->AddRef();
		*ppObject = pObject;
	}

	return hr;
}
static CComPtr<IBrailleTranslator> LoadBrailleTranslator()
{
	// This just returns a skeleton translator that does nothing.
	CComPtr<CSkeletonBrailleTranslator> skeletonTranslator;
	CreateWithRef(&skeletonTranslator);
	ATLASSERT(skeletonTranslator);
	CComQIPtr<IBrailleTranslator> translatorInterface = skeletonTranslator; return translatorInterface;
}

LRESULT CBrlEdit::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	RegisterMessages();
	CComPtr<IRichEditOle> pRichEditOle;
	::SendMessage(m_hWnd, EM_GETOLEINTERFACE,
		(WPARAM)0, (LPARAM)&pRichEditOle);
	m_pDoc = pRichEditOle;

#ifdef BOOKMARK_SUPPORT
	memset(m_docProperties.bookmark, -1L, sizeof(long)*NUMOFBOOKMARKS);
#endif

	//		s_DefWordBreakProc = (EDITWORDBREAKPROC)
	//			::SendMessage(m_hWnd, 0x00d1 /*EM_GETWORDBREAKPROC*/, 0, 0);
	::SendMessage(m_hWnd, 0x00d0/*EM_SETWORDBREAKPROC*/, 0, (LPARAM)&BrlEditWordBreakProc);
	if (!m_qipBrlTrans)
	{
		m_qipBrlTrans = LoadBrailleTranslator();
		if (!s_BrailleTable.Load(L"us_unicode.jbt"))
			MessageBox(_T("Couldn't load Braille Table"), MB_OK);
	}
	
	TCHAR szLanguage[4];
	GetLocaleInfo(LOCALE_SYSTEM_DEFAULT, LOCALE_SABBREVLANGNAME, szLanguage,
		sizeof(szLanguage) / sizeof(TCHAR));
	if ((szLanguage[0] == _T('e') || szLanguage[0] == _T('E')) &&
		(szLanguage[1] == _T('s') || szLanguage[1] == _T('S')))
		szLanguage[2] = _T('P');
	VARIANT_BOOL langResult;
	m_qipBrlTrans->SetTranslatorLanguage(szLanguage, &langResult);
	return 0;
}

#if 0
LRESULT CBrlEdit::OnContextMenu(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CComPtr<ITextSelection> pSel;
	if (!m_pDoc ||
		FAILED(m_pDoc->GetSelection(&pSel)) ||
		!pSel)
	{
		bHandled = FALSE;
		return 0;
	}

	HMENU hMenu = LoadMenu(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_EDITOR_CONTEXTMENU));
	if (hMenu == NULL)
	{
		bHandled = FALSE;
		return 0;
	}
	HMENU hPopupMenu;
	int cm;
#if 0
	if (IsMarkInRange(pSel))
		cm = cmMark;
	else
#endif
		cm = cmDef;
	hPopupMenu = GetSubMenu(hMenu, cm);
	if (cm == cmDef)
	{
		// fix up the default context menu so it only contains the relevant items
		DeleteBlockItems(hPopupMenu,pSel);
		CComPtr<ITextFont> pFont;
		if (SUCCEEDED(pSel->GetFont(&pFont)) && pFont)
		{
			long val;
			if (pFont->GetBold(&val) == S_OK && val == tomTrue)
				CheckMenuItem(hPopupMenu,ID_FONT_BOLD,MF_BYCOMMAND | MF_CHECKED);
			if (pFont->GetItalic(&val) == S_OK && val == tomTrue)
				CheckMenuItem(hPopupMenu,ID_FONT_ITALIC,MF_BYCOMMAND | MF_CHECKED);
			if (pFont->GetUnderline(&val) == S_OK && val == tomTrue)
				CheckMenuItem(hPopupMenu,ID_FONT_UNDERLINE,MF_BYCOMMAND | MF_CHECKED);
			if (pFont->GetStrikeThrough(&val) == S_OK && val == tomTrue)
				CheckMenuItem(hPopupMenu,ID_FONT_STRIKETHROUGH,MF_BYCOMMAND | MF_CHECKED);
		}
		CComPtr<ITextPara> pPara;
		if (SUCCEEDED(pSel->GetPara(&pPara)) && pPara)
		{
			long lAlignment,lNumbering;
			if (pPara->GetAlignment(&lAlignment) == S_OK)
			{
				if (lAlignment == tomAlignLeft)
					CheckMenuItem(hPopupMenu,ID_PARA_ALIGNLEFT,MF_BYCOMMAND | MF_CHECKED);
				else if (lAlignment == tomAlignCenter)
					CheckMenuItem(hPopupMenu,ID_PARA_ALIGNCENTER,MF_BYCOMMAND | MF_CHECKED);
				else if (lAlignment == tomAlignRight)
					CheckMenuItem(hPopupMenu,ID_PARA_ALIGNRIGHT,MF_BYCOMMAND | MF_CHECKED);
				else if (lAlignment == tomAlignJustify)
					CheckMenuItem(hPopupMenu,ID_PARA_JUSTIFY,MF_BYCOMMAND | MF_CHECKED);
			}
			if (pPara->GetListType(&lNumbering) == S_OK)
			{
				if (lNumbering == tomListNone)
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_NONE,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == tomListNumberAsArabic)
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_NUMBERPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsArabic | tomListPeriod))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_NUMBERPERIOD,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsArabic | tomListParentheses))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENNUMBERPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == tomListNumberAsUCLetter)
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UCPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsUCLetter | tomListPeriod))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UCPERIOD,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsUCLetter | tomListParentheses))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENUCPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == tomListNumberAsLCLetter)
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LCPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsLCLetter | tomListPeriod))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LCPERIOD,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsLCLetter | tomListParentheses))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENLCPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == tomListNumberAsUCRoman)
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsUCRoman | tomListPeriod))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UROMANPERIOD,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsUCRoman | tomListParentheses))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENUROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == tomListNumberAsLCRoman)
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsLCRoman | tomListPeriod))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LROMANPERIOD,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == (tomListNumberAsLCRoman | tomListParentheses))
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENLROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
				else if (lNumbering == tomListBullet)
					CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_BULLET,MF_BYCOMMAND | MF_CHECKED);
			}
		}
	}
	// Display the shortcut menu.
	POINT pt;
	pSel->GetPoint(TA_RIGHT | TA_BOTTOM,&pt.x,&pt.y);
	BOOL bRes = TrackPopupMenu(hPopupMenu,
								TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
								pt.x, pt.y, 0, m_hWnd, NULL);
	if (cm == cmDef)
	{
		switch(bRes)
		{
		case ID_EDIT_CUT:
			CutBlock();
			break;
		case ID_EDIT_COPY:
			CopyBlock();
			break;
		case ID_EDIT_PASTE:
			Paste();
			break;
		case ID_EDIT_DELETE:
			DeleteBlock();
			break;
		case ID_MARK_WORD:
			MarkUnit(tomWord);
			break;
		case ID_MARK_SENTENCE:
			MarkUnit(tomSentence);
			break;
		case ID_MARK_PARA:
			MarkUnit(tomParagraph);
			break;
		case ID_MARK_LINE:
			MarkUnit(tomLine);
			break;
		case ID_MARK_DOC:
			MarkUnit(tomStory);
			break;
		case ID_SELECT_WORD:
			SelectUnit(tomWord);
			break;
		case ID_SELECT_SENTENCE:
			SelectUnit(tomSentence);
			break;
		case ID_SELECT_PARA:
			SelectUnit(tomParagraph);
			break;
		case ID_SELECT_LINE:
			SelectUnit(tomLine);
			break;
		case ID_SELECT_DOC:
			SelectUnit(tomStory);
			break;
		case ID_EDIT_SETMARK:
			SetMarkFromSelection();
			break;
		case ID_EDIT_CLEARMARK:
			ClearMark();
			break;
		case ID_EDIT_MOVETOMARK:
			MoveSelectionToMark();
			break;
		case ID_FONT_BOLD:
			ToggleCharAttributes(CFE_BOLD);
			break;
		case ID_FONT_ITALIC:
			ToggleCharAttributes(CFE_ITALIC);
			break;
		case ID_FONT_UNDERLINE:
			ToggleCharAttributes(CFE_UNDERLINE);
			break;
		case ID_FONT_STRIKETHROUGH:
			ToggleCharAttributes(CFE_STRIKEOUT);
			break;
		// Format/Paragraph menu
		case ID_PARA_ALIGNLEFT:
			SetParaAlignment(tomAlignLeft);
			break;
		case ID_PARA_ALIGNCENTER:
			SetParaAlignment(tomAlignCenter);
			break;
		case ID_PARA_ALIGNRIGHT:
			SetParaAlignment(tomAlignRight);
			break;
		case ID_PARA_JUSTIFY:
			SetParaAlignment(tomAlignJustify);
			break;
		// Format/Paragraph/Numbering menu
		case ID_PARA_NUMBERING_NONE:
			SetParaNumbering(tomListNone);
			break;
		case ID_PARA_NUMBERING_NUMBERPAREN:
			SetParaNumbering(tomListNumberAsArabic);
			break;
		case ID_PARA_NUMBERING_NUMBERPERIOD:
			SetParaNumbering(tomListNumberAsArabic | tomListPeriod);
			break;
		case ID_PARA_NUMBERING_PARENNUMBERPAREN:
			SetParaNumbering(tomListNumberAsArabic | tomListParentheses);
			break;
		case ID_PARA_NUMBERING_UCPAREN:
			SetParaNumbering(tomListNumberAsUCLetter);
			break;
		case ID_PARA_NUMBERING_UCPERIOD:
			SetParaNumbering(tomListNumberAsUCLetter | tomListPeriod);
			break;
		case ID_PARA_NUMBERING_PARENUCPAREN:
			SetParaNumbering(tomListNumberAsUCLetter | tomListParentheses);
			break;
		case ID_PARA_NUMBERING_LCPAREN:
			SetParaNumbering(tomListNumberAsLCLetter);
			break;
		case ID_PARA_NUMBERING_LCPERIOD:
			SetParaNumbering(tomListNumberAsLCLetter | tomListPeriod);
			break;
		case ID_PARA_NUMBERING_PARENLCPAREN:
			SetParaNumbering(tomListNumberAsLCLetter | tomListParentheses);
			break;
		case ID_PARA_NUMBERING_UROMANPAREN:
			SetParaNumbering(tomListNumberAsUCRoman);
			break;
		case ID_PARA_NUMBERING_UROMANPERIOD:
			SetParaNumbering(tomListNumberAsUCRoman | tomListPeriod);
			break;
		case ID_PARA_NUMBERING_PARENUROMANPAREN:
			SetParaNumbering(tomListNumberAsUCRoman | tomListParentheses);
			break;
		case ID_PARA_NUMBERING_LROMANPAREN:
			SetParaNumbering(tomListNumberAsLCRoman);
			break;
		case ID_PARA_NUMBERING_LROMANPERIOD:
			SetParaNumbering(tomListNumberAsLCRoman | tomListPeriod);
			break;
		case ID_PARA_NUMBERING_PARENLROMANPAREN:
			SetParaNumbering(tomListNumberAsLCRoman | tomListParentheses);
			break;
		case ID_PARA_NUMBERING_BULLET:
			SetParaNumbering(tomListBullet);
			break;
		case ID_PARA_INCREASEINDENT:
			IncreaseIndents();
			break;
		case ID_PARA_DECREASEINDENT:
			DecreaseIndents();
			break;
		default:
			break;
		}
	}
	else if (cm == cmMark)
	{
		switch(bRes)
		{
		case ID_MARK_WORD:
			MarkUnit(tomWord);
			break;
		case ID_MARK_SENTENCE:
			MarkUnit(tomSentence);
			break;
		case ID_MARK_PARA:
			MarkUnit(tomParagraph);
			break;
		case ID_MARK_LINE:
			MarkUnit(tomLine);
			break;
		case ID_MARK_DOC:
			MarkUnit(tomStory);
			break;
		default:
			break;
		}
	}
	// Destroy the menu.
	DestroyMenu(hMenu);
	return 0;
}
LRESULT CBrlEdit::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
/*
	if (wParam == VK_F10)
	{
		if (GetKeyState(VK_SHIFT) < 0)
			return OnContextMenu(WM_CONTEXTMENU,0,0,bHandled);
		OnMainMenu();
		return 0;
	}
*/
	if (::GetKeyState(VK_CONTROL) < 0)
	{
		switch (wParam)
		{
		case 'M':
			SetMarkFromSelection();
			break;
		case 'X':
			DebugBreak();
			CutBlock();
			break;
		case 'C':
			CopyBlock();
			break;
		case 'V':
			Paste();
			break;
		case 'D':
			DeleteBlock();
			break;
		default:
			bHandled = FALSE;
			break;
		}
	}
	else
		bHandled = FALSE;
	return 0;
}
void CBrlEdit::OnMainMenu()
{
	CComPtr<ITextSelection> pSel;
	if (!m_pDoc ||
		FAILED(m_pDoc->GetSelection(&pSel)) ||
		!pSel)
		return;
	HMENU hMenu = LoadMenu(_Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_EDITOR_MAINMENU));
	if (hMenu == NULL)
		return;
	HMENU hPopupMenu = GetSubMenu(hMenu, 0);
	DeleteBlockItems(hPopupMenu,pSel,/*BOOL bOnlyDisable*/TRUE);
	CComPtr<ITextFont> pFont;
	if (SUCCEEDED(pSel->GetFont(&pFont)) && pFont)
	{
		long val;
		if (pFont->GetBold(&val) == S_OK && val == tomTrue)
			CheckMenuItem(hPopupMenu,ID_FONT_BOLD,MF_BYCOMMAND | MF_CHECKED);
		if (pFont->GetItalic(&val) == S_OK && val == tomTrue)
			CheckMenuItem(hPopupMenu,ID_FONT_ITALIC,MF_BYCOMMAND | MF_CHECKED);
		if (pFont->GetUnderline(&val) == S_OK && val == tomTrue)
			CheckMenuItem(hPopupMenu,ID_FONT_UNDERLINE,MF_BYCOMMAND | MF_CHECKED);
		if (pFont->GetStrikeThrough(&val) == S_OK && val == tomTrue)
			CheckMenuItem(hPopupMenu,ID_FONT_STRIKETHROUGH,MF_BYCOMMAND | MF_CHECKED);
	}
	CComPtr<ITextPara> pPara;
	if (SUCCEEDED(pSel->GetPara(&pPara)) && pPara)
	{
		long lAlignment,lNumbering;
		if (pPara->GetAlignment(&lAlignment) == S_OK)
		{
			if (lAlignment == tomAlignLeft)
				CheckMenuItem(hPopupMenu,ID_PARA_ALIGNLEFT,MF_BYCOMMAND | MF_CHECKED);
			else if (lAlignment == tomAlignCenter)
				CheckMenuItem(hPopupMenu,ID_PARA_ALIGNCENTER,MF_BYCOMMAND | MF_CHECKED);
			else if (lAlignment == tomAlignRight)
				CheckMenuItem(hPopupMenu,ID_PARA_ALIGNRIGHT,MF_BYCOMMAND | MF_CHECKED);
			else if (lAlignment == tomAlignJustify)
				CheckMenuItem(hPopupMenu,ID_PARA_JUSTIFY,MF_BYCOMMAND | MF_CHECKED);
		}
		if (pPara->GetListType(&lNumbering) == S_OK)
		{
			if (lNumbering == tomListNone)
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_NONE,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == tomListNumberAsArabic)
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_NUMBERPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsArabic | tomListPeriod))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_NUMBERPERIOD,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsArabic | tomListParentheses))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENNUMBERPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == tomListNumberAsUCLetter)
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UCPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsUCLetter | tomListPeriod))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UCPERIOD,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsUCLetter | tomListParentheses))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENUCPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == tomListNumberAsLCLetter)
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LCPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsLCLetter | tomListPeriod))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LCPERIOD,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsLCLetter | tomListParentheses))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENLCPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == tomListNumberAsUCRoman)
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsUCRoman | tomListPeriod))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_UROMANPERIOD,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsUCRoman | tomListParentheses))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENUROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == tomListNumberAsLCRoman)
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsLCRoman | tomListPeriod))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_LROMANPERIOD,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == (tomListNumberAsLCRoman | tomListParentheses))
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_PARENLROMANPAREN,MF_BYCOMMAND | MF_CHECKED);
			else if (lNumbering == tomListBullet)
				CheckMenuItem(hPopupMenu,ID_PARA_NUMBERING_BULLET,MF_BYCOMMAND | MF_CHECKED);
		}
	}
	RECT rc;
	GetWindowRect(&rc);
	BOOL bRes = TrackPopupMenu(hPopupMenu,
								TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD,
								rc.left, rc.top, 0, m_hWnd, NULL);
	switch(bRes)
	{
	// File menu
	case ID_FILE_NEW:
		New();
		break;
	case ID_FILE_OPEN:
		OpenSaveAsDialog(TRUE);
		break;
	case ID_FILE_SAVE:
		Save();
		break;
	case ID_FILE_SAVE_AS:
		OpenSaveAsDialog(FALSE);
		break;
	// Edit menu
	case ID_EDIT_CUT:
		CutBlock();
		break;
	case ID_EDIT_COPY:
		CopyBlock();
		break;
	case ID_EDIT_PASTE:
		Paste();
		break;
	case ID_EDIT_DELETE:
		DeleteBlock();
		break;
	case ID_EDIT_SETMARK:
		SetMarkFromSelection();
		break;
	case ID_EDIT_CLEARMARK:
		ClearMark();
		break;
	case ID_EDIT_MOVETOMARK:
		MoveSelectionToMark();
		break;
	// Edit/Mark menu
	case ID_MARK_WORD:
		MarkUnit(tomWord);
		break;
	case ID_MARK_SENTENCE:
		MarkUnit(tomSentence);
		break;
	case ID_MARK_PARA:
		MarkUnit(tomParagraph);
		break;
	case ID_MARK_LINE:
		MarkUnit(tomLine);
		break;
	case ID_MARK_DOC:
		MarkUnit(tomStory);
		break;
    // Edit/Quick Select menu
	case ID_SELECT_WORD:
		SelectUnit(tomWord);
		break;
	case ID_SELECT_SENTENCE:
		SelectUnit(tomSentence);
		break;
	case ID_SELECT_PARA:
		SelectUnit(tomParagraph);
		break;
	case ID_SELECT_LINE:
		SelectUnit(tomLine);
		break;
	case ID_SELECT_DOC:
		SelectUnit(tomStory);
		break;
#ifndef _WIN32_WCE
	// Edit Find and Replace
	case ID_EDIT_FIND:
		FindReplaceText(TRUE);
		break;
	case ID_EDIT_REPLACE:
		FindReplaceText(FALSE);
		break;
#endif	//  ndef _WIN32_WCE
	// Format/Font menu
	case ID_FONT_BOLD:
		ToggleCharAttributes(CFE_BOLD);
		break;
	case ID_FONT_ITALIC:
		ToggleCharAttributes(CFE_ITALIC);
		break;
	case ID_FONT_UNDERLINE:
		ToggleCharAttributes(CFE_UNDERLINE);
		break;
	case ID_FONT_STRIKETHROUGH:
		ToggleCharAttributes(CFE_STRIKEOUT);
		break;
	// Format/Paragraph menu
	case ID_PARA_ALIGNLEFT:
		SetParaAlignment(tomAlignLeft);
		break;
	case ID_PARA_ALIGNCENTER:
		SetParaAlignment(tomAlignCenter);
		break;
	case ID_PARA_ALIGNRIGHT:
		SetParaAlignment(tomAlignRight);
		break;
	case ID_PARA_JUSTIFY:
		SetParaAlignment(tomAlignJustify);
		break;
	// Format/Paragraph/Numbering menu
	case ID_PARA_NUMBERING_NONE:
		SetParaNumbering(tomListNone);
		break;
	case ID_PARA_NUMBERING_NUMBERPAREN:
		SetParaNumbering(tomListNumberAsArabic);
		break;
	case ID_PARA_NUMBERING_NUMBERPERIOD:
		SetParaNumbering(tomListNumberAsArabic | tomListPeriod);
		break;
	case ID_PARA_NUMBERING_PARENNUMBERPAREN:
		SetParaNumbering(tomListNumberAsArabic | tomListParentheses);
		break;
	case ID_PARA_NUMBERING_UCPAREN:
		SetParaNumbering(tomListNumberAsUCLetter);
		break;
	case ID_PARA_NUMBERING_UCPERIOD:
		SetParaNumbering(tomListNumberAsUCLetter | tomListPeriod);
		break;
	case ID_PARA_NUMBERING_PARENUCPAREN:
		SetParaNumbering(tomListNumberAsUCLetter | tomListParentheses);
		break;
	case ID_PARA_NUMBERING_LCPAREN:
		SetParaNumbering(tomListNumberAsLCLetter);
		break;
	case ID_PARA_NUMBERING_LCPERIOD:
		SetParaNumbering(tomListNumberAsLCLetter | tomListPeriod);
		break;
	case ID_PARA_NUMBERING_PARENLCPAREN:
		SetParaNumbering(tomListNumberAsLCLetter | tomListParentheses);
		break;
	case ID_PARA_NUMBERING_UROMANPAREN:
		SetParaNumbering(tomListNumberAsUCRoman);
		break;
	case ID_PARA_NUMBERING_UROMANPERIOD:
		SetParaNumbering(tomListNumberAsUCRoman | tomListPeriod);
		break;
	case ID_PARA_NUMBERING_PARENUROMANPAREN:
		SetParaNumbering(tomListNumberAsUCRoman | tomListParentheses);
		break;
	case ID_PARA_NUMBERING_LROMANPAREN:
		SetParaNumbering(tomListNumberAsLCRoman);
		break;
	case ID_PARA_NUMBERING_LROMANPERIOD:
		SetParaNumbering(tomListNumberAsLCRoman | tomListPeriod);
		break;
	case ID_PARA_NUMBERING_PARENLROMANPAREN:
		SetParaNumbering(tomListNumberAsLCRoman | tomListParentheses);
		break;
	case ID_PARA_NUMBERING_BULLET:
		SetParaNumbering(tomListBullet);
		break;
	case ID_PARA_INCREASEINDENT:
		IncreaseIndents();
		break;
	case ID_PARA_DECREASEINDENT:
		DecreaseIndents();
		break;
	case ID_TOOLS_SPELLCHECK:
		PostMessage(BEM_SPELLCHECK,0,0);
		break;
	default:
		break;
	}
	// Destroy the menu.
	DestroyMenu(hMenu);
}
void CBrlEdit::DeleteBlockItems(HMENU hPopupMenu,ITextSelection *pSel,BOOL bOnlyDisable)
{
	// Deletes/disables those menu items related to block operations
	// hPopupMenu Menu handle.
	// bOnlyDisable = FALSE grays out the items, = TRUE deletes the items
	if (!IsMarkSet())
	{
		if (bOnlyDisable)
		{
			EnableMenuItem(hPopupMenu,ID_EDIT_CLEARMARK,MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hPopupMenu,ID_EDIT_MOVETOMARK,MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			DeleteMenu(hPopupMenu,ID_EDIT_CLEARMARK,MF_BYCOMMAND);
			DeleteMenu(hPopupMenu,ID_EDIT_MOVETOMARK,MF_BYCOMMAND);
		}
	}
	else if (IsMarkInRange(pSel))
	{
		if (bOnlyDisable)
		{
			EnableMenuItem(hPopupMenu,ID_EDIT_SETMARK,MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hPopupMenu,ID_EDIT_MOVETOMARK,MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			DeleteMenu(hPopupMenu,ID_EDIT_SETMARK,MF_BYCOMMAND);
			DeleteMenu(hPopupMenu,ID_EDIT_MOVETOMARK,MF_BYCOMMAND);
		}
	}
	if (!IsMarkSet() ||
		IsMarkInRange(pSel))
	{
		// nothing can be cut, copied or deleted
		if (bOnlyDisable)
		{
			EnableMenuItem(hPopupMenu,ID_EDIT_CUT,MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hPopupMenu,ID_EDIT_COPY,MF_BYCOMMAND | MF_GRAYED);
			EnableMenuItem(hPopupMenu,ID_EDIT_DELETE,MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			DeleteMenu(hPopupMenu,ID_EDIT_CUT,MF_BYCOMMAND);
			DeleteMenu(hPopupMenu,ID_EDIT_COPY,MF_BYCOMMAND);
			DeleteMenu(hPopupMenu,ID_EDIT_DELETE,MF_BYCOMMAND);
		}
	}
	else
	{
		// delete the Mark submenu since there is already a marked block
		HMENU hParent;
		UINT uPosition;
		if (FindMarkSubmenu(hPopupMenu,&hParent,&uPosition))
		{
			if (bOnlyDisable)
				EnableMenuItem(hParent,uPosition,MF_BYPOSITION | MF_GRAYED);
			else
				DeleteMenu(hParent,uPosition,MF_BYPOSITION);
		}
	}
}
BOOL CBrlEdit::FindMarkSubmenu(HMENU hPopupMenu,HMENU *phParent,UINT *puPosition)
{
	/*
	Searches for the Mark submenu.
	HMENU hPopupMenu Starting point
	HMENU *phParent will be a menu containing the Mark submenu
	UINT *puPosition is the zero-based position of the Mark submneu in *phParent
	Returns TRUE if the Mark submenu is found, FALSE otherwise.
	*/
#if 0
#ifndef _WIN32_WCE
	ATLASSERT(::IsMenu(hPopupMenu));
	if (!::IsMenu(hPopupMenu))
		return FALSE;

#endif	//  ndef _WIN32_WCE
	HMENU hMenu;
	TCHAR lpString[128];
	for (int i=0;i<10;i++)
	{
		if (hMenu = GetSubMenu(hPopupMenu,i))
		{
			if (GetMenuString(hPopupMenu,i,
				lpString,sizeof(lpString)/sizeof(TCHAR),MF_BYPOSITION) &&
				lstrcmp(lpString,"&Mark") == 0)
			{
				*phParent = hPopupMenu;
				*puPosition = i;
				return TRUE;
			}
			else
			{
				if (FindMarkSubmenu(hMenu,phParent,puPosition))
					return TRUE;
			}
		}
	}
        #endif
	return FALSE;
}
HRESULT CBrlEdit::GetFormatInfo(long lpBuf,long flags)
{
	DWORD dwAttribs = 0;
	CComPtr<ITextRange> pBlock;
	HRESULT hres;
	if (FAILED(hres = GetBlock(&pBlock)) || !pBlock)
	{
		CComPtr<ITextSelection> pSel;
		if (m_pDoc && SUCCEEDED(hres = m_pDoc->GetSelection(&pSel)) && pSel)
			pBlock = pSel;
	}
	if (FAILED(hres) || !pBlock)
		return hres;
	if (flags & fitFont)
	{
		CComPtr<ITextFont> pFont;
		if (FAILED(hres = pBlock->GetFont(&pFont)) || !pFont)
			return hres;
		long val;
		if (pFont->GetBold(&val) == S_OK && val == tomTrue)
			dwAttribs |= CFE_BOLD;
		if (pFont->GetItalic(&val) == S_OK && val == tomTrue)
			dwAttribs |= CFE_ITALIC;
		if (pFont->GetUnderline(&val) == S_OK && val == tomTrue)
			dwAttribs |= CFE_UNDERLINE;
		if (pFont->GetStrikeThrough(&val) == S_OK && val == tomTrue)
			dwAttribs |= CFE_STRIKEOUT;
		((BrlEditFormat*)lpBuf)->dwAttribs = dwAttribs;
	}
	if (flags & fitPara)
	{
		CComPtr<ITextPara> pPara;
		if (FAILED(hres = pBlock->GetPara(&pPara)) || !pPara)
			return hres;
		long lAlignment, lNumbering;
		if (SUCCEEDED(pPara->GetAlignment(&lAlignment)))
			((BrlEditFormat*)lpBuf)->lAlignment = lAlignment;
		if (SUCCEEDED(pPara->GetListType(&lNumbering)))
			((BrlEditFormat*)lpBuf)->lNumbering = lNumbering;
	}
	return S_OK;
}
#endif
//EDITWORDBREAKPROC CBrlEdit::s_DefWordBreakProc = 0;
int CALLBACK CBrlEdit::BrlEditWordBreakProc(LPTSTR lpch,
											int ichCurrent,int cch,int code)
{
//	if (s_DefWordBreakProc)
//		return s_DefWordBreakProc(lpch,ichCurrent,cch,code);
	TCHAR *lpCurrentChar;
	TCHAR szDelimiters [20];
	if (CBrlEdit::m_docProperties.brailleGrade2)
		_tcscpy_s(szDelimiters, _T("[]72314680'"));
	else
		_tcscpy_s(szDelimiters, _T("[]();:,.?!\"'"));

	int nIndex;
	int nRet = 0;
//	int nLastAction;
	switch (code)
	{
	case 2://WB_ISDELIMITER:
		// Windows sends this code so that the wordbreak function can
		// check to see if the current character is the delimiter.
		// If so, return TRUE. This will cause a line break at the '\32'
		// character.
		if (lpch[ichCurrent] == _T(' ') ||
			lpch[ichCurrent] == _T('\t'))
			return TRUE;
		else
			return FALSE;
		break;
		// Because we have replaced the default wordbreak procedure, our
		// wordbreak procedure must provide the other standard features in
		// edit controls.
	case 0://WB_LEFT:
	case WB_MOVEWORDLEFT://		4
		// Windows sends this code when the user enters CTRL+LEFT ARROW.
		// The wordbreak function should scan the text buffer for the
		// beginning of the word from the current position and move the
		// caret to the beginning of the word.
		{
			BOOL bCharFound = FALSE;
			lpCurrentChar = lpch + ichCurrent;
			nIndex = ichCurrent;
			while (nIndex > 0 &&
				(*(lpCurrentChar-1) != _T(' ') &&
				*(lpCurrentChar-1) != 0x0d &&
				*(lpCurrentChar-1) != _T('\t')) ||
				!bCharFound)
			{
				lpCurrentChar--;
				nIndex--;
				if (*(lpCurrentChar) != _T(' ') &&
					*(lpCurrentChar) != 0x0d &&
					*(lpCurrentChar-1) != _T('\t'))
					// We have found the last char in the word. Continue
					// looking backwards till we find the first char of
					// the word.
				{
					bCharFound = TRUE;
					// We will consider a CR the start of a word.
					if (*(lpCurrentChar) == 0x0D)
						break;
				}
			}
			return nIndex;
		}
		break;
	case 1://WB_RIGHT:
	case WB_MOVEWORDRIGHT://	5
		//Windows sends this code when the user enters CTRL+RIGHT ARROW.
		//The wordbreak function should scan the text buffer for the
		//beginning of the word from the current position and move the
		//caret to the end of the word.
		for (lpCurrentChar = lpch+ichCurrent, nIndex = ichCurrent;
			nIndex < cch;
			nIndex++, lpCurrentChar++)
		{
			if (*lpCurrentChar == _T(' ') ||
				*lpCurrentChar == 0x0d ||
				*lpCurrentChar == _T('\t'))
			{
				lpCurrentChar++;
				nIndex++;
				while (*lpCurrentChar == _T(' ') ||
					*lpCurrentChar == 0x0d ||
					*lpCurrentChar == _T('\t'))
				{
					lpCurrentChar++;
					nIndex++;
				}
				return nIndex;
			}
		}
		return cch;
		break;
	case WB_LEFTBREAK://		6
		// Finds the next word end before the specified position.
		// This value is the same as WB_PREVBREAK.
		{
			lpCurrentChar = lpch + ichCurrent;
			nIndex = ichCurrent;
			while (nIndex > 0 &&
				*lpCurrentChar != _T(' ') &&
				*lpCurrentChar != 0x0d &&
				*lpCurrentChar != _T('\t'))
			{
				lpCurrentChar--;
				nIndex--;
			}
/*
			while (nIndex > 0 &&
				(*lpCurrentChar == _T(' ') ||
				*lpCurrentChar == 0x0d ||
				*lpCurrentChar == _T('\t')))
			{
				lpCurrentChar--;
				nIndex--;
			}
*/
			return nIndex;
		}
		break;
	case WB_RIGHTBREAK://		7
		// Finds the next end-of-word delimiter after the specified position. This value is the same as WB_NEXTBREAK.
		for (lpCurrentChar = lpch+ichCurrent, nIndex = ichCurrent;
			nIndex < cch;
			nIndex++, lpCurrentChar++)
		{
			if (*lpCurrentChar == _T(' ') ||
				*lpCurrentChar == 0x0d ||
				*lpCurrentChar == _T('\t'))
				return nIndex;
		}
		return cch;
		break;
	case WB_CLASSIFY://			3
		// Returns the character class and word-break flags of the character at the specified position.

		// Whitespace
		if (_tcschr(_T(" \t"), lpch[ichCurrent]))
		{
			nRet = WBF_ISWHITE | 0x01;
		}
		// End of a word or line
		else if ( (_tcschr(_T("\r\n"), lpch[ichCurrent])) ||
				  (_tcschr(_T(" \t"), lpch[ichCurrent+1])) ||
				  (ichCurrent == (cch-1)) )
		{
			nRet = WBF_BREAKLINE | 0x01;
		}
		// Delimiter
		else if (_tcschr(szDelimiters, lpch[ichCurrent]))
		{
			// If the character matched is "468", text is in Grade 2.
			// For this to evaluate to a sentence delimeter (".!?"), white space must
			// follow this character.
			if (_tcschr(_T("468'"), lpch[ichCurrent]))
			{
				if (_tcschr(_T(" \t\r\n"), lpch[ichCurrent+1]))
				{
					nRet = WBF_BREAKAFTER | 0x01;
				}
			}
			else
			{
				nRet = WBF_BREAKAFTER | 0x01;
			}
		}
		// All other characters.
		else
		{
			nRet = 0;
		}
		return nRet;
		break;
	default:
//		return s_DefWordBreakProc(lpch,ichCurrent,cch,code);
		return 0;
		break;
	}
}
void CBrlEdit::RegisterMessages()
{
}

BOOL CBrlEdit::SetComPortEmbossing( HANDLE hPort, int iBaud, int iBufSize)
{
	COMMTIMEOUTS CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0xffffffff;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 10;
	CommTimeouts.WriteTotalTimeoutConstant = 1000;

	if( !SetCommTimeouts( hPort, &CommTimeouts))
	{
		return FALSE;
	}

	DCB dcb;
	dcb.DCBlength=sizeof(struct _DCB);
	dcb.BaudRate = iBaud;
	dcb.fBinary = TRUE;
	dcb.fParity = NOPARITY; // for receiving to PC
	dcb.fOutxCtsFlow = FALSE; // for hardware control
	dcb.fOutxDsrFlow = FALSE; // for hardware control
	dcb.fDtrControl = DTR_CONTROL_DISABLE; // for hardware control
	dcb.fDsrSensitivity = FALSE; // for hardware control
	dcb.fTXContinueOnXoff = TRUE; // for software control
	dcb.fOutX = TRUE; // for software control
	dcb.fInX = TRUE; // for software control
	dcb.fRtsControl=RTS_CONTROL_DISABLE; // for hardware control
	dcb.fAbortOnError = FALSE;
	dcb.XonLim=static_cast<WORD>(iBufSize/4); // for software control
	dcb.XoffLim= static_cast<WORD>(iBufSize/4); // for software control
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.XonChar = 0x11;  // for software control
	dcb.XoffChar = 0x13;  // for software control

	if( !SetCommState( hPort, &dcb))
		return FALSE;

	return TRUE;
}

int CBrlEdit::WriteToPort( LPCTSTR lpszFileName, LPTSTR lpszPort, int iBaud)
{
	BOOL bComPort = FALSE;
	BOOL bNetwork = TRUE;
	TCHAR cPortHold[ MAX_PATH * 4 ];
	_tcscpy_s( cPortHold, lpszPort);

	//  the naming conventions \\.\<PORT>  or <PORT>: are both acceptable for Windows.
	if( _tcsnicmp( lpszPort, _T("\\\\.\\"), 4) == 0 || ((_tcsnicmp(lpszPort, _T("COM"),3 ) == 0 || _tcsnicmp(lpszPort, _T("LPT"),3 ) == 0) && _tcslen( lpszPort) < 7 ))
	{ // it is a COM or LPT port
		bNetwork = FALSE;
		if( _tcsstr( lpszPort, _T("COM")) != NULL )
		{
			bComPort = TRUE;
		}
	}
	HANDLE hPort = CreateFile( cPortHold, GENERIC_WRITE,	0, 0, OPEN_EXISTING, 0, NULL );
	if ( hPort == INVALID_HANDLE_VALUE )
	{
		CloseHandle( hPort );
		return 0;
	}

	HANDLE hFile = CreateFile( lpszFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, 0, NULL );
	if ( hFile==INVALID_HANDLE_VALUE)
	{
		CloseHandle( hPort );
		CloseHandle( hFile );
		return -3;
	}

	COMMTIMEOUTS OldCommTimeouts;
	DCB Old_dcb;
	DWORD dwd = GetFileSize( hFile, NULL ); // size in bytes
	if ( !bNetwork )
	{// Get Port information so can restore later
		if (!GetCommTimeouts( hPort, &OldCommTimeouts))
		{
			CloseHandle( hPort );
			CloseHandle( hFile );
			return 0;
		}

		if ( bComPort )
		{
			if(!GetCommState( hPort, &Old_dcb) || !SetComPortEmbossing( hPort, iBaud, dwd))
			{
				CloseHandle( hPort );
				CloseHandle( hFile );
				return 0;
			}
		}
		else
		{
			if(!SetLPTPortEmbossing( hPort ))
			{
				CloseHandle( hPort );
				CloseHandle( hFile );
				return 0;
			}
		}
	}

	PurgeComm( lpszPort,  PURGE_TXCLEAR | PURGE_RXCLEAR);

	int nNumBufs = dwd/MAX_BUF;
	int nLastBuf = dwd%MAX_BUF;
	LPSTR lpszBuffer = NULL;
	BOOL bWSuccess, bRSuccess;
	int iNumberBytes = 0;
	unsigned long ulNumberToWrite = 0, ulNumberWritten = 0;
	for(int i = 0; i < nNumBufs; i++)
	{
		lpszBuffer = new char[MAX_BUF];
		if ( !lpszBuffer )
		{
			CloseHandle( hPort );
			CloseHandle( hFile );
			return -3;
		}
		ZeroMemory(lpszBuffer, MAX_BUF);
		bWSuccess = bRSuccess = FALSE;
		iNumberBytes = MAX_BUF;
		ulNumberToWrite = ulNumberWritten = 0;

		bRSuccess = ReadFile(hFile, lpszBuffer, iNumberBytes, &ulNumberToWrite, NULL);
		if(ulNumberToWrite > 0 && bRSuccess)
		{
			if(!WriteFile(hPort, lpszBuffer, ulNumberToWrite, &ulNumberWritten, NULL))
			{
				ATLASSERT(0);
			}
			else
				bWSuccess = 1;
		}

		delete [] lpszBuffer;
		lpszBuffer = NULL;
		CloseHandle(hPort);

		Sleep(20000);

		hPort = CreateFile(lpszPort, GENERIC_WRITE,	0, 0, OPEN_EXISTING, 0, NULL);
		if(hPort == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hPort);
			CloseHandle(hFile);
			return 0;
		}
	}

	lpszBuffer = new char[nLastBuf];
	if( !lpszBuffer )
	{
		CloseHandle(hPort);
		CloseHandle(hFile);
		return -3;
	}
	ZeroMemory(lpszBuffer, nLastBuf);
	bWSuccess = bRSuccess = FALSE;
	iNumberBytes = nLastBuf;
	ulNumberToWrite = ulNumberWritten = 0;

	bRSuccess = ReadFile(hFile, lpszBuffer, iNumberBytes, &ulNumberToWrite, NULL);
	if(ulNumberToWrite > 0 && bRSuccess)
	{
		if(!WriteFile(hPort, lpszBuffer, ulNumberToWrite, &ulNumberWritten, NULL))
		{
			ATLASSERT(0);
		}
		else
			bWSuccess = 1;
	}

	delete [] lpszBuffer;

	if ( !bNetwork )
	{
		// Restore Port information
		if ( !SetCommTimeouts( hPort, &OldCommTimeouts) )
		{
			CloseHandle( hPort );
			CloseHandle( hFile );
			return 0;
		}
		if ( bComPort )
		{
			if ( !SetCommState( hPort, &Old_dcb) )
			{
				CloseHandle( hPort );
				CloseHandle( hFile );
				return 0;
			}
		}
	}

	CloseHandle( hPort );
	CloseHandle( hFile );

	if ( bWSuccess < 1)
	{
		return 0;
	}
	return bWSuccess;
}

BOOL CBrlEdit::SetLPTPortEmbossing( HANDLE hPort )
{
	COMMTIMEOUTS CommTimeouts;
	CommTimeouts.ReadIntervalTimeout = 0;
	CommTimeouts.ReadTotalTimeoutMultiplier = 0;
	CommTimeouts.ReadTotalTimeoutConstant = 0;
	CommTimeouts.WriteTotalTimeoutMultiplier = 0;
	CommTimeouts.WriteTotalTimeoutConstant = 3000;

	if( !SetCommTimeouts( hPort, &CommTimeouts))
	{
		return FALSE;
	}
	return TRUE;
}


long CBrlEdit::StripFormFeeds(LPBYTE pbBuff,long nLen)
	{
		// Check for form feeds in string
		int nIdx;
		for (nIdx=0;nIdx <nLen;nIdx++)
			if (pbBuff[nIdx] == '\f')
				break;
		if (nIdx == nLen)
			return nLen;							// none found
		// Make a temporary copy of the buffer to allow removing form
		// feeds from the original
		LPBYTE pbyTemp = new BYTE[nLen];
		memcpy(pbyTemp,pbBuff	,nLen);
		int nNewLen = 0;
		for (nIdx = 0;nIdx <nLen;nIdx++)
			if (pbyTemp[nIdx] != '\f')
				pbBuff[nNewLen++] = pbyTemp[nIdx];
		delete [] pbyTemp;
		return nNewLen;
	}

