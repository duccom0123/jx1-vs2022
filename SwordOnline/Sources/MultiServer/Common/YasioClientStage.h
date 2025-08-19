// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
/********************************************************************
	created:	2025/08/19
	file base:	YasioClientStage
	file ext:	h
	author:		duccom0123

	purpose:	Game client implementation using yasio
*********************************************************************/
#ifndef __INCLUDE_YASIOCLIENTSTAGE_H__
#define __INCLUDE_YASIOCLIENTSTAGE_H__
#pragma once

#include "YasioClient.h"
#include "Buffer.h"

/*
 * class CYasioGameClient
 */
class CYasioGameClient : public CYasioClient
{
  public:
    bool Startup();
    bool Cleanup(int isCleartheThread = false);
    bool FsGameServerConnectTo(const char *const &pAddressToConnectServer, unsigned short usPortToConnectServer);

    int SendMsg(const void *pBuffer, int nSize);
    bool SendPackToServer(const void *const pData, const unsigned int &datalength);
    const void *GetPackFromServer(unsigned int &datalength);
    bool Shutdown();
    void setBufferEmpty();

    /*
	 *  IUnknown COM Interface Methods
	 */
    bool QueryInterface(void **ppv);
    unsigned int AddRef();
    unsigned int Release();

  public:
    CYasioGameClient(unsigned int maxFreeBuffers, unsigned int maxFreeBuffers_Cache, unsigned int bufferSize_Cache = 8192, unsigned int bufferSize = 1024, int mSocketNo = 0);
    virtual ~CYasioGameClient();

    unsigned int GetMessageSize(const CBuffer *pBuffer) const;

  protected:
    unsigned int GetMinimumMessageSize() const;
    CBuffer *ProcessDataStream(CBuffer *pBuffer);
    void ProcessCommand(const CBuffer *pBuffer);

  private:
    virtual void OnStartConnections() override;
    virtual void OnStopConnections() override;
    virtual void OnShutdownComplete() override;
    virtual void OnShutdownInitiated() override;
    virtual void ReadCompleted(CBuffer *pBuffer) override;

  private:
    long m_lRefCount;
    CBuffer::Allocator m_theCacheAllocator;
    void *m_lpCallBackParam;

    CCriticalSection m_csReadAction;
    CBuffer *m_pRecvBuffer;
    CBuffer *m_pReadBuffer;

    /*
	 * No copies do not implement
	 */
    CYasioGameClient(const CYasioGameClient &rhs);
    CYasioGameClient &operator=(const CYasioGameClient &rhs);
};

/*
 * CYasioClientFactory
 */
class CYasioClientFactory
{
  public:
    /*
	 *  IUnknown COM Interface Methods
	 */
    unsigned int AddRef();
    unsigned int Release();
    CYasioGameClient *pObject;

  public:
    CYasioClientFactory();
    virtual ~CYasioClientFactory();

  private:
    long m_lRefCount;
    unsigned int m_bufferSize;
    unsigned int m_MaxPlauerCount;
    unsigned int m_nPrecision;
};

#endif
