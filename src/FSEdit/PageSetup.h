#ifndef __PAGESETUP_H__12899588693RPM_170847__
#define __PAGESETUP_H__12899588693RPM_170847__

#pragma once

const TCHAR Number [] = {_T('0'),_T('1'),_T('2'),_T('3'),_T('4'),_T('5'),_T('6'),_T('7'),_T('8'),_T('9'),_T('.'), _T(' ')};

template<class T>
T comp(LONG eval, T meas)
{
	return (T)eval/meas;
}

const size_t Offset [] =
{
	offsetof(DocProp, pageHeight),
	offsetof(DocProp, pageWidth),
	offsetof(DocProp, topMargin),
	offsetof(DocProp, bottomMargin),
	offsetof(DocProp, leftMargin),
	offsetof(DocProp, rightMargin)
};

#define PO	IDEDT_PAGE_HEIGHT



// Gets the string data from the edit box and converts it to
// numerical data.  This function may look ackward, but due to 
// the Microsoft implementation of templates, this is the best
// at the pressent time to solve this particular problem.
template<class T>
T GetEditData(UINT id, HWND HWnd, bool bBraille = true)
{
	CComBSTR bstr;
	CEdit edt(::GetDlgItem(HWnd, id));
	edt.GetWindowText(bstr.m_str);

	CString str(bstr.m_str);
	str.TrimLeft();
	int nLen = str.GetLength();
	int nIndex = 0;
	while(nIndex < nLen && (find(Number, Number+11, str[nIndex]) != Number+11))
		nIndex++;
		
	if(nIndex == 0 || (nIndex < nLen && str[nIndex] !=_T(' ')))
		return T(bBraille ? -1 : -1.0);
	
	str = str.Left(nIndex);
	if(bBraille)
		return T(_ttol(str));
	else
		return T(_ttof(str));
}


class CPageSetupDlg : public CDialogImpl<CPageSetupDlg>
{
	bool m_bBraille;
	bool m_bChanged;
	
	CString m_strMeasure1;
	CString m_strMeasure1A;

	CString m_strMeasure2;
	CString m_strMeasure2A;
	CString m_strFormat;

	DocProp& m_Prop;
	UINT m_nPageNum;
	long lReset;
	double dReset;
	bool bInvalid;
	WORD ID;
	
	HWND SetBuddy(HWND hWnd, HWND hWndBuddy){return (HWND)::SendMessage(hWnd, UDM_SETBUDDY, (WPARAM)hWndBuddy, 0L);}
	HWND GetBuddy(HWND hWnd){return (HWND)::SendMessage(hWnd, UDM_GETBUDDY, 0, 0L);}
	void SetRange(HWND hWnd, int nLower, int nUpper){::SendMessage(hWnd, UDM_SETRANGE, 0, MAKELPARAM(nUpper, nLower));}

public:
	CPageSetupDlg(bool bBraille, DocProp& Prop) : m_bBraille(bBraille),
									m_Prop(Prop), m_bChanged(false)
	{
		if(bBraille)
		{
			m_strFormat.LoadString(IDS_BRAILLE_FORMAT);
			m_strMeasure1.LoadString(IDS_BRAILLE_CELL);
			m_strMeasure1A.LoadString(IDS_BRAILLE_CELLS);
			m_strMeasure2.LoadString(IDS_BRAILLE_LPP);
			m_strMeasure2A.LoadString(IDS_BRAILLE_LSPP);
		}
		else
		{
			m_strFormat.LoadString(IDS_INCHES_FORMAT);
			m_strMeasure1.LoadString(IDS_NO_BRAILLE_INCHE);
			m_strMeasure1A.LoadString(IDS_NO_BRAILLE_INCHES);
		}

		bInvalid = false;
		ID = 0;

	}

	enum {IDD = IDD_PAGE_SETUP};

	BEGIN_MSG_MAP(CPageSetupDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_RANGE_HANDLER(IDEDT_PAGE_HEIGHT, IDEDT_MARGIN_RIGHT, OnEditCtrl)
		COMMAND_RANGE_HANDLER(IDRAD_NONE, IDRAD_ROMAN, OnRadioCtrl)
	END_MSG_MAP()

	LRESULT OnRadioCtrl(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(wNotifyCode == BN_CLICKED)
			m_nPageNum = (wID - IDRAD_NONE);

		return 0;
	}

	template<class T>
	bool IsInvalidValue(T& value, T& reset)
	{
		if(value < 0.0)
		{
			value = reset;
			return true;
		}

		return false;
	}

	LRESULT OnEditCtrl(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(wNotifyCode == EN_KILLFOCUS && m_bChanged)
		{// When the Edit box lose focus Check validity and Reset String.
			CString str;
			
			if(m_bBraille)
			{
				long lCols = GetEditData<long>(wID, m_hWnd, true);
				bInvalid = IsInvalidValue(lCols, lReset);
				if(bInvalid)
					ID = wID;
				if(wID == IDEDT_PAGE_HEIGHT)
					str.Format(m_strFormat, lCols, (lCols == 1) ? m_strMeasure2.GetBuffer() : m_strMeasure2A.GetBuffer());
				else if(wID == IDEDT_PAGE_WIDTH)
					str.Format(m_strFormat, lCols, (lCols == 1) ? m_strMeasure1.GetBuffer() : m_strMeasure1A.GetBuffer());
				else
					str.Format(m_strFormat, lCols, _T(""));
			}
			else
			{
				double dInches = GetEditData<double>(wID, m_hWnd, false);
				bInvalid = IsInvalidValue(dInches, dReset);
				str.Format(m_strFormat, dInches, (dInches == 1.0) ? m_strMeasure1.GetBuffer() : m_strMeasure1A.GetBuffer());
			}
			
			::SetWindowText(GetDlgItem(wID), str);
			m_bChanged = false;
		}
		else if(wNotifyCode == EN_SETFOCUS)
		{// Get the current value for resetting.
			if(bInvalid)
			{
				bInvalid = false;
				CString str;
				str.LoadString(IDS_INVALID_VALUE_ENTERED);
				MessageBox(str, _T(""), MB_OK|MB_ICONINFORMATION);

				CEdit eSet(GetDlgItem(wID));
				eSet.SetSel(-1, -1);

				CEdit edt(GetDlgItem(ID));
				edt.SetSel(0, -1);
				edt.SetFocus();
				return 0;
			}

			if(m_bBraille)
				lReset = GetEditData<long>(wID, m_hWnd, true);
			else
				dReset = GetEditData<double>(wID, m_hWnd, false);

		}
		else if(wNotifyCode == EN_CHANGE)
			m_bChanged = true;

		return 0;
	}

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CenterWindow();

		SetPropResult(IDEDT_PAGE_HEIGHT, 0, 99);
		SetPropResult(IDEDT_PAGE_WIDTH, 0, 99);
		SetPropResult(IDEDT_MARGIN_TOP, 0, 99);
		SetPropResult(IDEDT_MARGIN_BOTTOM, 0, 99);
		SetPropResult(IDEDT_MARGIN_LEFT, 0, 99);
		SetPropResult(IDEDT_MARGIN_RIGHT, 0, 99);
		
		CString str;
		str.LoadString((m_bBraille ? IDS_MEASURE_IS_BRAILLE : IDS_MEASURE_IS_INCHES));
		::SetWindowText(GetDlgItem(IDSTA_MESUREMENTS), str);

		m_nPageNum = (m_Prop.pageNumbering > 2) ? 0 : m_Prop.pageNumbering;
		CheckDlgButton(IDRAD_NONE + m_nPageNum, BST_CHECKED);

		CEdit edt(GetDlgItem(IDEDT_PAGE_HEIGHT));
		edt.SetFocus();
		edt.SetSel(0, -1);
		return DefWindowProc();
	}

	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		EndDialog(IDCANCEL);
		return 0;
	}

	bool IsPropInvalid(UINT uID, long& lResult)
	{
		lResult = GetPropResult(uID);
		if(-1 == lResult)
		{
			return true;
		}

		return false;
	}
	LRESULT OnOK(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled)
	{
		DocProp p;
		memset(&p, 0, sizeof(DocProp));
		p.pageNumbering = m_nPageNum;
		// Getting, checking and resetting edit boxes if the values are
		// invalid.
		for(UINT i = IDEDT_PAGE_HEIGHT; i <= IDEDT_MARGIN_RIGHT; i++)
		{
			long lResult;
			if(IsPropInvalid(i, lResult)) // Check to validate data.
			{
				// Since tabbing will change focus and data is validated
				// we simply set some values and change focus.
				ID = static_cast<WORD>(i);
				bInvalid = true;
				// we are just getting some other edit box.
				CEdit edt(GetDlgItem((i!=IDEDT_MARGIN_RIGHT? i+1:IDEDT_PAGE_HEIGHT)));
				edt.SetFocus();
				return 0;
			}

			Param(p, Offset[i-IDEDT_PAGE_HEIGHT]) = lResult;
		}

		// Everything went alright.  Copy data to DocProp reference.
		memcpy(&m_Prop.pageHeight, &p.pageHeight,
			offsetof(DocProp, rightMargin) - offsetof(DocProp, brailleGrade2));
		EndDialog(IDOK);
		return 0;
	}

	// Some Helper Functions
	void SetPropResult(UINT uECtrl, int nLowRange, int nHiRange)
	{

		CString str;
		if(m_bBraille)
		{
			lReset = comp(Param(m_Prop, Offset[uECtrl-PO]), 160L);
			if(IDEDT_PAGE_HEIGHT == uECtrl)
				str.Format(m_strFormat, lReset, (lReset == 1) ? m_strMeasure2.GetBuffer() : m_strMeasure2A.GetBuffer());
			else if(IDEDT_PAGE_WIDTH == uECtrl)
				str.Format(m_strFormat, lReset, (lReset == 1) ? m_strMeasure1.GetBuffer() : m_strMeasure1A.GetBuffer());
			else
				str.Format(m_strFormat, lReset, _T(""));

		}
		else
		{
			dReset = comp(Param(m_Prop, Offset[uECtrl-PO]), 1440.0);
			str.Format(m_strFormat, dReset, (dReset == 1.0) ? m_strMeasure1.GetBuffer() : m_strMeasure1A.GetBuffer());
		}

		::SetWindowText(GetDlgItem(uECtrl), str);
	}

	long GetPropResult(UINT id)
	{
		if(m_bBraille)
		{
			long lResult = GetEditData<long>(id, m_hWnd, true);
			return ((lResult < 0) ? -1 : (lResult*160));
		}
		else
		{
			double dResult = GetEditData<double>(id, m_hWnd, false);
			return ((dResult < 0.0) ? -1 : (long)(dResult*1440.0));
		}
	}
};

#endif
