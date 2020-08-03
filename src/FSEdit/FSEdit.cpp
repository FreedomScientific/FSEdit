// skipper.cpp : main source file for skipper.exe
//

#include "stdafx.h"
#include "resource.h"
#include "FSEdit.h"

#include "FSEditView.h"
#include "MainFrm.h"
#include "gtutility.h"
//#include <projects.h>


CAppModule _Module;
TCHAR szExeName[] = _T("PrgMutex");

int Run(LPTSTR lpstrCmdLine = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);
	
	CMainFrame wndMain;
	wndMain.SetCmdLine(CComBSTR(lpstrCmdLine));
	
	if(wndMain.CreateExtend() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}
	
	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();
	
	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	ATLASSERT(SUCCEEDED(hRes));
	
#if (_WIN32_IE >= 0x0300)
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(iccx);
	iccx.dwICC = ICC_BAR_CLASSES;	// change to support other controls
	BOOL bRet = ::InitCommonControlsEx(&iccx);
	bRet;
	ATLASSERT(bRet);
#else
	::InitCommonControls();
#endif

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));
	

	
	HINSTANCE hBrlEdit = LoadLibrary(_T("BrlEdit.dll"));

	ATLASSERT(hBrlEdit != NULL);
	int nRet = Run(lpstrCmdLine, nCmdShow);
	FreeLibrary(hBrlEdit);

	_Module.Term();
	::CoUninitialize();
	
	return nRet;
}

static BOOL IsLocationName(LPCTSTR name)
{
		return(FALSE);
}

void SetSavedLocation(LPCTSTR location)
{
	GTUtility::RegDataType type;
	type.dwType = REG_SZ;
	_tcscpy_s(type.sz, location);
	GTUtility::SetRegistryValue( _T("\\Software\\Freedom Scientific\\FSEdit"), NULL, _T("Last Location") , type);
}

void GetSavedLocation(CString& location)
{
	GTUtility::RegDataType type;
	type.dwType = REG_SZ;
	if (GTUtility::GetRegistryValue(_T("\\Software\\Freedom Scientific\\FSEdit"), _T("Last Location"), type))
	{
		location = type.sz;
	}
	else
		location.LoadString(IDS_MAIN_MEMORY);
}

void SetSavedFolder(LPCTSTR folder)
{
	GTUtility::RegDataType type;
	type.dwType = REG_SZ;
	_tcscpy_s(type.sz, folder);
	GTUtility::SetRegistryValue( _T("\\Software\\Freedom Scientific\\FSEdit"), NULL, _T("Last Folder") , type);
}

void GetSavedFolder(CString& folder)
{
	GTUtility::RegDataType type;
	type.dwType = REG_SZ;
	if (GTUtility::GetRegistryValue(_T("\\Software\\Freedom Scientific\\FSEdit"), _T("Last Folder"), type))
	{
		folder = type.sz;
	}
	else
		folder.LoadString(IDS_MY_DOCUMENTS);
}

void SetSavedFolderAndLocation(LPCTSTR rawPath)
{
	/* 
	**  \<location>\<Folder>\<File>
	**	location: Main memory (default), Storage Card, flash disk
	**	folder: \My Documents\X (My Documents implied if location not flash disk and another dir is specified)
	**/

	CString myDocuments;
	myDocuments.LoadString(IDS_MY_DOCUMENTS);
	CString mainMemory;
	mainMemory.LoadString(IDS_MAIN_MEMORY);

	CString path = rawPath;
	CString location;
	CString folder;
	CString tmp;

	// Get location
	path = path.Mid(1);
	location = path.SpanExcluding(_T("\\"));
	if (!IsLocationName(location))
		location = mainMemory;
	else
		path = path.Mid(location.GetLength() + 1);

	// Get folder		
	folder = path.SpanExcluding(_T("\\"));
	path = path.Mid(folder.GetLength() + 1);
	if (folder == myDocuments)
	{
		folder = path.SpanExcluding(_T("\\"));
		path = path.Mid(folder.GetLength() + 1);
		if (path.IsEmpty())
			folder.Empty();
	}

	SetSavedLocation(location);
	SetSavedFolder(folder);
}

