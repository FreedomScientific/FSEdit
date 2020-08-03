#include "stdafx.h"
#include <string>
#include <iostream>

using namespace std;
#include <stdio.h>

#include "charoutput.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CharOutput::CharOutput()
{
	m_length = 0;
}

CharOutput::~CharOutput()
{
}

int CharOutput::putString(LPCSTR lpszText)
{
	m_length += strlen(lpszText);
	return true;
}

void CharOutput::putChar( char c )
{
	m_length++;
}

//////////////////////////////////////////////////////////////////////
// FileCharOutput Class
//////////////////////////////////////////////////////////////////////
FileCharOutput::~FileCharOutput()
{
	if ( fh ) {
		fclose( fh );
	}
}

FileCharOutput::FileCharOutput(const char *filename)
{
	fopen_s(&fh, filename, "w+" );
}

void FileCharOutput::putChar(char c)
{
	fputc( c, fh );
	m_length++;
}

int FileCharOutput::putString(LPCSTR lpszText)
{
	fputs( lpszText, fh);
	m_length += strlen(lpszText);
	return true;
}

//==========================================================================
// CStringOutput
//==========================================================================
void CStringOutput::putChar( char c )
{
	m_data += c;
	m_length++;
}

int CStringOutput::putString(LPCSTR lpszText)
{
	m_data += lpszText;
	m_length += strlen(lpszText);
	return(true);
}

//==========================================================================
// HFileCharOutput
//==========================================================================
HFileCharOutput::HFileCharOutput(HANDLE hdl)
{
	m_hdl = hdl;
}

void HFileCharOutput::putChar( char c )
{
	DWORD bytesWritten;
	::WriteFile(m_hdl, &c, sizeof(c), &bytesWritten, NULL);
	m_length += bytesWritten;
}

int HFileCharOutput::putString(LPCSTR lpszText)
{
	DWORD bytesWritten;
	::WriteFile(m_hdl, lpszText,static_cast<DWORD>(strlen(lpszText)), &bytesWritten, NULL);
	m_length += bytesWritten;
	return(true);
}

//==========================================================================
// TrashCharOutput
//==========================================================================
void TrashCharOutput::putChar( char c )
{
	m_length++;
}

int TrashCharOutput::putString(LPCSTR lpszText)
{
	m_length += strlen(lpszText);
	return(true);
}
