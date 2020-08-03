// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#pragma warning (disable:4018)

#include "splchck.h"
#include "atlextra.h"
#include <Winnls.h>
#include <functional>
#include <list>
#include <string>
#include <map>
#include "FSFindReplaceDlg.h"

///////////////////////////////////////////////////////////////////////////
class CBrfWarningDlg : public CDialogImpl<CBrfWarningDlg>
{
public:
	CBrfWarningDlg(CString strFile) : m_strFile(strFile) {}

	enum {IDD = IDD_BRFWARNING_DLG};

	BEGIN_MSG_MAP(CBrfWarningDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDOK, OnContinue)
	END_MSG_MAP()


	static BOOL ShowConvertDlg();
	static BOOL IsBRF(CString strSaveAs, CString strCurrent);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCancel(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);
	LRESULT OnContinue(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandled);

private:

	static BOOL RegistryValue(BOOL bSet);

	CString m_strFile;
	BOOL m_bShowAgain;
};

#define USER_DICT1_ADD	0

typedef basic_string<TCHAR> tstring;

inline int SMulDiv(int nNumber, int nNumerator, int nDenominator)
{
	__int64 x;

	x = (__int64)nNumber * (__int64)nNumerator;
	x /= (__int64)nDenominator;

	return (int)x;
}

// Twips macros
#define	ONEINCH		1440
#define HALFINCH	 720
#define TWENTITHINCH  72
#define DEFAULTINDENT HALFINCH
#define CHARCOLUMN	160

const int ArrowKeys [] =
{
	VK_LEFT,
	VK_UP,
	VK_RIGHT,
	VK_DOWN
};

typedef vector<pair<UINT, pair<UINT, UINT> > > VEC_UIPARAM;

#define MAX_NMRU_FILES 6


enum CtrlNavigatorCode { _navigByChar=0, _navigByWord, _navigByLine, _navigByMark, _navigUnknown, };
class CtrlNavigator // just a little helper class to encapsulate moving around
{
  public:
	CtrlNavigator(CFSEditView *_tgt);
	int move(CtrlNavigatorCode byCode, int n);
	int moveByChar(int n); // move n chars left or right; returns 0=OK, else err code
	int moveByWord(int n); // idem
	int moveByLine(int n); // idem
	int moveByMark(int n); // idem

  private:
	CFSEditView *tgt;  // the control target of the navigation commands
    HWND jwnd; // Jaws Windows id
	static int jawsMsgId;

	// testing:
	int _move(LPCTSTR obj, int n);

};
	
enum _GotoStates { _GettingSign, _GettingDigits } ; //, _GettingObject };
class GotoStateTracker // just a little helper class to encapsulate Goto handling
{
  public:
	GotoStateTracker() : st(_GettingSign), dir(0), count(0), object(_navigByChar) { }
	void reset() { st = _GettingSign; dir = 0; count = 0; object = _navigByChar; userInput="";}
	int parseCharacter(int c); // returns 0=ok, keep going, 1=done, -1=error
	CtrlNavigatorCode getMoveInfo(int &n); // should be called after parse returns 1

    
	CString userInput;   // just for debugging....
  private:
	CtrlNavigatorCode getObject(int c);
	//unsigned short _toUpper(unsigned short c); 

	// DATA
	_GotoStates st; // state
	int dir; // 1 = up, 0 = down 
	int count;
	CtrlNavigatorCode object;

};


class CMainFrame : public CFrameWindowImpl<CMainFrame>,// public CUpdateUI<CMainFrame>,
                   public CMessageFilter, public CIdleHandler
{
	CString m_strFile;
	CString m_strDir;
	CString m_strFindReplace;

	CFSEditView m_view;
	CSpellChecker m_spl;
public:
	DWORD m_dwDateTimeFormat;
//	DWORD m_dwParaNum;
	DWORD m_dwSPDivider;
	DWORD m_dwWordWrap;
	DWORD m_dwAutosave;
    LRESULT processGotoMoveCharacter(int c);
private:
	WORD m_wFileExt;
	bool m_bCreate;
	DWORD m_dwFlags;
	bool m_bInitialName;
	bool m_bSPOpened;
	int m_instancesOpenedCount;
	bool m_bFileExplorer;

	BOOL m_bVisible;
//	HANDLE m_hMutex;
	HANDLE m_hPrgSemaphore;
	
	DWORD m_dwPrintWidth;
	DWORD m_dwNumOfCopies;

	bool CheckToSave(bool bOpen = false, bool bRecent = false, bool bExit = false, bool bActivate = false);
	
	template<class T, class S>
	bool GetFileName(T& strFile, S& strInsertAs)
	{
		CShellFileOpenDialog dlg;
		if (dlg.DoModal() == IDCANCEL)
			return false;
		dlg.GetFilePath(strFile);
		//strInsertAs = dlg.GetInsertAsString();
		return true;	
	}

	bool GetNewFileName(LPTSTR lpNewFile);


	GotoStateTracker _gtt;
    //LRESULT startGotoMove() { _gtt.reset(); }


	//const char *_instanceId;  // eiter "COPY1" or "COPY2"
	//const int m_nNumRecentFiles;
	bool bfound;
	//list<CString> lstMRUList;
    //CString mruList[MAX_NMRU_FILES];
	CString strRegPath;

	void AddToMRUList(const CString &fpath)
	{
		// needs work
	}
	void RemoveFromMRUList(const CString &fpath)
	{
		// needs work
	}
	CString GetFile(int idx) 
	{ 
		// needs work
		return L"";
	}
	CtrlNavigator *navgtor; //(&m_view);


	void UpdateMRUList() {
		// needs work
	}
	void GetMRUList()
	{
		// needs work
	}
	void SetNumberOfRecentFiles(int nNum)
	{
		// needs work
	}
	void SetRegPath(CString str){strRegPath = str;}
	
	void EnumMenuItems(HMENU hMenu);
	CFSFindReplaceDlg* m_pFDlg;
	
	void LaunchApp(ENLINK* pENLink);
	void AppProperties(ENLINK* pENLink);
	void GetDirectoryFromPath();
	UINT SkipperViewTwips();
	void FontFormat(const pair<DWORD, UINT>& pr);

	bool SPAlreadyOpened();
	int AppAlreadyOpened(bool bSPOpening = false);

	VEC_UIPARAM vecUIParams;

	CComBSTR strCmdLine;
	int nLenSPad;
	static LPCTSTR m_pszExeMsg;
	static LPCTSTR m_pszPrgMsg;

	bool m_bCanRevert;
	bool m_bAutoDetectLink;
	BOOL m_bNewFile;

	void RegistryValues(bool bSet = true);
	void GetPageSetup(HDC& hdc, RECT& rPage, RECT& rPrint);
public:

	void SetCmdLine(CComBSTR bstr){strCmdLine = bstr;}

	UINT PasteUpdate(int nItem = -1);
	UINT MarkUpdate(int nItem = -1);
	UINT CutCopyUpdate(int nItem = -1);
	UINT FontUpdate(int nItem = -1);
	UINT ParaUpdate(int nItem = -1);
	UINT IndentUpdate(int nItem = -1);
	UINT StatusBarUpdate(int nItem = -1);
	UINT RevertToSaveUpdate(int nItem = -1);
	UINT BulletNumberingUpdate(int nItem = -1);
	UINT AutoDetectLinkUpdate(int nItem = -1);
	UINT FindNextUpdate(int nItem = -1);
	UINT TimeStampUpdate(int nItem = -1);
	UINT UndoUpdate(int nItem = -1);
	UINT PrintUpdate(int nItem = -1);
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MENUBAR)
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle(){return FALSE;}
	
	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
		MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
		MESSAGE_HANDLER(CFSFindReplaceDlg::GetFindReplaceMsg(), OnFindReplace)
		MESSAGE_HANDLER(WM_INITMENUPOPUP, OnInitMenuPopup)
		MESSAGE_HANDLER(WM_HELP, DoHelp)
		MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus);
		MESSAGE_HANDLER(WM_SIZE, OnSize);
		MESSAGE_HANDLER(WM_COPYDATA, OnCopyData);
		MESSAGE_HANDLER(WM_UPDATE_TITLE, OnUpdateFileTitle)
		NOTIFY_CODE_HANDLER(EN_LINK, OnNotifyLink)
		COMMAND_ID_HANDLER(ID_CONTEXTMENU, OnContextMenu)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnFileOpen)
		COMMAND_ID_HANDLER(ID_FILE_SAVE, OnFileSave)
		COMMAND_ID_HANDLER(ID_FILE_SAVE_AS, OnFileSaveAs)
		COMMAND_ID_HANDLER(ID_EDIT_COPY, OnCopy)
		COMMAND_ID_HANDLER(ID_EDIT_CUT, OnCut)
		COMMAND_ID_HANDLER(ID_EDIT_FIND, OnFind)
		COMMAND_ID_HANDLER(ID_EDIT_FIND_NEXT, OnFindNext)
		COMMAND_ID_HANDLER(ID_EDIT_REPLACE, OnFind)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
//		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDM_TOOLS_SPELLCHECKER, OnSpellChecker)
		COMMAND_ID_HANDLER(IDM_TOOLS_SPELL_EDIT, OnSpellEdit)
		COMMAND_ID_HANDLER(IDM_TOOLS_INSERT_FILE, OnFileInsert)
		COMMAND_ID_HANDLER(IDR_FORMAT_PAGE_SETUP, OnPageSetup)
		COMMAND_ID_HANDLER(IDM_TOOLS_WORD_COUNT, OnWordCount)
		COMMAND_ID_HANDLER(IDM_FILE_REVERT_TO_SAVE, OnRevertToSave)
		COMMAND_ID_HANDLER(IDM_TOOLS_OPTIONS, OnOptions)
		COMMAND_ID_HANDLER(IDM_INSERT_DATE_TIME, OnDateTimeStamp)
//		COMMAND_ID_HANDLER(ID_INSERT_LINK, OnHyperLink)
//		COMMAND_ID_HANDLER(ID_INSERT_AUTODETECT, OnAutoDetectLink)
		COMMAND_ID_HANDLER(IDM_INSERT_BULLET_NUMBER, OnBulletNumbering)
		COMMAND_ID_HANDLER(IDM_FILE_PRINT, OnPrint)
		COMMAND_RANGE_HANDLER(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnRecentFile)
		COMMAND_RANGE_HANDLER(ID_FONT_BOLD, ID_FONT_MORE, OnFont)
		COMMAND_RANGE_HANDLER(ID_PARA_ALIGNLEFT, ID_PARA_JUSTIFY, OnJustify)
		COMMAND_RANGE_HANDLER(ID_PARA_INCREASEINDENT, ID_PARA_DECREASEINDENT, OnIndent)

		COMMAND_ID_HANDLER(ID_GOTO_QUERY, OnGOTOQuery)
		//COMMAND_ID_HANDLER(ID_MYNEW_ACC, OnGOTOQuery)
		COMMAND_ID_HANDLER(ID_GOTO_QUERY2, OnGOTOQuery2)
		COMMAND_ID_HANDLER(ID_GOTO_MOVE, OnGOTOMove)
		COMMAND_ID_HANDLER(ID_SETPARAGRAPH_DOUBLESPACE, OnSetDoubleSpace)

		


#ifdef BOOKMARK_SUPPORT
		COMMAND_RANGE_HANDLER(ID_BM_SETBOOKMARK, ID_BM_MOVE_BOOKMARK_PREV, OnBookMark)
#endif
//		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		CHAIN_COMMANDS_ALT_MEMBER((m_view), 0)
	END_MSG_MAP()
		
	HWND CreateExtend(HWND hWndParent = NULL, _U_RECT rect = NULL, DWORD dwStyle = 0, DWORD dwExStyle = 0, LPVOID lpCreateParam = NULL);

	LRESULT OnHyperLink(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAutoDetectLink(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnNotifyLink(int wID, LPNMHDR phdr, BOOL& bHandled);

	LRESULT OnInitMenuPopup(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT DoHelp(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnJustify(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnIndent(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFont(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFind(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFindNext(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPageSetup(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFindReplace(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnWordCount(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBookMark(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRevertToSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOptions(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnDateTimeStamp(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBulletNumbering(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPrint(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnRecentFile(WORD wNotifyCode, WORD wID, HWND hWndCtrl, BOOL& bHandle);
	LRESULT OnSpellChecker(WORD wLParam, WORD wHPara, HWND /*lParam*/, BOOL& bHandled);
	LRESULT OnSpellEdit(WORD, WORD, HWND, BOOL&);
	LRESULT OnFileInsert(WORD wLParam, WORD wHPara, HWND /*lParam*/, BOOL& bHandled);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnActivate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileSaveAs(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnContextMenu(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCopyData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUpdateFileTitle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCopy(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled);
	LRESULT OnCut(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled);

    LRESULT OnGOTOQuery(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnGOTOQuery2(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnGOTOMove(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSetDoubleSpace(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:

	// New paragraph formatting stuff
    void SetToSingleSpacing(PARAFORMAT2 &originalPf);
	void SetToDoubleSpacing(PARAFORMAT2 &originalPf);
	bool usingDouble;
    void RestorePreviousFormatting(const PARAFORMAT2 &originalPf);

	void SetParaNum();
	void SetWordWrap();
	void SetTitleBar(LPCTSTR szFile);
	LRESULT NotifyBrailleGradeChng();
	bool m_bBulletNum;

	void ProcessPrintDlgError(DWORD dwError);
	void LoadFileTypeFilter(LPTSTR lpszFileFilter, bool bLoadBRL = true);
	void BuildFilePath(CString& path);

	typedef map<CString, pair<int, int> > MAPFILE;	
	MAPFILE mapFiles;

	void RecordFile(const CString& strMapFile);
	CHARRANGE GetRecordFile(const CString& strMapFile);
};
//template<class T, class S>
//bool CMainFrame::GetFileName(T& strFile, S& strInsertAs)

// Array for Menu items to be updated.  
// 0 = EnableMenuItem
// 1 = CheckMenuItem
// Second number is for the function pointer in UIFun.
// If you add addition entries, update the NUM_UIPARAMS
// macro located just below this array to the correct
// number of entries.
const pair<UINT, pair<UINT, UINT> > UIParams [] =
{
	make_pair(ID_EDIT_COPY, make_pair(0, 0)),						// 1
	make_pair(ID_EDIT_CUT, make_pair(0, 0)),
	make_pair(ID_EDIT_PASTE, make_pair(0, 1)),
	make_pair(ID_EDIT_CLEARMARK, make_pair(0, 2)),
	make_pair(ID_EDIT_CLEAR, make_pair(0, 0)),
	make_pair(ID_FONT_BOLD, make_pair(1, 3)),
	make_pair(ID_FONT_ITALIC, make_pair(1, 3)),
	make_pair(ID_FONT_UNDERLINE, make_pair(1, 3)),
	make_pair(ID_FONT_STRIKETHROUGH, make_pair(1, 3)),
	make_pair(ID_PARA_ALIGNLEFT, make_pair(1, 4)),					// 10
	make_pair(ID_PARA_ALIGNCENTER, make_pair(1, 4)),
	make_pair(ID_PARA_ALIGNRIGHT, make_pair(1, 4)),
	make_pair(ID_PARA_JUSTIFY, make_pair(1, 4)),
	make_pair(ID_PARA_INCREASEINDENT, make_pair(0, 5)),
	make_pair(ID_PARA_DECREASEINDENT, make_pair(0, 5)),
	make_pair(ID_VIEW_STATUS_BAR, make_pair(1, 6)),
	make_pair(IDM_FILE_REVERT_TO_SAVE, make_pair(0, 7)),
	make_pair(IDM_INSERT_BULLET_NUMBER, make_pair(2, 8)),
	make_pair(ID_EDIT_FIND_NEXT, make_pair(0, 9)),
	make_pair(IDM_INSERT_DATE_TIME, make_pair(0, 10)),
	make_pair(ID_EDIT_UNDO, make_pair(0, 11)),
	make_pair(IDM_FILE_PRINT, make_pair(0, 12))
};

#define NUM_UIPARAMS	ArraySize<sizeof(UIParams),sizeof(pair<UINT, pair<UINT, UINT> >)>::Val


typedef UINT (CMainFrame::*pfun)(int);
const pfun UIFun [] =
{
	&CMainFrame::CutCopyUpdate,
	&CMainFrame::PasteUpdate,
	&CMainFrame::MarkUpdate,
	&CMainFrame::FontUpdate,
	&CMainFrame::ParaUpdate,
	&CMainFrame::IndentUpdate,
	&CMainFrame::StatusBarUpdate,
	&CMainFrame::RevertToSaveUpdate,
	&CMainFrame::BulletNumberingUpdate,
	&CMainFrame::FindNextUpdate,
	&CMainFrame::TimeStampUpdate,
	&CMainFrame::UndoUpdate,
	&CMainFrame::PrintUpdate
};

#ifdef BOOKMARK_SUPPORT
typedef bool (CFSEditView::*pBook)();
const pBook BMFun [] =
{
	&CFSEditView::PutBookMark,
	&CFSEditView::DeleteBookMark,
	&CFSEditView::GotoNextBookMark,
	&CFSEditView::GotoPrevBookMark
};
#endif
const DWORD aOptAttr[] =
{
	offsetof(CMainFrame, m_dwDateTimeFormat),
//	offsetof(CMainFrame, m_dwParaNum),
	offsetof(CMainFrame, m_dwSPDivider),
	offsetof(CMainFrame, m_dwWordWrap),
	offsetof(CMainFrame, m_dwAutosave)
};

#define NUM_OPTATTR	ArraySize<sizeof(aOptAttr),sizeof(DWORD)>::Val





/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

