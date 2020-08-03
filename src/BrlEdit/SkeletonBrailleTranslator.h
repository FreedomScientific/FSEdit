#pragma once
// This is a skeleton of the class that needs to be written to interface FSEdit with the Braille translator being used. 

#include "IBrailleTranslator.h"
class CSkeletonBrailleTranslator : public CComObjectRoot,
	public IBrailleTranslator
{
public:
	CSkeletonBrailleTranslator() = default;
	virtual ~CSkeletonBrailleTranslator() = default;

	STDMETHODIMP SetTranslatorLanguage(BSTR langAbbrev, VARIANT_BOOL* pResult)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP Translate(BSTR untranslatedText, BSTR* translatedText, long* pOffsets, long maxOffsets, long excludeSTart, long excludeEnd)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP BackTranslate(BSTR translatedText, BSTR* untranslatedText)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP BackTranslateChar(BSTR translatedText, long offsetOfChar, BSTR* unTranslated)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP TranslateFile(BSTR sourceName, BSTR destName, VARIANT_BOOL ConvertToGrade2Braille, VARIANT_BOOL * pResult)
	{
		return E_NOTIMPL;
	}
	STDMETHODIMP BackTranslateFile(BSTR sourceName, BSTR destName, VARIANT_BOOL * pResult)
	{
		return E_NOTIMPL;
	}

	BEGIN_COM_MAP(CSkeletonBrailleTranslator)
		COM_INTERFACE_ENTRY(IBrailleTranslator)
		END_COM_MAP()
	DECLARE_NOT_AGGREGATABLE(CSkeletonBrailleTranslator)

};

