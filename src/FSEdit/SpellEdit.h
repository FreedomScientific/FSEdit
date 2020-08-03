#ifndef __SPELLEDIT_H__123857574892486566927
#define __SPELLEDIT_H__123857574892486566927

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//#include "atlextra.h"
#include "atlwctrls.h"
#pragma warning (disable:4018)
#include "resource.h"
#include <vector>
#include <algorithm>

class CSpellEditDlg : public CDialogImpl<CSpellEditDlg>
{
	CSpellChecker& m_spl;
	bool m_bAdd;
	CComBSTR m_bstr;

	vector<CString> vec;
public:
	CSpellEditDlg(CSpellChecker& spl) : m_spl(spl) {}

	enum {IDD = IDD_SPELL_EDIT};

	BEGIN_MSG_MAP(CSpellEditDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCancel)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDBTN_ADD_WORD, OnAddWord)
		COMMAND_ID_HANDLER(IDBTN_EDIT, OnEditWord)
		COMMAND_ID_HANDLER(IDBTN_RESORT, OnResort)
		COMMAND_ID_HANDLER(IDBTN_DELETE, OnDelete)
		NOTIFY_HANDLER(IDLST_EDITOR, LVN_ENDLABELEDIT, OnEndEditNotify)
		NOTIFY_HANDLER(IDLST_EDITOR, LVN_KEYDOWN, OnKeyDownNotify)
	END_MSG_MAP()

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		// This may look strange, but it is actually needed because of Windows CE,
		// Pocket PC, WTL and Win32 messaging, all kinds of crap ends up coming
		// through here.  Thus we need only catch the close button control message
		// and end the dialog.  (P.S. if you don't beleive me, comment out these lines
		// or get rid of the IDOK command handler and see what happen)
		if(GetDlgItem(IDCANCEL) == hWndCtrl)
			EndDialog(IDCANCEL);

		return 0;
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		m_bAdd = false;
		m_bstr.Empty();
		CListCtrl lst(GetDlgItem(IDLST_EDITOR));
		lst.SetRedraw(FALSE);
		int nIndex = 0;

		SPLBUFFER start = {NULL, NULL, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0};
		SPLBUFFER splBuf;
		WCHAR buf[255];

		memset(buf, 0, sizeof(buf));
		memcpy(&splBuf, &start, sizeof(SPLBUFFER));
		splBuf.pwszOut = buf;
		splBuf.cwcOut = 255;

		while(1)
		{
			m_spl.EnumDict(USER_DICT1_ADD, nIndex, &splBuf);
			if(splBuf.cspl == 0)
				break;

			for(int i = 0; i < splBuf.cspl; i++)
				vec.push_back(CString(splBuf.aspl[i].pwsz));

			nIndex += splBuf.cspl;
		}

		sort(vec.begin(), vec.end());
		vector<CString>::iterator it;
		int i = 0;
		for(it = vec.begin(); it != vec.end(); it++, i++)
			lst.InsertItem(i, (*it)); 

		lst.SetRedraw();
		lst.Invalidate();
		lst.SetFocus();
		lst.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		return 0;
	}

	LRESULT OnAddWord(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		CListCtrl lst(GetDlgItem(IDLST_EDITOR));
		lst.SetFocus();
		int nCount = lst.GetItemCount();
		lst.InsertItem(nCount, _T(""));
		CEdit Edt(lst.EditLabel(nCount));
		ATLASSERT(Edt.m_hWnd != NULL);
		Edt.SetDlgCtrlID(IDEDT_LISTEDITCTRL);
		m_bAdd = true;
		return 0;
	}

	LRESULT OnEndEditNotify(int idCtrl, LPNMHDR pnmh, BOOL bHandled)
	{
		LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pnmh;
		CComBSTR bstr(pDispInfo->item.pszText);
		if((pDispInfo->item.mask & LVIF_TEXT) && bstr.Length())
		{
			CListCtrl lst(GetDlgItem(IDLST_EDITOR));
			lst.SetItemText(pDispInfo->item.iItem, 0, bstr.m_str);
			if(!m_bAdd)
			{
				vector<CString>::iterator it = remove(vec.begin(), vec.end(), CString(m_bstr.m_str));
				if(it != vec.end())
					vec.erase(it, vec.end());

				m_spl.RemoveDict(SPL_IGNORE_LEX, m_bstr.m_str);
				m_bstr.Empty();
			}
			else
				m_bAdd = false;

			m_spl.AddDict(USER_DICT1_ADD, pDispInfo->item.pszText, NULL);
			vec.push_back(CString(pDispInfo->item.pszText));
		}
		else if(m_bAdd)
		{
			m_bAdd = false;
			CListCtrl lst(GetDlgItem(IDLST_EDITOR));
			lst.DeleteItem(pDispInfo->item.iItem);
			lst.SetItemState(pDispInfo->item.iItem -1, LVIS_SELECTED, LVIS_SELECTED);
		}

		return 0;
	}

	LRESULT OnResort(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		CListCtrl lst(GetDlgItem(IDLST_EDITOR));
		lst.SetRedraw(FALSE);
		lst.DeleteAllItems();
		sort(vec.begin(), vec.end());
		vector<CString>::iterator it;
		int i = 0;
		for(it = vec.begin(); it != vec.end(); it++, i++)
			lst.InsertItem(i, (*it)); 
		int nCount = lst.GetItemCount();
		lst.SetRedraw();
		lst.RedrawItems(0, nCount);
		lst.UpdateWindow();
		return 0;
	}

	LRESULT OnDelete(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		CListCtrl lst(GetDlgItem(IDLST_EDITOR));
		int nItem = lst.GetNextItem(-1, LVNI_SELECTED);
		if(nItem < 0)
			return 0;

		CComBSTR bstr;
		lst.GetItemText(nItem, 0, bstr.m_str);
		vector<CString>::iterator it = remove(vec.begin(), vec.end(), CString(bstr.m_str));
		if(it != vec.end())
			vec.erase(it, vec.end());

		m_spl.RemoveDict(USER_DICT1_ADD, bstr.m_str);
		lst.DeleteItem(nItem);

		lst.SetItemState((nItem > lst.GetItemCount()) ? nItem - 1 : nItem, LVIS_SELECTED, LVIS_SELECTED);
	
		return 0;
	}

	LRESULT OnEditWord(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		CListCtrl lst(GetDlgItem(IDLST_EDITOR));
		lst.SetFocus();
		int nItem = lst.GetNextItem(-1, LVNI_SELECTED);
		if(nItem >= 0)
		{
			m_bstr.Empty();
			lst.GetItemText(nItem, 0, m_bstr.m_str);
			CEdit Edt(lst.EditLabel(nItem));
			Edt.SetDlgCtrlID(IDEDT_LISTEDITCTRL);
			ATLASSERT(Edt.m_hWnd != NULL);
		}

		return 0;
	}

	LRESULT OnKeyDownNotify(int idCtrl, LPNMHDR pnmh, BOOL bHandled)
	{
		LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pnmh;
		if(VK_F9 == pLVKeyDow->wVKey)
			SendMessage(WM_COMMAND, MAKEWPARAM(IDBTN_EDIT, 0));

		return 0;
	}
};

#endif