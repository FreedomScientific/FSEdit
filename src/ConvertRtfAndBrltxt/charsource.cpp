#include "stdafx.h"
#include <atlapp.h>
#include <atlmisc.h>

#include "charsource.h"

CharSource::CharSource()
{
}

CharSource::~CharSource()
{
}

//==========================================================================
// CStringSource
//==========================================================================
CStringSource::CStringSource(CString data)
{
	m_data = data;
	m_currPos = 0;
}

bool CStringSource::endOfInput()
{
	return(m_currPos >= m_data.GetLength());
}

char CStringSource::unGetChar( char ch )
{
	if (m_currPos <= 0)
		return(0);

	m_currPos--;
	return(ch);
}

char CStringSource::getChar()
{

	char szChars[5]={0};
	WCHAR szData[2]={0};
	szData[0]=m_data[m_currPos++];

	WideCharToMultiByte(GetACP(), 0, szData, 1, szChars, 2, NULL, NULL);
	return( szChars[0] );
}

WCHAR CStringSource::getwChar()
{

	WCHAR wData;
	wData=m_data[m_currPos++];
	return( wData );
}
//==========================================================================
// CCharBufferSource
//==========================================================================
CCharBufferSource::CCharBufferSource(char *lpData,int nLen)
{
	m_Data = lpData;
	m_currPos = 0;
	m_nLen = nLen;
}

bool CCharBufferSource::endOfInput()
{
	return(m_currPos >= m_nLen);
}

char CCharBufferSource::unGetChar( char ch )
{
	if (m_currPos <= 0)
		return(0);

	m_currPos--;
	return(ch);
}

char CCharBufferSource::getChar()
{
	return( m_Data[m_currPos++] );
}

WCHAR CCharBufferSource::getwChar()
{
	return( m_Data[m_currPos++] );
}


//==========================================================================
// FileCharSource
//==========================================================================
FileCharSource::FileCharSource()
{
	f = NULL;
}

FileCharSource::FileCharSource(char *filename)
{
	fopen_s(&f, filename, "r" );
}

FileCharSource::~FileCharSource()
{
	if (f) fclose(f);
}

FileCharSource::FileCharSource(FILE *fileHandle)
{
	f = fileHandle;
}

char FileCharSource::unGetChar(char ch)
{
	return static_cast<char>(ungetc(ch,f));
}

char FileCharSource::getChar()
{
	return  static_cast<char>(fgetc(f) );
}

bool FileCharSource::endOfInput()
{
	return( feof( f ) != 0 ); // casting to bool does not stop the compiler warning
}

//==========================================================================
// HFileCharSource
//==========================================================================
HFileCharSource::HFileCharSource(HANDLE hdl)
{
	m_hdl = hdl;
	SetFilePointer(m_hdl, 0, NULL, FILE_BEGIN) ; 
}

bool HFileCharSource::endOfInput()
{
	DWORD currPos = SetFilePointer(m_hdl, 0, NULL, FILE_CURRENT);
	DWORD size = GetFileSize(m_hdl, NULL) ; 

	return(currPos >= size);
}

char HFileCharSource::unGetChar( char ch )
{
	DWORD currPos = SetFilePointer(m_hdl, 0, NULL, FILE_CURRENT);
	if (currPos > 0)
		SetFilePointer(m_hdl, -1, NULL, FILE_CURRENT);
	ch = 0;
	return(ch);
}

char HFileCharSource::getChar()
{
	char tmp;
	DWORD bytesRead;
	ReadFile(m_hdl, &tmp, sizeof(tmp), &bytesRead, NULL);
	return(tmp);
}

WCHAR HFileCharSource::getwChar()
{
	return(getChar());
}
