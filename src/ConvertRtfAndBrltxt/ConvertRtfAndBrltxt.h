#pragma once

#define NUMOFBOOKMARKS	26L

class BrlTxtDocProperties
{
public:
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
	long bookmark[NUMOFBOOKMARKS];

public:
	BrlTxtDocProperties() { Reset(); }
	void Reset(void)
	{
		caretPosition = -1;
		markPosition = -1;
		brailleGrade2 = 1;
		pageHeight = -1;
		pageWidth = -1;
		pageNumbering = -1;
		topMargin = -1;
		bottomMargin = -1;
		leftMargin = -1;
		rightMargin = -1;
		memset(&bookmark, -1, sizeof(long)*NUMOFBOOKMARKS);
	}
};

//==========================================================================
// Import our user properties from an RTF data stream in a file
//==========================================================================
BOOL ImportDocPropertiesFromFile(LPCTSTR lpszFilePath,BrlTxtDocProperties& prop);
void ImportDocProperties(HANDLE hdl, BrlTxtDocProperties& prop);
void ImportDocProperties(LPSTR lpData, int nLen, BrlTxtDocProperties& prop );

//==========================================================================
// Get the current state of our doc properties
//==========================================================================
void GetDocProperties(BrlTxtDocProperties& prop);

//==========================================================================
// Set the property values that will be written to the RTF
// data stream. These values will be used by our EDITSTREAM callbacks
//==========================================================================
void SetDocProperties(BrlTxtDocProperties& prop);

//==========================================================================
// EDITSTREAM callback to place our user properties in the RTF data stream
// The destination of the stream is a CString
//==========================================================================
DWORD CALLBACK RTFStream_WriteToStringCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb);

//==========================================================================
// EDITSTREAM callback to place our user properties in the RTF data stream
// The destination of the stream is a file
//==========================================================================
DWORD CALLBACK RTFStream_WriteToFileCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb);

//==========================================================================
// Convert RTF into BRLTXT (file to file)
//==========================================================================
void RTFtoBRLTXT(HANDLE srcHdl, HANDLE destHdl);

//==========================================================================
// Convert RTF into BRLTXT (CString to CString)
//==========================================================================
void RTFtoBRLTXT(CString srcStr, CString& destStr);

//==========================================================================
// Convert RTF into BRLTXT (string to file)
//==========================================================================
void RTFtoBRLTXT(CString srcStr, HANDLE destHdl);

//==========================================================================
// Convert BRLTXT into RTF (CString to CString)
//==========================================================================
void BRLTXTtoRTF(CString srcStr, CString& destStr, BrlTxtDocProperties * pDocProp = 0);

//==========================================================================
// Convert BRLTXT into RTF (file to file)
//==========================================================================
void BRLTXTtoRTF(HANDLE srcHdl, HANDLE destHdl, BrlTxtDocProperties * pDocProp = 0);

//==========================================================================
// Convert BRLTXT into RTF (file to string)
//==========================================================================
BOOL BRLTXTtoRTF(LPCTSTR lpszFilePath, CString& destStr, BrlTxtDocProperties * pDocProp = 0);

//==========================================================================
// Convert RTF into TXT (file to file)
//==========================================================================
void RTFtoTXT(HANDLE srcHdl, HANDLE destHdl);

//==========================================================================
// Convert RTF into TXT (CString to CString)
//==========================================================================
void RTFtoTXT(CString srcStr, CString& destStr);

//==========================================================================
// Convert RTF into TXT (string to file)
//==========================================================================
void RTFtoTXT(CString srcStr, HANDLE destHdl);

//==========================================================================
// Convert TXT into RTF (CString to CString)
//==========================================================================
void TXTtoRTF(CString srcStr, CString& destStr, BrlTxtDocProperties * pDocProp = 0);

//==========================================================================
// Convert TXT into RTF (file to file)
//==========================================================================
void TXTtoRTF(HANDLE srcHdl, HANDLE destHdl, BrlTxtDocProperties * pDocProp = 0);

//==========================================================================
// Convert TXT into RTF (file to string)
//==========================================================================
void TXTtoRTF(HANDLE hSrc, CString& destStr, BrlTxtDocProperties * pDocProp = 0);
