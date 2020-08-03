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
#include "BRLReader.h"
#include "TXTWriter.h"


const int twipsPerPixel = 20;
const int pixelsPerSpace = 8;


int TwipsToTXTColumns(int twips)
{
	return(twips / twipsPerPixel / pixelsPerSpace);
}
/*
int ColumnsToTwips(int columns)
{
	return(columns * pixelsPerSpace * twipsPerPixel);
}

int TwipsToHorizMargin(int twips)
{
	// Since the normal braille doc is about 40 columns and the average
	// doc is 80 columns adjust the margins to try to center within the
	// smaller number of columns.

	return( TwipsToColumns(twips) / 2 );
}

int TwipsToVertMargin(int twips)
{
	// Since the normal braille doc is about 40 colums and the average
	// doc is 80 columns adjust the margins to try to center within the
	// smaller number of columns.

	return( TwipsToColumns(twips) );
}
*/
//==========================================================================
// RTFtoTXTReader
// RTF reader to convert RTF to TXT
//==========================================================================
RTFDestination* RTFtoTXTReader::NextDestination(int id)
{
	switch (id)
	{
		case rtfIVersion:
			return(&m_bodyDest);
		case rtfHeader:
			return(&m_headerDest);
		case rtfFooter:
			return(&m_footerDest);
		case rtfFieldInst:
			return(&m_fldInstr);
		case rtfUserProps:
			return(&m_userProps);
		default:
			return( RTFReader::NextDestination(id) );
	}
}

void RTFtoTXTReader::Flush(void)
{
	m_bodyDest.Flush(*this);
	m_headerDest.Flush(*this);
	m_footerDest.Flush(*this);
//	Output()->putString(" $ef ");
	Output()->putString("");
}

//==========================================================================
// TXTDestination
// Base class for all destinations that convert to TXT
//==========================================================================
bool TXTDestination::HandleToken(RTFReader& reader, Token& token)
{
	bool handled = true;

	CharOutput* output = GetOutput();
	if (!output)
		output = reader.Output();
	m_writer.SetOutput(output);

	switch (token.getType())
	{
		case controlToken:
			handled = HandleControlToken(reader, (ControlToken&)token);
			break;
		case charToken:
			if ( m_bAdjustParagraphFormat )
			{
				m_bCharTokenReceived = TRUE;
				if ( m_bAddTabForNewParagraph )
				{	
				//	m_writer.Write(reader.State(), " $t " );
					m_writer.Write(reader.State(), "\t" );
				}
				m_bAddTabForNewParagraph = FALSE;
			}
			////TRACE("CH [%s]\n", token.text.c_str());
			m_writer.Write(reader.State(), token.GetText());
			break;
		default:
			////TRACE("TOK? [%s]\n", token.text.c_str());
			handled = false;
			break;
	}
	return(handled);
}

bool TXTDestination::HandleControlToken(RTFReader& reader, ControlToken& token)
{
	bool handled = true;

	switch (token.rtfMajor)
	{
		case rtfCharAttr:
			// handle bold/italic/underline/subscript/superscript
			handled = HandleRtfCharAttr(reader, token);
			////TRACE("C-CA [%s] %d\n", token.text.c_str(), token.arg);
			break;
		case rtfSpecialChar:
			// handle paragraph tabs emDash etc
			////TRACE("C-SC [%s] %d\n", token.text.c_str(), token.arg);
			handled = HandleRtfSpecialChar(reader, token);
			break;
		case rtfParAttr:
			//TRACE("C-PA [%s] %d\n", token.text.c_str(), token.arg);
			handled = HandleRtfParAttr(reader, token);
			break;
		case rtfSectAttr:
			//TRACE("C-SA [%s] %d\n", token.text.c_str(), token.arg);
			handled = HandleRtfSecAttr(reader, token);
			break;
		case rtfDocAttr:
			//TRACE("C-DA [%s] %d\n", token.text.c_str(), token.arg);
			handled = HandleRtfDocAttr(reader, token);
			break;
		default:
			//TRACE("C-?? [%s]\n", token.text.c_str());
			handled = false;
			break;
	}

	return(handled);
}

bool TXTDestination::HandleRtfCharAttr(RTFReader& reader, ControlToken& token)
{
	bool handled = true;

	switch (token.rtfMinor)
	{
		case rtfPlain:
			reader.State().CA().Reset();
			break;
		case rtfBold:
			reader.State().CA().bold = (token.arg == 0) ? 0 : 1;
			break;
		case rtfItalic:
			reader.State().CA().italic = (token.arg == 0) ? 0 : 1;
			break;
		case rtfUnderline:
		case rtfDotUnderline:
		case rtfDbUnderline:
			reader.State().CA().underline = (token.arg == 0) ? 0 : 1;
			break;
		case rtfNoUnderline:
			reader.State().CA().underline = 0;
			break;
		case rtfStrikeThru:
			reader.State().CA().strikeThru = (token.arg == 0) ? 0 : 1;
			break;
		case rtfStrikeThruDl:
		case rtfStrikeThruD:
			reader.State().CA().strikeThru = (token.arg == 0) ? 0 : 2;
			break;
		default:
			//TRACE("CH [%s]\n", token.text.c_str());
			handled = false;
			break;
	}
	return(handled);
}

bool TXTDestination::HandleRtfSpecialChar(RTFReader& reader, ControlToken& token)
{
	bool handled = true;

	switch (token.rtfMinor)
	{
		case rtfCell:
		case rtfRow:
			break;
		case rtfPar:
		case rtfLine:
			// Treat a new paragraph the same as a new line since
			// RTF doesn't indent a paragraph by default and
			// TXT does
			
			if( m_bAdjustParagraphFormat )
			{
				// when there is more than one new line char in a row.
				// One new line char must be removed, and at the beginning of 
				// the next text we must put two spaces to indicate a new paragraph	
				
				if ( m_bCharTokenReceived == FALSE )
				{  // it is the second or higher new line char in a row

					// by waiting for this flag to be set we cause only the first duplicate new line char not to be written
					if ( m_bAddTabForNewParagraph == TRUE )
					{    
					//	m_writer.Write(reader.State(), " $l ");	
						m_writer.Write(reader.State(), "\r\n");
					}
					// this flag is for notification that there is a new paragraph (more than 1 new line char) and indicates that an indent must be added 
					m_bAddTabForNewParagraph = TRUE;
				}
				else
				{
					// it is the first new line char in a row 
				//	m_writer.Write(reader.State(), " $l ");
					m_writer.Write(reader.State(), "\r\n");
				}
				
				m_bCharTokenReceived = FALSE;
			}
			else
			{
			//	m_writer.Write(reader.State(), " $l ");	
				m_writer.Write(reader.State(), "\r\n");		
			}

			// We have to force the left indent each time we start a newline
			// since RTF has this setting as persistent and TXT resets it
			// for each line.
			if (reader.State().PA().leftIndent > 0)
			{
//				char tmp[10];

//				sprintf(tmp, " $to%02d ", TwipsToColumns(reader.State().PA().leftIndent));
//				m_writer.Write(reader.State(), tmp);
				int iColumns = TwipsToTXTColumns(reader.State().PA().leftIndent);
				for ( int iColAdded = 0; iColAdded <= iColumns; iColAdded++)
				{
					m_writer.Write(reader.State(), " ");
				}
			}
			break;
		case rtfPage:
//			m_writer.Write(reader.State(), " $f ");
			m_writer.Write(reader.State(), "\f");
			break;
		case rtfTab:
//			m_writer.Write(reader.State(), " $t ");
			m_writer.Write(reader.State(), "\t");
			break;
		case rtfCurHeadDate:
//			m_writer.Write(reader.State(), " $dt ");
//			break;
		case rtfCurHeadTime:
//			m_writer.Write(reader.State(), " $tm ");
//			break;
		case rtfEmDash:
		case rtfEnDash:
		case rtfEmSpace:
		case rtfEnSpace:
		case rtfBullet:
		case rtfLQuote:
		case rtfRQuote:
		case rtfLDblQuote:
		case rtfRDblQuote:
		case rtfNoBrkSpace:
		default:
			handled = false;
			break;
	}

	return(handled);
}

bool TXTDestination::HandleRtfParAttr(RTFReader& reader, ControlToken& token)
{
	bool handled = true;

	switch (token.rtfMinor)
	{
		case rtfParDef:
			reader.State().PA().Reset();
			break;
		case rtfQuadLeft:
			reader.State().PA().justify = 0;
			break;
		case rtfQuadRight:
			reader.State().PA().justify = 1;
			break;
		case rtfQuadCenter:
			reader.State().PA().justify = 2;
			break;
		case rtfQuadJust:
			reader.State().PA().justify = 3;
			break;
		case rtfLeftIndent:
			reader.State().PA().leftIndent = token.arg;
			break;
		default:
			handled = false;
			break;
	}

	return(handled);
}

bool TXTDestination::HandleRtfSecAttr(RTFReader& reader, ControlToken& token)
{
	bool handled = true;

	SecAttr& attr = reader.State().SA();
	DocAttr& docAttr = reader.State().DA();

	switch (token.rtfMinor)
	{
		case rtfSectDef:
			attr.Reset();
			docAttr.pageNums = 0;
			break;
		case rtfPageDecimal:
			docAttr.pageNums = (token.arg == 0) ? 0 : 1;
			break;
		case rtfPageURoman:
		case rtfPageLRoman:
			docAttr.pageNums = (token.arg == 0) ? 0 : 2;
			break;
		case rtfSectMarginLeft:
			attr.leftMargin = token.arg;
			break;
		case rtfSectMarginRight:
			attr.rightMargin = token.arg;
			break;
		case rtfSectMarginTop:
			attr.topMargin = token.arg;
			break;
		case rtfSectMarginBottom:
			attr.bottomMargin = token.arg;
			break;
		default:
			handled = false;
			break;
	}

	return(handled);
}

bool TXTDestination::HandleRtfDocAttr(RTFReader& reader, ControlToken& token)
{
	bool handled = true;

	DocAttr& attr = reader.State().DA();

	switch (token.rtfMinor)
	{
		case rtfPaperWidth:
			attr.pageWidth = token.arg;
			break;
		case rtfPaperHeight:
			attr.pageHeight = token.arg;
			break;
		case rtfLeftMargin:
			attr.leftMargin = token.arg;
			break;
		case rtfRightMargin:
			attr.rightMargin = token.arg;
			break;
		case rtfTopMargin:
			attr.topMargin = token.arg;
			break;
		case rtfBottomMargin:
			attr.bottomMargin = token.arg;
			break;
		default:
			handled = false;
			break;
	}

	return(handled);
}

//==========================================================================
// TXTHeaderDestination
// Destination for the RTF header group
//==========================================================================
void TXTHeaderDestination::Begin(RTFReader& reader)
{
//	reader.Output()->putString(" $hb ");
}

void TXTHeaderDestination::End(RTFReader& reader)
{
//	reader.Output()->putString(" $- ");
}

//==========================================================================
// TXTFooterDestination
// Destination for the RTF footer group
//==========================================================================
void TXTFooterDestination::Begin(RTFReader& reader)
{
//	reader.Output()->putString(" $fb ");
}

void TXTFooterDestination::End(RTFReader& reader)
{
//	reader.Output()->putString(" $- ");
}

//==========================================================================
// TXTFieldInstrDestination
// Destination for RTF field instructions
//==========================================================================
void TXTFieldInstrDestination::Begin(RTFReader& reader)
{
	m_instruction.m_data.erase();
}

void TXTFieldInstrDestination::End(RTFReader& reader)
{
	if (m_instruction.m_data.size() < 5)
		return;

	if (m_instruction.m_data.compare(1, 4, "TIME") == 0)
	{
//		reader.Output()->putString(" $tm ");
	}
	else if (m_instruction.m_data.compare(1, 4, "DATE") == 0)
	{
//		reader.Output()->putString(" $dt ");
	}
}

CharOutput* TXTFieldInstrDestination::GetOutput(void)
{
	return(&m_instruction);
}

//==========================================================================
// TXTUserPropsDestination
// Destination for the RTF userprops group
//==========================================================================
bool TXTUserPropsDestination::HandleToken(RTFReader& reader, Token& token)
{
	bool handled = true;

	switch (token.getType())
	{
		case controlToken:
			{
				ControlToken& ctlToken = (ControlToken&)token;

				switch (ctlToken.rtfMajor)
				{
					case rtfUserPropAttr:
						switch (ctlToken.rtfMinor)
						{
							case rtfPropName:
								FlushProperty(reader);
								m_currOutput = &m_propName;
								break;
							case rtfPropType:
								m_currOutput = &m_ignore;
								break;
							case rtfPropValue:
								m_currOutput = &m_propValue;
								break;
							case rtfPropLink:
								FlushProperty(reader);
								break;
							default:
								handled = false;
								break;
						}
						break;
					default:
						handled = false;
						break;
				}
			}
			break;
		default:
			handled = false;
			break;
	}

	if (!handled)
		handled = TXTDestination::HandleToken(reader, token);
	return(handled);
}

void TXTUserPropsDestination::Begin(RTFReader& reader)
{
	m_currOutput = &m_ignore;
}

void TXTUserPropsDestination::End(RTFReader& reader)
{	
	FlushProperty(reader);
}

void TXTUserPropsDestination::FlushProperty(RTFReader& reader)
{
	if ((m_propName.m_data.size() > 0)
		&& (m_propValue.m_data.size() > 0))
	{
		int value = atoi(m_propValue.m_data.c_str());

		if (m_propName.m_data.compare("Caret position") == 0)
		{
			reader.State().DA().caretPosition = value;
		}
		else if (m_propName.m_data.compare("Mark position") == 0)
		{
			reader.State().DA().markPosition = value;
		}
		else if (m_propName.m_data.compare("Braille grade 2") == 0)
		{
			reader.State().DA().brailleGrade2 = value;
		}
		else if(m_propName.m_data.find("Bookmark") != m_propName.m_data.npos)
		{
			int i = 0;
			do
			{
				char szBuf[12];
				memset(szBuf, 0, sizeof(szBuf));
				sprintf_s(szBuf, "Bookmark%d", i);
				if(m_propName.m_data.compare(szBuf) == 0)
				{
					reader.State().DA().bookmark[i] = value;
					break;
				}
			}while(++i < 26);
		}
	}

	m_propName.m_data.erase();
	m_propValue.m_data.erase();
}

CharOutput* TXTUserPropsDestination::GetOutput(void)
{
	return(m_currOutput);
}

//==========================================================================
// TXTWriter
// Class that emulates a printer for braille codes. It keep track
// of all printing states such as italic, underline, etc.
//==========================================================================
TXTWriter::TXTWriter()
{
	m_output = NULL;
	m_bFirstTime = TRUE;
	m_bEmphasizedText = TRUE;
}

TXTWriter::~TXTWriter()
{
}

void TXTWriter::Write(RTFReaderState& state,LPCSTR value)
{
//	if ( m_bFirstTime )
//	{   // Check the Emphasized Text Registry setting, 
		// which was set was set in the Emboss Dialog of skipper.
//		HKEY hKey(NULL);
//		DWORD dwDisp(0L);
		
//		DWORD dwTemp = 0;
//		DWORD dwSize = _MAX_PATH;
//		BYTE* pDataDWD = reinterpret_cast<BYTE*>(&dwTemp);

//		if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("\\Software\\Freedom Scientific\\Emboss"), 0L, KEY_READ, &hKey) == ERROR_SUCCESS)
//		{
//			if (::RegQueryValueEx(hKey, _T("EmphasizedText"), NULL, NULL, pDataDWD, &dwSize) == ERROR_SUCCESS)
//			{
//				m_bEmphasizedText = dwTemp;
//			}
//		}		
//		RegCloseKey(hKey);
//		m_bFirstTime = FALSE;
//	}
/*
	if ( m_bEmphasizedText )
	{
		if (state.CA().bold != m_charAttr.bold)
		{
			if (state.CA().bold)
				m_output->putString(" $bb ");
			else
				m_output->putString(" $bf ");
			m_charAttr.bold = state.CA().bold;
		}

		if (state.CA().underline != m_charAttr.underline)
		{
			if (state.CA().underline)
				m_output->putString(" $ub ");
			else
				m_output->putString(" $uf ");
			m_charAttr.underline = state.CA().underline;
		}

		if (state.CA().italic != m_charAttr.italic)
		{
			if (state.CA().italic)
				m_output->putString(" $ib ");
			else
				m_output->putString(" $if ");
			m_charAttr.italic = state.CA().italic;
		}
	}

	if (state.CA().strikeThru != m_charAttr.strikeThru)
	{
		// Turn off current strikethru (if any)
		switch (m_charAttr.strikeThru)				
		{
			case 1:	// Single
				m_output->putString(" $sof ");
				break;
			case 2:	// Double
				m_output->putString(" $dbsf ");
				break;
		}

		// Turn on new strikethru (if any)
		switch (state.CA().strikeThru)
		{
			case 1:	// Single
				m_output->putString(" $sob ");
				break;
			case 2:	// Dbl
				m_output->putString(" $dbsb ");
				break;
		}

		m_charAttr.strikeThru = state.CA().strikeThru;
	}

	if (state.PA().justify != m_parAttr.justify)
	{
		switch (state.PA().justify)
		{
			case 0:	// Left
				m_output->putString(" $jn ");
				break;
			case 1:	// Right
				m_output->putString(" $jr ");
				break;
			case 2:	// Center
				m_output->putString(" $c ");
				break;
			case 3:	// Full justify
				m_output->putString(" $jf ");
				break;
		}
		m_parAttr.justify = state.PA().justify;
	}

	if (state.PA().leftIndent != m_parAttr.leftIndent)
	{
		char tmp[10];

		sprintf(tmp, " $to%02d ", TwipsToColumns(state.PA().leftIndent));
		m_output->putString(tmp);
		m_parAttr.leftIndent = state.PA().leftIndent;
	}

	if (state.DA().pageNums != m_docAttr.pageNums)
	{
		switch (state.DA().pageNums)
		{
			case 0:	// None
				m_output->putString(" $pnnp ");
				break;
			case 1:	// Decimal
				m_output->putString(" $pnar ");
				break;
			case 2:	// Roman
				m_output->putString(" $pnrn ");
				break;
		}
		m_docAttr.pageNums = state.DA().pageNums;
	}
*/	
/* For now this code is being removed because we are putting in all the brf settings 
   another place.

	// Since the braille text program doesn't support separate margin
	// settings for a section vs the doc we always store the current
	// margin values in our docAttr variable. Our secAttr variable
	// not used.
	// When setting the margins we used the section setting if 
	// it's not the default or use the setting in the document
	// for the current state

	int margin;

	// Left margin	
	margin = state.SA().leftMargin;
	if (margin == -1)
		margin = state.DA().leftMargin;
	if (margin != m_docAttr.leftMargin)
	{
		char tmp[10];
		sprintf(tmp, " $ml%02d ", TwipsToHorizMargin(margin));
		m_output->putString(tmp);
		m_docAttr.leftMargin = margin;
	}

	// Right margin
	margin = state.SA().rightMargin;
	if (margin == -1)
		margin = state.DA().rightMargin;
	if (margin != m_docAttr.rightMargin)
	{
		char tmp[10];
		sprintf(tmp, " $mr%02d ", TwipsToHorizMargin(margin));
		m_output->putString(tmp);
		m_docAttr.rightMargin = margin;
	}

	// Top margin
	margin = state.SA().topMargin;
	if (margin == -1)
		margin = state.DA().topMargin;
	if (margin != m_docAttr.topMargin)
	{
		char tmp[10];
		sprintf(tmp, " $mt%02d ", TwipsToVertMargin(margin));
		m_output->putString(tmp);
		m_docAttr.topMargin = margin;
	}

	// Bottom margin
	margin = state.SA().bottomMargin;
	if (margin == -1)
		margin = state.DA().bottomMargin;
	if (margin != m_docAttr.bottomMargin)
	{
		char tmp[10];
		sprintf(tmp, " $mb%02d ", TwipsToVertMargin(margin));
		m_output->putString(tmp);
		m_docAttr.bottomMargin = margin;
	}

	if (state.DA().pageHeight != m_docAttr.pageHeight)
	{
		char tmp[10];
		//sprintf(tmp, " $pl%02d ", TwipsToVertMargin(state.DA().pageHeight));
		sprintf(tmp, " $pl%02d ", 25);	// Hard coded for now
		m_output->putString(tmp);
		m_docAttr.pageHeight = state.DA().pageHeight;
	}

	//if (state.DA().pageWidth != m_docAttr.pageWidth)
	// For testing force the page width to be 34 columns
	if (m_docAttr.pageWidth != ColumnsToTwips(34))
	{
		state.DA().pageWidth = ColumnsToTwips(34);

		char tmp[10];
		sprintf(tmp, " $pw%02d ", TwipsToColumns(state.DA().pageWidth));
		m_output->putString(tmp);
		m_docAttr.pageWidth = state.DA().pageWidth;
	}
*/
	m_output->putString(value);
}

