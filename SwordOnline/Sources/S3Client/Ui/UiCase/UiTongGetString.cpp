// -------------------------------------------------------------------------
//	ÎÄ¼þÃû		£º	UiGetString.cpp
//	´´½¨Õß		£º	Wooy(Wu yue)
//	´´½¨Ê±¼ä	£º	2003-3-18
//	¹¦ÄÜÃèÊö	£º	»ñµÃ×Ö·û´®½çÃæ
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiTongGetString.h"
#include "UiInformation.h"
#include "../UiSoundSetting.h"


#define	SCHEME_INI 	"UiTongGetString.ini"


KUiTongGetString* KUiTongGetString::m_pSelf = NULL;

KUiTongGetString::KUiTongGetString()
{
	m_pRequester = NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiTongGetString* KUiTongGetString::OpenWindow(const bool bEdit, const char* pszTitle,
						const char* pszInitString,
						KWndWindow* pRequester, unsigned int uParam,
 						int nMinLen, int nMaxLen)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTongGetString;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);

		m_pSelf->m_nMinLen = nMinLen;
		m_pSelf->m_nMaxLen = nMaxLen;
		m_pSelf->m_pRequester = pRequester;
		m_pSelf->m_uRequesterParam = uParam;
		if (pszTitle)
			m_pSelf->m_Title.SetText(pszTitle);
		else
			m_pSelf->m_Title.Clear();
		if (pszInitString)
			m_pSelf->m_StringEdit.SetText(pszInitString);
		else
			m_pSelf->m_StringEdit.Clear();
		m_pSelf->m_StringEdit.SetLimitText(nMaxLen);
		m_pSelf->BringToTop();
		m_pSelf->Show(bEdit);
		Wnd_SetExclusive((KWndWindow*)m_pSelf);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±ÕÏú»Ù´°¿Ú
//--------------------------------------------------------------------------
void KUiTongGetString::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		Wnd_ReleaseExclusive((KWndWindow*)m_pSelf);
		m_pSelf->m_pRequester = NULL;
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();
	}
}

KUiTongGetString*	KUiTongGetString::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//ÏÔÊ¾´°¿Ú
void KUiTongGetString::Show(bool bEdit)
{
	int Left, Top;
	ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	KWndImage::Show();
	m_StringEdit.Enable(false);
	if (bEdit)
	{
		m_StringEdit.Enable(true);	
		Wnd_SetFocusWnd(&m_StringEdit);
	}
	Wnd_SetExclusive((KWndWindow*)this);
}

//Òþ²Ø´°¿Ú
void KUiTongGetString::Hide()
{
	Wnd_SetFocusWnd(NULL);
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
void KUiTongGetString::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_StringEdit);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiTongGetString::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Title.Init(&Ini, "Title");
			m_pSelf->m_StringEdit.Init(&Ini, "StringInput");
			m_pSelf->m_OkBtn.Init(&Ini, "OkBtn");
			m_pSelf->m_CancelBtn.Init(&Ini, "CancelBtn");
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿ÚÏûÏ¢º¯Êý
//--------------------------------------------------------------------------
int KUiTongGetString::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOk();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOk();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦µã»÷È·ÈÏ°´Å¥
//--------------------------------------------------------------------------
void KUiTongGetString::OnOk()
{
	char	szString[MAX_MESSAGE_LENGTH];

	int nLen = m_StringEdit.GetText(szString, sizeof(szString), true);
	if (nLen >= m_nMinLen && nLen <= m_nMaxLen)
	{
		if (m_pRequester)
			m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, (int)&szString);
		CloseWindow(true);
	}
	else
	{
		UIMessageBox(((nLen < m_nMinLen) ?
			"V¨n tù nhËp vµo qu¸ ng¾n" : "V¨n tù nhËp vµo qu¸ dµi"));
	}
}

void KUiTongGetString::OnCancel()
{
	if (m_pRequester)
		m_pRequester->WndProc(WND_M_OTHER_WORK_RESULT, m_uRequesterParam, 0);
	CloseWindow(true);
}
