#include "stdafx.h"
#include <string>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std;

#include "charsource.h"
#include "charoutput.h"
#include "token.h"
#include "tokeniser.h"

#include "RTFReader.h"
#include "BRLReader.h"
#include "TXTWriter.h"
#include "TXTReader.h"

#include "BRLTxtReader.h"

#include "ConvertRtfAndBrltxt.h"

static BrlTxtDocProperties docProp;

static void CreateDocProperties(CString& str)
{
	CString tmp;

	//=======================================================================
	// Create the user props
	//=======================================================================
	str += "{\\*\\userprops ";
	tmp.Format(_T("{\\propname Caret position}\\proptype3{\\staticval %d}"), docProp.caretPosition);
	str += tmp;
	tmp.Format(_T("{\\propname Mark position}\\proptype3{\\staticval %d}"), docProp.markPosition);
	str += tmp;
	tmp.Format(_T("{\\propname Braille grade 2}\\proptype3{\\staticval %d}"), docProp.brailleGrade2);
	str += tmp;
	for(int j = 0; j < NUMOFBOOKMARKS; j++)
	{
		if(docProp.bookmark[j] != -1)
		{
			tmp.Format(_T("{\\propname Bookmark%d}\\proptype3{\\staticval %d}"), j, docProp.bookmark[j]);
			str += tmp;
		}
	}

	str += _T('}');

	//=======================================================================
	// Create the doc properties that are standard to RTF
	//=======================================================================
	str += "{";
	if (docProp.pageHeight != -1)
	{
		tmp.Format(_T("\\paperh%d"), docProp.pageHeight);
		str += tmp;
	}
	if (docProp.pageWidth != -1)
	{
		tmp.Format(_T("\\paperw%d"), docProp.pageWidth);
		str += tmp;
	}
	if (docProp.topMargin != -1)
	{
		tmp.Format(_T("\\margt%d"), docProp.topMargin);
		str += tmp;
	}
	if (docProp.bottomMargin != -1)
	{
		tmp.Format(_T("\\margb%d"), docProp.bottomMargin);
		str += tmp;
	}
	if (docProp.leftMargin != -1)
	{
		tmp.Format(_T("\\margl%d"), docProp.leftMargin);
		str += tmp;
	}
	if (docProp.rightMargin != -1)
	{
		tmp.Format(_T("\\margr%d"), docProp.rightMargin);
		str += tmp;
	}
	switch (docProp.pageNumbering)
	{
		case 1:	// Arabic
			str += "\\pgndec";
			break;
		case 2:	// Roman
			str += "\\pgnucrm";
			break;
		case 0:	// None
		default:
			break;
	}

	str += "}";
}

//==========================================================================
// EDITSTREAM callback to place our user properties in the RTF data stream
// The destination of the stream is a CString.
//==========================================================================
DWORD CALLBACK 
RTFStream_WriteToStringCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	BOOL bAddOurCustomTags = TRUE;
	int braceCount = 0;
	
	CString* pStream = (CString*)dwCookie;
	*pcb = cb;

	if ( strncmp("{\\rtf", (char*) pbBuff, 5) == 0 )
	{  // We only want to come in here the first time using the callback per file.
		while (cb )
		{
			switch (*pbBuff)
			{
			case '{':
				braceCount++;
				break;
			case '}':
				braceCount--;
				break;
			default:
				break;
			}
			*pStream += (char)*pbBuff++;
			if (braceCount == 2 && bAddOurCustomTags )
			{
				bAddOurCustomTags = FALSE;
				// After writing the second open brace write our
				// custom group & tags and doc properties
				CString tmp;
				CreateDocProperties(tmp);
				*pStream += tmp;
			}
			cb--;
		}
	}
	else
	{
		*pStream+= CString(pbBuff);
	}

	return 0;
}

//==========================================================================
// EDITSTREAM callback to place our user properties in the RTF data stream
// The destination of the stream is a file
//==========================================================================
DWORD CALLBACK RTFStream_WriteToFileCallback(DWORD_PTR dwCookie,
	LPBYTE pbBuff, LONG cb, LONG FAR *pcb)
{
	_ASSERTE(dwCookie != 0);
	_ASSERTE(pcb != NULL);
	USES_CONVERSION;
	DWORD bytesWritten;
	DWORD success = 0;
	int braceCount = 0;
	*pcb = cb;	// No matter how many chars we write to the
				// file we have to lie and say we wrote exactly
				// as many as we were requested to write or else
				// we get an assertion.
	if ( strncmp("{\\rtf", (char*) pbBuff, 5) == 0 )
	{  // We only want to come in here the first time using the callback per file.
		LONG scannedCount = 0;
		LONG secondLeftBrace = 0;
		BOOL foundInsertPosition = FALSE;
		while ((scannedCount < cb))
		{
			switch (pbBuff[scannedCount])
			{
			case '{':
				braceCount++;
				break;
			case '}':
				braceCount--;
				break;
			default:
				break;
			}
			if ( braceCount == 2) 
			{
				secondLeftBrace = scannedCount;
				foundInsertPosition = TRUE;
				break;
			}
			scannedCount++;
		}
	
		if (foundInsertPosition)
		{
		
			// Exclude the second '{' from the count
			scannedCount = secondLeftBrace;

			// First write the chars that we've scanned through
			if (scannedCount > 0)
			{
				success = !::WriteFile((HANDLE)dwCookie, pbBuff, scannedCount,
					(LPDWORD)&bytesWritten, NULL);
			}

			// Create our user & doc props
			CString userProps;
			CreateDocProperties(userProps);

			// Write the properties string in ansi and not unicode since the
			// RTF control does not handle unicode data
			LPSTR ansiText = W2A(userProps.GetBuffer(0));
			WriteFile((HANDLE)dwCookie, ansiText, userProps.GetLength(), (LPDWORD)&bytesWritten, NULL);

			// Now bump past the stuff we've already written and continue
			// as normal
			pbBuff += scannedCount;
			cb -= scannedCount;
		}	
	}
	if ((cb > 0) && (success == 0))
		success = !::WriteFile((HANDLE)dwCookie, pbBuff, cb, (LPDWORD)&bytesWritten, NULL);

	return(success);
}

//==========================================================================
// Import our user properties from an RTF data stream in a file
//==========================================================================
void ImportDocProperties(HANDLE hdl, BrlTxtDocProperties& prop )
{
	HFileCharSource src(hdl);
	TrashCharOutput dest;

	RTFtoBRLTXTReader parser;
	// Setting the bOnlyGetDocProperties to TRUE allows the parser to get the 
	// Doc Properties without having to parse the large Body of the file. This 
	// greatly helps performance.  
	parser.ReadDocument(&src, &dest, TRUE);

	prop.caretPosition = parser.State().DA().caretPosition;
	prop.markPosition = parser.State().DA().markPosition;
	prop.brailleGrade2 = parser.State().DA().brailleGrade2;
	prop.pageHeight = parser.State().DA().pageHeight;
	prop.pageWidth = parser.State().DA().pageWidth;
	prop.pageNumbering = parser.State().DA().pageNums;
	prop.topMargin = parser.State().DA().topMargin;
	prop.bottomMargin = parser.State().DA().bottomMargin;
	prop.leftMargin = parser.State().DA().leftMargin;
	prop.rightMargin = parser.State().DA().rightMargin;
	memcpy(&prop.bookmark, &parser.State().DA().bookmark, sizeof(long)*NUMOFBOOKMARKS);
}
BOOL ImportDocPropertiesFromFile(LPCTSTR lpszFilePath,BrlTxtDocProperties& prop)
{
	HANDLE hFile =     CreateFile(lpszFilePath,GENERIC_READ,FILE_SHARE_READ,
		  0,OPEN_EXISTING,0,0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
			return FALSE;
		}
	DWORD dwFileSize = GetFileSize(hFile,NULL);
	if (dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	HANDLE hMapping =CreateFileMapping( hFile,
                0,
                PAGE_READONLY,
                0,                   // size: high 32-bits
                0,           // size: low 32-bits
                NULL);
	if (hMapping == 0)
	{
					CloseHandle(hFile);
			return FALSE;
		}
	LPSTR lpContents = (LPSTR) MapViewOfFile( 
																	 hMapping,     // object to map view of
                FILE_MAP_READ, 
                0,              // high offset:  map from
                0,              // low offset:   beginning
                0);             // default: map entire file
						if (lpContents)
						{
		ImportDocProperties(lpContents,(int)dwFileSize,prop);
						UnmapViewOfFile(lpContents);
						}
						CloseHandle(hMapping);
								CloseHandle(hFile);
								return TRUE;
}
void ImportDocProperties(LPSTR lpData, int nLen, BrlTxtDocProperties& prop )
{
	CCharBufferSource	 src(lpData,nLen);
	TrashCharOutput dest;

	RTFtoBRLTXTReader parser;
	// Setting the bOnlyGetDocProperties to TRUE allows the parser to get the 
	// Doc Properties without having to parse the large Body of the file. This 
	// greatly helps performance.  
	parser.ReadDocument(&src, &dest, TRUE);

	prop.caretPosition = parser.State().DA().caretPosition;
	prop.markPosition = parser.State().DA().markPosition;
	prop.brailleGrade2 = parser.State().DA().brailleGrade2;
	prop.pageHeight = parser.State().DA().pageHeight;
	prop.pageWidth = parser.State().DA().pageWidth;
	prop.pageNumbering = parser.State().DA().pageNums;
	prop.topMargin = parser.State().DA().topMargin;
	prop.bottomMargin = parser.State().DA().bottomMargin;
	prop.leftMargin = parser.State().DA().leftMargin;
	prop.rightMargin = parser.State().DA().rightMargin;
	memcpy(&prop.bookmark, &parser.State().DA().bookmark, sizeof(long)*NUMOFBOOKMARKS);
}

//==========================================================================
// Get the current state of our doc properties
//==========================================================================
void GetDocProperties(BrlTxtDocProperties& prop)
{
	memcpy(&prop, &docProp, sizeof(prop));
}

//==========================================================================
// Set the property values that will be written to the RTF
// data stream. These values will be used by our EDITSTREAM callbacks
//==========================================================================
void SetDocProperties(BrlTxtDocProperties& prop)
{
	memcpy(&docProp, &prop, sizeof(docProp));
}

//==========================================================================
// Convert RTF into BRLTXT (file to file)
//==========================================================================
void RTFtoBRLTXT(HANDLE srcHdl, HANDLE destHdl)
{
	HFileCharSource src(srcHdl);
	HFileCharOutput dest(destHdl);

	RTFtoBRLTXTReader parser;
	parser.ReadDocument(&src, &dest);
}

//==========================================================================
// Convert RTF into BRLTXT (CString to CString)
//==========================================================================
void RTFtoBRLTXT(CString srcStr, CString& destStr)
{
	CStringSource src(srcStr);
	CStringOutput dest;

	RTFtoBRLTXTReader parser;
	parser.ReadDocument(&src, &dest);

	destStr = dest.m_data.c_str();
}

//==========================================================================
// Convert RTF into BRLTXT (string to file)
//==========================================================================
void RTFtoBRLTXT(CString srcStr, HANDLE destHdl)
{
	CStringSource src(srcStr);
	HFileCharOutput dest(destHdl);

	RTFtoBRLTXTReader reader;
	reader.ReadDocument(&src, &dest);
}

//==========================================================================
// Convert BRLTXT into RTF (CString to CString)
//==========================================================================
void BRLTXTtoRTF(CString srcStr, CString& destStr, BrlTxtDocProperties * pDocProp)
{
	CStringSource src(srcStr);
	CStringOutput dest;

	BrlTxt2RTFReader reader;
	reader.ReadDocument(src, &dest);

	destStr = dest.m_data.c_str();
}

//==========================================================================
// Convert BRLTXT into RTF (file to file)
//==========================================================================
void BRLTXTtoRTF(HANDLE srcHdl, HANDLE destHdl, BrlTxtDocProperties * pDocProp)
{
	HFileCharSource src(srcHdl);
	HFileCharOutput dest(destHdl);

	BrlTxt2RTFReader reader;
	reader.ReadDocument(src, &dest);
}

//==========================================================================
// Convert BRLTXT into RTF (file to string)
//==========================================================================
BOOL BRLTXTtoRTF(LPCTSTR lpszFilePath, CString& destStr, BrlTxtDocProperties * pDocProp)
{
	HANDLE hFile =     CreateFile(lpszFilePath,GENERIC_READ,FILE_SHARE_READ,
		  0,OPEN_EXISTING,0,0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
			return  FALSE;
		}
	DWORD dwFileSize = GetFileSize(hFile,NULL);
	if (dwFileSize == 0 || dwFileSize == INVALID_FILE_SIZE)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	HANDLE hMapping =CreateFileMapping( hFile,
                0,
                PAGE_READONLY,
                0,                   // size: high 32-bits
                0,           // size: low 32-bits
                NULL);
	if (hMapping == 0)
	{
					CloseHandle(hFile);
			return FALSE;
		}
	LPSTR lpContents = (LPSTR) MapViewOfFile( 
																	 hMapping,     // object to map view of
                FILE_MAP_READ, 
                0,              // high offset:  map from
                0,              // low offset:   beginning
                0);             // default: map entire file
						if (lpContents)
						{
	CCharBufferSource	 src(lpContents,(int)dwFileSize);
	CStringOutput dest;
	BrlTxt2RTFReader reader;
	reader.ReadDocument(src, &dest);
	destStr = dest.m_data.c_str();
						UnmapViewOfFile(lpContents);
						}
						CloseHandle(hMapping);
								CloseHandle(hFile);
								return TRUE;


}


//==========================================================================
// Convert RTF into TXT (file to file)
//==========================================================================
void RTFtoTXT(HANDLE srcHdl, HANDLE destHdl)
{
	HFileCharSource src(srcHdl);
	HFileCharOutput dest(destHdl);

	RTFtoTXTReader parser;
	parser.ReadDocument(&src, &dest);
}

//==========================================================================
// Convert RTF into TXT (CString to CString)
//==========================================================================
void RTFtoTXT(CString srcStr, CString& destStr)
{
	CStringSource src(srcStr);
	CStringOutput dest;

	RTFtoTXTReader parser;
	parser.ReadDocument(&src, &dest);

	destStr = dest.m_data.c_str();
}

//==========================================================================
// Convert RTF into TXT (string to file)
//==========================================================================
void RTFtoTXT(CString srcStr, HANDLE destHdl)
{
	CStringSource src(srcStr);
	HFileCharOutput dest(destHdl);

	RTFtoTXTReader reader;
	reader.ReadDocument(&src, &dest);
}

//==========================================================================
// Convert TXT into RTF (CString to CString)
//==========================================================================
void TXTtoRTF(CString srcStr, CString& destStr, BrlTxtDocProperties * pDocProp)
{
/*
 The Doc Properties parameter allows the user to pass in the Doc Properties that they would
 like put in the beginning tags of the RTF String.  The Doc Properties Values are not 
 permanently modified by this fuction call 
*/
	CStringSource src(srcStr);
	CStringOutput dest;
	CString strDocProp;
	if ( pDocProp != NULL )
	{
		// Original Doc Properties obtained. 
		BrlTxtDocProperties docPropertiesHold;
		GetDocProperties( docPropertiesHold ); 
		// Set Doc Properties to be put into RTF string.   
		SetDocProperties( *pDocProp );
		CreateDocProperties( strDocProp );
		
		// Original Doc Properties restored.  
		SetDocProperties( docPropertiesHold );
	}

	Txt2RTFReader reader;
	reader.ReadDocument(src, &dest, &strDocProp );

	destStr = dest.m_data.c_str();
}

//==========================================================================
// Convert TXT into RTF (file to file)
//==========================================================================
void TXTtoRTF(HANDLE srcHdl, HANDLE destHdl, BrlTxtDocProperties * pDocProp)
{
/*
 The Doc Properties parameter allows the user to pass in the Doc Properties that they would
 like put in the beginning tags of the RTF File.  The Doc Properties Values are not 
 permanently modified by this fuction call 
*/
	HFileCharSource src(srcHdl);
	HFileCharOutput dest(destHdl);
	CString strDocProp;
	if ( pDocProp != NULL )
	{
		// Original Doc Properties obtained. 
		BrlTxtDocProperties docPropertiesHold;
		GetDocProperties( docPropertiesHold ); 
		// Set Doc Properties to be put into RTF file.   
		SetDocProperties( *pDocProp );
		CreateDocProperties( strDocProp );
		
		// Original Doc Properties restored.  
		SetDocProperties( docPropertiesHold );
	}

	Txt2RTFReader reader;
	reader.ReadDocument(src, &dest, &strDocProp );
}

//==========================================================================
// Convert TXT into RTF (file to string)
//==========================================================================
void TXTtoRTF(HANDLE srcHdl, CString& destStr, BrlTxtDocProperties * pDocProp)
{
/*
 The Doc Properties parameter allows the user to pass in the Doc Properties that they would
 like put in the beginning tags of the RTF String.  The Doc Properties Values are not 
 permanently modified by this fuction call 
*/
	HFileCharSource src(srcHdl);
	CStringOutput dest;
	CString strDocProp;
	if ( pDocProp != NULL )
	{
		// Original Doc Properties obtained. 
		BrlTxtDocProperties docPropertiesHold;
		GetDocProperties( docPropertiesHold ); 
		// Set Doc Properties to be put into RTF string.   
		SetDocProperties( *pDocProp );
		CreateDocProperties( strDocProp );
		
		// Original Doc Properties restored.  
		SetDocProperties( docPropertiesHold );
	}

	Txt2RTFReader reader;
	reader.ReadDocument(src, &dest, &strDocProp );

	destStr = dest.m_data.c_str();
}
