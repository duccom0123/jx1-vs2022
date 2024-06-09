// RelayConnect.cpp: implementation of the CRelayConnect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "RelayConnect.h"
#include "RelayServer.h"
#include "DealRelay.h"
#include "S3Relay.h"
#include "../../Engine/Src/KGLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelayConnect::CRelayConnect(CRelayServer* pRelayServer, unsigned long id)
	: CNetConnect(pRelayServer, id)
{

}

CRelayConnect::~CRelayConnect()
{

}


void CRelayConnect::OnClientConnectCreate()
{
	KGLogPrintf(LOG_INFO,"relay connect create: %s\n", _ip2a(GetIP()));
}

void CRelayConnect::OnClientConnectClose()
{
	KGLogPrintf(LOG_INFO,"relay connect close: %s\n", _ip2a(GetIP()));
}


void CRelayConnect::RecvPackage(const void* pData, size_t size)
{
	dealrelay::DoRecvPackage(this, pData, size);
}
