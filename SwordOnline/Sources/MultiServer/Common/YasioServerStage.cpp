// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
#include "stdafx.h"
#include "YasioServerStage.h"
#include "KSG_EncodeDecode.h"
#include <time.h>

/*
 * CYasioGameConnection Implementation
 */
CYasioGameConnection::CYasioGameConnection(CYasioServer *server, yasio::inet::io_transport *transport, int connectionId) : CYasioConnection(server, transport, connectionId), m_theCacheAllocator(8192, 50), m_uKeyMode(0), m_uServerKey(0), m_uClientKey(0), m_bAuthenticated(false), m_playerData(nullptr)
{
    m_pReadBuffer = m_theCacheAllocator.Allocate();
    m_pRecvBuffer = m_theCacheAllocator.Allocate();
}

CYasioGameConnection::~CYasioGameConnection()
{
    if (m_pReadBuffer)
        m_pReadBuffer->Release();
    if (m_pRecvBuffer)
        m_pRecvBuffer->Release();
}

bool CYasioGameConnection::SendPackToClient(const void *const pData, const unsigned int &datalength)
{
    static const unsigned int s_len_protocol = sizeof(unsigned short);

    if (!pData || 0 == datalength)
    {
        return false;
    }
    if (m_uKeyMode != 0)
        return false;

    // Create a temporary buffer for the packet
    char *tempBuffer = (char *)malloc(s_len_protocol + datalength);
    if (!tempBuffer)
        return false;

    try
    {
        // Add package header
        const unsigned int headlength = s_len_protocol + datalength;
        memcpy(tempBuffer, &headlength, s_len_protocol);

        // Copy and encode data
        memcpy(tempBuffer + s_len_protocol, pData, datalength);
        KSG_EncodeBuf(datalength, (unsigned char *)(tempBuffer + s_len_protocol), &m_uServerKey);

        bool result = Write(tempBuffer, headlength);

        free(tempBuffer);
        return result;
    }
    catch (...)
    {
        free(tempBuffer);
        return false;
    }
}

const void *CYasioGameConnection::GetPackFromClient(unsigned int &datalength)
{
    CCriticalSection::Owner locker(m_csReadAction);
    m_pReadBuffer->Empty();
    m_pRecvBuffer = ProcessDataStream(m_pRecvBuffer);
    const unsigned char *pPackData = m_pReadBuffer->GetBuffer();
    datalength = m_pReadBuffer->GetUsed();
    return reinterpret_cast<const char *>(pPackData);
}

void CYasioGameConnection::setBufferEmpty()
{
    m_pReadBuffer->Empty();
    m_pRecvBuffer->Empty();
}

void CYasioGameConnection::SetCipherKeys(unsigned int serverKey, unsigned int clientKey)
{
    m_uServerKey = serverKey;
    m_uClientKey = clientKey;
}

void CYasioGameConnection::ProcessCommand(const CBuffer *pBuffer)
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

    KSG_DecodeBuf(used - s_len_protocol, (unsigned char *)Buff, &m_uClientKey);

    m_pReadBuffer->AddData(Buff, used - s_len_protocol);

    free(Buff);
    Buff = NULL;
}

unsigned int CYasioGameConnection::GetMinimumMessageSize() const
{
    static unsigned int length = sizeof(unsigned short) + sizeof(unsigned char);
    return length;
}

unsigned int CYasioGameConnection::GetMessageSize(const CBuffer *pBuffer) const
{
    const unsigned char *pData = pBuffer->GetBuffer();
    const unsigned int used = pBuffer->GetUsed();
    unsigned short wHeadLen = (unsigned short)(*(unsigned short *)(pData));
    return (unsigned int)(wHeadLen);
}

CBuffer *CYasioGameConnection::ProcessDataStream(CBuffer *pBuffer)
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
                Close();
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

/*
 * CYasioGameServer Implementation
 */
CYasioGameServer::CYasioGameServer(unsigned int maxFreeBuffers, unsigned int maxFreeBuffers_Cache, unsigned int bufferSize_Cache, unsigned int bufferSize) : CYasioServer(maxFreeBuffers, bufferSize), m_lRefCount(0)
{
}

CYasioGameServer::~CYasioGameServer()
{
}

bool CYasioGameServer::Startup(const std::string &address, unsigned short port)
{
    bool hr = false;

    try
    {
        if (StartListening(address, port))
        {
            hr = true;
        }
    }
    catch (...)
    {
    }

    return hr;
}

bool CYasioGameServer::Cleanup()
{
    bool hr = false;

    try
    {
        StopListening();
        hr = true;
    }
    catch (...)
    {
    }

    return hr;
}

bool CYasioGameServer::Shutdown()
{
    StopListening();
    return true;
}

size_t CYasioGameServer::GetPlayerCount() const
{
    return GetConnectionCount();
}

CYasioGameConnection *CYasioGameServer::GetPlayerConnection(int connectionId)
{
    return CastToGameConnection(GetConnection(connectionId));
}

void CYasioGameServer::DisconnectPlayer(int connectionId)
{
    CloseConnection(connectionId);
}

void CYasioGameServer::BroadcastToAllPlayers(const void *pData, unsigned int dataLength)
{
    BroadcastToAll(reinterpret_cast<const char *>(pData), dataLength);
}

bool CYasioGameServer::QueryInterface(void **ppv)
{
    *ppv = dynamic_cast<void *>(this);
    return true;
}

unsigned int CYasioGameServer::AddRef()
{
#ifdef WIN32
    return InterlockedIncrement(&m_lRefCount);
#else
    return __sync_fetch_and_add(&m_lRefCount, 1);
#endif
}

unsigned int CYasioGameServer::Release()
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

void CYasioGameServer::OnStartListening()
{
    // Server started listening
}

void CYasioGameServer::OnStopListening()
{
    // Server stopped listening
}

void CYasioGameServer::OnClientConnect(CYasioConnection *connection)
{
    CYasioGameConnection *gameConnection = CastToGameConnection(connection);
    if (gameConnection)
    {
        // Send cipher handshake
        SendCipherHandshake(gameConnection);

        // Call user handler
        if (m_connectHandler)
        {
            m_connectHandler(gameConnection);
        }
    }
}

void CYasioGameServer::OnClientDisconnect(CYasioConnection *connection)
{
    CYasioGameConnection *gameConnection = CastToGameConnection(connection);
    if (gameConnection)
    {
        // Call user handler
        if (m_disconnectHandler)
        {
            m_disconnectHandler(gameConnection);
        }
    }
}

void CYasioGameServer::OnClientMessage(CYasioConnection *connection, CBuffer *buffer)
{
    CYasioGameConnection *gameConnection = CastToGameConnection(connection);
    if (gameConnection && buffer)
    {
        // Add received data to connection's buffer
        {
            CCriticalSection::Owner locker(gameConnection->m_csReadAction);
            gameConnection->m_pRecvBuffer->AddData(reinterpret_cast<const char *>(buffer->GetBuffer()), buffer->GetUsed());
        }

        // Process any complete messages
        unsigned int dataLength = 0;
        const void *pData = gameConnection->GetPackFromClient(dataLength);

        if (pData && dataLength > 0)
        {
            // Call user message handler
            if (m_messageHandler)
            {
                m_messageHandler(gameConnection, pData, dataLength);
            }
        }
    }
}

void CYasioGameServer::OnError(const std::string &message)
{
    // Log error or handle as needed
}

void CYasioGameServer::SendCipherHandshake(CYasioGameConnection *gameConnection)
{
    if (!gameConnection)
        return;

    // Initialize cipher keys
    srand((unsigned int)time(NULL));
    unsigned int serverKey = rand();
    unsigned int clientKey = rand();

    gameConnection->SetCipherKeys(serverKey, clientKey);

    // Prepare account begin packet
    gameConnection->m_theSendAccountBegin.wLen = sizeof(ACCOUNT_BEGIN);
    gameConnection->m_theSendAccountBegin.AccountBegin.ProtocolType = CIPHER_PROTOCOL_TYPE;
    gameConnection->m_theSendAccountBegin.AccountBegin.Mode = 0;
    gameConnection->m_theSendAccountBegin.AccountBegin.ServerKey = ~serverKey;
    gameConnection->m_theSendAccountBegin.AccountBegin.ClientKey = ~clientKey;

    // Send handshake
    gameConnection->Write(reinterpret_cast<const char *>(&gameConnection->m_theSendAccountBegin), sizeof(gameConnection->m_theSendAccountBegin));
}

CYasioGameConnection *CYasioGameServer::CastToGameConnection(CYasioConnection *connection)
{
    return dynamic_cast<CYasioGameConnection *>(connection);
}

/*
 * CYasioServerFactory Implementation
 */
CYasioServerFactory::CYasioServerFactory() : m_lRefCount(0), m_bufferSize(0), m_MaxPlayerCount(0), m_nPrecision(0)
{
    pObject = NULL;
}

CYasioServerFactory::~CYasioServerFactory()
{
}

unsigned int CYasioServerFactory::AddRef()
{
#ifdef WIN32
    return ::InterlockedIncrement(&m_lRefCount);
#else
    return __sync_fetch_and_add(&m_lRefCount, 1);
#endif
}

unsigned int CYasioServerFactory::Release()
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
