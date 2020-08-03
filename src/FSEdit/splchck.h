#ifndef CE_SPELL_CHECKER_INCLUDED
#define CE_SPELL_CHECKER_INCLUDED
#pragma once

#include "resource.h"
#include "FSEditView.h"
#include <vector>
#include <splapi.h>
// The above is a no-op on desktop Windows. It remains because things don't currently compile without it. But Spell checking won't work until HunSpell or similar repalces this.
// Wrapper class for the SPLBUFFER struct
class CSplBuffer : public SPLBUFFER
{
public:
	CSplBuffer()
	{
		Clear();
	}
	CSplBuffer(const SPLBUFFER &splBuf)
	{
		Set(splBuf.pwszIn,splBuf.cwcIn,splBuf.iwcIn,splBuf.dwMode,
			splBuf.pwszOut,splBuf.cwcOut,splBuf.cwcUsed,
			splBuf.aspl,splBuf.cspl,
			splBuf.dwErr,splBuf.iwcErr,splBuf.cwcErr);
	}
	~CSplBuffer()
	{
		Free();
	}
	void Clear()
	{
		pwszIn=NULL;			// Text to be processed
		pwszOut=NULL;		// Results written here
		memset(aspl,0,sizeof(aspl));		// Information about results
		iwcIn=0;			// Start processing at this index
		cwcIn=0;			// Input buffer is this size
		dwMode=SPL_NO_STATE_INFO;			// Processing mode
		cwcOut=0;			// Output buffer is this size
		cwcUsed=0;		// This much was used
		dwErr=SPL_OK;			// What type of error was detected
		iwcErr=0;			// Where the error occurs
		cwcErr=0;			// How long the error is
		cspl=0;			// How many suggestions were made
	}
	void Free()
	{
		delete (WCHAR*)pwszIn;
		delete pwszOut;
	}
	void FreeAndClear() { Free(); Clear(); }
	void Set(LPCWSTR pwText,DWORD dwTextSize,
		DWORD dwStartProcessingAt=0,DWORD dwProcessingMode=SPL_NO_STATE_INFO,
		LPCWSTR pwSuggestBuf=0,DWORD dwSuggestBufSize=0,DWORD dwSuggestBufUsed=0,
		const SPLSUGGEST *pSuggestionScores=0,DWORD dwHowManySuggestions=0,
		DWORD dwErrorType=SPL_OK,DWORD dwErrorOffset=0,DWORD dwErrorLength=0)
	{
		FreeAndClear();
		
		LPWSTR pw=new WCHAR[dwTextSize+1]; // always allocate 1 extra char for the null terminator
		if (pw)
		{
			if (pwText)
			{
				memcpy(pw,pwText,dwTextSize*sizeof(WCHAR));
				pw[dwTextSize] = L'\0';
			}
			else
				memset(pw,0,(dwTextSize+1)*sizeof(WCHAR));
		}
		pwszIn=pw;
		cwcIn=dwTextSize;			// Input buffer is this size

		iwcIn=dwStartProcessingAt;			// Start processing at this index
		dwMode=dwProcessingMode;

		DWORD dwOutLen = max(128,dwSuggestBufSize);
		pwszOut=new WCHAR[dwOutLen];		// Results written here
		if (pwszOut)
		{
			memset(pwszOut,0,dwOutLen*sizeof(WCHAR));
			if (pwSuggestBuf && 
				dwSuggestBufUsed > 0 && dwSuggestBufUsed <= dwOutLen)
				memcpy(pwszOut,pwSuggestBuf,dwSuggestBufUsed*sizeof(WCHAR));
		}
		cwcOut=dwOutLen;			// Output buffer is this size
		cwcUsed=dwSuggestBufUsed;
	
		if (pSuggestionScores && dwHowManySuggestions > 0 && pwSuggestBuf && 
			pwszOut)
		{
			/* Each SPLSUGGEST in the array aspl should contain a pointer to 
			the beginning of each suggestion in the null separated, double null
			terminated buffer pwszOut. We assume that the parameters 
			pSuggestionScores and pwSuggestBuf follow the same structure. 
			So, we have to translate pointers to offsets in pSuggestBuf to 
			pointers to the equivalent offsets in pwszOut. */
			for (DWORD i=0; i < dwHowManySuggestions; i++)
			{
				aspl[i].pwsz = pwszOut + 
								(pSuggestionScores[i].pwsz - pwSuggestBuf);
				aspl[i].nScore = pSuggestionScores[i].nScore;
			}
		}
		cspl = dwHowManySuggestions;

		dwErr = dwErrorType;
		iwcErr = dwErrorOffset;
		cwcErr = dwErrorLength;
	}
	BOOL HasValidError() const
	{
		return (pwszIn && dwErr != SPL_OK);
	}
	LPCTSTR GetError() const
	{
		if (!HasValidError())
			return NULL;
		return pwszIn + iwcErr;
	}
	DWORD GetErrorLength() const
	{
		return cwcErr;
	}
	DWORD GetErrorEnd() const
	{
		return iwcErr + cwcErr;
	}
};
// Wrapper class for the CE spell checker API.
class CSpl
{
public:
	CSpl() : m_hspl(NULL){} 
	CSpl(int clex, WCHAR** ppwsz)
	{
	}
	virtual ~CSpl()
	{ }
		
	int Init(int nNumDict, WCHAR** ppwsz) { return 0; }
	int SetOptions(DWORD dwOptions) { return 0; }
	int GetOptions(DWORD& rdwOptions) { return 0;  }
	int SetLimit(DWORD dwLimit) {return 0;}
	int GetLimit(DWORD *pdwLimit) {return 0;}
	int Check(SPLBUFFER* splBuff) { return 0; }
	int Suggest(SPLBUFFER* splBuff) {return 0;}
	int Replace(SPLBUFFER& splBuf) { return 0; }
	int AddDict(int nID, const WCHAR *pwszAdd, const WCHAR *pwszChange) {return 0;}
	int RemoveDict(int nID, const WCHAR *pwsz) {return 0;}
	int ClearDict(int nID) {return 0;}
	int SizeDict(int nID, DWORD *pdw) {return 0;}
	int EnumDict(int nID, DWORD nIndex, SPLBUFFER* splBuff) { return 0; }
	BOOL IsNull(){return m_hspl == NULL;}
protected:
	HSPL m_hspl;
};
class CSpellChecker : public CSpl
{
public:
	int DoChecking(CFSEditView &view);
	BOOL FindError(CFSEditView &view,IBrailleTranslator *pBrlTrans, // IN parameters
		CSplBuffer &splBuf, // IN/OUT: last error found
		long &nErrStart, // IN: at which index the checking should start. 
						// OUT: the offset of the error found 
		long &nErrEnd // OUT: error end
		);
	CSpellChecker();
	CSpellChecker(int clex, WCHAR** ppwsz);
	virtual ~CSpellChecker();
	BOOL GetSuggestions(const CSplBuffer* psplBuf, vector<CString>& vec);

	long m_nStop;	// Index of where to stop spell check if user specifically 
					// selected text for spell checking.
	LPTSTR m_document;

protected:
	DWORD FindBrailleWordBreak(UINT nCode, DWORD nStart);
};
#endif // ifndef CE_SPELL_CHECKER_INCLUDED
