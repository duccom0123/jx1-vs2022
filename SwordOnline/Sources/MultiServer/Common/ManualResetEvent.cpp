#include "stdafx.h"
#include "ManualResetEvent.h"

CManualResetEvent::CManualResetEvent( bool initialState /* = false */ )
   : CEvent( 0, true, initialState )
{

}

CManualResetEvent::CManualResetEvent( const char *pName, bool initialState /* = false */ )
   : CEvent( 0, true, initialState, pName )
{
   
}
