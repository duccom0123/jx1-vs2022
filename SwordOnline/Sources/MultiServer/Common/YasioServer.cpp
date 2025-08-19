// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
#include "stdafx.h"
#include "YasioServer.h"
#include <chrono>
#include "../../Engine/Src/KGLog.h"

CYasioConnection::CYasioConnection(CYasioServer *server, yasio::inet::io_transport *transport, int connectionId) : m_server(server), m_transport(transport), m_connectionId(connectionId), m_bConnected(true), m_refCount(1)
{
}

CYasioConnection::~CYasioConnection()
{
}

void CYasioConnection::Close()
{
    CCriticalSection::Owner lock(m_criticalSection);
    if (m_bConnected && m_transport)
    {
        m_bConnected = false;
        if (m_server && m_server->m_ioService)
        {
            m_server->m_ioService->close(m_transport);
        }
    }
}

bool CYasioConnection::Write(const char *pData, unsigned int dataLength)
{
    if (!m_bConnected || !m_transport || !pData || dataLength == 0)
        return false;

    try
    {
        yasio::sbyte_buffer packet;
        packet.resize(dataLength);
        memcpy(packet.data(), pData, dataLength);

        if (m_server && m_server->m_ioService)
        {
            m_server->m_ioService->write(m_transport, std::move(packet));
            return true;
        }
    }
    catch (...)
    {
        return false;
    }
    return false;
}

bool CYasioConnection::Write(CBuffer *pBuffer)
{
    if (!pBuffer || !m_bConnected)
        return false;

    unsigned int dataLength = pBuffer->GetUsed();
    if (dataLength == 0)
        return false;

    return Write(reinterpret_cast<const char *>(pBuffer->GetBuffer()), dataLength);
}

std::string CYasioConnection::GetRemoteAddress() const
{
    if (m_transport)
    {
        // Yasio không có remote_endpoint trực tiếp trên channel
        // Cần lưu thông tin này khi kết nối được tạo
        return "";
    }
    return "";
}

unsigned short CYasioConnection::GetRemotePort() const
{
    if (m_transport)
    {
        // Tương tự như trên
        return 0;
    }
    return 0;
}

void CYasioConnection::AddRef()
{
#ifdef WIN32
    InterlockedIncrement(&m_refCount);
#else
    __sync_fetch_and_add(&m_refCount, 1);
#endif
}

void CYasioConnection::Release()
{
#ifdef WIN32
    if (InterlockedDecrement(&m_refCount) <= 0)
#else
    if (__sync_fetch_and_sub(&m_refCount, 1) <= 1)
#endif
    {
        delete this;
    }
}

/*
 * CYasioServer Implementation
 */
CYasioServer::CYasioServer(unsigned int maxFreeBuffers, unsigned int bufferSize) : CBuffer::Allocator(bufferSize, maxFreeBuffers), m_listenPort(0), m_bListening(false), m_bShutdown(false), m_nextConnectionId(1)
{
    // Initialize yasio service
    m_ioService = std::make_unique<yasio::io_service>();

    // Setup event callback
    // m_ioService->set_option(yasio::YOPT_S_EVENT_CB, [this](yasio::inet::event_ptr &&event) { OnYasioEvent(std::move(event)); });
}

CYasioServer::~CYasioServer()
{
    try
    {
        StopListening();
    }
    catch (...)
    {
    }
}

bool CYasioServer::StartListening(const std::string &address, unsigned short port)
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    if (m_bListening)
        return false;

    try
    {
        m_listenAddress = address;
        m_listenPort = port;

        // Configure yasio server
        m_ioService->set_option(yasio::YOPT_C_LOCAL_HOST, 0, address.c_str());
        m_ioService->set_option(yasio::YOPT_C_LOCAL_PORT, 0, port);

        // Set server options
        m_ioService->set_option(yasio::YOPT_S_TCP_KEEPALIVE, 0, 1, 60, 1);
        m_ioService->set_option(yasio::YOPT_C_LFBFD_PARAMS, 0, 65536, -1, 0, 0, 0);

        // Start yasio service
        m_ioService->start([this](yasio::inet::event_ptr &&event) { OnYasioEvent(std::move(event)); });

        // Start listening
        m_ioService->open(0, yasio::YCK_TCP_SERVER);

        m_bListening = true;
        m_bShutdown = false;

        // Start worker thread
        Start();

        OnStartListening();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void CYasioServer::StopListening()
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    if (m_ioService && m_bListening)
    {
        m_bShutdown = true;
        m_bListening = false;

        // Close all connections
        CloseAllConnections();

        // Stop yasio service
        m_ioService->close(0);
        m_ioService->stop();

        OnStopListening();
    }
}

int CYasioServer::Run()
{
    try
    {
        while (!m_bShutdown)
        {
            if (m_ioService)
            {
                // Dispatch yasio events
                m_ioService->dispatch(10);   // 10ms timeout
            }

            // Sleep briefly to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    catch (...)
    {
        StopListening();
    }
    return 0;
}

CYasioConnection *CYasioServer::GetConnection(int connectionId)
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    auto it = m_connections.find(connectionId);
    if (it != m_connections.end())
    {
        return it->second.get();
    }
    return nullptr;
}

void CYasioServer::CloseConnection(int connectionId)
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    auto it = m_connections.find(connectionId);
    if (it != m_connections.end())
    {
        it->second->Close();
        m_connections.erase(it);
    }
}

void CYasioServer::CloseAllConnections()
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    for (auto &pair : m_connections)
    {
        pair.second->Close();
    }
    m_connections.clear();
}

bool CYasioServer::WriteToConnection(int connectionId, const char *pData, unsigned int dataLength)
{
    CYasioConnection *connection = GetConnection(connectionId);
    if (connection)
    {
        return connection->Write(pData, dataLength);
    }
    return false;
}

bool CYasioServer::WriteToConnection(int connectionId, CBuffer *pBuffer)
{
    CYasioConnection *connection = GetConnection(connectionId);
    if (connection)
    {
        return connection->Write(pBuffer);
    }
    return false;
}

void CYasioServer::BroadcastToAll(const char *pData, unsigned int dataLength)
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    for (auto &pair : m_connections)
    {
        pair.second->Write(pData, dataLength);
    }
}

void CYasioServer::BroadcastToAll(CBuffer *pBuffer)
{
    if (!pBuffer)
        return;

    unsigned int dataLength = pBuffer->GetUsed();
    if (dataLength == 0)
        return;

    BroadcastToAll(reinterpret_cast<const char *>(pBuffer->GetBuffer()), dataLength);
}

size_t CYasioServer::GetConnectionCount() const
{
    CCriticalSection::Owner lock(m_connectionsMutex);
    return m_connections.size();
}

void CYasioServer::OnYasioEvent(yasio::inet::event_ptr &&event)
{
    switch (event->kind())
    {
    case yasio::YEK_ON_PACKET:
        HandleMessage(event);
        break;
    case yasio::YEK_ON_OPEN:
        HandleAccept(event);
        break;
    case yasio::YEK_ON_CLOSE:
        HandleClose(event);
        break;
    default:
        HandleError(event);
        break;
    }
}

void CYasioServer::HandleAccept(yasio::inet::event_ptr &event)
{
    if (event->status() == 0)   // Success
    {
        auto transport = event->transport();
        if (transport)
        {
            AddConnection(transport);
        }
    }
    else
    {
        OnError("Failed to accept client connection");
    }
}

void CYasioServer::HandleMessage(yasio::inet::event_ptr &event)
{
    auto transport = event->transport();
    if (!transport)
        return;

    CYasioConnection *connection = FindConnectionByTransport(transport);
    if (!connection)
        return;

    auto packet = event->packet();
    if (packet.empty())
        return;

    try
    {
        CBuffer *pBuffer = Allocate();
        pBuffer->AddData(reinterpret_cast<const char *>(packet.data()), packet.size());

        OnClientMessage(connection, pBuffer);

        pBuffer->Release();
    }
    catch (...)
    {
        OnError("Error processing client message");
    }
}

void CYasioServer::HandleClose(yasio::inet::event_ptr &event)
{
    auto transport = event->transport();
    if (transport)
    {
        CYasioConnection *connection = FindConnectionByTransport(transport);
        if (connection)
        {
            OnClientDisconnect(connection);
            RemoveConnection(connection->GetConnectionId());
        }
    }
}

void CYasioServer::HandleError(yasio::inet::event_ptr &event)
{
    std::string errorMsg = "Server network error: " + std::to_string(event->status());
    OnError(errorMsg);
}

void CYasioServer::AddConnection(yasio::inet::io_transport *transport)
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    int connectionId = m_nextConnectionId++;
    auto connection = std::make_shared<CYasioConnection>(this, transport, connectionId);

    m_connections[connectionId] = connection;

    OnClientConnect(connection.get());
}

void CYasioServer::RemoveConnection(int connectionId)
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    m_connections.erase(connectionId);
}

CYasioConnection *CYasioServer::FindConnectionByTransport(yasio::inet::io_transport *transport)
{
    CCriticalSection::Owner lock(m_connectionsMutex);

    for (auto &pair : m_connections)
    {
        if (pair.second->GetTransport() == transport)
        {
            return pair.second.get();
        }
    }
    return nullptr;
}

void CYasioServer::ReleaseBuffers()
{
    Flush();
}

void CYasioServer::OnError(const std::string &message)
{
    // Default implementation - derived classes can override
}
