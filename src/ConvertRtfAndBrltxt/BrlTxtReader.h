class BrlTxtReader
{
protected:
	CharOutput* m_output;

public:
	BrlTxtReader();
	virtual ~BrlTxtReader();

	virtual void ReadDocument(CharSource& source, CharOutput* out);
	CharOutput* Output(void) { return(m_output); }

protected:
	char NextChar(CharSource& src);

	virtual void HandleTag(CString tag) {}
};

class BrlTxt2RTFReader : public BrlTxtReader
{
public:
	virtual void ReadDocument(CharSource& source, CharOutput* out);

protected:
	virtual void HandleTag(CString tag);
};