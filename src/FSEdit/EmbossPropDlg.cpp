// EmbossPropDlg.cpp: implementation of the CEmbossPropDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "EmbossPropDlg.h"
#include "gtutility.h"




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#ifndef COM_MAX
	#define	COM_MAX 17
#endif


#define MAX_CELLS_PER_LINE			100

#define MIN_CELLS_PER_LINE			1

#define MAX_LINES_PER_PAGE			 100
#define MIN_LINES_PER_PAGE			  1
#define MAX_TOP_MARGIN               30
#define MIN_TOP_MARGIN			  	  0
#define MAX_LEFT_MARGIN				 50
#define MIN_LEFT_MARGIN				  0

#define EMB_KEY = _T("\\Software\\Freedom Scientific\\Embosser");



const UINT CEmbossPropDlg::MAX_NUMBER_COMBO = 50;

static const TCHAR* COMBO_EMBOSS_LPT = _T("LPT%i:");
static const TCHAR* COMBO_EMBOSS_COM = _T("COM%i:");
static const TCHAR* COMPORT_STRING = _T("COM");
static const TCHAR* IRDA_STRING	= _T("IRDA");
static const UINT NUMBER_OF_LPT_PORTS = 0;
static const UINT NUMBER_OF_COM_PORTS = 0;
static const TCHAR* PORT_INDICATOR_STRING = _T("\\\\.\\");
//static const TCHAR* COMBO_EMBOSSER = _T("(None)\nBax 10\nBraille Blazer\nBraille Bookmaker\nBraille Express\nBrailleInferno\nBraillo 30\nBraillo 200\nBraillo Comet\nElekul\nEverest-D\nIndex Basic\nJuliet\nLED-120 @25 Lines\nLED-120 @27 Lines\nLED-120 @30 Lines\nMarathon\nMBOSS\nOhtsuki (braille and print)\nOhtsuki (braille only)\nPED\nPortaThiel\nPUMA\nRESUS\nRomeo\nRomeo 25\nTED\nThiel\nThomas\nVersaPoint\nUnlisted\nUnlisted Interpoint\nParagon\nMountbatten\nVersaPoint Duo\nIndex Basic-D\nET\nPortaThiel Interpoint\nThiel Impacto\nJuliet Pro\nBraille Inferno\nPUMA 6\n");
static const TCHAR* COMBO_EMBOSSER = _T("Unlisted\nUnlisted Interpoint\nBax 10\nBraille Blazer\nBraille Bookmaker\nBraille Express\nBraillo 30\nBraillo 200\nBraillo Comet\nElekul\nEverest-D\nIndex Basic\nJuliet\nLED-120 @25 Lines\nLED-120 @27 Lines\nLED-120 @30 Lines\nMarathon\nMBOSS\nOhtsuki (braille and print)\nOhtsuki (braille only)\nPED\nPortaTheil\nRESUS\nRomeo\nRomeo 25\nTED\nThiel\nThomas\nVersaPoint\nParagon\nMountbatten\nVersaPoint Duo\nIndex Basic-D\nET\nPortaThiel Interpoint\nThiel Impacto\nJuliet Pro\n");

LRESULT CEmbossPropDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	
	SetDefaultMemberVariables();
	CenterWindow();
	m_btnOk.Attach(GetDlgItem( IDOK ));
	m_btnCancel.Attach(GetDlgItem( IDCANCEL ));
	m_btnInterpointPages.Attach(GetDlgItem( IDC_CHECK_INTERPOINT_PAGES_E ));
	m_btnInterpointMargins.Attach(GetDlgItem( IDC_CHECK_INTERPOINT_MARGINS_E ));	


//	m_grpCells.Attach(GetDlgItem(  ));
//	m_grpLines.Attach(GetDlgItem(  ));
//	m_staMaxCells.Attach(GetDlgItem(  ));	
//	m_staCells.Attach(GetDlgItem( IDC_STATIC_CELLS_E ));
//	m_staMaxLines.Attach(GetDlgItem(  ));
//	m_staLines.Attach(GetDlgItem( IDC_STATIC_LINES_E ));
//	m_staWhereVal.Attach(GetDlgItem( IDC_STATIC_WHEREVAL ));

	m_edtTopMargin.Attach(GetDlgItem( IDC_EDIT_TOP_MARGIN_E ));
	m_edtLeftMargin.Attach(GetDlgItem( IDC_EDIT_LEFT_MARGIN_E ));
	m_edtMaxCellsPerLine.Attach(GetDlgItem( IDC_EDIT_MAX_CELLS_E ));
	m_edtCellsPerLine.Attach(GetDlgItem( IDC_EDIT_CELLS_E ));
	m_edtMaxLinesPerPage.Attach(GetDlgItem( IDC_EDIT_MAX_LINES_E ));
	m_edtLinesPerPage.Attach(GetDlgItem( IDC_EDIT_LINES_E ));


	// Get last saved Embosser Properties from Registry
	GTUtility::RegDataType type;
	for(int x = BEGIN_EMB_PROP; x < END_EMB_PROP; x++)
	{
		type.dwType = Skipper::_Emb[x].second;
		if(GTUtility::GetRegistryValue(GTUtility::bstrFSEmb,Skipper::_Emb[x].first, type))
		{
			if ( type.dwType == REG_DWORD )
			{
				DwordRegParam(*this, aEmbProp[x]) = type.dw;
			}
			else
			{
				 CStringRegParam(*this, aEmbProp[x])= type.sz;
			}
		}
	}

	m_btnInterpointPages.SetCheck( m_bInterpointPagesValue );
	m_btnInterpointMargins.SetCheck( m_bInterpointMarginsValue );
	
	_TCHAR szOutText[5];	
	_itot_s( m_iTopMarginValue, szOutText, 10 );	
	m_edtTopMargin.SetWindowText( szOutText );

	_itot_s( m_iLeftMarginValue, szOutText, 10 );	
	m_edtLeftMargin.SetWindowText( szOutText );

	_itot_s( m_iCellsPerLineValue, szOutText, 10 );
	m_edtCellsPerLine.SetWindowText( szOutText );

	_itot_s( m_iMaxCellsPerLineValue, szOutText, 10 );	
	m_edtMaxCellsPerLine.SetWindowText( szOutText );

	_itot_s( m_iLinesPerPageValue, szOutText, 10 );
	m_edtLinesPerPage.SetWindowText( szOutText );

	_itot_s( m_iMaxLinesPerPageValue, szOutText, 10 );
	m_edtMaxLinesPerPage.SetWindowText( szOutText );

	InitializeEmbosserCombo();
//	InitializeConnectionCombo();
//	InitializeNetworkDeviceCombo();
    
	AdjustEditFields( m_sEmbosserSel, FALSE );
	m_cbxEmbosser.SetFocus();
	return 0;
}

LRESULT CEmbossPropDlg::OnRadioPages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CEmbossPropDlg::OnFocusConnection(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}

LRESULT CEmbossPropDlg::OnFocusEmbosser(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}
/*
LRESULT CEmbossPropDlg::OnFocusNetworkDevice(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	
	return 0;
}
*/
LRESULT CEmbossPropDlg::OnKillfocusEditMaxCells(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szOutText[5];
	_TCHAR szInText[5];
	m_edtMaxCellsPerLine.GetWindowText( szInText, 5);
	int iText = _ttoi(szInText);
	if ( iText > MAX_CELLS_PER_LINE )
	{
		iText = MAX_CELLS_PER_LINE;
	}
	if ( iText < MIN_CELLS_PER_LINE )
	{
		iText = MIN_CELLS_PER_LINE;
	}
	_itot_s( iText, szOutText, 10 );
	m_edtMaxCellsPerLine.SetWindowText( szOutText);
	m_iMaxCellsPerLineValue = iText;	
	return 0;
}

LRESULT CEmbossPropDlg::OnKillfocusEditMaxLines(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szOutText[5];
	_TCHAR szInText[5];
	m_edtMaxLinesPerPage.GetWindowText( szInText, 5);
	int iText = _ttoi(szInText);
	if ( iText > MAX_LINES_PER_PAGE )
	{
		iText = MAX_LINES_PER_PAGE;
	}
	if ( iText < MIN_LINES_PER_PAGE )
	{
		iText = MIN_LINES_PER_PAGE;
	}
	_itot_s( iText, szOutText, 10 );
	m_edtMaxLinesPerPage.SetWindowText( szOutText);
	m_iMaxLinesPerPageValue = iText;	
	return 0;
}
LRESULT CEmbossPropDlg::OnKillfocusEditCells(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szOutText[5];
	_TCHAR szInText[5];
	m_edtCellsPerLine.GetWindowText( szInText, 5);
	int iText = _ttoi(szInText);
	if ( iText > MAX_CELLS_PER_LINE )
	{
		iText = MAX_CELLS_PER_LINE;
	}
	if ( iText < MIN_CELLS_PER_LINE )
	{
		iText = MIN_CELLS_PER_LINE;
	}
	_itot_s( iText, szOutText, 10 );
	m_edtCellsPerLine.SetWindowText( szOutText);
	m_iCellsPerLineValue = iText;
	return 0;
}

LRESULT CEmbossPropDlg::OnKillfocusEditLines(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szOutText[5];
	_TCHAR szInText[5];
	m_edtLinesPerPage.GetWindowText( szInText, 5 );
	int iText = _ttoi(szInText);
	if ( iText > MAX_LINES_PER_PAGE )
	{
		iText = MAX_LINES_PER_PAGE;
	}
	if ( iText < MIN_LINES_PER_PAGE )
	{
		iText = MIN_LINES_PER_PAGE;
	}
	_itot_s( iText, szOutText, 10 );
	m_edtLinesPerPage.SetWindowText( szOutText);
	m_iLinesPerPageValue = iText;
	return 0;
}

LRESULT CEmbossPropDlg::OnKillfocusEditTopMargin(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szInText[256];
	_TCHAR szOutText[256];
	m_edtTopMargin.GetWindowText( szInText, 256 );
	int iText = _ttoi(szInText);
	if ( iText > MAX_TOP_MARGIN )
	{
		iText = MAX_TOP_MARGIN;
	}
	if ( iText < MIN_TOP_MARGIN )
	{
		iText = MIN_TOP_MARGIN;
	}
	_itot_s( iText, szOutText, 10 );
	m_edtTopMargin.SetWindowText( szOutText);
	m_iTopMarginValue = iText;	
	return 0;
}

LRESULT CEmbossPropDlg::OnKillfocusEditLeftMargin(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szInText[256];
	_TCHAR szOutText[256];
	m_edtLeftMargin.GetWindowText( szInText, 256 );
	int iText = _ttoi(szInText);
	if ( iText > MAX_LEFT_MARGIN )
	{
		iText = MAX_LEFT_MARGIN;
	}
	if ( iText < MIN_LEFT_MARGIN )
	{
		iText = MIN_LEFT_MARGIN;
	}
	_itot_s( iText, szOutText, 10 );
	m_edtLeftMargin.SetWindowText( szOutText);
	m_iLeftMarginValue = iText;	
	return 0;
}
/*
LRESULT CEmbossPropDlg::OnSelectConnection(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CString sText, sTemp;
	int iTypeChosen=m_cbxConnection.GetCurSel();
	m_cbxConnection.GetLBText(iTypeChosen, sText);
	m_sConnectionSel = sText;
	sTemp.LoadString( IDS_EMB_NETWORK_DEVICE_LABEL );

	if (m_sConnectionSel.CompareNoCase( sTemp) == 0 )
		m_edtNetworkPort.ShowWindow( SW_SHOW );
	else
		m_edtNetworkPort.ShowWindow( SW_HIDE );
	return 0;
}
*/
LRESULT CEmbossPropDlg::OnSelectEmbosser(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CString sText;
	_TCHAR szCells[5];
	int iTypeChosen=m_cbxEmbosser.GetCurSel();
	m_cbxEmbosser.GetLBText(iTypeChosen, sText.GetBuffer(MAX_PATH));
	m_sEmbosserSel = sText;
	AdjustEditFields( m_sEmbosserSel );

	_itot_s( m_iCellsPerLineValue, szCells, 10 );
	m_edtCellsPerLine.SetWindowText( szCells );

	_itot_s( m_iLinesPerPageValue, szCells, 10 );
	m_edtLinesPerPage.SetWindowText( szCells );

	_itot_s( m_iMaxCellsPerLineValue, szCells, 10 );
	m_edtMaxCellsPerLine.SetWindowText( szCells );

	_itot_s( m_iMaxLinesPerPageValue, szCells, 10 );
	m_edtMaxLinesPerPage.SetWindowText( szCells );	

	return 0;
}

LRESULT CEmbossPropDlg::OnKillfocusCheckInterpointPages(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	if( m_btnInterpointPages.GetCheck() )
	{
		m_bInterpointPagesValue = TRUE;
	}
	else
	{
		m_bInterpointPagesValue = FALSE;
	}
	return 0;
}

LRESULT CEmbossPropDlg::OnKillfocusCheckInterpointMargins(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	if( m_btnInterpointMargins.GetCheck() )
	{
		m_bInterpointMarginsValue = TRUE;
	}
	else
	{
		m_bInterpointMarginsValue = FALSE;
	}
	return 0;
}

BOOL CEmbossPropDlg::InitializeEmbosserCombo()
{	
	m_cbxEmbosser.Attach(GetDlgItem( IDC_COMBO_EMBOSSER_E ));
//	m_Embosser.SetTitleStrID( IDC_STATIC_EMBOSSER_E );
	m_cbxEmbosser.ResetContent();
	
	CString sItem;
	CString sList = COMBO_EMBOSSER;
	int iLocNewLine; 
	while( 1 ) 
	{
		iLocNewLine = sList.Find(_T("\n"));
		if ( iLocNewLine > 1 )
		{
			sItem = sList.Left( iLocNewLine );
			m_cbxEmbosser.AddString( sItem );
			if ( sList.GetLength() > (iLocNewLine + 1) )
			{
				sList = sList.Mid( iLocNewLine + 1);
				continue;
			}
		}
		break;
	}
	int	iIndex=m_cbxEmbosser.FindStringExact(-1,m_sEmbosserSel);
	if (iIndex<0 || iIndex > MAX_NUMBER_COMBO)
	{
		iIndex=0;
	}
	m_cbxEmbosser.SetCurSel( iIndex );
	return TRUE;
}
/*
BOOL CEmbossPropDlg::InitializeNetworkDeviceCombo()
{	
	m_edtNetworkPort.Attach(GetDlgItem( IDC_PORT_LOC_EDIT  ));
//	m_cbxNetworkDevice.Attach(GetDlgItem( IDC_COMBO_PRINTERNAME  ));
	CString sSel;
	sSel.LoadString( IDS_EMB_NETWORK_DEVICE_LABEL );
	if (m_sConnectionSel.CompareNoCase( sSel ) == 0 )
	{
//		m_cbxNetworkDevice.EnableWindow( TRUE );
		m_edtNetworkPort.ShowWindow( SW_SHOW );
//		m_edtNetworkPort.EnableWindow( TRUE );
	}
	else
	{
//		m_cbxNetworkDevice.EnableWindow( FALSE );
		m_edtNetworkPort.ShowWindow( SW_HIDE );
//		m_edtNetworkPort.EnableWindow( FALSE );
	}
	m_edtNetworkPort.SetWindowText( m_sNetworkPortSel );

	return FALSE;
}

BOOL CEmbossPropDlg::InitializeConnectionCombo()
{
	m_cbxConnection.Attach(GetDlgItem( IDC_COMBO_CONNECTION_E ));
//	m_cbxConnection.SetTitleStrID( IDC_STATIC_CONNECTION_E );

	char szLPTPort[5];
	m_cbxConnection.ResetContent();
	CString sNetDevLabel;
	sNetDevLabel.LoadString(IDS_EMB_NETWORK_DEVICE_LABEL);
	
	m_cbxConnection.AddString( sNetDevLabel );
	for( UINT j=1; j <= NUMBER_OF_LPT_PORTS; j++ )
	{
		wsprintf( ( unsigned short *) szLPTPort, COMBO_EMBOSS_LPT, j);
		m_cbxConnection.AddString( ( unsigned short *) szLPTPort );
	}
// Disable Serial Port Search (we currently do not support serial port embossing )
		// Get The available Com Ports that are on the users computer
	BOOL bPorts[COM_MAX+1]={0};
	if ( GetSerialPorts((LPBOOL)bPorts,sizeof(bPorts))== TRUE )
	{
		for (UINT i=0;i<=COM_MAX;i++)
		{
			if (bPorts[i]==1)
			{
				TCHAR szStr[12]={0};
				// String COM must remain hardcoded
				CString sComPort = COMPORT_STRING;
				sComPort+= _T("%u");
				::_stprintf(szStr, sComPort ,i+1);
				m_cbxConnection.AddString(szStr);
			}
		}
	}
	else 
	{
		char szCOMPort[5];
		for( UINT k=1; k <= NUMBER_OF_COM_PORTS; k++ )
		{
			wsprintf( ( unsigned short *) szCOMPort, COMBO_EMBOSS_COM, k);
			m_cbxConnection.AddString( ( unsigned short *) szCOMPort );
		}
	}
	int	iIndex= m_cbxConnection.FindStringExact(-1,m_sConnectionSel);
	if (iIndex<0 || iIndex > MAX_NUMBER_COMBO)
	{
		iIndex=0;
	}
	m_cbxConnection.SetCurSel( iIndex );

  return TRUE;

}
*/

void CEmbossPropDlg::AdjustEditFields( const CString& sEmbosser, BOOL bMakeChanges)
{
	if( sEmbosser.CompareNoCase( _T("(None)")) == 0)
	{// PageSizeMax = 27,42
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Bax 10")) == 0)
	{// Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Braille Blazer")) == 0)
	{// PageSizeMax = 27,34  PageSizeDes = 25,32  LineWrap 
		m_bEmbosserGoesToNewLine = TRUE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 34;
			m_iLinesPerPageValue = 25;			
			m_iMaxCellsPerLineValue = 34;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Braille Bookmaker")) == 0)
	{// PageSizeMax = 27,44  Driver = Enabling  Interpoint  VPGraphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;			
			m_iMaxCellsPerLineValue = 44;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Braille Express")) == 0)
	{// PageSizeMax = 27,44  Driver = Enabling  Interpoint   VPGraphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;			
			m_iMaxCellsPerLineValue = 44;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Blazer Inferno")) == 0)
	{
		m_bEmbosserGoesToNewLine = TRUE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;			
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	
	if( sEmbosser.CompareNoCase( _T("Braillo 200")) == 0)
	{// Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;			
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	
	if( sEmbosser.CompareNoCase( _T("Braillo 30")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}

	if( sEmbosser.CompareNoCase( _T("Braillo Comet")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}

	if( sEmbosser.CompareNoCase( _T("Elekul")) == 0)
	{// Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("ET")) == 0)
	{// PageSizeMax = 27,40  Driver = Enabling  Interpoint  VPGraphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}	
	if( sEmbosser.CompareNoCase( _T("Everest-D")) == 0)
	{//  Driver = Index  Interpoint  VPGraphics  Index20Graphics  Index16Graphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Index Basic")) == 0)
	{//  Driver = Index  VPGraphics  Index20Graphics  Index16Graphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Index Basic-D")) == 0)
	{//  Driver = Index  Interpoint  VPGraphics  Index20Graphics  Index16Graphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}	

	if( sEmbosser.CompareNoCase( _T("Juliet")) == 0)
	{//  Driver = Enabling  Interpoint   PageSizeMax = 27,42  VPGraphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;	
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Juliet Pro")) == 0)
	{// PageSizeMax = 27,42  Driver = Enabling  Interpoint  VPGraphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("LED-120 @25 Lines")) == 0)
	{// PageSizeMax = 25,40
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 25;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("LED-120 @27 Lines")) == 0)
	{// PageSizeMax = 27,40
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("LED-120 @30 Lines")) == 0)
	{// PageSizeMax = 30,42
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 30;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}

	if( sEmbosser.CompareNoCase( _T("Marathon")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("MBOSS")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Mountbatten")) == 0)
	{// SingleSheet
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}	
	if( sEmbosser.CompareNoCase( _T("Ohtsuki (braille and print)")) == 0)
	{// PageSizeMax = 20,42  PageSizeDes = 20,40  Driver = Ohtsuki  OhtsukiMode  PageWrap
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = TRUE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 20;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 20;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Ohtsuki (braille only)")) == 0)
	{// PageSizeMax = 25,42  Driver = Ohtsuki  PageWrap
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = TRUE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 25;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Paragon")) == 0)
	{// Interpoint  PageSizeMax = 27,40
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}	
	if( sEmbosser.CompareNoCase( _T("PED")) == 0)
	{//  Interpoint   SingleSheet
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}

	if( sEmbosser.CompareNoCase( _T("PortaThiel")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("PortaThiel Interpoint")) == 0)
	{// Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}	
	if( sEmbosser.CompareNoCase( _T("PUMA")) == 0)
	{// CodePageTable = "pumavi.cpt"  Driver = Puma
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("PUMA 6")) == 0)
	{// CodePageTable = "pumavi.cpt"  Driver = Puma
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("RESUS")) == 0)
	{// Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Romeo")) == 0)
	{// PageSizeMax = 27,42
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Romeo 25")) == 0)
	{// PageSizeMax = 27,40
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	
	if( sEmbosser.CompareNoCase( _T("TED")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Thiel")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}

	if( sEmbosser.CompareNoCase( _T("Thiel Impacto")) == 0)
	{// Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}	
	if( sEmbosser.CompareNoCase( _T("Thomas")) == 0)
	{// PageSizeMax = 27,40  Driver = Enabling  Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("VersaPoint")) == 0)
	{// Driver = Versapoint  DataBits7  VPGraphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}	
	
	if( sEmbosser.CompareNoCase( _T("VersaPoint Duo")) == 0)
	{// PageSizeMax = 27,40  Driver = Enabling  Interpoint  VPGraphics
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 40;
			m_iMaxLinesPerPageValue = 27;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}	
	if( sEmbosser.CompareNoCase( _T("Unlisted")) == 0)
	{
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( FALSE );
			m_btnInterpointMargins.SetCheck( FALSE );
		}
		return;
	}
	if( sEmbosser.CompareNoCase( _T("Unlisted Interpoint")) == 0)
	{// Interpoint
		m_bEmbosserGoesToNewLine = FALSE;
		m_bEmbosserGoesToNewPage = FALSE;
		if( bMakeChanges )
		{
			m_iCellsPerLineValue = 40;
			m_iLinesPerPageValue = 25;
			m_iMaxCellsPerLineValue = 42;
			m_iMaxLinesPerPageValue = 26;
			m_btnInterpointPages.SetCheck( TRUE );
			m_btnInterpointMargins.SetCheck( TRUE );
		}
		return;
	}	
	m_bEmbosserGoesToNewLine = FALSE;
	m_bEmbosserGoesToNewPage = FALSE;
	if( bMakeChanges )
	{
		m_iCellsPerLineValue = 40;
		m_iLinesPerPageValue = 25;
		m_iMaxCellsPerLineValue = 42;
		m_iMaxLinesPerPageValue = 27;
		m_btnInterpointPages.SetCheck( FALSE );
		m_btnInterpointMargins.SetCheck( FALSE );
	}
}


/**************************************************************
	Method:			GetSerialPorts
***************************************************************/	
BOOL CEmbossPropDlg::GetSerialPorts(
	/*
		A pointer to a BOOLEAN array, passed to GetSerialPorts by reference.
		This boolean array must be large enough to hold COM_MAX+1 (18) BOOL's.
		When the function returns, if a given COM port exists, the BOOL at the
		appropriate index of the BOOLEAN aray is set to 1.  Otherwise it is
		set to 0.
	*/
	LPBOOL lpbPorts,
	/*
		The size of the BOOLEAN array.  Set this value equal to the number of
		BYTES allocated for the BOOLEAN array pointed to by lpbPorts.  This
		parameter is only used for validation of the lpbPorts parameter.
		(if uiSize is not at least sizeof(BOOL)*(COM_MAX+1) BYTES, the BOOLEAN array
		pointed to by lpbPorts is assumed to be too small and the function returns
		-1 to indicate falure due to invalid parameters.
	*/
	UINT uiSize)
{
	int iVal=0;
	HKEY hkSerialComm(NULL);
	DWORD dwIndex=0;
	TCHAR szValueName[MAX_PATH*2]={0};
	DWORD dwValueNameSize=0;
	TCHAR szValue[MAX_PATH*2]={0};
	DWORD dwValueSize=0; 
	DWORD dwType=0;
	LPCTSTR lpszT=NULL;
	dwType=0;
	dwValueNameSize=sizeof(szValueName);
	dwValueSize=sizeof(szValue);
	if	(
				lpbPorts==NULL
				|| uiSize<sizeof(BOOL)*(COM_MAX+1)
			)
	{
		/*
			Indicate invalid parameters, return -1
		*/
		return -1;
	}
	/*
		Initialize each of the BOOL's stored in the BOOLEAN array 
		pointed too by lpbPorts to be equal to FALSE (0);
	*/
	ZeroMemory((LPVOID)lpbPorts,uiSize);
	/*
		Open the registry key HKLM\Hardware\DeviceMap\SerialComm.  This
		registry key is used in Windows 9x, Windows NT, and Windows 2000
		to store information about which COMM Ports are installed on the 
		system.  To determine which ports are installed and thus populate
		the BOOLEAN array pointed to by lpbPorts, open the key and 
		enumerate the values found in this key.
		Each value is in the form of COM# where # is the number of the installed
		serial port.  Extract the number from the value string and set that index
		of the BOOLEAN array pointed to by lpbPorts equal to TRUE (1) to indicate
		that that COMM port is installed.
	*/
	LPTSTR lpszSubKey= _T("\\HARDWARE\\DEVICEMAP\\SERIALCOMM");
    if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpszSubKey, 0L, /*KEY_EXECUTE|*/KEY_READ, &hkSerialComm) == ERROR_SUCCESS)
	{   

		BOOL bRET = (BOOL)::RegEnumValue(hkSerialComm,dwIndex,szValueName,&dwValueNameSize,
			NULL, &dwType, (LPBYTE)szValue,&dwValueSize);
		while ( bRET == ERROR_SUCCESS)
		{
			lpszT=szValue;
			while (_istdigit(*lpszT)==0 && *lpszT!=_T('\0'))
			{
				lpszT++;
			}
			if (_T('\0')==*lpszT)
			{
				continue;
			}
			iVal=::_ttoi(lpszT)-1;
			if (iVal>=0 && iVal<COM_MAX)
			{
				lpbPorts[iVal]=TRUE;
			}
			dwIndex++;
			dwType=0;
			dwValueNameSize=sizeof(szValueName);
			dwValueSize=sizeof(szValue);
		}
		RegCloseKey (hkSerialComm);
		if (dwIndex>0)
		{
			return TRUE;
		}
	}
	else
	{
		/*
			Indicate failure, return FALSE
		*/
		return FALSE;
	}
	/*
		This point should not be reached.  If it is return -2 to 
		indicate an unknown error.
	*/
	return -1;
}

LRESULT CEmbossPropDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	int iCellsPlusLMarginValue;
	int iCellsPlusTMarginValue;
	m_btnOk.SetFocus();

	_TCHAR szTemp[ _MAX_PATH * 2 ];
	m_cbxEmbosser.GetWindowText( szTemp, _MAX_PATH * 2 );
	m_sEmbosserSel= szTemp;
/*	m_cbxConnection.GetWindowText( szTemp, _MAX_PATH * 2 );
	m_sConnectionSel= szTemp;

	CString sTemp;
	sTemp.LoadString( IDS_EMB_NETWORK_DEVICE_LABEL );
	if (m_sConnectionSel.CompareNoCase( sTemp ) == 0 )
	{
		m_bIsNetworkEmb = TRUE;
	}
	else
	{
		m_bIsNetworkEmb = FALSE;
	}

//dddddd	m_cbxNetworkDevice.GetWindowText( szTemp, _MAX_PATH * 2 );
//dddddd	m_sNetworkDeviceSel= szTemp;
	m_edtNetworkPort.GetWindowText( szTemp, _MAX_PATH * 2 );
	m_sNetworkPortSel= szTemp;
*/
	m_bInterpointPagesValue = m_btnInterpointPages.GetCheck();
	m_bInterpointMarginsValue = m_btnInterpointMargins.GetCheck();

//	if ( m_sNetworkPortSel.IsEmpty() )
//	{
//		GetPrinterInformation( m_sNetworkDeviceSel , m_sNetworkPortSel );
//	}
	CString sMsg;

	_TCHAR szInputText[5];
	m_edtCellsPerLine.GetWindowText( szInputText, 5);
	m_iCellsPerLineValue = _ttoi( szInputText);
    
	m_edtMaxCellsPerLine.GetWindowText( szInputText, 5);
	m_iMaxCellsPerLineValue = _ttoi( szInputText);
	
	m_edtLinesPerPage.GetWindowText( szInputText, 5);
	m_iLinesPerPageValue = _ttoi( szInputText);

	m_edtMaxLinesPerPage.GetWindowText( szInputText, 5);
	m_iMaxLinesPerPageValue = _ttoi( szInputText);

	m_edtTopMargin.GetWindowText( szInputText, 5);
	m_iTopMarginValue = _ttoi( szInputText);
	 
	m_edtLeftMargin.GetWindowText( szInputText, 5);
	m_iLeftMarginValue = _ttoi( szInputText);

	m_iBottomMarginValue = 0;
	m_iRightMarginValue = 0;
    
	// added when decided cells and line number should not include margins from 
	// users perspective.
	iCellsPlusLMarginValue = m_iCellsPerLineValue + m_iLeftMarginValue;
	iCellsPlusTMarginValue = m_iLinesPerPageValue + m_iTopMarginValue;


	if ( (iCellsPlusLMarginValue > m_iMaxCellsPerLineValue ) )
	{
		sMsg.Format( IDS_EMB_CELLS_TOO_HIGH );
		MessageBox(sMsg, _T(""), MB_OK|MB_ICONINFORMATION);
		m_edtCellsPerLine.SetFocus();
		return 0;
	}
	
	if ( (iCellsPlusTMarginValue > m_iMaxLinesPerPageValue ) )
	{
		sMsg.Format( IDS_EMB_LINES_TOO_HIGH );
		MessageBox(sMsg, _T(""), MB_OK|MB_ICONINFORMATION);
		m_edtLinesPerPage.SetFocus();
		return 0;
	}
/*
	if ( (m_iTopMarginValue >= m_iLinesPerPageValue ) )
	{
		sMsg.Format( IDS_EMB_TMARGIN_TOO_HIGH );
		MessageBox(sMsg, _T(""), MB_OK|MB_ICONINFORMATION);
		m_edtTopMargin.SetFocus();
		return 0;
	}


	if ( (m_iLeftMarginValue >= m_iCellsPerLineValue ) )
	{
		sMsg.Format( IDS_EMB_LMARGIN_TOO_HIGH );
		MessageBox(sMsg, _T(""), MB_OK|MB_ICONINFORMATION);
		m_edtLeftMargin.SetFocus();
		return 0;
	}
*/
	if( m_iMaxCellsPerLineValue != iCellsPlusLMarginValue )
	{
		m_bEmbosserGoesToNewLine = FALSE;
	}
	if( m_iMaxLinesPerPageValue != iCellsPlusTMarginValue )
	{
		m_bEmbosserGoesToNewPage = FALSE;
	}
	// Write Embosser Properties to Registry

	GTUtility::RegDataType type;
	for(int i = BEGIN_EMB_PROP; i < END_EMB_PROP; i++)
	{
		type.dwType = Skipper::_Emb[i].second;
		if ( type.dwType == REG_DWORD )
		{
			type.dw = DwordRegParam(*this, aEmbProp[i]);
		}
		else
		{
			_tcscpy_s( type.sz, CStringRegParam( *this, aEmbProp[i]));
		}
		GTUtility::SetRegistryValue(GTUtility::bstrFSEmb, NULL, Skipper::_Emb[i].first, type);
	}

	
	EndDialog(IDOK);
	return 0;
}

LRESULT CEmbossPropDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}

void CEmbossPropDlg::SetDefaultMemberVariables()
{	
	// Default settings for Braille Blazer 
	m_bInterpointMarginsValue = FALSE;
	m_bInterpointPagesValue = FALSE;
	m_iCellsPerLineValue = 34;
	m_iMaxCellsPerLineValue = 34;
	m_iLinesPerPageValue = 25;
	m_iMaxLinesPerPageValue = 27;
  	m_iTopMarginValue = 0;
  	m_iBottomMarginValue = 0;
  	m_iLeftMarginValue = 0;
  	m_iRightMarginValue = 0;

	CString sTemp = COMBO_EMBOSSER;
	int iLoc = sTemp.Find(_T("\n"));
	if ( iLoc > 0 )
	{
		m_sEmbosserSel = _T("Braille Blazer");
//		m_sEmbosserSel = sTemp.Left( iLoc );
	}
	else
	{
		m_sEmbosserSel = _T("Unlisted");
	}
	
//	m_sConnectionSel = COMPORT_STRING;
//	m_sConnectionSel += _T("1");
	
	m_sNetworkDeviceSel = _T("");
//	m_sNetworkPortSel = _T("");

//	m_bIsNetworkEmb = FALSE;
	
	m_bEmbosserGoesToNewLine = FALSE;
	m_bEmbosserGoesToNewPage = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
//
#define COMBO_MAX_NUMBER	20

LRESULT CEmbossConnectDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_sConnectionSel = COMPORT_STRING;
	m_sConnectionSel += _T("1");
	m_sNetworkPortSel = _T("");
	m_bIsNetworkEmb = FALSE;

	GTUtility::RegDataType type;
	for(int i = BEGIN_CONNECT, k = 0; i < END_CONNECT; i++, k++)
	{
		type.dwType = Skipper::_Emb[i].second;
		if(GTUtility::GetRegistryValue(GTUtility::bstrFSEmb,Skipper::_Emb[i].first, type))
		{
			if ( type.dwType == REG_DWORD )
			{
				DwordRegParam(*this, aConnect[k]) = type.dw;
			}
			else
			{
				 CStringRegParam(*this, aConnect[k])= type.sz;
			}
		}
	}

	m_cbxConnection.Attach(GetDlgItem( IDC_COMBO_CONNECTION_E ));

	_TCHAR szLPTPort[5];
	m_cbxConnection.ResetContent();

	CString sNetDevLabel;
	sNetDevLabel.LoadString(IDS_EMB_NETWORK_DEVICE_LABEL);
	m_cbxConnection.AddString( sNetDevLabel );

	CString sIRDALabel = IRDA_STRING;
	m_cbxConnection.AddString( sIRDALabel );

	for( UINT j=1; j <= NUMBER_OF_LPT_PORTS; j++ )
	{
		wsprintf(szLPTPort, COMBO_EMBOSS_LPT, j);
		m_cbxConnection.AddString(szLPTPort);
	}

// Disable Serial Port Search (we currently do not support serial port embossing )
/*		// Get The available Com Ports that are on the users computer
	BOOL bPorts[COM_MAX+1]={0};
	if ( GetSerialPorts((LPBOOL)bPorts,sizeof(bPorts))== TRUE )
	{
		for (UINT i=0;i<=COM_MAX;i++)
		{
			if (bPorts[i]==1)
			{
				TCHAR szStr[12]={0};
				// String COM must remain hardcoded
				CString sComPort = COMPORT_STRING;
				sComPort+= _T("%u");
				::_stprintf(szStr, sComPort ,i+1);
				m_cbxConnection.AddString(szStr);
			}
		}
	}
	else */
	{
		_TCHAR szCOMPort[5];
		for( UINT k=1; k <= NUMBER_OF_COM_PORTS; k++ )
		{
			wsprintf(szCOMPort, COMBO_EMBOSS_COM, k);
			m_cbxConnection.AddString(szCOMPort);
		}
	}
	int	iIndex= m_cbxConnection.FindStringExact(-1,m_sConnectionSel);
	if (iIndex<0 || iIndex > COMBO_MAX_NUMBER)
	{
		iIndex=0;
	}
	m_cbxConnection.SetCurSel( iIndex );

///////////////////////////////////////////////////////////////////////////////

	m_edtNetworkPort.Attach(GetDlgItem( IDC_PORT_LOC_EDIT  ));
//	m_cbxNetworkDevice.Attach(GetDlgItem( IDC_COMBO_PRINTERNAME  ));
	CString sSel;
	sSel.LoadString( IDS_EMB_NETWORK_DEVICE_LABEL );
	if (m_sConnectionSel.CompareNoCase( sSel ) == 0 )
	{
//		m_cbxNetworkDevice.EnableWindow( TRUE );
		m_edtNetworkPort.ShowWindow( SW_SHOW );
//		m_edtNetworkPort.EnableWindow( TRUE );
	}
	else
	{
//		m_cbxNetworkDevice.EnableWindow( FALSE );
		m_edtNetworkPort.ShowWindow( SW_HIDE );
//		m_edtNetworkPort.EnableWindow( FALSE );
	}
	m_edtNetworkPort.SetWindowText( m_sNetworkPortSel );
	m_cbxConnection.SetFocus();
	return 0;
}

LRESULT CEmbossConnectDlg::OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	_TCHAR szTemp[ _MAX_PATH * 2 ];
	m_cbxConnection.GetWindowText( szTemp, _MAX_PATH * 2 );
	m_sConnectionSel= szTemp;

	CString sTemp;
	sTemp.LoadString( IDS_EMB_NETWORK_DEVICE_LABEL );
	if (m_sConnectionSel.CompareNoCase( sTemp ) == 0 )
	{
		m_bIsNetworkEmb = TRUE;
	}
	else
	{
		m_bIsNetworkEmb = FALSE;
	}

//dddddd	m_cbxNetworkDevice.GetWindowText( szTemp, _MAX_PATH * 2 );
//dddddd	m_sNetworkDeviceSel= szTemp;
	m_edtNetworkPort.GetWindowText( szTemp, _MAX_PATH * 2 );
	m_sNetworkPortSel= szTemp;


	GTUtility::RegDataType type;
	for(int i = BEGIN_CONNECT, k = 0; i < END_CONNECT; i++, k++)
	{
		type.dwType = Skipper::_Emb[i].second;
		if ( type.dwType == REG_DWORD )
		{
			type.dw = DwordRegParam(*this, aConnect[k]);
		}
		else
		{
			_tcscpy_s( type.sz, CStringRegParam( *this, aConnect[k]));
		}
		GTUtility::SetRegistryValue(GTUtility::bstrFSEmb, NULL, Skipper::_Emb[i].first, type);
	}

	EndDialog(IDOK);
	return 0;
}

LRESULT CEmbossConnectDlg::OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	EndDialog(IDCANCEL);
	return 0;
}

LRESULT CEmbossConnectDlg::OnSelectConnectionChng(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
{
	CString sText, sTemp;
	int iTypeChosen=m_cbxConnection.GetCurSel();
	m_cbxConnection.GetLBText(iTypeChosen, sText.GetBuffer(MAX_PATH));
	m_sConnectionSel = sText;
	sTemp.LoadString( IDS_EMB_NETWORK_DEVICE_LABEL );
	if (m_sConnectionSel.CompareNoCase( sTemp) == 0 )
	{
//ddddd		m_cbxNetworkDevice.EnableWindow( TRUE );
		m_edtNetworkPort.ShowWindow( SW_SHOW );
	}
	else
	{
//dddddd		m_cbxNetworkDevice.EnableWindow( FALSE );
		m_edtNetworkPort.ShowWindow( SW_HIDE );
	}	

	return 0;
}

BOOL CEmbossConnectDlg::GetSerialPorts(
	/*
		A pointer to a BOOLEAN array, passed to GetSerialPorts by reference.
		This boolean array must be large enough to hold COM_MAX+1 (18) BOOL's.
		When the function returns, if a given COM port exists, the BOOL at the
		appropriate index of the BOOLEAN aray is set to 1.  Otherwise it is
		set to 0.
	*/
	LPBOOL lpbPorts,
	/*
		The size of the BOOLEAN array.  Set this value equal to the number of
		BYTES allocated for the BOOLEAN array pointed to by lpbPorts.  This
		parameter is only used for validation of the lpbPorts parameter.
		(if uiSize is not at least sizeof(BOOL)*(COM_MAX+1) BYTES, the BOOLEAN array
		pointed to by lpbPorts is assumed to be too small and the function returns
		-1 to indicate falure due to invalid parameters.
	*/
	UINT uiSize)
{
	int iVal=0;
	HKEY hkSerialComm(NULL);
	DWORD dwIndex=0;
	TCHAR szValueName[MAX_PATH*2]={0};
	DWORD dwValueNameSize=0;
	TCHAR szValue[MAX_PATH*2]={0};
	DWORD dwValueSize=0; 
	DWORD dwType=0;
	LPCTSTR lpszT=NULL;
	dwType=0;
	dwValueNameSize=sizeof(szValueName);
	dwValueSize=sizeof(szValue);
	if	(
				lpbPorts==NULL
				|| uiSize<sizeof(BOOL)*(COM_MAX+1)
			)
	{
		/*
			Indicate invalid parameters, return -1
		*/
		return -1;
	}
	/*
		Initialize each of the BOOL's stored in the BOOLEAN array 
		pointed too by lpbPorts to be equal to FALSE (0);
	*/
	ZeroMemory((LPVOID)lpbPorts,uiSize);
	/*
		Open the registry key HKLM\Hardware\DeviceMap\SerialComm.  This
		registry key is used in Windows 9x, Windows NT, and Windows 2000
		to store information about which COMM Ports are installed on the 
		system.  To determine which ports are installed and thus populate
		the BOOLEAN array pointed to by lpbPorts, open the key and 
		enumerate the values found in this key.
		Each value is in the form of COM# where # is the number of the installed
		serial port.  Extract the number from the value string and set that index
		of the BOOLEAN array pointed to by lpbPorts equal to TRUE (1) to indicate
		that that COMM port is installed.
	*/
	LPTSTR lpszSubKey= _T("\\HARDWARE\\DEVICEMAP\\SERIALCOMM");
    if( ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, lpszSubKey, 0L, /*KEY_EXECUTE|*/KEY_READ, &hkSerialComm) == ERROR_SUCCESS)
	{   

		BOOL bRET = (BOOL)::RegEnumValue(hkSerialComm,dwIndex,szValueName,&dwValueNameSize,
			NULL, &dwType, (LPBYTE)szValue,&dwValueSize);
		while ( bRET == ERROR_SUCCESS)
		{
			lpszT=szValue;
			while (_istdigit(*lpszT)==0 && *lpszT!=_T('\0'))
			{
				lpszT++;
			}
			if (_T('\0')==*lpszT)
			{
				continue;
			}
			iVal=::_ttoi(lpszT)-1;
			if (iVal>=0 && iVal<COM_MAX)
			{
				lpbPorts[iVal]=TRUE;
			}
			dwIndex++;
			dwType=0;
			dwValueNameSize=sizeof(szValueName);
			dwValueSize=sizeof(szValue);
		}
		RegCloseKey (hkSerialComm);
		if (dwIndex>0)
		{
			return TRUE;
		}
	}
	else
	{
		/*
			Indicate failure, return FALSE
		*/
		return FALSE;
	}
	/*
		This point should not be reached.  If it is return -2 to 
		indicate an unknown error.
	*/
	return -1;
}