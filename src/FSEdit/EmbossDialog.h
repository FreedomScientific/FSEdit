// EmbossDialog.h: interface for the CEmbossDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMBOSSDIALOG_H__CF015FFA_91E4_484B_AB42_CEDF2FF3DFDF__INCLUDED_)
#define AFX_EMBOSSDIALOG_H__CF015FFA_91E4_484B_AB42_CEDF2FF3DFDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "skipperview.h"


class CEmbossDialog : public CDialogImpl<CEmbossDialog>  
{
	CSkipperView& m_view;
public:

	CEmbossDialog(CSkipperView& rview) : m_view( rview ) {}
	
	CButton	m_btnProperties;
//	CButton	m_btnAllPages;
//	CButton	m_btnPages;	
	CButton m_btnGrade2Braille;
	CButton m_btnEmphasizedText;
	CButton	m_btnOk;
	CButton	m_btnCancel;
//	CStatic	m_staTo;
//	CStatic	m_staFrom;
//	CEdit	m_edtTo;
//	CEdit	m_edtFrom;
	CEdit	m_edtNumCopies;

	enum {IDD = IDD_EMBOSS_DLG};

	BEGIN_MSG_MAP(CEmbossDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnOK)
		COMMAND_ID_HANDLER(ID_EMBOSS_CANCEL, OnCancel)
//		ON_WM_VSCROLL()
//		ON_WM_HELPINFO()
		COMMAND_HANDLER(IDC_BUTTON_PROPERTIES_E, BN_CLICKED,OnPropertiesSelected)
		COMMAND_HANDLER(IDC_BUTTON_CONNECTION, BN_CLICKED,OnConnection)
//		COMMAND_HANDLER(IDC_RADIO_SELECTED, BN_CLICKED,OnRadioSelected)		
		COMMAND_HANDLER(IDC_RADIO_ALLPAGES_E, BN_CLICKED,OnRadioAllpages)		
		COMMAND_HANDLER(IDC_RADIO_PAGES_E, BN_CLICKED,OnRadioPages)		
		COMMAND_HANDLER(IDC_EDIT_FROM_E, EN_KILLFOCUS,OnKillfocusEditFrom)		
		COMMAND_HANDLER(IDC_EDIT_TO_E, EN_KILLFOCUS,OnKillfocusEditTo)		
		COMMAND_HANDLER(IDC_EDIT_NUMCOPIES_E, EN_KILLFOCUS,OnKillfocusEditNumcopies)		
		COMMAND_HANDLER(IDC_EDIT_NUMCOPIES_E, EN_CHANGE,OnChangeEditNumcopies)	
		COMMAND_HANDLER(IDC_CHECK_GRADE2_E, BN_CLICKED,OnChangeGrade2)	

	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnPropertiesSelected(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnConnection(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
//	LRESULT OnRadioSelected(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnRadioAllpages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnRadioPages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditFrom(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditTo(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditNumcopies(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnChangeEditNumcopies(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnChangeGrade2(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);

	int  m_iNumCopies;
	BOOL m_bGrade2Braille;
	BOOL m_bEmphasizedText;

};
#define BEGIN_EMB_TVAL     END_CONNECT 
#define END_EMB_TVAL       BEGIN_EMB_TVAL + ArraySize<sizeof(aEmbTempVal),sizeof(size_t)>::Val  

const size_t aEmbTempVal[] =
{
	// TEMP VALUES
	offsetof(CEmbossDialog, m_iNumCopies),
	offsetof(CEmbossDialog, m_bGrade2Braille),	
	offsetof(CEmbossDialog, m_bEmphasizedText),
};
#endif // !defined(AFX_EMBOSSDIALOG_H__CF015FFA_91E4_484B_AB42_CEDF2FF3DFDF__INCLUDED_)
