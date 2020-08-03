#include "stdafx.h"
#include <string>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std;

#include "charsource.h"
#include "charoutput.h"
#include "TxtReader.h"

TxtReader::TxtReader()
{
	m_output = NULL;
}

TxtReader::~TxtReader()
{
}

void TxtReader::ReadDocument(CharSource& src, CharOutput* dest, CString * pStrDocProp)
{
	m_output = dest;
	CString tag;
	while (!src.endOfInput())
	{	
		char c = src.getChar();
		switch (c)
		{
			case '\t':
				{
					dest->putString("\\tab ");
				}
				break;
			case '\r':
				{
					dest->putString("\\par ");
					// Found \r see if it is \r\n.  If so put marker after \r\n.  
					if (!src.endOfInput())
					{
						char next = src.getChar();
						if (next == '\n')
							break;	// tag stops at first space
						else
							src.unGetChar( next );
					}
				}
				break;
			case '\n':
				{
					dest->putString("\\par ");
				}
				break;
			case '\f':
				{
					dest->putString("\\page ");
				}
				break;
			case '{':
			case '}':
				// Need a leading backslash
				dest->putString("\\");
				dest->putChar(c);
				break;
			default:
				if ( c <= 127 && c >= 0)
				{  // we are dealing with ANSI so no need for Unicode
					dest->putChar(c);
				}
				else
				{   // since we are dealing with a potential accented character
					// we will save as a Unicode
					char szString[20]={0};
					src.unGetChar(c);
					WCHAR wChar = src.getwChar();
					if (wChar > 0x7fff)
					{
						// needs work
						//wChar = wChar + 0x10000;
						sprintf_s(szString, "\\u-%i\\\'%02x", wChar, (unsigned char) c);
					} 
					else
					{
						sprintf_s(szString, "\\u%i\\\'%02x", wChar, (unsigned char) c);
					}
					dest->putString(szString);
				}
				break;
		}
	}
}

char TxtReader::NextChar(CharSource& src)
{
	char c = src.getChar();
	if ((c == ' ') && !src.endOfInput())
	{
		// Found space, check for BackSlash
		char peek = src.getChar();
		if (peek == '\\')
			return(0);	// Return 0 for tag found
		src.unGetChar(peek);
	}
	return(c);
}

void Txt2RTFReader::ReadDocument(CharSource& source, CharOutput* out, CString * pStrDocProp)
{
	//out->putString("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033{\\fonttbl{\\f0\\fswiss\\fcharset0 Arial;}}");
	USES_CONVERSION;
	out->putString("{\\rtf1\\ansi");
	out->putString("\\uc1\\pard\\f0 ");
	if ( pStrDocProp != NULL )
	{
		out->putString( T2A(*pStrDocProp) );
	}	
	TxtReader::ReadDocument(source, out);

	out->putString("\\par}");
}

void Txt2RTFReader::HandleTag(CString tag)
{
	class ConvertTags
	{
	public:
		char* from;
		char* to;
		int format;

		ConvertTags(char* fromTag = NULL, char* toTag = NULL, int formatCode = 0)
		{
			from = fromTag;
			to = toTag;
			format = formatCode;
		}
	};

	static char* timeFmt = "{\\field{\\*\\fldinst { TIME \\\\@ \"H:mm\" }}{\\fldrslt %d:%02d}}";
	static char* dateFmt = "{\\field{\\*\\fldinst { DATE \\\\@ \"M/d/yyyy\" }}{\\fldrslt %d/%d/%d}}";
	static ConvertTags convert[] =
	{
		// These must be sorted by the length of the tag (descending)
//		ConvertTags("dbsb", "striked1"),	// Doublestrike begin
//		ConvertTags("dbsf", "striked0"),	// Doublestrike finished
//		ConvertTags("pnar", "pgndec"),	// Page numbers arabic
//		ConvertTags("pnrn", "pgnucrm"),	// Page number roman
//		ConvertTags("sob", "strike"),		// Strike-out begin
//		ConvertTags("sof", "strike0"),	// Strike-out finished
//		ConvertTags("jn", "ql"),			// No justification
//		ConvertTags("jf", "qj"),			// Full justification
//		ConvertTags("jr", "qr"),			// Right justification
//		ConvertTags("ub", "ul"),			// Underline begin
//		ConvertTags("uf", "ul0"),			// Underline finished
//		ConvertTags("ib", "i"),				// Italic begin
//		ConvertTags("if", "i0"),			// Italic finished
//		ConvertTags("bb", "b"),				// Bold begin
//		ConvertTags("bf", "b0"),			// Bold finished
//		ConvertTags("ml", "marglsxn", 5),	// Set left margin
//		ConvertTags("mr", "margrsxn", 5),	// Set right margin
//		ConvertTags("mt", "margtsxn", 5),	// Set top margin
//		ConvertTags("mb", "margbsxn", 5),	// Set bottom margin
//		ConvertTags("pl", "paperh", 5),	// Page length
//		ConvertTags("pw", "paperw", 5),	// Page width
//		ConvertTags("hb", "header", 2),	// Header begin
//		ConvertTags("fb", "footer", 2),	// Footer begin
//		ConvertTags("tm", timeFmt, 3),	// Insert current time
//		ConvertTags("dt", dateFmt, 4),	// Insert current date
//		ConvertTags("to", "li", 5),		// Line indent
		
		ConvertTags("\r\n", "\\par"),		// Newline 
		ConvertTags("\r", "\\par"),			// Newline 
		ConvertTags("\n\r", "\\par"),		// Newline 
		ConvertTags("\n", "\\par"),			// Newline 
//		ConvertTags("c", "qc"),				// Center current line
//		ConvertTags("p", "par"),			// New paragraph
		ConvertTags("\f", "\\page"),		// New page
		ConvertTags("\t", "\\tab"),			// Tab
//		ConvertTags("-", "}", 1),			// End of header/footer
		ConvertTags()
	};

	for (int idx = 0; convert[idx].from; idx++)
	{
		int tagLen =static_cast<int>(strlen(convert[idx].from));

		if (tag.Left(tagLen) == convert[idx].from)
		{
			switch (convert[idx].format)
			{
				case 0:	// Standard command: '\' + string
					Output()->putChar('\\');
					Output()->putString(convert[idx].to);
					Output()->putChar(' ');
					break;
				case 1:	// String without '\' and space
					Output()->putString(convert[idx].to);
					break;
				case 2:	// New block plus command string
					Output()->putChar('{');
					Output()->putChar('\\');
					Output()->putString(convert[idx].to);
					Output()->putChar(' ');
					break;
				case 3:	// Time
					{
						char tmp[20];
						SYSTEMTIME systime;
						GetSystemTime(&systime);

						sprintf_s(tmp, convert[idx].to,
							systime.wHour, systime.wMinute);
						Output()->putString(tmp);
					}
					break;
				case 4:	// Date
					{
						char tmp[20];
						SYSTEMTIME systime;
						GetSystemTime(&systime);

						sprintf_s(tmp, convert[idx].to,
							systime.wMonth, systime.wDay, systime.wYear);
						Output()->putString(tmp);
					}
					break;
				case 5:	// Tag has an argument in columns. Convert to twips.
					{
						extern int ColumnsToTwips(int columns);

						Output()->putChar('\\');
						Output()->putString(convert[idx].to);

						int columns = _ttoi(tag.GetBuffer(0) + tagLen);
						char tmp[20];
						sprintf_s(tmp, "%d", ColumnsToTwips(columns));
						Output()->putString(tmp);
						Output()->putChar(' ');
					}
					break;
				default:
					break;
			}
		}
	}
}

