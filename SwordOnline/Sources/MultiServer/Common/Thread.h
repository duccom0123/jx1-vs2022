/********************************************************************
	created:	2003/02/14
	file base:	Thread
	file ext:	h
	author:		liupeng
	
	purpose:	
*********************************************************************/
#pragma once

/*
 * CThread
 */
class CThread
{
  public:
    CThread();

    virtual ~CThread();

    HANDLE GetHandle() const;

    void Wait() const;

    bool Wait(DWORD timeoutMillis) const;

    void Start();

    void Terminate(DWORD exitCode = 0);

  private:
    virtual int Run() = 0;

    static unsigned int __stdcall ThreadFunction(void *pV);

    HANDLE m_hThread;

    /*
	 * No copies do not implement
	 */
    CThread(const CThread &rhs);
    CThread &operator=(const CThread &rhs);
};
