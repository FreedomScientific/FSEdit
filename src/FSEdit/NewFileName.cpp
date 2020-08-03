// NewFileName.cpp

#include "stdafx.h"
#include "resource.h"
#include "NewFileName.h"
#include "openfile.h"

/* static */ bool CNewFileName::m_bBraille2 = true;
/* static */ int CNewFileName::m_nExt = 0;

LRESULT CNewFileName::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();

	m_btnOK.Attach(GetDlgItem(IDOK));
	m_btnOK.EnableWindow(FALSE);
	m_edtNewFileName.Attach(GetDlgItem(IDEDT_NEW_NAME));
//	m_cbxFileType.Attach(GetDlgItem(IDCBX_FILE_TYPE));
	m_cbxFolders.Attach(GetDlgItem(IDCBX_FOLDERS));
	m_xbxBraille2.Attach(GetDlgItem(IDXBOX_BRAILL2));
	m_xbxBraille2.SetCheck(m_bBraille2);


//	vector<pair<DWORD, CString> >::iterator it, itEnd = COpenFile::Types().end();
//	advance(itEnd, -1);
//	for(it = COpenFile::Types().begin(); it != itEnd; it++)
//	{
//		CString str;
//		str.LoadString((*it).first);
//		m_cbxFileType.AddString(str);
//	}

	CString strtemp;
	strtemp.LoadString(IDS_NONE);
	m_cbxFolders.AddString(strtemp);

	auto itF = COpenFile::Folders().begin();
	for(advance(itF, 1); itF != COpenFile::Folders().end(); itF++)
		m_cbxFolders.AddString(itF->second);
	
//	CString strFolder = COpenFile::GetLastFolder();
//	int n = m_cbxFolders.FindString(-1, strFolder);
//	if(n = CB_ERR)
//		n = 0;

	m_cbxFolders.SetCurSel(COpenFile::GetLastFolderSel());
//	m_cbxFileType.SetCurSel(m_nExt);
	m_edtNewFileName.SetFocus();
	return 0;
}

LRESULT CNewFileName::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CComBSTR bstr;
	m_edtNewFileName.GetWindowText(bstr.m_str);
	m_strName = CString(bstr.m_str);
	m_strExt.LoadString(IDS_SKIPPER_EXT); // = COpenFile::GetLastExtension();
	m_strName += m_strExt.Right(4);
	m_nExt = 0; //m_cbxFileType.GetCurSel();
	EndDialog(IDOK);
	m_bBraille2 = (m_xbxBraille2.GetCheck() == 1);

	int nSel = m_cbxFolders.GetCurSel();
	if(nSel == 0)
	{
		CString str1;
		str1.LoadString(IDS_MY_DOCUMENTS);
		m_strDirectory = _T('\\') + str1 + _T('\\');
	}
	else
	{
		CString str1, str2;
		str1.LoadString(IDS_MY_DOCUMENTS);
		m_cbxFolders.GetLBText(nSel, str2.GetBuffer(MAX_PATH));
		m_strDirectory = _T('\\') + str1 + _T('\\') + str2 + _T('\\');
	}

	COpenFile::SetLastFolderSel(nSel);
	return 0;
}

LRESULT CNewFileName::OnChange(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_btnOK.EnableWindow(m_edtNewFileName.LineLength());
	return 0;
}

LRESULT CNewFileName::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}