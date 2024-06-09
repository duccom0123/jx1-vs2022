// Bishop.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "Application.h"


#include "../../Engine/Src/KGLog.h"


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


void KPIGetExePath(LPSTR lpExePath, DWORD dwSize)
{
	if (lpExePath)
	{
		memset(lpExePath, 0, dwSize);
		GetModuleFileName(NULL, lpExePath, dwSize);
		DWORD dwEndAt;
		dwEndAt = strlen(lpExePath) - 1;
		while (dwEndAt >= 1)
		{
			if (lpExePath[dwEndAt - 1] == '\\')
			{
				lpExePath[dwEndAt] = 0;
				break;
			}
			dwEndAt--;
		}
	}
}
int main(int argc, char* argv[])
{
	KGLOG_PARAM LogParam = {
		"logs",
		"bishop",
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

	CBishopApp app;

	int nRet = app.Run();

Exit0:

	KGLogUnInit(NULL);

	return 0;
}
