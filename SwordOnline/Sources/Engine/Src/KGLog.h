
#pragma once


#include "MSUnknown.h"
#include "kwin32.h"

// priorities/facilities are encoded into a single 32-bit quantity, where the
// bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
// (0-big number).  Both the priorities and the facilities map roughly
// one-to-one to strings in the syslogd(8) source code.  This mapping is
// included in this file.
// 
// priorities (these are ordered)

enum KGLOG_PRIORITY
{
    LOG_RESERVE0  =   0,  // LOG_EMERG  =   0,  // system is unusable
    LOG_RESERVE1  =   1,  // LOG_ALERT  =   1,  // action must be taken immediately
    LOG_RESERVE2  =   2,  // LOG_CRIT   =   2,  // critical conditions
    LOG_ERR		=   3,  // error conditions
    LOG_WARNING	=   4,  // warning conditions
    LOG_RESERVE3  =   5,  // LOG_NOTICE =   5,  // normal but significant condition
    LOG_INFO	    =   6,  // informational 
    LOG_DEBUG	    =   7,  // debug-level messages
    LOG_PRIORITY_MAX
};

enum KGLOG_OPTIONS
{
    LOG_OPTION_FILE      =   0x01,   // log on to file, default
    LOG_OPTION_CONSOLE   =   0x02,   // log on the console if errors in sending
    LOG_OPTION_STDERR    =   0x04,   // log on the stderr stream
    //LOG_UDP       =   0x08,   // log to syslogd server use UDP
};

#define PATH_MAX    256
#define KGLOG_PRIMASK    0x07

#define	KGLOG_PRI(pri)	((pri) & KGLOG_PRIMASK)

// arguments to setlogmask.
#define	KGLOG_MASK(pri) (1 << (pri))		    // mask for one priority
#define	KGLOG_UPTO(pri) ((1 << ((pri)+1)) - 1)	// all priorities through pri


typedef struct _KGLOG_PARAM
{
    char szPath[PATH_MAX];      // if LOG_FILE
    char szIdent[PATH_MAX];     // if LOG_FILE
	char szProcName[32];			//��������
    KGLOG_OPTIONS Options;          // 0 is default
    int  nMaxLineEachFile;

} KGLOG_PARAM;

// cszIdent is file name prefix
ENGINE_API int KGLogInit(const KGLOG_PARAM& cLogParam, void *pvContext);
ENGINE_API int KGLogUnInit(void *pvContext);

//  Set the log mask level if nPriorityMask != 0
ENGINE_API int KGLogSetPriorityMask(int nPriorityMask);

ENGINE_API int KGLogPrintf(KGLOG_PRIORITY Priority, const char cszFormat[], ...);

#define LOG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "LOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, __FUNCTION__  \
            );                  \
            goto EXIT0;         \
        }                       \
    } while (false)


#define LOG_CHECK(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "LOG_PROCESS_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, __FUNCTION__  \
            );                  \
        }                       \
    } while (false)

#define LOG_PROCESS_SUCCESS(Condition) \
    do  \
    {   \
        if (Condition)          \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "LOG_PROCESS_SUCCESS(%s) at line %d in %s\n", #Condition, __LINE__, __FUNCTION__  \
            );                  \
            goto EXIT1;         \
        }                       \
    } while (false)

#define LOG_PROCESS_ERROR_RET(Condition, Code) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "LOG_PROCESS_ERROR_RET_CODE(%s, %d) at line %d in %s\n", \
                #Condition, (Code), __LINE__, __FUNCTION__                  \
            );                  \
            nResult = (Code);   \
            goto EXIT0;         \
        }                       \
    } while (false)

#define LOG_PROCESS_ERROR_RET_COM(Condition, Code) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "LOG_PROCESS_ERROR_RET_COM(%s, %d) at line %d in %s\n", \
                #Condition, (Code), __LINE__, __FUNCTION__                  \
            );                  \
            hrResult = (Code);  \
            goto EXIT0;         \
        }                       \
    } while (false)


#define KGLOG_COM_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (FAILED(Condition))  \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "KGLOG_COM_PROCESS_ERROR(0x%X) at line %d in %s\n", (Condition), __LINE__, __FUNCTION__  \
            );                  \
            goto EXIT0;         \
        }                       \
    } while (false)


#define KGLOG_COM_PROCESS_SUCCESS(Condition)   \
    do  \
    {   \
        if (SUCCEEDED(Condition))   \
        {                           \
            KGLogPrintf(            \
                LOG_DEBUG,        \
                "KGLOG_COM_PROCESS_SUCCESS(0x%X) at line %d in %s\n", (Condition), __LINE__, __FUNCTION__  \
            );                      \
            goto EXIT1;             \
        }                           \
    } while (false)

#define LOG_CHECK_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "LOG_CHECK_ERROR(%s) at line %d in %s\n", #Condition, __LINE__, __FUNCTION__  \
            );                  \
        }                       \
    } while (false)

#define LOG_COM_CHECK_ERROR(Condition) \
    do  \
    {   \
        if (FAILED(Condition))       \
        {                       \
            KGLogPrintf(        \
                LOG_DEBUG,    \
                "LOG_COM_CHECK_ERROR(0x%X) at line %d in %s\n", (Condition), __LINE__, __FUNCTION__  \
            );                  \
        }                       \
    } while (false)

#define MYSQL_PROCESS_ERROR(Condition, pMySQL)           \
	do                                                          \
{                                                           \
	if (!(Condition))                                       \
	{                                                       \
	KGLogPrintf(                                        \
	LOG_DEBUG,                                    \
	"[MySQL] error line %d, function %s(): \"%s\"",	\
	__LINE__, __FUNCTION__, mysql_error(pMySQL)      \
	);                                                  \
	goto EXIT0;                                         \
	}                                                       \
} while (false)


#define PROCESS_ERROR(Condition) \
	do  \
{   \
	if (!(Condition))   \
	goto EXIT0;     \
} while (false)

#define USE_ARGUMENT(arg) ((void)(arg))