// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
/********************************************************************
	created:	2025/08/19
	file base:	YasioClient
	file ext:	h
	author:		duccom0123

	purpose:	Yasio-based network client implementation
*********************************************************************/
#pragma once
#include "yasio/yasio.hpp"
#include "Buffer.h"
#include "CriticalSection.h"
#include "Thread.h"
#include "Cipher.h"
#include <string>
#include <memory>
#include <functional>

using namespace yasio;

/*
 * class CYasioClient
 */
class CYasioClient : protected CThread, protected CBuffer::Allocator
{
  public:
    virtual ~CYasioClient();

    void Connect(const std::string &addressToConnectServer, unsigned short portToConnectServer);
    bool StartConnections();
    void StopConnections();
    void InitiateShutdown();
    void WaitForShutdownToComplete(int isCleartheThread = false);
    void Write(const char *pData, unsigned int dataLength);
    bool Write(CBuffer *pBuffer);

    bool getSocketStates()
    {
        return m_bConnected;
    }
    void setSocketStates(bool ver)
    {
        m_bConnected = ver;
    }

  protected:
    CYasioClient(unsigned int maxFreeBuffers, unsigned int bufferSize, int mSocketNo);

    void ReleaseBuffers();
    bool WaitAndVerifyCipher();

    virtual int Run();

    // Cipher keys
    unsigned m_uKeyMode;
    unsigned m_uServerKey;
    unsigned m_uClientKey;

#pragma pack(1)
    struct
    {
        unsigned short wLen;
        ACCOUNT_BEGIN AccountBegin;
    } m_theSendAccountBegin;
#pragma pack()

  private:
    // Yasio networking
    std::unique_ptr<yasio::io_service> m_ioService;
    yasio::inet::io_transport *m_transport;

    // Virtual function callbacks
    virtual void OnStartConnections() {};
    virtual void OnStopConnections() {};
    virtual void OnShutdownInitiated() {};
    virtual void OnShutdownComplete() {};
    virtual void OnConnect() {};
    virtual void OnClose() {};
    virtual void ReadCompleted(CBuffer *pBuffer) = 0;
    virtual void OnWrite() {};
    virtual void OnError(const std::string &message);

    // Connection management
    std::string m_address;
    unsigned short m_port;
    volatile bool m_bConnected;
    volatile bool m_bShutdown;
    int m_SocketNo;

    CCriticalSection m_criticalSection;

    // Event handlers for yasio
    void OnYasioEvent(yasio::inet::event_ptr &&event);
    void HandleConnect(yasio::inet::event_ptr &event);
    void HandleMessage(yasio::inet::event_ptr &event);
    void HandleClose(yasio::inet::event_ptr &event);
    void HandleError(yasio::inet::event_ptr &event);

    enum SocketState
    {
        STATE_NONE,
        STATE_STOP,
        STATE_START,
    };
    int m_SocketState;
};
