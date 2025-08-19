// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
#include "stdafx.h"
#include "YasioClientStage.h"
#include "KSG_EncodeDecode.h"
#include <time.h>

/*
 * class CYasioGameClient
 */
CYasioGameClient::CYasioGameClient(unsigned int maxFreeBuffers, unsigned int maxFreeBuffers_Cache, unsigned int bufferSize_Cache /*= 8192*/, unsigned int bufferSize, /*= 1024*/
                                   int mSocketNo)
    : CYasioClient(maxFreeBuffers, bufferSize, mSocketNo), m_theCacheAllocator(bufferSize_Cache, maxFreeBuffers_Cache), m_lRefCount(0)
{
    m_pReadBuffer = m_theCacheAllocator.Allocate();
    m_pRecvBuffer = m_theCacheAllocator.Allocate();
}

CYasioGameClient::~CYasioGameClient()
{
    m_pReadBuffer->Release();
    m_pRecvBuffer->Release();
}

bool CYasioGameClient::Startup()
{
    bool hr = false;

    try
    {
        Start();
        hr = true;
    }
    catch (...)
    {
    }

    return hr;
}

bool CYasioGameClient::Cleanup(int isCleartheThread)
{
    bool hr = false;

    try
    {
        if (isCleartheThread)
        {
            WaitForShutdownToComplete(isCleartheThread);
        }
        else
        {
            StopConnections();
            WaitForShutdownToComplete();
        }

        hr = true;
    }
    catch (...)
    {
    }

    return hr;
}

bool CYasioGameClient::FsGameServerConnectTo(const char *const &pAddressToConnectServer, unsigned short usPortToConnectServer)
{
    bool hr = false;

    try
    {
        Connect(pAddressToConnectServer, usPortToConnectServer);

        if (StartConnections())
        {
            hr = true;
        }
    }
    catch (...)
    {
    }

    return hr;
}

int CYasioGameClient::SendMsg(const void *pBuffer, int nSize)
{
    return SendPackToServer((const unsigned char *)pBuffer, nSize);
}

bool CYasioGameClient::SendPackToServer(const void *const pData, const unsigned int &datalength)
{
    static const unsigned int s_len_protocol = sizeof(unsigned short);

    if (!pData || 0 == datalength)
    {
        return false;
    }
    if (m_uKeyMode != 0)
        return false;

    CBuffer *pBuffer = Allocate();

    /*
	 * Add package header
	 */
    const unsigned int headlength = s_len_protocol + datalength;
    pBuffer->AddData(reinterpret_cast<const char *>(&headlength), s_len_protocol);

    // Encode data
    KSG_EncodeBuf(datalength, (unsigned char *)pData, &m_uClientKey);
    pBuffer->AddData(reinterpret_cast<const char *const>(pData), datalength);

    bool isSend = false;

    if (Write(pBuffer))
        isSend = true;

    pBuffer->Release();

    return isSend;
}

const void *CYasioGameClient::GetPackFromServer(unsigned int &datalength)
{
    CCriticalSection::Owner locker(m_csReadAction);
    m_pReadBuffer->Empty();
    m_pRecvBuffer = ProcessDataStream(m_pRecvBuffer);
    const unsigned char *pPackData = m_pReadBuffer->GetBuffer();
    datalength = m_pReadBuffer->GetUsed();
    return reinterpret_cast<const char *>(pPackData);
}

void CYasioGameClient::setBufferEmpty()
{
    m_pReadBuffer->Empty();
    m_pRecvBuffer->Empty();
}

bool CYasioGameClient::Shutdown()
{
    StopConnections();
    return true;
}

bool CYasioGameClient::QueryInterface(void **ppv)
{
    /*
	 * By definition all COM objects support the IUnknown interface
	 */
    *ppv = dynamic_cast<void *>(this);
    return true;
}

unsigned int CYasioGameClient::AddRef()
{
#ifdef WIN32
    return InterlockedIncrement(&m_lRefCount);
#else
    return __sync_fetch_and_add(&m_lRefCount, 1);
#endif
}

unsigned int CYasioGameClient::Release()
{
#ifdef WIN32
    if (InterlockedDecrement(&m_lRefCount) > 0)
#else
    if (__sync_fetch_and_sub(&m_lRefCount, 1) > 0)
#endif
    {
        return m_lRefCount;
    }
    delete this;
    return 0L;
}

void CYasioGameClient::ProcessCommand(const CBuffer *pBuffer)
{
    static const unsigned int s_len_protocol = sizeof(unsigned short);
    const unsigned char *pPackData = pBuffer->GetBuffer();
    const unsigned int used = pBuffer->GetUsed();
    if (used <= s_len_protocol)
    {
        return;
    }

    if (m_uKeyMode != 0)
        return;

    char *Buff = NULL;
    Buff = (char *)malloc(used - s_len_protocol + 1);
    if (Buff == NULL)
        return;
    memset(Buff, 0, used - s_len_protocol + 1);

    memcpy(Buff, pPackData + s_len_protocol, used - s_len_protocol);

    KSG_DecodeBuf(used - s_len_protocol, (unsigned char *)Buff, &m_uServerKey);

    m_pReadBuffer->AddData(Buff, used - s_len_protocol);

    free(Buff);
    Buff = NULL;
}

unsigned int CYasioGameClient::GetMinimumMessageSize() const
{
    static unsigned int length = sizeof(unsigned short) + sizeof(unsigned char);
    /*
	 * The smallest possible command we accept is a byte only package
	 */
    return length;
}

unsigned int CYasioGameClient::GetMessageSize(const CBuffer *pBuffer) const
{
    const unsigned char *pData = pBuffer->GetBuffer();
    const unsigned int used = pBuffer->GetUsed();
    unsigned short wHeadLen = (unsigned short)(*(unsigned short *)(pData));
    return (unsigned int)(wHeadLen);
}

CBuffer *CYasioGameClient::ProcessDataStream(CBuffer *pBuffer)
{
    if (pBuffer == NULL)
        return NULL;

    const unsigned int used = pBuffer->GetUsed();
    if (used >= GetMinimumMessageSize())
    {
        const unsigned int messageSize = GetMessageSize(pBuffer);
        if (messageSize == 0)
        {
            if (used == (pBuffer->GetSize() - 1))
            {
                Shutdown();
                pBuffer->Empty();
            }
        }
        else if (used == messageSize)
        {
            ProcessCommand(pBuffer);
            pBuffer->Empty();
        }
        else if (used > messageSize)
        {
            CBuffer *pMessage = pBuffer->SplitBuffer(messageSize);
            ProcessCommand(pMessage);
            pMessage->Release();
        }
    }
    return pBuffer;
}

void CYasioGameClient::OnStartConnections()
{
    setSocketStates(true);
}

void CYasioGameClient::OnShutdownInitiated()
{
}

void CYasioGameClient::OnShutdownComplete()
{
}

void CYasioGameClient::OnStopConnections()
{
    setSocketStates(false);
}

void CYasioGameClient::ReadCompleted(CBuffer *pBuffer)
{
    try
    {
        const unsigned char *pPackData = pBuffer->GetBuffer();
        unsigned int used = pBuffer->GetUsed();
        if (used > 0)
        {
            CCriticalSection::Owner locker(m_csReadAction);
            m_pRecvBuffer->AddData(reinterpret_cast<const char *>(pPackData), used);
        }
        pBuffer->Empty();
    }
    catch (...)
    {
        StopConnections();
    }
}

CYasioClientFactory::CYasioClientFactory() : m_lRefCount(0), m_bufferSize(0), m_MaxPlauerCount(0), m_nPrecision(0)

{
    pObject = NULL;
}

CYasioClientFactory::~CYasioClientFactory()
{
}

unsigned int CYasioClientFactory::AddRef()
{
#ifdef WIN32
    return ::InterlockedIncrement(&m_lRefCount);
#else
    return __sync_fetch_and_add(&m_lRefCount, 1);
#endif
}

unsigned int CYasioClientFactory::Release()
{
#ifdef WIN32
    if (::InterlockedDecrement(&m_lRefCount) > 0)
#else
    if (__sync_fetch_and_sub(&m_lRefCount, 1) > 0)
#endif
    {
        return m_lRefCount;
    }

    delete this;
    return 0L;
}
