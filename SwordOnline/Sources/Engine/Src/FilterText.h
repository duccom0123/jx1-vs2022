// -------------------------------------------------------------------------
//	文件名		：	FilterText.h
//	创建者		：	谢茂培 (Hsie)
//	创建时间	：	2003-07-23 10:13:07
//	功能描述	：
//
// -------------------------------------------------------------------------
#ifndef __FILTERTEXT_H__
#define __FILTERTEXT_H__

#include "unknwn.h"


const TCHAR leadchar_common = '=';
const TCHAR leadchar_advance = '+';
const TCHAR leadchar_ignore = '-';
const TCHAR leadchar_insensitive = '@';

struct ITextFilter : IUnknown
{
    virtual BOOL AddExpression(LPCTSTR szExp) = 0;
    virtual BOOL Clearup() = 0;
    virtual BOOL IsTextPass(LPCTSTR text) = 0;
};

extern "C" ENGINE_API ITextFilter *CreateTextFilter();

#endif   // __FILTERTEXT_H__
