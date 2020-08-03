/*Copyright 2001
 *
 * tokeniser.cpp
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
// tokeniser.cpp: implementation of the Tokeniser class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <ctype.h>
#include <string>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std ;

#include "charsource.h"	// 
#include "token.h"
#include "tokeniser.h"

#include "rtftokens.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static bool	hashTableReady = false;

Tokeniser::Tokeniser()
{
	finished = true;
	lookupInit();
}

Tokeniser::~Tokeniser()
{
}

Tokeniser::Tokeniser(CharSource *source)
{
	Tokeniser();
	setCharSource (source);
	finished = false;
}

//#define	TRACE2FILE		1
#ifdef	TRACE2FILE
static CFile traceFile("Trace.txt", CFile::modeCreate|CFile::modeWrite);
#endif

Token *Tokeniser::nextToken()
{
	Token* next = NULL;

	if ( !charSource ) return next;

	char ch = 0;

	while (!charSource->endOfInput() && (ch == 0))
	{
		ch = charSource->getChar();
		switch (ch) 
		{
		case '\\':
			next = backslash();
			break;
		case '{':
			next = new GroupToken( startGroup );
			break;
		case '}':
			next = new GroupToken( endGroup );
			break;
		case '\xff':
			break;
		case 0x0D:
		case 0x0A:
			ch = 0;
			break;
		default:
			next = new CharToken( ch );
			break;
		}
	}

	if (ch == 0)
		finished = true;

#ifdef	TRACE2FILE
	if (next)
	{
		static indent = 0;
		int incIndent = 0;
		CString trace;
		ControlToken* ctrlToken = (ControlToken*)next;

		switch (next->getType())
		{
			case controlToken:
				switch (ctrlToken->rtfMajor)
				{
					case rtfCharAttr:
						trace.Format("C-CA [%s] %d\n", next->text.c_str(), ctrlToken->arg);
						break;
					case rtfSpecialChar:
						trace.Format("C-SC [%s] %d\n", next->text.c_str(), ctrlToken->arg);
						break;
					case rtfParAttr:
						trace.Format("C-PA [%s] %d\n", next->text.c_str(), ctrlToken->arg);
						break;
					case rtfSectAttr:
						trace.Format("C-SA [%s] %d\n", next->text.c_str(), ctrlToken->arg);
						break;
					case rtfDocAttr:
						trace.Format("C-DA [%s] %d\n", next->text.c_str(), ctrlToken->arg);
						break;
					case rtfUserPropAttr:
						trace.Format("C-UP [%s] %d\n", next->text.c_str(), ctrlToken->arg);
						break;						
					default:
						trace.Format("C-?? [%s] %d\n", next->text.c_str(), ctrlToken->arg);
						break;
				}
				break;
			case charToken:
				trace.Format("CH [%s]\n", next->text.c_str());
				break;
			case groupToken:
				{
					GroupToken* grpTkn = (GroupToken*)next;
					trace.Format("%s\n", next->text.c_str());
					if (grpTkn->group == startGroup)
						incIndent = 1;
					else
						incIndent = -1;
				}
				break;
			default:
				trace.Format("TOK? [%s]\n", next->text.c_str());
				break;
		}

		if (incIndent == -1)	
			indent--;
		for (int i = 0; i < indent; i++)
		{
//			TRACE("\t");
			traceFile.Write("\t", 1);
		}
		if (incIndent == 1)
			indent++;
//		TRACE(trace);
		traceFile.Write(trace, trace.GetLength());
	}
#endif

	return(next);
}

void Tokeniser::setCharSource(CharSource *source)
{
	charSource = source;
	finished = false;
}

bool Tokeniser::isFinished()
{
	return finished;
}

int Tokeniser::buildArg(char ch)
{
	int i=0;
	char arg[MAX_RTF_TOKEN];

	if( charSource->endOfInput())
	{
		return -1;
	}
	if(ch=='-')
	{
		arg[i++]='-';
		ch=charSource->getChar();
		if(charSource->endOfInput())
		{
			arg[0]=0;

			return atoi( arg );
		}
	}
	for(;iswdigit(ch);i++)
	{
		arg[i]=ch;
		if(i>=MAX_RTF_TOKEN-1)
		{
			arg[MAX_RTF_TOKEN-1]=0;
			while(iswdigit(ch)) {
				ch=charSource->getChar();
				if(charSource->endOfInput())
					return atoi( arg );
			} 
			break;
		}
		ch=charSource->getChar();
		if(charSource->endOfInput())
		{
			arg[i+1]=0;
			return atoi(arg);
		}
	}
	arg[i]=0;
	 
	if(!iswspace(ch)) {
		charSource->unGetChar( ch );
	}
	if( arg[0] == '\0' ) {
		return -1;
	} else {
		return atoi( arg );
	}
}

Token *Tokeniser::buildControlToken( char ch, bool isDestination)
{
	int i=0;
	char token[MAX_RTF_TOKEN];
	ControlToken * control = new ControlToken();
	control->destination = isDestination;

	for(i=1;;i++)
	{
		token[i-1]=ch;
		if(i>=MAX_RTF_TOKEN-1)
		{
			do {
				ch=charSource->getChar();
				if(charSource->endOfInput()) {
					token[i] = '\0';
					control->SetText(token);
					return control;
				}
			} while (iswalpha(ch)); 	
			control->arg = buildArg(ch);
			break;
		}
		ch= charSource->getChar();
		if(charSource->endOfInput())
		{
			token[i]='\0';
			break;
		}
		if( !iswalpha(ch) )
		{
			token[i]='\0';
			control->arg = buildArg(ch);
			break;
		}
	}
	token[i]='\0';
	control->SetText(token);
	//control->text = token;
	lookup (token, control);
	return (Token *) control;
}

Token *Tokeniser::backslash()
{
	// already seen a backslash

	char ch = charSource->getChar();
	if(charSource->endOfInput())
	{
		return (Token *) new InvalidToken("backslash at end of file");
	}
	if ( ch == '*' ) {
		return handleDestination();
	}

	switch (ch) 
	{
		case '\\':
		case '{':
		case '}':
		case '_':  // it seems \_ is another valid RTF escape 
			return (Token * ) new CharToken( ch );
			break;
		case '\'': // single quote -> rtf escaped character 
		{
			char ch1, ch2;
			ch1 = charSource->getChar();
			ch2 = charSource->getChar();
			if(!charSource->endOfInput()) 
			{
				if(iswxdigit(ch1)&&iswxdigit(ch2))
				{
					// needs work: ch = chartoi(ch1)*16+chartoi(ch2);
					return (Token * ) new CharToken( ch );
				}
			} 
			return (Token *) new InvalidToken("invalid rtf character escape code");
			break;
		}
		case 'u': // possible unicode character 
		{
			char ch2, ch1;
			ch1 = charSource->getChar();
			if (!iswalpha(ch1) || ch1 =='-' )
			{ 
				char digits[6]={'0'};
				bool bNegative=FALSE;
				bool bAccruing=TRUE;
				 // check to see that the first char is negative sign
				if (ch1=='-')
				{
					bNegative=TRUE;
					if(!charSource->endOfInput()) 
					{
						ch1=charSource->getChar();
					}
					else
						ch1=0;
				}
				// check that we now have a numeric character
				if (!iswdigit(ch1))
					bAccruing=FALSE;
				else
					digits[0]=ch1;
				// get more digits - up to 5 in all.
				if(bAccruing && !charSource->endOfInput()) 
				{
					ch1 = charSource->getChar();
					if (!iswdigit(ch1))
						bAccruing=FALSE;
					else
						digits[1]=ch1;
				}
				if(bAccruing && !charSource->endOfInput()) 
				{
					ch1 = charSource->getChar();
					if (!iswdigit(ch1))
						bAccruing=FALSE;
					else
						digits[2]=ch1;
				}
				if(bAccruing && !charSource->endOfInput()) 
				{
					ch1 = charSource->getChar();
					if (!iswdigit(ch1))
						bAccruing=FALSE;
					else
						digits[3]=ch1;
				}
				if(bAccruing && !charSource->endOfInput()) 
				{
					ch1 = charSource->getChar();
					if (!iswdigit(ch1))
					{
						bAccruing=FALSE;
					}
					else
						digits[4]=ch1;
				}

				if (!bAccruing)
					charSource->unGetChar(ch1);

				WCHAR wNew=0;
				int i=0;
				while (digits[i]) 
				{
					wNew = static_cast<WCHAR>(wNew * 10 + chartoi(digits[i++]));
				}

				if (bNegative)
				{ // correct if a negative decimal value was provided
					wNew = static_cast<WCHAR>(0x10000 - wNew);

				}
				
				char szChars[5]={0};
				TCHAR szData[2]={0};
				char szReplace='?';
				szData[0]=wNew;

				// next, according to RTF spec a replacement character can be specfied to be used 
				// in place of the unicode char for non-unicode apps.  For example \u321? or \u321\'3f
				// So now we determine what the replace char is...
				ch1 = charSource->getChar();
				if (!charSource->endOfInput())
				{
					if (ch1=='\\')
					{
						ch1 = charSource->getChar();
						if (!charSource->endOfInput() && ch1 == '\'')
						{
							ch1 = charSource->getChar();
							ch2 = charSource->getChar();
							if(!charSource->endOfInput()) 
							{
								if(iswxdigit(ch1)&&iswxdigit(ch2))
								{
									szReplace = static_cast<char>(chartoi(ch1)*16+chartoi(ch2));
								}
							}
						}
					}
					else
					{ // the replace character was not encoded in the stream
						szReplace=ch1;
					}
				}
				WideCharToMultiByte(GetACP(), 0, szData, 1, szChars, 2, &szReplace, NULL);
				return (Token * ) new CharToken( szChars[0] );

				break;
			}
			else 
			{ // not a unicode character so drop thru and process as default
				charSource->unGetChar(ch1);
			}
		} // intentional drop thru to default!!!!
		default:
			if (iswalpha(ch)) 
			{
				return buildControlToken(ch, false);
			} else {
				return (Token *) new InvalidToken("unrecognised rtf escape code");
			}
			break;
	}
}

Token * Tokeniser::handleDestination()
{
	char ch = charSource->getChar();
	if(charSource->endOfInput())
	{
		return (Token *) new InvalidToken("backslash at end of file");
	}

	if ( ch == '\\' )
	{
		ch = charSource->getChar();
		if(charSource->endOfInput())
		{
			return (Token *) new InvalidToken("backslash at end of file");
		}

		if (iswalpha(ch)) 
		{
			return buildControlToken( ch, true);
		}
		else
		{
			return (Token *) new InvalidToken("non-alphabetic character in destination name");
		}
	}
	else
	{
		return (Token *) new InvalidToken("\\destination expected after '\\*'");
	}

}

int Tokeniser::chartoi(char ch)
{
	if((ch>='0')&&(ch<='9'))
		return ch-'0';
	if((ch>='A')&&(ch<='Z'))
		return ch-'A'+10;
	if((ch>='a')&&(ch<='z'))
		return ch-'a'+10;
	return -1;
}

/*
 * Initialize lookup table hash values.  Only need to do this once.
 */

void
Tokeniser::lookupInit ()
{
	RTFKey	*rp;

	if (!hashTableReady )
	{
		for (rp = rtfKey; rp->rtfKStr != (char *) NULL; rp++)
			rp->rtfKHash = hash (rp->rtfKStr);
		hashTableReady = true;
	}
}


/*
 * Determine major and minor number of control next->  If it's
 * not found, the class turns into rtfUnknown.
 */

void
Tokeniser::lookup (char *s, ControlToken *token)
{
	RTFKey	*rp;
	int	hashValue;

	hashValue = hash (s);
	for (rp = rtfKey; rp->rtfKStr != (char *) NULL; rp++)
	{
		if (hashValue == rp->rtfKHash && strcmp (s, rp->rtfKStr) == 0)
		{
			//token->rtfClass = rtfControl;
			token->rtfMajor = rp->rtfKMajor;
			if ( token->rtfMajor == rtfDestination ) {
				token->destination = true;
			}
			token->rtfMinor = rp->rtfKMinor;
			return;
		}
	}
	//token->rtfClass = rtfUnknown;
}


/*
 * Compute hash value of symbol
 */

int
Tokeniser::hash (char *s)
{
	char	c;
	int	val = 0;

	while ((c = *s++) != '\0') {
		val += (int) c;
	}
	return (val);
}

