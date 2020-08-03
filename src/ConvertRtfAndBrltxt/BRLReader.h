class BRLTXTWriter
{
protected:
	CharOutput* m_output;
	CharAttr m_charAttr;
	ParAttr m_parAttr;
	SecAttr m_secAttr;
	DocAttr m_docAttr;
	TableAttr m_tableAttr;

public:
	BRLTXTWriter();
	virtual ~BRLTXTWriter();

	void SetOutput(CharOutput* output) { m_output = output; }
	void Write(RTFReaderState& state, LPCSTR value);
private:
	BOOL m_bFirstTime;
	BOOL m_bEmphasizedText;
};

class BRLTXTDestination : public RTFDestination
{
protected:
	BRLTXTWriter m_writer;

public:
	virtual bool HandleToken(RTFReader& reader, Token& token);

protected:
	bool HandleRtfCharAttr(RTFReader& reader, ControlToken& token);
	bool HandleRtfSpecialChar(RTFReader& reader, ControlToken& token);
	bool HandleRtfParAttr(RTFReader& reader, ControlToken& token);
	bool HandleRtfSecAttr(RTFReader& reader, ControlToken& token);
	bool HandleRtfDocAttr(RTFReader& reader, ControlToken& token);
	bool HandleUserPropAttr(RTFReader& reader, ControlToken& token);
	bool HandleControlToken(RTFReader& reader, ControlToken& token);
};

class BRLTXTBodyDestination : public BRLTXTDestination
{
	bool IsBodyDest(void) { return(TRUE); }
};

class BRLTXTHeaderDestination : public BRLTXTDestination
{
public:
	virtual void Begin(RTFReader& reader);
	virtual void End(RTFReader& reader);
};

class BRLTXTFooterDestination : public BRLTXTDestination
{
public:
	virtual void Begin(RTFReader& reader);
	virtual void End(RTFReader& reader);
};

class BRLTXTFieldInstrDestination : public BRLTXTDestination
{
protected:
	CStringOutput m_instruction;

public:
	virtual void Begin(RTFReader& reader);
	virtual void End(RTFReader& reader);
	virtual CharOutput* GetOutput(void);
};

class BRLTXTUserPropsDestination : public BRLTXTDestination
{
protected:
	CStringOutput m_propName;
	CStringOutput m_propValue;
	CStringOutput m_ignore;
	CharOutput* m_currOutput;

public:
	virtual void Begin(RTFReader& reader);
	virtual void End(RTFReader& reader);
	virtual CharOutput* GetOutput(void);
	bool IsUserPropsDest(void) { return(TRUE); }	
	virtual bool HandleToken(RTFReader& reader, Token& token);

protected:
	void FlushProperty(RTFReader& reader);
};

class RTFtoBRLTXTReader : public RTFReader
{
protected:
	BRLTXTBodyDestination m_bodyDest;
	BRLTXTHeaderDestination m_headerDest;
	BRLTXTFooterDestination m_footerDest;
	BRLTXTFieldInstrDestination m_fldInstr;
	BRLTXTUserPropsDestination m_userProps;
	
public:
	virtual RTFDestination* NextDestination(int id);
	virtual void Flush(void);
};
