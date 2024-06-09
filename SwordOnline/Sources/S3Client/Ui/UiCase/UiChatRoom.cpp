/*****************************************************************************************
//	ΩÁ√Ê--¡ƒÃÏ÷˜ΩÁ√Ê
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-3-10
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include <crtdbg.h>
#include "../Elem/WndMessage.h"
#include "UiChatRoom.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiPlayerBar.h"
#include "UiGetString.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/Src/KDebug.h"
#include "../../../Engine/Src/Text.h"
#include "../../../Engine/Src/KSG_StringProcess.h"
#include "../../../core/src/GameDataDef.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "KProtocol.h"
#include "malloc.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;
extern IInlinePicEngineSink *g_pIInlinePicSink;

enum INPUT_STRING_PARAM
{
	ISP_CHATROOM_JOIN		= 0x100,
	ISP_CHATROOM_PASSWORD	= 0x200,
	ISP_CHATROOM_OPENGAME	= 0x400,
	ISP_CHATROOM_HIGHT		= 0x800,
	ISP_CHATROOM_LOW		= 0x120,
};

#define	SCHEME_INI					"UiChatRoom.ini"

KUiChatRoom* KUiChatRoom::m_pSelf = NULL;


KUiChatRoom::KUiChatRoom()
{
	m_nRoomFigure = CHATROOM_MEMBER;
	m_nRoomDiceTime = 0;
	m_bIsInRoom = FALSE;
	m_uKeyPicIndex = -1;
	m_uDicePicIndex = -1;
	m_uLastUpdateTime = 0;
	m_szRoomName[0] = 0;
}

KUiChatRoom::~KUiChatRoom()
{
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫»Áπ˚¥∞ø⁄’˝±ªœ‘ æ£¨‘Ú∑µªÿ µ¿˝÷∏’Î
//--------------------------------------------------------------------------
KUiChatRoom* KUiChatRoom::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫¥Úø™¥∞ø⁄£¨∑µªÿŒ®“ªµƒ“ª∏ˆ¿‡∂‘œÛ µ¿˝
//--------------------------------------------------------------------------
KUiChatRoom* KUiChatRoom::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiChatRoom;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		if(m_pSelf->m_uLastUpdateTime < IR_GetCurrentTime())
		{
			m_pSelf->UpdateInterface();
			m_pSelf->m_uLastUpdateTime = IR_GetCurrentTime() + 3000;
		}
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫≥ı ºªØ
//--------------------------------------------------------------------------
void KUiChatRoom::Initialize()
{
	AddChild(&m_BtnClose);
	AddChild(&m_BtnExitRoom);
	AddChild(&m_BtnEnterRoom);
	AddChild(&m_BtnDelRoom);
	AddChild(&m_BtnOpenDice);
	AddChild(&m_BtnHightDice);
	AddChild(&m_BtnLowDice);
	AddChild(&m_BtnPassword);
	AddChild(&m_TxtCurRoomTitle);
	AddChild(&m_RoomList);
	AddChild(&m_MemberList);
	AddChild(&m_BlackList);
	m_BtnEnterRoom.Show();
	m_BtnDelRoom.Hide();
	m_BtnOpenDice.Hide();
	m_BtnHightDice.Hide();
	m_BtnLowDice.Hide();
	m_BtnPassword.Hide();

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫πÿ±’¥∞ø⁄£¨Õ¨ ±ø…“‘—°‘Ú «∑Ò…æ≥˝∂‘œÛ µ¿˝
//--------------------------------------------------------------------------
void KUiChatRoom::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
		{
			m_pSelf->Hide();
		}
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫‘ÿ»Î¥∞ø⁄µƒΩÁ√Ê∑Ω∞∏
//--------------------------------------------------------------------------
void KUiChatRoom::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[MAX_PATH];
		KIniFile	Ini;
		sprintf(Buff, "%s\\" SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->KWndShowAnimate::Init(&Ini, "Main");
			m_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
			m_pSelf->m_BtnExitRoom.Init(&Ini, "BtnExitRoom");
			m_pSelf->m_BtnEnterRoom.Init(&Ini, "BtnEnterRoom");
			m_pSelf->m_BtnDelRoom.Init(&Ini, "BtnDelRoom");
			m_pSelf->m_BtnOpenDice.Init(&Ini, "BtnOpenDice");
			m_pSelf->m_BtnHightDice.Init(&Ini, "BtnHightDice");
			m_pSelf->m_BtnLowDice.Init(&Ini, "BtnLowDice");
			m_pSelf->m_BtnPassword.Init(&Ini, "BtnPassword");
			m_pSelf->m_TxtCurRoomTitle.Init(&Ini, "TxtCurRoomTitle");
			m_pSelf->m_RoomList.Init(&Ini, "RoomList");
			m_pSelf->m_MemberList.Init(&Ini, "MemberList");
			m_pSelf->m_BlackList.Init(&Ini, "BlackList");

			Ini.GetInteger("Main", "RoomNameLen", 0, &m_pSelf->m_nRoomNameLen);
			Ini.GetInteger("Main", "CreaterLen", 0, &m_pSelf->m_nCreaterLen);
			Ini.GetInteger("Main", "TimeLen", 0, &m_pSelf->m_nTimeLen);
			Ini.GetInteger("Main", "MemNumLen", 0, &m_pSelf->m_nMemNumLen);
			Ini.GetInteger("MemberList", "MemNameLen", 0, &m_pSelf->m_nMemNameLen);
			m_pSelf->m_TxtCurRoomTitle.SetText("Ch≠a vµo phﬂng");
			Ini.GetString("RoomList", "Key", "", Buff, MAX_PATH);
			g_pIInlinePicSink->AddCustomInlinePic((WORD&)m_pSelf->m_uKeyPicIndex, Buff);
			Ini.GetString("RoomList", "Dice", "", Buff, MAX_PATH);
			g_pIInlinePicSink->AddCustomInlinePic((WORD&)m_pSelf->m_uDicePicIndex, Buff);
		}
	}
}


//--------------------------------------------------------------------------
//	π¶ƒ‹£∫¥∞ø⁄œ˚œ¢∫Ø ˝
//--------------------------------------------------------------------------
int KUiChatRoom::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndWindow*)uParam);
		break;
	case WND_N_LIST_ITEM_D_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_RoomList)
		{
			if (nParam >= 0 &&m_bIsInRoom==FALSE)
			{
				if(KSG_StringGetValue(1, m_RoomList.GetMessageListBox()->GetOneMessageData(nParam), 6) == 1)
					KUiGetString::OpenWindow(GSA_NORMAL, "NhÀp mÀt m∑ phﬂng", "", this, ISP_CHATROOM_JOIN, 9999);
				else
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,KSG_StringGetValue(4, m_RoomList.GetMessageListBox()->GetOneMessageData(nParam), 1));
					nValue=KSG_StringSetValue(4,nValue,2,0);
					g_pCoreShell->OperationRequest(GOI_CHATROOM,  nValue, TRUE);
				}
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MemberList)
		{
			if (nParam >= 0)
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,m_MemberList.GetMessageListBox()->GetOneMessageData(nParam));
					nValue=KSG_StringSetValue(4,nValue,2,0);
					g_pCoreShell->OperationRequest(GOI_CHATROOM, nValue, FALSE);
					return 0;
				}
				char szName[32];
				int nLen = m_MemberList.GetMessageListBox()->GetOneMessage(nParam, szName, 32, TRUE);

				int nName = 0;
				if (nLen >= 0)
				{
					szName[nLen] = 0;
					while (nName < nLen)
					{
						if (szName[nName] == ' ')
						{
							szName[nName] = 0;
							break;
						}
						nName++;
					}
					if(nName > 0)
					{
						if (!KUiPlayerBar::IsSelfName((char*)szName))
							KUiPlayerBar::InputNameMsg(false, szName, true);
					}
				}
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BlackList)
		{
			if(m_nRoomFigure == CHATROOM_HOST)
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,m_BlackList.GetMessageListBox()->GetOneMessageData(nParam));
					nValue=KSG_StringSetValue(4,nValue,2,1);
					g_pCoreShell->OperationRequest(GOI_CHATROOM, nValue, FALSE);
					return 0;
				}

				char szName[32];
				m_BlackList.GetMessageListBox()->GetOneMessage(nParam, szName, 32, TRUE);
				if (!KUiPlayerBar::IsSelfName((char*)szName))
					KUiPlayerBar::InputNameMsg(false, szName, true);
			}
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		{
			if (uParam == ISP_CHATROOM_JOIN)
			{
				int nIndex = m_RoomList.GetMessageListBox()->GetCurSel();
				if (nIndex >= 0 && m_bIsInRoom ==FALSE)
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,KSG_StringGetValue(4, m_RoomList.GetMessageListBox()->GetOneMessageData(nIndex), 1));
					nValue=KSG_StringSetValue(4,nValue,2,nParam);
					g_pCoreShell->OperationRequest(GOI_CHATROOM,  nValue, TRUE);
				}
			}
			else if (uParam == ISP_CHATROOM_PASSWORD)
			{
				if(m_nRoomFigure == CHATROOM_HOST)
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,nParam);
					nValue=KSG_StringSetValue(4,nValue,2,2);
					g_pCoreShell->OperationRequest(GOI_CHATROOM, nValue, FALSE);
				}
			}
			else if(uParam == ISP_CHATROOM_OPENGAME)
			{
				if(nParam <= 0)
					return 0;
				if(m_nRoomFigure == CHATROOM_HOST)
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,nParam);
					nValue=KSG_StringSetValue(4,nValue,2,3);
					g_pCoreShell->OperationRequest(GOI_CHATROOM, nValue, FALSE);
				}
			}
			else if(uParam == ISP_CHATROOM_HIGHT)
			{
				if(nParam <= 0)
					return 0;
				if(m_nRoomFigure == CHATROOM_MEMBER)
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,nParam);
					nValue=KSG_StringSetValue(4,nValue,2,4);
					g_pCoreShell->OperationRequest(GOI_CHATROOM, nValue, FALSE);
				}
			}
			else if(uParam == ISP_CHATROOM_LOW)
			{
				if(nParam <= 0)
					return 0;
				if(m_nRoomFigure == CHATROOM_MEMBER)
				{
					int nValue=0;
					nValue=KSG_StringSetValue(4,nValue,1,nParam);
					nValue=KSG_StringSetValue(4,nValue,2,5);
					g_pCoreShell->OperationRequest(GOI_CHATROOM, nValue, FALSE);
				}
			}
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}
// -------------------------------------------------------------------------
// π¶ƒ‹		: µ„ª˜∞¥≈•∫Ûµƒ¥¶¿Ì
// -------------------------------------------------------------------------
void KUiChatRoom::OnClickButton(KWndWindow* pWnd)
{
	if (pWnd == (KWndWindow*)&m_BtnPassword)
	{
		if(m_bIsInRoom && m_nRoomFigure == CHATROOM_HOST)
			KUiGetString::OpenWindow(GSA_NORMAL, "NhÀp mÀt m∑ phﬂng", "", this, ISP_CHATROOM_PASSWORD, 9999);
	}
	else if (pWnd == (KWndWindow*)&m_BtnOpenDice)
	{
		if(m_bIsInRoom && m_nRoomFigure == CHATROOM_HOST && m_nRoomDiceTime == 0)
			KUiGetString::OpenWindow(GSA_NORMAL, "NhÀp giÌi hπn mÁi thµnh vi™n", "", this, ISP_CHATROOM_OPENGAME, 100);
	}
	else if (pWnd == (KWndWindow*)&m_BtnHightDice)
	{
		if(m_bIsInRoom && m_nRoomFigure == CHATROOM_MEMBER && m_nRoomDiceRet ==0 && m_nRoomDiceTime > 0)
			KUiGetString::OpenWindow(GSA_NORMAL, "NhÀp Æi”m th≠Îng", "", this, ISP_CHATROOM_HIGHT, 100);
	}
	else if (pWnd == (KWndWindow*)&m_BtnLowDice)
	{
		if(m_bIsInRoom && m_nRoomFigure == CHATROOM_MEMBER && m_nRoomDiceRet ==0 && m_nRoomDiceTime > 0)
			KUiGetString::OpenWindow(GSA_NORMAL, "NhÀp Æi”m th≠Îng", "", this, ISP_CHATROOM_LOW, 100);
	}
	else if (pWnd == (KWndWindow*)&m_BtnEnterRoom)
	{
		int nSel =m_RoomList.GetMessageListBox()->GetCurSel();
		if (nSel >= 0 &&m_bIsInRoom==FALSE)
		{
			if(KSG_StringGetValue(1, m_RoomList.GetMessageListBox()->GetOneMessageData(nSel), 6) == 1)
				KUiGetString::OpenWindow(GSA_NORMAL, "NhÀp mÀt m∑ phﬂng", "", this, ISP_CHATROOM_JOIN, 9999);
			else
			{
				int nValue=0;
				nValue=KSG_StringSetValue(4,nValue,1,KSG_StringGetValue(4, m_RoomList.GetMessageListBox()->GetOneMessageData(nSel), 1));
				nValue=KSG_StringSetValue(4,nValue,2,0);
				g_pCoreShell->OperationRequest(GOI_CHATROOM,  nValue, TRUE);
			}
		}
	}
	else if(pWnd == (KWndWindow*)&m_BtnDelRoom)
	{
		if(m_bIsInRoom && m_nRoomFigure == CHATROOM_HOST)
			g_pCoreShell->OperationRequest(GOI_CHATROOM, -3, TRUE);
	}
	else if(pWnd == (KWndWindow*)&m_BtnExitRoom)
	{
		if(m_bIsInRoom)
			g_pCoreShell->OperationRequest(GOI_CHATROOM, -2, TRUE);
	}
	else if (pWnd == (KWndWindow*)&m_BtnClose)
		CloseWindow(false);
}


void KUiChatRoom::Breathe()
{
	if(m_pSelf)
	{
		if(m_bIsInRoom)
		{
			int nMsgLen=0, nCurLen=0;
			char szMsg[128], szBuf[12];
			memset(szMsg, 0, sizeof(szMsg));
			nCurLen = strlen(m_szRoomName);
			memcpy(szMsg + nMsgLen, m_szRoomName, nCurLen);
			nMsgLen += nCurLen;
			if(m_nRoomDiceRet ==0)
			{
				if(m_nRoomDiceTime > 0)
				{
					m_nRoomDiceTime--;
					szMsg[nMsgLen++] = KTC_TAB;
					szMsg[nMsgLen] = KTC_INLINE_PIC;
					*((WORD*)(szMsg + nMsgLen+1)) = m_pSelf->m_uDicePicIndex;
					nMsgLen += 3;
					nCurLen = sprintf(szBuf, "%d", m_nRoomDiceTime / GAME_FPS);
					memcpy(szMsg + nMsgLen, szBuf, nCurLen);
					nMsgLen += nCurLen;
				}
			}
			else if(m_nRoomDiceRet == 1)
			{
				szMsg[nMsgLen++] = KTC_TAB;
				szMsg[nMsgLen] = KTC_INLINE_PIC;
				*((WORD*)(szMsg + nMsgLen+1)) = m_pSelf->m_uDicePicIndex;
				nMsgLen += 3;
				nCurLen = sprintf(szBuf, "Tµi");
				memcpy(szMsg + nMsgLen, szBuf, nCurLen);
				nMsgLen += nCurLen;
			}
			else if(m_nRoomDiceRet == 2)
			{
				szMsg[nMsgLen++] = KTC_TAB;
				szMsg[nMsgLen] = KTC_INLINE_PIC;
				*((WORD*)(szMsg + nMsgLen+1)) = m_pSelf->m_uDicePicIndex;
				nMsgLen += 3;
				nCurLen = sprintf(szBuf, "Xÿu");
				memcpy(szMsg + nMsgLen, szBuf, nCurLen);
				nMsgLen += nCurLen;
			}
			m_TxtCurRoomTitle.SetText(szMsg);
		}
		else
			m_TxtCurRoomTitle.SetText("Ch≠a vµo phﬂng");
	}
}

void KUiChatRoom::UpdateInterface()
{
	if(m_pSelf && g_pCoreShell)
	{
		m_pSelf->m_RoomList.GetMessageListBox()->Clear();
		m_pSelf->m_MemberList.GetMessageListBox()->Clear();
		m_pSelf->m_BlackList.GetMessageListBox()->Clear();

		g_pCoreShell->OperationRequest(GOI_CHATROOM, -1, TRUE);
	}
}

void KUiChatRoom::ServerRequestRoomList(KOneRoom *pRoom)
{
	if(m_pSelf && pRoom)
	{
		unsigned int uTextBKColor = 0x00000000;
		int nMsgLen=0, nCurLen=0;
		char szMsg[128], szBuf[12];
		memset(szMsg, 0, sizeof(szMsg));
		nCurLen = strlen(pRoom->lpszRoomName);
		memcpy(szMsg + nMsgLen, pRoom->lpszRoomName, nCurLen);
		if(nCurLen < m_pSelf->m_nRoomNameLen)
			memset(szMsg + nMsgLen + nCurLen, 0x20, m_pSelf->m_nRoomNameLen - nCurLen);
		nMsgLen += m_pSelf->m_nRoomNameLen;
		nCurLen = strlen(pRoom->lpszRoomHost);
		memcpy(szMsg + nMsgLen, pRoom->lpszRoomHost, nCurLen);
		if(nCurLen < m_pSelf->m_nCreaterLen)
			memset(szMsg + nMsgLen + nCurLen, 0x20, m_pSelf->m_nCreaterLen - nCurLen);
		nMsgLen += m_pSelf->m_nCreaterLen;
		if(pRoom->nRoomLeftTime == -1)
			nCurLen = sprintf(szBuf, "N/A");
		else if(pRoom->nRoomLeftTime >0)
			nCurLen = sprintf(szBuf, "%dh%dp", pRoom->nRoomLeftTime / 60 / 60, pRoom->nRoomLeftTime / 60 % 60);
		else
			return;

		memcpy(szMsg + nMsgLen, szBuf, nCurLen);
		if(nCurLen < m_pSelf->m_nTimeLen)
			memset(szMsg + nMsgLen + nCurLen, 0x20, m_pSelf->m_nTimeLen - nCurLen);
		nMsgLen += m_pSelf->m_nTimeLen;
		nCurLen = sprintf(szBuf, "%dng≠Íi", pRoom->byRoomMemNum);
		memcpy(szMsg + nMsgLen, szBuf, nCurLen);
		if(nCurLen < m_pSelf->m_nMemNumLen)
			memset(szMsg + nMsgLen + nCurLen, 0x20, m_pSelf->m_nMemNumLen - nCurLen);
		nMsgLen += m_pSelf->m_nMemNumLen;
		if(KSG_StringGetValue(1, pRoom->nRoomParam, 5) ==1)
			uTextBKColor = 0x16ff0000;

		if(KSG_StringGetValue(1, pRoom->nRoomParam, 6)==1)
		{
			if(KSG_StringGetValue(1, pRoom->nRoomParam, 7)==0)
			{
				memset(szMsg + nMsgLen, 0x20, 3);
				nMsgLen+=3;
			}
			szMsg[nMsgLen] = '*';
			nMsgLen ++;
		}
		if(KSG_StringGetValue(1, pRoom->nRoomParam, 7)==1)
		{
			memset(szMsg + nMsgLen, 0x20, 1);
			nMsgLen+=1;
			szMsg[nMsgLen] = KTC_INLINE_PIC;
			*((WORD*)(szMsg + nMsgLen+1)) = m_pSelf->m_uDicePicIndex;
			nMsgLen += 3;
		}
		nMsgLen = TEncodeText(szMsg, nMsgLen);
		m_pSelf->m_RoomList.GetMessageListBox()->AddOneMessage(szMsg, nMsgLen, uTextBKColor, pRoom->nRoomParam);
	}
}

void KUiChatRoom::ServerRequestMemberList(KOneMem *pMem)
{
	if(m_pSelf)
	{
		int nMsgLen=0, nCurLen=0;
		char szMsg[128], szBuf[12];
		memset(szMsg, 0, sizeof(szMsg));
		nCurLen = strlen(pMem->lpszName);
		memcpy(szMsg + nMsgLen, pMem->lpszName, nCurLen);
		if(nCurLen < m_pSelf->m_nMemNameLen)
			memset(szMsg + nMsgLen + nCurLen, 0x20, m_pSelf->m_nMemNameLen - nCurLen);
		nMsgLen += m_pSelf->m_nMemNameLen;

		if(pMem->bIsRoomHost)
		{
			m_pSelf->m_nRoomDiceTime = 0;
			m_pSelf->m_nRoomDiceTime = KSG_StringGetValue(4, pMem->nMemParam, 1);
			m_pSelf->m_nRoomDiceRet = KSG_StringGetValue(1, pMem->nMemParam, 9);
			if(KSG_StringGetValue(4, pMem->nMemParam, 2) > 0)
			{
				nCurLen = sprintf(szBuf, "%dÆÂng", KSG_StringGetValue(4, pMem->nMemParam, 2));
				memcpy(szMsg + nMsgLen, szBuf, nCurLen);
				nMsgLen += nCurLen;
			}
			m_pSelf->m_MemberList.GetMessageListBox()->AddOneMessage(szMsg, nMsgLen, 0x16ff0000, pMem->nMemIndex);
		}
		else
		{
			if (KSG_StringGetValue(4, pMem->nMemParam, 1) > 0)
			{
				if(KSG_StringGetValue(4, pMem->nMemParam, 1)==1)
					nCurLen = sprintf(szBuf, "%dTµi", KSG_StringGetValue(4, pMem->nMemParam, 2));
				else if(KSG_StringGetValue(4, pMem->nMemParam, 1)==2)
					nCurLen = sprintf(szBuf, "%dXÿu", KSG_StringGetValue(4, pMem->nMemParam, 2));
				else if(KSG_StringGetValue(4, pMem->nMemParam, 1)==3)
					nCurLen = sprintf(szBuf, "+%dÆÂng", KSG_StringGetValue(4, pMem->nMemParam, 2));
				else if(KSG_StringGetValue(4, pMem->nMemParam, 1)==4)
					nCurLen = sprintf(szBuf, "-%dÆÂng", KSG_StringGetValue(4, pMem->nMemParam, 2));
				memcpy(szMsg + nMsgLen, szBuf, nCurLen);
				nMsgLen += nCurLen;
			}
			m_pSelf->m_MemberList.GetMessageListBox()->AddOneMessage(szMsg, nMsgLen, 0, pMem->nMemIndex);
		}
	}
}

void KUiChatRoom::ServerRequestBlackList(KOneMem *pMem)
{
	if(m_pSelf)
	{
		m_pSelf->m_BlackList.GetMessageListBox()->AddOneMessage(pMem->lpszName, strlen(pMem->lpszName), 0, pMem->nMemIndex);
	}
}

void KUiChatRoom::Join(const char* lpszName, int nFigure)
{
	if(m_pSelf)
	{
		if(m_pSelf->m_bIsInRoom == FALSE)
		{
			sprintf(m_pSelf->m_szRoomName, "Phﬂng hi÷n tπi: %s", lpszName);
			m_pSelf->m_nRoomFigure = nFigure;
			m_pSelf->m_bIsInRoom = TRUE;
			if(nFigure == CHATROOM_HOST)
			{
				m_pSelf->m_BtnEnterRoom.Hide();
				m_pSelf->m_BtnDelRoom.Show();
				m_pSelf->m_BtnOpenDice.Show();
				m_pSelf->m_BtnHightDice.Hide();
				m_pSelf->m_BtnLowDice.Hide();
				m_pSelf->m_BtnPassword.Show();
			}
			else if(nFigure == CHATROOM_MEMBER)
			{
				m_pSelf->m_BtnEnterRoom.Show();
				m_pSelf->m_BtnDelRoom.Hide();
				m_pSelf->m_BtnOpenDice.Hide();
				m_pSelf->m_BtnHightDice.Show();
				m_pSelf->m_BtnLowDice.Show();
				m_pSelf->m_BtnPassword.Hide();
			}
		}
	}
}

void KUiChatRoom::Leave()
{
	if(m_pSelf)
	{
		m_pSelf->m_bIsInRoom = FALSE;
		m_pSelf->m_MemberList.GetMessageListBox()->Clear();
		m_pSelf->m_BtnEnterRoom.Show();
		m_pSelf->m_BtnDelRoom.Hide();
		m_pSelf->m_BtnOpenDice.Hide();
		m_pSelf->m_BtnHightDice.Hide();
		m_pSelf->m_BtnLowDice.Hide();
		m_pSelf->m_BtnPassword.Hide();
	}
}