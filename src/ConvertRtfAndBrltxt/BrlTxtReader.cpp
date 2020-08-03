#include "stdafx.h"
#include <string>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std;

#include "charsource.h"
#include "charoutput.h"
#include "BrlTxtReader.h"

BrlTxtReader::BrlTxtReader()
{
	m_output = NULL;
}

BrlTxtReader::~BrlTxtReader()
{
}

void BrlTxtReader::ReadDocument(CharSource& src, CharOutput* dest)
{
	m_output = dest;

	while (!src.endOfInput())
	{
		char c = NextChar(src);
		switch (c)
		{
			case 0:	// Found tag
				{
					// Found $ tag. Stream is at position after $
					CString tag;
					while (!src.endOfInput())
					{
						char next = src.getChar();
						if (next == ' ')
							break;	// tag stops at first space

						tag += next;
					}

					HandleTag(tag);
				}
				break;
			case '\\':	// Backslash
			case '{':
			case '}':
				// Need a leading backslash
				dest->putString("\\");
				dest->putChar(c);
				break;
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
			default:
				dest->putChar(c);
				break;
		}
	}
}

char BrlTxtReader::NextChar(CharSource& src)
{
	char c = src.getChar();
	if ((c == ' ') && !src.endOfInput())
	{
		// Found space, check for $
		char peek = src.getChar();
		if (peek == '$' && !src.endOfInput())
		{
			peek = src.getChar();  // check next 
			if (peek == '$' ) // just check that we are not dealing with a double dollar sign
			{  // if so allow the space to go thru, we will deal with double dollar next time...
				src.unGetChar(peek);
				return (c);
			}
			else
			{
				src.unGetChar(peek);
				return(0);	// Return 0 for tag found
			}
		}
		src.unGetChar(peek);
	}

	if (c == '$' && !src.endOfInput())
	{
			char peek = src.getChar();  // check next 
			if (peek == '$' ) // just check that if we are dealing with a double dollar sign
			{  // we convert to just one dollar sign
				return (c);
			} 
			// not a double dollar so we re-insert the char
			src.unGetChar(peek);

	}
	return(c);
}

void BrlTxt2RTFReader::ReadDocument(CharSource& source, CharOutput* out)
{
	//out->putString("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033{\\fonttbl{\\f0\\fswiss\\fcharset0 Arial;}}");
	out->putString("{\\rtf1\\ansi");
	out->putString("\\uc1\\pard\\f0 ");

	BrlTxtReader::ReadDocument(source, out);

	out->putString("\\par}");
}

void BrlTxt2RTFReader::HandleTag(CString tag)
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
		ConvertTags("dbsb", "striked1"),	// Doublestrike begin
		ConvertTags("dbsf", "striked0"),	// Doublestrike finished
		ConvertTags("pnar", "pgndec"),	// Page numbers arabic
		ConvertTags("pnrn", "pgnucrm"),	// Page number roman
		ConvertTags("sob", "strike"),		// Strike-out begin
		ConvertTags("sof", "strike0"),	// Strike-out finished
		ConvertTags("jn", "ql"),			// No justification
		ConvertTags("jf", "qj"),			// Full justification
		ConvertTags("jr", "qr"),			// Right justification
		ConvertTags("ub", "ul"),			// Underline begin
		ConvertTags("uf", "ul0"),			// Underline finished
		ConvertTags("ib", "i"),				// Italic begin
		ConvertTags("if", "i0"),			// Italic finished
		ConvertTags("bb", "b"),				// Bold begin
		ConvertTags("bf", "b0"),			// Bold finished
		ConvertTags("ml", "marglsxn", 5),	// Set left margin
		ConvertTags("mr", "margrsxn", 5),	// Set right margin
		ConvertTags("mt", "margtsxn", 5),	// Set top margin
		ConvertTags("mb", "margbsxn", 5),	// Set bottom margin
		ConvertTags("pl", "paperh", 5),	// Page length
		ConvertTags("pw", "paperw", 5),	// Page width
		ConvertTags("hb", "header", 2),	// Header begin
		ConvertTags("fb", "footer", 2),	// Footer begin
		ConvertTags("tm", timeFmt, 3),	// Insert current time
		ConvertTags("dt", dateFmt, 4),	// Insert current date
		ConvertTags("to", "li", 5),		// Line indent
		ConvertTags("l", "par"),		// Newline (treat as $p)
		ConvertTags("c", "qc"),				// Center current line
		ConvertTags("p", "par"),			// New paragraph
		ConvertTags("f", "page"),			// New page
		ConvertTags("t", "tab"),			// Tab
		ConvertTags("-", "}", 1),			// End of header/footer
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

