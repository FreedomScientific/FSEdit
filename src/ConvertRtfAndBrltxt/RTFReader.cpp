#include "stdafx.h"
#include <string>
#include <vector>
#include <stack>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std ;

#include "charsource.h"
#include "charoutput.h"
#include "token.h"
#include "tokeniser.h"
#include "rtftokens.h"

#include "RTFReader.h"

//==========================================================================
// RTF character attributes
//==========================================================================
CharAttr::CharAttr()
{
	Reset();
}

void CharAttr::Reset(void)
{
	bold = 0;
	allCaps = 0;
	deleted = 0;
	fontNum = 0;
	fontSize = 0;
	italic = 0;
	outline = 0;
	strikeThru = 0;
	underline = 0;
	dotUnderline = 0;
	dbUnderline = 0;
	noUnderline = 0;
	invisible = 0;
	charStyleNum = 0;
	charCharSet = 0;
	language = 0;
}

//==========================================================================
// RTF paragraph attributes
//==========================================================================
ParAttr::ParAttr()
{
	Reset();
}

void ParAttr::Reset(void)
{
	justify = 0;
	leftIndent = 0;
}

//==========================================================================
// RTF section attributes
//==========================================================================
SecAttr::SecAttr()
{
	Reset();
}

void SecAttr::Reset(void)
{
	leftMargin = -1;
	rightMargin = -1;
	topMargin = -1;
	bottomMargin = -1;
}

//==========================================================================
// RTF document attributes
//==========================================================================
DocAttr::DocAttr()
{
	Reset();
}

void DocAttr::Reset(void)
{
	// 1800 and 1440 are the default margin settings for an RTF file
	// but since the default margin for braille is 0 we're ignoring
	// the defaults

	leftMargin = 0;
	rightMargin = 0;
	topMargin = 0;
	bottomMargin = 0;
	pageNums = 0;

	// 12,240 and 15,840 are the default page sizes for an RTF file
	pageWidth =  6400; //12240;  40 cells per line
	pageHeight = 4000; //15840; 25 lines per page

	caretPosition = -1;
	markPosition = -1;
	brailleGrade2 = 1;
	memset(&bookmark, -1, sizeof(long)*26);
}

//==========================================================================
// RTFDestination
// Base class for all RTF destinations
//==========================================================================
RTFDestination::RTFDestination()
{
	m_bCharTokenReceived = FALSE;
	m_bAddTabForNewParagraph = FALSE;
	m_bAdjustParagraphFormat = TRUE;
}

RTFDestination::~RTFDestination()
{
}

bool RTFDestination::HandleToken(RTFReader& reader, Token& token)
{
	return(false);
}

//==========================================================================
// RTFReaderState
// Class that stores the RTF reader state when we push/pop groups
//==========================================================================
DocAttr RTFReaderState::m_docAttr;	// There are only one set of doc attr

RTFReaderState::RTFReaderState()
{
	m_currDest = NULL;
}

RTFReaderState::RTFReaderState( RTFReaderState* state)
{
	m_currDest = state->m_currDest;
	m_charAttr = state->m_charAttr;
	m_parAttr = state->m_parAttr;
	m_secAttr = state->m_secAttr;
	m_tableAttr = state->m_tableAttr;
	// No need to copy doc attr, there's only one set for all states
}

RTFReaderState::~RTFReaderState()
{
}

void RTFReaderState::SetDestination(RTFDestination* dest)
{
	m_currDest = dest;
}

void RTFReaderState::ResetDocAttributes(void)
{
	m_docAttr.Reset();
}

//==========================================================================
// RTFReader
// Base class for all RTF reader classes
//==========================================================================
RTFReader::RTFReader()
{
}

RTFReader::~RTFReader()
{
}

void RTFReader::ReadDocument(CharSource * source, CharOutput * out, BOOL bOnlyGetDocProperties)
{
	BOOL bHandledUserProps = FALSE;
	Tokeniser tokeniser(source);
	stack<RTFReaderState *> readerState;

	RTFReaderState::ResetDocAttributes();

	m_output = out;

	while ( !tokeniser.isFinished() )
	{
		m_currState = NULL;
		if (readerState.size())
			m_currState = readerState.top();

		Token *token = tokeniser.nextToken();

		if (!token)
			continue;

		if (token->destination)
		{
			if (m_currState)
			{
				m_currState->SetDestination( NextDestination( ((ControlToken *)token)->rtfMinor) );

				if (m_currState->Destination())
					m_currState->Destination()->Begin(*this);
			}
		}
		else
		{
			switch (token->getType())
			{
				case groupToken:
					if (((GroupToken*)token)->group == startGroup )
					{
						if (m_currState)
							m_currState = new RTFReaderState(m_currState);
						else
							m_currState = new RTFReaderState;
						readerState.push(m_currState);
					}
					else
					{
						// Check if the reader state change will change the
						// destination. If so, tell the destination it's ending
						if (m_currState)
						{
							readerState.pop();

							// Check if the reader state change will change the
							// destination. If so, tell the destination it's ending
							if (m_currState->Destination() && readerState.size())
							{
								RTFReaderState* nextState = readerState.top();

								if (m_currState->Destination() != nextState->Destination())
								{
									if ( m_currState->Destination()->IsUserPropsDest() )
									{
										// Current state is changing from IsUserPropsDest so 
										// at this point we can assume that UserProps have been handled 
										bHandledUserProps = TRUE;
									}
									else if ( nextState->Destination()->IsBodyDest()  && bOnlyGetDocProperties && bHandledUserProps )	
									{
										/*
										After the UserProps are handled a small BodyDest is handled.  
										After that the rest of the rtf props are handled.  
										After that the large BodyDest is handled. 
										If the code has entered here the large BodyDest is about to be handled.  
										So at this point all the necessary Props have been handled so we can exit. 
										*/
										m_currState->Destination()->End(*this);
										delete m_currState;
										delete token;
										Flush();
										return;
									}
									m_currState->Destination()->End(*this);
								}
							}

							delete m_currState;
						}
					}
					break;
				default:
					if (m_currState && m_currState->Destination())
						m_currState->Destination()->HandleToken(*this, *token);
					break;
			}
		}

		delete token;
	}

	Flush();
}

RTFDestination* RTFReader::NextDestination(int id)
{
	return(&m_ignoreDest);
}

