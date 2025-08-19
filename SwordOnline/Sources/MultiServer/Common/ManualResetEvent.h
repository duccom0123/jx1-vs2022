/********************************************************************
	created:	2003/02/14
	file base:	ManualResetEvent
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#ifndef __INCLUDE_MANUALRESETEVENT_H__
#define __INCLUDE_MANUALRESETEVENT_H__

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
	#pragma once
#endif

#include "Event.h"


class CManualResetEvent : public CEvent
{
public:
	
	explicit CManualResetEvent( bool initialState = false );	
	explicit CManualResetEvent( const char *pName, bool initialState = false );
	
private:
	
	/*
	 * No copies do not implement
	 */
	CManualResetEvent( const CManualResetEvent &rhs );
	CManualResetEvent &operator=( const CManualResetEvent &rhs );
};


#endif //__INCLUDE_MANUALRESETEVENT_H__