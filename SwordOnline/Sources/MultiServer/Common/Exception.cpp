#include "stdafx.h"
#include "Exception.h"


/*
 * CException
 */
CException::CException( const _tstring &where, const _tstring &message )
		: m_where( where ), 
		m_message( message )
{
}

_tstring CException::GetWhere() const 
{ 
	return m_where;
}

_tstring CException::GetMessage() const 
{ 
	return m_message; 
}

void CException::MessageBox(HWND hWnd /* = NULL */) const 
{ 
	::MessageBox( hWnd, GetMessage().c_str(), GetWhere().c_str(), MB_ICONSTOP );
}
