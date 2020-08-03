class CharOutput  
{
protected:
	size_t m_length;

public:
	virtual void putChar( char c );
	virtual int putString(LPCSTR lpszText);
	CharOutput();
	virtual ~CharOutput();
	size_t GetLength(void) { return(m_length); }
};

class FileCharOutput : public CharOutput  
{
public:
	int putString(LPCSTR lpszText);
	void putChar( char c );
	FileCharOutput( const char *filename );
	//FileCharOutput();
	virtual ~FileCharOutput();

private:
	FILE * fh;
};

class CStringOutput : public CharOutput  
{
public:
	string m_data;

public:
	virtual void putChar( char c );
	virtual int putString(LPCSTR lpszText);
};

class HFileCharOutput : public CharOutput  
{
public:
	HANDLE m_hdl;

public:
	HFileCharOutput(HANDLE hdl);
	virtual void putChar( char c );
	virtual int putString(LPCSTR lpszText);
};

class TrashCharOutput : public CharOutput  
{
public:
	virtual void putChar( char c );
	virtual int putString(LPCSTR lpszText);
};

