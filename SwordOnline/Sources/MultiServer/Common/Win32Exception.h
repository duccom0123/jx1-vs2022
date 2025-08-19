/********************************************************************
	created:	2003/02/13
	file base:	Win32Exception
	file ext:	h
	author:		liupeng
	
	purpose:	Header file for CWin32Exception class
*********************************************************************/
#ifndef __INCLUDE_WIN32EXCEPTION_H__
#define __INCLUDE_WIN32EXCEPTION_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

#include "Exception.h"


/*
 * CWin32Exception
 */
class CWin32Exception : public CException
{
public: 
	
	CWin32Exception( const _tstring &where, DWORD error );	
	DWORD GetError() const { return m_error; };
	
protected:
	
	DWORD m_error;
};



#endif //__INCLUDE_WIN32EXCEPTION_H__