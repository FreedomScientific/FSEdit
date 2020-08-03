/*Copyright 2001
 *
 * token.cpp
 *
 * Author: Gregory Ford greg@reddfish.co.nz
 * RTF token parser based on:
 *      rtf2html by Dmitry Porapov <dpotapov@capitalsoft.com>,
 *      based on earlier work of Chuck Shotton.
 *      distributed under BSD-style license
 * RTF token lists and hashing algorithm based on
 *      RTF Tools, Release 1.10 
 *      6 April 1994	
 *      Paul DuBois	dubois@primate.wisc.edu 
 *
 * Copying permitted under GNU licence (see COPYING)
 */ 
// token.cpp: implementation of the Token class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <string>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std ;


#include "token.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Token::Token()
{
	destination = false;
	m_szText[0] = '\0';
}

Token::~Token()
{

}
void Token::SetText(LPCSTR lpszText)
{
	if (lpszText)
		strncpy_s(m_szText,lpszText,MAX_TOKEN_TEXT-1);
	m_szText[MAX_TOKEN_TEXT-1] = '\0';
}
//////////////////////////////////////////////////////////////////////
// GroupToken Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_FIXED_BL_ALLOC(GroupToken,10)

GroupToken::GroupToken()
{
}

GroupToken::~GroupToken()
{
}

//////////////////////////////////////////////////////////////////////
// ControlToken Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_FIXED_BL_ALLOC(ControlToken,10)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ControlToken::ControlToken()
{
	arg = 0;
	hasArg = false;
	m_szText[0] = '\0';
	
}

ControlToken::~ControlToken()
{

}

//////////////////////////////////////////////////////////////////////
// CharToken Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_FIXED_BL_ALLOC(CharToken,10)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CharToken::CharToken()
{
	m_szText[0] = '\0';
}

CharToken::~CharToken()
{

}


RTFTokenType CharToken::getType()
{
	return charToken;
}

RTFTokenType ControlToken::getType()
{
	return controlToken;
}


RTFTokenType GroupToken::getType()
{
	return groupToken;
}

RTFTokenType InvalidToken::getType()
{
	return invalidToken;
}


GroupToken::GroupToken(GroupType t)
{
	group = t;
	m_szText[0] = '\0';
	if ( t == startGroup ) {
		strcpy_s(m_szText,"{");
	} else {
		strcpy_s(m_szText,"}");
	}
}

CharToken::CharToken(char ch)
{
	m_szText[0] = ch;
	m_szText[1] = '\0';
	this->ch = ch;
}

//////////////////////////////////////////////////////////////////////
// InvalidToken Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_FIXED_BL_ALLOC(InvalidToken,10)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

InvalidToken::InvalidToken()
{

}

InvalidToken::~InvalidToken()
{

}


InvalidToken::InvalidToken(const char *errorMessage)
{
	this->errorMessage = errorMessage;
}
