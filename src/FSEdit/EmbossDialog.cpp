// EmbossDialog.cpp: implementation of the CEmbossDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "EmbossDialog.h"
#include "EmbossPropDlg.h"
#include "gtutility.h"


#define MIN_NUMBER_OF_COPIES      1
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


LRESULT CEmbossDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CenterWindow();
/*
	CArkBaseDoc *pDoc = m_pOwnerDlg->GetDocument();
	ASSERT( pDoc);
	char cPageCount[1];
	int nPageCount = pDoc->GetPageCount();	
	m_FromEdit.SetWindowText( _T("1") );
	_itot( nPageCount,cPageCount, 10);
	m_ToEdit.SetWindowText( cPageCount );
*/
	
	m_btnProperties.Attach(GetDlgItem(IDC_BUTTON_PROPERTIES_E ));

//	m_btnAllPages.Attach(GetDlgItem(IDC_RADIO_ALLPAGES_E));
//	m_btnAllPages.EnableWindow( TRUE );
//	m_btnAllPages.SetCheck( TRUE );
	
//	m_btnPages.Attach(GetDlgItem(IDC_RADIO_PAGES_E ));	
//	m_btnPages.EnableWindow( TRUE );

	
	m_btnGrade2Braille.Attach(GetDlgItem(IDC_CHECK_GRADE2_E ));
	
	m_btnEmphasizedText.Attach(GetDlgItem(IDC_CHECK_EMPHASIZED_TXT_E ));
	m_btnEmphasizedText.SetCheck( TRUE );

	DocProp Prop;
	m_view.GetDocumentProp(Prop);
	if(Prop.brailleGrade2)
	{  // if it is a grade 2 document we want to disable the grade 2 option
		m_btnEmphasizedText.SetFocus();
		m_btnGrade2Braille.SetCheck( FALSE );
		m_btnGrade2Braille.EnableWindow( FALSE );
	}
	else
	{  // if it is not a grade 2 document we want to enable the grade 2 option
		m_btnGrade2Braille.EnableWindow( TRUE );
		m_btnGrade2Braille.SetCheck( TRUE );
		m_btnGrade2Braille.SetFocus();
	}

	m_btnOk.Attach(GetDlgItem(IDOK ));
	m_btnCancel.Attach(GetDlgItem(IDCANCEL ));
	
//	m_staTo.Attach(GetDlgItem(IDC_STATIC_TO_E ));
//	m_staTo.EnableWindow( TRUE );
	
//	m_staFrom.Attach(GetDlgItem(IDC_STATIC_FROM_E ));
//	m_staFrom.EnableWindow( TRUE );
	
//	m_edtTo.Attach(GetDlgItem(IDC_EDIT_TO_E ));
//	m_edtTo.EnableWindow( FALSE );
	
//	m_edtFrom.Attach(GetDlgItem(IDC_EDIT_FROM_E ));
//	m_edtFrom.EnableWindow( FALSE );
	
	m_edtNumCopies.Attach(GetDlgItem(IDC_EDIT_NUMCOPIES_E ));
	m_edtNumCopies.SetWindowText( _T("1") );

	// This "OK" button will generate an IDCANCEL when the PACmate "Close"
	// button is pressed.

	return 0;
}

LRESULT CEmbossDialog::OnPropertiesSelected(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
/*
	// Set Parameters for Print Dialog 
	PRINTDLG pd;
	memset(&pd, 0, sizeof(pd));
	pd.cbStruct = sizeof(pd);
	pd.hwndOwner = m_view.m_hWnd;
	pd.dwFlags = PD_DISABLEMARGINS;
	if(PrintDlg(&pd) == FALSE)
	{
		ProcessPrintDlgError(CommDlgExtendedError());
		return 0;
	}

	// Initialize DOCINFO
	CComBSTR bstr;
	GetWindowText(bstr.m_str);
	DOCINFO docinfo;
	memset(&docinfo, 0, sizeof(docinfo));
	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = bstr.m_str;

	// Start the printing process.
	if(!(::StartDoc(pd.hdc, &docinfo) < 0))
	{
		SIZE Size;
		// Get Page Size and set the page rect.
		Size.cx = SMulDiv(GetDeviceCaps(pd.hdc, PHYSICALWIDTH), 1440, GetDeviceCaps(pd.hdc, LOGPIXELSX));
		Size.cy = SMulDiv(GetDeviceCaps(pd.hdc, PHYSICALHEIGHT), 1440, GetDeviceCaps(pd.hdc, LOGPIXELSY));
		RECT PageRect = {0, 0, Size.cx, Size.cy};

		// Calculate margins.
		RECT rcMargins = GetPrinterMargins(pd.hdc);

		// Calculate the rendering rect and convert to twips.
		RECT PrintRect = {rcMargins.left, rcMargins.top,
			Size.cx - SMulDiv(rcMargins.right, 1440, GetDeviceCaps(pd.hdc, LOGPIXELSX)),
			Size.cy - SMulDiv(rcMargins.bottom, 1440, GetDeviceCaps(pd.hdc, LOGPIXELSX))};

		PrintRect.right -= 1440 + SMulDiv(rcMargins.right, 1440, GetDeviceCaps(pd.hdc, LOGPIXELSX));
		PrintRect.left += 1440 - SMulDiv(rcMargins.left, 1440, GetDeviceCaps(pd.hdc, LOGPIXELSX));
		PrintRect.bottom -= 1440 + SMulDiv(rcMargins.bottom, 1440, GetDeviceCaps(pd.hdc, LOGPIXELSY));
		PrintRect.top += 1440 - SMulDiv(rcMargins.top, 1440, GetDeviceCaps(pd.hdc, LOGPIXELSY));

		// Get number of characters in skipper.
		GETTEXTLENGTHEX tex = {GTL_DEFAULT, 1200};
		long lastChar = 0, lastOffset = ::SendMessage(m_view.m_hWnd, EM_GETTEXTLENGTHEX, (WPARAM)&tex, 0);

		FORMATRANGE fr;
		// Begin sending pages to printer;
		do
		{
			if(::StartPage(pd.hdc) < 0)
				break; // Error
			else
			{
				fr.hdcTarget = pd.hdc;
				fr.hdc = pd.hdc;
				fr.rcPage = PageRect;
				fr.rc = PrintRect;
				lastChar = ::SendMessage(m_view.m_hWnd, EM_FORMATRANGE, (WPARAM)TRUE, (LPARAM)&fr);
				fr.chrg.cpMin = lastChar;
				fr.chrg.cpMax = lastOffset;
				::EndPage(pd.hdc);
			}
		}while(fr.chrg.cpMin != -1 && fr.chrg.cpMin < lastOffset);

		::EndDoc(pd.hdc);
	}

	DeleteDC(pd.hdc);	
*/

	CEmbossPropDlg ePropDlg;
	if ( ePropDlg.DoModal() == IDOK)
	{ 
		CString sCloseButton;
		sCloseButton.LoadString( IDS_CLOSE_LABEL);
		m_btnCancel.SetWindowText( sCloseButton );	
	}	
	return 0;
}

LRESULT CEmbossDialog::OnConnection(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CEmbossConnectDlg dlg;

	dlg.DoModal();

	return 0;
}

LRESULT CEmbossDialog::OnRadioAllpages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CEmbossDialog::OnRadioPages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CEmbossDialog::OnKillfocusEditFrom(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CEmbossDialog::OnKillfocusEditTo(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CEmbossDialog::OnKillfocusEditNumcopies(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szInValue[5];
    m_edtNumCopies.GetWindowText( szInValue, 5 );
    WORD wCopies =  (WORD)_ttoi( szInValue );     // save since control only allows ints
	if ( wCopies < MIN_NUMBER_OF_COPIES )
	{
		_TCHAR szOutValue[1];
		wCopies = MIN_NUMBER_OF_COPIES;
		_itot_s( wCopies, szOutValue, 10);
		m_edtNumCopies.SetWindowText( szOutValue );
	}	
	return 0;
}

LRESULT CEmbossDialog::OnChangeEditNumcopies(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CEmbossDialog::OnChangeGrade2(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	if (m_btnGrade2Braille.GetCheck())
	{
		m_btnEmphasizedText.EnableWindow( TRUE );
	}
	else
	{
		m_btnEmphasizedText.EnableWindow( FALSE );
	}	
	return 0;
}

// This handler gets both the IDOK and IDCANCEL messages.
LRESULT CEmbossDialog::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	m_bGrade2Braille = m_btnGrade2Braille.GetCheck();
	if ( m_btnEmphasizedText.IsWindowEnabled() )
	{
		m_bEmphasizedText = m_btnEmphasizedText.GetCheck();
	}
	else
	{
		m_bEmphasizedText = FALSE;
	}
	_TCHAR szText[5];
	m_edtNumCopies.GetWindowText( szText, 5 );
	m_iNumCopies = _ttoi(szText);
	
	// Write Embosser Properties to Registry
	GTUtility::RegDataType type;
	int j =0;
	int i = BEGIN_EMB_TVAL;
	for( ; i < END_EMB_TVAL; i++, j++)
	{
		type.dwType = Skipper::_Emb[i].second;
		if ( type.dwType == REG_DWORD )
		{
			type.dw = DwordRegParam(*this, aEmbTempVal[j]);
		}
		else
		{
			_tcscpy_s( type.sz, CStringRegParam( *this, aEmbTempVal[j]));
		}
		GTUtility::SetRegistryValue(GTUtility::bstrFSEmb, NULL, Skipper::_Emb[i].first, type);
	}

	EndDialog(IDOK);

	return 0;
}

// This handler processes the ID_EMBOSS_CANCEL messge generated by the "Cancel" button.
LRESULT CEmbossDialog::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}