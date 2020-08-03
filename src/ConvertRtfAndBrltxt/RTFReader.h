class RTFReaderState;
class RTFReader;

class RTFDestination
{
public:
	RTFDestination();
	virtual ~RTFDestination();

	virtual void Begin(RTFReader& reader) {}
	virtual void End(RTFReader& reader) {}
	virtual bool HandleToken(RTFReader& reader, Token& token);
	virtual void Flush(RTFReader& reader) {}
	virtual CharOutput* GetOutput(void) { return(NULL); }
	virtual bool IsUserPropsDest(void) { return(FALSE); }
	virtual	bool IsBodyDest(void) { return(FALSE); }
protected:
	BOOL m_bCharTokenReceived;
	BOOL m_bAddTabForNewParagraph;
	BOOL m_bAdjustParagraphFormat;
};

class RTFIgnoreDest : public RTFDestination
{
	// Simple destination that just ignores everything
};

struct CharAttr
{
	unsigned int bold : 1;
	unsigned int allCaps : 1;
	unsigned int deleted : 1;
	unsigned int fontNum : 1;
	unsigned int fontSize : 1;
	unsigned int italic : 1;
	unsigned int outline : 1;
	unsigned int strikeThru : 2; // 0-none,1-single,2-double
	unsigned int underline : 1;
	unsigned int dotUnderline : 1;
	unsigned int dbUnderline : 1;
	unsigned int noUnderline : 1;
	unsigned int invisible : 1;
	unsigned int charStyleNum : 1;
	unsigned int charCharSet : 1;
	unsigned int language : 1;

public:
	CharAttr();
	void Reset(void);
};

struct ParAttr
{
	int leftIndent;	// In twips
	unsigned int justify : 2;	// 0-left,1-right,2-center,3-full

public:
	ParAttr();
	void Reset(void);
};

struct SecAttr
{
	// all margins are in twips
	int leftMargin;
	int rightMargin;
	int topMargin;
	int bottomMargin;

public:
	SecAttr();
	void Reset(void);
};

struct DocAttr
{
	// all margins and sizes are in twips
	int leftMargin;
	int rightMargin;
	int topMargin;
	int bottomMargin;
	int pageHeight;
	int pageWidth;

	// This setting is in a SecAttr in RTF but we need it in the doc
	unsigned int pageNums : 2;

	// Braille specific doc attributes
	long caretPosition;
	long markPosition;
	long brailleGrade2;
	long markEmphasisText;

	long bookmark[26];
public:
	DocAttr();
	void Reset(void);
};

struct TableAttr
{
	char filler;
};

class RTFReaderState
{
protected:
	RTFDestination* m_currDest;
	CharAttr m_charAttr;
	ParAttr m_parAttr;
	SecAttr m_secAttr;
	TableAttr m_tableAttr;
	static DocAttr m_docAttr;	// There are only one set of doc attr

public:
	RTFReaderState();
	RTFReaderState( RTFReaderState* state);
	virtual ~RTFReaderState();

	static void ResetDocAttributes(void);

	RTFDestination* Destination(void) { return(m_currDest); }
	void SetDestination(RTFDestination* dest);
	CharAttr& CA(void) { return(m_charAttr); }
	ParAttr& PA(void) { return(m_parAttr); }
	SecAttr& SA(void) { return(m_secAttr); }
	DocAttr& DA(void) { return(m_docAttr); }
	TableAttr& TA(void) { return(m_tableAttr); }
};

class RTFReader
{
protected:
	RTFIgnoreDest m_ignoreDest;
	RTFReaderState* m_currState;
	CharOutput* m_output;

public:
	RTFReader();
	virtual ~RTFReader();

	void ReadDocument(CharSource * source, CharOutput * out, BOOL bOnlyGetDocProperties = FALSE);
	RTFReaderState& State(void) { return(*m_currState); }
	CharOutput* Output(void) { return(m_output); }

	virtual RTFDestination* NextDestination(int id);
	virtual void Flush(void) {}
};






