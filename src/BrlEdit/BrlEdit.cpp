// BrlEdit.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f BrlEditps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include "initguid.h"
//#include "BrlEdit.h"

//#include "BrlEdit_i.c"
#include "BrlEditWnd.h"
#include "BrlEditCtrl.h"
#include "gtutility.h"
//#include "ResourceDLL_FSSDK.h"

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

static HINSTANCE s_hLibRichEdit = 0;
HINSTANCE g_hInst = 0;
static ATOM s_ClassAtom;
static WNDPROC s_lpfnWndProcRichEdit = 0;
static LRESULT CALLBACK BrlEditWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
static CBrlEdit *theBrlEditObj = NULL;

static bool CreateBrlEdit(HWND hwnd);
static bool DestroyBrlEdit();

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		_Module.Init(ObjectMap, (HINSTANCE)hInstance);
		g_hInst = (HINSTANCE)hInstance;
		
		s_hLibRichEdit = LoadLibrary(_T("msftedit.dll"));
		if (!s_hLibRichEdit)
			return FALSE;

		WNDCLASS wc;
		if (!GetClassInfo(s_hLibRichEdit,GetRichEditControlClassName(),&wc))
			return FALSE;
		s_lpfnWndProcRichEdit = wc.lpfnWndProc;
#ifndef UNDER_CE
		DisableThreadLibraryCalls((HINSTANCE)hInstance);
#endif
		wc.style = CS_GLOBALCLASS | CS_PARENTDC;
		wc.lpfnWndProc = BrlEditWindowProc;
		wc.hInstance = (HINSTANCE)hInstance;
		wc.hIcon = 0;
		wc.hCursor = 0;
		wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
		wc.lpszMenuName = 0;
		wc.lpszClassName = BRLEDIT_CLASS;
		s_ClassAtom = ::RegisterClass(&wc);
		break;
	case DLL_PROCESS_DETACH:
		UnregisterClass(BRLEDIT_CLASS,_Module.m_hInst);

		DestroyBrlEdit();

		if (s_hLibRichEdit)
			FreeLibrary(s_hLibRichEdit);

		_Module.Term();
		break;
	default:
		break;
	}

	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer();
	return S_OK;
}

static LRESULT CALLBACK BrlEditWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_CREATE:
		{
			if(CreateBrlEdit(hwnd))
			{
				BOOL bHandled = FALSE;
				CallWindowProc(s_lpfnWndProcRichEdit, hwnd, uMsg, wParam, lParam);
				return theBrlEditObj->OnCreate(uMsg,wParam,lParam,bHandled);
			}
		}
	case WM_DESTROY:
	case WM_CLOSE:
		DestroyBrlEdit();
	default:
		return CallWindowProc(s_lpfnWndProcRichEdit, hwnd, uMsg, wParam, lParam);
	}
}

static bool CreateBrlEdit(HWND hwnd)
{

	theBrlEditObj = new CBrlEdit;
	theBrlEditObj->SubclassWindow(hwnd);
	return true;
}

static bool DestroyBrlEdit()
{
	if(theBrlEditObj != NULL)
	{
		theBrlEditObj->UnsubclassWindow();
		if(::IsWindow(theBrlEditObj->m_hWnd))
			theBrlEditObj->DestroyWindow();
		delete theBrlEditObj;
		theBrlEditObj = NULL;
		return true;
	}
	return false;
}

int __declspec( dllexport ) Hibernate(BOOL hibernate)
{
	// This function is called when WM_HIBERNATE is received.
	// All apps that use the BrlEdit function should call it.
	// Currently we can't seem to get things to work properly when
	// the richedit DLL is unloaded to we're only unloading
	// the BrlTransCE DLL

	if(hibernate)
	{
		FreeLibrary(s_hLibRichEdit);
		s_hLibRichEdit = 0;
	}
	else
	{
		s_hLibRichEdit = LoadLibrary(_T("RICHED20.DLL"));
	}

	return(0);
}
