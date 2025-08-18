// -------------------------------------------------------------------------
//	ÎÄ¼şÃû		£º	UiReconnect.h
//	´´½¨Õß		£º	Freeway Chen
//	´´½¨Ê±¼ä	£º	2003-7-18
//	¹¦ÄÜÃèÊö	£º	×Ô¶¯ÖØÁ¬µÄÁ÷³Ì¿ØÖÆ	
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "UiReconnect.h"
#include "UiInformation.h"
#include "UiSysMsgCentre.h"
#include "../UiShell.h"
#include "../UiCase/UiInit.h"
#include "../Elem/UiImage.h"
#include "../UiBase.h"
#include "../../Login/Login.h"
#include "../../../Engine/Src/Text.h"
#include "../../../Engine/Src/KDebug.h"
#include <crtdbg.h>

#define	MSG_RECONNECTING				"HÖ thèng ®ang kÕt nèi l¹i lÇn thø <color=255,0,0>%d<color>\n%s"
#define	MSG_WAIT_TO_RECONNECT			"TiÕn hµnh kÕt nèi l¹i trong %d gi©y (lÇn thø <color=255,0,0>%d<color>)"
#define	MSG_WAIT_NEXT_GROUP_RECONNECT	"TiÕn hµnh kÕt nèi l¹i trong %d gi©y"
#define	MSG_HIDE_RECONNECT_MSG_ID		"21"

//Á½×é¶ÏÏßÖØÁ¬µÄ¼ä¸ôÊ±¼ä£¬µ¥Î»ºÁÃë
#define RECONNECT_GROUP_INTERVAL		60000
//Á½´Î¶ÏÏßÖØÁ¬µÄ¼ä¸ôÊ±¼ä£¬µ¥Î»ºÁÃë
#define RECONNECT_INTERVAL				10000
//¿ªÊ¼Ê±²»¿É¼ûµÄÖØÁ¬µÄ´ÎÊı
#define INVISIBLE_RECONNECT_TIMES		3
//²»¿É¼ûµÄÁ½´Î¶ÏÏßÖØÁ¬µÄ¼ä¸ôÊ±¼ä£¬µ¥Î»ºÁÃë
#define INVISIBLE_RECONNECT_INTERVAL	5000

//È¡ÏûÖØÁ¬µÄ°´Å¥µÄÎÄ×Ö
#define	RECONNECT_QUIT_BTN_LABEL	"Tho¸t"
//Á¢¼´ÖØÁ¬µÄ°´Å¥µÄÎÄ×Ö
#define	RECONNECT_START_AT_ONCE		"KÕt nèi l¹i"

KReconnectWnd*	KReconnectWnd::m_pSelf = NULL;

bool KReconnectWnd::LaunchReconnect(int nMaxTimes)
{
	_ASSERT(m_pSelf == NULL);
	if (m_pSelf == NULL)
		m_pSelf = new KReconnectWnd;
	if (m_pSelf)
	{
		m_pSelf->m_bStop = false;
		Wnd_AddWindow(m_pSelf, WL_TOPMOST);
		if (nMaxTimes >= 1)
			m_pSelf->m_nMaxReconnectTimes = nMaxTimes;
		else
			m_pSelf->m_nMaxReconnectTimes = 1;
		m_pSelf->FirstReconnect();
	}
	else
	{
		Exit(true);
	}
	return (m_pSelf != NULL);
}

bool KReconnectWnd::IsReconnecttingGoingOn()
{
	return (m_pSelf != NULL);
}

void KReconnectWnd::FirstReconnect()
{
	m_nReconnectTimes = 0;
	m_bWaitToReconnect = true;
	m_uWaitStartTime = IR_GetCurrentTime();
	m_uToWaitTime = rand () % INVISIBLE_RECONNECT_INTERVAL + 1000;

	KIniFile* pConfig = g_UiBase.GetCommConfigFile();
	m_nHideTimes = INVISIBLE_RECONNECT_TIMES;
	if (pConfig)
	{
		pConfig->GetInteger("Main", "AutoReconnectHideTimes", 3, &m_nHideTimes);
		if (m_nHideTimes < 0)
			m_nHideTimes = 0;
	}
}

bool KReconnectWnd::StartReconnect()
{
	if (m_nHideTimes > 0 ||
		(++m_nReconnectTimes) < m_nMaxReconnectTimes)
	{
		/*if (m_nHideTimes > 0)
		{
			KSystemMessage	Msg;
			Msg.byConfirmType = SMCT_NONE;
			Msg.byParamSize = 0;
			Msg.byPriority = 0;
			Msg.eType = SMT_NORMAL;
			Msg.uReservedForUi = 0;

			KIniFile*	pIni = g_UiBase.GetCommConfigFile();
			if (pIni)
			{
				if (pIni->GetString("InfoString", MSG_HIDE_RECONNECT_MSG_ID,
					"", Msg.szMessage, sizeof(Msg.szMessage)))
				{
					KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
				}
				g_UiBase.CloseCommConfigFile();
			}
		}*/

		g_LoginLogic.ReturnToIdle();
		g_LoginLogic.AutoLogin();
		m_bWaitToReconnect = false;
	}
	else
	{
		m_nReconnectTimes = 0;
		m_pSelf->m_bWaitToReconnect = true;
		m_pSelf->m_uWaitStartTime = IR_GetCurrentTime();
		m_pSelf->m_uToWaitTime = RECONNECT_GROUP_INTERVAL;
	}
	return true;
}

void KReconnectWnd::Exit(bool bQuitGame)
{
	if (m_pSelf)
	{
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
	if (bQuitGame)
	{
		g_LoginLogic.ReturnToIdle();
		UiEndGame();
		KUiInit::OpenWindow(true, false);
	}
}

// ÓÃÀ´¿ØÖÆÏÂÒ»´ÎÖØÁ¬µÄÊ±¼ä
void KReconnectWnd::Breathe()
{
	if (m_bStop)
		return;
	char	szInfo[256];
	int		nInfoLen;
	if (m_bWaitToReconnect)
	{	//====µÈ´ı½øĞĞÏÂ´Î×Ô¶¯ÖØÁ¬====
		unsigned int uRemain;
		if (uRemain = IR_GetRemainTime(m_uToWaitTime, m_uWaitStartTime))
		{
			if (m_nHideTimes == 0)
			{
				if (uRemain < RECONNECT_INTERVAL)
				{
					nInfoLen = sprintf(szInfo, MSG_WAIT_TO_RECONNECT,
						(uRemain + 999)/ 1000, m_nReconnectTimes + 1);
				}
				else
				{
					nInfoLen = sprintf(szInfo, MSG_WAIT_NEXT_GROUP_RECONNECT,
						(uRemain + 999) / 1000);
				}			
				nInfoLen = TEncodeText(szInfo, nInfoLen);
				g_UiInformation.Show(szInfo, RECONNECT_QUIT_BTN_LABEL, RECONNECT_START_AT_ONCE, this, 0, nInfoLen);
			}
		}
		else
		{	//====¿ªÊ¼ÏÂ´Î×Ô¶¯ÖØÁ¬====
			StartReconnect();
		}
		return;
	}

	LOGIN_LOGIC_STATUS eStatus = g_LoginLogic.GetStatus();
	const char* pReconnectMsg = NULL;
	switch (eStatus)
	{
	case LL_S_IN_GAME://====ÖØÁ¬³É¹¦====
		g_UiInformation.Close();
		UiResumeGame();
		Exit(false);
		break;
	case LL_S_IDLE:	//====ÖØÁ¬Ê§°Ü====
		{
			LOGIN_LOGIC_RESULT_INFO eResult= g_LoginLogic.GetResult();
			if (eResult == LL_R_ACCOUNT_NOT_ENOUGH_POINT)
			{
				m_bStop = true;
				pReconnectMsg = "Kh«ng ®ñ thêi gian ®Ó ch¬i!. H·y n¹p card ®Ó tiÕp tôc trß ch¬i!";
			}
			else if (eResult == LL_R_ACCOUNT_FREEZE)
			{
				m_bStop = true;
				pReconnectMsg = "Tµi kho¶n nµy ®· bŞ khãa";
			}
			else
			{
				m_bWaitToReconnect = true;
				m_uWaitStartTime = IR_GetCurrentTime();
				if (m_nHideTimes > 0)
				{
					m_nHideTimes--;
					m_uToWaitTime = INVISIBLE_RECONNECT_INTERVAL;
				}
				else
				{
					m_uToWaitTime = RECONNECT_INTERVAL;
					pReconnectMsg = "KÕt nèi thÊt b¹i";
				}
			}
		}
		break;
	case LL_S_ACCOUNT_CONFIRMING:	//µÈ´ıÕËºÅÃÜÂëÑéÖ¤
		pReconnectMsg = "§ang tiÕn hµnh kiÓm tra tµi kho¶n vµ mËt khÈu";
		break;
	case LL_S_WAIT_ROLE_LIST:		//µÈ´ı½ÓÊÕ½ÇÉ«ÁĞ±íÊı¾İ
		pReconnectMsg = "§ang nhËn danh s¸ch nh©n vËt";
		break;
	case LL_S_WAIT_TO_LOGIN_GAMESERVER:	//µÈ´ıµÇÂ½ÓÎÏ··şÎñÆ÷
		pReconnectMsg = "§ang phôc vô trß ch¬i";
		break;
	case LL_S_ENTERING_GAME:		//ÕıÔÚ½øÈëÓÎÏ·
		pReconnectMsg = "§ang ®¨ng nhËp vµo trß ch¬i";
		break;
	}

	if (m_bStop)
	{
		nInfoLen = strlen(pReconnectMsg);
		g_UiInformation.Show(pReconnectMsg, RECONNECT_QUIT_BTN_LABEL, NULL, this, 0, nInfoLen);
	}
	else if (pReconnectMsg && m_nHideTimes == 0)
	{
		nInfoLen = sprintf(szInfo, MSG_RECONNECTING,
				m_nReconnectTimes + 1, pReconnectMsg);
		nInfoLen = TEncodeText(szInfo, nInfoLen);
		g_UiInformation.Show(szInfo, RECONNECT_QUIT_BTN_LABEL, NULL, this, 0, nInfoLen);
	}
}

int KReconnectWnd::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)//´°¿Úº¯Êı
{
	if (uMsg == WND_M_OTHER_WORK_RESULT)
	{
		if (nParam == 1)	//Á¢¼´ÖØÁ¬
		{
			StartReconnect();
			Breathe();
		}
		else
			Exit(true);		//È¡Ïû£¬ÍË³öÓÎÏ·
	}
	return 0;
}
