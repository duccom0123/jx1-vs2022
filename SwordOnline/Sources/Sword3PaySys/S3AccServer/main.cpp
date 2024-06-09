#include "stdafx.h"

#include "stdlib.h"
#include "stdio.h"
#include "S3PDBSocketPool.h"
#include "S3PDBConnectionPool.h"
#include "GlobalFun.h"

#include <process.h>

#include "ServerPort.h"
#include "../../Engine/Src/KGLog.h"
#include <initguid.h> 
#define _WIN32_DCOM
#include "../../Multiserver/Heaven/Interface/IServer.h"

#include "adoid.h"     // ADO GUID's
#include "adoint.h"



#include "resource.h"

#include "time.h"

#include <locale.h>

DEFINE_GUID(IID_IServerFactory,
	0xa4984b66, 0x8cde, 0x4752, 0x9b, 0xee, 0xda, 0xcc, 0x6, 0xa6, 0x35, 0x48);
DEFINE_GUID(IID_IIOCPServer,
	0xfc6b562e, 0x89c2, 0x4fbc, 0xbf, 0x5f, 0x55, 0x3c, 0x9, 0xae, 0x93, 0xd8);

////////////////////////////////////////////////////////////////////////////////
void OnQuitSignal(int)
{
	KGLogPrintf(LOG_INFO, "[Paysys] Get quit signal!");
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
	OnQuitSignal(0);

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


typedef HRESULT(__stdcall* pfnCreateServerInterface)(
	REFIID	riid,
	void** ppv
	);

void __stdcall ServerEventNotify(
	LPVOID lpParam,
	const unsigned long& ulnID,
	const unsigned long& ulnEventType)
{
	S3PDBSocketPool* pSocketPool = (S3PDBSocketPool*)lpParam;
	switch (ulnEventType)
	{
	case enumClientConnectCreate:
		pSocketPool->AddUserClientID(ulnID);
		break;
	case enumClientConnectClose:
		pSocketPool->RemoveUserClientID(ulnID);
		break;
	}
}


int main(int argc, char* argv[])
{
	KGLOG_PARAM LogParam = {
		"logs",
		"paysys",
		"",
		(KGLOG_OPTIONS)(LOG_OPTION_FILE | LOG_OPTION_CONSOLE),
		10000 * 10
	};
	_tcscat(LogParam.szProcName, LogParam.szIdent);
	TCHAR szExePath[MAX_PATH + 1];
	KPIGetExePath(szExePath, MAX_PATH);

	TCHAR szLogDir[MAX_PATH + 1];
	_tcscpy(szLogDir, szExePath);

	_tcscat(szLogDir, LogParam.szProcName);
	::CreateDirectory(szLogDir, NULL);
	int nRetCode = KGLogInit(LogParam, NULL);
	if (nRetCode == 0)
	{
		return 0;
	}

	// -------------------- initialize  -------------------------- 
	nRetCode = _SetRuntimeEnvironment();

	if (nRetCode == 0)
	{
		KGLogUnInit(NULL);
		return 0;
	}

	TCHAR szINIPath[MAX_PATH + 1];
	_tcscpy(szINIPath, szExePath);
	_tcscat(szINIPath, "acc_setup.ini");
	TCHAR szPort[32];
	sprintf(szPort, "%d", ACCSVR_ACCOUNT_VERIFY_PORT);
	DWORD dwResult = KPIGetPrivateProfileString("Local",
		"port",
		szPort,
		szPort,
		31,
		szINIPath);
	short siPort = atoi(szPort);
	dwResult = KPIGetPrivateProfileString("Local",
		"maxconnect",
		"10",
		szPort,
		31,
		szINIPath);
	int snMaxPlayerCount = atoi(szPort);
	dwResult = KPIGetPrivateProfileString("Local",
		"precision",
		"1",
		szPort,
		31,
		szINIPath);
	int snPrecision = atoi(szPort);
	dwResult = KPIGetPrivateProfileString("Local",
		"maxbuffer",
		"15",
		szPort,
		31,
		szINIPath);
	int snMaxBuffer = atoi(szPort);
	dwResult = KPIGetPrivateProfileString("Local",
		"sizebuffer",
		"1048576",
		szPort,
		31,
		szINIPath);
	int snBufferSize = atoi(szPort);

	dwResult = KPIGetPrivateProfileString("Local",
		"timelog",
		"3000",
		szPort,
		31,
		szINIPath);
	int nTimelog = atoi(szPort);

	IServer* pServer = NULL;

	HMODULE hModule = ::LoadLibrary("heaven.dll");

	if (hModule)
	{
		pfnCreateServerInterface pFactroyFun = (pfnCreateServerInterface)GetProcAddress(hModule, "CreateInterface");

		IServerFactory* pServerFactory = NULL;

		if (SUCCEEDED(pFactroyFun(IID_IServerFactory, reinterpret_cast<void**>(&pServerFactory))))
		{
			pServerFactory->SetEnvironment(snMaxPlayerCount, snPrecision, snMaxBuffer, snBufferSize);

			pServerFactory->CreateServerInterface(IID_IIOCPServer, reinterpret_cast<void**>(&pServer));

			pServerFactory->Release();
		}
	}

	_tcscpy(szINIPath, szExePath);
	_tcscat(szINIPath, "DataBase.ini");

	dwResult = KPIGetPrivateProfileString("Local",
		"DBConnects",
		"5",
		szPort,
		31,
		szINIPath);
	int nDBConns = atoi(szPort);

	S3PDBConnectionPool* pConnectPool = S3PDBConnectionPool::Instance();
	S3PDBSocketPool* pSocketPool = S3PDBSocketPool::Instance();

	char* tempbuff = "Nomal exit !!!";

	if (pServer == NULL)
	{
		::MessageBox(NULL, "heaven Error!", "error", MB_OK | MB_ICONERROR);
		goto Exit0;
	}
	if (!pConnectPool->Init(szINIPath, "account", nDBConns))
	{
		::MessageBox(NULL, "Connect DB Error!", "error", MB_OK | MB_ICONERROR);
		goto Exit0;
	}

	pSocketPool->Start(pServer, snMaxPlayerCount);

	pServer->Startup();

	pServer->RegisterMsgFilter((LPVOID)pSocketPool, ServerEventNotify);
	pServer->OpenService(INADDR_ANY, siPort);

	MSG msg;
	KGLogPrintf(LOG_INFO, "[Paysys] start success\n");

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		//if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	pSocketPool->Stop();


Exit0:
	S3PDBSocketPool::ReleaseInstance();
	S3PDBConnectionPool::ReleaseInstance();

	if (pServer)
	{
		pServer->CloseService();
		pServer->Cleanup();
		pServer->Release();
	}

	if (hModule)
		::FreeLibrary(hModule);

	KGLogUnInit(NULL);

	return 0;
}
