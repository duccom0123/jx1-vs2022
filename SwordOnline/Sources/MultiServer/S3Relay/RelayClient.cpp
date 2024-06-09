// RelayClient.cpp: implementation of the CRelayClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "RelayClient.h"
#include "RelayCenter.h"
#include "DealRelay.h"
#include "S3Relay.h"
#include "../../Engine/Src/KGLog.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRelayClient::CRelayClient(CRelayCenter* pRelayCenter, BOOL bAutoFree)
	: CNetClient(pRelayCenter, bAutoFree)
{

}

CRelayClient::~CRelayClient()
{

}


void CRelayClient::OnStartupFail()
{
	KGLogPrintf(LOG_INFO,"relay client startup fail: %s\n", _ip2a(GetSvrIP()));
}

void CRelayClient::OnShutdownFail()
{
	KGLogPrintf(LOG_INFO,"relay client shutdown fail: %s\n", _ip2a(GetSvrIP()));
}


void CRelayClient::OnServerEventCreate()
{
	KGLogPrintf(LOG_INFO,"relay client create: %s\n", _ip2a(GetSvrIP()));
}

void CRelayClient::OnServerEventClose()
{
	KGLogPrintf(LOG_INFO,"relay client close: %s\n", _ip2a(GetSvrIP()));
}


void CRelayClient::RecvPackage(const void* pData, size_t size)
{
	dealrelay::DoRecvPackage(this, pData, size);
}

