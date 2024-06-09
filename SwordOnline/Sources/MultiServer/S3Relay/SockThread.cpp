// SockThread.cpp: implementation of the CSockThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SockThread.h"
#include "S3Relay.h"
#include "Global.h"
#include "../../Engine/Src/KGLog.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSockThread::CSockThread()
	: m_step(step_NONE), m_cntLoop(0), m_tickLoop(0)
{

}

CSockThread::~CSockThread()
{

}


DWORD CSockThread::Main(LPVOID lpParam)
{
	try
	{
		DEBUGDO( m_step = step_NONE );
		DEBUGDO( m_tickLoop = 0 );
		DEBUGDO( m_cntLoop = 0 );


		DEBUGDO( m_step = step_EnterLoop);
		EnterLoop();

		while (!IsAskingStop())
		{
			DEBUGDO( m_tickLoop = ::GetTickCount() );
			DEBUGDO( ++ m_cntLoop );


			DEBUGDO( m_step = step_PrepareSock);
			PrepareSock();


			DEBUGDO( m_step = step_RelayCenter);
			g_RelayCenter.Route();
			DEBUGDO( m_step = step_RelayServer);
			g_RelayServer.Route();

			DEBUGDO( m_step = step_RootCenter);
			g_RootCenter.Route();

			DEBUGDO( m_step = step_GatewayCenter);
			g_GatewayCenter.Route();

			DEBUGDO( m_step = step_HostServer);
			g_HostServer.Route();
			DEBUGDO( m_step = step_TongServer);
			g_TongServer.Route();
			DEBUGDO( m_step = step_ChatServer);
			g_ChatServer.Route();


			DEBUGDO( m_step = step_UnprepareSock);
			UnprepareSock();


			DEBUGDO( m_step = step_Sleep);
			::Sleep(breathe_interval);
		}

		DEBUGDO( m_step = step_LeaveLoop);
		LeaveLoop();
	}
	catch (...)
	{
		KGLogPrintf(LOG_INFO,"fatal: sock thread except\n");
	}

	return 0;
}


HANDLE CSockThread::Start()
{
	if (m_hProcessor != NULL)
		return m_hProcessor;

	m_step = step_NONE;
	m_cntLoop = 0;
	m_tickLoop = 0;

	HANDLE reth = KThread::Start();
	if (!reth)
	{
		KGLogPrintf(LOG_INFO,"FAIL: SockThread start\n");
		return NULL;
	}

	KGLogPrintf(LOG_INFO,"SockThread start success\n");

	return reth;
}

BOOL CSockThread::Stop()
{
	if (!m_hProcessor)
		return TRUE;

	if (!KThread::Stop())
	{
		KGLogPrintf(LOG_INFO,"FAIL: SockThread stop\n");
		return FALSE;
	}

	m_step = step_NONE;
	m_cntLoop = 0;
	m_tickLoop = 0;

	KGLogPrintf(LOG_INFO,"SockThread stop success\n");

	return TRUE;
}


BOOL CSockThread::TraceInfo()
{
	static const char* stepdesc[] = {
			"NONE",
			"EnterLoop",
			"PrepareSock",
			"RelayCenter", 
			"RelayServer",
			"RootCenter",
			"GatewayCenter",
			"HostServer",
			"TongServer",
			"ChatServer",
			"UnprepareSock",
			"LeaveLoop",
			"Sleep",
	};

	KGLogPrintf(LOG_INFO,"message: [SockThread] tickLoop: %d, cntLoop: %d, step: %s\n", m_tickLoop, m_cntLoop, stepdesc[m_step]);

	return TRUE;
}
