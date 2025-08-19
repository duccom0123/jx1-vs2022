// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
#include "stdafx.h"
#include "YasioClient.h"
#include "KSG_EncodeDecode.h"
#include <chrono>


CYasioClient::CYasioClient(unsigned int maxFreeBuffers, unsigned int bufferSize, int mSocketNo) : CBuffer::Allocator(bufferSize, maxFreeBuffers), m_port(0), m_bConnected(false), m_bShutdown(false), m_SocketNo(mSocketNo), m_SocketState(STATE_NONE), m_uKeyMode(0), m_uServerKey(0), m_uClientKey(0), m_transport(nullptr)
{
    // Initialize yasio service
    m_ioService = std::make_unique<yasio::io_service>();

    // Setup event callback
    m_ioService->set_option(yasio::YOPT_S_EVENT_CB, [this](yasio::inet::event_ptr &&event) { OnYasioEvent(std::move(event)); });
}

CYasioClient::~CYasioClient()
{
    try
    {
        StopConnections();
    }
    catch (...)
    {
    }
}

void CYasioClient::Connect(const std::string &addressToConnectServer, unsigned short portToConnectServer)
{
    m_address = addressToConnectServer;
    m_port = portToConnectServer;
}

bool CYasioClient::StartConnections()
{
    CCriticalSection::Owner lock(m_criticalSection);

    if (m_bConnected)
        return false;

    try
    {
        // Configure yasio channel
        yasio::inet::io_hostent host(m_address.c_str(), m_port);
        m_ioService->set_option(yasio::YOPT_C_REMOTE_HOST, 0, host.host_.c_str());
        m_ioService->set_option(yasio::YOPT_C_REMOTE_PORT, 0, host.port_);

        // Set TCP no delay
        m_ioService->set_option(yasio::YOPT_S_TCP_KEEPALIVE, 0, 1, 60, 1);
        m_ioService->set_option(yasio::YOPT_C_LFBFD_PARAMS, 0, 65536, -1, 0, 0, 0);

        // Start yasio service
        m_ioService->start([this](yasio::inet::event_ptr &&event) { OnYasioEvent(std::move(event)); });

        // Open connection
        m_ioService->open(0, yasio::YCK_TCP_CLIENT);

        m_SocketState = STATE_START;

        // Start worker thread
        Start();

        return true;
    }
    catch (...)
    {
        return false;
    }
}

void CYasioClient::StopConnections()
{
    CCriticalSection::Owner lock(m_criticalSection);

    if (m_ioService && !m_bShutdown)
    {
        m_bShutdown = true;
        m_bConnected = false;

        // Close yasio service
        m_ioService->close(0);
        m_ioService->stop();

        m_SocketState = STATE_STOP;
        OnStopConnections();
    }
}

void CYasioClient::InitiateShutdown()
{
    StopConnections();
    OnShutdownInitiated();
}

void CYasioClient::WaitForShutdownToComplete(int isCleartheThread)
{
    if (isCleartheThread)
        Terminate();
    else
    {
        InitiateShutdown();
        Terminate();
    }
}

int CYasioClient::Run()
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
        StopConnections();
    }
    return 0;
}

void CYasioClient::Write(const char *pData, unsigned int dataLength)
{
    if (m_bConnected && dataLength > 0 && pData)
    {
        CBuffer *pBuffer = Allocate();
        pBuffer->AddData(pData, dataLength);
        Write(pBuffer);
        pBuffer->Release();
    }
}

bool CYasioClient::Write(CBuffer *pBuffer)
{
    if (!pBuffer || !m_bConnected)
        return false;

    unsigned int uDataLength = pBuffer->GetUsed();
    if (uDataLength == 0)
        return false;

    try
    {
        // Send data via yasio
        yasio::sbyte_buffer packet;
        packet.resize(uDataLength);
        memcpy(packet.data(), pBuffer->GetBuffer(), uDataLength);

        m_ioService->write(m_transport, std::move(packet));
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void CYasioClient::OnYasioEvent(yasio::inet::event_ptr &&event)
{
    switch (event->kind())
    {
    case yasio::YEK_ON_PACKET:
        HandleMessage(event);
        break;
    case yasio::YEK_ON_OPEN:
        HandleConnect(event);
        break;
    case yasio::YEK_ON_CLOSE:
        HandleClose(event);
        break;
    default:
        HandleError(event);
        break;
    }
}

void CYasioClient::HandleConnect(yasio::inet::event_ptr &event)
{
    if (event->status() == 0)   // Success
    {
        m_transport = event->transport();
        m_bConnected = true;

        // Wait and verify cipher if needed
        if (!WaitAndVerifyCipher())
        {
            StopConnections();
            return;
        }

        OnStartConnections();
        OnConnect();
    }
    else
    {
        OnError("Failed to connect to server");
        StopConnections();
    }
}

void CYasioClient::HandleMessage(yasio::inet::event_ptr &event)
{
    if (!m_bConnected)
        return;

    auto packet = event->packet();
    if (packet.empty())
        return;

    try
    {
        CBuffer *pBuffer = Allocate();
        pBuffer->AddData(reinterpret_cast<const char *>(packet.data()), packet.size());

        ReadCompleted(pBuffer);

        pBuffer->Release();
    }
    catch (...)
    {
        OnError("Error processing received data");
    }
}

void CYasioClient::HandleClose(yasio::inet::event_ptr &event)
{
    m_bConnected = false;
    m_transport = nullptr;
    OnClose();
    OnStopConnections();
}

void CYasioClient::HandleError(yasio::inet::event_ptr &event)
{
    std::string errorMsg = "Network error: " + std::to_string(event->status());
    OnError(errorMsg);
}

void CYasioClient::ReleaseBuffers()
{
    Flush();
}

void CYasioClient::OnError(const std::string &message)
{
    // Default implementation - derived classes can override
}

bool CYasioClient::WaitAndVerifyCipher()
{
    // For now, return true - cipher verification can be implemented later
    // This would involve waiting for and processing the ACCOUNT_BEGIN packet
    m_uKeyMode = 0;
    m_uServerKey = 0;
    m_uClientKey = 0;
    return true;
}
