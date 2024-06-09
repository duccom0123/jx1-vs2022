

#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>


#ifdef __GNUC__
#include <unistd.h>
#endif
#include "KWin32.h"
#include "KGLog.h"
#include "KMutex.h"
#include "KThread.h"
#include <fcntl.h>


static int              gs_nPriorityMask    = 0xff;
static KGLOG_PARAM      gs_LogParam = { 0 };
static int              gs_nLogFile = -1;
int                     gs_nCurrentLine = 0;
KMutex                  gs_Lock;
static char             gs_szLogFileNamePrefix[PATH_MAX] = { 0 };
static int              gs_nIsFirstTimeToWriteLog = true;
struct tm               gs_tmLastChangeFile = { 0 }; 



#if (defined(_MSC_VER) || defined(__ICL))
#define _LOG_OPENFILEFLAG_   (O_CREAT | O_APPEND | O_WRONLY | O_TRUNC | O_BINARY)
#define _LOG_OPENFILEMODE_   (0x0100 | 0x0080)
#else
#define _LOG_OPENFILEFLAG_   (O_CREAT | O_APPEND | O_WRONLY | O_TRUNC)
#define _LOG_OPENFILEMODE_   (S_IREAD | S_IWRITE |  S_IRGRP | S_IROTH)
#endif

static int _ChangeLogFile()
{
    int nResult  = false;
    int nRetCode = false;
	time_t tmtNow = 0;

    char szLogFullPath[PATH_MAX];
    char szLogFileName[PATH_MAX];

    nRetCode = mkdir(gs_LogParam.szPath);
    if ((nRetCode == -1) && (errno != EEXIST))
    {
        fprintf(stderr, "Log: mkdir(\"%s\") failed = %s\n", gs_LogParam.szPath, strerror(errno));
        goto EXIT0;
    }

    nRetCode = mkdir(gs_szLogFileNamePrefix);
    if ((nRetCode == -1) && (errno != EEXIST))
    {
        fprintf(stderr, "Log: mkdir(\"%s\") failed = %s\n", gs_szLogFileNamePrefix, strerror(errno));
        goto EXIT0;
    }

    if (gs_nLogFile != -1)
    {
        close(gs_nLogFile);
        gs_nLogFile = -1;
    }
    
    struct tm tmNow; 

    tmtNow = time(NULL);

    localtime_r(&tmtNow, &tmNow);

    nRetCode = snprintf(
        szLogFullPath, sizeof(szLogFullPath) - 1,
        "%s%d_%2.2d_%2.2d/",
        gs_szLogFileNamePrefix, 
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday
    );
    ASSERT((nRetCode != -1) && (nRetCode < (sizeof(szLogFullPath) - 1)));
    szLogFullPath[sizeof(szLogFullPath) - 1] = '\0';

    nRetCode = mkdir(szLogFullPath);
    if ((nRetCode == -1) && (errno != EEXIST))
    {
        fprintf(stderr, "Log: mkdir(\"%s\") failed = %s\n", szLogFullPath, strerror(errno));
    }
    
    nRetCode = snprintf(
        szLogFileName, sizeof(szLogFileName) - 1,
        "%s%s_%d_%2.2d_%2.2d_%2.2d_%2.2d_%2.2d.log", 
        szLogFullPath, 
        gs_LogParam.szIdent,
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec
    );
    ASSERT((nRetCode != -1) && (nRetCode < (sizeof(szLogFileName) - 1)));
    szLogFileName[sizeof(szLogFileName) - 1] = '\0';

    gs_nLogFile = open(szLogFileName, _LOG_OPENFILEFLAG_, _LOG_OPENFILEMODE_);
    if (gs_nLogFile == -1)
    {
        fprintf(stderr, "Log: open(\"%s\") failed = %s\n", szLogFileName, strerror(errno));
    }
    PROCESS_ERROR(gs_nLogFile != -1);

    nResult = true;
EXIT0:
    if (!nResult)
    {
        if (gs_nLogFile != -1)
        {
            close(gs_nLogFile);
            gs_nLogFile = -1;
        }
    }
    return nResult;
}

// cszIdent is file name prefix
int KGLogInit(const KGLOG_PARAM& cLogParam, void *pvContext)
{
    int nResult  = false;
    int nRetCode = false;
    USE_ARGUMENT(pvContext);

    PROCESS_ERROR(cLogParam.szPath[0]  != '\0');
    PROCESS_ERROR(cLogParam.szIdent[0] != '\0');

    gs_LogParam = cLogParam;

    if (gs_LogParam.Options == 0)
        gs_LogParam.Options =(KGLOG_OPTIONS)(LOG_OPTION_FILE | LOG_OPTION_CONSOLE);

    if (gs_LogParam.nMaxLineEachFile <= 0)
        gs_LogParam.nMaxLineEachFile = 32 * 1024;
  
    gs_LogParam.szPath[sizeof(gs_LogParam.szPath) - 1]  = '\0';
    gs_LogParam.szIdent[sizeof(gs_LogParam.szIdent) - 1] = '\0';

    if (gs_LogParam.Options & LOG_OPTION_FILE)
    {
        nRetCode = (int)strlen(gs_LogParam.szPath);
        ASSERT((nRetCode > 0) && "Invalid log file path !");
        PROCESS_ERROR(nRetCode < PATH_MAX);

        if (gs_LogParam.szPath[nRetCode - 1] == '\\')
        {
            gs_LogParam.szPath[nRetCode - 1] = '/';
        }
        
        if (gs_LogParam.szPath[nRetCode - 1] != '/')
        {            
            gs_LogParam.szPath[nRetCode] = '/';
            gs_LogParam.szPath[nRetCode + 1] = '\0';
        }

        nRetCode = (int)strlen(gs_LogParam.szIdent);
        ASSERT((nRetCode > 0) && "Invalid log file path !");
 
        if ((gs_LogParam.szIdent[nRetCode - 1] == '/') || (gs_LogParam.szIdent[nRetCode - 1] == '\\'))
        {            
            gs_LogParam.szIdent[nRetCode - 1] = '\0';
        }

        nRetCode = (int)snprintf(
            gs_szLogFileNamePrefix, 
            sizeof(gs_szLogFileNamePrefix) - 1, 
            "%s%s/", 
            gs_LogParam.szPath, 
            gs_LogParam.szIdent
        );
        gs_szLogFileNamePrefix[sizeof(gs_szLogFileNamePrefix) - 1] = '\0';
        PROCESS_ERROR((nRetCode != -1) && (nRetCode < (sizeof(gs_szLogFileNamePrefix) - 1)));
    }
    
    nResult = true;
EXIT0:
    return nResult;
}

int KGLogUnInit(void *pvContext)
{
    int nResult = false;

    USE_ARGUMENT(pvContext);

    if (gs_nLogFile != -1)
    {
        close(gs_nLogFile);
        gs_nLogFile = -1;
    }

    nResult = true;
//EXIT0:
    return nResult;
}

//  Set the log mask level if nPriorityMask != 0
int KGLogSetPriorityMask(int nPriorityMask)
{
//	if (nPriorityMask != 0)
	gs_nPriorityMask = nPriorityMask;

    return true;
}

static const char *gs_caszPriorityString[LOG_PRIORITY_MAX] =
{
    "RESR0",
    "RESR1",
    "RESR2",
    "ERROR",
    "WARN ",
    "RESR3",
    "INFO ",
    "DEBUG"
};

int KGLogPrintf(KGLOG_PRIORITY Priority, const char cszFormat[], ...)
{
    if ((KGLOG_MASK(KGLOG_PRI(Priority)) & gs_nPriorityMask) == 0)
        return true;

    int nResult     = false;
    int nRetCode    = false;
    int nLockFlag   = false;
    int nBufferLen  = 0;
    char szBuffer[1024];

    time_t tmtNow = 0;
    struct tm tmNow; 


    tmtNow = time(NULL);

    localtime_r(&tmtNow, &tmNow);
	
	nRetCode = snprintf(
        szBuffer, sizeof(szBuffer) - 1,
        "%d-%02d-%02d %02d:%02d:%02d[%4s][%s][%p]: ",
        tmNow.tm_year + 1900,
        tmNow.tm_mon + 1,
        tmNow.tm_mday,
        tmNow.tm_hour,
        tmNow.tm_min,
        tmNow.tm_sec,
		gs_LogParam.szProcName,
        gs_caszPriorityString[KGLOG_PRI(Priority)],
        KGetCurrentThreadId()
    );
	
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    if ((nRetCode != -1) && (nRetCode < (sizeof(szBuffer) - 1)))
        nBufferLen = nRetCode;
    else
        nBufferLen = sizeof(szBuffer) - 1;       

    va_list marker;
	va_start(marker, cszFormat);

    nRetCode = vsnprintf(szBuffer + nBufferLen, sizeof(szBuffer) - 1 - nBufferLen, cszFormat, marker);

    va_end(marker);

    szBuffer[sizeof(szBuffer) - 1] = '\0';

	 // bugfix: linux buffer overflow (glibc 2.1�Ժ�vsnprintf��ѭC99������ֵ��vc2005��ͬ) [12/9/2011 simon]
    if ((nRetCode != -1) && (nRetCode < (int)sizeof(szBuffer) - 1 - nBufferLen))
        nBufferLen += nRetCode;
    else
        nBufferLen = sizeof(szBuffer) - 1;       

    if (
        (szBuffer[nBufferLen - 1] != '\n') && 
        (szBuffer[nBufferLen - 1] != '\r')
    )
    {
        if (nBufferLen >= (sizeof(szBuffer) - 1))   // if full
            nBufferLen = sizeof(szBuffer) - 2; // fixed: potential overflow [12/9/2011 simon]

        szBuffer[nBufferLen] = '\n';
        szBuffer[nBufferLen + 1] = '\0';
        nBufferLen++;
    }
    
    if (gs_LogParam.Options & LOG_OPTION_CONSOLE)
    {
        fputs(szBuffer, stdout);
    }
    if (gs_LogParam.Options & LOG_OPTION_STDERR)
    {
        fputs(szBuffer, stderr);
    }

    if (gs_LogParam.Options & LOG_OPTION_FILE)
    {
        gs_Lock.Lock();
        nLockFlag = true;        

        if (
            gs_nIsFirstTimeToWriteLog ||
            (gs_nCurrentLine >= gs_LogParam.nMaxLineEachFile) ||
            (!(
                (gs_tmLastChangeFile.tm_mday == tmNow.tm_mday) &&
                (gs_tmLastChangeFile.tm_mon  == tmNow.tm_mon)  &&
                (gs_tmLastChangeFile.tm_year == tmNow.tm_year)
            ))
        )
        {            
            gs_nIsFirstTimeToWriteLog = false;
            gs_tmLastChangeFile = tmNow;
            
            nRetCode = _ChangeLogFile();
            PROCESS_ERROR(nRetCode);
            gs_nCurrentLine = 0;
        }

        if (gs_nLogFile != -1)
        {
            nRetCode = write(gs_nLogFile, szBuffer, nBufferLen);
            if (nRetCode == -1)
            {
                puts("warning : error on write log file, please check your hard disk!");
            }
        }        
        ++gs_nCurrentLine;
    }

    nResult = true;
EXIT0:
    if (nLockFlag)
    {
        gs_Lock.Unlock();
        nLockFlag = false;
    }
    return nResult;
}

