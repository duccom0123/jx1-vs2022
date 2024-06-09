//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KThread.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KThread_H
#define KThread_H
//---------------------------------------------------------------------------
typedef void (* TThreadFunc)(void* arg);

#ifndef WIN32
#include <pthread.h>
#endif
#include <time.h>
//---------------------------------------------------------------------------
#ifdef WIN32
class ENGINE_API KThread
#else
class KThread
#endif
{
private:
#ifdef WIN32
	HANDLE			m_ThreadHandle;
	DWORD			m_ThreadId;
#else
     pthread_t  p_thread;
#endif
	TThreadFunc 	m_ThreadFunc;
	LPVOID			m_ThreadParam;
public:
	KThread();
	~KThread();
	BOOL			Create(TThreadFunc lpFunc, void* lpParam);
	void			Destroy();
	void			Suspend();
	void			Resume();
	BOOL			IsRunning();
	void			WaitForExit();
	int				GetPriority();
	BOOL			SetPriority(int priority);
//private:
	DWORD			ThreadFunction();
#ifdef WIN32
//	static			DWORD __stdcall ThreadProc(LPVOID lpParam);
#else
//	void * ThreadProc(LPVOID lpParam);
#endif
};
#ifdef WIN32

inline void* KGetCurrentThreadId()
{
	return (void*)(INT_PTR)GetCurrentThreadId();
}

#else     //linux

inline void* KGetCurrentThreadId()
{
	return (void*)pthread_self();
}

#endif
#if (defined(_MSC_VER) || defined(__ICL))

inline struct tm* localtime_r(const time_t* timep, struct tm* result)
{
	struct tm* ptm = NULL;
#ifdef WIN32
	if (0 == localtime_s(result, timep))
		ptm = result;
#else
	ptm = localtime(timep);
	if (
		(result) &&
		(ptm)
		)
	{
		*result = *ptm;
	}
#endif
	return ptm;
};
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif


