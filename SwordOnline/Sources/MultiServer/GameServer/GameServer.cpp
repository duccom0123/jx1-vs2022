// GameServer.cpp : Defines the entry point for the console application.
//

/*#ifdef _STANDALONE
#ifndef __linux
//#include <winsock2.h>
#endif
#endif*/
#include "StdAfx.h"
#include "KSOServer.h"

extern KSwordOnLineSever g_SOServer;

int g_nPort = 0;
int main(int argc, char* argv[])
{
    SetConsoleCP(65001);         // Input UTF-8
    SetConsoleOutputCP(65001);   // Output UTF-8

    // Thiết lập môi trường cơ bản
    setlocale(LC_ALL, "en_US.UTF-8");   // Thiết lập locale UTF-8 cụ thể

	BOOL bRunning = TRUE;
	if (argc == 2)
	{
		g_nPort = atoi(argv[1]);
	}

	if (!g_SOServer.Init())
		return 0;

	while(bRunning)
	{
		bRunning = g_SOServer.Breathe();
	}

	g_SOServer.Release();
	return 1;
}

#ifdef _STANDALONE
extern "C"
{
void lua_outerrmsg(const char * szerrmsg)
{
	fprintf(stderr, szerrmsg);
}
};
#endif