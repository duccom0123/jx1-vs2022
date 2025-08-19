// ***************************************************************************************
// Chuyển đổi sang yasio bởi duccom0123 19/08/2025
// ***************************************************************************************
/********************************************************************
	created:	2025/08/19
	file base:	NetworkAdapter
	file ext:	h
	author:		duccom0123

	purpose:	Adapter để migration từ SocketClient sang YasioClient
*********************************************************************/
#ifndef __INCLUDE_NETWORKADAPTER_H__
#define __INCLUDE_NETWORKADAPTER_H__
#pragma once


#ifdef _CLIENT

#pragma region Client
// Include yasio headers
#include "YasioClientStage.h"

// Type aliases for easy migration
using CGameClient = CYasioGameClient;
using CClientFactory = CYasioClientFactory;

// Direct client creation - C++ hiện đại approach
inline std::unique_ptr<CGameClient> CreateGameClient(unsigned int maxFreeBuffers = 100, unsigned int maxFreeBuffers_Cache = 50, unsigned int bufferSize_Cache = 8192, unsigned int bufferSize = 1024, int mSocketNo = 0)
{
    return std::make_unique<CYasioGameClient>(maxFreeBuffers, maxFreeBuffers_Cache, bufferSize_Cache, bufferSize, mSocketNo);
}

// Legacy raw pointer version (for backward compatibility only)
inline CGameClient *CreateGameClientLegacy(unsigned int maxFreeBuffers = 100, unsigned int maxFreeBuffers_Cache = 50, unsigned int bufferSize_Cache = 8192, unsigned int bufferSize = 1024, int mSocketNo = 0)
{
    return new CYasioGameClient(maxFreeBuffers, maxFreeBuffers_Cache, bufferSize_Cache, bufferSize, mSocketNo);
}

#pragma endregion
#else

#pragma region Server
// Include yasio server headers
#include "YasioServerStage.h"

// Type aliases for easy migration
using CGameServer = CYasioGameServer;
using CServerFactory = CYasioServerFactory;
using CGameConnection = CYasioGameConnection;

// Direct server creation - C++ hiện đại approach
inline std::unique_ptr<CGameServer> CreateGameServer(unsigned int maxFreeBuffers = 1000, unsigned int maxFreeBuffers_Cache = 500, unsigned int bufferSize_Cache = 8192, unsigned int bufferSize = 1024)
{
    return std::make_unique<CYasioGameServer>(maxFreeBuffers, maxFreeBuffers_Cache, bufferSize_Cache, bufferSize);
}

// Legacy raw pointer version (for backward compatibility only)
inline CGameServer *CreateGameServerLegacy(unsigned int maxFreeBuffers = 1000, unsigned int maxFreeBuffers_Cache = 500, unsigned int bufferSize_Cache = 8192, unsigned int bufferSize = 1024)
{
    return new CYasioGameServer(maxFreeBuffers, maxFreeBuffers_Cache, bufferSize_Cache, bufferSize);
}

// Server event handler types for easier migration
typedef std::function<void(CGameConnection *)> PlayerConnectHandler;
typedef std::function<void(CGameConnection *)> PlayerDisconnectHandler;
typedef std::function<void(CGameConnection *, const void *, unsigned int)> PlayerMessageHandler;

// Helper function to setup server event handlers
inline void SetupServerHandlers(CGameServer *server, PlayerConnectHandler onConnect = nullptr, PlayerDisconnectHandler onDisconnect = nullptr, PlayerMessageHandler onMessage = nullptr)
{
    if (server)
    {
        if (onConnect)
            server->SetConnectHandler(onConnect);
        if (onDisconnect)
            server->SetDisconnectHandler(onDisconnect);
        if (onMessage)
            server->SetMessageHandler(onMessage);
    }
}

// Utility functions for server management
inline bool StartGameServer(CGameServer *server, const std::string &address, unsigned short port)
{
    return server ? server->Startup(address, port) : false;
}

inline bool StopGameServer(CGameServer *server)
{
    return server ? server->Cleanup() : false;
}

inline size_t GetServerPlayerCount(CGameServer *server)
{
    return server ? server->GetPlayerCount() : 0;
}

inline void BroadcastToAllPlayers(CGameServer *server, const void *data, unsigned int length)
{
    if (server && data && length > 0)
    {
        server->BroadcastToAllPlayers(data, length);
    }
}

#pragma endregion

#endif   // _CLIENT


#endif
