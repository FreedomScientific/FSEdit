#pragma once

#include "atlbase.h"

#define DefParam(v,p,d)	((v*)((BYTE*)&p+d))

namespace GTUtility
{
	static CComBSTR bstrFSOpt =  _T("\\Software\\Freedom Scientific\\Options");
	static CComBSTR bstrFSFile = _T("\\Software\\Freedom Scientific\\File");
	static CComBSTR bstrFSEmb =  _T("\\Software\\Freedom Scientific\\Emboss");
	static CComBSTR bstrFSPrint = _T("\\Software\\Freedom Scientific\\Print");
	static CComBSTR bstrFSShowConvertBrf = _T("\\Software\\Freedom Scientific\\ConvertBRF");
	static CComBSTR bstrFSFileExt = _T("\\Software\\Freedom Scientific\\FileExt");

	struct RegDataType
	{
		DWORD dwType;
		union 
		{
			DWORD dw;
			TCHAR sz[256];
		};
	
		bool bSuccess;
		DWORD dwSize;
	};

	inline static BYTE* InitRegistryValue(RegDataType& value, DWORD& dwSize)
	{
		
		if(REG_DWORD == value.dwType)
		{
			dwSize = sizeof(DWORD);
			return reinterpret_cast<BYTE*>(&value.dw);
		}
		else if(REG_SZ == value.dwType)
		{
			dwSize = static_cast<int>(_tcslen(value.sz)*sizeof(TCHAR));
			return reinterpret_cast<BYTE*>(&value.sz);
		}
		
		return NULL;
	}

	inline static bool SetRegistryValue(LPTSTR strKey, LPTSTR strClass, LPTSTR strVName, RegDataType& value)
	{
		HKEY hKey(NULL);
		DWORD dwDisp(0L);

		if(::RegCreateKeyEx(HKEY_LOCAL_MACHINE, strKey, NULL, strClass, 0L, KEY_WRITE, NULL, &hKey, &dwDisp) != ERROR_SUCCESS)
			return false;

		DWORD dwSize = 0L;
		const BYTE* pData = InitRegistryValue(value, dwSize);
		if(dwSize == 0L)
			return false;

		bool bSuccess = (::RegSetValueEx(hKey, strVName, 0L, value.dwType, pData, dwSize) == ERROR_SUCCESS);

		RegCloseKey(hKey);
		return bSuccess;
	}

	inline static bool GetRegistryValue(LPTSTR strKey, LPTSTR strVName, RegDataType& value)
	{
		HKEY hKey(NULL);

		if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKey, 0L, KEY_READ, &hKey) != ERROR_SUCCESS)
			return false;

		DWORD dwSize = 0L;
		BYTE* pData = InitRegistryValue(value, dwSize);
		// Clear out receiving buffers and set size of receiving buffer 
		// to the size of the value to be retrieved.
		value.dw = 0;
		_tcscpy_s( value.sz, _T(""));
		RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwSize, NULL, NULL);		

		bool bSuccess = (::RegQueryValueEx(hKey, strVName, NULL, &value.dwType, pData, &dwSize) == ERROR_SUCCESS);
		
		if ( value.dwType == REG_SZ && bSuccess && dwSize > 0)
		{// cut down the received value string to its proper size
			CString sTemp = value.sz;
			sTemp = sTemp.Left( dwSize/2 );
			_tcscpy_s( value.sz, sTemp);
		}
		value.dwSize = dwSize;
		RegCloseKey(hKey);
		return bSuccess;


	}

	template<typename T>
	bool GetRegistryEntry(HKEY hKEY, LPCTSTR strSubKey, DWORD* dwType, LPCTSTR strName, T* entry, DWORD* dwDataSize)
	{
		HKEY hKey(NULL);

		if(ERROR_SUCCESS != ::RegOpenKeyEx(hKEY, strSubKey, NULL, KEY_READ, &hKey))
			return false;

		BYTE* pData = reinterpret_cast<BYTE*>(entry);

		if(ERROR_SUCCESS != ::RegQueryValueEx(hKey, strName, NULL, dwType, pData, dwDataSize))
		{
			::RegCloseKey(hKey);
			return false;
		}

		::RegCloseKey(hKey);
		return true;
	}

	template<typename T>
	bool SetRegistryEntry(HKEY hKEY, LPCTSTR strSubKey, DWORD dwType, LPCTSTR strName, T* entry, DWORD dwDataSize)
	{
		HKEY hKey(NULL);

		if(ERROR_SUCCESS != ::RegOpenKeyEx(hKEY, strSubKey, NULL, KEY_WRITE, &hKey))
			return false;

		const BYTE* pData = reinterpret_cast<BYTE*>(entry);
		if(ERROR_SUCCESS != ::RegSetValueEx(hKey, strName, 0L, dwType, pData, dwDataSize))
		{
			::RegCloseKey(hKey);
			return false;
		}

		::RegCloseKey(hKey);
		return true;
	}

	template<typename T>
	bool SetRegistryEntryEx(HKEY hKEY, LPCTSTR strSubKey, DWORD dwType, LPCTSTR strName, T* entry, DWORD dwDataSize)
	{
		HKEY hKey(NULL);
		DWORD dwDisp(0L);

		if(ERROR_SUCCESS != ::RegCreateKeyEx(hKEY, strSubKey, NULL, NULL, 0L, KEY_WRITE, NULL, &hKey, &dwDisp))
			return false;

		const BYTE* pData = reinterpret_cast<BYTE*>(entry);
		if(ERROR_SUCCESS != ::RegSetValueEx(hKey, strName, 0L, dwType, pData, dwDataSize))
		{
			::RegCloseKey(hKey);
			return false;
		}

		::RegCloseKey(hKey);
		return true;
	}

	inline bool IsValidDotPattern(LPCTSTR szDotPattern)
	{
		if(!szDotPattern || !*szDotPattern)
			return false;

		bool l_bDots[8] = {0};
		for(LPCTSTR l_pcPtr = szDotPattern; l_pcPtr && *l_pcPtr; ++l_pcPtr)
		{
			switch(*l_pcPtr)
			{
			case L'1':
				if(l_bDots[0])
					return false;
				l_bDots[0] = true;
				break;
			case L'2':
				if(l_bDots[1])
					return false;
				l_bDots[1] = true;
				break;
			case L'3':
				if(l_bDots[2])
					return false;
				l_bDots[2] = true;
				break;
			case L'4':
				if(l_bDots[3])
					return false;
				l_bDots[3] = true;
				break;
			case L'5':
				if(l_bDots[4])
					return false;
				l_bDots[4] = true;
				break;
			case L'6':
				if(l_bDots[5])
					return false;
				l_bDots[5] = true;
				break;
			case L'7':
				if(l_bDots[6])
					return false;
				l_bDots[6] = true;
				break;
			case L'8':
				if(l_bDots[7])
					return false;
				l_bDots[7] = true;
				break;
			default:
				return false;
				break;
			}
		}

		return true;
	}

};

#define DwordRegParam(p,d) *DefParam(DWORD,p,d)
#define CStringRegParam(p,d) *DefParam(CString,p,d)

namespace Skipper
{
	template<typename F, typename S>
	struct _opts
	{
		F first;
		S second;
	};
	typedef _opts<LPTSTR, DWORD> SKIPPER_OPTS;

	// Options Dialog attributes / Registry values
	const SKIPPER_OPTS _Opts[] =
	{
		{_T("TDFormat"), REG_DWORD},
//		{_T("ParaNum"), REG_DWORD},
		{_T("SPDivider"), REG_DWORD},
		{_T("WordWrap"), REG_DWORD},
		{_T("Autosave"), REG_DWORD}
	};

	typedef _opts<LPTSTR, DWORD> _File;
	typedef _opts<LPTSTR, DWORD> _FileSZ;
	// Other Registry values
	const _File Braille2 = {_T("Braille2"), REG_DWORD};
	const _File LastFolder = {_T("Last Folder"), REG_SZ};
	const _File LastFileType = {_T("Last File Type"), REG_DWORD};
	
	typedef _opts<LPTSTR, DWORD> SKIPPER_EMB;
	
	// Embosser Settings / Registry values
	const SKIPPER_EMB _Emb[] =
	{
		// PERMANENT PROPERTIES
		{_T("EmbDoesNewLine"), REG_DWORD},
		{_T("EmbDoesNewPage"), REG_DWORD},		
		{_T("IntptMargins"), REG_DWORD},
		{_T("IntptPages"), REG_DWORD},
		{_T("CellsPerLine"), REG_DWORD},
		{_T("MaxCellsPerLine"), REG_DWORD},
		{_T("LinesPerPage"), REG_DWORD},
		{_T("MaxLinesPerPage"), REG_DWORD},
		{_T("TopMargin"), REG_DWORD},
		{_T("BottomMargin"), REG_DWORD},
		{_T("LeftMargin"), REG_DWORD},
		{_T("RightMargin"), REG_DWORD},
		{_T("Embosser"), REG_SZ},
		{_T("NetworkDevice"), REG_SZ},
		// Connection Dlg Parameters
		{_T("Connection"), REG_SZ},
		{_T("NetworkPort"), REG_SZ},
		{_T("IsNetworkEmb"),REG_DWORD}, 
		// TEMP VALUES
		{_T("NumberCopies"), REG_DWORD},
		{_T("Grade2"), REG_DWORD},  
		{_T("EmphasizedText"), REG_DWORD} 
	};

	const LPTSTR lpstrPageWidth = _T("PageWidth");
	const LPTSTR lpstrNumOfCopies = _T("NumOfCopies");
	const LPTSTR lpstrSaveFileExt = _T("SaveFileExt");
};

#define WM_RUNFUNC WM_USER+1


template<DWORD IDD_MSG_DLG, typename R = int, typename A = int, typename C = CWindow>
class TMessageBox : public CDialogImpl<TMessageBox<IDD_MSG_DLG,R,A,C> >
{
	void (*Func)();
	R (*TFunc)();

	void (*FuncA)(A);
	R (*TFuncA)(A);

	void (C::*CFunc)();
	R (C::*TCFunc)();

	void (C::*CFuncA)(A);
	R (C::*TCFuncA)(A);

	R& rValue;
	A aValue;
	R value;
	
	C* ptr;

	INT m_nType;
	DWORD m_dwFocus;
public:
	TMessageBox(void (*pfunc)()) : Func(pfunc), TFunc(NULL) {m_nType = 0;}
	TMessageBox(R (*pfunc)()) : TFunc(pfunc), Func(NULL), rValue(value) {m_nType = 0;}
	TMessageBox(R (*pfunc)(), R& r) : TFunc(pfunc), Func(NULL), rValue(r) {m_nType = 0;}

	TMessageBox(void (*pfunc)(A)) : FuncA(pfunc), TFuncA(NULL) {m_nType = 1;}
	TMessageBox(R (*pfunc)(A)) : TFuncA(pfunc), FuncA(NULL), rValue(value) {m_nType = 1;}
	TMessageBox(R (*pfunc)(A), R& r) : TFuncA(pfunc), FuncA(NULL), rValue(r) {m_nType = 1;}

	TMessageBox(void (C::*pfunc)(), C* p) : CFunc(pfunc), TCFunc(NULL), ptr(p) {m_nType = 2;}
	TMessageBox(R (C::*pfunc)(), C* p) : TCFunc(pfunc), CFunc(NULL), rValue(value) {m_nType = 2;}
	TMessageBox(R (C::*pfunc)(), R& r, C* p) : TCFunc(pfunc), CFunc(NULL), rValue(r) {m_nType = 2;}

	TMessageBox(void (C::*pfunc)(A), C* p) : CFuncA(pfunc), TCFuncA(NULL), ptr(p) {m_nType = 3;}
	TMessageBox(R (C::*pfunc)(A), C* p) : TCFuncA(pfunc), CFuncA(NULL), rValue(value), ptr(p) {m_nType = 3;}
	TMessageBox(R (C::*pfunc)(A), R& r, C* p) : TCFuncA(pfunc), CFuncA(NULL), rValue(r), ptr(p) {m_nType = 3;}

	enum {IDD = IDD_MSG_DLG};

	BEGIN_MSG_MAP(TMessageBox)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_RUNFUNC, OnRunFunction)
	END_MSG_MAP()

	int DoModal(DWORD dwFocus = 0)
	{
		m_dwFocus = dwFocus;
		return CDialogImpl<TMessageBox>::DoModal();
	}

	int DoModal(A val, DWORD dwFocus = 0)
	{
		m_dwFocus = dwFocus;
		aValue = val;
		return static_cast<int>(CDialogImpl<TMessageBox>::DoModal());
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PostMessage(WM_RUNFUNC);
		::SetFocus(GetDlgItem(m_dwFocus));
		return 0;
	}

	LRESULT OnRunFunction(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_nType == 3)
			(CFuncA != NULL) ? (ptr->*CFuncA)(aValue) : (rValue = (ptr->*TCFuncA)(aValue));
		else if(m_nType == 2)
			(CFunc != NULL) ? (ptr->*CFunc)() : (rValue = (ptr->*TCFunc)());
		else if(m_nType == 1)
			(FuncA != NULL) ? FuncA(aValue) : (rValue = TFuncA(aValue));
		else
			(Func != NULL) ? Func() : (rValue = TFunc());

		EndDialog(IDOK);
		return 0;
	}
};

template<class _T1, class _T2, class _T3> 
struct trip {
	typedef _T1 first_type;
	typedef _T2 second_type;
	typedef _T3 third_type;
	trip()
		: first(_T1()), second(_T2()), third(_T3()) {}
	trip(const _T1& _V1, const _T2& _V2, const _T3& _V3)
		: first(_V1), second(_V2), third(_V3) {}
	template<class U, class V, class W> trip(const trip<U, V, W> &p)
		: first(p.first), second(p.second), third(p.third) {}
	_T1 first;
	_T2 second;
	_T3 third;
};

template<class _T1, class _T2, class _T3> inline
bool __cdecl operator==(const trip<_T1, _T2, _T3>& _X, const trip<_T1, _T2, _T3>& _Y)
{return (_X.first == _Y.first && _X.second == _Y.second && _X.third == _Y.third);}

template<class _T1, class _T2, class _T3> inline
bool __cdecl operator!=(const trip<_T1, _T2, _T3>& _X, const trip<_T1, _T2, _T3>& _Y)
{return (!(_X == _Y)); }

template<class _T1, class _T2, class _T3> inline
trip<_T1, _T2, _T3> __cdecl make_trip(const _T1& _X, const _T2& _Y, const _T3& _Z)
{return (trip<_T1, _T2, _T3>(_X, _Y, _Z)); }

