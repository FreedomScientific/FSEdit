#pragma once
__interface
	__declspec(uuid("11111111-2222-3333-5555-666666666666"))
	IBrailleTranslator : IUnknown
{
	HRESULT SetTranslatorLanguage(BSTR langAbbrev,VARIANT_BOOL *pResult);
	HRESULT Translate(BSTR untranslatedText,BSTR *translatedText,long *pOffsets,long maxOffsets,long excludeSTart,long excludeEnd);
	HRESULT BackTranslate(BSTR translatedText,BSTR *untranslatedText);
	HRESULT BackTranslateChar(BSTR translatedText,long offsetOfChar,BSTR *unTranslated);
	HRESULT TranslateFile(BSTR sourceName,BSTR destName,VARIANT_BOOL convertToGrade2Braille,VARIANT_BOOL *pResult);
	HRESULT BackTranslateFile(BSTR sourceName,BSTR destName,VARIANT_BOOL *pResult);
};
