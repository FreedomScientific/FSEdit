// splchck.cpp
// Last modified: 11/21/2002 AG

#include "stdafx.h"
#include "splchck.h"
#include "SpellChkDlg.h"
#include <richedit.h>

CSpellChecker::CSpellChecker(int clex, WCHAR** ppwsz)
	: CSpl(clex, ppwsz)
{
	m_document = NULL;
	m_nStop		= 0;
}

CSpellChecker::CSpellChecker()
	: CSpl()
{
	m_document = NULL;
	m_nStop		= 0;
} 

CSpellChecker::~CSpellChecker()
{
	if (m_document != NULL)
		delete [] m_document;
}

int CSpellChecker::DoChecking(CFSEditView &view)
{
	// determines if the current document is a grade 2 document
	DocProp dp;
	view.GetDocumentProp(dp);
	CComQIPtr<IBrailleTranslator> qipBrlTrans;
	if (dp.brailleGrade2)
		view.GetBrlTrans(&qipBrlTrans);

	CSplBuffer splBuf;
	long nErrStart=0, nErrEnd=0;

	// Check if there is selected text.  If so, spell check only 
	// selected text.  Will ask user if they want to continue checking
	// the rest of the document.
	CHARRANGE cr;
	view.GetSel(cr);
	//NKDbgPrintfW( _T("CSpellChecker::DoChecking cpMin: %d, cpMax: %d\r\n"), cr.cpMin, cr.cpMax);
	//NKDbgPrintfW( _T("CSpellChecker::DoChecking initial m_nStop: %d(0x%X)\r\n"), m_nStop, m_nStop);
	if (cr.cpMin != cr.cpMax) 
	{
		m_nStop   = cr.cpMax;
	}

	nErrStart = cr.cpMin;	// start of marked area
	nErrEnd   = cr.cpMax;
	
	//NKDbgPrintfW( _T("CSpellChecker::DoChecking m_nStop %d\r\n"), m_nStop);

	int result = SPL_OK;

	if(FindError(view,qipBrlTrans,splBuf,nErrStart,nErrEnd))
	{
		CSpellDlg dlg(view, qipBrlTrans, *this, splBuf, nErrStart, nErrEnd);
		result =static_cast<int>(dlg.DoModal());
	}

	return(result);
}
/*
	Function name: GetSuggestions
	Parameters:
	CSplBuffer *psplBuf, // points to an SPLBUFFER containing error
	vector<CString> &vec // will hold the suggestions
	Purpose: gets suggestions for the error contained in psplBuf
	Return value: true if function succeeds, false otherwise.
*/
BOOL CSpellChecker::GetSuggestions(const CSplBuffer* psplBuf, vector<CString>& vec)
{
	CSplBuffer splSuggest;
	splSuggest.Set(psplBuf->GetError(),psplBuf->GetErrorLength());
//	splSuggest.dwMode = SPL_STARTS_SENTENCE;
	int nResult = Suggest(&splSuggest);

	for(int i = 0; i < splSuggest.cspl; i++)
		vec.push_back(splSuggest.aspl[i].pwsz);

	return nResult;
}

/*
	Function name: FindError
	Parameters:
		CSkipperView &view - in: the current document view
		IBrailleTranslator *pBrlTrans - in: Braille translation of text
		CSplBuffer &splBuf - in/out
		long &nStart - in/out
		long &nEnd - in/out
	Purpose: Find words that are not in the spelling dictionary
*/
BOOL CSpellChecker::FindError(CFSEditView &view,IBrailleTranslator *pBrlTrans, // IN parameters
		CSplBuffer &splBuf,long &nStart,long &nEnd) // IN/OUT parameters
{
	TEXTRANGE trTemp;
	int iLength;
	int nChars = view.GetTextLengthEx(GTL_PRECISE | GTL_NUMCHARS,1200);
	//NKDbgPrintfW( _T("CSpellChecker::FindError %d chars, marked from %d to %d\r\n"), nChars, nStart, nEnd);
	if (nChars == E_INVALIDARG ||
		nStart < 0 || nStart >= nChars)
		return FALSE;

	if ( m_document != NULL)
		delete [] m_document;
	m_document = new TCHAR [nChars + 2];
	if (!m_document) 
		return(FALSE);

	if ( nStart == nEnd)	// no marked area
		nEnd = nChars;

	trTemp.chrg.cpMin = nStart;
	trTemp.chrg.cpMax = nEnd;
	trTemp.lpstrText = m_document;
	iLength = view.GetTextRange(&trTemp);
	//NKDbgPrintfW( _T("CSpellChecker::FindError GettextRange: %d\r\n"), iLength);
	if (iLength <= 0)	// Error code or Zero Length String returned from GetTextRange()
		return(FALSE);

	while(nStart < nChars)
	{
		if (pBrlTrans)
			nEnd = FindBrailleWordBreak(WB_MOVEWORDRIGHT, nStart);
		else
			nEnd = view.FindWordBreak(WB_MOVEWORDRIGHT, nStart);

		if (nEnd <= nStart || nEnd > nChars+1) // nEnd may be equal to nChars+1
			// in the last word of the document.
			break;

		// If a stopping index point was passed in, stop spell check there.
		//NKDbgPrintfW( _T("CSpellChecker::FindError nStart: %d, nEnd: %d, m_nStop: %d\r\n"), nStart, nEnd, m_nStop);
		if ((m_nStop != 0) && (nEnd > m_nStop+1)) 
		{
			CString strTitle, strMsg;
			strTitle.LoadString(IDS_SPELLCHECKER);
			strMsg.LoadString(IDS_CONTINUE_SPELL_CHECK);
			if (MessageBox(view.m_hWnd, strMsg, strTitle, MB_YESNO|MB_ICONQUESTION) == IDNO)
				break;
			m_nStop = 0;	// indicate finished with marked region
		}

		int nLen = nEnd-nStart+1;
		// Who frees this????  is there a memory leak here ???
		LPTSTR lpszText = (LPTSTR) _alloca(sizeof(TCHAR)*nLen);
		if (!lpszText)
			break;

		trTemp.chrg.cpMin = nStart;
		trTemp.chrg.cpMax = nEnd;
		trTemp.lpstrText = lpszText;
		iLength = view.GetTextRange(&trTemp);
		if (iLength <= 0)
			break;

		if (pBrlTrans) // we need to backtranslate the line
			// before checking the spelling
		{
			CComBSTR bstrTranslated;
			pBrlTrans->BackTranslate(lpszText,&bstrTranslated);
			if (bstrTranslated)
				splBuf.Set(bstrTranslated,bstrTranslated.Length());
		}
		else
		{
			splBuf.Set(lpszText,lstrlen(lpszText));
		}

		int nCheck = Check(&splBuf);
		//NKDbgPrintfW( _T("CSpellChecker::FindError Check of '%s': %s, rc: %d\r\n"), lpszText, (splBuf.dwErr == SPL_OK ? _T("OK") : _T("ERR")), nCheck);
		if(nCheck)
		{
			if(splBuf.dwErr != SPL_OK)
				return TRUE; // we found a word that is not in the dictionary
		}
		nStart = nEnd;
	}

	return FALSE;
}

DWORD CSpellChecker::FindBrailleWordBreak(UINT nCode, DWORD nStart)
{
	nStart++;

	while (m_document[nStart] != TCHAR(0))
	{
		switch (m_document[nStart])
		{
		case TCHAR('\r'):
		case TCHAR('\n'):
		case TCHAR('\t'):
		case TCHAR(' '):
			return(nStart);
		default:
			nStart++;
			break;
		}
	}
	return(nStart);
}
