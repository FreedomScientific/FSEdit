// mainfrm.cpp
//

#pragma warning (disable:4018)

#include "stdafx.h"
#include "resource.h"
#include "FSEdit.h"
#include "FSEditView.h"
#include "atlextra.h"
#include "mainfrm.h"
//#include "aboutdlg.h"
#include "SpellChkDlg.h"
#include "SpellEdit.h"
#include "pagesetup.h"
//#include "HyperLinkDlg.h"
#include "NewFileDlg.h"
#include "format.h"
#include "gtutility.h"
#include <IBrailleTranslator.h>
#include "OpenFile.h"
#include "ErrorCodes.h"
#include "FSFindReplaceDlg.h"
//#include <uniqueid.h>
#include <commdlg.h>
#include <tlhelp32.h>
#include <algorithm>
#include <stdio.h>

#ifdef _UNICODE
#pragma message ("_UNICODE is defined")
#else
#pragma message ("_UNICODE is not defined")
#endif


/* static */ BOOL CBrfWarningDlg::IsBRF(CString strSaveAs, CString strCurrent)
{
	CString extAs = strSaveAs.Right(4);
	CString extCur = strCurrent.Right(4);

	if(extAs.CompareNoCase(_T(".brf")) == 0 && extCur.CompareNoCase(_T(".brf")) != 0)
		return TRUE;

	return FALSE;
}

/* static */ BOOL CBrfWarningDlg::ShowConvertDlg()
{
	return RegistryValue(FALSE);
}

/* static */ BOOL CBrfWarningDlg::RegistryValue(BOOL bSet)
{
	GTUtility::RegDataType type;
	type.dwType = REG_DWORD;
	if(bSet)
	{
		type.dw = 0L;
		GTUtility::SetRegistryValue(GTUtility::bstrFSShowConvertBrf, NULL, _T("Convert"), type);
	}
	else
	{
		if(GTUtility::GetRegistryValue(GTUtility::bstrFSShowConvertBrf, _T("Convert"), type) != ERROR_SUCCESS)
		{
			type.dw = 1L;
			GTUtility::SetRegistryValue(GTUtility::bstrFSShowConvertBrf, NULL, _T("Convert"), type);
			return TRUE;
		}
	}

	return type.dw;
}

static bool fileExists(const CString &fpath)
{
	bool rc = false;

	FILE *f = nullptr;
	_tfopen_s(&f,(const TCHAR *)fpath, _T("r"));
	if( f != NULL)
	{
		rc=true;
	    fclose(f);
	}
	return rc;
}

LRESULT CBrfWarningDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CString str, strMsg;
	str.LoadString(IDS_CONVERTBRF);
	strMsg.Format(str, m_strFile.GetBuffer());
	CStatic staConvertMsg;
	staConvertMsg.Attach(GetDlgItem(IDSTA_CONVERTBRF_MSG));
	staConvertMsg.SetWindowText(strMsg);

	CButton btn;
	btn.Attach(GetDlgItem(IDCHK_DONOTSHOW));
	btn.SetCheck(TRUE);
	return 0;
}

LRESULT CBrfWarningDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CButton btn;
	btn.Attach(GetDlgItem(IDCHK_DONOTSHOW));
	if(!btn.GetCheck())
		RegistryValue(TRUE);

	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CBrfWarningDlg::OnContinue(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CButton btn;
	btn.Attach(GetDlgItem(IDCHK_DONOTSHOW));
	if(!btn.GetCheck())
		RegistryValue(TRUE);
	
	EndDialog(IDOK);
	return 0;
}



#define SKIPPER			_T("FSEdit")
#define SKIPPER_DASH	_T("FSEdit - ")
#define CLASSNAME		_T("SKIPPER_WHEEL")

#define DEFAULT_LINELENGTH	9360


TCHAR m_szPrgName[] = _T("FSEditAppCounterSemaphore");
LPCTSTR CMainFrame::m_pszPrgMsg = _T("PrgMessage");

DocProp BrlDefault = {-1,-1,1,(CHARCOLUMN * 4),(CHARCOLUMN * 32),0,0,0,0,0};
DocProp TxtDefault = {-1,-1,0,(ONEINCH * 11),(long(ONEINCH * 8.5)),0,ONEINCH,ONEINCH,ONEINCH,ONEINCH};

//////////////////////////////////////////////////////////////////////////////
// CMainFrame functions



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	/* Commented out by waishan because the FindReplace dialog 
	is no longer a modeless dialog.

	// Good Fix by Glen for Find Replace Dialog box.
	if (m_pFDlg && m_pFDlg->IsDialogMessage(pMsg))
		return TRUE;
	*/

	return m_view.PreTranslateMessage(pMsg);
}

LRESULT CMainFrame::OnSpellChecker(WORD wLParam, WORD wHPara, HWND /*lParam*/, BOOL& bHandled)
{
	CString str;
	if(m_spl.DoChecking(m_view) == SPL_OK) // no error was found
	{
		str.LoadString(IDS_NO_ERRORS_FOUND);
	}
	else
	{
		str.LoadString(IDS_FINISHED_DOCUMENT);
	}

	SetFocus();

	MessageBox(str, _T(""), MB_OK|MB_ICONINFORMATION);
	return 0;
}


int CMainFrame::AppAlreadyOpened(bool bSPOpening /* = false */)
{
	m_hPrgSemaphore = CreateSemaphore(NULL, 2, 2, m_szPrgName);
	if (m_hPrgSemaphore == NULL)
		return(-1);	// Something bad happened. Don't allow instance to run.

	BOOL appAlreadyExists = (GetLastError() == ERROR_ALREADY_EXISTS);

	switch (WaitForSingleObject(m_hPrgSemaphore, 0))
	{
	case WAIT_OBJECT_0:
		if (appAlreadyExists)
		{
			//NKDbgPrintfW(L"Instance count = 1\r\n");
			return(1);
		}
		//NKDbgPrintfW(L"Instance count = 0\r\n");
		return(0);
	case WAIT_TIMEOUT:
	default:
		/**
		{
			HWND hWnd = FindWindow(CLASSNAME, NULL);
			if(hWnd)
			{
				// If a command-line argument was passed in, send it to the first
				// instance using WM_COPYDATA.
				// Afterward, bring the app to the foreground.
				if(strCmdLine.Length() != 0)
				{
					COPYDATASTRUCT pcds;
					pcds.dwData = 0;
					pcds.lpData = (void*)strCmdLine;
					pcds.cbData = (strCmdLine.Length()+1)*sizeof(_TCHAR);
					::SendMessage(hWnd, WM_COPYDATA, ID_FILE_OPEN, (LPARAM)(PCOPYDATASTRUCT) &pcds);
					//NKDbgPrintfW(L"Pass command line\r\n");
				}

				//NKDbgPrintfW(L"Bring prev to foreground\r\n");
				SetForegroundWindow((HWND)((ULONG) hWnd | 0x00000001));
			}
		}
		**/
		//NKDbgPrintfW(L"Instance count = 2\r\n");
		return(2);
	}
}

HWND CMainFrame::CreateExtend(HWND hWndParent /* = NULL */, _U_RECT rect /* = NULL */, DWORD dwStyle /* = 0 */, DWORD dwExStyle /* = 0 */, LPVOID lpCreateParam /* = NULL */)
{

	m_bSPOpened = false;
	m_instancesOpenedCount = 0;

	GetWndClassInfo().m_wc.lpszClassName = CLASSNAME;

	m_instancesOpenedCount = AppAlreadyOpened();


	return CreateEx(hWndParent, rect, dwStyle, dwExStyle, lpCreateParam);
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if(m_instancesOpenedCount >= 2)
	{
		PostQuitMessage(0);
		return 0;
	}

	SetWindowText(SKIPPER);
	RegistryValues(false);
	LoadFolders()();
	m_strDir = COpenFile::GetLastDirectory(); //DEF_DIRECTORY;

	usingDouble = false; // new spacing stuff

	CreateSimpleStatusBar();
	// needs work to create menu
	RECT rc;
	GetClientRect(&rc);
	MoveWindow(rc.left+1, rc.top+28, rc.right+2, rc.bottom-24);

	m_hWndClient = m_view.CreateExtended(m_hWnd, rcDefault, NULL,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_MULTILINE | ES_NOHIDESEL, WS_EX_CLIENTEDGE);

	// Set to word wrap
	m_view.SetTargetDevice(NULL, 0);
	m_pFDlg = NULL;
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

		vecUIParams.reserve(NUM_UIPARAMS);
	copy(UIParams, UIParams+NUM_UIPARAMS, back_inserter(vecUIParams));
	m_bBulletNum = false;
	m_bVisible = TRUE;
	m_bCanRevert = false;
	m_bAutoDetectLink = false;
	m_bFileExplorer = false;
	// If m_dwParaNum has not been initialized, set it to 1.
//	if ((m_dwParaNum <= 0) || (m_dwParaNum >= NUM_NUMBERING))
//		m_dwParaNum = 1;

	m_view.SetEventMask(m_view.GetEventMask() | ENM_LINK);

		if(m_bSPOpened && (m_instancesOpenedCount >= 2))
		{
			PostQuitMessage(0);
			return 0;
		}

///////////////////////////////
// 
		CHARFORMAT cf;
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_SIZE;

		// Getting original Formats
		m_view.GetDefaultCharFormat(cf);

		// Put in explicit cast to long to stop compiler warnings
		cf.yHeight = (long)(POINT_SIZE * 10);
		m_view.SetDefaultCharFormat(cf);
///////////////////////////////

		if(strCmdLine.Length() != 0)
		{
			CString strtemp(strCmdLine.m_str);
			int f = 0;
			if((f = strtemp.ReverseFind(' ')) != -1)
			{
				m_strFile = strtemp.Left(f);
				if(_tcsstr(strCmdLine.m_str, _T("-r")))
					m_view.SetReadOnly();
				else
					m_strFile = strCmdLine.m_str;
			}
			else
				m_strFile = strCmdLine.m_str;

//			m_view.OpenFile(m_strFile);
//			SetCursorPosition(m_strFile);


			m_view.SetModify(false);
			m_bCanRevert = true;
			m_bNewFile = false;
			PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)m_strFile));
		}
	// Set the JAWS window as the notify window for special events.
	// This allows a close communication between Skipper and JAWS.
#ifdef _WIN32_WCE
	HWND hWnd = ::FindWindow(_T("wce_jfw"),NULL);
#else
	HWND hWnd = ::FindWindow(_T("JFW"),NULL);
#endif // _WIN32_WCE
	m_view.SetNotify(hWnd);

	::ShowWindow(m_hWndStatusBar, SW_HIDE);

	m_view.SetTargetDevice(GetDC(), m_dwPrintWidth);

	m_view.SetUndoLimit(25);
//	m_view.InitNetworkPrinters();
	m_bInitialName = true;

	m_bCreate = false;
//	if(!(strCmdLine.Length() != 0 && _tcsstr(strCmdLine.m_str, _T("-s"))))
	if(strCmdLine.Length() == 0 )
	{
		m_bCreate = true;
		::PostMessage(m_hWnd, WM_COMMAND, MAKELONG(ID_FILE_NEW, 0), 1);
	}
	else
	{
			m_bFileExplorer = true;
			PostMessage(WM_COMMAND, MAKEWPARAM(ID_FILE_OPEN, 0), 0L);
//			m_bFileExplorer = false;
	}


	// prepare for the special modes
    m_view.setMyOwner(this);

	
	return 0;
}

void CMainFrame::RegistryValues(bool bSet /* = true */)
{
	if(bSet)
	{
		GTUtility::RegDataType type;
		type.dwType = Skipper::Braille2.second;
		type.dw = CNewFileDlg::m_bBraille2 ? 1L : 0L;
		GTUtility::SetRegistryValue(GTUtility::bstrFSOpt, NULL, Skipper::Braille2.first, type);

		type.dwType = Skipper::LastFolder.second;
		CString str = COpenFile::GetLastFolder();
		int n = str.GetLength();
		_tcscpy_s(type.sz, str.GetBuffer(n));;
		str.ReleaseBuffer(n);
		GTUtility::SetRegistryValue(GTUtility::bstrFSFile, NULL, Skipper::LastFolder.first, type);

		type.dwType = Skipper::LastFileType.second;
		type.dw = COpenFile::GetLastFileType();
		GTUtility::SetRegistryValue(GTUtility::bstrFSFile, NULL, Skipper::LastFileType.first, type);

		DWORD dwType = REG_DWORD, dwSize = sizeof(DWORD);
		GTUtility::SetRegistryEntryEx(HKEY_LOCAL_MACHINE, GTUtility::bstrFSPrint, dwType, Skipper::lpstrPageWidth, &m_dwPrintWidth, dwSize);
		GTUtility::SetRegistryEntryEx(HKEY_LOCAL_MACHINE, GTUtility::bstrFSPrint, dwType, Skipper::lpstrNumOfCopies, &m_dwNumOfCopies, dwSize);
		
		DWORD dwFileExt = m_wFileExt;
		GTUtility::SetRegistryEntryEx(HKEY_LOCAL_MACHINE, GTUtility::bstrFSFileExt, dwType, Skipper::lpstrSaveFileExt, &dwFileExt, dwSize);
	}
	else
	{
		GTUtility::RegDataType type;
		for(int x = 0; x < NUM_OPTATTR; x++)
		{
			type.dwType = Skipper::_Opts[x].second;
			if(GTUtility::GetRegistryValue(GTUtility::bstrFSOpt,Skipper::_Opts[x].first, type))
				DwordRegParam(*this, aOptAttr[x]) = type.dw;
			else
				DwordRegParam(*this, aOptAttr[x]) = (x < 2) ? 0L : 1L;
		}

		type.dwType = Skipper::Braille2.second;
		if(GTUtility::GetRegistryValue(GTUtility::bstrFSOpt,Skipper::Braille2.first, type))
			CNewFileDlg::m_bBraille2 = (type.dw == 1);

		type.dwType = Skipper::LastFolder.second;
		if(GTUtility::GetRegistryValue(GTUtility::bstrFSFile, Skipper::LastFolder.first, type))
			COpenFile::SetLastFolder(type.sz);

		type.dwType = Skipper::LastFileType.second;
		if(GTUtility::GetRegistryValue(GTUtility::bstrFSFile, Skipper::LastFileType.first, type))
		{
			if(IDS_SKIPPER_TEXT <= type.dw && type.dw <= IDS_ALL_FILES)
				COpenFile::SetLastFileType(type.dw);
			else
				COpenFile::SetLastFileType(IDS_SKIPPER_TEXT);
		}

		DWORD dwType = 0L, dwSize = sizeof(DWORD);
		if(!GTUtility::GetRegistryEntry(HKEY_LOCAL_MACHINE, GTUtility::bstrFSPrint, &dwType, Skipper::lpstrPageWidth, &m_dwPrintWidth, &dwSize))
			m_dwPrintWidth = DEFAULT_LINELENGTH;

		if(!GTUtility::GetRegistryEntry(HKEY_LOCAL_MACHINE, GTUtility::bstrFSPrint, &dwType, Skipper::lpstrNumOfCopies, &m_dwNumOfCopies, &dwSize))
			m_dwNumOfCopies = 1;

		DWORD dwFileExt;
		if(GTUtility::GetRegistryEntry(HKEY_LOCAL_MACHINE, GTUtility::bstrFSFileExt, &dwType, Skipper::lpstrSaveFileExt, &dwFileExt, &dwSize))
			m_wFileExt = (unsigned short)dwFileExt;
		else
			m_wFileExt = 0;

	}
}

bool CMainFrame::CheckToSave(bool bOpen /* = false */, bool bRecent /* = false */, bool bExit /* = false */, bool bActivate /* = false */)
{
	CHARRANGE cr;
	m_view.GetSel(cr);
	CString strFile = m_strFile;

	CShellFileOpenDialog dlg;
	if(m_view.GetModify()) {// Has the document been modified?
		CString str, strMsg, strDlgTtl;
		str.LoadString(IDS_SAVE_FILE);
		strDlgTtl.LoadString(IDS_APP_NAME);
		int n = IDYES;

		bool bNew = (!bOpen && !bRecent && !bExit && !bActivate);
		 
		// Unless focus has changed or user is exiting app, prompt user.
		if(bExit || bOpen || bNew || bRecent) {
			// Ask the user if he wants to save the document.
			n = MessageBox(str, strDlgTtl, MB_ICONQUESTION | MB_YESNOCANCEL);
		}

		if(n == IDYES ) {
			bool bSave = (bOpen || bExit || bNew || bActivate || bRecent);
			// If exiting and no filename is assigned to contents, 
			// get a filename one.
			if ((bExit || bActivate) && m_strFile.IsEmpty()){
				TCHAR szNewFile[_MAX_PATH] = {0};
				if (!GetNewFileName(szNewFile))
					return false;
				
				strFile = m_strFile = szNewFile;
				int nT = 1;
				OnFileSave(0, 0, NULL, nT);
				return true;
			}

			if(bSave)	{
				// Prompt to save before opening another document.
				// If user cancels, cancel the whole operation.
				int nT = 1;
				if(OnFileSave(0, 0, NULL, nT)) {// save the file the original filed

					if(!bOpen)				// If we are not opening another file - quit.
						return true;
				} else {  // save operation failed
					// something went wrong with the save operation... for example say
					// it failed the check to prevent the use of the same file by 
					// both fs edit instances which is done during OnFileSave(...)
				    return false; // this aborts the whole thing... user needs to start again
				}
			}

			if(bOpen ) 	{
				if (dlg.DoModal() == IDCANCEL)
				    return false;
			}
		} else if(n == IDNO) {	// Don't bother to save current document.
			if(bOpen ) {
				if (dlg.DoModal() == IDCANCEL)
				    return false;
				}

		} else  {// n == IDCANCEL
			return false;
		}
	}
	else	// If already saved or document is empty.
	{
		if(bOpen)
		{
			if (dlg.DoModal() == IDCANCEL) 
				return false;
			CString filePath;
			dlg.GetFilePath(filePath);
						AddToMRUList(filePath);
		}
	}

	// If opening a file from Open or the Recent File List, clear screen and 
	// update view.  Else, rely on the operation to update the view because the 
	// user may cancel and clearing the screen now would be bad.  Do not need
	// to clear the screen for focus changes.
	if (bOpen || bRecent)
	{
		m_view.SetWindowText(CString());	// Clear View text buffer.
		dlg.GetFilePath(m_strFile);
		m_view.SetModify(false);			// Set the modified flag.
	}
	m_view.Invalidate();				// Update the view
	
	
	m_bFileExplorer = false;

	return true;
}

LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	return 0;
}


LRESULT CMainFrame::OnFileNew(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_dwAutosave)
	{
		if(!CheckToSave())
			return 0;
	}

	CNewFileDlg dlg;
	DocProp Prop;
		if(dlg.DoModal() == IDCANCEL)
		{
			// If user just opened this app and chose Cancel, exit app.
			// Else, just cancel the File New operation.
			if(m_bInitialName)
			{
				SendMessage(WM_CLOSE, 0, 0);
				return 0;
			}
			else
				return 0;
		}
		memcpy(&Prop, (CNewFileDlg::GetFileExt()>2 ? &TxtDefault : &BrlDefault), sizeof(DocProp));
		Prop.brailleGrade2 = CNewFileDlg::m_bBraille2;
		m_strDir = dlg.GetDirectory();
	m_view.SetDocumentProp(Prop);
	
	m_bInitialName = false;
	m_view.SetWindowText(CString());	// Clear View text buffer.
	m_view.Invalidate();				// Update the view
	m_view.SetModify(false);			// Set the modified flag.

#ifdef BOOKMARK_SUPPORT
	m_view.DeleteAllBookMarks();
#endif
	COpenFile::SetLastFolder(m_strDir);
	m_strFile.Empty();
	CString str;
	str.LoadString(IDS_NEW_FILE);
//	SetTitleBar(str);
	PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)str));
	m_view.SetModify(false);
	m_bCanRevert = false;
	m_bNewFile = true;

    	NotifyBrailleGradeChng();

	
	return 0;
}

	LRESULT CMainFrame::OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!CheckToSave(m_bFileExplorer != true))
		return 0;

	m_bFileExplorer = false;

#ifdef BOOKMARK_SUPPORT
	m_view.DeleteAllBookMarks();
#endif
	m_strDir = COpenFile::GetLastDirectory();
	m_strFile.TrimRight(); m_strFile.TrimLeft();
	m_view.OpenFile(m_strFile);
	
	PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)m_strFile));

	m_view.SetModify(false);
	m_bCanRevert = true;
	m_bNewFile = false;

	return 0;
}



LRESULT CMainFrame::OnRevertToSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CString strTitle, strMsg;
	strTitle.LoadString(IDS_TITLE_REVERT_TO_SAVE);
	strMsg.LoadString(IDS_REVERT_TO_SAVE);
	if(::MessageBox(m_hWnd, strMsg, strTitle, MB_YESNO | MB_ICONQUESTION) == IDNO)
		return 0;

	m_view.SetWindowText(CString());	// Clear View text buffer.
	m_view.Invalidate();				// Update the view

#ifdef BOOKMARK_SUPPORT
	m_view.DeleteAllBookMarks();
#endif
	m_view.OpenFile(m_strFile);
	m_strFile.ReleaseBuffer();

	PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)m_strFile));
	GetDirectoryFromPath();

	long lPoint = m_view.GetCaret();
	m_view.SetSel(lPoint, lPoint);
	m_view.SetModify(false);
	m_bCanRevert = true;
	return 0;
}

void CMainFrame::BuildFilePath(CString& path)
{
	CString myDocuments;
	myDocuments.LoadString(IDS_MY_DOCUMENTS);
	CString mainMemory;
	mainMemory.LoadString(IDS_MAIN_MEMORY);

	CString location;
	CString folder;

	GetSavedLocation(location);
	GetSavedFolder(folder);

	if (!location.IsEmpty() && !folder.IsEmpty())
	{
		if (location != mainMemory)
		{
			path += _T("\\");
			path += location;
		}

		if ((folder != myDocuments) && (location != _T("flash disk")))
		{
			path += _T("\\");
			path += myDocuments;
		}

		CString strFolderNone;
		strFolderNone.LoadString(IDS_NONE);

		if (folder != strFolderNone)
		{
			path += _T("\\");
			path += folder;
		}
	}

	//NKDbgPrintfW(L"BFP [%s]\n", path);
}

LRESULT CMainFrame::OnFileSave(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
	if(m_strFile.IsEmpty()) // If there is no name for the file go get one.
	{
		CString path;
		GetNewFileName(path.GetBuffer(_MAX_PATH));
		path.ReleaseBuffer();

		static TCHAR szFilter[ _MAX_PATH * 10 ];		
		LoadFileTypeFilter(szFilter);

		OPENFILENAME of;
		memset(&of, 0, sizeof(of));
		// 255 characters for file name +1 character for terminating null.
		TCHAR szFileName[256] = {0};
		_tcscpy_s(szFileName, path);
		of.lStructSize = sizeof(of);
		of.hwndOwner = m_hWnd;
		of.lpstrFile = szFileName;
		of.nMaxFile = 255;
		of.lpstrFilter = szFilter;
		of.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		  

        SetSavedFolderAndLocation(szFileName);

		m_strFile = szFileName;		
	}

	WIN32_FIND_DATA fd;
	if(FindFirstFile(m_strFile, &fd) != INVALID_HANDLE_VALUE)
	{
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
		{
			CString str, str2;
			str.LoadString(IDS_FILE_READONLY_MSG);
			str2.LoadString(IDS_FILE_READONLY);
			if(MessageBox(str, str2, MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				fd.dwFileAttributes &= ~FILE_ATTRIBUTE_READONLY;
				SetFileAttributes(m_strFile, fd.dwFileAttributes);
			}
			else
				return 0;
		}
	}
 
	long lPos = 0;
	// Scratch Pad
		{
		DocProp Prop;
		memset(&Prop, 0, sizeof(Prop));
		m_view.GetDocumentProp(Prop);

		// BRF files must be saved as grade 2
		CString l_csBRFExtension;
		CString l_csFileExt;
		l_csBRFExtension.LoadString(IDS_BRF_EXT);
		l_csBRFExtension = l_csBRFExtension.Right(3);
		l_csFileExt = m_strFile.Right(3);
		if(l_csBRFExtension.CompareNoCase(l_csFileExt) == 0)
			Prop.brailleGrade2 = 1;

		CHARRANGE cr;
		m_view.GetSel(cr);
		lPos = Prop.caretPosition = cr.cpMax;
		Prop.markPosition = m_view.GetMarker();
		m_view.SetDocumentProp(Prop);
	}

	if(m_strFile.Find(_T('\\')) == -1)
		m_strFile = m_strDir + m_strFile;

	m_view.SaveFile(m_strFile.GetBuffer(m_strFile.GetLength()), m_bNewFile);
	m_strFile.ReleaseBuffer();

	// Scratch Pad
	AddToMRUList(m_strFile);

	SetTitleBar(m_strFile);
	PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)m_strFile));
	m_view.SetModify(false);
	m_bCanRevert = true;
    
	m_view.SetSel(lPos, lPos);
	return 1;
}

void CMainFrame::GetDirectoryFromPath()
{
	if(m_strFile.Find(_T('\\')) == -1)
		m_strFile = m_strDir + m_strFile;
	else
	{
		int n = m_strFile.ReverseFind(_T('\\'));
		CString strtemp = m_strFile.Left(n+1);
		while((n = strtemp.Find(_T('\\'), n)) != -1)
		{
			strtemp.Insert(n, '\\');
			n += 2;
		}

		m_strDir = strtemp;
	}
}


LRESULT CMainFrame::OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OPENFILENAME of;
	memset(&of, 0, sizeof(of));
		// 255 characters for file name +1 character for terminating null.
	TCHAR szFileName[256] = {0};

	// Scratch Pad
	CString strFile;
		memcpy(szFileName, m_strFile.GetBuffer(m_strFile.GetLength()), m_strFile.GetLength()*sizeof(TCHAR));
		m_strFile.ReleaseBuffer();

	CString sOriginalFileName = szFileName;

	static TCHAR szFilter[ _MAX_PATH * 10 ];		
	LoadFileTypeFilter(szFilter);

	if (m_strFile.IsEmpty())
	{
		CString path;
		GetNewFileName(path.GetBuffer(_MAX_PATH));
		path.ReleaseBuffer();
		_tcscpy_s(szFileName, path);
	}
	else
		_tcscpy_s(szFileName, m_strFile);
	
	of.lStructSize = sizeof(of);
	of.hwndOwner = m_hWnd;
	of.lpstrFile = szFileName;
	of.nMaxFile = 255;
	of.lpstrFilter = szFilter;
	of.lpstrDefExt = COpenFile::GetLastExtension(); //str;
	of.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	of.nFilterIndex = m_wFileExt;

	CHARRANGE cr;
	m_view.GetSel(cr);
	strFile = m_strFile;

	
	SetSavedFolderAndLocation(szFileName);

	m_wFileExt = (unsigned short) of.nFilterIndex;

	if(CBrfWarningDlg::IsBRF(szFileName, m_strFile) && CBrfWarningDlg::ShowConvertDlg())
	{
		if(m_strFile.IsEmpty() && m_view.GetTextLength() == 0)
		{
			m_strFile = szFileName;
			PostMessage(WM_COMMAND, ID_FILE_SAVE, 0);
			return 0;
		}

		
		CBrfWarningDlg dlg(m_strFile);
		if(dlg.DoModal() == IDCANCEL)
			return 0;
	}


	m_strFile = szFileName;
	GetDirectoryFromPath();
	DocProp Prop;
	memset(&Prop, 0, sizeof(Prop));
	m_view.GetDocumentProp(Prop);

//	CHARRANGE cr;
	m_view.GetSel(cr);
	long lPos = Prop.caretPosition = cr.cpMax;
	Prop.markPosition = m_view.GetMarker();
	m_view.SetDocumentProp(Prop);

	if( !m_view.SaveFileAs(m_strFile.GetBuffer(m_strFile.GetLength()), m_bNewFile))
	{
		m_strFile = sOriginalFileName;
	}
	m_strFile.ReleaseBuffer();

	// Scratch Pad
		AddToMRUList(m_strFile);
		
	{
		nLenSPad = m_view.GetWindowTextLength();
		m_strFile = strFile;
	}

//	SetTitleBar(m_strFile);
	PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)m_strFile));
	m_view.SetModify(false);
	m_bCanRevert = true;
	
	m_view.SetSel(lPos, lPos);

	return 1;
}
LRESULT CMainFrame::OnRecentFile(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandle)
{
	int fid = wID - ID_FILE_MRU_FILE1; // just to see it in the debugger...
	CString strFile = GetFile(fid);
    
	if (fileExists(strFile) == false) {
		CString msg;
		msg.LoadString(IDS_FILE_NOLONGER_EXISTS);
		MessageBox(msg, NULL, MB_OK | MB_ICONERROR);
        RemoveFromMRUList(strFile);
		return 0;
	}

    
	if(m_dwAutosave)
	{
		if(!CheckToSave(false, true))
			return 0;
	}

	m_view.SetModify(false);
	m_strFile = strFile;

	GetDirectoryFromPath();
	m_view.OpenFile(m_strFile);
	m_strFile.ReleaseBuffer();
	m_view.SetModify(false);

	PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)m_strFile));

	m_bCanRevert = true;

	m_view.SetFocus();

	NotifyBrailleGradeChng();

	return 0;
}

LRESULT CMainFrame::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Scratch Pad
	{
		if(!CheckToSave(false, false, true))
			return 0;

		UpdateMRUList();
		long count;
		ReleaseSemaphore(m_hPrgSemaphore, 1, &count);
		CloseHandle(m_hPrgSemaphore);
	}
	
	RegistryValues();

	m_view.SetModify(false);
	bHandled = 0;

	return 1;
}

LRESULT CMainFrame::OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bVisible = !::IsWindowVisible(m_hWndStatusBar);
	::ShowWindow(m_hWndStatusBar, m_bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
	return 0;
}

//LRESULT CMainFrame::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	CAboutDlg dlg;
//	dlg.DoModal();
//
//	return 0;
//}

LRESULT CMainFrame::OnFind(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(m_pFDlg == NULL)
	{
		CString str;
		BOOL bFindDlg = TRUE;
		m_view.GetSelText(str.GetBuffer(0xffff)); // needs work
		if(str.IsEmpty())
			str = m_strFindReplace;
		
		bFindDlg = (wID == ID_EDIT_FIND);

		DocProp dp;
		m_view.GetDocumentProp(dp);
		CComQIPtr<IBrailleTranslator> qipBrlTrans;
		if (dp.brailleGrade2)
			m_view.GetBrlTrans(&qipBrlTrans);

		m_pFDlg = new CFSFindReplaceDlg(m_view, qipBrlTrans, bFindDlg, str, NULL, FR_DOWN, m_hWnd);
	}

	int nRet = m_pFDlg->DoModal(m_hWnd);
	// If search was unsuccessful, display finish message.
	if (nRet == 0)
	{
		CString str;
		str.LoadString(IDS_FINISHED_SEARCH);
		MessageBox(str, _T(""), MB_OK|MB_ICONINFORMATION);
		m_view.resumeNormalMode();
	} else { // succesful search
    	m_view.startSearchMode();
	}
	return nRet;
}

LRESULT CMainFrame::OnFindNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	FINDTEXTEX ft = CFSFindReplaceDlg::SetFindInfo(m_view, m_strFindReplace);
	if (CFSFindReplaceDlg::SearchForText(m_view, ft, m_dwFlags) == FALSE)
	{
		CString str;
		str.LoadString(IDS_FINISHED_SEARCH);
		MessageBox(str, _T(""), MB_OK|MB_ICONINFORMATION);
		m_view.resumeNormalMode();
	} else {
		m_view.startSearchMode();
	}

	return 0;
}

LRESULT CMainFrame::OnFindReplace(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	USES_CONVERSION;
	CFSFindReplaceDlg *pDlg = CFSFindReplaceDlg::GetNotifier(lParam);
	if (pDlg == NULL)
	{
		bHandled = FALSE;
	}
	else if (pDlg->IsTerminating())
	{
//		m_view.ClearMark();
		m_pFDlg = NULL;
	}
	else if (pDlg->FindNext())
	{
		m_dwFlags = m_pFDlg->GetFlags();
		m_strFindReplace = m_pFDlg->GetFindString();
	}
	else if (pDlg->ReplaceCurrent() || pDlg->ReplaceAll())
	{
		m_dwFlags = m_pFDlg->GetFlags();
		m_strFindReplace = m_pFDlg->GetReplaceString();
	}
	else
	{
		ATLASSERT(0);
		pDlg->PostMessage(WM_CLOSE, 0, 0);
	}

	return 0;
}

void CMainFrame::FontFormat(const pair<DWORD, UINT>& pr)
{
	CHARFORMAT cf;
	m_view.GetSelectionCharFormat(cf);

	cf.dwMask = pr.first;
	cf.dwEffects & pr.second ? cf.dwEffects &= ~pr.second : cf.dwEffects |= pr.second;
	m_view.SetSelectionCharFormat(cf);
}

LRESULT CMainFrame::OnFont(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(wID != ID_FONT_MORE)
		FontFormat(SKStyles[wID - ID_FONT_BOLD].second);
	else
	{
		CHARFORMAT cf;
		cf.cbSize = sizeof(CHARFORMAT);
		cf.dwMask = CFM_SIZE | CFM_FACE;

//		CHARRANGE cr;
//		m_view.GetSel(cr);
//		if(cr.cpMax != cr.cpMin)
//			m_view.SetSel(cr.cpMax, cr.cpMax);

		// Getting original Formats
		m_view.GetSelectionCharFormat(cf);

		CFontDlg dlg(cf);
		dlg.DoModal(m_view.m_hWnd);
		if(dlg.DialogCancel() == IDCANCEL)
			return 0;

		// Update styles
		for_each(FontDlgFun, FontDlgFun+4, CUpdateFontStyles(&dlg, cf));

		// Update size and face type
		// Put in explicit cast to long to stop compiler warnings
		cf.yHeight = (long)abs(POINT_SIZE * dlg.GetSize());
		_tcscpy_s(cf.szFaceName, dlg.GetFaceName());

		// Setting new Formats
		m_view.SetSelectionCharFormat(cf);
	}

	return 0;
}

LRESULT CMainFrame::OnJustify(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CHARRANGE cr;
	m_view.GetSel(cr);
	m_view.SetSel(cr.cpMin, cr.cpMax);

	PARAFORMAT2 pf;
	pf.cbSize = sizeof(PARAFORMAT2);
	pf.dwMask = SKStyles[wID-ID_FONT_BOLD].second.first;
	pf.wAlignment =static_cast<WORD>(SKStyles[wID-ID_FONT_BOLD].second.second);
	m_view.SetParaFormat(pf);
	return 0;
}
// Function calculates the witdth of view in twips.
UINT CMainFrame::SkipperViewTwips()
{
	static UINT nTwips = 0;
	if(nTwips)
		return nTwips;

	int nLogPix = GetDeviceCaps(m_view.GetDC(), LOGPIXELSX);
	CRect rc;
	m_view.GetClientRect(&rc);
	double d = (double)rc.Width()/(double)nLogPix;
	nTwips = (UINT) d * ONEINCH;

	return nTwips;
}

LRESULT CMainFrame::OnIndent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const int* iKey = find_if(ArrowKeys, ArrowKeys+4, Indent(m_view, SkipperViewTwips(), wID));
	if(iKey == ArrowKeys+4)
		Indent(m_view, SkipperViewTwips(), wID)();

	return 0;
}

// Update menu functions
UINT CMainFrame::PasteUpdate(int nItem /* = -1 */)
{
	UINT nEffect = 0;

	if (!OpenClipboard())
		return MF_GRAYED;

	HGLOBAL ClipData = GetClipboardData(CF_UNICODETEXT);
	nEffect = (ClipData != NULL) ? MF_ENABLED : MF_GRAYED;
	CloseClipboard();

	return nEffect;
}

UINT CMainFrame::MarkUpdate(int nItem /* = -1 */)
{
	return (m_view.GetMarker() != -1) ? MF_ENABLED : MF_GRAYED;
}

UINT CMainFrame::CutCopyUpdate(int nItem /* = -1 */)
{
	CHARRANGE cr;
	m_view.GetSel(cr);
	if(cr.cpMax != cr.cpMin)
		return MF_ENABLED;

//	long lMark;
//	if((lMark = m_view.GetMarker()) != -1)
//		return MF_ENABLED;

	return MF_GRAYED; //(min(lMark,cr.cpMin) != max(lMark,cr.cpMax)) ? MF_ENABLED : MF_GRAYED;
}

UINT CMainFrame::FontUpdate(int nItem /* = -1 */)
{
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	m_view.GetSelectionCharFormat(cf);

	return (cf.dwEffects & SKStyles[nItem - ID_FONT_BOLD].second.second) ? MF_CHECKED : MF_UNCHECKED;
}

UINT CMainFrame::ParaUpdate(int nItem /* = -1 */)
{
	PARAFORMAT2 pf;
	pf.cbSize = sizeof(PARAFORMAT2);
	m_view.GetParaFormat(pf);
	return (pf.wAlignment == SKStyles[nItem - ID_FONT_BOLD].second.second) ? MF_CHECKED : MF_UNCHECKED;
}

UINT CMainFrame::IndentUpdate(int nItem /* = -1 */)
{
	PARAFORMAT2 pf;
	pf.cbSize = sizeof(PARAFORMAT2);
	m_view.GetParaFormat(pf);
	LONG lParam = (nItem==ID_PARA_INCREASEINDENT) ? SkipperViewTwips()-pf.dxRightIndent : 0;
	return Indent(m_view, SkipperViewTwips(), nItem)(pf.dxStartIndent, lParam) ? MF_CHECKED : MF_GRAYED;
}


UINT CMainFrame::BulletNumberingUpdate(int Item /* = -1 */)
{
//	if(m_dwParaNum == 0)
//		return MAKELONG(MF_GRAYED, MF_UNCHECKED);

	return m_bBulletNum ? MAKELONG(MF_ENABLED, MF_CHECKED) : MAKELONG(MF_CHECKED, MF_UNCHECKED);

}
LRESULT CMainFrame::OnGOTOQuery(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    int nLine = m_view.GetLineCount();
    int line = m_view.LineFromChar(-1) + 1;
    int lineIdx = m_view.LineIndex(-1);


    long x1=0, x2=0;
    m_view.GetSel(x1, x2);
    int col = x1 - lineIdx + 1;
    
	CString msg;
	msg.Format(IDS_LINE_COL_POSITION, line, nLine, col);
	// needs work
	//JFWSayAndBrailleFlashString(msg, FALSE);
	return 0;
}
LRESULT CMainFrame::OnGOTOQuery2(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    int line = m_view.LineFromChar(-1) + 1;
    int lineIdx = m_view.LineIndex(-1);


    long x1=0, x2=0;
    m_view.GetSel(x1, x2);
    int col = x1 - lineIdx + 1;
    
	CString msg;
	msg.Format(IDS_LINE_COL_POSITION2, line, col);
	MessageBox(msg, _T(""), MB_OK|MB_ICONINFORMATION);
	return 0;
}

UINT CMainFrame::StatusBarUpdate(int nItem /* = -1 */)
{
	return m_bVisible ? MF_CHECKED : MF_UNCHECKED;
}

UINT CMainFrame::RevertToSaveUpdate(int nItem /* = -1 */)
{
	return (m_bCanRevert && m_view.GetModify()) ? MF_ENABLED : MF_GRAYED;
}

UINT CMainFrame::AutoDetectLinkUpdate(int nItem /* = -1 */)
{
	return m_bAutoDetectLink ? MF_CHECKED : MF_UNCHECKED;
}

UINT CMainFrame::FindNextUpdate(int nItem /* = -1 */)
{
	return m_strFindReplace.IsEmpty() ? MF_GRAYED : MF_ENABLED;
}

UINT CMainFrame::TimeStampUpdate(int nItem /* = -1 */)
{
	return MF_ENABLED;
}

UINT CMainFrame::UndoUpdate(int nItem /* = -1 */)
{
	return (::SendMessage(m_view.m_hWnd, EM_CANUNDO, 0, 0) == 0 ? MF_GRAYED : MF_ENABLED);
}

UINT CMainFrame::PrintUpdate(int nItem /* = -1 */)
{
	DocProp Prop;
	memset(&Prop, 0, sizeof(Prop));
	m_view.GetDocumentProp(Prop);
	return (Prop.brailleGrade2 ? MF_GRAYED : MF_ENABLED);
}

void CMainFrame::EnumMenuItems(HMENU hMenu)
{
	if(hMenu != NULL)
	{
		int nCount = GetMenuItemCount(hMenu);
		for(int i = 0; i < nCount; i++)
		{
			VEC_UIPARAM::iterator it = find_if(vecUIParams.begin(), vecUIParams.end(), FindPair_first<UINT, pair<UINT, UINT> >(GetMenuItemID(hMenu, i)));
			if(it != vecUIParams.end())
			{
				UINT nEffect = (this->*UIFun[(*it).second.second])((*it).first);
				if((*it).second.first == 2)
				{
					CheckMenuItem(hMenu, (*it).first, HIWORD(nEffect));
					EnableMenuItem(hMenu, (*it).first, LOWORD(nEffect));
				}
				else if((*it).second.first == 1)
					CheckMenuItem(hMenu, (*it).first, nEffect);
				else
					EnableMenuItem(hMenu, (*it).first, nEffect);
			}
#ifdef ARM	// Since Pocket PC only sends the menu handle for the main menu bar pop up menu,
			else	// we have to enumerate down through the menu items list.
				EnumMenuItems(GetSubMenu(hMenu, i));
#endif
		}
	}
}

LRESULT CMainFrame::OnInitMenuPopup(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	HMENU hMenu = (HMENU)wParam;
	if(hMenu == NULL)
		return 1;

	// Get the pop up menu item count and loop through each item.
	// Update if applicable (enable, disable, check, or uncheck).
	EnumMenuItems(hMenu);

	return 0;
}

LRESULT CMainFrame::OnContextMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	POINT pt;
	GetCaretPos(&pt);
	HMENU hMenu = ::LoadMenu(_Module.GetModuleInstance(), MAKEINTRESOURCE(IDR_SKIPPER_CONTEX	));
	HMENU hSubMenu = GetSubMenu(hMenu, 0);
	int nCount = GetMenuItemCount(hSubMenu);
	for(int i = nCount-1; i >= 0; i--)
	{
		HMENU hMenuTemp = GetSubMenu(hSubMenu, i);
		if(hMenuTemp)
			continue;

		VEC_UIPARAM::iterator it = find_if(vecUIParams.begin(), vecUIParams.end(), FindPair_first<UINT, pair<UINT, UINT> >(GetMenuItemID(hSubMenu, i)));
		if(it == vecUIParams.end())
			continue;

		UINT nEffect = (this->*UIFun[(*it).second.second])((*it).first);
		if(nEffect == MF_GRAYED)
			::RemoveMenu(hSubMenu, i, MF_BYPOSITION);
	}

	TrackPopupMenuEx(hSubMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, m_hWnd, NULL);

	return 0;
}


LRESULT CMainFrame::OnPageSetup(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DocProp Prop;
	memset(&Prop, 0, sizeof(Prop));
	m_view.GetDocumentProp(Prop);

	CPageSetupDlg dlg(CNewFileDlg::GetFileExt() < 3, Prop);
	if(dlg.DoModal() != IDOK)
		return 0;

	Prop.markPosition = m_view.GetMarker();
	m_view.SetDocumentProp(Prop);
	return 0;
}

// Function in BrlEdit DLL that handles hibernation


LRESULT CMainFrame::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (LOWORD(wParam))
	{
	case WA_INACTIVE:
	{
		HWND hWnd = GetForegroundWindow();
		TCHAR sz[64];
		memset(sz, 0, sizeof(sz));
		::GetWindowText(hWnd, sz, 64);
		bool bJawMenu = !(_tcsstr(sz, _T("JAWS")));
		if (m_dwAutosave && !lParam && /*!m_strFile.IsEmpty() &&*/ bJawMenu)
			CheckToSave(false, false, false, true); //OnFileSave(0, 0, NULL, handled);
	}
	break;
	}
	return(0);
}

LRESULT CMainFrame::OnFileInsert(WORD wLParam, WORD wHPara, HWND /*lParam*/, BOOL& bHandled)
{
	CHARRANGE cr;
	m_view.GetSel(cr);
	m_view.SetSel(cr);

	CString strFileName, strInsertAs, strTemp;
	int nInsertAs = 0;

	if(!GetFileName(strFileName, strInsertAs))
		return 0;
	strTemp.LoadString(IDS_INSERT_AS_G2BRL);
	if( strTemp.CompareNoCase( strInsertAs ) == 0 )
	{
		nInsertAs = 1;
	}
		
	m_view.InsertFileText(strFileName.GetBuffer(strFileName.GetLength()), cr, nInsertAs );
	return 0;
}

LRESULT CMainFrame::OnSpellEdit(WORD, WORD, HWND, BOOL&)
{
	CSpellEditDlg dlg(m_spl);

	dlg.DoModal();
	return 0;
}

#ifdef BOOKMARK_SUPPORT
LRESULT CMainFrame::OnBookMark(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	if(GetKeyState(VK_CONTROL))
		((&m_view)->*BMFun[wID-ID_BM_SETBOOKMARK])();

	return 0;
}
#endif
TCHAR id[] = { _T(' '), _T('\t')};
struct Space : public unary_function<char, bool>
{
	Space() : bPrev(false){}
	bool operator()(TCHAR c)
	{
		if(bPrev && (find(id, id+2, c) != id+2))
			return false;

		return (bPrev = (find(id, id+2, c) != id+2));
	}

private:
	bool bPrev;
};

LRESULT CMainFrame::OnWordCount(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CTheWaitCursor wait;
	int nCount = 0, nLineCount = m_view.GetLineCount();
	for(int i = 0; i < nLineCount; i++)
	{
		int nIndex = m_view.LineIndex(i);
		int nLineLength = m_view.LineLength(nIndex);
		CString str;
		m_view.GetLine(i, str.GetBuffer(nLineLength), nLineLength);
		str.ReleaseBuffer(nLineLength);
		if(str.IsEmpty())
			continue;

		str.TrimLeft(); str.TrimRight();
		tstring tsz((LPCTSTR)str);
		nCount += static_cast<int>(count_if(tsz.begin(), tsz.end(), Space()) + 1);
	}

	CString strMsg, strTitle, strFormat;

	strFormat.LoadString(IDS_WORDS_COUNTED);
	strMsg.Format(strFormat, nCount);
	strTitle.LoadString(IDS_TITLE_WORD_COUNT);
	::MessageBox(m_hWnd, strMsg, strTitle, MB_OK|MB_ICONINFORMATION);
	return 0;
}

LRESULT CMainFrame::OnOptions(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	COptionsDlg dlg(m_dwWordWrap, m_dwAutosave /*m_dwParaNum*/, m_dwSPDivider, m_dwDateTimeFormat);
	if(dlg.DoModal() == IDCANCEL)
		return 0;
#if 0
	GTUtility::RegDataType type;
	for(int i = 0; i < NUM_OPTATTR; i++)
	{
		if(DwordRegParam(*this, aOptAttr[i]) != ((&dlg)->*OPTFun[i])())
		{
			type.dwType = Skipper::_Opts[i].second;
			type.dw = ((&dlg)->*OPTFun[i])();
			if(GTUtility::SetRegistryValue(GTUtility::bstrFSOpt, NULL, Skipper::_Opts[i].first, type))
				DwordRegParam(*this, aOptAttr[i]) = type.dw;
		}
	}
#endif
	SetWordWrap();
	return 0;
}

void CMainFrame::SetParaNum()
{
	int nBullet = m_bBulletNum ? /*m_dwParaNum*/ 0 : 0;
	PARAFORMAT2 pf;
	pf.cbSize = sizeof(PARAFORMAT2);
	m_view.GetParaFormat(pf);
	pf.dwMask |= (PFM_NUMBERING | PFM_NUMBERINGSTYLE | PFM_NUMBERINGSTART);

	pf.wNumbering = (unsigned short) SKParaNum[nBullet].second.first;
	pf.wNumberingStyle =static_cast<WORD>(SKParaNum[nBullet].second.second);
	if(SKParaNum[nBullet].second.first == 3 || SKParaNum[nBullet].second.first == 4)
		pf.wNumberingStart = 0;
	else
		pf.wNumberingStart = 1;

	m_view.SetParaFormat(pf);
}


void CMainFrame::SetWordWrap()
{
	m_view.SetTargetDevice(NULL, m_dwWordWrap);
}

LRESULT CMainFrame::OnDateTimeStamp(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& /*bHandled*/)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString str = GetFormatDateTimeString(m_dwDateTimeFormat, st);

	DocProp Prop;
	m_view.GetDocumentProp(Prop);
	if(Prop.brailleGrade2)
	{
			CComBSTR bstrTranslated;
		CComQIPtr<IBrailleTranslator> qipBrlTrans;
			m_view.GetBrlTrans(&qipBrlTrans);
			if (qipBrlTrans)
			{
				qipBrlTrans->Translate(CComBSTR(str),&bstrTranslated,0,0,0,0);
				if (bstrTranslated)
					str = bstrTranslated;
			}
	}

	CHARRANGE cr;
	m_view.GetSel(cr);
	m_view.SetSel(cr);
	m_view.ReplaceSel(str);
	return 0;
}

LRESULT CMainFrame::OnBulletNumbering(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bBulletNum = !m_bBulletNum;
	SetParaNum();
	return 0;
}

//LRESULT CMainFrame::OnHyperLink(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	LONG lMark = m_view.GetMarker();
//	CHARRANGE cr;
//	m_view.GetSel(cr);
//	m_view.SetSel(min(lMark, cr.cpMin), max(lMark,cr.cpMax));

//	CHARFORMAT2 cf;
//	cf.cbSize = sizeof(CHARFORMAT2);
//	m_view.GetSelectionCharFormat(cf);

//	cf.dwMask = CFM_LINK;
//	(cf.dwEffects & CFE_LINK) ? cf.dwEffects &= ~CFE_LINK : cf.dwEffects |= CFE_LINK;
//	m_view.SetSelectionCharFormat(cf);

//	m_view.GetSel(cr);
//	m_view.SetSel(-1, cr.cpMax);

//	return 0;
//}

//LRESULT CMainFrame::OnAutoDetectLink(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//{
//	m_bAutoDetectLink = !m_bAutoDetectLink;
//	m_view.SetAutoURLDetect(m_bAutoDetectLink);
//	return 0;
//}

void CMainFrame::LaunchApp(ENLINK* pENLink)
{
	m_view.SetSel(pENLink->chrg);
	CComBSTR bstr;
	m_view.GetWindowText(bstr.m_str);
	CString mstr(bstr.m_str);
	CString strURL = mstr.Mid(pENLink->chrg.cpMin, pENLink->chrg.cpMax - pENLink->chrg.cpMin);
		SHELLEXECUTEINFO se;
		se.cbSize = sizeof(SHELLEXECUTEINFO);
		se.fMask = SEE_MASK_NOCLOSEPROCESS;
		se.hwnd = m_hWnd;
		se.lpVerb = _T("open");
		se.lpFile = strURL;
		se.lpParameters = NULL;
		se.lpDirectory = NULL;
		se.nShow = SW_SHOWNORMAL;
		se.hInstApp = _Module.GetModuleInstance();
		ShellExecuteEx(&se);

	m_view.SetSel(-1, pENLink->chrg.cpMax);
}

void CMainFrame::AppProperties(ENLINK* pENLink)
{
	m_view.SetSel(pENLink->chrg);
	CComBSTR bstr;
	m_view.GetWindowText(bstr.m_str);
	CString mstr(bstr.m_str);
	CString strURL = mstr.Mid(pENLink->chrg.cpMin, pENLink->chrg.cpMax - pENLink->chrg.cpMin);

	SHELLEXECUTEINFO se;
	se.cbSize = sizeof(SHELLEXECUTEINFO);
	se.fMask = SEE_MASK_NOCLOSEPROCESS;
	se.hwnd = m_hWnd;
	se.lpVerb = _T("properties");
	se.lpFile = strURL;
	se.lpParameters = NULL;
	se.lpDirectory = NULL;
 	se.nShow = SW_SHOWNORMAL;
	se.hInstApp = _Module.GetModuleInstance();
	ShellExecuteEx(&se);
}

LRESULT CMainFrame::OnNotifyLink(int wID, LPNMHDR phdr, BOOL& bHandled)
{
	ENLINK* pENLink = ( ENLINK* )phdr ;

	switch(pENLink->msg)
	{
	case WM_LBUTTONDOWN:
		LaunchApp(pENLink);
		break;
	case WM_RBUTTONDOWN:
		AppProperties(pENLink);
		break;
	default:
		break;
	}

	return 0;
}

RECT GetPrinterMargins(HDC& hdc)
{
	long lphyX, lphyY, lprnX, lprnY;
	RECT rc;
	memset(&rc, 0, sizeof(RECT));
	lphyX = GetDeviceCaps(hdc, PHYSICALWIDTH);
	lphyY = GetDeviceCaps(hdc, PHYSICALHEIGHT);
	lprnX = GetDeviceCaps(hdc, HORZRES);
	lprnY = GetDeviceCaps(hdc, VERTRES);
	rc.left = GetDeviceCaps(hdc, PHYSICALOFFSETX);
	rc.top = GetDeviceCaps(hdc, PHYSICALOFFSETY);
	rc.right = lphyX - lprnX - rc.left;
	rc.bottom = lphyY - lprnY - rc.top;

	return rc;
}

void CMainFrame::GetPageSetup(HDC& hdc, RECT& rPage, RECT& rPrint)
{
	POINT pt;
	pt.y = GetDeviceCaps(hdc, PHYSICALOFFSETY);
	pt.x = GetDeviceCaps(hdc, PHYSICALOFFSETX);
	int xOffset = max(0, GetDeviceCaps(hdc, LOGPIXELSX) * 1 - pt.x);
	int yOffset = max(0, GetDeviceCaps(hdc, LOGPIXELSY) * 1 - pt.y);

	pt.x = GetDeviceCaps(hdc, PHYSICALWIDTH);
	pt.y = GetDeviceCaps(hdc, PHYSICALHEIGHT);
	int xOffsetRight = xOffset + GetDeviceCaps (hdc, HORZRES) - pt.x - GetDeviceCaps (hdc, LOGPIXELSX) * 1;
	int yOffsetBottom = yOffset + GetDeviceCaps (hdc, VERTRES) - pt.y - GetDeviceCaps (hdc, LOGPIXELSY) * 1; 

	SIZE Size;
	// Get Page Size and set the page rect.
	Size.cx = SMulDiv(GetDeviceCaps(hdc, PHYSICALWIDTH), 1440, GetDeviceCaps(hdc, LOGPIXELSX));
	Size.cy = SMulDiv(GetDeviceCaps(hdc, PHYSICALHEIGHT), 1440, GetDeviceCaps(hdc, LOGPIXELSY));

	rPage.left = xOffset;
	rPage.right = Size.cx + xOffsetRight;
	rPage.top = yOffset;
	rPage.bottom = Size.cy + yOffsetBottom;

	// Calculate margins.
	RECT rcMargins = GetPrinterMargins(hdc);
	
	// Calculate the rendering rect and convert to twips.
	rPrint.left = rcMargins.left;
	rPrint.right = Size.cx;
	rPrint.top = rcMargins.top;
	rPrint.bottom = Size.cy;

	rPrint.right -= 1440 + (SMulDiv(rcMargins.right, 1440, GetDeviceCaps(hdc, LOGPIXELSX)) - xOffsetRight);
	rPrint.left += 1440 - (SMulDiv(rcMargins.left, 1440, GetDeviceCaps(hdc, LOGPIXELSX)) + xOffset);
	rPrint.bottom -= 1440 + (SMulDiv(rcMargins.bottom, 1440, GetDeviceCaps(hdc, LOGPIXELSY)) - yOffsetBottom);
	rPrint.top += 1440 - (SMulDiv(rcMargins.top, 1440, GetDeviceCaps(hdc, LOGPIXELSY)) + yOffset);

	if(m_dwPrintWidth > (rPrint.right - rPrint.left))
	{
		m_dwPrintWidth = (rPrint.right - rPrint.left);
		m_view.SetTargetDevice(GetDC(), m_dwPrintWidth);
	}
	else
		rPrint.right = rPrint.left + m_dwPrintWidth;

}

LRESULT CMainFrame::OnPrint(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CCopiesDlg dlg((m_dwNumOfCopies<1?1:m_dwNumOfCopies));
	if(dlg.DoModal() == IDCANCEL)
		return 0;

	m_dwNumOfCopies = dlg.GetNumCopies();
    int dblSp = dlg.IsDoubleSpaceRequired();
	
    //CString msg;
	//const unsigned short *dsp = dblSp==1?_T("yes"):_T("no");
	//msg.Format(_T("OnPrint nCopies=%d, double space=%s"), m_dwNumOfCopies, dsp);
	//::MessageBox(NULL, msg, _T(""), MB_OK|MB_ICONINFORMATION);
	

    // take of double spacing if requested...
    PARAFORMAT2 opf;  // originalPf
	if (dblSp == 1) {
        SetToDoubleSpacing(opf);
	}
	
	// Clear FormatRange buffer.
	FORMATRANGE fr;
	memset(&fr, 0, sizeof(fr));
	m_view.FormatRange(fr, FALSE);

	// Set Parameters for Print Dialog
	PRINTDLG pd;
	memset(&pd, 0, sizeof(pd));
	pd.lStructSize = sizeof(pd);
	pd.hwndOwner = m_view.m_hWnd;
	pd.Flags = PSD_DISABLEMARGINS;
	if(PrintDlg(&pd) == FALSE)
	{
		ProcessPrintDlgError(CommDlgExtendedError());
     	if (dblSp == 1) {
            RestorePreviousFormatting(opf);
		}
		return 0;
	}

	RECT PageRect, PrintRect;
	GetPageSetup(pd.hDC, PageRect, PrintRect);

	// Initialize DOCINFO
	CComBSTR bstr;
	GetWindowText(bstr.m_str);
	DOCINFO docinfo;
	memset(&docinfo, 0, sizeof(docinfo));
	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = bstr.m_str;

	int nDocs = 0;
	while(nDocs < m_dwNumOfCopies)
	{
		if(!(::StartDoc(pd.hDC, &docinfo) < 0))
		{
			// Get number of characters in skipper.
			long lastChar = 0, lastOffset = 0;
			if(pd.Flags & PD_SELECTION)
			{
				CHARRANGE cr;
				m_view.GetSel(cr);
				lastChar = cr.cpMin;
				lastOffset = cr.cpMax;
			}
			else
			{
				GETTEXTLENGTHEX tex = {GTL_DEFAULT, 1200};
//			long lastChar = 0, lastOffset = ::SendMessage(m_view.m_hWnd, EM_GETTEXTLENGTHEX, (WPARAM)&tex, 0);
				lastOffset = static_cast<int>(::SendMessage(m_view.m_hWnd, EM_GETTEXTLENGTHEX, (WPARAM)&tex, 0));
			}

			memset(&fr, 0, sizeof(fr));
			// Begin sending pages to printer;
			do
			{
				if(::StartPage(pd.hDC) < 0)
					break; // Error
				else
				{
					fr.hdcTarget = pd.hDC;
					fr.hdc = pd.hDC;
					fr.rcPage = PageRect;
					fr.rc = PrintRect;
					fr.chrg.cpMin = lastChar;
					fr.chrg.cpMax = lastOffset;
					lastChar = static_cast<int>(::SendMessage(m_view.m_hWnd, EM_FORMATRANGE, (WPARAM)TRUE, (LPARAM)&fr));
//					fr.chrg.cpMin = lastChar;
					::EndPage(pd.hDC);
				}
			}while(fr.chrg.cpMin != -1 && fr.chrg.cpMin < lastOffset);

			::EndDoc(pd.hDC);
		}

		nDocs++;
	}

	DeleteDC(pd.hDC);
	// Clear FormatRange buffer again. (be nice)
	memset(&fr, 0, sizeof(fr));
	m_view.FormatRange(fr, FALSE);

    // put back original formatting if it was changed to double spacing...
	if (dblSp == 1) {
        RestorePreviousFormatting(opf);
	}

	return 0;
}

// Process Error information and report to user.
void CMainFrame::ProcessPrintDlgError(DWORD dwError)
{
	CString str, strTitle;
	switch(dwError)
	{

	case 0:
		return;
	case PDERR_NODEVICES:
		str.LoadString(IDS_NOPRINTERS);
		break;
	case PDERR_PRINTERNOTFOUND:
		str.LoadString(IDS_PRINTERNOTFOUND);
		break;
	case PDERR_INITFAILURE:
		str.LoadString(IDS_CREATEDCFAILURE);
		break;
	default:
		str.LoadString(IDS_PRINTDLG_DEFAULT);
		break;
	}

	strTitle.LoadString(IDS_PRINTDLG_ERROR_TITLE);
	MessageBox(str, strTitle, MB_OK|MB_ICONINFORMATION);
}

LRESULT CMainFrame::DoHelp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	// needs work
	return 0;
}

void RemoveUnwantedCharacters(CString& str)
{
	CString strName;
	int nLen = str.GetLength();
	for(int i = 0; i < nLen; i++)
	{
		if (iswprint(str[i]))
			strName += str[i];
	}

	str = strName;
}


// Create a file name for a new, un-named file.  Will use similar handling as
// in Pocket Word.  Will populate lpNewFile with the full file path.
bool CMainFrame::GetNewFileName(LPTSTR lpNewFile)
{
	CString str, strPath;

	CString strDir;
	BuildFilePath(strDir);
	strDir += _T('\\');

	// Default extension is the FSEdit extension.
	CString strExt;
	strExt.LoadString(IDS_SKIPPER_EXT);
	strExt = strExt.Right(3);

  	int nLineCount = m_view.GetLineCount();
	for (int i = 0; i < nLineCount; i++)
	{
		int nIndex = m_view.LineIndex(i);
		if (nIndex == -1)
			break;

		int nLineLength = m_view.LineLength(nIndex);
		str.Empty();
		m_view.GetLine(i, str.GetBuffer(nLineLength), nLineLength);
		str.ReleaseBuffer(nLineLength);
		str.TrimLeft(); str.TrimRight();

		if(str.IsEmpty())
			continue;

		/* If the document is a Braille grade 2 document, we need to back 
			translate the line to avoid having illegal chars as part of the file
			name. */
		DocProp dp;
		m_view.GetDocumentProp(dp);
		CComQIPtr<IBrailleTranslator> qipBrlTrans;
		if (dp.brailleGrade2)
		{
			m_view.GetBrlTrans(&qipBrlTrans);
			if (qipBrlTrans)
			{
				CComBSTR bstrTranslated;
				qipBrlTrans->BackTranslate(CComBSTR(str),&bstrTranslated);
				if (bstrTranslated)
					str = bstrTranslated;
			}
		}

		// If text is less than 22 characters, will use entire string as 
		// the new file name.  Else, will use only the first 21 characters.
		if (str.GetLength() > 21)
			str = str.Left(21);
		
		RemoveUnwantedCharacters(str);
		break;
	}

	if (str.IsEmpty())
		str.LoadString(IDS_NEW_FILE);

	while (1)
	{
		// If file already exists, add a number at the end to make it unique.
		strPath.Format(_T("%s%s.%s"), strDir.GetBuffer(), str.GetBuffer(), strExt.GetBuffer());
		int n = 1;
		HANDLE hFile;
		//while (access(strPath) == 0)
		while ((hFile = CreateFile(strPath, 
							GENERIC_READ, 
							0, 
							NULL, 
							OPEN_EXISTING, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL)) != INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			strPath.Format(_T("%s%s(%d).%s"), strDir.GetBuffer(), str.GetBuffer(), n, strExt.GetBuffer());;
			
			// Make sure length of file name is within bounds.  When it gets
			// too long, go back with the last name and stick with it.
			if (strPath.GetLength() > _MAX_PATH)
			{
				n--;
				strPath.Format(_T("%s%s(%d).%s"), strDir.GetBuffer(), str.GetBuffer(), n, strExt.GetBuffer());;
				break;
			}
			n++;
		}
		break;
	}

	if (!strPath.IsEmpty())
	{
		_tcscpy_s(lpNewFile,MAX_PATH ,strPath);
		return true;
	}

	return false;
}

LRESULT CMainFrame::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// If focus has changed to another application, save the buffer contents.
	if (m_dwAutosave && GetForegroundWindow() != m_hWnd)
		CheckToSave(false, false, false, true);			

	return true;
}

LRESULT CMainFrame::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// Handling the Smart Minimize (X) button.
	// If user minimized, save the buffer contents.
	switch (wParam)
    {
    case SIZE_MINIMIZED:
//		if(m_dwAutosave)
//			CheckToSave(false, false, true);
		break;
	}
	return CFrameWindowImpl<CMainFrame>::OnSize(uMsg, wParam, lParam, bHandled);	
}

void CMainFrame::SetTitleBar(LPCTSTR szFile)
{
	// If the file path is included, extract the path.
	CString strFile = szFile;
	int nPos = strFile.ReverseFind(_T('\\'));
	if (nPos != -1)
		strFile.Delete(0, nPos+1);

	// Check if document is in Grade 2 Braille.
	DocProp Prop;
	m_view.GetDocumentProp(Prop);
	if (Prop.brailleGrade2)
	{
		CString strG2;
		strG2.LoadString(IDS_GRADE2);
		strFile.Format(_T("%s - %s"), strFile.GetBuffer(), strG2.GetBuffer());
	}

	SetWindowText(SKIPPER_DASH + strFile);
}

LRESULT CMainFrame::NotifyBrailleGradeChng()
{	
#ifdef _WIN32_WCE
	HWND hWnd = ::FindWindow(_T("wce_jfw"),NULL);
#else
	HWND hWnd = ::FindWindow(_T("JFW"),NULL);
#endif // _WIN32_WCE
	
	if (hWnd)
	{
		DocProp Prop;
		m_view.GetDocumentProp(Prop);
		return ::SendMessage(hWnd, 
							::RegisterWindowMessage(L"BEN_BRLGRADE_CHANGED"), 
							0, 
							(LPARAM)Prop.brailleGrade2);
	}
	return 0;
}

LRESULT CMainFrame::OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (wParam)
	{
	case ID_FILE_OPEN:
		if (!lParam)
			break;

		// Automatically save any modifications.
		if(m_dwAutosave && m_view.GetModify())
		{
			int nT = 1;
			OnFileSave(0, 0, NULL, nT);
//			CheckToSave(false, false, true);
		}

//		if(!m_strFile.IsEmpty())
//			GetCursorPosition(m_strFile);
		
			// Parse the command line.
		COPYDATASTRUCT *pcds = (COPYDATASTRUCT *) lParam;
		CString str = (LPCTSTR) pcds->lpData;
		int f = 0;
		if((f = str.Find(' ')) != -1)
		{
			m_strFile = str.Left(f);
			if(_tcsstr(str, _T("-r")))
				m_view.SetReadOnly();
			else
				m_strFile = str;
		}
		else
			m_strFile = str;

		// Open the file
		m_view.OpenFile(m_strFile);
		PostMessage(WM_UPDATE_TITLE, 0, (LPARAM)((LPCTSTR)m_strFile));


		m_view.SetModify(false);
		m_bCanRevert = true;
		m_bNewFile = false;

	    break;
	}
	return TRUE;
}

LRESULT CMainFrame::OnUpdateFileTitle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if((int)wParam)
	{
		TCHAR szBuf[64];
		::GetWindowText(m_hWnd, szBuf, 64);
		CString str(szBuf);
		str += CString(_T(" *"));
		SetWindowText(str);
	}
	else
		SetTitleBar((LPCTSTR)lParam);

	return 0;
}

void CMainFrame::LoadFileTypeFilter(LPTSTR lpszFileFilter, bool bLoadBRL /* = true */ )
{
	size_t iLen = 0;
	for( DWORD i = IDS_SKIPPER_TEXT, k = IDS_SKIPPER_EXT; i < IDS_ALL_FILES; i++, k++)
	{
		if(IDS_BRIALLE_TEXT == i && bLoadBRL)
			continue;

		LoadString( _Module.GetResourceInstance(), i, lpszFileFilter, _MAX_PATH );		
		iLen = _tcslen( lpszFileFilter );
		lpszFileFilter += (iLen + 1);
		LoadString( _Module.GetResourceInstance(), k, lpszFileFilter, 8 );
		iLen = _tcslen( lpszFileFilter );
		lpszFileFilter += (iLen + 1);
	}
	*lpszFileFilter = _T('\0');  // Double NULL at end of filter. 
}

void CMainFrame::RecordFile(const CString& strMapFile)
{
	if(mapFiles.size() > 256)
	{
		MAPFILE::iterator it = mapFiles.begin();
		mapFiles.erase(it);
	}

	CHARRANGE cr;
	m_view.GetSel(cr);
	MAPFILE::iterator it = mapFiles.find(strMapFile);
	if(it == mapFiles.end())
		mapFiles.insert(MAPFILE::value_type(strMapFile, make_pair(cr.cpMax, cr.cpMin)));
	else
	{
		(*it).second.first = cr.cpMax;
		(*it).second.second = cr.cpMin;
	}
}

CHARRANGE CMainFrame::GetRecordFile(const CString& strMapFile)
{
	CHARRANGE cr = {-1, -1};
	MAPFILE::iterator it = mapFiles.find(strMapFile);
	if(it == mapFiles.end())
		return cr;

	cr.cpMax = (*it).second.first;
	cr.cpMin = (*it).second.second;
	return cr;
}

LRESULT CMainFrame::OnCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CHARRANGE cr;
	m_view.GetSel(cr);
  	if((cr.cpMax - cr.cpMin) > 65565)
	{
		CString str;
		str.LoadString(IDS_STRING_TOO_LARGE);
		::MessageBox(m_hWnd, str, _T("Error"), MB_OK);
	}
	else
		m_view.Copy();

	return 0;
}

LRESULT CMainFrame::OnCut(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
	CHARRANGE cr;
	m_view.GetSel(cr);
  	if((cr.cpMax - cr.cpMin) > 65565)
	{
		CString str;
		str.LoadString(IDS_STRING_TOO_LARGE);
		::MessageBox(m_hWnd, str, _T("Error"), MB_OK);
	}
	else
		m_view.Cut();

	return 0;
}


static void _cleanMenu(CMenu &m)
{
	int n = GetMenuItemCount(m);
	for(int i=n-1; i>=0; --i)
		m.DeleteMenu(i, MF_BYPOSITION);
}
// This function assumes MRU it's the only dynamic submenu for the File menu

static  void _showError()
{
    LPVOID lpMsgBuf;
    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        0, // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
    );
    // Process any inserts in lpMsgBuf.
    // ...
    // Display the string.
    MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
    // Free the buffer.
    LocalFree( lpMsgBuf );
}



// SkipperView stuff
void CFSEditView::callMyOwnerSearchNext() 
{
	int b = true;
    owner->OnFindNext(0, ID_EDIT_FIND_NEXT, 0, b);
}
void CFSEditView::callMyOwnerProcessGoto(unsigned int c) 
{
    owner->processGotoMoveCharacter(c);
}

int GotoStateTracker::parseCharacter(int c) // returns 0=ok, keep going, 1=done, -1=error
{
	userInput += (TCHAR)c;
	bool sign = c == TCHAR('-');
	bool digit = c >= '0' && c <= '9';
    object = getObject(c);

	int rc = 0; // so far so good... need to continue parsing

	if (st == _GettingSign) {
		if (sign) {
			dir = 1;
			st = _GettingDigits;
		} else if (digit) {
			count = c - '0';
			st = _GettingDigits;
		} else if (object != _navigUnknown) {
			count = 1;
			rc = 1;  // done
		} else {
			rc = -1; // parsing error
		}
	} else if (st == _GettingDigits) {
		if (digit) {
			count = count*10 + c - '0';
		} else if (object != _navigUnknown) {
			rc = 1;  // done
		} else {
			rc = -1;
		}
	} 
    return rc;
}
CtrlNavigatorCode GotoStateTracker::getMoveInfo(int &n) // should be called after parse returns 1 
{
    n = count==0? 1 : count;
    n = dir==1? -n : n;

    return object;
}
//enum CtrlNavigatorCode { _navigByChar=0, _navigByWord, _navigByLine, _navigByMark,  };
CtrlNavigatorCode GotoStateTracker::getObject(int c)
{
	if (c >= 'a' && c <= 'z')
		c += 'A' - 'a';
	return c == 'L'? _navigByLine : c=='C'? _navigByChar : 
	    c=='M'? _navigByMark : c=='W'?_navigByWord : _navigUnknown;
}

LRESULT CMainFrame::processGotoMoveCharacter(int c)
{
    int rc = _gtt.parseCharacter(c) ;
	if (rc == -1) {

	    CString msg;
	    msg.Format(IDS_INVALID_GOTOMOVE);
        msg += _gtt.userInput;

		// JFWSayAndBrailleFlashString(msg, FALSE);

		//CString msg = _T("Error in move command: ") + _gtt.userInput;
	    //MessageBox(msg, _T(""), MB_OK|MB_ICONINFORMATION);
	
		// clean up state
		//m_view.resumeNormalMode();
		m_view.stopGotoMode();  // this will force resumeNormalMode() to be called from
		                        // CSkipperView::OnChar() 
		_gtt.reset();

	} else if (rc == 1) { // done

		int n;
	    CtrlNavigatorCode obj = _gtt.getMoveInfo(n); // should be called after parse returns 1

		navgtor->move(obj, n);
		m_view.stopGotoMode();  // this will force resumeNormalMode() to be called from
		                        // CSkipperView::OnChar() 
		_gtt.reset();
	}
	return 0;
}
LRESULT CMainFrame::OnGOTOMove(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	//CString str = "You entered goto move";
	//MessageBox(str, _T(""), MB_OK|MB_ICONINFORMATION);
	_gtt.reset();
	//JFWSayAndBrailleFlashString(_T("move"), FALSE);
    m_view.startGotoMode();
	return 0;
}

// new, double spacing stuff...
void CMainFrame::SetToDoubleSpacing(PARAFORMAT2 &originalPf)
{
	PARAFORMAT2 pf;
	pf.cbSize = sizeof(PARAFORMAT2);
	m_view.GetParaFormat(pf);

	// save it
	originalPf = pf;

	// set to double spacing 
	pf.dwMask |= (PFM_SPACEAFTER );
	pf.bLineSpacingRule = 2;
	m_view.SetParaFormat(pf);
}
void CMainFrame::SetToSingleSpacing(PARAFORMAT2 &originalPf)
{
	PARAFORMAT2 pf;
	pf.cbSize = sizeof(PARAFORMAT2);
	m_view.GetParaFormat(pf);

	// save it
	originalPf = pf;

	// set to single spacing 
	pf.dwMask |= (PFM_SPACEAFTER );
	pf.bLineSpacingRule = 0;
	m_view.SetParaFormat(pf);
}

void CMainFrame::RestorePreviousFormatting(const PARAFORMAT2 &originalPf)
{
	PARAFORMAT2 pf = originalPf;
	m_view.SetParaFormat(pf);
}

LRESULT CMainFrame::OnSetDoubleSpace(WORD /*wNotifyCode*/, WORD wID,
									 HWND /*hWndCtl*/, BOOL& /*bHandled*/) 
{
	//CString str = "Calling the double space...";
	//MessageBox(str, _T(""), MB_OK|MB_ICONINFORMATION);

    PARAFORMAT2 pf;
	if (usingDouble == false) {
        SetToDoubleSpacing(pf);
        usingDouble = true;
    } else {
        SetToSingleSpacing(pf);
		usingDouble = false;
	}

	return 0;
}



int CtrlNavigator::jawsMsgId = 0;
CtrlNavigator::CtrlNavigator(CFSEditView *_tgt) : tgt(_tgt) 
{ 
	jwnd = ::FindWindow(_T("wce_jfw"),NULL);
	if (jawsMsgId == 0)
		jawsMsgId = RegisterWindowMessage(_T("JAWS_RQST_CARET_MOVE"));  // need to validate
} 

int CtrlNavigator::move(CtrlNavigatorCode byCode, int n)
{
	if (byCode != _navigUnknown)  // send all valid requests to Jaws
	    ::PostMessage(jwnd, jawsMsgId, byCode, n);
    
	return 0; // al is OK
	/**
	if (byCode == _navigByChar)
		return moveByChar(n);
	if (byCode == _navigByWord)
		return moveByWord(n);
	if (byCode == _navigByLine)
		return moveByLine(n);
	if (byCode == _navigByMark)
		return moveByMark(n);
	return -1; // erroneous request
    **/
}
int CtrlNavigator::moveByChar(int n)
{
	return _move(_T("CHAR"), n);
}
int CtrlNavigator::moveByWord(int n)
{
	return _move(_T("WORD"), n);
}
int CtrlNavigator::moveByLine(int n)
{
	//return _move(_T("LINE"), n);
    
    int line, col;
	tgt->getCaretLineColumn(line, col);
    //CString msg;
	//msg.Format(_T("Got request to move n=%d lines... currently l=%d, col=%d"), n, line, col);
	//MessageBox(NULL, msg, _T(""), MB_OK|MB_ICONINFORMATION);

	// find the target character idx (how many bytes from beginning of file to 
	// this character) and its position (x, y coor with respect to the control)
	int line2 = line + n;  // new line, same column
	if (line2 < 0)
		line2 = 0;  // if trying to go too far... just go to the beginning of the file
	int lineIdx = tgt->getLineIndex(line2);
    int charIdx = lineIdx + col;
	tgt->moveCaretToThisCharacterIndex(charIdx);

	return 0;
}
int CtrlNavigator::moveByMark(int n)
{
	return _move(_T("MARK"), n);
}
	
int CtrlNavigator::_move(LPCTSTR obj, int n)
{
    CString msg;
	msg.Format(_T("Goto completed n=%d c=%s "), n, obj);
	MessageBox(NULL, msg, _T(""), MB_OK|MB_ICONINFORMATION);
    //MessageBox( NULL, (LPCTSTR)lpMsgBuf, _T("Error"), MB_OK | MB_ICONINFORMATION );
	return 0;

}

