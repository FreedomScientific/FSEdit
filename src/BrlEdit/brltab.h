/***
   NAME
     brltab.h

   DESCRIPTION
     Header file for BrailleTable class

   Created
     2/22/96  by Glen Gordon

   NOTES
Made Unicode compatible by JKS 24 Feb 05
*/
/*
Copyright (C) 1996      by Henter-Joyce, Inc.  All rights reserved.
*/
#pragma once;
	  #define MAX_CELLS 1024
#include "sparse.h"
#include <map>
class CBrailleTable
{
public:
	enum
	{
		MaxCellsPerPattern=7,
		MaxTableEntries=65536
	};

	struct DotPattern
	{
		BYTE dotPattern[MaxCellsPerPattern];
		BYTE len;

		DotPattern() :
			len(0)
		{
			memset(dotPattern, NULL, sizeof(dotPattern));
		}

		DotPattern& operator=(BYTE cell)
		{
			dotPattern[0] = cell;
			len = 1;
			return *this;
		}

		void ApplyMask(BYTE mask)
		{
			// Note that if the mask is 0xff, then there's nothing to mask off of a BYTE
			if (mask == 0xff)
				return;

			for (int i = 0; i < len; ++i)
			{
				dotPattern[i] &= mask;
			}
		}
	};
private:
	typedef CSparseArray<DotPattern,MaxTableEntries, 256> DotPatternTable;
	DotPatternTable m_Table;
	DotPatternTable m_InputTable;
	BYTE byTranslated[MAX_CELLS];
	//! The code page in which the section being read was written
	UINT m_cpSection;
	BYTE byTransMask;
	UINT m_InputSection;
	BOOL m_bTableAttributesSection;
	LPSTR ProcessLine(LPSTR lpszBuf);
	WCHAR szFileName[_MAX_FNAME];

public:
	CBrailleTable();
	~CBrailleTable();
	BOOL Load(LPCWSTR lpszFile);
	LPCWSTR Path() const
	{
		return szFileName;
	};
	LPBYTE Translate(LPCWSTR lpszBuffer,int nLen); // translate a buffer to dots
	int Translate(WCHAR ch, DotPattern& p);  // translate a char to dots
	WCHAR Untranslate(DotPattern& p);  // translate dots to a char

	void Set8Dot();
	void Set6Dot();
private:
	int HexToDecimal(LPCSTR lpszHex);
};


inline CBrailleTable::CBrailleTable()
{
	memset(&byTranslated,0,MAX_CELLS);
	m_cpSection = CP_ACP;
	m_InputSection = 0;
	m_bTableAttributesSection = FALSE;
	szFileName[0] = _T('\0');
	Set8Dot();
}

inline CBrailleTable::~CBrailleTable()
{
}

inline void CBrailleTable::Set8Dot()
{
	byTransMask = 0xff;
}

inline void CBrailleTable::Set6Dot()
{
	byTransMask = 0x3f;
}

