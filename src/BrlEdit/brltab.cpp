#include "stdafx.h"
#include "brltab.h"

static inline bool IsUnicodeBraille(WCHAR ch)
{
	return (ch >= 0x2800 && ch <= 0x28ff);
}

BOOL CBrailleTable::Load(LPCWSTR lpszFile)
{
	if (!lpszFile || !*lpszFile)
		return FALSE;
	if (_wcsicmp(lpszFile,szFileName) == 0)
		return TRUE;              // reloading current table
	else
		StringCbCopyW(szFileName,sizeof(szFileName),lpszFile);
	BOOL fResult = FALSE;

#ifdef _WIN32_WCE
	HANDLE hFile = CreateFileW(lpszFile,
	                          GENERIC_READ,
	                          FILE_SHARE_READ | FILE_SHARE_WRITE,
	                          NULL, // security attributes
	                          OPEN_EXISTING,
	                          0,
	                          NULL);
#else
	USES_CONVERSION;
	// We use CreateFileA here  to make sure that we can run on Windows
	// 9X.  9X doesn't support CreateFileW
	HANDLE hFile = CreateFileA(W2CA(lpszFile),
	                          GENERIC_READ,
	                          FILE_SHARE_READ | FILE_SHARE_WRITE,
	                          NULL, // security attributes
	                          OPEN_EXISTING,
	                          0,
	                          NULL);
#endif
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;                     // file doesn't exist
	int nFileSize = GetFileSize(hFile, NULL);
	if (nFileSize < 5)
	{
		CloseHandle(hFile);
		return FALSE;
	}
	LPSTR lpszWork = new char[nFileSize + 1];

	DWORD  nBytesRead = 0;
	if (ReadFile(hFile, lpszWork, nFileSize, &nBytesRead, NULL))
	{
		lpszWork[nBytesRead] = '\0';
		m_Table.Clear();
		m_InputTable.Clear();
		LPSTR lpszT = lpszWork;

		while (*lpszT)
			lpszT = ProcessLine(lpszT);
		fResult = TRUE;
	}
	delete[] lpszWork;
	CloseHandle(hFile);
	return fResult;
}

LPSTR CBrailleTable::ProcessLine(LPSTR lpszBuffer)
{
	// skip leading white space
	while (*lpszBuffer && (*lpszBuffer == ' ' ||
	                       *lpszBuffer == '\t'))
		lpszBuffer++;
	// find the end of the line and 0-terminate it
	LPSTR lpszT = lpszBuffer;

	while (*lpszT && *lpszT != '\r')
		lpszT++;
	// skip over \r\n
	if (*lpszT)
	{
		*lpszT++ = '\0';            // get rid of \r
		if (*lpszT && *lpszT == '\n')
			lpszT++;
	}
	// Return if we're at the end of the buffer or
	// if this line is a comment (starts with a ;)
	if (lpszBuffer == lpszT || *lpszBuffer == ';')
		return lpszT;               // nothing to do
	// we now have a zero-terminated buffer containing one line
	// See if this line specifies which type of table we have
	// [OEM] or [ANSI]
	
	// Attributes section contains keys that may be used by Settings Center and
	// JAWS.  It was originally created to have a "Name" key which would contain
	// a friendly localized name for the table.  This way, JAWS and Settings
	// Center can display something other than the filename.  The section has
	// no table entries, therefore, ProcessLine will completely skip this
	// section.
	if (m_bTableAttributesSection && *lpszBuffer == '[')
	{
		m_bTableAttributesSection = FALSE;
	}

	if (*lpszBuffer == '[' && _strnicmp(lpszBuffer, "[Table Attributes]", _countof(_T("[Table Attributes]"))) == 0)
	{
		m_bTableAttributesSection = TRUE;
	}

	if (m_bTableAttributesSection)
		return lpszT;
	
	if (m_InputSection && *lpszBuffer == '[' )
	{
		m_InputSection = 0;
	}
	
	if (*lpszBuffer == '[' && _strnicmp(lpszBuffer, "[INPUT]", 5) == 0)
	{
		m_InputSection = TRUE;
	}

	if (*lpszBuffer == '[' && _strnicmp(lpszBuffer, "[OEM]", 5) == 0)
	{
		m_cpSection = CP_OEMCP;
	}
	else if (*lpszBuffer == '[' && _strnicmp(lpszBuffer, "[ANSI]", 6) == 0)
	{
		m_cpSection = CP_ACP;
	}
	else if (*lpszBuffer == '[' && _strnicmp(lpszBuffer, "[cp", 3) == 0)
	{
		m_cpSection = (UINT) atoi(lpszBuffer+3);
		if (m_cpSection == 0)
			m_cpSection = CP_ACP;
	}
	else
	{
		// this is the definition of a Braille character
		// if it is a single char followed by an =, we interpret it literally.
		// if it is a \ followed by digits, followed by = then this is an ASCII value
		// if it is U+abcd we treat it as a unicode mapping
		// if it is none of the above, we skip the line
		UINT nIdx = 0;
		int nValue = 0;
		BOOL bUnicodeDefinition=FALSE;

		if (lpszBuffer[1] == '=')
			nIdx = *lpszBuffer++;
		else if (*lpszBuffer == '\\')
		{
			lpszBuffer++;
			while (*lpszBuffer && isdigit(*lpszBuffer))
			{
				nIdx *= 10;
				nIdx += (*lpszBuffer++)-'0';
			}
		}
		else if (_strnicmp(lpszBuffer,"U+",2)==0 && isxdigit(lpszBuffer[2]))
		{
			int nHexLen=0;
			LPSTR lpszHex=lpszBuffer+2;
			while (lpszHex&& lpszHex[nHexLen]&& _istxdigit(lpszHex[nHexLen]))
				nHexLen++;
			if (nHexLen > 0)
			{
				char oldCh=lpszHex[nHexLen];
				lpszHex[nHexLen]='\0';
				lpszBuffer=lpszHex+nHexLen;
				WORD wIdx = (WORD)HexToDecimal(lpszHex);
				lpszHex[nHexLen]=oldCh;
				if (wIdx < 0xffff)
				{
					nIdx=wIdx;
					bUnicodeDefinition=TRUE;
				}
			}
			while (lpszBuffer && *lpszBuffer && *lpszBuffer !='=')
				lpszBuffer++;
		}
		else
			return lpszT;             // malformed line
		if (*lpszBuffer != '=' ||
		        nIdx >=0xffff)
			return lpszT;             // skip this line
		lpszBuffer++;               // skip over =
		DotPattern p;
		memset(&p,0,sizeof(p));
		BOOL bEmptyPattern=TRUE;
		while (*lpszBuffer)
		{
			nValue=0;
			while (*lpszBuffer && isspace(*lpszBuffer))
				lpszBuffer++;
			int nDigits = 0;
			while (*lpszBuffer && *lpszBuffer >='0' && *lpszBuffer <='8')
			{
				nDigits++;
				if (*lpszBuffer != '0')
					nValue += (1 << (*lpszBuffer - '1'));
				lpszBuffer++;
				bEmptyPattern=FALSE;
			}
			if (p.len < MaxCellsPerPattern && nDigits > 0)
				p.dotPattern[p.len++] = (BYTE)nValue;
			if (!*lpszBuffer)
				break;
			lpszBuffer++;
		}

		// check nIdx and convert if necessary
		if (!bUnicodeDefinition && (nIdx > 127 && nIdx <= 255))
		{
			char ch=LOBYTE(nIdx);
			DWORD dwFlags=MB_PRECOMPOSED;
			WCHAR szW[5]=L"\0";
			int nConvLen=0;

			// now convert this  char to a wide char
			nConvLen=MultiByteToWideChar(m_cpSection,// code page
																	 dwFlags,    // character-type options
					&ch, // string to map
					1, // number of bytes in string
					szW, // wide-character buffer
					5);// size of buffer
			if (nConvLen)
				nIdx=szW[0];
			else
				nIdx=ch;
		}
		// don't want to overwrite an existing definition if OemToAnsi converts a char for which we already have a definition,
		//such chars include OEM 213 which maps to ANSI 105
		if (bEmptyPattern || p.len==0)
		{
			p.len=1;
			p.dotPattern[0]=0;
		}
		if (!m_InputSection) 
		{
			if (m_Table[nIdx].len==0)
			{
				m_Table.SetAt(nIdx,p);
			}
		} else
		{
			if (m_InputTable[nIdx].len==0)
			{
				m_InputTable.SetAt(nIdx,p);
			}
		}
	}
	return lpszT;
}

int CBrailleTable::Translate(WCHAR ch, DotPattern& p)
{
	if (IsUnicodeBraille(ch))
	{
		p = (ch & byTransMask);
		return p.len;
	}

	if (m_Table.EntryAt(ch).len > 0)
	{
		p = m_Table[ch];
	}
	else // if we don't have a braille table entry we at least put out a space.
	{
		p = m_Table[_T(' ')];
	}

	p.ApplyMask(byTransMask);
	return p.len;
}

LPBYTE CBrailleTable::Translate(LPCWSTR lpszBuffer,int nLen)
{
	memset(&byTranslated,0,MAX_CELLS);
	int nBytes = (int)wcslen(lpszBuffer)/2;
	nBytes = min(MAX_CELLS,min(nBytes,nLen));
	byTranslated[nBytes] = '\0';
	LPCWSTR lpszSrc=lpszBuffer;
	LPBYTE lpbyDest=byTranslated;
	for (lpszSrc = lpszBuffer;*lpszSrc;lpszSrc++)
	{
		DotPattern p;
		memset(&p,0,sizeof(p));
		//int nLen=Translate(*lpszSrc,p);
		for (int nCells=0;nCells < p.len; nCells++)
		{
			lpbyDest+=nCells;
		}
		lpbyDest+=p.len;
	}
	return byTranslated;
}

WCHAR CBrailleTable::Untranslate(DotPattern& p)
{
	for (int n=0; n < MaxTableEntries; n++)
	{
		if (memcmp(&p.dotPattern,m_InputTable[n].dotPattern, p.len)==0)
		{
			TCHAR szDots[100]=_T("");
			for (int i=0; i<p.len; i++)
			{
				TCHAR szTemp[100];
				wsprintf(szTemp, _T("%x"), p.dotPattern[i]);
				_tcscat_s(szDots, szTemp);
			}
			return (WCHAR)n;
		}
	}
	for (int n=0; n < MaxTableEntries; n++)
	{
		if (memcmp(&p.dotPattern,m_Table[n].dotPattern, p.len)==0)
		{
			TCHAR szDots[100]=_T("");
			for (int i=0; i<p.len; i++)
			{
				TCHAR szTemp[100];
				wsprintf(szTemp, _T("%x"), p.dotPattern[i]);
				_tcscat_s(szDots, szTemp);
			}
			return (WCHAR)n;
	    }
	}

	return 0;
}

int CBrailleTable::HexToDecimal(LPCSTR lpszHex)
{
	if (!lpszHex)
		return 0;

	char ch;
	LPCSTR pch=lpszHex;
	int n = 0;

	while((ch = *pch++) != 0)
	{
		if (isdigit(ch))
			ch -= '0';
		else if (ch >= 'A' && ch <='F')
			ch += (char)(10 - 'A');
		else if (ch >= 'a' && ch <= 'f')
			ch += (char)(10 - 'a');
		else
			ch = (char)0;
		n =16 * n + ch;
	}
	return n;
}

