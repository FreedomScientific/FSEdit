class TxtReader
{
protected:
	CharOutput* m_output;

public:
	TxtReader();
	virtual ~TxtReader();

	virtual void ReadDocument(CharSource& source, CharOutput* out, CString * pStrDocProp = 0);
	CharOutput* Output(void) { return(m_output); }

protected:
	char NextChar(CharSource& src);

	virtual void HandleTag(CString tag) {}
};

class Txt2RTFReader : public TxtReader
{
public:
	virtual void ReadDocument(CharSource& source, CharOutput* out, CString * pStrDocProp = 0);

protected:
	virtual void HandleTag(CString tag);
};