// S3Relay.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include <objbase.h>
#include <initguid.h>  // Include only once in your application
#include "../../Multiserver/Heaven/Interface/IServer.h"
#include "../../Multiserver/Rainbow/Interface/IClient.h"

#include "S3Relay.h"
#include "Global.h"

#include "time.h"

#include "LogFile.h"
#include "../../Engine/Src/KGLog.h"
#include <locale.h>

static CLockSRSW gs_lockRecv;
static CLockSRSW gs_lockSend;
static size_t gs_recvdatasize = 0;
static size_t gs_senddatasize = 0;

static BOOL gs_initialized = FALSE;

static CLogFile gs_LogFile;

/////////////////////////////////////////////////


const char log_directory[] = "relay_log";
const size_t log_threshold = 1024 * 1024;


const UINT elapse_log = 3000;


/////////////////////////////////////////////////


void gTrackRecvData(const void* pData, size_t size)
{
	AUTOLOCKWRITE(gs_lockRecv);
	gs_recvdatasize += size;
}
void gTrackSendData(const void* pData, size_t size)
{
	AUTOLOCKWRITE(gs_lockSend);
	gs_senddatasize += size;
}
size_t gGetRecvDataSize()
{
	AUTOLOCKREAD(gs_lockRecv);
	return gs_recvdatasize;
}
size_t gGetSendDataSize()
{
	AUTOLOCKREAD(gs_lockSend);
	return gs_senddatasize;
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	Login(HWND, UINT, WPARAM, LPARAM);


std::_tstring g_rootloginname;
std::_tstring g_rootloginpswd;
BOOL g_interrupt = FALSE;

HWND g_mainwnd = NULL;
HWND g_listwnd = NULL;



static int  sLogProgram(BOOL bEnter)
{
	SYSTEMTIME systm;
	::GetLocalTime(&systm);
	TCHAR buff[256];
	int len = 0;

	if (bEnter)
		len = _stprintf(buff, "\r\n*\r\n***** S3Relay Start (@ %04d/%02d/%02d %02d:%02d:%02d.%03d) >>>>>\r\n*\r\n\r\n",
			systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds);
	else
		len = _stprintf(buff, "\r\n*\r\n***** S3Relay Stop (@ %04d/%02d/%02d %02d:%02d:%02d.%03d) <<<<<\r\n*\r\n\r\n",
			systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond, systm.wMilliseconds);

	gs_LogFile.TraceLog(buff, len * sizeof(TCHAR));
	return 0;
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
	COORD       BufferSize = { 120, 300 };
	SMALL_RECT  WinRect = { 0, 0, BufferSize.X - 1, 27 };

	hStdin = ::GetStdHandle(STD_INPUT_HANDLE);
	LOG_PROCESS_ERROR(hStdin != INVALID_HANDLE_VALUE);

	hStdount = ::GetStdHandle(STD_OUTPUT_HANDLE);
	LOG_PROCESS_ERROR(hStdount != INVALID_HANDLE_VALUE);

	// disable quick edit mode of console. 
	// because quick edit maybe lock stdout
	nRetCode = ::GetConsoleMode(hStdin, &dwMode);
	LOG_PROCESS_ERROR(nRetCode);

	dwMode &= ~ENABLE_QUICK_EDIT_MODE;  //disable QUICK_EDIT MODE

	nRetCode = ::SetConsoleMode(hStdin, dwMode);
	LOG_PROCESS_ERROR(nRetCode);

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
	rlimit sLimit = { -1, -1 };

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


int main(int argc, char* argv[])
{
	KGLOG_PARAM LogParam = {
		"logs",
		"relay",
		"",
		(KGLOG_OPTIONS)(LOG_OPTION_FILE | LOG_OPTION_CONSOLE),
		10000 * 10
	};
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


	nRetCode = _SetRuntimeEnvironment();

	if (nRetCode == 0)
	{
		KGLogUnInit(NULL);
		return 0;
	}
	gInitialize();

	MSG msg;
	KGLogPrintf(LOG_INFO, "[relay] start success\n");

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




	return 1;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case IDM_CLEAR_TRACE:
			::SendMessage(g_listwnd, LB_RESETCONTENT, 0, 0);
			break;
		case IDM_UPDATE_TRACE:

			break;

		case IDM_TRACE_HOSTSERVER: g_HostServer.TraceInfo(); break;
		case IDM_TRACE_CHATSERVER: g_ChatServer.TraceInfo(); break;
		case IDM_TRACE_TONGSERVER: g_TongServer.TraceInfo(); break;
		case IDM_TRACE_RELAYSERVER: g_RelayServer.TraceInfo(); break;

		case IDM_TRACE_ROOTCENTER: g_RootCenter.TraceInfo(); break;
		case IDM_TRACE_GATEWAYCENTER: g_GatewayCenter.TraceInfo(); break;
		case IDM_TRACE_DBROLECENTER: g_DBCenter.TraceInfo(); break;
		case IDM_TRACE_RELAYCENTER: g_RelayCenter.TraceInfo(); break;

		case IDM_TRACE_PLAYER_DETAIL: g_HostServer.TracePlayer(); break;
		case IDM_TRACE_PLAYER:
			KGLogPrintf(LOG_INFO,"message: online player total: %d", g_HostServer.GetPlayerCount());
			break;

		case IDM_TRACE_FRIENDPLAYER:
			KGLogPrintf(LOG_INFO,"message: friend player total: %d", g_FriendMgr.GetPlayerCount());
			break;

		case IDM_TRACE_FRIENDAFQS:
			KGLogPrintf(LOG_INFO,"message: friend AFQS: %d", g_FriendMgr.GetAFQSize());
			break;

		case IDM_TRACE_STATUS:
			KGLogPrintf(LOG_INFO,"message: [0] RootClient: %d, GatewayClient: %d, DBRoleClient: %d",
				g_RootCenter.GetClientCount(), g_GatewayCenter.GetClientCount(), g_DBCenter.GetClientCount());
			KGLogPrintf(LOG_INFO,"message: [1] RelayClient: %d, RelayConnect: %d",
				g_RelayCenter.GetClientCount(), g_RelayServer.GetConnectCount());
			KGLogPrintf(LOG_INFO,"message: [2] HostConnect: %d, ChatConnect: %d, TongConnect: %d",
				g_HostServer.GetConnectCount(), g_ChatServer.GetConnectCount(), g_TongServer.GetConnectCount());
			break;
		case IDM_TRACE_SOCKTHREAD:
			g_threadSock.TraceInfo();
			break;
		case IDM_TRACE_DATASIZE:
			KGLogPrintf(LOG_INFO,"message: recvsum: %ld B, sendsum: %ld B", gGetRecvDataSize(), gGetSendDataSize());
			break;
		case IDM_TRACE_MAC:
		{ {
				std::_tstring descLocal;
				{ {
						size_t lenLocal = 0;
						const UCHAR* macLocal = gGetHostMac(local_adapt, &lenLocal);
						for (int i = 0; i < lenLocal; i++)
						{
							char buff[3];
							_stprintf(buff, "%02X", macLocal[i]);
							if (i > 0)
								descLocal += '-';
							descLocal += buff;
						}
					}}
				std::_tstring descGlobal;
				{ {
						size_t lenGlobal = 0;
						const UCHAR* macGlobal = gGetHostMac(global_adapt, &lenGlobal);
						for (int i = 0; i < lenGlobal; i++)
						{
							char buff[3];
							_stprintf(buff, "%02X", macGlobal[i]);
							if (i > 0)
								descGlobal += '-';
							descGlobal += buff;
						}
					}}

				KGLogPrintf(LOG_INFO,"message: localMAC: %s, globalMAC: %s\n", descLocal.c_str(), descGlobal.c_str());
			}}
		break;
		case IDM_TRACE_IP:
			KGLogPrintf(LOG_INFO,"message: localIP: 0x%08X (%s), globalIP: 0x%08X (%s)", gGetHostIP(local_adapt), std::_tstring(_ip2a(gGetHostIP(local_adapt))).c_str(), gGetHostIP(global_adapt), std::_tstring(_ip2a(gGetHostIP(global_adapt))).c_str());
			break;
		case IDM_STARTUP:
			if (gs_initialized)
				KGLogPrintf(LOG_INFO,"waring: already Initialized");
			else
			{
				KGLogPrintf(LOG_INFO,"Initializing ...");
				if (gInitialize())
				{
					gs_initialized = TRUE;
					KGLogPrintf(LOG_INFO,"Initialize OK");
				}
				else
				{
					gUninitialize();
					KGLogPrintf(LOG_INFO,"error: Initialize FAIL");
				}
			}
			break;
		case IDM_SHUTDOWN:
			if (!gs_initialized)
			{
				KGLogPrintf(LOG_INFO,"waring: already Uninitialized");
				gUninitialize();
			}
			else
			{
				KGLogPrintf(LOG_INFO,"Uninitializing ...");
				gUninitialize();
				gs_initialized = FALSE;
				KGLogPrintf(LOG_INFO,"Uninitialize OK");
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		//auto shutdown
		::SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_SHUTDOWN, 0), NULL);
		//if (g_nShowTime)
		//	KillTimer(hWnd, g_nShowTime);
		//gShowTrace();
		//gShowTrace();

		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		if (::MessageBox(hWnd, "Are you sure ?", "warning", MB_YESNO) == IDYES)
		{
			DestroyWindow(hWnd);
		}
		break;
	case WM_SIZE:
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		MoveWindow(g_listwnd, 0, 0, rc.right, rc.bottom, TRUE);
	}
	break;
	case WM_TIMER:
		switch (wParam)
		{
		case timer_log: break;
		case timer_rootclient:		gReOneClient(oneclient_root); break;
		case timer_gatewayclient:	gReOneClient(oneclient_gateway); break;
		case timer_dbclient:		gReOneClient(oneclient_db); break;
		case timer_friendudtdb: gFriendTimingUpdateDB(); break;
		default: break;
		}
		break;
	case UM_CONSIGNCLIENTSD:
	{ {
			IClient* pClient = (IClient*)wParam;
			if (pClient)
			{
				//pClient->Shutdown();
				pClient->Cleanup();
				pClient->Release();
			}
		}}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

// Mesage handler for login box.
LRESULT CALLBACK Login(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
	{ {
			TCHAR buffer[_MAX_PATH];
			switch (wParam)
			{
			case IDOK:
				::GetDlgItemText(hDlg, IDC_EDIT_NAME, buffer, _MAX_PATH);
				g_rootloginname = buffer;
				::GetDlgItemText(hDlg, IDC_EDIT_PSWD, buffer, _MAX_PATH);
				g_rootloginpswd = buffer;
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			case IDCANCEL:
				g_interrupt = TRUE;
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
		}}
	break;
	}
	return FALSE;
}

HWND gGetMainWnd()
{
	return g_mainwnd;
}

HWND gGetListBox()
{
	return g_listwnd;
}
