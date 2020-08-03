#ifndef __OPENFILE_H__83HKEDKKR839DMAD
#define __OPENFILE_H__83HKEDKKR839DMAD
#pragma once
#include <list>
#include <algorithm>
using namespace std;

class COpenFile : public CDialogImpl<COpenFile>
{
	static DWORD m_dwLastType;
	static DWORD m_dwLastFolderSel;
	static DWORD m_dwLastInsertAsSel;
	static CString m_strLastLocation;
	static CString m_strLastFolder;

	static BOOL m_bEnableInsertAsCombo;
	

//	static vector<pair<DWORD, CString> > vecTypes;
//	static vector<pair<int, CString> > vecSubFolders;
//	static vector<CString> vecAltSubFolders;
//	static vector<pair<int, CString> > vecTopFolders;

	static list<pair<DWORD, CString> > lstTypes;
	static list<pair<int, CString> > lstSubFolders;
	static std::vector<CString> lstAltSubFolders;
	static list<pair<int, CString> > lstTopFolders;


	void LoadSubDirectories(CString strDir);
	void AddTopFolder(CString strFolder, CString strAltName = CString(_T("")));
	void AddSubFolder(CString strFolder, CString strAltName = CString(_T("")));
	void ReportError(int nErr);
	
	static void LoadFileFolders();
	static void LoadSubFolders(CString strDir);

public:
	enum {IDD = IDD_OPENFILE_DLG};
	static DWORD GetLastFolderSel(){return m_dwLastFolderSel;}
	static void SetLastFolderSel(DWORD dwFolder ){m_dwLastFolderSel = dwFolder;}
	static void SetFolderDirect(CString strFolder){m_strLastFolder = strFolder;}
	static CString GetLastFolder(){return m_strLastFolder;}
	static void SetLastFolder(CString strFolder){m_strLastFolder = strFolder;}
	static DWORD GetLastFileType(){return m_dwLastType;}
	static void SetLastFileType(int dwType){m_dwLastType = dwType;}
	static CString GetLastDirectory();
	static CString GetLastExtension();
	
	static void EnableInsertAsCombo( BOOL bEnable=TRUE );

//	static vector<pair<int, CString> >& Folders(){return vecSubFolders;}
//	static vector<pair<DWORD, CString> >& Types(){return vecTypes;}
//	static vector<CString>& AltFolders(){return vecAltSubFolders;}
	static list<pair<int, CString> >& Folders(){return lstSubFolders;}
	static list<pair<DWORD, CString> >& Types(){return lstTypes;}
//	static vector<CString>& AltFolders(){return lstAltSubFolders;}


	BEGIN_MSG_MAP(COpenFile)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_HANDLER(IDEDT_FILE, EN_CHANGE, OnNameChange)
		COMMAND_HANDLER(IDCBX_LOCATION, CBN_SELCHANGE, OnSelLocationChange)
		COMMAND_HANDLER(IDCBX_FOLDERS, CBN_SELCHANGE, OnSelFolderChange)
		COMMAND_HANDLER(IDCBX_TYPE, CBN_SELCHANGE, OnSelTypeChange)
		COMMAND_HANDLER(IDCBX_INSERT_AS, CBN_SELCHANGE, OnSelInsertAsChange)
		NOTIFY_HANDLER(IDLST_FILES, LVN_ITEMCHANGED, OnFileChanged)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
	END_MSG_MAP()

	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelLocationChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelFolderChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelTypeChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnSelInsertAsChange(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnFileChanged(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);
	LRESULT OnNameChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	CString GetFileName(){return strFile;}

	DWORD GetLastInsertAsSel(){return m_dwLastInsertAsSel;}
	CComBSTR GetInsertAsString(){return bstrInsertAs;}
	
	friend class LoadFolders;
private:
	void GetInsertAsValues();
	bool IsFile(WIN32_FIND_DATA& fd);
	bool IsValidFileName(CString& strFile);
	void InsertFiles(CString strPath, int nFolder, int nType);
	int FillListCtrl();
	int FillListCtrl(list<pair<int, CString> >& lst, CString strDir);
	int GetExtension(CString& str);

	CComboBox m_cbxTypes;
	CComboBox m_cbxFolders;
	CComboBox m_cbxLocations;
	CComboBox m_cbxInsertAs;
	CListCtrl m_lstFiles;
	CEdit m_edtFiles;
	vector<CString> m_locationNames;

	CString strFile;
	//CString strInsertAs;
	CComBSTR bstrInsertAs;
};

class LoadFolders
{
public:
	void operator()()
	{
		COpenFile::LoadFileFolders();

		for(DWORD i = IDS_SKIPPER_TEXT, k = IDS_SKIPPER_EXT; i <= IDS_ALL_FILES; i++, k++)
		{
//			if(IDS_BRIALLE_TEXT == i)
//				continue;

			CString strExt;
			strExt.LoadString(k);
			COpenFile::lstTypes.push_back(make_pair(i, strExt));
		}
	}
};
#endif
