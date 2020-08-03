class CharSource  
{
public:
	virtual bool endOfInput() = 0 ; //{return true;};
	virtual char unGetChar( char ch ) = 0 ; // {return '\0';};
	virtual char getChar() = 0 ;  //{return '\0';};
	virtual WCHAR getwChar() =0;
	CharSource();
	~CharSource();
};

class CStringSource : public CharSource
{
protected:
	CString m_data;
	int m_currPos;

public:
	CStringSource(CString data);

	virtual bool endOfInput();
	virtual char unGetChar( char ch );
	virtual char getChar();
	virtual WCHAR getwChar();
};

class FileCharSource : public CharSource  
{
public:
	FileCharSource();
	FileCharSource( char * filename );
	FileCharSource( FILE * fileHandle );
	virtual ~FileCharSource();
	char unGetChar(char ch);
	char getChar();
	bool endOfInput();

private:
	FILE * f;
};

class HFileCharSource  : public CharSource
{
protected:
	HANDLE m_hdl;

public:
	HFileCharSource(HANDLE hdl);

	virtual bool endOfInput();
	virtual char unGetChar(char ch);
	virtual char getChar();
	virtual WCHAR getwChar();
};
class CCharBufferSource : public CharSource
{
protected:
	char	*m_Data;
	int m_nLen;
	int m_currPos;

public:
	CCharBufferSource(char *lpData,int nLen);

	virtual bool endOfInput();
	virtual char unGetChar( char ch );
	virtual char getChar();
	virtual WCHAR getwChar();
};
