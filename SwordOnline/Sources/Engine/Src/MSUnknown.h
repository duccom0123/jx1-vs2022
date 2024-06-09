
#pragma once
#include<stdint.h>

#ifdef _WIN32

#include <Unknwn.h>

#else   // POSIX

#ifdef __cplusplus
extern "C"{
#endif

#define __RPC_FAR


#if (defined(_MSC_VER) || defined(__ICL))
#define MIDL_INTERFACE(x)   struct __declspec(uuid(x)) __declspec(novtable)
#else
#define MIDL_INTERFACE(x)   struct
#endif

#ifndef BEGIN_INTERFACE
#define BEGIN_INTERFACE
#endif

#ifndef END_INTERFACE
#define END_INTERFACE 
#endif

#ifndef interface
#define interface struct
#endif // interface

#ifndef GUID_DEFINED
#define GUID_DEFINED
	typedef struct _GUID {          // size is 16
		DWORD Data1;
		WORD   Data2;
		WORD   Data3;
		BYTE  Data4[8];
	} GUID;

#define REFGUID const GUID &
#endif // GUID_DEFINED

#ifndef __IID_DEFINED__
#define __IID_DEFINED__
		typedef GUID IID;

#define REFIID const IID &
#endif  // __IID_DEFINED__

#ifndef UUID_DEFINED
#define UUID_DEFINED
		typedef GUID UUID;

#endif

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
		typedef GUID CLSID;

#define REFCLSID const CLSID &
#endif  // CLSID_DEFINED

#ifndef _SYS_GUID_OPERATORS_
#define _SYS_GUID_OPERATORS_    1

		inline int IsEqualGUID(const GUID &RGuid1, const GUID &RGuid2)
		{
			return (
				((uint32_t *) &RGuid1)[0] == ((uint32_t *) &RGuid2)[0] &&
				((uint32_t *) &RGuid1)[1] == ((uint32_t *) &RGuid2)[1] &&
				((uint32_t *) &RGuid1)[2] == ((uint32_t *) &RGuid2)[2] &&
				((uint32_t *) &RGuid1)[3] == ((uint32_t *) &RGuid2)[3]);
		}

		inline int operator==(const GUID &GuidOne, const GUID &GuidOther)
		{
			return IsEqualGUID(GuidOne, GuidOther);
		}

		inline int operator!=(const GUID &GuidOne, const GUID &GuidOther)
		{
			return !(GuidOne == GuidOther);
		}

#endif  // _SYS_GUID_OPERATORS_

//------------------------------------------------------------------
#include "ComError.h"

extern const IID IID_IUnknown;

    MIDL_INTERFACE("00000000-0000-0000-C000-000000000046")
    IUnknown
    {
    public:
        BEGIN_INTERFACE
        virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject) = 0;
        
        virtual ULONG STDMETHODCALLTYPE AddRef( void) = 0;
        
        virtual ULONG STDMETHODCALLTYPE Release( void) = 0;
        
        END_INTERFACE
    };

/* [local] */ 

typedef /* [unique] */ IUnknown __RPC_FAR *LPUNKNOWN;



extern const IID IID_IClassFactory;

    MIDL_INTERFACE("00000001-0000-0000-C000-000000000046")
    IClassFactory : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE CreateInstance( 
            /* [unique][in] */ IUnknown *pUnkOuter,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE LockServer( 
            /* [in] */ int fLock) = 0;
        
    };

/* [local] */ 

typedef /* [unique] */ IClassFactory __RPC_FAR *LPCLASSFACTORY;




#ifdef __cplusplus
}
#endif

#endif  // _WIN32

