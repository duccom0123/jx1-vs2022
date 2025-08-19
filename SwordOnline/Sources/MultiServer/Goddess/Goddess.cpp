// Goddess.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <objbase.h>
#include <initguid.h>
#include <winsock2.h>
#include <process.h>

#include "kprotocoldef.h"

#include "IDBRoleServer.h"

#include "console.h"
#include "Event.h"
#include "macro.h"
#include "Inifile.h"
#include "Utils.h"
#include "tstring.h"
#include "Library.h"
#include "Buffer.h"
#include "CriticalSection.h"

#include "IServer.h"
#include "HeavenInterface.h"
#include "NetworkAdapter.h"

#include "ClientNode.h"

#include <iostream>
#include <strstream>
#include <string>
#include <list>
#include <memory>
#include <time.h>

#include "../../Engine/Src/KGLog.h"
#include "../../Engine/Src/FilterText.h"
ITextFilter *g_libFilterText;
ITextFilter *g_fltRoleName;

const char log_directory[] = "goddess_log";
const size_t log_threshold = 1024 * 1024;

using namespace std;

using OnlineGameLib::Win32::Console::clrscr;
using OnlineGameLib::Win32::Console::setcolor;

/*
 * User data
 */

// Thay thế IOCP server bằng yasio server
static std::unique_ptr<CGameServer> g_pYasioServer = nullptr;

static CEvent g_hQuitEvent(NULL, true, false);
static CEvent g_hStartWorkEvent(NULL, true, false);

static size_t g_nMaxRoleCount = 3;

static unsigned short g_nServerPort = 5001;

static HANDLE g_hThread = NULL;

static CCriticalSection g_csPlayer;

typedef map<size_t, CClientNode *> stdPlayerMap;
static stdPlayerMap g_thePlayer;

static CPackager g_theRecv;
static CPackager g_theSend;

UINT g_nDBEngineLoop = 0;
UINT g_nServiceLoop = 0;

extern HWND hListOutput;

#define WM_CREATE_ENGINE WM_USER + 0x100
#define WM_ADD_CLIENT WM_USER + 0x200
#define WM_DEL_CLIENT WM_USER + 0x300

static size_t g_BackupSleepTime = 0;   //备份线程自动挂起时间
static DWORD g_BackupSpaceTime = 0;
static bool g_IsBackupSuspend = false;   //备份线程是否手工挂起
static DWORD g_BackupBeginTime = 0;

/*
 * Helper function
 */
BOOL CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool CreateDatabaseEngine(HWND hwnd);
void DestroyDatabaseEngine();

void LoadSetting();
void SaveSetting();

//==========监视的线程
DWORD WINAPI MonitorProc(LPVOID lpParameter)
{
    while (true)
    {
        Sleep(60 * 1000);   //1次
    }
    return 0;
}

DWORD WINAPI ThreadProcess(void *pParam)
{
    // Với yasio server, logic processing được handle internally
    // Thread này có thể được simplified hoặc removed
    // Yasio sử dụng event-driven architecture thay vì polling

    g_hStartWorkEvent.Wait();

    while (!g_hQuitEvent.Wait(0))
    {
        // Yasio server tự động handle connections và messages
        // Chỉ cần maintain service loop counter
        if (++g_nServiceLoop & 0x80000000)
        {
            g_nServiceLoop = 0;
        }

        // Yasio handles I/O internally, chỉ cần sleep nhẹ
        ::Sleep(10);
    }

    return 0L;
}
//////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    /*
	CTRL_C_EVENT        CTRL+C signal was received
	CTRL_BREAK_EVENT    CTRL+BREAK signal was received
	CTRL_CLOSE_EVENT    the user closes the console (either by clicking Close on the console window's window menu, or by clicking the End Task button command from Task Manager).
	*/
    //OnQuitSignal(0);

    return true;
}

static int _SetWin32ConsoleProperty()
{
    int nResult = false;
    int nRetCode = false;
    HANDLE hStdin = INVALID_HANDLE_VALUE;
    HANDLE hStdount = INVALID_HANDLE_VALUE;
    DWORD dwMode = 0;
    COORD BufferSize = {120, 300};
    SMALL_RECT WinRect = {0, 0, BufferSize.X - 1, 27};

    hStdin = ::GetStdHandle(STD_INPUT_HANDLE);
    LOG_PROCESS_ERROR(hStdin != INVALID_HANDLE_VALUE);

    hStdount = ::GetStdHandle(STD_OUTPUT_HANDLE);
    LOG_PROCESS_ERROR(hStdount != INVALID_HANDLE_VALUE);

    // set window size
    nRetCode = ::SetConsoleScreenBufferSize(hStdount, BufferSize);
    LOG_PROCESS_ERROR(nRetCode);

    nRetCode = ::SetConsoleWindowInfo(hStdount, true, &WinRect);
    LOG_PROCESS_ERROR(nRetCode);

    // set text color
    nRetCode = ::SetConsoleTextAttribute(hStdount, FOREGROUND_GREEN);
    LOG_PROCESS_ERROR(nRetCode);

    // get window close notify
    nRetCode = ::SetConsoleCtrlHandler(&ConsoleHandlerRoutine, TRUE);
    LOG_PROCESS_ERROR(nRetCode);

    //EXIT1:
    nResult = true;
EXIT0:
    return nResult;
}

#endif
////////////////////////////////////////////////////////////////////////////////
// @brief :Set runtime environment
// @return : true if successful, false if failed.
static int _SetRuntimeEnvironment()
{
    int nResult = false;
    int nRetCode = false;

#ifdef __GNUC__
    //rlimit rlimno = {5000, 5000};
    rlimit sLimit = {-1, -1};

    //nRetCode = setrlimit(RLIMIT_NOFILE, &rlimno);
    //LOG_PROCESS_ERROR(nRetCode == 0);

    // enable create coredump file
    nRetCode = setrlimit(RLIMIT_CORE, &sLimit);
    LOG_PROCESS_ERROR(nRetCode == 0);
#endif

    setlocale(LC_ALL, "");

#ifdef _WIN32
    _set_error_mode(_OUT_TO_MSGBOX);

    nRetCode = _SetWin32ConsoleProperty();
    LOG_PROCESS_ERROR(nRetCode);
#endif

    //EXIT1:
    nResult = true;
EXIT0:
    return nResult;
}

int main(int argc, char *argv[])
{

    KGLOG_PARAM LogParam = {"logs", "goddess", "", (KGLOG_OPTIONS)(LOG_OPTION_FILE | LOG_OPTION_CONSOLE), 10000 * 10};
    _tcscat(LogParam.szProcName, LogParam.szIdent);
    TCHAR szExePath[MAX_PATH + 1];

    if (szExePath)
    {
        memset(szExePath, 0, MAX_PATH);
        GetModuleFileName(NULL, szExePath, MAX_PATH);
        DWORD dwEndAt;
        dwEndAt = strlen(szExePath) - 1;
        while (dwEndAt >= 1)
        {
            if (szExePath[dwEndAt - 1] == '\\')
            {
                szExePath[dwEndAt] = 0;
                break;
            }
            dwEndAt--;
        }
    }

    TCHAR szLogDir[MAX_PATH + 1];
    _tcscpy(szLogDir, szExePath);

    _tcscat(szLogDir, LogParam.szProcName);
    ::CreateDirectory(szLogDir, NULL);

    int nRetCode = KGLogInit(LogParam, NULL);
    if (nRetCode == 0)
    {
        return 0;
    }

    //role name filter
    g_libFilterText = CreateTextFilter();
    g_fltRoleName = CreateTextFilter();
    if (!g_libFilterText || !g_fltRoleName)
    {
        MessageBox(NULL, "text filter's initing has failed", "error", MB_OK | MB_ICONERROR);
        return -1;
    }

    nRetCode = _SetRuntimeEnvironment();

    if (nRetCode == 0)
    {
        KGLogUnInit(NULL);
        return 0;
    }

    LoadSetting();

    CreateDatabaseEngine(0);
    MSG msg;
    KGLogPrintf(LOG_INFO, "[Goddess] start success\n");

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
        //if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    KGLogUnInit(NULL);
    DestroyDatabaseEngine();

    return 1;
}

bool CreateDatabaseEngine(HWND hwnd)
{
    if (!InitDBInterface(g_nMaxRoleCount))
    {
        ::MessageBox(NULL, "Setup dbserver is failed!", "Warning", MB_OK | MB_ICONSTOP);

        return false;
    }

    StartBackupTimer(g_BackupSleepTime, g_BackupBeginTime);   //开始运行备份线程
    ::SetWindowText(GetDlgItem(hwnd, IDC_LAB_BACKUP_STATUS), "Backup thread status: Running");

    /*
     * Migration sang yasio server thay vì IOCP
     */
    // Tạo yasio game server với các tham số phù hợp
    g_pYasioServer = CreateGameServer(1024, 20, 8192, 1024);

    if (!g_pYasioServer)
    {
        ::MessageBox(NULL, "Initialization failed! Cannot create yasio server", "Warning", MB_OK | MB_ICONSTOP);
        return false;
    }

    // Setup server handlers cho yasio
    SetupServerHandlers(
        g_pYasioServer.get(),
        [](CGameConnection *conn)
        {
            // Player connect handler - LUỒNG BẮT ĐẦU KHI CÓ KẾT NỐI MỚI
            if (conn)
            {
                // Tương tự ServerEventNotify với enumClientConnectCreate
                // 1. Tạo hoặc setup client data
                // 2. Add vào player management
                // 3. Log connection
                KGLogPrintf(LOG_INFO, "[Yasio] New connection established\n");

                // TODO: Thêm vào player map tương tự IOCP
                // CClientNode *pNode = CClientNode::AddNode(...)
                // g_thePlayer.insert(...)
                CClientNode *pNode = CClientNode::AddNode(conn, conn->GetConnectionId());

                if (pNode)
                {
                    CCriticalSection::Owner lock(g_csPlayer);

                    g_thePlayer.insert(stdPlayerMap::value_type(conn->GetConnectionId(), pNode));
                }
            }
        },
        [](CGameConnection *conn)
        {
            // Player disconnect handler - KHI NGẮT KẾT NỐI
            if (conn)
            {
                // Tương tự ServerEventNotify với enumClientConnectClose
                // 1. Remove from player map
                // 2. Cleanup resources
                // 3. Log disconnection
                KGLogPrintf(LOG_INFO, "[Yasio] Connection closed\n");
                CCriticalSection::Owner lock(g_csPlayer);
                stdPlayerMap::iterator it;
                if (g_thePlayer.end() != (it = g_thePlayer.find(conn->GetConnectionId())))
                {
                    g_thePlayer.erase(conn->GetConnectionId());
                }
            }
        },
        [](CGameConnection *conn, const void *data, unsigned int length)
        {
            // Player message handler - KHI NHẬN DATA
            if (conn && data && length > 0)
            {
                // Tương tự ThreadProcess loop
                // 1. Parse message protocol
                // 2. Route to appropriate handler
                // 3. Process game logic
                // 4. Send response if needed
                KGLogPrintf(LOG_INFO, "[Yasio] Received %d bytes\n", length);

                // Tìm CClientNode tương ứng với connection và gọi AppendData
                {
                    CCriticalSection::Owner lock(g_csPlayer);
                    stdPlayerMap::iterator it = g_thePlayer.find(conn->GetConnectionId());
                    if (it != g_thePlayer.end())
                    {
                        CClientNode *pNode = it->second;
                        if (pNode)
                        {
                            pNode->AppendData(data, length);
                        }
                    }
                }
            }
        });

    // Khởi động yasio server
    if (!StartGameServer(g_pYasioServer.get(), "0.0.0.0", g_nServerPort))
    {
        ::MessageBox(NULL, "Failed to start yasio server", "Warning", MB_OK | MB_ICONSTOP);
        return false;
    }

    DWORD dwThreadID = 0L;

    // Tạo thread cho yasio server processing
    g_hThread = ::CreateThread(NULL, 0, ThreadProcess, nullptr, 0, &dwThreadID);

    if (!g_hThread)
    {
        return false;
    }

    CClientNode::Start(nullptr);   // Yasio server tự quản lý connections, không cần pass server instance

    g_hStartWorkEvent.Set();

    return true;
}

void DestroyDatabaseEngine()
{
    CClientNode::End();

    g_hQuitEvent.Set();

    // Wait for thread to finish
    if (g_hThread)
    {
        DWORD result = ::WaitForSingleObject(g_hThread, 5000);
        if (result == WAIT_TIMEOUT)
        {
            ::TerminateThread(g_hThread, (DWORD)(-1));
        }
        SAFE_CLOSEHANDLE(g_hThread);
    }

    // Yasio server cleanup - đơn giản hơn IOCP
    if (g_pYasioServer)
    {
        StopGameServer(g_pYasioServer.get());
        g_pYasioServer.reset();   // Smart pointer tự động cleanup
    }

    ReleaseDBInterface();
}

void LoadSetting()
{
    CIniFile theConfigFile;

    _tstring sAppPath = GetAppFullPath(NULL);

    _tstring sConfigFile;

    sConfigFile = sAppPath + "Goddess.cfg";

    theConfigFile.SetFile(sConfigFile.c_str());

    g_nServerPort = theConfigFile.ReadInteger("Setting", "Port", 5001);

    g_nMaxRoleCount = theConfigFile.ReadInteger("Setting", "MaxRoleCount", 3);

    g_BackupSleepTime = theConfigFile.ReadInteger("Setting", "BackupSleepTime", 0);

    g_BackupSpaceTime = theConfigFile.ReadInteger("Setting", "BackupSpaceTime", 0);

    g_BackupBeginTime = theConfigFile.ReadInteger("Setting", "BackupBeginTime", 0);
}

void SaveSetting()
{
    CIniFile theConfigFile;

    _tstring sAppPath = GetAppFullPath(NULL);

    _tstring sConfigFile;

    sConfigFile = sAppPath + "Goddess.cfg";

    theConfigFile.SetFile(sConfigFile.c_str());

    theConfigFile.WriteInteger("Setting", "Port", g_nServerPort);

    theConfigFile.WriteInteger("Setting", "MaxRoleCount", g_nMaxRoleCount);

    theConfigFile.WriteInteger("Setting", "BackupSleepTime", g_BackupSleepTime);

    theConfigFile.WriteInteger("Setting", "BackupSpaceTime", g_BackupSpaceTime);

    theConfigFile.WriteInteger("Setting", "BackupBeginTime", g_BackupBeginTime);
}