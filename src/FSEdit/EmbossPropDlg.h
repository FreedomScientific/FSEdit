// EmbossPropDlg.h: interface for the CEmbossPropDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMBOSSPROPDLG_H__3C9835A9_7CB9_4515_9A50_08AAE6E4834C__INCLUDED_)
#define AFX_EMBOSSPROPDLG_H__3C9835A9_7CB9_4515_9A50_08AAE6E4834C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "atldlgs.h"
#include <map>

class CEmbossConnectDlg : public CDialogImpl<CEmbossConnectDlg>
{
public:
	BOOL m_bIsNetworkEmb;
	CString m_sConnectionSel;
	CString m_sNetworkPortSel;

private:
	CComboBox m_cbxConnection;
	CEdit m_edtNetworkPort;

	BOOL GetSerialPorts (LPBOOL lpbPorts,UINT uiSize);

public:
	enum{IDD = IDC_EMBOSS_CONNECTION_DLG};

	BEGIN_MSG_MAP(CEmbossConnectDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_HANDLER(IDC_COMBO_CONNECTION_E, CBN_SELCHANGE,OnSelectConnectionChng)
	END_MSG_MAP();

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnSelectConnectionChng(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
};

class CEmbossPropDlg : public CDialogImpl<CEmbossPropDlg> 
{
//	map<CString, DWORD> mapEmbProp;
public:

	CEmbossPropDlg(){}

	CButton	m_btnOk;
	CButton	m_btnCancel;
	CButton m_btnInterpointPages;
	CButton m_btnInterpointMargins;	
	CComboBox m_cbxEmbosser;
//	CComboBox m_cbxConnection;
//	CComboBox m_cbxNetworkDevice;
	CStatic m_grpCells;
	CStatic m_grpLines;
	CStatic m_staMaxCells;	
	CStatic m_staCells;
	CStatic m_staMaxLines;
	CStatic m_staLines;
	CStatic	m_staWhereVal;
	CEdit m_edtTopMargin;
	CEdit m_edtLeftMargin;
	CEdit m_edtMaxCellsPerLine;
	CEdit m_edtCellsPerLine;
	CEdit m_edtMaxLinesPerPage;
	CEdit m_edtLinesPerPage;
//	CEdit m_edtNetworkPort;

	enum {IDD = IDD_EMBOSS_PROP_DLG};

	BEGIN_MSG_MAP(CEmbossDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)

		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)	
		COMMAND_HANDLER(IDC_RADIO_PAGES_E, BN_CLICKED,OnRadioPages)		
		COMMAND_HANDLER(IDC_COMBO_CONNECTION_E, CBN_SETFOCUS,OnFocusConnection)		
		COMMAND_HANDLER(IDC_COMBO_EMBOSSER_E, CBN_SETFOCUS,OnFocusEmbosser)		
//		COMMAND_HANDLER(IDC_COMBO_PRINTERNAME, CBN_SETFOCUS,OnFocusNetworkDevice)		
		COMMAND_HANDLER(IDC_EDIT_MAX_CELLS_E, EN_KILLFOCUS,OnKillfocusEditMaxCells)		
		COMMAND_HANDLER(IDC_EDIT_MAX_LINES_E, EN_KILLFOCUS,OnKillfocusEditMaxLines)		
		COMMAND_HANDLER(IDC_EDIT_TOP_MARGIN_E, EN_KILLFOCUS,OnKillfocusEditTopMargin)	
		COMMAND_HANDLER(IDC_EDIT_LEFT_MARGIN_E, EN_KILLFOCUS,OnKillfocusEditLeftMargin)	
//		COMMAND_HANDLER(IDC_COMBO_CONNECTION_E, CBN_SELCHANGE,OnSelectConnection)
		COMMAND_HANDLER(IDC_COMBO_EMBOSSER_E, CBN_SELCHANGE,OnSelectEmbosser)		
		COMMAND_HANDLER(IDC_CHECK_INTERPOINT_PAGES_E, EN_KILLFOCUS,OnKillfocusCheckInterpointPages)		
		COMMAND_HANDLER(IDC_CHECK_INTERPOINT_MARGINS_E, EN_KILLFOCUS,OnKillfocusCheckInterpointMargins)		
	
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnRadioPages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnFocusConnection(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnFocusEmbosser(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnFocusNetworkDevice(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditMaxCells(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditMaxLines(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditCells(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditLines(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditTopMargin(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusEditLeftMargin(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
//	LRESULT OnSelectConnection(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnSelectEmbosser(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusCheckInterpointPages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnKillfocusCheckInterpointMargins(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);

	BOOL m_bEmbosserGoesToNewLine;
	BOOL m_bEmbosserGoesToNewPage;

	BOOL m_bInterpointMarginsValue;
	BOOL m_bInterpointPagesValue;

	int m_iCellsPerLineValue;
	int m_iMaxCellsPerLineValue;

	int m_iLinesPerPageValue;
	int m_iMaxLinesPerPageValue;

	int m_iTopMarginValue;
	int m_iBottomMarginValue;
	int m_iLeftMarginValue;
	int m_iRightMarginValue;

	CString m_sEmbosserSel;
//	CString m_sConnectionSel;	
	CString m_sNetworkDeviceSel;
//	CString m_sNetworkPortSel;

//	BOOL m_bIsNetworkEmb;

private:
	BOOL GetSerialPorts (LPBOOL lpbPorts,UINT uiSize);
	static const UINT MAX_NUMBER_COMBO;
	BOOL InitializeEmbosserCombo();
//	BOOL InitializeNetworkDeviceCombo();
//	BOOL InitializeConnectionCombo();
	void AdjustEditFields( const CString& sEmbosser, BOOL bMakeChanges = TRUE );
	void SetDefaultMemberVariables();
};

#define BEGIN_EMB_PROP    0
#define END_EMB_PROP      ArraySize<sizeof(aEmbProp),sizeof(size_t)>::Val

#define BEGIN_CONNECT	END_EMB_PROP
#define END_CONNECT		ArraySize<sizeof(aConnect),sizeof(size_t)>::Val + END_EMB_PROP

const size_t aEmbProp[] =
{	
	// PERMANENT PROPERTIES
	offsetof(CEmbossPropDlg, m_bEmbosserGoesToNewLine),
	offsetof(CEmbossPropDlg, m_bEmbosserGoesToNewPage),	
	offsetof(CEmbossPropDlg, m_bInterpointMarginsValue),
	offsetof(CEmbossPropDlg, m_bInterpointPagesValue),
	offsetof(CEmbossPropDlg, m_iCellsPerLineValue),
	offsetof(CEmbossPropDlg, m_iMaxCellsPerLineValue),
	offsetof(CEmbossPropDlg, m_iLinesPerPageValue),
	offsetof(CEmbossPropDlg, m_iMaxLinesPerPageValue),
	offsetof(CEmbossPropDlg, m_iTopMarginValue),
	offsetof(CEmbossPropDlg, m_iBottomMarginValue),
	offsetof(CEmbossPropDlg, m_iLeftMarginValue),
	offsetof(CEmbossPropDlg, m_iRightMarginValue),
	offsetof(CEmbossPropDlg, m_sEmbosserSel),
//	offsetof(CEmbossPropDlg, m_sConnectionSel),
	offsetof(CEmbossPropDlg, m_sNetworkDeviceSel),
//	offsetof(CEmbossPropDlg, m_sNetworkPortSel),
//	offsetof(CEmbossPropDlg, m_bIsNetworkEmb)
};

const size_t aConnect[] =
{
	offsetof(CEmbossConnectDlg, m_sConnectionSel),
	offsetof(CEmbossConnectDlg, m_sNetworkPortSel),
	offsetof(CEmbossConnectDlg, m_bIsNetworkEmb)
};

#endif // !defined(AFX_EMBOSSPROPDLG_H__3C9835A9_7CB9_4515_9A50_08AAE6E4834C__INCLUDED_)
