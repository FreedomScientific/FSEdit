// OpenFile.cpp

#include "stdafx.h"
#include "resource.h"
#include "FSEdit.h"
#include <vector>
#include <list>
#include <algorithm>
//#include <projects.h>
#include "OpenFile.h"
#include "gtutility.h"

#define DEF_DIRECTORY	_T("\\My Documents\\")

// static variables
DWORD COpenFile::m_dwLastFolderSel = 0;
DWORD COpenFile::m_dwLastInsertAsSel = 0;
CString COpenFile::m_strLastFolder = _T("");
CString COpenFile::m_strLastLocation = _T("");
BOOL COpenFile::m_bEnableInsertAsCombo = 0;

DWORD COpenFile::m_dwLastType = IDS_SKIPPER_TEXT;

list<pair<DWORD, CString> > COpenFile::lstTypes;
list<pair<int, CString> > COpenFile::lstSubFolders;
list<pair<int, CString> > COpenFile::lstTopFolders;
vector<CString> COpenFile::lstAltSubFolders;



/* static */ CString COpenFile::GetLastDirectory()
{
	if(m_strLastFolder.IsEmpty())
		m_strLastFolder = DEF_DIRECTORY;

	return m_strLastFolder;
}

/* static */ CString COpenFile::GetLastExtension()
{
	list<pair<DWORD, CString> >::iterator it = lstTypes.begin();
	advance(it, m_dwLastType - IDS_SKIPPER_TEXT);

	return (*it).second.Right(4);
}

int COpenFile::FillListCtrl(list<pair<int, CString> >& lst, CString strDir)
{
	int nType = m_cbxTypes.GetCurSel();

	CString strSlash(_T('\\'));
	CString strPath;
	list<pair<DWORD, CString> >::iterator it = lstTypes.begin();
	advance(it, nType);
	for(list<pair<int, CString> >::iterator itS = lst.begin(); itS != lst.end(); itS++)
	{
		if((*itS).first)
			strPath = strDir + (*itS).second + strSlash + (*it).second;
		else
			strPath = strDir + strSlash + (*it).second;

//		InsertFiles(strPath, nLoc, (*itS).first);
	}

	return 0;
}

int COpenFile::FillListCtrl()//*
{
	m_lstFiles.DeleteAllItems();
	m_lstFiles.SetRedraw(FALSE);

	int nLoc = m_cbxLocations.GetCurSel();
	m_dwLastFolderSel = m_cbxFolders.GetCurSel();
	m_strLastFolder = lstAltSubFolders[m_dwLastFolderSel];
	m_strLastLocation = m_locationNames[m_cbxLocations.GetCurSel()];
	int nType = m_cbxTypes.GetCurSel();

	list<pair<int, CString> >::iterator itT, itS;
	itT = find_if(lstTopFolders.begin(), lstTopFolders.end(), FindPair_first<int, CString>(nLoc));
	ATLASSERT(itT != lstTopFolders.end());
	if(itT == lstTopFolders.end())
		return 1;

	CString strSlash(_T('\\'));
	if(m_dwLastFolderSel) // Individual folder.
	{
		itS = find_if(lstSubFolders.begin(), lstSubFolders.end(), FindPair_first<int, CString>(m_dwLastFolderSel));
		ATLASSERT(itS != lstSubFolders.end());
		if(itS == lstSubFolders.end())
			return 2;

		list<pair<DWORD, CString> >::iterator it = lstTypes.begin();
		advance(it, nType);
		CString strPath = (*itT).second + (*itS).second + strSlash + (*it).second;
		InsertFiles(strPath, nLoc, m_dwLastFolderSel);
	}
	else // All folders
	{
		CString strPath;
		list<pair<DWORD, CString> >::iterator it = lstTypes.begin();
		advance(it, nType);
		for(itS = lstSubFolders.begin(); itS != lstSubFolders.end(); itS++)
		{
			if((*itS).first)
				strPath = (*itT).second + (*itS).second + strSlash + (*it).second;
			else
				strPath = (*itT).second + strSlash + (*it).second;

			InsertFiles(strPath, nLoc, (*itS).first);
		}
	}

	m_lstFiles.SetRedraw(TRUE);
	m_lstFiles.Invalidate();

	return 0;
}

// Function checks the file name and path.
bool COpenFile::IsValidFileName(CString& name)
{
	WIN32_FIND_DATA fd;
	return (FindFirstFile(name, &fd) != INVALID_HANDLE_VALUE);
}

void COpenFile::InsertFiles(CString strPath, int nTFolder, int nTSub)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	if((hFind = ::FindFirstFile(strPath, &fd)) != INVALID_HANDLE_VALUE)
	{
		if(IsFile(fd))
		{
			int nItem = m_lstFiles.GetItemCount();
			m_lstFiles.InsertItem(nItem, fd.cFileName);
			m_lstFiles.SetItemData(nItem, MAKELONG(nTFolder, nTSub));
		}
	}
	while(FindNextFile(hFind, &fd))
	{
		if(!IsFile(fd))
			continue;

		int nItem = m_lstFiles.GetItemCount();
		m_lstFiles.InsertItem(nItem, fd.cFileName);
		m_lstFiles.SetItemData(nItem, MAKELONG(nTFolder, nTSub));

			}

	FindClose(hFind);
}

// Function checks to make sure it is not a directory you are trying to reading.
bool COpenFile::IsFile(WIN32_FIND_DATA& fd)
{
	return (CString(fd.cFileName) != CString(_T(".")) && CString(fd.cFileName) != CString(_T("..")) && !(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

int COpenFile::GetExtension(CString& str)
{
	list<pair<DWORD, CString> >::iterator it = lstTypes.begin();
	for(int i = 0; it != lstTypes.end(); i++ , it++)
	{
		CString strtest = (*it).second.Right(4);
		if(str.Find(strtest, 0) != -1)
			return i;
	}

	return -1;
}

LRESULT COpenFile::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CComBSTR bstrFile;
	m_edtFiles.GetWindowText(bstrFile.m_str);
	strFile = CString(bstrFile.m_str);
	CString strSlash(_T('\\'));

	GetInsertAsValues();

	int nItem = m_lstFiles.GetNextItem(-1, LVNI_SELECTED);
	CComBSTR bstrTemp;
	m_lstFiles.GetItemText(nItem, 0, bstrTemp.m_str);
	CString strTemp(bstrTemp.m_str);
	list<pair<int, CString> >::iterator itT, itS;
	if(!strTemp.Compare(strFile)) // Quick Check edit box against list control selection.
	{
		DWORD dwParam = m_lstFiles.GetItemData(nItem);
		itT = find_if(lstTopFolders.begin(), lstTopFolders.end(), FindPair_first<int, CString>(LOWORD(dwParam)));
		ATLASSERT(itT != lstTopFolders.end());
		if (itT == lstTopFolders.end())
			return 0;
		itS = find_if(lstSubFolders.begin(), lstSubFolders.end(), FindPair_first<int, CString>(HIWORD(dwParam)));
		ATLASSERT(itS != lstSubFolders.end());
		if(itS == lstSubFolders.end())
		return 0;
		strFile = (*itT).second + (*itS).second + strSlash + strFile;
	}
	else	// Something is changed. Check for a valid file.
	{
		bool bInvalid = true;
		for(itT = lstTopFolders.begin(); (itT != lstTopFolders.end() && bInvalid ); itT++)
		{
			CString strtemp = (*itT).second + strSlash + strFile;
			if(IsValidFileName(strtemp))
			{
				bInvalid = false;
				strFile = strtemp;
				break;
			}
			for(itS = lstSubFolders.begin(); itS != lstSubFolders.end(); itS++)
			{
				strtemp = (*itT).second + (*itS).second + strSlash + strFile;
				if(IsValidFileName(strtemp))
				{
					bInvalid = false;
					strFile = strtemp;
					break;
				}
			}
		}

		if(bInvalid)
		{
			CString str;
			str.LoadString(IDS_INVALID_FILE_NAME);
			MessageBox(str, _T(""), MB_OK|MB_ICONINFORMATION);
			return 0;
		}
	}

	EndDialog(IDOK);

	SetSavedLocation(m_strLastLocation);
	SetSavedFolder(m_strLastFolder);

	return 0;
}

LRESULT COpenFile::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT COpenFile::OnSelLocationChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int nErr = FillListCtrl();
	ReportError(nErr);
	return 0;
}

// Function updates selection of new folder.
LRESULT COpenFile::OnSelFolderChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int nErr = FillListCtrl();
	ReportError(nErr);
	return 0;
}

// Function updates for selection of new file type.
LRESULT COpenFile::OnSelTypeChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	int nErr = FillListCtrl();
	ReportError(nErr);
	int nType = m_cbxTypes.GetCurSel();
	list<pair<DWORD, CString> >::iterator it = lstTypes.begin();
	advance(it, nType);
	m_dwLastType = (*it).first;
	return 0;
}

// Function updates for selection of new file type.
LRESULT COpenFile::OnSelInsertAsChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	GetInsertAsValues();
	return 0;
}


//  Selection in list control is changing here  and the edit box needs updating.
LRESULT COpenFile::OnFileChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	int nItem = m_lstFiles.GetNextItem(-1, LVNI_SELECTED);
	if(nItem != -1)
	{
		CComBSTR bstr;
		m_lstFiles.GetItemText(nItem, 0, bstr.m_str);
		m_edtFiles.SetWindowText(bstr);
	}
	else
		m_edtFiles.SetWindowText(_T(""));
	return 0;
}

// Enables and Disables the 'OK' button.  Checks to see if the edit box is empty or not.
LRESULT COpenFile::OnNameChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	::EnableWindow(GetDlgItem(IDOK), m_edtFiles.LineLength());
	return 0;
}

void COpenFile::LoadSubDirectories(CString strDir)
{
	WIN32_FIND_DATA fd;
	CString strFolder = strDir + _T("\\*.*");
	HANDLE hFind = FindFirstFile(strFolder, &fd);

	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				wcscmp(fd.cFileName,L".") != 0 &&
				wcscmp(fd.cFileName, L"..") != 0)
			{
				if(m_cbxFolders.FindStringExact(-1, fd.cFileName) == CB_ERR)
				{
					CString strTemp = CString(_T("\\")) + fd.cFileName;
					AddSubFolder(strTemp, fd.cFileName);
//					LoadSubDirectories(strDir + strTemp);
				}
//				else
//				{
//					LoadSubDirectories(strDir + CString(_T("\\")) + fd.cFileName);
//				}
			}
		}while(FindNextFile(hFind, &fd));
	}
}

void COpenFile::AddTopFolder(CString strFolder, CString strAltName /* = CString(_T("")) */)
{
	int nIndex = m_cbxLocations.AddString(strAltName);
	lstTopFolders.push_back(make_pair(nIndex, strFolder));
	m_locationNames.push_back(strAltName);
}

void COpenFile::AddSubFolder(CString strFolder, CString strAltName /* = CString(_T("")) */)
{
	int nIndex = m_cbxFolders.AddString(strAltName);
	lstAltSubFolders.push_back(strAltName);
	lstSubFolders.push_back(make_pair(nIndex, strFolder));
}

void COpenFile::EnableInsertAsCombo( BOOL bEnable )
{
	m_bEnableInsertAsCombo = bEnable;
}

LRESULT COpenFile::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_cbxFolders.Attach(GetDlgItem(IDCBX_FOLDERS));
	m_lstFiles.Attach(GetDlgItem(IDLST_FILES));
	m_edtFiles.Attach(GetDlgItem(IDEDT_FILE));
	m_cbxTypes.Attach(GetDlgItem(IDCBX_TYPE));
	m_cbxLocations.Attach(GetDlgItem(IDCBX_LOCATION));
	m_cbxInsertAs.Attach(GetDlgItem(IDCBX_INSERT_AS));

	lstSubFolders.clear();
	lstTopFolders.clear();
	lstAltSubFolders.clear();

	CString strFolder, str, strTemp;
	strFolder.LoadString(IDS_NONE);
	AddSubFolder(_T(""),strFolder);

	strFolder.LoadString(IDS_MY_DOCUMENTS);
	str.LoadString(IDS_MAIN_MEMORY);
	AddTopFolder(_T("\\") + strFolder, str);
	LoadSubDirectories(_T("\\") + strFolder);

	
	GetSavedLocation(m_strLastLocation);
	GetSavedFolder(m_strLastFolder);

	int nIndex = m_cbxLocations.FindStringExact(0, m_strLastLocation);
	if (nIndex != -1)
		m_cbxLocations.SetCurSel(nIndex);
	else
		m_cbxLocations.SetCurSel(0);

	nIndex = m_cbxFolders.FindStringExact(0, m_strLastFolder);
	if (nIndex != -1)
		m_cbxFolders.SetCurSel(nIndex);
	else
		m_cbxFolders.SetCurSel(0);

	DWORD dwLastEntry = 0;
	for(list<pair<DWORD, CString> >::iterator it = lstTypes.begin(); it != lstTypes.end(); it++)
	{
		str.LoadString((*it).first);
		dwLastEntry = m_cbxTypes.AddString(str);
	}

	m_cbxTypes.SetCurSel( dwLastEntry /*m_dwLastType - IDS_SKIPPER_TEXT*/);

	int nErr = FillListCtrl();
	ReportError(nErr);

	strTemp.LoadString(IDS_INSERT_AS_TEXT);	
	m_cbxInsertAs.AddString( strTemp );
	strTemp.LoadString(IDS_INSERT_AS_G2BRL);
	m_cbxInsertAs.AddString( strTemp );
	
	m_cbxInsertAs.SetCurSel(GetLastInsertAsSel());
	m_cbxInsertAs.EnableWindow( m_bEnableInsertAsCombo );

	m_edtFiles.SetFocus();
	return 0;
}

/* static */ void COpenFile::LoadSubFolders(CString strDir)
{
	WIN32_FIND_DATA fd;
	CString strFolder = strDir + _T("\\*.*");
	HANDLE hFind = FindFirstFile(strFolder, &fd);

	if(hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				wcscmp(fd.cFileName, L".") != 0 &&
				wcscmp(fd.cFileName, L"..") != 0 &&
				(find_if(COpenFile::lstSubFolders.begin(), COpenFile::lstSubFolders.end(), FindPair_second<int, CString>(CString(fd.cFileName))) == COpenFile::lstSubFolders.end()))
			{
				COpenFile::lstSubFolders.push_back(make_pair(-1, CString(fd.cFileName)));
				COpenFile::lstAltSubFolders.push_back(CString(fd.cFileName));
			}
		}while(FindNextFile(hFind, &fd));
	}
}

/* static */ void COpenFile::LoadFileFolders()
{
	lstSubFolders.clear();
	lstTopFolders.clear();
	lstAltSubFolders.clear();

	CString strFolder, str;
	strFolder.LoadString(IDS_NONE);
	lstSubFolders.push_back(make_pair(-1, strFolder));
	lstAltSubFolders.push_back(strFolder);

	strFolder.LoadString(IDS_MY_DOCUMENTS);
	str.LoadString(IDS_MAIN_MEMORY);
	COpenFile::lstTopFolders.push_back(make_pair(-1, strFolder));
	LoadSubFolders(_T('\\') + strFolder);

}
void COpenFile::ReportError(int nErr)
{
	if(nErr == 0)
		return;

	CString strErr;
	if(nErr == 1)
		strErr.LoadString(IDS_ERR_NO_STORAGE);
	else if(nErr == 2)
		strErr.LoadString(IDS_ERR_NO_FOLDER);

	MessageBox(strErr, _T(""), MB_OK|MB_ICONINFORMATION);
}

void COpenFile::GetInsertAsValues()
{
	int nIndex = m_cbxInsertAs.GetCurSel();
	if(nIndex == CB_ERR)
		return;

	if(nIndex == 0)
		bstrInsertAs.LoadString(IDS_INSERT_AS_TEXT);	
	else if(nIndex == 1)
		bstrInsertAs.LoadString(IDS_INSERT_AS_G2BRL);

	m_dwLastInsertAsSel = nIndex;
}
