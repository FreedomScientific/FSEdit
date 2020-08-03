#ifndef __ATLWCTRLS_H__83Y5HEFKALR85719457
#define __ATLWCTRLS_H__83Y5HEFKALR85719457
#pragma once

#include <atlmisc.h>
#include <vector>
#include <commctrl.h>
using namespace std;

#ifndef WM_REFLECT_BASE
#define WM_REFLECT_BASE 0xBC00
#endif

#define CONTROL_REFLECT_CODE_HANDLER(cd, func) \
{ \
	if (((WM_COMMAND + WM_REFLECT_BASE) == uMsg) && \
		(NULL != lParam) && \
		(cd == ((LPNMHDR)lParam)->code)) \
	{\
		bHandled = FALSE;\
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled);\
		if(bHandled)\
			return TRUE;\
	}\
}

#define NOTIFY_REFLECT_CODE_HANDLER(cd, func) \
{ \
	if (((WM_NOTIFY + WM_REFLECT_BASE) == uMsg) && \
		(NULL != lParam) && \
		(cd == ((LPNMHDR)lParam)->code)) \
	{\
		bHandled = FALSE;\
		lResult = func((int)wParam, (LPNMHDR)lParam, bHandled);\
		if(bHandled)\
			return TRUE;\
	}\
}

#define WM_NOTIFY_REFLECTOR()\
if(uMsg == WM_NOTIFY)\
{\
	UINT code = ((LPNMHDR)lParam)->code;\
	switch(code)\
	{\
	case NM_SETCURSOR:\
		break;\
	default:\
		if ((NULL != lParam) &&\
		(NULL != ((LPNMHDR)lParam)->hwndFrom))\
		{\
			BOOL bHandled = ::SendMessage(((LPNMHDR)lParam)->hwndFrom, WM_NOTIFY + WM_REFLECT_BASE, wParam, lParam);\
			if(bHandled) \
				return TRUE; \
		};\
	};\
}\

class CWaitCursorX
{
public:
// Data
	HCURSOR m_hWaitCursor;
	HCURSOR m_hOldCursor;
	bool m_bInUse;

// Constructor/destructor
	CWaitCursorX(bool bSet = true, LPCTSTR lpstrCursor = IDC_WAIT, bool bSys = true) : m_hOldCursor(NULL), m_bInUse(false)
	{
		HINSTANCE hInstance = bSys ? NULL : _Module.GetResourceInstance();
		m_hWaitCursor = ::LoadCursor(hInstance, lpstrCursor);
		ATLASSERT(m_hWaitCursor != NULL);

		if(bSet)
			Set();
	}

	~CWaitCursorX()
	{
		Restore();
	}

// Methods
	bool Set()
	{
		if(m_bInUse)
			return false;
		m_hOldCursor = ::SetCursor(m_hWaitCursor);
		m_bInUse = true;
		return true;
	}

	bool Restore()
	{
		if(!m_bInUse)
			return false;
		::SetCursor(m_hOldCursor);
		m_bInUse = false;
		return true;
	}
};


template <class Base>
class CListCtrlT : public Base
{
public:
// Constructors
	CListCtrlT(HWND hWnd = NULL) : Base(hWnd) { }

	CListCtrlT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	}
	HWND Create(HWND hWndParent, LPRECT lpRect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			HMENU hMenu = NULL, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, lpRect, szWindowName, dwStyle, dwExStyle, hMenu, lpCreateParam);
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_LISTVIEW;
	}

	COLORREF GetBkColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, LVM_GETBKCOLOR, 0, 0L);
	}
	BOOL SetBkColor(COLORREF cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETBKCOLOR, 0, cr);
	}
	HIMAGELIST GetImageList(int nImageListType) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HIMAGELIST)::SendMessage(m_hWnd, LVM_GETIMAGELIST, nImageListType, 0L);
	}
	HIMAGELIST SetImageList(HIMAGELIST hImageList, int nImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HIMAGELIST)::SendMessage(m_hWnd, LVM_SETIMAGELIST, nImageList, (LPARAM)hImageList);
	}
	int GetItemCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0L);
	}
	BOOL GetItem(LV_ITEM* pItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)pItem);
	}
	BOOL SetItem(const LV_ITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)pItem);
	}
	BOOL SetItemState(int nItem, UINT nState, UINT nStateMask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(nItem, 0, LVIF_STATE, NULL, 0, nState, nStateMask, 0);
	}
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(nItem, nSubItem, LVIF_TEXT, lpszText, 0, 0, 0, 0);
	}
	BOOL SetItemData(int nItem, DWORD dwData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItem(nItem, 0, LVIF_PARAM, NULL, 0, 0, 0, (LPARAM)dwData);
	}
	int InsertItem(const LV_ITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)pItem);
	}
	int InsertItem(int nItem, LPCTSTR lpszItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(LVIF_TEXT, nItem, lpszItem, 0, 0, 0, 0);
	}
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return InsertItem(LVIF_TEXT|LVIF_IMAGE, nItem, lpszItem, 0, 0, nImage, 0);
	}
	BOOL DeleteItem(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_DELETEITEM, nItem, 0L);
	}
	BOOL DeleteAllItems()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_DELETEALLITEMS, 0, 0L);
	}
	UINT GetCallbackMask() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, LVM_GETCALLBACKMASK, 0, 0L);
	}
	BOOL SetCallbackMask(UINT nMask)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCALLBACKMASK, nMask, 0L);
	}
	int GetNextItem(int nItem, int nFlags) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETNEXTITEM, nItem, MAKELPARAM(nFlags, 0));
	}
	int FindItem(LV_FINDINFO* pFindInfo, int nStart) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_FINDITEM, nStart, (LPARAM)pFindInfo);
	}
	int HitTest(LV_HITTESTINFO* pHitTestInfo) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)pHitTestInfo);
	}
	BOOL SetItemPosition(int nItem, POINT pt)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMPOSITION32, nItem, (LPARAM)&pt);
	}
	BOOL GetItemPosition(int nItem, LPPOINT lpPoint) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEMPOSITION, nItem, (LPARAM)lpPoint);
	}
	int GetStringWidth(LPCTSTR lpsz) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETSTRINGWIDTH, 0, (LPARAM)lpsz);
	}
	BOOL EnsureVisible(int nItem, BOOL bPartialOK)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_ENSUREVISIBLE, nItem, MAKELPARAM(bPartialOK, 0));
	}
	BOOL Scroll(SIZE size)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SCROLL, size.cx, size.cy);
	}
	BOOL RedrawItems(int nFirst, int nLast)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_REDRAWITEMS, nFirst, nLast);
	}
	BOOL Arrange(UINT nCode)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_ARRANGE, nCode, 0L);
	}
	HWND EditLabel(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HWND)::SendMessage(m_hWnd, LVM_EDITLABEL, nItem, 0L);
	}
	HWND GetEditControl() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HWND)::SendMessage(m_hWnd, LVM_GETEDITCONTROL, 0, 0L);
	}
	BOOL GetColumn(int nCol, LV_COLUMN* pColumn) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETCOLUMN, nCol, (LPARAM)pColumn);
	}
	BOOL SetColumn(int nCol, const LV_COLUMN* pColumn)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMN, nCol, (LPARAM)pColumn);
	}
	int InsertColumn(int nCol, const LV_COLUMN* pColumn)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_INSERTCOLUMN, nCol, (LPARAM)pColumn);
	}
	BOOL DeleteColumn(int nCol)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_DELETECOLUMN, nCol, 0L);
	}
	int GetColumnWidth(int nCol) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETCOLUMNWIDTH, nCol, 0L);
	}
	BOOL SetColumnWidth(int nCol, int cx)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, nCol, MAKELPARAM(cx, 0));
	}
	BOOL GetViewRect(LPRECT lpRect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETVIEWRECT, 0, (LPARAM)lpRect);
	}
	COLORREF GetTextColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, LVM_GETTEXTCOLOR, 0, 0L);
	}
	BOOL SetTextColor(COLORREF cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETTEXTCOLOR, 0, cr);
	}
	COLORREF GetTextBkColor() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (COLORREF)::SendMessage(m_hWnd, LVM_GETTEXTBKCOLOR, 0, 0L);
	}
	BOOL SetTextBkColor(COLORREF cr)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETTEXTBKCOLOR, 0, cr);
	}
	int GetTopIndex() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETTOPINDEX, 0, 0L);
	}
	int GetCountPerPage() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_GETCOUNTPERPAGE, 0, 0L);
	}
	BOOL GetOrigin(LPPOINT lpPoint) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETORIGIN, 0, (LPARAM)lpPoint);
	}
	BOOL Update(int nItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_UPDATE, nItem, 0L);
	}
	BOOL SetItemState(int nItem, LV_ITEM* pItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)pItem);
	}
	UINT GetItemState(int nItem, UINT nMask) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, LVM_GETITEMSTATE, nItem, nMask);
	}
	void SetItemCount(int nItems)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, 0L);
	}
	BOOL SortItems(PFNLVCOMPARE pfnCompare, DWORD dwData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SORTITEMS, dwData, (LPARAM)pfnCompare);
	}
	UINT GetSelectedCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, LVM_GETSELECTEDCOUNT, 0, 0L);
	}

	BOOL GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		lpRect->left = nCode;
		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEMRECT, (WPARAM)nItem, (LPARAM)lpRect);
	}

	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
	{
		LV_COLUMN column;
		column.mask = LVCF_TEXT|LVCF_FMT;
		column.pszText = (LPTSTR)lpszColumnHeading;
		column.fmt = nFormat;
		if (nWidth != -1)
		{
			column.mask |= LVCF_WIDTH;
			column.cx = nWidth;
		}
		if (nSubItem != -1)
		{
			column.mask |= LVCF_SUBITEM;
			column.iSubItem = nSubItem;
		}
		return InsertColumn(nCol, &column);
	}

	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM item;
		item.mask = nMask;
		item.iItem = nItem;
		item.iSubItem = 0;
		item.pszText = (LPTSTR)lpszItem;
		item.state = nState;
		item.stateMask = nStateMask;
		item.iImage = nImage;
		item.lParam = lParam;
		return InsertItem(&item);
	}

	int HitTest(POINT pt, UINT* pFlags) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_HITTESTINFO hti;
		hti.pt = pt;
		int nRes = (int)::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)&hti);
		if (pFlags != NULL)
			*pFlags = hti.flags;
		return nRes;
	}

	BOOL SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
		int nImage, UINT nState, UINT nStateMask, LPARAM lParam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM lvi;
		lvi.mask = nMask;
		lvi.iItem = nItem;
		lvi.iSubItem = nSubItem;
		lvi.stateMask = nStateMask;
		lvi.state = nState;
		lvi.pszText = (LPTSTR) lpszItem;
		lvi.iImage = nImage;
		lvi.lParam = lParam;
		return (BOOL)::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi);
	}

#ifndef _ATL_NO_COM
	BOOL GetItemText(int nItem, int nSubItem, BSTR& bstrText) const
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bstrText == NULL);
		LV_ITEM lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.iSubItem = nSubItem;

		LPTSTR lpstrText = NULL;
		int nLen = 128;
		int nRes;
		do
		{
			nLen *= 2;
			lvi.cchTextMax = nLen;
			if(lpstrText != NULL)
			{
				delete [] lpstrText;
				lpstrText = NULL;
			}
			ATLTRY(lpstrText = new TCHAR[nLen]);
			if(lpstrText == NULL)
				break;
			lpstrText[0] = NULL;
			lvi.pszText = lpstrText;
			nRes  = (int)::SendMessage(m_hWnd, LVM_GETITEMTEXT, (WPARAM)nItem,
				(LPARAM)&lvi);
		} while (nRes == nLen-1);

		bstrText = ::SysAllocString(T2OLE(lpstrText));
		delete [] lpstrText;

		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif //!_ATL_NO_COM

	int GetItemText(int nItem, int nSubItem, LPTSTR lpszText, int nLen) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.iSubItem = nSubItem;
		lvi.cchTextMax = nLen;
		lvi.pszText = lpszText;
		return (int)::SendMessage(m_hWnd, LVM_GETITEMTEXT, (WPARAM)nItem, (LPARAM)&lvi);
	}

	DWORD GetItemData(int nItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_ITEM lvi;
		memset(&lvi, 0, sizeof(LV_ITEM));
		lvi.iItem = nItem;
		lvi.mask = LVIF_PARAM;
		::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)&lvi);
		return (DWORD)lvi.lParam;
	}

	void RemoveImageList(int nImageList)
	{
		HIMAGELIST h = (HIMAGELIST)::SendMessage(m_hWnd, LVM_GETIMAGELIST, (WPARAM)nImageList, 0L);
		if (h != NULL)
			::SendMessage(m_hWnd, LVM_SETIMAGELIST, (WPARAM)nImageList, 0L);
	}

	HIMAGELIST CreateDragImage(int nItem, LPPOINT lpPoint)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HIMAGELIST)::SendMessage(m_hWnd, LVM_CREATEDRAGIMAGE, nItem, (LPARAM)lpPoint);
	}

	BOOL AddColumn(LPCTSTR strItem,int nItem,int nSubItem = -1,
			int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
			int nFmt = LVCFMT_LEFT)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		LV_COLUMN lvc;
		lvc.mask = nMask;
		lvc.fmt = nFmt;
		lvc.pszText = (LPTSTR)strItem;
		lvc.cx = GetStringWidth(lvc.pszText) + 15;
		if(nMask & LVCF_SUBITEM)
		{
			if(nSubItem != -1)
				lvc.iSubItem = nSubItem;
			else
				lvc.iSubItem = nItem;
		}
		return InsertColumn(nItem, &lvc);
	}

	BOOL AddItem(int nItem,int nSubItem,LPCTSTR strItem,int nImageIndex = -1)
	{
		LV_ITEM lvItem;
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = nItem;
		lvItem.iSubItem = nSubItem;
		lvItem.pszText = (LPTSTR) strItem;
		if(nImageIndex != -1){
			lvItem.mask |= LVIF_IMAGE;
			lvItem.iImage = nImageIndex;
		}
		if(nSubItem == 0)
			return InsertItem(&lvItem);
		return SetItem(&lvItem);
	}

	// single-selection only
	int GetSelectedIndex() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_SINGLESEL);

		return (int)::SendMessage(m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
	}

	BOOL GetSelectedItem(LV_ITEM* pItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_SINGLESEL);
		ATLASSERT(pItem != NULL);

		pItem->iItem = (int)::SendMessage(m_hWnd, LVM_GETNEXTITEM, -1, MAKELPARAM(LVNI_ALL | LVNI_SELECTED, 0));
		if(pItem->iItem == -1)
			return FALSE;

		return (BOOL)::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)pItem);
	}

	// new common control support
#if (_WIN32_IE >= 0x0400)
	BOOL GetBkImage(LPLVBKIMAGE plvbki)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETBKIMAGE, 0, (LPARAM)plvbki);
	}
	BOOL SetBkImage(LPLVBKIMAGE plvbki)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETBKIMAGE, 0, (LPARAM)plvbki);
	}
	DWORD GetExtendedListViewStyle()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0L);
	}
	// dwExMask = 0 means all styles
	DWORD SetExtendedListViewStyle(DWORD dwExStyle, DWORD dwExMask = 0)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, dwExMask, dwExStyle);
	}
	HCURSOR GetHotCursor()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, LVM_GETHOTCURSOR, 0, 0L);
	}
	HCURSOR SetHotCursor(HCURSOR hHotCursor)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HCURSOR)::SendMessage(m_hWnd, LVM_SETHOTCURSOR, 0, (LPARAM)hHotCursor);
	}
	int GetHotItem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_GETHOTITEM, 0, 0L);
	}
	int SetHotItem(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_SETHOTITEM, nIndex, 0L);
	}
	int GetSelectionMark()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_GETSELECTIONMARK, 0, 0L);
	}
	int SetSelectionMark(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_SETSELECTIONMARK, 0, nIndex);
	}
	BOOL GetWorkAreas(int nWorkAreas, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETWORKAREAS, nWorkAreas, (LPARAM)lpRect);
	}
	BOOL SetWorkAreas(int nWorkAreas, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETWORKAREAS, nWorkAreas, (LPARAM)lpRect);
	}
	BOOL GetColumnOrderArray(int nCount, int* lpnArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETCOLUMNORDERARRAY, nCount, (LPARAM)lpnArray);
	}
	BOOL SetColumnOrderArray(int nCount, int* lpnArray)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_SETCOLUMNORDERARRAY, nCount, (LPARAM)lpnArray);
	}
	DWORD GetHoverTime()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetExtendedListViewStyle() & (LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE));
		return ::SendMessage(m_hWnd, LVM_GETHOVERTIME, 0, 0L);
	}
	DWORD SetHoverTime(DWORD dwHoverTime)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetExtendedListViewStyle() & (LVS_EX_TRACKSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_TWOCLICKACTIVATE));
		return ::SendMessage(m_hWnd, LVM_SETHOVERTIME, 0, dwHoverTime);
	}
	HWND GetHeader()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HWND)::SendMessage(m_hWnd, LVM_GETHEADER, 0, 0L);
	}
	BOOL GetSubItemRect(int nItem, int nSubItem, int nFlag, LPRECT lpRect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_REPORT);
		ATLASSERT(lpRect != NULL);
		lpRect->top = nSubItem;
		lpRect->left = nFlag;
		return (BOOL)::SendMessage(m_hWnd, LVM_GETSUBITEMRECT, nItem, (LPARAM)lpRect);
	}
	BOOL GetCheckState(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetExtendedListViewStyle() & LVS_EX_CHECKBOXES);
		UINT uRet = GetItemState(nIndex, LVIS_STATEIMAGEMASK);
//REVIEW
		return (uRet >> 12) - 1;
	}
	BOOL SetCheckState(int nItem, BOOL bCheck)
	{
		int nCheck = bCheck ? 2 : 1;    // one based index
		return SetItemState(nItem, INDEXTOSTATEIMAGEMASK(nCheck), LVIS_STATEIMAGEMASK);
	}
	DWORD ApproximateViewRect(int cx = -1, int cy = -1, int nCount = -1)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, LVM_APPROXIMATEVIEWRECT, nCount, MAKELPARAM(cx, cy));
	}
	BOOL GetNumberOfWorkAreas(int* pnWorkAreas)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, LVM_GETNUMBEROFWORKAREAS, 0, (LPARAM)pnWorkAreas);
	}
	DWORD SetIconSpacing(int cx, int cy)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(GetStyle() & LVS_ICON);
		return ::SendMessage(m_hWnd, LVM_SETICONSPACING, 0, MAKELPARAM(cx, cy));
	}
	void SetItemCountEx(int nItems, DWORD dwFlags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT((GetStyle() & LVS_OWNERDATA) && (GetStyle() & (LVS_REPORT | LVS_LIST)));
		::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, dwFlags);
	}
	int SubItemHitTest(LPLVHITTESTINFO lpInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, LVM_SUBITEMHITTEST, 0, (LPARAM)lpInfo);
	}
#endif //(_WIN32_IE >= 0x0400)
};

typedef CListCtrlT<CWindow>     CListCtrl;

#define WC_COMBOBOXEXW         L"ComboBoxEx32"
#define WC_COMBOBOXEXA         "ComboBoxEx32"

#ifdef UNICODE
#define WC_COMBOBOXEX          WC_COMBOBOXEXW
#else
#define WC_COMBOBOXEX          WC_COMBOBOXEXA
#endif


#define CBEIF_TEXT              0x00000001
#define CBEIF_IMAGE             0x00000002
#define CBEIF_SELECTEDIMAGE     0x00000004
#define CBEIF_OVERLAY           0x00000008
#define CBEIF_INDENT            0x00000010
#define CBEIF_LPARAM            0x00000020

#define CBEIF_DI_SETITEM        0x10000000

#define CBEM_INSERTITEMA        (WM_USER + 1)
#define CBEM_SETIMAGELIST       (WM_USER + 2)
#define CBEM_GETIMAGELIST       (WM_USER + 3)
#define CBEM_GETITEMA           (WM_USER + 4)
#define CBEM_SETITEMA           (WM_USER + 5)
#define CBEM_DELETEITEM         CB_DELETESTRING
#define CBEM_GETCOMBOCONTROL    (WM_USER + 6)
#define CBEM_GETEDITCONTROL     (WM_USER + 7)
#if (_WIN32_IE >= 0x0400)
#define CBEM_SETEXSTYLE         (WM_USER + 8)  // use  SETEXTENDEDSTYLE instead
#define CBEM_SETEXTENDEDSTYLE   (WM_USER + 14)   // lparam == new style, wParam (optional) == mask
#define CBEM_GETEXSTYLE         (WM_USER + 9) // use GETEXTENDEDSTYLE instead
#define CBEM_GETEXTENDEDSTYLE   (WM_USER + 9)
#define CBEM_SETUNICODEFORMAT   CCM_SETUNICODEFORMAT
#define CBEM_GETUNICODEFORMAT   CCM_GETUNICODEFORMAT
#else
#define CBEM_SETEXSTYLE         (WM_USER + 8)
#define CBEM_GETEXSTYLE         (WM_USER + 9)
#endif
#define CBEM_HASEDITCHANGED     (WM_USER + 10)
#define CBEM_INSERTITEMW        (WM_USER + 11)
#define CBEM_SETITEMW           (WM_USER + 12)
#define CBEM_GETITEMW           (WM_USER + 13)

template <class Base>
class TComboBoxT : public Base
{
public:
// Constructors
	TComboBoxT(HWND hWnd = NULL) : Base(hWnd) { }

	TComboBoxT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	}
	HWND Create(HWND hWndParent, LPRECT lpRect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			HMENU hMenu = NULL, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, lpRect, szWindowName, dwStyle, dwExStyle, hMenu, lpCreateParam);
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return _T("COMBOBOX");
	}

	// for entire combo box
	int GetCount() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETCOUNT, 0, 0L);
	}
	int GetCurSel() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETCURSEL, 0, 0L);
	}
	int SetCurSel(int nSelect)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETCURSEL, nSelect, 0L);
	}
	LCID GetLocale() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LCID)::SendMessage(m_hWnd, CB_GETLOCALE, 0, 0L);
	}
	LCID SetLocale(LCID nNewLocale)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LCID)::SendMessage(m_hWnd, CB_SETLOCALE, (WPARAM)nNewLocale, 0L);
	}
	int GetTopIndex() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETTOPINDEX, 0, 0L);
	}
	int SetTopIndex(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETTOPINDEX, nIndex, 0L);
	}
	int InitStorage(int nItems, UINT nBytes)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_INITSTORAGE, (WPARAM)nItems, nBytes);
	}
	void SetHorizontalExtent(UINT nExtent)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_SETHORIZONTALEXTENT, nExtent, 0L);
	}
	UINT GetHorizontalExtent() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (UINT)::SendMessage(m_hWnd, CB_GETHORIZONTALEXTENT, 0, 0L);
	}
	int SetDroppedWidth(UINT nWidth)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETDROPPEDWIDTH, nWidth, 0L);
	}
	int GetDroppedWidth() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETDROPPEDWIDTH, 0, 0L);
	}

	// for edit control
	DWORD GetEditSel() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CB_GETEDITSEL, 0, 0L);
	}
	BOOL LimitText(int nMaxChars)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_LIMITTEXT, nMaxChars, 0L);
	}
	BOOL SetEditSel(int nStartChar, int nEndChar)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELONG(nStartChar, nEndChar));
	}

	// for combobox item
	DWORD GetItemData(int nIndex) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CB_GETITEMDATA, nIndex, 0L);
	}
	int SetItemData(int nIndex, DWORD dwItemData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
	}
	void* GetItemDataPtr(int nIndex) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LPVOID)GetItemData(nIndex);
	}
	int SetItemDataPtr(int nIndex, void* pData)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return SetItemData(nIndex, (DWORD)(LPVOID)pData);
	}
	int GetLBText(int nIndex, LPTSTR lpszText) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETLBTEXT, nIndex, (LPARAM)lpszText);
	}
#ifndef _ATL_NO_COM
	BOOL GetLBTextBSTR(int nIndex, BSTR& bstrText) const
	{
		USES_CONVERSION;
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(bstrText == NULL);

		int nLen = GetLBTextLen(nIndex);
		if(nLen == CB_ERR)
			return FALSE;

		LPTSTR lpszText = (LPTSTR)_alloca((nLen + 1) * sizeof(TCHAR));

		if(GetLBText(nIndex, lpszText) == CB_ERR)
			return FALSE;

		bstrText = ::SysAllocString(T2OLE(lpszText));
		return (bstrText != NULL) ? TRUE : FALSE;
	}
#endif //!_ATL_NO_COM
	int GetLBTextLen(int nIndex) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETLBTEXTLEN, nIndex, 0L);
	}

	int SetItemHeight(int nIndex, UINT cyItemHeight)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
	}
	int GetItemHeight(int nIndex) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETITEMHEIGHT, nIndex, 0L);
	}
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
	}
	int SetExtendedUI(BOOL bExtended = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SETEXTENDEDUI, bExtended, 0L);
	}
	BOOL GetExtendedUI() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_GETEXTENDEDUI, 0, 0L);
	}
	void GetDroppedControlRect(LPRECT lprect) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_GETDROPPEDCONTROLRECT, 0, (DWORD)lprect);
	}
	BOOL GetDroppedState() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CB_GETDROPPEDSTATE, 0, 0L);
	}

// Operations
	// for drop-down combo boxes
	void ShowDropDown(BOOL bShowIt = TRUE)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_SHOWDROPDOWN, bShowIt, 0L);
	}

	// manipulating listbox items
	int AddString(LPCTSTR lpszString)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpszString);
	}
	int DeleteString(UINT nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_DELETESTRING, nIndex, 0L);
	}
	int InsertString(int nIndex, LPCTSTR lpszString)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_INSERTSTRING, nIndex, (LPARAM)lpszString);
	}
	void ResetContent()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0L);
	}
	int Dir(UINT attr, LPCTSTR lpszWildCard)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_DIR, attr, (LPARAM)lpszWildCard);
	}

	// selection helpers
	int FindString(int nStartAfter, LPCTSTR lpszString) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_FINDSTRING, nStartAfter, (LPARAM)lpszString);
	}
	int SelectString(int nStartAfter, LPCTSTR lpszString)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_SELECTSTRING, nStartAfter, (LPARAM)lpszString);
	}

	// Clipboard operations
	void Clear()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CLEAR, 0, 0L);
	}
	void Copy()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_COPY, 0, 0L);
	}
	void Cut()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_CUT, 0, 0L);
	}
	void Paste()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, WM_PASTE, 0, 0L);
	}
};

typedef TComboBoxT<CWindow> TComboBox;


#ifdef UNICODE
#define COMBOBOXEXITEM            COMBOBOXEXITEMW
#define PCOMBOBOXEXITEM           PCOMBOBOXEXITEMW
#define PCCOMBOBOXEXITEM          PCCOMBOBOXEXITEMW
#else
#define COMBOBOXEXITEM            COMBOBOXEXITEMA
#define PCOMBOBOXEXITEM           PCOMBOBOXEXITEMA
#define PCCOMBOBOXEXITEM          PCCOMBOBOXEXITEMA
#endif

#ifdef UNICODE
#define CBEM_INSERTITEM         CBEM_INSERTITEMW
#define CBEM_SETITEM            CBEM_SETITEMW
#define CBEM_GETITEM            CBEM_GETITEMW
#else
#define CBEM_INSERTITEM         CBEM_INSERTITEMA
#define CBEM_SETITEM            CBEM_SETITEMA
#define CBEM_GETITEM            CBEM_GETITEMA
#endif

template <class Base>
class TComboBoxExT : public TComboBoxT< Base >
{
public:
// Constructors
	TComboBoxExT(HWND hWnd = NULL) : TComboBoxT< Base >(hWnd) { }

	TComboBoxExT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, RECT& rcPos, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			UINT nID = 0, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, rcPos, szWindowName, dwStyle, dwExStyle, nID, lpCreateParam);
	}
	HWND Create(HWND hWndParent, LPRECT lpRect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = 0, DWORD dwExStyle = 0,
			HMENU hMenu = NULL, LPVOID lpCreateParam = NULL)
	{
		return CWindow::Create(GetWndClassName(), hWndParent, lpRect, szWindowName, dwStyle, dwExStyle, hMenu, lpCreateParam);
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return WC_COMBOBOXEX;
	}

#if (_WIN32_IE >= 0x0400)
	DWORD GetExtendedStyle() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CBEM_GETEXTENDEDSTYLE, 0, 0L);
	}
	DWORD SetExtendedStyle(DWORD dwExMask, DWORD dwExStyle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, CBEM_SETEXTENDEDSTYLE, dwExMask, dwExStyle);
	}
#endif //(_WIN32_IE >= 0x0400)
	HIMAGELIST GetImageList() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HIMAGELIST)::SendMessage(m_hWnd, CBEM_GETIMAGELIST, 0, 0L);
	}
	HIMAGELIST SetImageList(HIMAGELIST hImageList)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HIMAGELIST)::SendMessage(m_hWnd, CBEM_SETIMAGELIST, 0, (LPARAM)hImageList);
	}

// Operations
	int InsertItem(const COMBOBOXEXITEM FAR* lpcCBItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CBEM_INSERTITEM, 0, (LPARAM)lpcCBItem);
	}
	int DeleteItem(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CBEM_DELETEITEM, nIndex, 0L);
	}
	BOOL GetItem(PCOMBOBOXEXITEM pCBItem) const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBEM_GETITEM, 0, (LPARAM)pCBItem);
	}
	BOOL SetItem(const COMBOBOXEXITEM FAR* lpcCBItem)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBEM_SETITEM, 0, (LPARAM)lpcCBItem);
	}
	HWND GetComboCtrl() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HWND)::SendMessage(m_hWnd, CBEM_GETCOMBOCONTROL, 0, 0L);
	}
	HWND GetEditCtrl() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (HWND)::SendMessage(m_hWnd, CBEM_GETEDITCONTROL, 0, 0L);
	}
	BOOL HasEditChanged() const
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, CBEM_HASEDITCHANGED, 0, 0L);
	}
};

typedef TComboBoxExT<CWindow>       TComboBoxEx;

#endif