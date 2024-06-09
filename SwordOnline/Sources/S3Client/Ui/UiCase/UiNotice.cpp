/*****************************************************************************************
//	界面--选择游戏服务器窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-12
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiShell.h"
#include "UiInit.h"
#include "UiSelServer.h"
#include "UiNotice.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "../../Login/Login.h"
#define	SCHEME_INI				"UiNotice.ini"


KUiNotice* KUiNotice::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiNotice::KUiNotice()
{
	m_szNotice[0] = 0;
	m_szMessage[0] = 0;
}

KUiNotice::~KUiNotice()
{
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiNotice* KUiNotice::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiNotice;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		g_LoginLogic.ReturnToIdle();
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->Show();
		m_pSelf->UpdateNotice();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiNotice::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiNotice::Initialize()
{
	AddChild(&m_List);
	AddChild(&m_Notice);
	AddChild(&m_MessageList);
	AddChild(&m_MessageScroll);
	m_MessageList.SetScrollbar(&m_MessageScroll);
	AddChild(&m_Close);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiNotice::LoadScheme(const char* pScheme)
{
	char		Buff[255];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_List     .Init(&Ini, "List");
		m_Notice.Init(&Ini, "Notice");
		m_MessageList.Init(&Ini, "MessageList");
		m_MessageScroll.Init(&Ini, "MessageScroll");
		m_Close   .Init(&Ini, "Close");
		Ini.GetString("Message", "Notice", "", m_szNotice, sizeof(m_szNotice));
		Ini.GetString("Message", "Message", "", m_szMessage, sizeof(m_szMessage));
	}
}


void KUiNotice::UpdateNotice()
{
	m_List.SetText(m_szNotice);
	m_MessageList.AddOneMessage(m_szMessage, strlen(m_szMessage));
}
//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiNotice::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam);
		break;
		case WM_KEYDOWN:
		nRet = OnKeyDown(uParam);
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_MessageScroll)
			m_MessageList.SetFirstShowLine(nParam);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}


void KUiNotice::OnClickButton(KWndButton* pBtn)
{
	if (pBtn == &m_Close)
		{
		if (KUiSelServer::OpenWindow())
			CloseWindow(true);
		}
	
}


int KUiNotice::OnKeyDown(unsigned int uKey)
{
	int	nRet = 1;
	KWndWindow* pToActive = NULL;
	switch (uKey)
	{
	case VK_RETURN:
		if (KUiSelServer::OpenWindow())
				CloseWindow(true);
		break;
	default:
		nRet = 0;
	break;
	}
	if (pToActive)
		pToActive->SetCursorAbove();
	return nRet;
}
