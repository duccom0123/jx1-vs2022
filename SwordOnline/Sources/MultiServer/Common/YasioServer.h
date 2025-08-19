// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
/********************************************************************
	created:	2025/08/19
	file base:	YasioServer
	file ext:	h
	author:		duccom0123

	purpose:	Yasio-based network server implementation
*********************************************************************/
#pragma once
#include "yasio/yasio.hpp"
#include "Buffer.h"
#include "CriticalSection.h"
#include "Thread.h"
#include "Cipher.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <functional>

using namespace yasio;

/*
 * Forward declarations
 */
class CYasioServer;

/*
 * class CYasioConnection - Represents a client connection
 */
class CYasioConnection
{
  public:
    CYasioConnection(CYasioServer *server, yasio::inet::io_transport *transport, int connectionId);
    virtual ~CYasioConnection();

    // Connection management
    void Close();
    bool IsConnected() const
    {
        return m_bConnected;
    }
    int GetConnectionId() const
    {
        return m_connectionId;
    }
    yasio::inet::io_transport *GetTransport() const
    {
        return m_transport;
    }

    // Data sending
    bool Write(const char *pData, unsigned int dataLength);
    bool Write(CBuffer *pBuffer);

    // Connection info
    std::string GetRemoteAddress() const;
    unsigned short GetRemotePort() const;

    // Reference counting
    void AddRef();
    void Release();

  private:
    CYasioServer *m_server;
    yasio::inet::io_transport *m_transport;
    int m_connectionId;
    volatile bool m_bConnected;
    volatile long m_refCount;

    mutable CCriticalSection m_criticalSection;

    // No copying
    CYasioConnection(const CYasioConnection &);
    CYasioConnection &operator=(const CYasioConnection &);
};

/*
 * class CYasioServer
 */
class CYasioServer : protected CThread, protected CBuffer::Allocator
{
  public:
    virtual ~CYasioServer();

    // Server management
    bool StartListening(const std::string &address, unsigned short port);
    void StopListening();
    bool IsListening() const
    {
        return m_bListening;
    }

    // Connection management
    CYasioConnection *GetConnection(int connectionId);
    void CloseConnection(int connectionId);
    void CloseAllConnections();

    // Data sending to specific connection
    bool WriteToConnection(int connectionId, const char *pData, unsigned int dataLength);
    bool WriteToConnection(int connectionId, CBuffer *pBuffer);

    // Broadcast to all connections
    void BroadcastToAll(const char *pData, unsigned int dataLength);
    void BroadcastToAll(CBuffer *pBuffer);

    // Server info
    std::string GetListenAddress() const
    {
        return m_listenAddress;
    }
    unsigned short GetListenPort() const
    {
        return m_listenPort;
    }
    size_t GetConnectionCount() const;

  protected:
    CYasioServer(unsigned int maxFreeBuffers, unsigned int bufferSize);

    void ReleaseBuffers();

    virtual int Run();

    // Event callbacks - override these in derived classes
    virtual void OnStartListening() {};
    virtual void OnStopListening() {};
    virtual void OnClientConnect(CYasioConnection *connection) {};
    virtual void OnClientDisconnect(CYasioConnection *connection) {};
    virtual void OnClientMessage(CYasioConnection *connection, CBuffer *buffer) = 0;
    virtual void OnError(const std::string &message);

  private:
    // Yasio networking
    std::unique_ptr<yasio::io_service> m_ioService;

    // Server state
    std::string m_listenAddress;
    unsigned short m_listenPort;
    volatile bool m_bListening;
    volatile bool m_bShutdown;

    // Connection management
    std::unordered_map<int, std::shared_ptr<CYasioConnection>> m_connections;
    int m_nextConnectionId;
    mutable CCriticalSection m_connectionsMutex;

    // Yasio event handling
    void OnYasioEvent(yasio::inet::event_ptr &&event);
    void HandleAccept(yasio::inet::event_ptr &event);
    void HandleMessage(yasio::inet::event_ptr &event);
    void HandleClose(yasio::inet::event_ptr &event);
    void HandleError(yasio::inet::event_ptr &event);

    // Connection management helpers
    void AddConnection(yasio::inet::io_transport *transport);
    void RemoveConnection(int connectionId);
    CYasioConnection *FindConnectionByTransport(yasio::inet::io_transport *transport);

    friend class CYasioConnection;
};
