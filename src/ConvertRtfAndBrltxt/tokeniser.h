/*Copyright 2001
 *
 * tokeniser.h
 *
 * Author: Gregory Ford greg@reddfish.co.nz
 * RTF token parser based on:
 *      rtf2html by Dmitry Porapov <dpotapov@capitalsoft.com>,
 *      based on earlier work of Chuck Shotton.
 *      distributed under BSD-style license
 * RTF token lists and hashing algorithm based on
 *      RTF Tools, Release 1.10 
 *      6 April 1994	
 *      Paul DuBois	dubois@primate.wisc.edu 
 *
 * Copying permitted under GNU licence (see COPYING)
 */ 
// tokeniser.h: interface for the Tokeniser class.
//
//////////////////////////////////////////////////////////////////////

#define MAX_RTF_TOKEN 40

class Tokeniser  
{
public:
	Tokeniser( CharSource *source );
	bool isFinished();
	bool finished;
	CharSource *charSource;
	void setCharSource( CharSource *source );
	Token *handleDestination();
	Token *backslash();
	Token *buildControlToken(char ch, bool isDestination);
	Token *nextToken();
	Tokeniser();
	virtual ~Tokeniser();

protected:


	static int hash (char *s);
	static void lookup (char *s, ControlToken *token);
	static void lookupInit ();

	int chartoi( char ch );
	int buildArg( char ch );
};


# define	rtfUnknown	0
# define	rtfGroup	1
# define	rtfText		2
# define	rtfControl	3
# define	rtfEOF		4
# define	rtfMaxClass	5	/* highest class + 1 */

/*
 * Group class major numbers
 */

# define	rtfBeginGroup	0
# define	rtfEndGroup	1

/*
 * Control class major and minor numbers.
 */

//# define	rtfVersion	0

# define	rtfDefFont	1

# define	rtfCharSet	2
# define		rtfAnsiCharSet		0
# define		rtfMacCharSet		1
# define		rtfPcCharSet		2
# define		rtfPcaCharSet		3


/* destination minor numbers should be zero-based and sequential */

# define	rtfDestination	3
# define		rtfFontTbl		0
# define		rtfFontAltName		1	/* new in 1.10 */
# define		rtfEmbeddedFont		2	/* new in 1.10 */
# define		rtfFontFile		3	/* new in 1.10 */
# define		rtfFileTbl		4	/* new in 1.10 */
# define		rtfFileInfo		5	/* new in 1.10 */
# define		rtfColorTbl		6
# define		rtfStyleSheet		7
# define		rtfKeyCode		8
# define		rtfRevisionTbl		9	/* new in 1.10 */
# define		rtfInfo			10
# define		rtfITitle		11
# define		rtfISubject		12
# define		rtfIAuthor		13
# define		rtfIOperator		14
# define		rtfIKeywords		15
# define		rtfIComment		16
# define		rtfIVersion		17
# define		rtfIDoccomm		18
# define		rtfIVerscomm		19
# define		rtfNextFile		20	/* reclassified in 1.10 */
# define		rtfTemplate		21	/* reclassified in 1.10 */
# define		rtfFNSep		22
# define		rtfFNContSep		23
# define		rtfFNContNotice		24
# define		rtfENSep		25	/* new in 1.10 */
# define		rtfENContSep		26	/* new in 1.10 */
# define		rtfENContNotice		27	/* new in 1.10 */
# define		rtfPageNumLevel		28	/* new in 1.10 */
# define		rtfParNumLevelStyle	29	/* new in 1.10 */
# define		rtfHeader		30
# define		rtfFooter		31
# define		rtfHeaderLeft		32
# define		rtfHeaderRight		33
# define		rtfHeaderFirst		34
# define		rtfFooterLeft		35
# define		rtfFooterRight		36
# define		rtfFooterFirst		37
# define		rtfParNumText		38	/* new in 1.10 */
# define		rtfParNumbering		39	/* new in 1.10 */
# define		rtfParNumTextAfter	40	/* new in 1.10 */
# define		rtfParNumTextBefore	41	/* new in 1.10 */
# define		rtfBookmarkStart	42
# define		rtfBookmarkEnd		43
# define		rtfPict			44
# define		rtfObject		45
# define		rtfObjClass		46
# define		rtfObjName		47
# define		rtfObjTime		48	/* new in 1.10 */
# define		rtfObjData		49
# define		rtfObjAlias		50
# define		rtfObjSection		51
# define		rtfObjResult		52
# define		rtfObjItem		53	/* new in 1.10 */
# define		rtfObjTopic		54	/* new in 1.10 */
# define		rtfDrawObject		55	/* new in 1.10 */
# define		rtfFootnote		56
# define		rtfAnnotRefStart	57	/* new in 1.10 */
# define		rtfAnnotRefEnd		58	/* new in 1.10 */
# define		rtfAnnotID		59	/* reclassified in 1.10 */
# define		rtfAnnotAuthor		60	/* new in 1.10 */
# define		rtfAnnotation		61	/* reclassified in 1.10 */
# define		rtfAnnotRef		62	/* new in 1.10 */
# define		rtfAnnotTime		63	/* new in 1.10 */
# define		rtfAnnotIcon		64	/* new in 1.10 */
# define		rtfField		65
# define		rtfFieldInst		66
# define		rtfFieldResult		67
# define		rtfDataField		68	/* new in 1.10 */
# define		rtfIndex		69
# define		rtfIndexText		70
# define		rtfIndexRange		71
# define		rtfTOC			72
# define		rtfNeXTGraphic		73
# define		rtfCustomDestination	74
# define		rtfBody				75
# define		rtfUserProps		76
# define		rtfMaxDestination	77	/* highest dest + 1 */

# define	rtfFontFamily	4
# define		rtfFFNil		0
# define		rtfFFRoman		1
# define		rtfFFSwiss		2
# define		rtfFFModern		3
# define		rtfFFScript		4
# define		rtfFFDecor		5
# define		rtfFFTech		6
# define		rtfFFBidirectional	7	/* new in 1.10 */

# define	rtfColorName	5
# define		rtfRed			0
# define		rtfGreen		1
# define		rtfBlue			2

# define	rtfSpecialChar	6
	/* special chars seen in \info destination */
# define		rtfIIntVersion		0
# define		rtfICreateTime		1
# define		rtfIRevisionTime	2
# define		rtfIPrintTime		3
# define		rtfIBackupTime		4
# define		rtfIEditTime		5
# define		rtfIYear		6
# define		rtfIMonth		7
# define		rtfIDay			8
# define		rtfIHour		9
# define		rtfIMinute		10
# define		rtfISecond		11	/* new in 1.10 */
# define		rtfINPages		12
# define		rtfINWords		13
# define		rtfINChars		14
# define		rtfIIntID		15
	/* other special chars */
# define		rtfCurHeadDate		16
# define		rtfCurHeadDateLong	17
# define		rtfCurHeadDateAbbrev	18
# define		rtfCurHeadTime		19
# define		rtfCurHeadPage		20
# define		rtfSectNum		21	/* new in 1.10 */
# define		rtfCurFNote		22
# define		rtfCurAnnotRef		23
# define		rtfFNoteSep		24
# define		rtfFNoteCont		25
# define		rtfCell			26
# define		rtfRow			27
# define		rtfPar			28
# define		rtfSect			29
# define		rtfPage			30
# define		rtfColumn		31
# define		rtfLine			32
# define		rtfSoftPage		33	/* new in 1.10 */
# define		rtfSoftColumn		34	/* new in 1.10 */
# define		rtfSoftLine		35	/* new in 1.10 */
# define		rtfSoftLineHt		36	/* new in 1.10 */
# define		rtfTab			37
# define		rtfEmDash		38
# define		rtfEnDash		39
# define		rtfEmSpace		40	/* new in 1.10 */
# define		rtfEnSpace		41	/* new in 1.10 */
# define		rtfBullet		42
# define		rtfLQuote		43
# define		rtfRQuote		44
# define		rtfLDblQuote		45
# define		rtfRDblQuote		46
# define		rtfFormula		47
# define		rtfNoBrkSpace		49
# define		rtfNoReqHyphen		50
# define		rtfNoBrkHyphen		51
# define		rtfOptDest		52
# define		rtfLTRMark		53	/* new in 1.10 */
# define		rtfRTLMark		54	/* new in 1.10 */
# define		rtfNoWidthJoiner	55	/* new in 1.10 */
# define		rtfNoWidthNonJoiner	56	/* new in 1.10 */
# define		rtfCurHeadPict		57	/* valid? */
/*# define		rtfCurAnnot		58*/	/* apparently not used */

# define	rtfStyleAttr	7
# define		rtfAdditive		0	/* new in 1.10 */
# define		rtfBasedOn		1
# define		rtfNext			2

# define	rtfDocAttr	8
# define		rtfDefTab		0
# define		rtfHyphHotZone		1
# define		rtfHyphConsecLines	2	/* new in 1.10 */
# define		rtfHyphCaps		3	/* new in 1.10 */
# define		rtfHyphAuto		4	/* new in 1.10 */
# define		rtfLineStart		5
# define		rtfFracWidth		6
# define		rtfMakeBackup		7
# define		rtfRTFDefault		8
# define		rtfPSOverlay		9
# define		rtfDocTemplate		10	/* new in 1.10 */
# define		rtfDefLanguage		11
# define		rtfFENoteType		12	/* new in 1.10 */
# define		rtfFNoteEndSect		13
# define		rtfFNoteEndDoc		14
# define		rtfFNoteText		15
# define		rtfFNoteBottom		16
# define		rtfENoteEndSect		17	/* new in 1.10 */
# define		rtfENoteEndDoc		18	/* new in 1.10 */
# define		rtfENoteText		19	/* new in 1.10 */
# define		rtfENoteBottom		20	/* new in 1.10 */
# define		rtfFNoteStart		21
# define		rtfENoteStart		22	/* new in 1.10 */
# define		rtfFNoteRestartPage	23	/* new in 1.10 */
# define		rtfFNoteRestart		24
# define		rtfFNoteRestartCont	25	/* new in 1.10 */
# define		rtfENoteRestart		26	/* new in 1.10 */
# define		rtfENoteRestartCont	27	/* new in 1.10 */
# define		rtfFNoteNumArabic	28	/* new in 1.10 */
# define		rtfFNoteNumLLetter	29	/* new in 1.10 */
# define		rtfFNoteNumULetter	30	/* new in 1.10 */
# define		rtfFNoteNumLRoman	31	/* new in 1.10 */
# define		rtfFNoteNumURoman	32	/* new in 1.10 */
# define		rtfFNoteNumChicago	33	/* new in 1.10 */
# define		rtfENoteNumArabic	34	/* new in 1.10 */
# define		rtfENoteNumLLetter	35	/* new in 1.10 */
# define		rtfENoteNumULetter	36	/* new in 1.10 */
# define		rtfENoteNumLRoman	37	/* new in 1.10 */
# define		rtfENoteNumURoman	38	/* new in 1.10 */
# define		rtfENoteNumChicago	39	/* new in 1.10 */
# define		rtfPaperWidth		40
# define		rtfPaperHeight		41
# define		rtfPaperSize		42	/* new in 1.10 */
# define		rtfLeftMargin		43
# define		rtfRightMargin		44
# define		rtfTopMargin		45
# define		rtfBottomMargin		46
# define		rtfFacingPage		47
# define		rtfGutterWid		48
# define		rtfMirrorMargin		49
# define		rtfLandscape		50
# define		rtfPageStart		51
# define		rtfWidowCtrl		52
# define		rtfLinkStyles		53	/* new in 1.10 */
# define		rtfNoAutoTabIndent	54	/* new in 1.10 */
# define		rtfWrapSpaces		55	/* new in 1.10 */
# define		rtfPrintColorsBlack	56	/* new in 1.10 */
# define		rtfNoExtraSpaceRL	57	/* new in 1.10 */
# define		rtfNoColumnBalance	58	/* new in 1.10 */
# define		rtfCvtMailMergeQuote	59	/* new in 1.10 */
# define		rtfSuppressTopSpace	60	/* new in 1.10 */
# define		rtfSuppressPreParSpace	61	/* new in 1.10 */
# define		rtfCombineTblBorders	62	/* new in 1.10 */
# define		rtfTranspMetafiles	63	/* new in 1.10 */
# define		rtfSwapBorders		64	/* new in 1.10 */
# define		rtfShowHardBreaks	65	/* new in 1.10 */
# define		rtfFormProtected	66	/* new in 1.10 */
# define		rtfAllProtected		67	/* new in 1.10 */
# define		rtfFormShading		68	/* new in 1.10 */
# define		rtfFormDisplay		69	/* new in 1.10 */
# define		rtfPrintData		70	/* new in 1.10 */
# define		rtfRevProtected		71	/* new in 1.10 */
# define		rtfRevisions		72
# define		rtfRevDisplay		73
# define		rtfRevBar		74
# define		rtfAnnotProtected	75	/* new in 1.10 */
# define		rtfRTLDoc		76	/* new in 1.10 */
# define		rtfLTRDoc		77	/* new in 1.10 */

# define	rtfSectAttr	9
# define		rtfSectDef		0
# define		rtfENoteHere		1
# define		rtfPrtBinFirst		2
# define		rtfPrtBin		3
# define		rtfSectStyleNum		4	/* new in 1.10 */
# define		rtfNoBreak		5
# define		rtfColBreak		6
# define		rtfPageBreak		7
# define		rtfEvenBreak		8
# define		rtfOddBreak		9
# define		rtfColumns		10
# define		rtfColumnSpace		11
# define		rtfColumnNumber		12	/* new in 1.10 */
# define		rtfColumnSpRight	13	/* new in 1.10 */
# define		rtfColumnWidth		14	/* new in 1.10 */
# define		rtfColumnLine		15
# define		rtfLineModulus		16
# define		rtfLineDist		17
# define		rtfLineStarts		18
# define		rtfLineRestart		19
# define		rtfLineRestartPg	20
# define		rtfLineCont		21
# define		rtfSectPageWid		22
# define		rtfSectPageHt		23
# define		rtfSectMarginLeft	24
# define		rtfSectMarginRight	25
# define		rtfSectMarginTop	26
# define		rtfSectMarginBottom	27
# define		rtfSectMarginGutter	28
# define		rtfSectLandscape	29
# define		rtfTitleSpecial		30
# define		rtfHeaderY		31
# define		rtfFooterY		32
# define		rtfPageStarts		33
# define		rtfPageCont		34
# define		rtfPageRestart		35
# define		rtfPageNumRight		36	/* renamed in 1.10 */
# define		rtfPageNumTop		37
# define		rtfPageDecimal		38
# define		rtfPageURoman		39
# define		rtfPageLRoman		40
# define		rtfPageULetter		41
# define		rtfPageLLetter		42
# define		rtfPageNumHyphSep	43	/* new in 1.10 */
# define		rtfPageNumSpaceSep	44	/* new in 1.10 */
# define		rtfPageNumColonSep	45	/* new in 1.10 */
# define		rtfPageNumEmdashSep	46	/* new in 1.10 */
# define		rtfPageNumEndashSep	47	/* new in 1.10 */
# define		rtfTopVAlign		48
# define		rtfBottomVAlign		49
# define		rtfCenterVAlign		50
# define		rtfJustVAlign		51
# define		rtfRTLSect		52	/* new in 1.10 */
# define		rtfLTRSect		53	/* new in 1.10 */

# define	rtfTblAttr	10
# define		rtfRowDef		0
# define		rtfRowGapH		1
# define		rtfCellPos		2
# define		rtfMergeRngFirst	3
# define		rtfMergePrevious	4
# define		rtfRowLeft		5
# define		rtfRowRight		6
# define		rtfRowCenter		7
# define		rtfRowLeftEdge		8
# define		rtfRowHt		9
# define		rtfRowHeader		10	/* new in 1.10 */
# define		rtfRowKeep		11	/* new in 1.10 */
# define		rtfRTLRow		12	/* new in 1.10 */
# define		rtfLTRRow		13	/* new in 1.10 */
# define		rtfRowBordTop		14	/* new in 1.10 */
# define		rtfRowBordLeft		15	/* new in 1.10 */
# define		rtfRowBordBottom	16	/* new in 1.10 */
# define		rtfRowBordRight		17	/* new in 1.10 */
# define		rtfRowBordHoriz		18	/* new in 1.10 */
# define		rtfRowBordVert		19	/* new in 1.10 */
# define		rtfCellBordBottom	20
# define		rtfCellBordTop		21
# define		rtfCellBordLeft		22
# define		rtfCellBordRight	23
# define		rtfCellShading		24
# define		rtfCellBgPatH		25
# define		rtfCellBgPatV		26
# define		rtfCellFwdDiagBgPat	27
# define		rtfCellBwdDiagBgPat	28
# define		rtfCellHatchBgPat	29
# define		rtfCellDiagHatchBgPat	30
# define		rtfCellDarkBgPatH	31
# define		rtfCellDarkBgPatV	32
# define		rtfCellFwdDarkBgPat	33
# define		rtfCellBwdDarkBgPat	34
# define		rtfCellDarkHatchBgPat	35
# define		rtfCellDarkDiagHatchBgPat 36
# define		rtfCellBgPatLineColor	37
# define		rtfCellBgPatColor	38

# define	rtfParAttr	11
# define		rtfParDef		0
# define		rtfStyleNum		1
# define		rtfHyphenate		2	/* new in 1.10 */
# define		rtfInTable		3
# define		rtfKeep			4
# define		rtfNoWidowControl	5	/* new in 1.10 */
# define		rtfKeepNext		6
# define		rtfOutlineLevel		7	/* new in 1.10 */
# define		rtfNoLineNum		8
# define		rtfPBBefore		9
# define		rtfSideBySide		10
# define		rtfQuadLeft		11
# define		rtfQuadRight		12
# define		rtfQuadJust		13
# define		rtfQuadCenter		14
# define		rtfFirstIndent		15
# define		rtfLeftIndent		16
# define		rtfRightIndent		17
# define		rtfSpaceBefore		18
# define		rtfSpaceAfter		19
# define		rtfSpaceBetween		20
# define		rtfSpaceMultiply	21	/* new in 1.10 */
# define		rtfSubDocument		22	/* new in 1.10 */
# define		rtfRTLPar		23	/* new in 1.10 */
# define		rtfLTRPar		24	/* new in 1.10 */
# define		rtfTabPos		25
# define		rtfTabLeft		26	/* new in 1.10 */
# define		rtfTabRight		27
# define		rtfTabCenter		28
# define		rtfTabDecimal		29
# define		rtfTabBar		30
# define		rtfLeaderDot		31
# define		rtfLeaderHyphen		32
# define		rtfLeaderUnder		33
# define		rtfLeaderThick		34
# define		rtfLeaderEqual		35
# define		rtfParLevel		36	/* new in 1.10 */
# define		rtfParBullet		37	/* new in 1.10 */
# define		rtfParSimple		38	/* new in 1.10 */
# define		rtfParNumCont		39	/* new in 1.10 */
# define		rtfParNumOnce		40	/* new in 1.10 */
# define		rtfParNumAcross		41	/* new in 1.10 */
# define		rtfParHangIndent	42	/* new in 1.10 */
# define		rtfParNumRestart	43	/* new in 1.10 */
# define		rtfParNumCardinal	44	/* new in 1.10 */
# define		rtfParNumDecimal	45	/* new in 1.10 */
# define		rtfParNumULetter	46	/* new in 1.10 */
# define		rtfParNumURoman		47	/* new in 1.10 */
# define		rtfParNumLLetter	48	/* new in 1.10 */
# define		rtfParNumLRoman		49	/* new in 1.10 */
# define		rtfParNumOrdinal	50	/* new in 1.10 */
# define		rtfParNumOrdinalText	51	/* new in 1.10 */
# define		rtfParNumBold		52	/* new in 1.10 */
# define		rtfParNumItalic		53	/* new in 1.10 */
# define		rtfParNumAllCaps	54	/* new in 1.10 */
# define		rtfParNumSmallCaps	55	/* new in 1.10 */
# define		rtfParNumUnder		56	/* new in 1.10 */
# define		rtfParNumDotUnder	57	/* new in 1.10 */
# define		rtfParNumDbUnder	58	/* new in 1.10 */
# define		rtfParNumNoUnder	59	/* new in 1.10 */
# define		rtfParNumWordUnder	60	/* new in 1.10 */
# define		rtfParNumStrikethru	61	/* new in 1.10 */
# define		rtfParNumForeColor	62	/* new in 1.10 */
# define		rtfParNumFont		63	/* new in 1.10 */
# define		rtfParNumFontSize	64	/* new in 1.10 */
# define		rtfParNumIndent		65	/* new in 1.10 */
# define		rtfParNumSpacing	66	/* new in 1.10 */
# define		rtfParNumInclPrev	67	/* new in 1.10 */
# define		rtfParNumCenter		68	/* new in 1.10 */
# define		rtfParNumLeft		69	/* new in 1.10 */
# define		rtfParNumRight		70	/* new in 1.10 */
# define		rtfParNumStartAt	71	/* new in 1.10 */
# define		rtfBorderTop		72
# define		rtfBorderBottom		73
# define		rtfBorderLeft		74
# define		rtfBorderRight		75
# define		rtfBorderBetween	76
# define		rtfBorderBar		77
# define		rtfBorderBox		78
# define		rtfBorderSingle		79
# define		rtfBorderThick		80
# define		rtfBorderShadow		81
# define		rtfBorderDouble		82
# define		rtfBorderDot		83
# define		rtfBorderDash		84	/* new in 1.10 */
# define		rtfBorderHair		85
# define		rtfBorderWidth		86
# define		rtfBorderColor		87
# define		rtfBorderSpace		88
# define		rtfShading		89
# define		rtfBgPatH		90
# define		rtfBgPatV		91
# define		rtfFwdDiagBgPat		92
# define		rtfBwdDiagBgPat		93
# define		rtfHatchBgPat		94
# define		rtfDiagHatchBgPat	95
# define		rtfDarkBgPatH		96
# define		rtfDarkBgPatV		97
# define		rtfFwdDarkBgPat		98
# define		rtfBwdDarkBgPat		99
# define		rtfDarkHatchBgPat	100
# define		rtfDarkDiagHatchBgPat	101
# define		rtfBgPatLineColor	102
# define		rtfBgPatColor		103

# define	rtfCharAttr	12
# define		rtfPlain		0
# define		rtfBold			1
# define		rtfAllCaps		2
# define		rtfDeleted		3
# define		rtfSubScript		4
# define		rtfSubScrShrink		5	/* new in 1.10 */
# define		rtfNoSuperSub		6	/* new in 1.10 */
# define		rtfExpand		7
# define		rtfExpandTwips		8	/* new in 1.10 */
# define		rtfKerning		9	/* new in 1.10 */
# define		rtfFontNum		10
# define		rtfFontSize		11
# define		rtfItalic		12
# define		rtfOutline		13
# define		rtfRevised		14
# define		rtfRevAuthor		15	/* new in 1.10 */
# define		rtfRevDTTM		16	/* new in 1.10 */
# define		rtfSmallCaps		17
# define		rtfShadow		18
# define		rtfStrikeThru		19
# define		rtfUnderline		20
# define		rtfDotUnderline		21	/* renamed in 1.10 */
# define		rtfDbUnderline		22
# define		rtfNoUnderline		23
# define		rtfWordUnderline	24	/* renamed in 1.10 */
# define		rtfSuperScript		25
# define		rtfSuperScrShrink	26	/* new in 1.10 */
# define		rtfInvisible		27
# define		rtfForeColor		28
# define		rtfBackColor		29
# define		rtfRTLChar		30	/* new in 1.10 */
# define		rtfLTRChar		31	/* new in 1.10 */
# define		rtfCharStyleNum		32	/* new in 1.10 */
# define		rtfCharCharSet		33	/* new in 1.10 */
# define		rtfLanguage		34
# define		rtfGray			35
# define		rtfStrikeThruDl		36
# define		rtfStrikeThruD		37	// New in Word 6J

# define	rtfPictAttr	13
# define		rtfMacQD		0
# define		rtfPMMetafile		1
# define		rtfWinMetafile		2
# define		rtfDevIndBitmap		3
# define		rtfWinBitmap		4
# define		rtfPixelBits		5
# define		rtfBitmapPlanes		6
# define		rtfBitmapWid		7
# define		rtfPicWid		8
# define		rtfPicHt		9
# define		rtfPicGoalWid		10
# define		rtfPicGoalHt		11
# define		rtfPicScaleX		12
# define		rtfPicScaleY		13
# define		rtfPicScaled		14
# define		rtfPicCropTop		15
# define		rtfPicCropBottom	16
# define		rtfPicCropLeft		17
# define		rtfPicCropRight		18
# define		rtfPicMFHasBitmap	19	/* new in 1.10 */
# define		rtfPicMFBitsPerPixel	20	/* new in 1.10 */
# define		rtfPicBinary		21

# define	rtfBookmarkAttr	14
# define		rtfBookmarkFirstCol	0
# define		rtfBookmarkLastCol	1

# define	rtfNeXTGrAttr	15
# define		rtfNeXTGWidth		0
# define		rtfNeXTGHeight		1

# define	rtfFieldAttr	16
# define		rtfFieldDirty		0
# define		rtfFieldEdited		1
# define		rtfFieldLocked		2
# define		rtfFieldPrivate		3
# define		rtfFieldAlt		4	/* new in 1.10 */

# define	rtfTOCAttr	17
# define		rtfTOCType		0
# define		rtfTOCLevel		1

# define	rtfPosAttr	18
# define		rtfAbsWid		0
# define		rtfAbsHt		1
# define		rtfRPosMargH		2
# define		rtfRPosPageH		3
# define		rtfRPosColH		4
# define		rtfPosX			5
# define		rtfPosNegX		6	/* new in 1.10 */
# define		rtfPosXCenter		7
# define		rtfPosXInside		8
# define		rtfPosXOutSide		9
# define		rtfPosXRight		10
# define		rtfPosXLeft		11
# define		rtfRPosMargV		12
# define		rtfRPosPageV		13
# define		rtfRPosParaV		14
# define		rtfPosY			15
# define		rtfPosNegY		16	/* new in 1.10 */
# define		rtfPosYInline		17
# define		rtfPosYTop		18
# define		rtfPosYCenter		19
# define		rtfPosYBottom		20
# define		rtfNoWrap		21
# define		rtfDistFromTextAll	22	/* renamed in 1.10 */
# define		rtfDistFromTextX	23	/* new in 1.10 */
# define		rtfDistFromTextY	24	/* new in 1.10 */
# define		rtfTextDistY		25
# define		rtfDropCapLines		26	/* new in 1.10 */
# define		rtfDropCapType		27	/* new in 1.10 */

# define	rtfObjAttr	19
# define		rtfObjEmb		0
# define		rtfObjLink		1
# define		rtfObjAutoLink		2
# define		rtfObjSubscriber	3
# define		rtfObjPublisher		4	/* new in 1.10 */
# define		rtfObjICEmb		5
# define		rtfObjLinkSelf		6
# define		rtfObjLock		7
# define		rtfObjUpdate		8	/* new in 1.10 */
# define		rtfObjHt		9
# define		rtfObjWid		10
# define		rtfObjSetSize		11
# define		rtfObjAlign		12	/* new in 1.10 */
# define		rtfObjTransposeY	13
# define		rtfObjCropTop		14
# define		rtfObjCropBottom	15
# define		rtfObjCropLeft		16
# define		rtfObjCropRight		17
# define		rtfObjScaleX		18
# define		rtfObjScaleY		19
# define		rtfObjResRTF		20
# define		rtfObjResPict		21
# define		rtfObjResBitmap		22
# define		rtfObjResText		23
# define		rtfObjResMerge		24
# define		rtfObjBookmarkPubObj	25
# define		rtfObjPubAutoUpdate	26

# define	rtfFNoteAttr	20			/* new in 1.10 */
# define		rtfFNAlt		0	/* new in 1.10 */

# define	rtfKeyCodeAttr	21			/* new in 1.10 */
# define		rtfAltKey		0	/* new in 1.10 */
# define		rtfShiftKey		1	/* new in 1.10 */
# define		rtfControlKey		2	/* new in 1.10 */
# define		rtfFunctionKey		3	/* new in 1.10 */

# define	rtfACharAttr	22			/* new in 1.10 */
# define		rtfACBold		0	/* new in 1.10 */
# define		rtfACAllCaps		1	/* new in 1.10 */
# define		rtfACForeColor		2	/* new in 1.10 */
# define		rtfACSubScript		3	/* new in 1.10 */
# define		rtfACExpand		4	/* new in 1.10 */
# define		rtfACFontNum		5	/* new in 1.10 */
# define		rtfACFontSize		6	/* new in 1.10 */
# define		rtfACItalic		7	/* new in 1.10 */
# define		rtfACLanguage		8	/* new in 1.10 */
# define		rtfACOutline		9	/* new in 1.10 */
# define		rtfACSmallCaps		10	/* new in 1.10 */
# define		rtfACShadow		11	/* new in 1.10 */
# define		rtfACStrikeThru		12	/* new in 1.10 */
# define		rtfACUnderline		13	/* new in 1.10 */
# define		rtfACDotUnderline	14	/* new in 1.10 */
# define		rtfACDbUnderline	15	/* new in 1.10 */
# define		rtfACNoUnderline	16	/* new in 1.10 */
# define		rtfACWordUnderline	17	/* new in 1.10 */
# define		rtfACSuperScript	18	/* new in 1.10 */

# define	rtfFontAttr	23			/* new in 1.10 */
# define		rtfFontCharSet		0	/* new in 1.10 */
# define		rtfFontPitch		1	/* new in 1.10 */
# define		rtfFontCodePage		2	/* new in 1.10 */
# define		rtfFTypeNil		3	/* new in 1.10 */
# define		rtfFTypeTrueType	4	/* new in 1.10 */

# define	rtfFileAttr	24			/* new in 1.10 */
# define		rtfFileNum		0	/* new in 1.10 */
# define		rtfFileRelPath		1	/* new in 1.10 */
# define		rtfFileOSNum		2	/* new in 1.10 */

# define	rtfFileSource	25			/* new in 1.10 */
# define		rtfSrcMacintosh		0	/* new in 1.10 */
# define		rtfSrcDOS		1	/* new in 1.10 */
# define		rtfSrcNTFS		2	/* new in 1.10 */
# define		rtfSrcHPFS		3	/* new in 1.10 */
# define		rtfSrcNetwork		4	/* new in 1.10 */

/*
 * Drawing attributes
 */

# define	rtfDrawAttr	26			/* new in 1.10 */
# define		rtfDrawLock		0	/* new in 1.10 */
# define		rtfDrawPageRelX		1	/* new in 1.10 */
# define		rtfDrawColumnRelX	2	/* new in 1.10 */
# define		rtfDrawMarginRelX	3	/* new in 1.10 */
# define		rtfDrawPageRelY		4	/* new in 1.10 */
# define		rtfDrawColumnRelY	5	/* new in 1.10 */
# define		rtfDrawMarginRelY	6	/* new in 1.10 */
# define		rtfDrawHeight		7	/* new in 1.10 */

# define		rtfDrawBeginGroup	8	/* new in 1.10 */
# define		rtfDrawGroupCount	9	/* new in 1.10 */
# define		rtfDrawEndGroup		10	/* new in 1.10 */
# define		rtfDrawArc		11	/* new in 1.10 */
# define		rtfDrawCallout		12	/* new in 1.10 */
# define		rtfDrawEllipse		13	/* new in 1.10 */
# define		rtfDrawLine		14	/* new in 1.10 */
# define		rtfDrawPolygon		15	/* new in 1.10 */
# define		rtfDrawPolyLine		16	/* new in 1.10 */
# define		rtfDrawRect		17	/* new in 1.10 */
# define		rtfDrawTextBox		18	/* new in 1.10 */

# define		rtfDrawOffsetX		19	/* new in 1.10 */
# define		rtfDrawSizeX		20	/* new in 1.10 */
# define		rtfDrawOffsetY		21	/* new in 1.10 */
# define		rtfDrawSizeY		22	/* new in 1.10 */

# define		rtfCOAngle		23	/* new in 1.10 */
# define		rtfCOAccentBar		24	/* new in 1.10 */
# define		rtfCOBestFit		25	/* new in 1.10 */
# define		rtfCOBorder		26	/* new in 1.10 */
# define		rtfCOAttachAbsDist	27	/* new in 1.10 */
# define		rtfCOAttachBottom	28	/* new in 1.10 */
# define		rtfCOAttachCenter	29	/* new in 1.10 */
# define		rtfCOAttachTop		30	/* new in 1.10 */
# define		rtfCOLength		31	/* new in 1.10 */
# define		rtfCONegXQuadrant	32	/* new in 1.10 */
# define		rtfCONegYQuadrant	33	/* new in 1.10 */
# define		rtfCOOffset		34	/* new in 1.10 */
# define		rtfCOAttachSmart	35	/* new in 1.10 */
# define		rtfCODoubleLine		36	/* new in 1.10 */
# define		rtfCORightAngle		37	/* new in 1.10 */
# define		rtfCOSingleLine		38	/* new in 1.10 */
# define		rtfCOTripleLine		39	/* new in 1.10 */

# define		rtfDrawTextBoxMargin	40	/* new in 1.10 */
# define		rtfDrawTextBoxText	41	/* new in 1.10 */
# define		rtfDrawRoundRect	42	/* new in 1.10 */

# define		rtfDrawPointX		43	/* new in 1.10 */
# define		rtfDrawPointY		44	/* new in 1.10 */
# define		rtfDrawPolyCount	45	/* new in 1.10 */

# define		rtfDrawArcFlipX		46	/* new in 1.10 */
# define		rtfDrawArcFlipY		47	/* new in 1.10 */

# define		rtfDrawLineBlue		48	/* new in 1.10 */
# define		rtfDrawLineGreen	49	/* new in 1.10 */
# define		rtfDrawLineRed		50	/* new in 1.10 */
# define		rtfDrawLinePalette	51	/* new in 1.10 */
# define		rtfDrawLineDashDot	52	/* new in 1.10 */
# define		rtfDrawLineDashDotDot	53	/* new in 1.10 */
# define		rtfDrawLineDash		54	/* new in 1.10 */
# define		rtfDrawLineDot		55	/* new in 1.10 */
# define		rtfDrawLineGray		56	/* new in 1.10 */
# define		rtfDrawLineHollow	57	/* new in 1.10 */
# define		rtfDrawLineSolid	58	/* new in 1.10 */
# define		rtfDrawLineWidth	59	/* new in 1.10 */

# define		rtfDrawHollowEndArrow	60	/* new in 1.10 */
# define		rtfDrawEndArrowLength	61	/* new in 1.10 */
# define		rtfDrawSolidEndArrow	62	/* new in 1.10 */
# define		rtfDrawEndArrowWidth	63	/* new in 1.10 */
# define		rtfDrawHollowStartArrow	64	/* new in 1.10 */
# define		rtfDrawStartArrowLength	65	/* new in 1.10 */
# define		rtfDrawSolidStartArrow	66	/* new in 1.10 */
# define		rtfDrawStartArrowWidth	67	/* new in 1.10 */

# define		rtfDrawBgFillBlue	68	/* new in 1.10 */
# define		rtfDrawBgFillGreen	69	/* new in 1.10 */
# define		rtfDrawBgFillRed	70	/* new in 1.10 */
# define		rtfDrawBgFillPalette	71	/* new in 1.10 */
# define		rtfDrawBgFillGray	72	/* new in 1.10 */
# define		rtfDrawFgFillBlue	73	/* new in 1.10 */
# define		rtfDrawFgFillGreen	74	/* new in 1.10 */
# define		rtfDrawFgFillRed	75	/* new in 1.10 */
# define		rtfDrawFgFillPalette	76	/* new in 1.10 */
# define		rtfDrawFgFillGray	77	/* new in 1.10 */
# define		rtfDrawFillPatIndex	78	/* new in 1.10 */

# define		rtfDrawShadow		79	/* new in 1.10 */
# define		rtfDrawShadowXOffset	80	/* new in 1.10 */
# define		rtfDrawShadowYOffset	81	/* new in 1.10 */

/*
 * index entry attributes
 */

# define	rtfIndexAttr	27			/* new in 1.10 */
# define		rtfIndexNumber		0	/* new in 1.10 */
# define		rtfIndexBold		1	/* reclassified in 1.10 */
# define		rtfIndexItalic		2	/* reclassified in 1.10 */

/*
 * user prop attributes
 */

# define rtfUserPropAttr	28
# define		rtfPropName			0
# define		rtfPropType			1
# define		rtfPropValue		2
# define		rtfPropLink			3

/*
 * \wmetafile argument values
 */

# define	rtfWmMmText		1
# define	rtfWmMmLometric		2
# define	rtfWmMmHimetric		3
# define	rtfWmMmLoenglish	4
# define	rtfWmMmHienglish	5
# define	rtfWmMmTwips		6
# define	rtfWmMmIsotropic	7
# define	rtfWmMmAnisotropic	8

/*
 * \pmmetafile argument values
 */

# define	rtfPmPuArbitrary	4
# define	rtfPmPuPels		8
# define	rtfPmPuLometric		12
# define	rtfPmPuHimetric		16
# define	rtfPmPuLoenglish	20
# define	rtfPmPuHienglish	24
# define	rtfPmPuTwips		28

/*
 * \lang argument values
 */

# define	rtfLangNoLang			0x0400
# define	rtfLangAlbanian			0x041c
# define	rtfLangArabic			0x0401
# define	rtfLangBahasa			0x0421
# define	rtfLangBelgianDutch		0x0813
# define	rtfLangBelgianFrench		0x080c
# define	rtfLangBrazilianPortuguese	0x0416
# define	rtfLangBulgarian		0x0402
# define	rtfLangCatalan			0x0403
# define	rtfLangLatinCroatoSerbian	0x041a
# define	rtfLangCzech			0x0405
# define	rtfLangDanish			0x0406
# define	rtfLangDutch			0x0413
# define	rtfLangAustralianEnglish	0x0c09
# define	rtfLangUKEnglish		0x0809
# define	rtfLangUSEnglish		0x0409
# define	rtfLangFinnish			0x040b
# define	rtfLangFrench			0x040c
# define	rtfLangCanadianFrench		0x0c0c
# define	rtfLangGerman			0x0407
# define	rtfLangGreek			0x0408
# define	rtfLangHebrew			0x040d
# define	rtfLangHungarian		0x040e
# define	rtfLangIcelandic		0x040f
# define	rtfLangItalian			0x0410
# define	rtfLangJapanese			0x0411
# define	rtfLangKorean			0x0412
# define	rtfLangBokmalNorwegian		0x0414
# define	rtfLangNynorskNorwegian		0x0814
# define	rtfLangPolish			0x0415
# define	rtfLangPortuguese		0x0816
# define	rtfLangRhaetoRomanic		0x0417
# define	rtfLangRomanian			0x0418
# define	rtfLangRussian			0x0419
# define	rtfLangCyrillicSerboCroatian	0x081a
# define	rtfLangSimplifiedChinese	0x0804
# define	rtfLangSlovak			0x041b
# define	rtfLangCastilianSpanish		0x040a
# define	rtfLangMexicanSpanish		0x080a
# define	rtfLangSwedish			0x041d
# define	rtfLangSwissFrench		0x100c
# define	rtfLangSwissGerman		0x0807
# define	rtfLangSwissItalian		0x0810
# define	rtfLangThai			0x041e
# define	rtfLangTraditionalChinese	0x0404
# define	rtfLangTurkish			0x041f
# define	rtfLangUrdu			0x0420

/*
 * CharSet indices
 */

# define	rtfCSGeneral	0	/* general (default) charset */
# define	rtfCSSymbol	1	/* symbol charset */

/*
 * Flags for auto-charset-processing.  Both are on by default.
 */

# define	rtfReadCharSet		0x01	/* auto-read charset files */
# define	rtfSwitchCharSet	0x02	/* auto-switch charset maps */

/*
 * Style types
 */

# define	rtfParStyle	0	/* the default */
# define	rtfCharStyle	1
# define	rtfSectStyle	2



