// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
/********************************************************************
	created:	2025/08/19
	file base:	YasioServerStage
	file ext:	h
	author:		duccom0123

	purpose:	Game server implementation using yasio
*********************************************************************/
#ifndef __INCLUDE_YASIOSERVERSTAGE_H__
#define __INCLUDE_YASIOSERVERSTAGE_H__
#pragma once

#include "YasioServer.h"
#include "Buffer.h"
#include "Cipher.h"
#include <functional>

/*
 * Forward declarations
 */
class CYasioGameServer;

/*
 * class CYasioGameConnection - Game-specific connection
 */
class CYasioGameConnection : public CYasioConnection
{
  public:
    CYasioGameConnection(CYasioServer *server, yasio::inet::io_transport *transport, int connectionId);
    virtual ~CYasioGameConnection();

    // Game-specific functionality
    bool SendPackToClient(const void *const pData, const unsigned int &datalength);
    const void *GetPackFromClient(unsigned int &datalength);
    void setBufferEmpty();

    // Cipher support
    void SetCipherKeys(unsigned int serverKey, unsigned int clientKey);
    unsigned int GetServerKey() const
    {
        return m_uServerKey;
    }
    unsigned int GetClientKey() const
    {
        return m_uClientKey;
    }

    // Authentication state
    bool IsAuthenticated() const
    {
        return m_bAuthenticated;
    }
    void SetAuthenticated(bool authenticated)
    {
        m_bAuthenticated = authenticated;
    }

    // Player data
    void SetPlayerData(void *playerData)
    {
        m_playerData = playerData;
    }
    void *GetPlayerData() const
    {
        return m_playerData;
    }

    // Protocol handling
    unsigned int GetMessageSize(const CBuffer *pBuffer) const;
    unsigned int GetMinimumMessageSize() const;

  protected:
    CBuffer *ProcessDataStream(CBuffer *pBuffer);
    void ProcessCommand(const CBuffer *pBuffer);

  private:
    // Cipher keys
    unsigned int m_uKeyMode;
    unsigned int m_uServerKey;
    unsigned int m_uClientKey;

    // Authentication and player data
    bool m_bAuthenticated;
    void *m_playerData;

    // Protocol buffers
    CBuffer::Allocator m_theCacheAllocator;
    CBuffer *m_pReadBuffer;
    CBuffer *m_pRecvBuffer;
    CCriticalSection m_csReadAction;

    // Account begin packet for cipher handshake
#pragma pack(1)
    struct
    {
        unsigned short wLen;
        ACCOUNT_BEGIN AccountBegin;
    } m_theSendAccountBegin;
#pragma pack()

    friend class CYasioGameServer;
};

/*
 * class CYasioGameServer
 */
class CYasioGameServer : public CYasioServer
{
  public:
    // Server management
    bool Startup(const std::string &address, unsigned short port);
    bool Cleanup();
    bool Shutdown();

    // Player management
    size_t GetPlayerCount() const;
    CYasioGameConnection *GetPlayerConnection(int connectionId);
    void DisconnectPlayer(int connectionId);
    void BroadcastToAllPlayers(const void *pData, unsigned int dataLength);

    // Message handling callbacks
    typedef std::function<void(CYasioGameConnection *, const void *, unsigned int)> MessageHandler;
    void SetMessageHandler(MessageHandler handler)
    {
        m_messageHandler = handler;
    }

    // Connection event callbacks
    typedef std::function<void(CYasioGameConnection *)> ConnectionHandler;
    void SetConnectHandler(ConnectionHandler handler)
    {
        m_connectHandler = handler;
    }
    void SetDisconnectHandler(ConnectionHandler handler)
    {
        m_disconnectHandler = handler;
    }

    /*
	 *  IUnknown COM Interface Methods
	 */
    bool QueryInterface(void **ppv);
    unsigned int AddRef();
    unsigned int Release();

  public:
    CYasioGameServer(unsigned int maxFreeBuffers = 1000, unsigned int maxFreeBuffers_Cache = 500, unsigned int bufferSize_Cache = 8192, unsigned int bufferSize = 1024);
    virtual ~CYasioGameServer();

  protected:
    // Override YasioServer callbacks
    virtual void OnStartListening() override;
    virtual void OnStopListening() override;
    virtual void OnClientConnect(CYasioConnection *connection) override;
    virtual void OnClientDisconnect(CYasioConnection *connection) override;
    virtual void OnClientMessage(CYasioConnection *connection, CBuffer *buffer) override;
    virtual void OnError(const std::string &message) override;

  private:
    // Reference counting
    long m_lRefCount;

    // Event handlers
    MessageHandler m_messageHandler;
    ConnectionHandler m_connectHandler;
    ConnectionHandler m_disconnectHandler;

    // Helper methods
    void SendCipherHandshake(CYasioGameConnection *gameConnection);
    CYasioGameConnection *CastToGameConnection(CYasioConnection *connection);

    /*
	 * No copies do not implement
	 */
    CYasioGameServer(const CYasioGameServer &rhs);
    CYasioGameServer &operator=(const CYasioGameServer &rhs);
};

/*
 * CYasioServerFactory
 */
class CYasioServerFactory
{
  public:
    /*
	 *  IUnknown COM Interface Methods
	 */
    unsigned int AddRef();
    unsigned int Release();
    CYasioGameServer *pObject;

  public:
    CYasioServerFactory();
    virtual ~CYasioServerFactory();

  private:
    long m_lRefCount;
    unsigned int m_bufferSize;
    unsigned int m_MaxPlayerCount;
    unsigned int m_nPrecision;
};

#endif
