class TXTWriter
{
protected:
	CharOutput* m_output;
	CharAttr m_charAttr;
	ParAttr m_parAttr;
	SecAttr m_secAttr;
	DocAttr m_docAttr;
	TableAttr m_tableAttr;

public:
	TXTWriter();
	virtual ~TXTWriter();

	void SetOutput(CharOutput* output) { m_output = output; }
	void Write(RTFReaderState& state, LPCSTR value);
private:
	BOOL m_bFirstTime;
	BOOL m_bEmphasizedText;
};

class TXTDestination : public RTFDestination
{
protected:
	TXTWriter m_writer;

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

class TXTBodyDestination : public TXTDestination
{
};

class TXTHeaderDestination : public TXTDestination
{
public:
	virtual void Begin(RTFReader& reader);
	virtual void End(RTFReader& reader);
};

class TXTFooterDestination : public TXTDestination
{
public:
	virtual void Begin(RTFReader& reader);
	virtual void End(RTFReader& reader);
};

class TXTFieldInstrDestination : public TXTDestination
{
protected:
	CStringOutput m_instruction;

public:
	virtual void Begin(RTFReader& reader);
	virtual void End(RTFReader& reader);
	virtual CharOutput* GetOutput(void);
};

class TXTUserPropsDestination : public TXTDestination
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
	virtual bool HandleToken(RTFReader& reader, Token& token);

protected:
	void FlushProperty(RTFReader& reader);
};

class RTFtoTXTReader : public RTFReader
{
protected:
	TXTBodyDestination m_bodyDest;
	TXTHeaderDestination m_headerDest;
	TXTFooterDestination m_footerDest;
	TXTFieldInstrDestination m_fldInstr;
	TXTUserPropsDestination m_userProps;
	
public:
	virtual RTFDestination* NextDestination(int id);
	virtual void Flush(void);
};
