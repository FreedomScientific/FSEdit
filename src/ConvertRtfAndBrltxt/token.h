/*Copyright 2001
 *
 * token.h
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
// token.h: interface for the Token class.
//
//////////////////////////////////////////////////////////////////////
#include "blockalloc.h"
typedef enum tag_TokenType {
	invalidToken,
	charToken,
	groupToken,
	controlToken
} RTFTokenType;


class Token  
{
	enum {
		MAX_TOKEN_TEXT=64
	};
public:
	bool destination;
	virtual RTFTokenType getType()=0;
	Token();
	virtual ~Token();
	void SetText(LPCSTR szText);
	LPCSTR GetText()
	{
		return m_szText;
	}
 protected:
char m_szText[MAX_TOKEN_TEXT];	// store the token text in case it is needed
};

enum GroupType { startGroup, endGroup };

class GroupToken : public Token  
{
public:
	GroupToken( GroupType t );
	RTFTokenType getType();
	GroupToken();
	virtual ~GroupToken();
	GroupType group;
	DECLARE_FIXED_BL_ALLOC(GroupToken)
};


class ControlToken : public Token  
{
public:
	int rtfMinor;
	int rtfMajor;
	RTFTokenType getType();
	ControlToken();
	virtual ~ControlToken();
	bool hasArg;
	int arg;		// arg value if present
	DECLARE_FIXED_BL_ALLOC(ControlToken)
};


class CharToken : public Token  
{
public:
	CharToken( char ch );
	RTFTokenType getType();
	CharToken();
	virtual ~CharToken();
	char ch;
	DECLARE_FIXED_BL_ALLOC(CharToken)
};


class InvalidToken : public Token  
{
public:
	string errorMessage;
	InvalidToken( const char *errorMessage );
	RTFTokenType getType();
	InvalidToken();
	virtual ~InvalidToken();
	DECLARE_FIXED_BL_ALLOC(InvalidToken)
};

