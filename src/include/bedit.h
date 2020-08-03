// BEdit.h: interface for the CBEdit class.
// Created: 11/26/02 by AG
//////////////////////////////////////////////////////////////////////
/* Purpose: subclass a CEdit window in order to handle the CONTAINS_BRL_TEXT
	message. Used for dialog edit boxes that may contain grade 2 text, when the
	application view contains a grade 2 document. */
#if !defined(AFX_BEDIT_H__3E2B0574_B0C0_4780_BF9D_7CA28F57B057__INCLUDED_)
#define AFX_BEDIT_H__3E2B0574_B0C0_4780_BF9D_7CA28F57B057__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// comes from c:\p4\JAWS\WINCE......\include

class CBEdit : public CWindowImpl<CBEdit, CEdit>
{
public:
	CBEdit(HWND hWnd = NULL,BOOL bContainsBrlText=FALSE)
	{
		m_bBrlText = bContainsBrlText;
	}
	virtual ~CBEdit() {};
	BEGIN_MSG_MAP(CBEdit)
		MESSAGE_HANDLER(::RegisterWindowMessage(L"CONTAINS_BRL_TEXT"), OnContainsBrlText)
	END_MSG_MAP()
	LRESULT OnContainsBrlText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return m_bBrlText;
	}
protected:
	BOOL m_bBrlText;
};

#endif // !defined(AFX_BEDIT_H__3E2B0574_B0C0_4780_BF9D_7CA28F57B057__INCLUDED_)
