/*****************************************************************************************
//	½çÃæ--ÏûÏ¢ÖÐÐÄÃæ°å
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-13
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "UiMsgCentrePad.h"
#include "UiChatCentre.h"
#include "UiFaceSelector.h"
#include "UiSysMsgCentre.h"
#include "UiNewsSysMsg.h"
#include "../UiBase.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KStrBase.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../UiSoundSetting.h"

#include "UiPlayerBar.h"
#include "UiPlayerControlBar.h"

#include "KRelayProtocol.h"
#include "KProtocolDef.h"

#include "UIGame.h"

#define		SEL_CHANNEL_MENU		1

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;

int VerticalSplitTwoWindow(KWndWindow* pLeft, KWndWindow* pRight, int nAt)
{
	if (pLeft == NULL || pRight == NULL)
		return 0;
	int nL_x, nL_y;
	pLeft->GetPosition(&nL_x, &nL_y);
	int nR_x, nR_y;
	pRight->GetPosition(&nR_x, &nR_y);
	int nL_w, nL_h;
	pLeft->GetSize(&nL_w, &nL_h);
	int nR_w, nR_h;
	pRight->GetSize(&nR_w, &nR_h);
	if (nL_x + nL_w > nR_x + nR_w)
		return 0;
	if ((nAt - nL_x < 0) || (nR_w - (nAt - nL_x - nL_w) < 0))
		return 0;
	if (nAt - nL_x - nL_w == 0)	//²»ÓÃµ÷ÕûÁË£¬Æ«ÒÆÁ¿Îª0
		return 0;

	pLeft->SetSize(nAt - nL_x, nL_h);
	pRight->SetPosition(nR_x + (nAt - nL_x - nL_w), nR_y);
	pRight->SetSize(nR_w - (nAt - nL_x - nL_w), nR_h);
	return nAt;
}

///////////////////////////////////////////////////////////////////////////////////////////
int KSysMsgCentrePad::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	static int nUpSpeed = 0;
	static int nDownSpeed = 0;
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_OpenSysButton)
		{
			if (m_OpenSysButton.IsButtonChecked())
				m_SysRoom.Show();
			else
				m_SysRoom.Hide();
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_UpButton)
		{
			nUpSpeed = 0;
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_DownButton)
		{
			nDownSpeed = 0;
			return 1;
		}	
		
	}
	else if (uMsg == WND_N_BUTTON_DOWN || WND_N_BUTTON_HOLD)
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_UpButton)
		{
			if (nUpSpeed % 5 == 0)
			{
				m_SysRoom.ClearHideLine();
				m_SysRoom.SetFirstShowLine(max(m_SysRoom.GetFirstShowLine() - 1, 0));
			}
			nUpSpeed++;
			return 1;
		}
		if (uParam == (unsigned int)(KWndWindow*)&m_DownButton)
		{
			if (nDownSpeed % 5 == 0)
			{
				m_SysRoom.ClearHideLine();
				int nLine = m_SysRoom.GetItemLineCount(m_SysRoom.GetMsgCount()) - m_SysRoom.GetMaxShowLine();
				m_SysRoom.SetFirstShowLine(min(m_SysRoom.GetFirstShowLine() + 1, nLine));
			}
			nDownSpeed++;
			return 1;
		}
	}
	return KWndWindow::WndProc(uMsg, uParam, nParam);
}

void KSysMsgCentrePad::ScrollBottom()
{
	m_SysRoom.ClearHideLine();
	int nCount = m_SysRoom.GetMsgCount();
	if (nCount > 0)
	{
		int nLine = m_SysRoom.GetItemLineCount(nCount) - m_SysRoom.GetMaxShowLine();
		m_SysRoom.SetFirstShowLine(max(nLine, 0));
	}
}
///////////////////////////////////////////////////////////////////////////////////////////

#define	SCHEME_INI		"UiMsgCentrePad.ini"

KUiMsgCentrePad* KUiMsgCentrePad::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiMsgCentrePad* KUiMsgCentrePad::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiMsgCentrePad;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiMsgCentrePad::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
		{
			m_pSelf->Clear();
			m_pSelf->Hide();
		}
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

KUiMsgCentrePad::KUiMsgCentrePad()
{
	m_uAutoDelMsgInterval = 0;
	m_nSizeBtnDragOffsetY = 0;
	m_bSizeUp = false;
	m_bExpandWnd = false;
	m_bSizingWnd = false;
	m_bShowShadow = false;
	m_nMinTopPos = 20;
	m_nMaxBottomPos = 500;

	m_nChannelsResource = 0;
	m_nActivateChannels = 0;
	m_pActivateChannel = NULL;

	m_nCheckOnPicIndex = -1;
	m_nCheckOffPicIndex = -1;

	m_nCurChannelSel = CHANNELTAB_PUBLIC;
	m_NameColor.Color_dw = 0;
	m_BorderNameColor.Color_dw = 0;
	memset(m_szReply, 0, sizeof(m_szReply));
}


void KUiMsgCentrePad::SetAutoDelMsgInterval(unsigned int uInterval /*= 0*/)
{
	m_uAutoDelMsgInterval = uInterval;
	if (m_uAutoDelMsgInterval)
		m_uLastDelMsgTime = IR_GetCurrentTime();
}

void KUiMsgCentrePad::SystemMessageArrival(const char* pMsgBuff, unsigned short nMsgLength)//cong bo
{
	if (m_pSelf && pMsgBuff && nMsgLength > 0)
	{
		if (m_pSelf->m_nDefaultChannelResource >= 0 && m_pSelf->m_nDefaultChannelResource < m_pSelf->m_nChannelsResource)
		{			
			m_pSelf->ChannelMessageArrival(m_pSelf->FindActivateChannelIndex(m_pSelf->m_ChannelsResource[m_pSelf->m_nDefaultChannelResource].cTitle),
											m_pSelf->m_DefaultChannelSendName,
											pMsgBuff, nMsgLength, &m_pSelf->m_Sys.m_SysRoom, false);
			m_pSelf->m_Sys.ScrollBottom();
		}
	}
}

void KUiMsgCentrePad::ChannelMessageArrival(int nChannelIndex, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, KWndMessageListBox* pM, bool bName, const char* pItem, BYTE btSomeFlag)
{
	if (nChannelIndex < 0 || nChannelIndex >= m_nActivateChannels)
		return;

	char Buffer[MAX_MESSAGE_LENGTH];
	int nLen = 0;
	if (szSendName)
		nLen = strlen(szSendName);

	if (nLen <= 0)	
	{
		return;
	}

	if (!m_pActivateChannel[nChannelIndex].bSubscribe)
		return;

	int nOffset = 0;

	KRColor uColor;
	uColor.Color_dw = 0xFFFFFFFF;
	if (m_pActivateChannel[nChannelIndex].ResourceIndex >= 0)
	{
		Buffer[nOffset++] = KTC_INLINE_PIC;
		*((WORD*)(Buffer + nOffset)) = m_ChannelsResource[m_pActivateChannel[nChannelIndex].ResourceIndex].nTextPicIndex;
		nOffset += 2;
		uColor = m_ChannelsResource[m_pActivateChannel[nChannelIndex].ResourceIndex].uTextColor;
		Buffer[nOffset++] = uColor.Color_b.a;
		Buffer[nOffset++] = uColor.Color_b.r;
		Buffer[nOffset++] = uColor.Color_b.g;
		Buffer[nOffset++] = uColor.Color_b.b;

		if (btSomeFlag & 0x02)
		{
			if (m_pActivateChannel[nChannelIndex].ResourceIndex == CH_TEAM)
			{
				KRColor uTextColor;
				uTextColor.Color_dw = 0xffffffff;
				uTextColor.Color_b.a = KTC_COLOR;
				uColor = uTextColor;
			}
		}

		if (btSomeFlag & 0x04)
		{
			if (m_pActivateChannel[nChannelIndex].ResourceIndex == CH_TONG)
			{
				KRColor uTextColor;
				uTextColor.Color_dw = 0xffdc7500;
				uTextColor.Color_b.a = KTC_COLOR;
				uColor = uTextColor;
			}
		}

		Buffer[nOffset++] = uColor.Color_b.a;
		Buffer[nOffset++] = uColor.Color_b.r;
		Buffer[nOffset++] = uColor.Color_b.g;
		Buffer[nOffset++] = uColor.Color_b.b;
		
		uColor = m_ChannelsResource[m_pActivateChannel[nChannelIndex].ResourceIndex].uTextBorderColor;
		Buffer[nOffset++] = uColor.Color_b.a;
		Buffer[nOffset++] = uColor.Color_b.r;
		Buffer[nOffset++] = uColor.Color_b.g;
		Buffer[nOffset++] = uColor.Color_b.b;

		if (m_ChannelsResource[m_pActivateChannel[nChannelIndex].ResourceIndex].szSoundFileName[0] != 0)
			UiSoundPlay(m_ChannelsResource[m_pActivateChannel[nChannelIndex].ResourceIndex].szSoundFileName);
	}

	if (pMsgBuff[0] != '\t' && bName)
	{
		if (btSomeFlag & 0x01)
		{
			KRColor uTextColor;
			uTextColor.Color_dw = 0xffee1289;
			uTextColor.Color_b.a = KTC_COLOR;
			
			Buffer[nOffset++] = uTextColor.Color_b.a;
			Buffer[nOffset++] = uTextColor.Color_b.r;
			Buffer[nOffset++] = uTextColor.Color_b.g;
			Buffer[nOffset++] = uTextColor.Color_b.b;
		}
		else
		{
			Buffer[nOffset++] = m_NameColor.Color_b.a;
			Buffer[nOffset++] = m_NameColor.Color_b.r;
			Buffer[nOffset++] = m_NameColor.Color_b.g;
			Buffer[nOffset++] = m_NameColor.Color_b.b;
		}

		Buffer[nOffset++] = m_BorderNameColor.Color_b.a;
		Buffer[nOffset++] = m_BorderNameColor.Color_b.r;
		Buffer[nOffset++] = m_BorderNameColor.Color_b.g;
		Buffer[nOffset++] = m_BorderNameColor.Color_b.b;

		if (btSomeFlag & 0x08)
		{
			if (m_pActivateChannel[nChannelIndex].ResourceIndex == CH_NEARBY || 
				m_pActivateChannel[nChannelIndex].ResourceIndex == CH_WORLD ||
				m_pActivateChannel[nChannelIndex].ResourceIndex == CH_CITY)
			{
				int nNameLen = strlen((char*)defTONG_VICEROY_AGNAME);
    			memcpy(Buffer + nOffset, defTONG_VICEROY_AGNAME, nNameLen);
    			nOffset += nNameLen;
			}
		}
    	memcpy(Buffer + nOffset, szSendName, nLen);
    	nOffset += nLen;
    	Buffer[nOffset++] = ':';

		Buffer[nOffset++] = KTC_COLOR_RESTORE;
		Buffer[nOffset++] = KTC_BORDER_RESTORE;
	}

	if (pMsgBuff[0] == '\t')
	{
		nMsgLength--;
		memcpy(&Buffer[nOffset], pMsgBuff+1, nMsgLength);
	}
	else
		memcpy(&Buffer[nOffset], pMsgBuff, nMsgLength);

	KItemBtn Item;
	Item.Release();
	if(pItem && pItem[0])
	{
		memcpy(Item.m_pItem, pItem, sizeof(Item.m_pItem));
		char m_pItem[MAX_SENTENCE_LENGTH];
		memset(m_pItem, 0, sizeof(m_pItem));
		memcpy(m_pItem, pItem, sizeof(m_pItem));
		int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&m_pItem);
		if(nIdx)
		{
			char szName[82];
			g_pCoreShell->GetGameData(GDI_ITEM_NAME, (unsigned int)&szName, nIdx);
			int nItemLen = strlen(szName);
			memmove(szName+1, szName, nItemLen);
			szName[0] = '<';
			nItemLen++;
			szName[nItemLen] = '>';
			nItemLen++;
			szName[nItemLen] = '\0';
			int nPos = 0;
			while(nPos < nMsgLength)
			{
				if(pMsgBuff[nPos] == '<')
				{
					int i;
					for(i=0; i < nItemLen; i++)
					{
						if(pMsgBuff[nPos+i] != szName[i])
							break;
					}
					if(i == nItemLen)
						break;
				}
				nPos++;
			}
			int nLastPos = 0, nTotalLen = 0;
			int nMaxLineW = pM->GetMaxLineWidth();
			int nMaxLineL = 0;
			Item.nLine = TGetEncodedItemChatLineCount(Buffer, nOffset+nPos, nMaxLineW, nMaxLineL, pM->GetFontSize(), Item.nPic, nLastPos, nTotalLen, 0, 0, true);
			Item.nLeft = nLastPos + Item.nPic * 3;
			if(Item.nLeft == 0 && (nTotalLen % nMaxLineL > nMaxLineL - 5 || nTotalLen % nMaxLineL < 5))
				Item.nLine++;
			if((Item.nLeft + nItemLen) <= (nMaxLineW - 3))
			{
				memmove(Buffer+nOffset+nPos+1, Buffer+nOffset+nPos, nMsgLength-nPos);
				nMsgLength++;
				memset(Buffer+nOffset+nPos, 0x20, nItemLen+1);
				Buffer[nOffset+nMsgLength] = '\0';
				Item.nLeft += 1;
			}
			else
			{
				memmove(Buffer+nOffset+nPos+1, Buffer+nOffset+nPos, nMsgLength-nPos);
				nMsgLength++;
				memset(Buffer+nOffset+nPos, 0x20, nItemLen+1);
				Buffer[nOffset+nPos] = KTC_ENTER;
				if(nItemLen > (nMaxLineW - 3))
				{
					memcpy(Buffer+nOffset+nPos+nItemLen-(nItemLen-nMaxLineW), Buffer+nOffset+nPos+nItemLen, nMsgLength+nOffset-(nOffset+nPos+nItemLen));
					nMsgLength -= nItemLen-nMaxLineW;
					Buffer[nOffset+nMsgLength] = '\0';
				}
				else
					Buffer[nOffset+nMsgLength] = '\0';
				Item.nPic = 0;
				Item.nLeft = 0;
				Item.nLine++;
			}
			g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, nIdx);
		}
	}

	nOffset += FilterTextColor(Buffer + nOffset, nMsgLength, uColor);
	Buffer[nOffset] = 0;

	if (pM)
	{
		pM->AddOneMessage(Buffer, nOffset, 0, -1, &Item);
	}

	if (m_pActivateChannel[nChannelIndex].ResourceIndex == CH_CHATROOM)
	{
		m_MessageChannel[CHANNELTAB_CHATROOM].GetMessageListBox()->AddOneMessage(Buffer, nOffset, 0, -1, &Item);
		if(m_MessageChannel[m_nCurChannelSel].GetScrollBar()->IsDraggingSlide()==false)
			m_MessageChannel[CHANNELTAB_CHATROOM].GetScrollBar()->SetScrollPos(m_MessageChannel[CHANNELTAB_CHATROOM].GetScrollBar()->GetMaxValue());
	}
	else if (m_pActivateChannel[nChannelIndex].ResourceIndex == CH_FACTION)
	{
		m_MessageChannel[CHANNELTAB_FACTION].GetMessageListBox()->AddOneMessage(Buffer, nOffset, 0, -1, &Item);
		if(m_MessageChannel[m_nCurChannelSel].GetScrollBar()->IsDraggingSlide()==false)
			m_MessageChannel[CHANNELTAB_FACTION].GetScrollBar()->SetScrollPos(m_MessageChannel[CHANNELTAB_FACTION].GetScrollBar()->GetMaxValue());
	}
	else if (m_pActivateChannel[nChannelIndex].ResourceIndex == CH_TONG)
	{
		m_MessageChannel[CHANNELTAB_TONG].GetMessageListBox()->AddOneMessage(Buffer, nOffset, 0, -1, &Item);
		if(m_MessageChannel[m_nCurChannelSel].GetScrollBar()->IsDraggingSlide()==false)
			m_MessageChannel[CHANNELTAB_TONG].GetScrollBar()->SetScrollPos(m_MessageChannel[CHANNELTAB_TONG].GetScrollBar()->GetMaxValue());
	}
	m_uLastDelMsgTime = IR_GetCurrentTime();
}

void KUiMsgCentrePad::MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, KWndMessageListBox* pM, const char* pItem)
{
	KRColor uColor;
	KRColor uBKColor;
	KRColor uBorderColor;
	WORD nPic = 0;
	bool bSelf = false;
	if (KUiPlayerBar::IsSelfName(szSourceName))
	{
		bSelf = true;
		uColor = m_MSNInfo.uTextColorSelf;
		nPic = m_MSNInfo.nSelfTextPicIndex;
		uBKColor = m_MSNInfo.uTextBKColorSelf;
		uBorderColor = m_MSNInfo.uTextBorderColorSelf;
	}
	else
	{
		bool bFriend = KUiChatCentre::IsMyFriend(szSourceName);
		if (bFriend)
		{
			uColor = m_MSNInfo.uTextFriendColor;
			nPic = m_MSNInfo.nFriendTextPicIndex;

			uBorderColor = m_MSNInfo.uTextBorderFriendColor;

			if (m_MSNInfo.szFriendSoundFileName[0] != 0)
				UiSoundPlay(m_MSNInfo.szFriendSoundFileName);
		}
		else
		{
			uColor = m_MSNInfo.uTextColorUnknown;
			nPic = m_MSNInfo.nStrangerTextPicIndex;
			uBorderColor = m_MSNInfo.uTextBorderColorUnknown;
		}
		uBKColor.Color_dw = 0;
	}

	ShowMSNMessage(bSelf ? szSendName : szSourceName, pMsgBuff, nMsgLength, pM, uColor, uBKColor, uBorderColor, nPic, pItem);

	if (!bSelf)
	{
		if (pMsgBuff[0] != '\t' && m_szReply[0])
		{
			int nMsgLen = strlen(m_szReply);
			if (KUiPlayerBar::IsCanSendMessage(m_szReply, nMsgLen, szSourceName, -1))
			{
				char Buffer[1536];
				nMsgLen = KUiFaceSelector::ConvertFaceText(Buffer, m_szReply, nMsgLen);
				nMsgLen = TEncodeText(Buffer, nMsgLen);
				memmove(Buffer + 1, Buffer, nMsgLen);
				Buffer[0] = '\t';
				nMsgLen++;
				KUiPlayerBar::OnSendSomeoneMessage(szSourceName, Buffer, nMsgLen);
			}
		}
	}
}

void KUiMsgCentrePad::ShowMSNMessage(char* szName, const char* pMsgBuff, unsigned short nMsgLength, KWndMessageListBox* pM, KRColor uColor, KRColor uBKColor, KRColor uBorderColor, WORD nPic, const char* pItem)
{
	char Buffer[MAX_MESSAGE_LENGTH];
	int nOffset = 0;
	Buffer[nOffset++] = KTC_INLINE_PIC;
	*((WORD*)(Buffer + nOffset)) = nPic;
	nOffset += 2;

	Buffer[nOffset++] = uColor.Color_b.a;
	Buffer[nOffset++] = uColor.Color_b.r;
	Buffer[nOffset++] = uColor.Color_b.g;
	Buffer[nOffset++] = uColor.Color_b.b;

	Buffer[nOffset++] = uBorderColor.Color_b.a;
	Buffer[nOffset++] = uBorderColor.Color_b.r;
	Buffer[nOffset++] = uBorderColor.Color_b.g;
	Buffer[nOffset++] = uBorderColor.Color_b.b;

	Buffer[nOffset++] = m_NameColor.Color_b.a;
	Buffer[nOffset++] = m_NameColor.Color_b.r;
	Buffer[nOffset++] = m_NameColor.Color_b.g;
	Buffer[nOffset++] = m_NameColor.Color_b.b;

	Buffer[nOffset++] = m_BorderNameColor.Color_b.a;
	Buffer[nOffset++] = m_BorderNameColor.Color_b.r;
	Buffer[nOffset++] = m_BorderNameColor.Color_b.g;
	Buffer[nOffset++] = m_BorderNameColor.Color_b.b;

	int nLen = 0;
	nLen = strlen(szName);

	memcpy(Buffer + nOffset, szName, nLen);
	nOffset += nLen;

	Buffer[nOffset++] = ':';

	Buffer[nOffset++] = KTC_COLOR_RESTORE;
	Buffer[nOffset++] = KTC_BORDER_RESTORE;
	
	if (pMsgBuff[0] == '\t')
	{
		nMsgLength--;
		memcpy(&Buffer[nOffset], pMsgBuff+1, nMsgLength);
	}
	else
		memcpy(&Buffer[nOffset], pMsgBuff, nMsgLength);

	KItemBtn Item;
	Item.Release();
	if(pItem && pItem[0])
	{
		memcpy(Item.m_pItem, pItem, sizeof(Item.m_pItem));
		char m_pItem[MAX_SENTENCE_LENGTH];
		memset(m_pItem, 0, sizeof(m_pItem));
		memcpy(m_pItem, pItem, sizeof(m_pItem));
		int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&m_pItem);
		if(nIdx)
		{
			char szName[82];
			g_pCoreShell->GetGameData(GDI_ITEM_NAME, (unsigned int)&szName, nIdx);
			int nItemLen = strlen(szName);
			memmove(szName+1, szName, nItemLen);
			szName[0] = '<';
			nItemLen++;
			szName[nItemLen] = '>';
			nItemLen++;
			szName[nItemLen] = '\0';
			int nPos = 0;
			while(nPos < nMsgLength)
			{
				if(pMsgBuff[nPos] == '<')
				{
					int i;
					for(i=0; i < nItemLen; i++)
					{
						if(pMsgBuff[nPos+i] != szName[i])
							break;
					}
					if(i == nItemLen)
						break;
				}
				nPos++;
			}
			int nLastPos = 0, nTotalLen = 0;
			int nMaxLineW = pM->GetMaxLineWidth();
			int nMaxLineL = 0;
			Item.nLine = TGetEncodedItemChatLineCount(Buffer, nOffset+nPos, nMaxLineW, nMaxLineL, pM->GetFontSize(), Item.nPic, nLastPos, nTotalLen, 0, 0, true);
			Item.nLeft = nLastPos + Item.nPic * 3;
			if(Item.nLeft == 0 && (nTotalLen % nMaxLineL > nMaxLineL - 5 || nTotalLen % nMaxLineL < 5))
				Item.nLine++;
			if((Item.nLeft + nItemLen) <= (nMaxLineW - 3))
			{
				memmove(Buffer+nOffset+nPos+1, Buffer+nOffset+nPos, nMsgLength-nPos);
				nMsgLength++;
				memset(Buffer+nOffset+nPos, 0x20, nItemLen+1);
				Buffer[nOffset+nMsgLength] = '\0';
				Item.nLeft += 1;
			}
			else
			{
				memmove(Buffer+nOffset+nPos+1, Buffer+nOffset+nPos, nMsgLength-nPos);
				nMsgLength++;
				memset(Buffer+nOffset+nPos, 0x20, nItemLen+1);
				Buffer[nOffset+nPos] = KTC_ENTER;
				if(nItemLen > (nMaxLineW - 3))
				{
					memcpy(Buffer+nOffset+nPos+nItemLen-(nItemLen-nMaxLineW), Buffer+nOffset+nPos+nItemLen, nMsgLength+nOffset-(nOffset+nPos+nItemLen));
					nMsgLength -= nItemLen-nMaxLineW;
					Buffer[nOffset+nMsgLength] = '\0';
				}
				else
					Buffer[nOffset+nMsgLength] = '\0';
				Item.nPic = 0;
				Item.nLeft = 0;
				Item.nLine++;
			}
			g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, nIdx);
		}
	}

	nOffset += FilterTextColor(Buffer + nOffset, nMsgLength, uColor);
	Buffer[nOffset] = 0;

	m_MessageChannel[CHANNELTAB_PUBLIC].GetMessageListBox()->AddOneMessage(Buffer, nOffset, uBKColor.Color_dw, -1, &Item);
	m_MessageChannel[CHANNELTAB_PRIVATE].GetMessageListBox()->AddOneMessage(Buffer, nOffset, uBKColor.Color_dw, -1, &Item);
	if(m_MessageChannel[m_nCurChannelSel].GetScrollBar()->IsDraggingSlide()==false)
	{
		m_MessageChannel[CHANNELTAB_PUBLIC].GetScrollBar()->SetScrollPos(m_MessageChannel[CHANNELTAB_PUBLIC].GetScrollBar()->GetMaxValue());
		m_MessageChannel[CHANNELTAB_PRIVATE].GetScrollBar()->SetScrollPos(m_MessageChannel[CHANNELTAB_PRIVATE].GetScrollBar()->GetMaxValue());
	}
		
	m_uLastDelMsgTime = IR_GetCurrentTime();
}


int KUiMsgCentrePad::NewChannelMessageArrival(DWORD nChannelID, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, const char* pItem, BYTE btSomeFlag)
{
	if (m_pSelf && pMsgBuff)
	{
		int nChannelIndex = m_pSelf->FindActivateChannelIndex(nChannelID);
		int nID = m_pSelf->m_pActivateChannel[nChannelIndex].ResourceIndex;
		if (nChannelIndex >= 0)
		{
			m_pSelf->ChannelMessageArrival(nChannelIndex, szSendName, pMsgBuff, nMsgLength, m_pSelf->m_MessageChannel[CHANNELTAB_PUBLIC].GetMessageListBox(), true, pItem, btSomeFlag);
			if(m_pSelf->m_MessageChannel[m_pSelf->m_nCurChannelSel].GetScrollBar()->IsDraggingSlide()==false)
				m_pSelf->m_MessageChannel[CHANNELTAB_PUBLIC].GetScrollBar()->SetScrollPos(
					m_pSelf->m_MessageChannel[CHANNELTAB_PUBLIC].GetScrollBar()->GetMaxValue());
		}
		if (nID == CH_SYSTEM)
		{
			m_pSelf->ChannelMessageArrival(nChannelIndex, szSendName, pMsgBuff, nMsgLength, m_pSelf->m_MessageChannel[CHANNELTAB_SYSTEM].GetMessageListBox(), true, pItem, btSomeFlag);
			if(m_pSelf->m_MessageChannel[m_pSelf->m_nCurChannelSel].GetScrollBar()->IsDraggingSlide()==false)
				m_pSelf->m_MessageChannel[CHANNELTAB_SYSTEM].GetScrollBar()->SetScrollPos(
					m_pSelf->m_MessageChannel[CHANNELTAB_SYSTEM].GetScrollBar()->GetMaxValue());
		}
		return nChannelIndex;
	}

	return -1;
}

KUiMsgCentrePad* KUiMsgCentrePad::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiMsgCentrePad::NewMSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, const char* pItem)
{
	if (m_pSelf && pMsgBuff)
	{
		m_pSelf->MSNMessageArrival(szSourceName, szSendName, pMsgBuff, nMsgLength, m_pSelf->m_MessageChannel[CHANNELTAB_PRIVATE].GetMessageListBox(), pItem);
	}
}

void KUiMsgCentrePad::ShowSomeoneMessage(char* szSourceName, const char* pMsgBuff, unsigned short nMsgLength)
{
	if (m_pSelf && pMsgBuff)
	{
		KRColor uColor;
		KRColor uBKColor;
		KRColor uBorderColor;
		WORD nPic = 0;

		bool bFriend = KUiChatCentre::IsMyFriend(szSourceName);
		if (bFriend)
		{
			uColor = m_pSelf->m_MSNInfo.uTextFriendColor;
			nPic = m_pSelf->m_MSNInfo.nFriendTextPicIndex;

			uBorderColor = m_pSelf->m_MSNInfo.uTextBorderFriendColor;

			if (m_pSelf->m_MSNInfo.szFriendSoundFileName[0] != 0)
				UiSoundPlay(m_pSelf->m_MSNInfo.szFriendSoundFileName);
		}
		else
		{
			uColor = m_pSelf->m_MSNInfo.uTextColorUnknown;
			nPic = m_pSelf->m_MSNInfo.nStrangerTextPicIndex;
			uBorderColor = m_pSelf->m_MSNInfo.uTextBorderColorUnknown;
		}
		uBKColor.Color_dw = 0;

		m_pSelf->ShowMSNMessage(szSourceName, pMsgBuff, nMsgLength, m_pSelf->m_MessageChannel[CHANNELTAB_PRIVATE].GetMessageListBox(), uColor, uBKColor, uBorderColor, nPic);
	}
}

void KUiMsgCentrePad::OpenChannel(char* channelName, DWORD nChannelID, BYTE cost)
{
	if (nChannelID == -1)
		return;

	if (m_pSelf)
	{
		char ChannelName[__X_NAME_LEN_];
		int nChannelIndex = -1;
		for (int n = 0; n < m_pSelf->m_nChannelsResource; n++)
		{
			ChannelName[0];
			if (m_pSelf->m_ChannelsResource[n].cFormatName[0] == 0)
			{
				return;
			}
			if (!m_pSelf->ReplaceSpecialField(ChannelName, m_pSelf->m_ChannelsResource[n].cFormatName))
				continue;
			if (ChannelName[0] == 0)
				continue;
			if (strcmp(channelName, ChannelName) == 0)
			{
				nChannelIndex = n;
				break;
			}
		}
		
		if (nChannelIndex >= 0)
		{
			KChannelActivateInfo Info;
			Info.nChannelID = nChannelID;
			Info.cost = cost;
			Info.ResourceIndex = nChannelIndex;
			Info.bSubscribe = true;
			m_pSelf->SendChannelSubscribe(m_pSelf->AddActivateChannel(Info), true);
		}
	}
}

void KUiMsgCentrePad::CloseActivateChannel(int nChannelIndex)
{
	if (nChannelIndex < 0 || nChannelIndex >= m_nActivateChannels)
		return;

	if (m_pActivateChannel[nChannelIndex].bSubscribe)
		SendChannelSubscribe(nChannelIndex, false);

	ReleaseActivateChannel(nChannelIndex);
}

void KUiMsgCentrePad::QueryAllChannel()
{
	if (m_pSelf)
	{
		for (int n = 0; n < m_pSelf->m_nChannelsResource; n++)
		{
			m_pSelf->SendQueryChannelID(n);
		}
	}
}

void KUiMsgCentrePad::ReplaceChannelName(char* szDest, size_t nDestSize, char* szSrc)
{
	if (m_pSelf && szSrc[0] != 0)
	{
		for (int n = 0; n < m_pSelf->m_nChannelsResource; n++)
		{
			for (int m = 0; m < MAX_CHANNELRESOURCE; m++)
			{
				if (m_pSelf->m_ChannelsResource[n].cShortTitle[m][0] != 0)
				{
					if (strcmpi(szSrc, m_pSelf->m_ChannelsResource[n].cShortTitle[m]) == 0)
					{
						strncpy(szDest, m_pSelf->m_ChannelsResource[n].cTitle, nDestSize);
						return;
					}
				}
			}
		}
	}
	strncpy(szDest, szSrc, nDestSize);
}

static char s_keys [][__X_NAME_LEN_] =
{
	"Team#",
	"Faction#",
	"Tong#",
	"Msgr#",
	"ChatRoom#",
	"\\S",
	"GM",
};

bool KUiMsgCentrePad::ReplaceSpecialField(char* szDest, char* szSrc)
{
	char s_keyDests [][__X_NAME_LEN_] =
	{
		"",
		"",
		"",
		"",
		"",
		""
	};

	KUiPlayerTeam	Team;
	Team.nTeamServerID = -1;
	g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&Team, 0);
	if (Team.nTeamServerID >= 0)
	{
		sprintf(s_keyDests[0], "%d", Team.nTeamServerID);
	}
	KUiPlayerBaseInfo self;
	self.nCurFaction = -1;
	self.nCurTong = 0;
	self.nMissionGroup = -1;
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&self, 0);
	if (self.nCurFaction >= 0)
	{
		sprintf(s_keyDests[1], "%d", self.nCurFaction);
	}
	if (self.nCurTong != 0)
	{
		sprintf(s_keyDests[2], "%u", self.nCurTong);
	}	
	if (self.nMissionGroup >= 0)
	{
		sprintf(s_keyDests[3], "%d", self.nMissionGroup);
	}		
	if (self.nRoomId >= 0)
	{
		sprintf(s_keyDests[4], "%d", self.nRoomId);
	}	
	int nKeySize = sizeof(s_keys) / __X_NAME_LEN_;
	char key[__X_NAME_LEN_];

	if (m_pSelf)
	{
		int n = 0;
		int m = 0;
		int nKey = -1;
		while(szSrc[n] != 0)
		{
			if (szSrc[n] == '<')
			{
				nKey = 0;
				memset(key, 0, sizeof(key));
			}
			else if (szSrc[n] == '>')
			{
				for (int i = 0; i < nKeySize; i++)
				{
					if (strcmpi(s_keys[i], key) == 0)	//Ö§³ÖµÄ¹Ø¼ü×Ö
					{
						if (s_keyDests[i][0] == 0)
							return false;
						strcpy(szDest + m, s_keyDests[i]);
						m += strlen(s_keyDests[i]);
						nKey = -1;
						break;
					}
				}

				if (nKey >= 0)	//²»Ö§³ÖµÄ¹Ø¼ü×Ö
				{
					strcpy(szDest + m, key);
					m += nKey;
					nKey = -1;
					return false;
				}
			}
			else
			{
				if (nKey >= 0)
				{
					key[nKey++] = szSrc[n];
				}
				else
				{
					szDest[m++] = szSrc[n];
				}
			}

			n++;
		}

		szDest[m] = 0;
	}

	return true;
}

void KUiMsgCentrePad::CloseSelfChannel(SelfChannel type)
{
	if (m_pSelf)
	{
		switch (type)
		{
		case ch_Team:
		case ch_Faction:
		case ch_Tong:
		case ch_Msgr:
		case ch_Cr:
		case ch_Screen:
			{
				m_pSelf->CloseActivateChannel(m_pSelf->FindActivateChannelIndexByKey(s_keys[type]));
			}
			break;
		default:
			return;
		}
	}
}

bool KUiMsgCentrePad::GetChannelSubscribe(int nChannelIndex)
{
	if (m_pSelf && nChannelIndex >= 0 && nChannelIndex < m_pSelf->m_nActivateChannels)
	{
		return m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe;
	}
	return false;
}

bool KUiMsgCentrePad::IsChannelType(int nChannelIndex, SelfChannel type)
{
	if (m_pSelf && nChannelIndex >= 0 && nChannelIndex < m_pSelf->m_nActivateChannels)
	{
		switch (type)
		{
		case ch_Team:
		case ch_Faction:
		case ch_Tong:
		case ch_Msgr:
		case ch_Cr:
		case ch_Screen:
		case ch_GM:
			{
				return m_pSelf->FindActivateChannelIndexByKey(s_keys[type]) == nChannelIndex;
			}
			break;
		default:
			return false;
		}
	}
	return false;
}


int KUiMsgCentrePad::FindActivateChannelResourceIndex(char* cTitle)
{
	if (cTitle == NULL || cTitle[0] == 0)
		return -1;

	for (int n = 0; n < m_nActivateChannels; n++)
	{
		if (m_pActivateChannel[n].ResourceIndex < 0)
			continue;
		if (strcmp(cTitle, m_ChannelsResource[m_pActivateChannel[n].ResourceIndex].cTitle) == 0)
			return m_pActivateChannel[n].ResourceIndex;
	}
	return -1;
}

int KUiMsgCentrePad::FindActivateChannelIndex(char* cTitle)
{
	if (cTitle == NULL || cTitle[0] == 0)
		return -1;

	for (int n = 0; n < m_nActivateChannels; n++)
	{
		if (m_pActivateChannel[n].ResourceIndex < 0)
			continue;
		if (strcmp(cTitle, m_ChannelsResource[m_pActivateChannel[n].ResourceIndex].cTitle) == 0)
			return n;
	}
	return -1;
}

int KUiMsgCentrePad::FindActivateChannelIndex(DWORD nChannelID)
{
	for (int n = 0; n < m_nActivateChannels; n++)
	{
		if (m_pActivateChannel[n].nChannelID == nChannelID)
			return n;
	}
	return -1;
}

BYTE KUiMsgCentrePad::FindActivateChannelCost(DWORD nChannelID)
{
	for (int n = 0; n < m_nActivateChannels; n++)
	{
		if (m_pActivateChannel[n].nChannelID == nChannelID)
			return m_pActivateChannel[n].cost;
	}
	return -1;
}

int KUiMsgCentrePad::FindChannelResourceIndex(char* cTitle)
{
	if (cTitle == NULL || cTitle[0] == 0)
		return -1;

	for (int n = 0; n < m_nChannelsResource; n++)
	{
		if (strcmp(cTitle, m_ChannelsResource[n].cTitle) == 0)
			return n;
	}
	return -1;
}

int	KUiMsgCentrePad::FindActivateChannelIndexByKey(char* cKey)
{
	if (cKey == NULL || cKey[0] == 0)
		return -1;

	for (int n = 0; n < m_nActivateChannels; n++)
	{
		if (m_pActivateChannel[n].ResourceIndex >= 0)
		{
			if (m_ChannelsResource[m_pActivateChannel[n].ResourceIndex].cFormatName[0] != 0)
			{
				char *pdest = strstr(m_ChannelsResource[m_pActivateChannel[n].ResourceIndex].cFormatName, cKey);
				if (pdest)
					return n;
			}
		}
	}
	return -1;
}

int	KUiMsgCentrePad::IsNeverCloseActivateChannel(int nChannelIndex)
{
	if (nChannelIndex < 0 || nChannelIndex >= m_nActivateChannels)
		return 0;
	int n = m_pActivateChannel[nChannelIndex].ResourceIndex;
	if (n >= 0 && m_ChannelsResource[n].nNeverClose)	//²»ÄÜ¹Ø±ÕµÄÆµµÀ
		return 1;
	return 0;
}

bool KUiMsgCentrePad::GetChannelMenuinfo(int nChannelIndex, WORD* pnPicIndex, WORD* pPicHei, KRColor* puTextColor, KRColor* puBkColor, char* pszMenuText, short* pnCheckPicIndex)
{
	if (m_pSelf && nChannelIndex >= 0 && nChannelIndex < m_pSelf->m_nActivateChannels)
	{
		int n = m_pSelf->m_pActivateChannel[nChannelIndex].ResourceIndex;
		if (n >= 0)
		{
			if (m_pSelf->m_ChannelsResource[n].nNeverClose)
			{
				if (pnCheckPicIndex)
					*pnCheckPicIndex = -1;
			}
			else
			{
				if (pnCheckPicIndex)
					*pnCheckPicIndex = m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe ? m_pSelf->m_nCheckOnPicIndex : m_pSelf->m_nCheckOffPicIndex;
			}
				
			if (pnPicIndex)
				*pnPicIndex = m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe ? m_pSelf->m_ChannelsResource[n].nMenuPicIndex : m_pSelf->m_ChannelsResource[n].nMenuDeactivatePicIndex;
			if (puTextColor)
				*puTextColor = m_pSelf->m_ChannelsResource[n].uTextColor;
			if (pPicHei)
				*pPicHei = m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe ? m_pSelf->m_ChannelsResource[n].nMenuPicHeight : m_pSelf->m_ChannelsResource[n].nMenuDeactivatePicHeight;
			if (puBkColor)
				*puBkColor = m_pSelf->m_ChannelsResource[n].uMenuBkColor;
			if (pszMenuText)
			{
				if (n== CH_MSGR)
				{
					TMissionLadderSelfInfo		tmp;
					g_pCoreShell->GetGameData(GDI_MISSION_SELFDATA,
					(unsigned int)(&tmp), NULL);
					if(tmp.szMissionName[0])
						sprintf(pszMenuText, "%s[%d]", tmp.szMissionName, tmp.ucGroup);
					else
						strcpy(pszMenuText, m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe ? m_pSelf->m_ChannelsResource[n].cMenuText : m_pSelf->m_ChannelsResource[n].cMenuDeactivateText);
				}
				else if(n == CH_CHATROOM)
				{
					KUiPlayerBaseInfo	Info;
					memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
					g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Info, 0);
					if(Info.nRoomId >=0)
					{
						char szName[10];
						g_pCoreShell->GetGameData(GDI_CHATROOM_NAME, (unsigned int)szName, 0);
						sprintf(pszMenuText, "Phßng %s", szName);
					}
				}
				else
					strcpy(pszMenuText, m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe ? m_pSelf->m_ChannelsResource[n].cMenuText : m_pSelf->m_ChannelsResource[n].cMenuDeactivateText);
			}
			return true;
		}
	}
	return false;
}

DWORD KUiMsgCentrePad::GetChannelID(int nChannelIndex)
{
	if (m_pSelf && nChannelIndex >= 0 && nChannelIndex < m_pSelf->m_nActivateChannels)
	{
		return m_pSelf->m_pActivateChannel[nChannelIndex].nChannelID;
	}
	return -1;
}

char* KUiMsgCentrePad::GetChannelTitle(int nChannelIndex)
{
	if (m_pSelf && nChannelIndex >= 0 && nChannelIndex < m_pSelf->m_nActivateChannels)
	{
		if (m_pSelf->m_pActivateChannel[nChannelIndex].ResourceIndex >= 0)
			return m_pSelf->m_ChannelsResource[m_pSelf->m_pActivateChannel[nChannelIndex].ResourceIndex].cTitle;
	}
	return NULL;
}

DWORD KUiMsgCentrePad::GetChannelCount()
{
	if (m_pSelf)
	{
		return m_pSelf->m_nActivateChannels;
	}
	return 0;
}

int KUiMsgCentrePad::GetChannelIndex(char* channelName)
{
	if (m_pSelf)
		return m_pSelf->FindActivateChannelIndex(channelName);
	return -1;
}

int KUiMsgCentrePad::GetChannelIndex(DWORD dwID)
{
	if (m_pSelf)
		return m_pSelf->FindActivateChannelIndex(dwID);
	return -1;
}

BYTE KUiMsgCentrePad::GetChannelCost(DWORD dwID)
{
	if (m_pSelf)
		return m_pSelf->FindActivateChannelCost(dwID);
	return -1;
}

int	KUiMsgCentrePad::PushChannelData(DWORD dwID, const char* Buffer, int nLen)
{
	int nIndex = KUiMsgCentrePad::GetChannelIndex(dwID);
	if (nIndex >= 0 && m_pSelf && Buffer && nLen > 0)
	{
		int nRes = m_pSelf->m_pActivateChannel[nIndex].ResourceIndex;
		if (nRes >= 0 &&
			m_pSelf->m_pActivateChannel[nIndex].uLeftSendMsgNum < m_pSelf->m_ChannelsResource[nRes].uSendMsgNum)
		{
			int nCur = IR_GetCurrentTime();
			int nLeft = (nCur - m_pSelf->m_pActivateChannel[nIndex].uLastSendMsgTime) > m_pSelf->m_ChannelsResource[nRes].uSendMsgInterval ? 0 : (m_pSelf->m_ChannelsResource[nRes].uSendMsgInterval - (nCur - m_pSelf->m_pActivateChannel[nIndex].uLastSendMsgTime));
			nLeft += m_pSelf->m_pActivateChannel[nIndex].uLeftSendMsgNum * m_pSelf->m_ChannelsResource[nRes].uSendMsgInterval;
			int nOffset = m_pSelf->m_pActivateChannel[nIndex].uBufferOffset;
			unsigned int nSendTime = nCur + nLeft;
			memcpy(m_pSelf->m_pActivateChannel[nIndex].Buffer + nOffset, &nSendTime, sizeof(nSendTime));
			nOffset += sizeof(nSendTime);
			memcpy(m_pSelf->m_pActivateChannel[nIndex].Buffer + nOffset, &nLen, sizeof(nLen));
			nOffset += sizeof(nLen);
			memcpy(m_pSelf->m_pActivateChannel[nIndex].Buffer + nOffset, Buffer, nLen);
			nOffset += nLen;
			
			m_pSelf->m_pActivateChannel[nIndex].uLeftSendMsgNum++;
			m_pSelf->m_pActivateChannel[nIndex].uBufferOffset = nOffset;

			return nLeft;
		}
	}
	return -1;
}

int	KUiMsgCentrePad::GetChannelData(DWORD& dwID, BYTE& cost, char*& Buffer, int& nLen)
{
	if (m_pSelf)
	{
		unsigned int nSendTime = IR_GetCurrentTime();
		for (int i = 0; i < m_pSelf->m_nActivateChannels; i++)
		{
			if (m_pSelf->m_pActivateChannel[i].uLeftSendMsgNum > 0)
			{
				unsigned int* pST = (unsigned int*)(m_pSelf->m_pActivateChannel[i].Buffer);
				if (*pST <= nSendTime)
				{
					dwID = m_pSelf->m_pActivateChannel[i].nChannelID;
					cost = m_pSelf->m_pActivateChannel[i].cost;
					nLen = *((int *)(m_pSelf->m_pActivateChannel[i].Buffer + sizeof(unsigned int)));
					Buffer = m_pSelf->m_pActivateChannel[i].Buffer + sizeof(unsigned int) + sizeof(int);
					return 1;
				}
			}
		}
	}

	return 0;
}

int	KUiMsgCentrePad::PopChannelData(DWORD dwID)
{
	if (m_pSelf)
	{
		unsigned int nSendTime = IR_GetCurrentTime();
		for (int i = 0; i < m_pSelf->m_nActivateChannels; i++)
		{
			if (m_pSelf->m_pActivateChannel[i].nChannelID == dwID &&
				m_pSelf->m_pActivateChannel[i].uLeftSendMsgNum > 0)
			{
				int nLen = *((int *)(m_pSelf->m_pActivateChannel[i].Buffer + sizeof(unsigned int)));
				int nMem = nLen + sizeof(unsigned int) + sizeof(int);
				memmove(m_pSelf->m_pActivateChannel[i].Buffer,
						m_pSelf->m_pActivateChannel[i].Buffer + nMem,
						m_pSelf->m_pActivateChannel[i].uBufferOffset - nMem);
				m_pSelf->m_pActivateChannel[i].uBufferOffset -= nMem;
				m_pSelf->m_pActivateChannel[i].uLeftSendMsgNum--;
				m_pSelf->m_pActivateChannel[i].uLastSendMsgTime = nSendTime;

				return 1;
			}
		}
	}

	return 0;
}

void KUiMsgCentrePad::SetReply(const char* Buff, int nLen)
{
	if (m_pSelf)
	{
		memcpy(m_pSelf->m_szReply, Buff, nLen);

		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.byParamSize = 0;
		Msg.byPriority = 0;
		Msg.eType = SMT_NORMAL;
		Msg.uReservedForUi = 0;
		sprintf(Msg.szMessage, "BËt chÕ ®é tù ®éng håi ©m");
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
	}
}

void KUiMsgCentrePad::ClearReply()
{
	if (m_pSelf)
	{
		memset(m_pSelf->m_szReply, 0, sizeof(m_pSelf->m_szReply));

		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.byParamSize = 0;
		Msg.byPriority = 0;
		Msg.eType = SMT_NORMAL;
		Msg.uReservedForUi = 0;
		sprintf(Msg.szMessage, "Hñy chÕ ®é tù ®éng håi ©m");
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
	}
}

bool KUiMsgCentrePad::GetPeopleMenuinfo(char* szDest, WORD* pnPicIndex, WORD* pPicHei, KRColor* puTextColor, KRColor* puBkColor)
{
	if (m_pSelf)
	{
		bool bFriend = KUiChatCentre::IsMyFriend(szDest);
		if (bFriend)
		{
			if (pnPicIndex)
				*pnPicIndex = m_pSelf->m_MSNInfo.nFriendMenuPicIndex;
			if (pPicHei)
				*pPicHei = m_pSelf->m_MSNInfo.nFriendMenuPicHeight;
			if (puBkColor)
				*puBkColor = m_pSelf->m_MSNInfo.uFriendMenuBkColor;
			if (puTextColor)
				*puTextColor = m_pSelf->m_MSNInfo.uTextColorSelf;
		}
		else
		{
			if (pnPicIndex)
				*pnPicIndex = m_pSelf->m_MSNInfo.nStrangerMenuPicIndex;
			if (pPicHei)
				*pPicHei = m_pSelf->m_MSNInfo.nStrangerMenuPicHeight;
			if (puBkColor)
				*puBkColor = m_pSelf->m_MSNInfo.uStrangerMenuBkColor;
			if (puTextColor)
				*puTextColor = m_pSelf->m_MSNInfo.uTextColorUnknown;
		}
		
		return true;
	}
	return false;
}

void KUiMsgCentrePad::SendQueryChannelID(int nChannelResourceIndex)
{
	if (nChannelResourceIndex < 0 || nChannelResourceIndex >= m_nChannelsResource)
		return;

	if (FindActivateChannelResourceIndex(m_ChannelsResource[nChannelResourceIndex].cTitle) >= 0)	//ÒÑÉêÇë¹ýÁË
		return;

	char ChannelName[__X_NAME_LEN_];
	ChannelName[0];
	if (m_ChannelsResource[nChannelResourceIndex].cFormatName[0] == 0)
		return;
	if (!ReplaceSpecialField(ChannelName, m_ChannelsResource[nChannelResourceIndex].cFormatName))
		return;
	if (ChannelName[0] == 0)
		return;

	char ProtocolBuffer[sizeof(tagExtendProtoHeader) + sizeof(PLAYERCOMM_QUERYCHANNELID)];
	int nProtocolOffset = 0;
	tagExtendProtoHeader* pHead = (tagExtendProtoHeader*)ProtocolBuffer;
	pHead->ProtocolType = c2s_extend;
	nProtocolOffset += sizeof(tagExtendProtoHeader);

	PLAYERCOMM_QUERYCHANNELID* pdata = (PLAYERCOMM_QUERYCHANNELID*)(ProtocolBuffer + nProtocolOffset);
	pdata->ProtocolFamily = pf_playercommunity;
	pdata->ProtocolID =	playercomm_c2s_querychannelid;
	strncpy(pdata->channel, ChannelName, __X_NAME_LEN_);
	
	nProtocolOffset += sizeof(PLAYERCOMM_QUERYCHANNELID);

	pHead->wLength = nProtocolOffset - 1;

	g_pCoreShell->SendNewDataToServer(ProtocolBuffer, nProtocolOffset);
}

void KUiMsgCentrePad::SendChannelSubscribe(int nChannelIndex, bool b)
{
	if (nChannelIndex < 0 || nChannelIndex >= m_nActivateChannels)
		return;

	char ProtocolBuffer[sizeof(tagExtendProtoHeader) + sizeof(PLAYERCOMM_SUBSCRIBE)];
	int nProtocolOffset = 0;
	tagExtendProtoHeader* pHead = (tagExtendProtoHeader*)ProtocolBuffer;
	pHead->ProtocolType = c2s_extend;
	nProtocolOffset += sizeof(tagExtendProtoHeader);

	PLAYERCOMM_SUBSCRIBE* pdata = (PLAYERCOMM_SUBSCRIBE*)(ProtocolBuffer + nProtocolOffset);
	pdata->ProtocolFamily = pf_playercommunity;
	pdata->ProtocolID =	playercomm_c2s_subscribe;
	pdata->channelid = m_pActivateChannel[nChannelIndex].nChannelID;
	pdata->subscribe = b;
	
	nProtocolOffset += sizeof(PLAYERCOMM_SUBSCRIBE);

	pHead->wLength = nProtocolOffset - 1;

	g_pCoreShell->SendNewDataToServer(ProtocolBuffer, nProtocolOffset);
}

int	KUiMsgCentrePad::AddActivateChannel(const KChannelActivateInfo& Item)
{
	int n = FindActivateChannelIndex(Item.nChannelID);
	if (n >= 0)
		return n;
	//ÐÂÔöÆµµÀ
	KChannelActivateInfo* pChannels = new KChannelActivateInfo[m_nActivateChannels + 1];
	if (m_nActivateChannels > 0 && m_pActivateChannel)
	{
		memcpy(pChannels, m_pActivateChannel, m_nActivateChannels * sizeof(KChannelActivateInfo));
		m_nActivateChannels = ReleaseActivateChannelAll();
	}
	m_pActivateChannel = pChannels;
	pChannels = m_pActivateChannel + m_nActivateChannels;
	*pChannels = Item;
	pChannels->uLastSendMsgTime = IR_GetCurrentTime() - m_ChannelsResource[Item.ResourceIndex].uSendMsgInterval;	//È·±£µÚÒ»Ìõ¿Ï¶¨ÄÜ·¢
	pChannels->uLeftSendMsgNum = 0;
	pChannels->uBufferOffset = 0;
	memset(pChannels->Buffer, 0, sizeof(pChannels->Buffer));
	m_nActivateChannels++;

	return m_nActivateChannels - 1;
}

int	KUiMsgCentrePad::ReleaseActivateChannel(int nChannelIndex)
{
	if (nChannelIndex < 0 || nChannelIndex >= m_nActivateChannels)
		return -1;

	if (nChannelIndex + 1 < m_nActivateChannels)
	{
		memmove(m_pActivateChannel + nChannelIndex, m_pActivateChannel + nChannelIndex + 1, (m_nActivateChannels - nChannelIndex - 1) * sizeof(KChannelActivateInfo));
	}

	m_nActivateChannels--;

	return m_nActivateChannels;
}

int	KUiMsgCentrePad::ReleaseActivateChannelAll()
{
	if (m_pSelf)
	{
		int nOld = m_pSelf->m_nActivateChannels;
		if (m_pSelf->m_pActivateChannel)
			delete [] m_pSelf->m_pActivateChannel;
		m_pSelf->m_pActivateChannel = NULL;
		m_pSelf->m_nActivateChannels = 0;
		return nOld;
	}
	return 0;
}

//³õÊ¼»¯
void KUiMsgCentrePad::Initialize()
{
	AddChild(&m_MoveImg);
	AddChild(&m_SizeBtn);
	
	AddChild(&m_BgShadowBtn);
	
	AddChild(&m_TabAllBtn);
	AddChild(&m_TabSecrectBtn);
	AddChild(&m_TabTeamBtn);
	AddChild(&m_TabFactionBtn);
	AddChild(&m_TabTongBtn);
	AddChild(&m_TabOtherBtn);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_LOWEST);

	Wnd_AddWindow(&m_Sys, WL_LOWEST);	//ÔÚKUiMsgCentrePadÖ®ºó¼ÓÈë,ÒªµÈ±³¾°ÏÈ»­
	m_Sys.AddChild(&m_Sys.m_SysRoom);
	m_Sys.AddChild(&m_Sys.m_UpButton);
	m_Sys.AddChild(&m_Sys.m_OpenSysButton);
	m_Sys.AddChild(&m_Sys.m_DownButton);
	m_Sys.m_OpenSysButton.CheckButton(1);

	for (int i = 0; i < CHANNELTAB_NUM; i++)
	{
		AddChild(&m_MessageChannel[i]);
		m_MessageChannel[i].GetMessageListBox()->SetItemActived(FALSE);
		m_MessageChannel[i].SetPosition(HIDE_POS);
	}
	m_MessageChannel[CHANNELTAB_PUBLIC].SetPosition(SHOW_POS);
	m_MessageChannel[CHANNELTAB_PUBLIC].GetMessageListBox()->SetItemActived(TRUE);
}
void KUiMsgCentrePad::ShowChatTab(KIniFile* pSetting)
{
	int	i;
	if (pSetting)
	{
		char	szKey[16];
		for (i = 0; i < CHANNELTAB_NUM; i++)
		{
			sprintf(szKey, "ChatTabLabel_%d", i);
			pSetting->GetString("ChatTab", szKey, "",m_ChatTab[i].szChatTabName,sizeof(m_ChatTab[i].szChatTabName));
			sprintf(szKey, "ChatTabType_%d", i);
			pSetting->GetInteger("ChatTab", szKey, -1,&m_ChatTab[i].nId);
			m_TabButton[i].SetText(m_ChatTab[i].szChatTabName);
		}
	}
	int XOffset, X, Y;
	pSetting->GetInteger("TabButtons", "XOffset", 0, &XOffset);
	pSetting->GetInteger2("TabButtons", "Start", &X, &Y);
	for (i = 0;i < CHANNELTAB_NUM; i++)
	{
		AddChild(&m_TabButton[i]);
		m_TabButton[i].SetPosition(X, Y);
		X += XOffset;
	}
}
// -------------------------------------------------------------------------
// ¹¦ÄÜ	: »î¶¯º¯Êý
// -------------------------------------------------------------------------
void KUiMsgCentrePad::Breathe()
{
	if (m_uAutoDelMsgInterval &&
		IR_IsTimePassed(m_uAutoDelMsgInterval, m_uLastDelMsgTime))
	{
		for (int i = 0; i < CHANNELTAB_NUM; i++)
			m_MessageChannel[i].GetMessageListBox()->HideNextLine();
		m_Sys.m_SysRoom.HideNextLine();
		m_uLastDelMsgTime = IR_GetCurrentTime();
	}
}

//ÔØÈë½çÃæ·½°¸
void KUiMsgCentrePad::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
			m_pSelf->LoadScheme(&Ini);
			
	}
}

//ÖØÐÂ³õÊ¼»¯½çÃæ
void KUiMsgCentrePad::DefaultScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	if (m_pSelf)
	{
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			int	nValue1, nValue2;
			Ini.GetInteger("Main", "Left",  0, &nValue1);
			Ini.GetInteger("Main", "Top",   0, &nValue2);
			m_pSelf->SetPosition(nValue1, nValue2);
			m_pSelf->SetSize(ms_DefaultWidth, ms_DefaultHeight);
		}
	}
}

//Òþ²ØËùÓÐÊä³ö
void KUiMsgCentrePad::HideAllMessage()
{
	if (m_pSelf)
	{
		for (int i = 0; i < CHANNELTAB_NUM; i++)
			m_pSelf->m_MessageChannel[i].GetMessageListBox()->HideAllLine();
		m_pSelf->m_Sys.m_SysRoom.HideAllLine();
	}
}

//ÏÔÊ¾ËùÓÐÊä³ö
void KUiMsgCentrePad::ShowAllMessage()
{
	if (m_pSelf)
	{
		for (int i = 0; i < CHANNELTAB_NUM; i++)
			m_pSelf->m_MessageChannel[i].GetMessageListBox()->ClearHideLine();
		m_pSelf->m_Sys.m_SysRoom.ClearHideLine();
	}
}


extern IInlinePicEngineSink *g_pIInlinePicSink;

int KUiMsgCentrePad::ms_DefaultWidth = 0;
int KUiMsgCentrePad::ms_DefaultHeight = 0;

void KUiMsgCentrePad::LoadScheme(KIniFile* pIni)
{
	//_ASSERT(pIni);
	
	Init(pIni, "Main");
	pIni->GetInteger2("Main", "WindowRange", &m_nMinTopPos, &m_nMaxBottomPos);

	m_MoveImg		.Init(pIni, "MoveImg");

	m_BgShadowBtn   .Init(pIni, "ShadowBtn");

	m_SizeBtn		.Init(pIni, "SizeBtn");
	
	for (int i = 0;i < CHANNELTAB_NUM;i++)
		m_TabButton[i].Init(pIni, "TabButton");
		
	ShowChatTab(pIni);

	m_TabButton[CHANNELTAB_PUBLIC].CheckButton(true);
	
	int nV;
	pIni->GetInteger("Main", "SizeUp", 0, &nV);
	m_bSizeUp = (nV != 0);
	if (m_bSizeUp)
		m_SizeBtn.SetStyle(m_SizeBtn.GetStyle() & ~WND_S_MOVE_WITH_R_EDGE & ~WND_S_MOVE_WITH_B_EDGE);

	char szImage[MAX_PATH];
	
	m_nCheckOnPicIndex = -1;
	if (pIni->GetString("Main", "CheckOnImage", "", szImage, MAX_PATH))
	{
		if (g_pIInlinePicSink)
		{
			g_pIInlinePicSink->AddCustomInlinePic((WORD&)m_nCheckOnPicIndex, szImage);
		}
	}
	m_nCheckOffPicIndex = -1;
	if (pIni->GetString("Main", "CheckOffImage", "", szImage, MAX_PATH))
	{
		if (g_pIInlinePicSink)
		{
			g_pIInlinePicSink->AddCustomInlinePic((WORD&)m_nCheckOffPicIndex, szImage);
		}
	}

	for (int i = 0; i < CHANNELTAB_NUM; i++)
		m_MessageChannel[i].Init(pIni, "ChatRoom");

	m_Sys.Init(pIni, "SysRoom");
	m_Sys.m_OpenSysButton.Init(pIni, "SysRoom_Open");
	m_Sys.m_OpenSysButton.CheckButton(true);
	
	m_Sys.m_SysRoom.Init(pIni, "SysRoom_List");
	m_Sys.m_UpButton.Init(pIni, "SysRoom_Up");
	m_Sys.m_DownButton.Init(pIni, "SysRoom_Down");
	
	m_TabAllBtn.Init(pIni, "TabAllBtn");
	m_TabSecrectBtn.Init(pIni, "TabSecrectBtn");
	m_TabTeamBtn.Init(pIni, "TabTeamBtn");
	m_TabFactionBtn.Init(pIni, "TabFactionBtn");
	m_TabTongBtn.Init(pIni, "TabTongBtn");
	m_TabOtherBtn.Init(pIni, "TabOtherBtn");

	char ChName[64];

	pIni->GetString("MSNRoom", "TextColorSelf", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uTextColorSelf.Color_dw = GetColor(ChName);
	m_MSNInfo.uTextColorSelf.Color_b.a = KTC_COLOR;
	pIni->GetString("MSNRoom", "TextBorderColorSelf", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uTextBorderColorSelf.Color_dw = GetColor(ChName);
	m_MSNInfo.uTextBorderColorSelf.Color_b.a = KTC_BORDER_COLOR;
	pIni->GetString("MSNRoom", "TextBKColorSelf", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uTextBKColorSelf.Color_dw = (GetColor(ChName) & 0xffffff);
	int nAlpha;
	pIni->GetInteger("MSNRoom", "TextBKColorSelfAlpha", 128, &nAlpha);
	if (nAlpha > 255 || nAlpha < 0)
		m_MSNInfo.uTextBKColorSelf.Color_dw |= 0x16000000;
	else
		m_MSNInfo.uTextBKColorSelf.Color_dw |= (((255 - nAlpha) << 21) & 0xff000000);

	pIni->GetString("MSNRoom", "TextColorUnknown", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uTextColorUnknown.Color_dw = GetColor(ChName);
	m_MSNInfo.uTextColorUnknown.Color_b.a = KTC_COLOR;
	pIni->GetString("MSNRoom", "TextBorderColorUnknown", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uTextBorderColorUnknown.Color_dw = GetColor(ChName);
	m_MSNInfo.uTextBorderColorUnknown.Color_b.a = KTC_BORDER_COLOR;
	pIni->GetString("MSNRoom", "TextColorFriend", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uTextFriendColor.Color_dw = GetColor(ChName);
	m_MSNInfo.uTextFriendColor.Color_b.a = KTC_COLOR;
	pIni->GetString("MSNRoom", "TextBorderColorFriend", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uTextBorderFriendColor.Color_dw = GetColor(ChName);
	m_MSNInfo.uTextBorderFriendColor.Color_b.a = KTC_BORDER_COLOR;

	m_MSNInfo.nFriendMenuPicIndex = -1;
	if (pIni->GetString("MSNRoom", "Friend_MenuImage", "", szImage, MAX_PATH))
	{
		if (g_pIInlinePicSink)
		{
			m_MSNInfo.nFriendMenuPicHeight = 0;
			KImageParam	Param;
			Param.nHeight = 0;
			if (g_pRepresentShell &&
				g_pRepresentShell->GetImageParam(szImage, &Param, ISI_T_SPR))
			{
				m_MSNInfo.nFriendMenuPicHeight = Param.nHeight;
			}
			g_pIInlinePicSink->AddCustomInlinePic(m_MSNInfo.nFriendMenuPicIndex, szImage);
		}
	}
	m_MSNInfo.nFriendTextPicIndex = -1;
	if (pIni->GetString("MSNRoom", "Friend_TextImage", "", szImage, MAX_PATH))
	{
		if (g_pIInlinePicSink)
		{
			g_pIInlinePicSink->AddCustomInlinePic(m_MSNInfo.nFriendTextPicIndex, szImage);
		}
	}
	pIni->GetString("MSNRoom", "Friend_MenuBkColor", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uFriendMenuBkColor.Color_dw = GetColor(ChName);
	m_MSNInfo.uFriendMenuBkColor.Color_b.a = 0;

	pIni->GetString("MSNRoom", "Friend_Sound", "", m_MSNInfo.szFriendSoundFileName, sizeof(m_MSNInfo.szFriendSoundFileName));

	m_MSNInfo.nStrangerMenuPicIndex = -1;
	if (pIni->GetString("MSNRoom", "Stranger_MenuImage", "", szImage, MAX_PATH))
	{
		if (g_pIInlinePicSink)
		{
			m_MSNInfo.nStrangerMenuPicHeight = 0;
			KImageParam	Param;
			Param.nHeight = 0;
			if (g_pRepresentShell &&
				g_pRepresentShell->GetImageParam(szImage, &Param, ISI_T_SPR))
			{
				m_MSNInfo.nStrangerMenuPicHeight = Param.nHeight;
			}
			g_pIInlinePicSink->AddCustomInlinePic(m_MSNInfo.nStrangerMenuPicIndex, szImage);
		}
	}
	m_MSNInfo.nStrangerTextPicIndex = -1;
	if (pIni->GetString("MSNRoom", "Stranger_TextImage", "", szImage, MAX_PATH))
	{
		if (g_pIInlinePicSink)
		{
			g_pIInlinePicSink->AddCustomInlinePic(m_MSNInfo.nStrangerTextPicIndex, szImage);
		}
	}
	m_MSNInfo.nSelfTextPicIndex = -1;
	if (pIni->GetString("MSNRoom", "Self_TextImage", "", szImage, MAX_PATH))
	{
		if (g_pIInlinePicSink)
		{
			g_pIInlinePicSink->AddCustomInlinePic(m_MSNInfo.nSelfTextPicIndex, szImage);
		}
	}
	
	pIni->GetString("MSNRoom", "Stranger_MenuBkColor", "0,0,0", ChName, sizeof(ChName));
	m_MSNInfo.uStrangerMenuBkColor.Color_dw = GetColor(ChName);
	m_MSNInfo.uStrangerMenuBkColor.Color_b.a = 0;

	SetAutoDelMsgInterval(SECOND_AUTODELMSG);
	int nCh;
	for (nCh = 0; nCh < MAX_CHANNELRESOURCE; nCh++)
	{
		sprintf(ChName, "%s%d", "Channel", nCh);
		if (pIni->GetString("Channels", ChName, "", m_ChannelsResource[nCh].cTitle, 32))
		{
			pIni->GetString(m_ChannelsResource[nCh].cTitle, "TextColor", "0,0,0", ChName, sizeof(ChName));
			m_ChannelsResource[nCh].uTextColor.Color_dw = GetColor(ChName);
			m_ChannelsResource[nCh].uTextColor.Color_b.a = KTC_COLOR;

			pIni->GetString(m_ChannelsResource[nCh].cTitle, "TextBorderColor", "0,0,0", ChName, sizeof(ChName));
			m_ChannelsResource[nCh].uTextBorderColor.Color_dw = GetColor(ChName);
			m_ChannelsResource[nCh].uTextBorderColor.Color_b.a = KTC_BORDER_COLOR;

			for (int m = 0; m < MAX_CHANNELRESOURCE; m++)
			{
				m_ChannelsResource[nCh].cShortTitle[m][0] = 0;
				sprintf(ChName, "%s%d", "ShortName", m);
				pIni->GetString(m_ChannelsResource[nCh].cTitle, ChName, "", m_ChannelsResource[nCh].cShortTitle[m], sizeof(m_ChannelsResource[nCh].cShortTitle[m]));
			}

			pIni->GetString(m_ChannelsResource[nCh].cTitle, "FormatName", "", m_ChannelsResource[nCh].cFormatName, sizeof(m_ChannelsResource[nCh].cFormatName));

			pIni->GetString(m_ChannelsResource[nCh].cTitle, "MenuText", "", m_ChannelsResource[nCh].cMenuText, sizeof(m_ChannelsResource[nCh].cMenuText));
			pIni->GetString(m_ChannelsResource[nCh].cTitle, "DeactivateMenuText", m_ChannelsResource[nCh].cMenuText, m_ChannelsResource[nCh].cMenuDeactivateText, sizeof(m_ChannelsResource[nCh].cMenuDeactivateText));
			m_ChannelsResource[nCh].nMenuPicIndex = -1;
			if (pIni->GetString(m_ChannelsResource[nCh].cTitle, "MenuImage", "", szImage, MAX_PATH))
			{
				if (g_pIInlinePicSink)
				{
					m_ChannelsResource[nCh].nMenuPicHeight = 0;
					KImageParam	Param;
					Param.nHeight = 0;
					if (g_pRepresentShell &&
						g_pRepresentShell->GetImageParam(szImage, &Param, ISI_T_SPR))
					{
						m_ChannelsResource[nCh].nMenuPicHeight = Param.nHeight;
					}
					g_pIInlinePicSink->AddCustomInlinePic(m_ChannelsResource[nCh].nMenuPicIndex, szImage);
				}
			}
			m_ChannelsResource[nCh].nMenuDeactivatePicIndex = m_ChannelsResource[nCh].nMenuPicIndex;
			m_ChannelsResource[nCh].nMenuDeactivatePicHeight = m_ChannelsResource[nCh].nMenuPicHeight;
			if (pIni->GetString(m_ChannelsResource[nCh].cTitle, "DeactivateMenuImage", "", szImage, MAX_PATH))
			{
				if (g_pIInlinePicSink)
				{
					m_ChannelsResource[nCh].nMenuDeactivatePicHeight = 0;
					KImageParam	Param;
					Param.nHeight = 0;
					if (g_pRepresentShell &&
						g_pRepresentShell->GetImageParam(szImage, &Param, ISI_T_SPR))
					{
						m_ChannelsResource[nCh].nMenuDeactivatePicHeight = Param.nHeight;
					}
					g_pIInlinePicSink->AddCustomInlinePic(m_ChannelsResource[nCh].nMenuDeactivatePicIndex, szImage);
				}
			}
			m_ChannelsResource[nCh].nTextPicIndex = -1;
			if (pIni->GetString(m_ChannelsResource[nCh].cTitle, "TextImage", "", szImage, MAX_PATH))
			{
				if (g_pIInlinePicSink)
				{
					g_pIInlinePicSink->AddCustomInlinePic(m_ChannelsResource[nCh].nTextPicIndex, szImage);
				}
			}
			pIni->GetString(m_ChannelsResource[nCh].cTitle, "MenuBkColor", "0,0,0", ChName, sizeof(ChName));
			m_ChannelsResource[nCh].uMenuBkColor.Color_dw = GetColor(ChName);
			m_ChannelsResource[nCh].uMenuBkColor.Color_b.a = 0;

			pIni->GetInteger(m_ChannelsResource[nCh].cTitle, "NeverClose", 0, &m_ChannelsResource[nCh].nNeverClose);
			pIni->GetString(m_ChannelsResource[nCh].cTitle, "Sound", "", m_ChannelsResource[nCh].szSoundFileName, sizeof(m_ChannelsResource[nCh].szSoundFileName));

			//pIni->GetInteger(m_ChannelsResource[nCh].cTitle, "SendMsgInterval", 0, (int*)(&m_ChannelsResource[nCh].uSendMsgInterval));
			//pIni->GetInteger(m_ChannelsResource[nCh].cTitle, "SendMsgNum", 0, (int*)(&m_ChannelsResource[nCh].uSendMsgNum));
			
			switch (nCh)
			{
				case CH_NEARBY:
					m_ChannelsResource[nCh].uSendMsgInterval = 2000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_TEAM:
					m_ChannelsResource[nCh].uSendMsgInterval = 800;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_WORLD:
					m_ChannelsResource[nCh].uSendMsgInterval = 60000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_FACTION:
					m_ChannelsResource[nCh].uSendMsgInterval = 10000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_SYSTEM:
					m_ChannelsResource[nCh].uSendMsgInterval = 15000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_CITY:
					m_ChannelsResource[nCh].uSendMsgInterval = 20000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_TONG:
					m_ChannelsResource[nCh].uSendMsgInterval = 10000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_MSGR:
					m_ChannelsResource[nCh].uSendMsgInterval = 2000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				case CH_CHATROOM:
					m_ChannelsResource[nCh].uSendMsgInterval = 2000;
					m_ChannelsResource[nCh].uSendMsgNum = 2;
					break;
				default:
					m_ChannelsResource[nCh].uSendMsgInterval = 1000000;
					m_ChannelsResource[nCh].uSendMsgNum = 0;
					break;					
			}

			m_ChannelsResource[nCh].uSendMsgNum++;
			if (m_ChannelsResource[nCh].uSendMsgNum > 10)
				m_ChannelsResource[nCh].uSendMsgNum = 10;
			if (m_ChannelsResource[nCh].uSendMsgNum <= 0)
				m_ChannelsResource[nCh].uSendMsgNum = 1;
		}
		else
			break;
	}
	if (nCh < MAX_CHANNELRESOURCE)
	{
		m_ChannelsResource[nCh].cTitle[0] = 0;
		for (int m = 0; m < MAX_CHANNELRESOURCE; m++)
		{
			m_ChannelsResource[nCh].cShortTitle[m][0] = 0;
		}
	}
	m_nChannelsResource = nCh;

	m_nDefaultChannelResource = -1;
	if (pIni->GetString("Channels", "DefaultChannel", "", ChName, 32))
	{
		m_nDefaultChannelResource = FindChannelResourceIndex(ChName);
	}

	pIni->GetString("Channels", "DefaultChannelSendName", "HÖ thèng", m_DefaultChannelSendName, 32);

	pIni->GetString("Main", "NameTextColor", "0,0,0", ChName, sizeof(ChName));
	m_NameColor.Color_dw = GetColor(ChName);
	m_NameColor.Color_b.a = KTC_COLOR;
	pIni->GetString("Main", "NameTextBorderColor", "0,0,0", ChName, sizeof(ChName));
	m_BorderNameColor.Color_dw = GetColor(ChName);
	m_BorderNameColor.Color_b.a = KTC_BORDER_COLOR;

	RECT rc;
	if (m_Style & WND_S_SIZE_WITH_ALL_CHILD)
	{
		GetAllChildLayoutRect(&rc);
		m_Width = rc.right - rc.left;;
		m_Height = rc.bottom - rc.top;
	}

	ms_DefaultWidth = m_Width;
	ms_DefaultHeight = m_Height;
}

int	KUiMsgCentrePad::PtInWindow(int x, int y)
{
	int nRet = 0;
	if (KWndWindow::PtInWindow(x, y))
	{
		nRet =  (
				m_MoveImg.PtInWindow(x, y) ||
				m_SizeBtn.PtInWindow(x, y) ||
				m_TabButton[CHANNELTAB_PUBLIC].PtInWindow(x, y) ||
				m_TabButton[CHANNELTAB_PRIVATE].PtInWindow(x, y) ||
				m_TabButton[CHANNELTAB_CHATROOM].PtInWindow(x, y) ||
				m_TabButton[CHANNELTAB_TONG].PtInWindow(x, y) ||
				m_TabButton[CHANNELTAB_FACTION].PtInWindow(x, y) ||
				m_TabButton[CHANNELTAB_SYSTEM].PtInWindow(x, y) ||
				m_MessageChannel[CHANNELTAB_PUBLIC].PtInWindow(x, y) ||
				m_MessageChannel[CHANNELTAB_PRIVATE].PtInWindow(x, y) ||
				m_MessageChannel[CHANNELTAB_CHATROOM].PtInWindow(x, y) ||
				m_MessageChannel[CHANNELTAB_TONG].PtInWindow(x, y) ||
				m_MessageChannel[CHANNELTAB_FACTION].PtInWindow(x, y) ||
				m_MessageChannel[CHANNELTAB_SYSTEM].PtInWindow(x, y) ||
				m_BgShadowBtn.PtInWindow(x, y)
				);
	}
	return nRet;
}

//´°¿Úº¯Êý
int KUiMsgCentrePad::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	//case WND_M_POPUPMENU:
	//	if (uParam == (unsigned int)(KWndWindow*)(m_ChatRoom.GetMessageListBox()))
	//		PopupChannelMenu(LOWORD(nParam), HIWORD(nParam));
	//	break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == SEL_CHANNEL_MENU)
			{
				if (short(LOWORD(nParam)) >= 0)
				{
					CheckChannel(LOWORD(nParam), !m_pActivateChannel[LOWORD(nParam)].bSubscribe);
					KPopupMenuData* p = KPopupMenu::GetMenuData();
					if (p)
					{
						PopupChannelMenu(p->nXX, p->nYY);
					}
				}
				else
					KPopupMenu::Cancel();
			}
		}
		break;
	case WND_N_CHILD_MOVE:
		if (uParam == (unsigned int)(KWndWindow*)&m_SizeBtn)
			DragWndSize(nParam);
		else if (uParam == (unsigned int)(KWndWindow*)&m_MoveImg)
			DragWndPos(nParam);
		break;
	case WND_N_LIST_ITEM_SEL:
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			char szCurText[256];
			szCurText[0] = 0;
			int nLen = GetMessageSendName((KWndWindow*)uParam, nParam, szCurText);
			if (nLen > 0)
			{
				char cName[32];
				ReplaceChannelName(cName, 31, szCurText);
				bool bChannel = (GetChannelIndex(cName) >= 0);
				KUiPlayerBar::InputNameMsg(bChannel, cName, TRUE);
			}
		}
		break;
	case WND_M_POPUPMENU:
		{
			char szCurText[256];
			szCurText[0] = 0;
			int nLen = GetMessageSendName((KWndWindow*)uParam, LOWORD(nParam), HIWORD(nParam), szCurText);

			if (nLen > 0)
			{
				char cName[32];
				ReplaceChannelName(cName, 31, szCurText);
				bool bChannel = (GetChannelIndex(cName) >= 0);
				if (!bChannel)		
				{
					if (GetKeyState(VK_CONTROL) & 0x8000)
					{
						KUiPlayerItem SelectPlayer;
						int nKind = -1;
						if (!(g_pCoreShell &&
							  g_pCoreShell->FindSpecialNPC(cName, &SelectPlayer, nKind) &&
							  nKind == kind_player
							 )
						   )
						{
							strncpy(SelectPlayer.Name, cName, 32);
							SelectPlayer.Name[31] = 0;
							SelectPlayer.nIndex = -1;
							SelectPlayer.uId = 0;
						}
						PopUpContextPeopleMenu(SelectPlayer, LOWORD(nParam), HIWORD(nParam));
					}
					else if (GetKeyState(VK_MENU) & 0x8000)
					{
						PopUpContextEmoteMenu(cName, LOWORD(nParam), HIWORD(nParam));
					}
				}
			}
		}
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BgShadowBtn)
		{
			ShowAllMessage();
			m_bShowShadow = m_BgShadowBtn.IsButtonChecked();
		}
		else 
		{
			for (int i = 0; i < CHANNELTAB_NUM; i++)
			{	
				if (uParam == (unsigned int)(KWndWindow*)&m_TabButton[i])
				{
					m_nCurChannelSel = i;
					m_TabButton[i].CheckButton(true);
					m_MessageChannel[i].GetMessageListBox()->SetItemActived(TRUE);
					m_MessageChannel[i].SetPosition(SHOW_POS);
				}
				else
				{
					m_TabButton[i].CheckButton(false);
					m_MessageChannel[i].GetMessageListBox()->SetItemActived(FALSE);
					m_MessageChannel[i].SetPosition(HIDE_POS);			
				}
			}
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		m_uLastDelMsgTime = IR_GetCurrentTime();
		break;
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

int	KUiMsgCentrePad::GetMessageSendName(KWndWindow* pWnd, int nIndex, char* szCurText)
{
	if (nIndex < 0)
		return 0;

	int nLen = 0;

	for (int i = 0; i < CHANNELTAB_NUM; i++)
	{
		if (pWnd == (KWndWindow*)&m_MessageChannel[i])
		{
			nLen = m_MessageChannel[i].GetMessageListBox()->GetOneMessage(nIndex, szCurText, 256, TRUE);
		}
	}

	int nName = 0;
	if (nLen >= 0)
	{
		szCurText[nLen] = 0;
		while (nName < nLen)
		{
			if (szCurText[nName] == 0x20)
			{
				return 0;
			}
			else if (szCurText[nName] == ':')
			{
				szCurText[nName] = 0;
				break;
			}
			nName++;
		}
	}
	return nName;
}

int	KUiMsgCentrePad::GetMessageSendName(KWndWindow* pWnd, int x, int y, char* szCurText)
{
	int nIndex = -1;

	for (int i = 0; i < CHANNELTAB_NUM; i++)
	{
		if (pWnd == (KWndWindow*)&m_MessageChannel[i])
		{
			nIndex = m_MessageChannel[i].GetMessageListBox()->HitTextAtPoint(x, y);
		}
	}
	return GetMessageSendName(pWnd, nIndex, szCurText);
}

//»æÖÆ´°¿Ú
void KUiMsgCentrePad::PaintWindow()
{
	KWndWindow::PaintWindow();
	if ((m_bSizingWnd || m_bShowShadow || m_bExpandWnd) && g_pRepresentShell)
	{
		KRUShadow	bg;
		for (int i = 0; i < CHANNELTAB_NUM; i++)
		{
			m_MessageChannel[i].GetMessageListBox()->GetAbsolutePos(&bg.oPosition.nX, &bg.oPosition.nY);
			bg.Color.Color_dw = 0x16000000;
			m_MessageChannel[i].GetMessageListBox()->GetSize(&bg.oEndPos.nX, &bg.oEndPos.nY);
			bg.oEndPos.nX += bg.oPosition.nX;
			bg.oEndPos.nY += bg.oPosition.nY;
			g_pRepresentShell->DrawPrimitives(1, &bg, RU_T_SHADOW, true);
		}

		if (m_bShowShadow)
		{
			m_Sys.m_SysRoom.GetAbsolutePos(&bg.oPosition.nX, &bg.oPosition.nY);
			bg.Color.Color_dw = 0x16000000;
			m_Sys.m_SysRoom.GetSize(&bg.oEndPos.nX, &bg.oEndPos.nY);
			bg.oEndPos.nX += bg.oPosition.nX;
			bg.oEndPos.nY += bg.oPosition.nY;
			g_pRepresentShell->DrawPrimitives(1, &bg, RU_T_SHADOW, true);
		}
	}
}

void KUiMsgCentrePad::DragWndPos(int nMoveOffset)
{
	if (nMoveOffset)
	{
		int nMoveOffsetX = (short)(nMoveOffset & 0xffff);
		int nMoveOffsetY = (short)((nMoveOffset >> 16) & 0xffff);
		int x, y;
		m_MoveImg.GetPosition(&x, &y);
		x -= nMoveOffsetX;
		y -= nMoveOffsetY;
		m_MoveImg.SetPosition(x, y);	//°´Å¥¸ú×Å´°¿Ú±ä»¯£¬ËùÒÔÒªÏÈ±ä»ØÔ­Ñù
		
		y = m_Top + nMoveOffsetY;

		if (y < m_nMinTopPos)
			y = m_nMinTopPos;
		else if (y + m_Height > m_nMaxBottomPos)
			y = m_nMaxBottomPos - m_Height;

		SetPosition(m_Left, y);
	}
}

void KUiMsgCentrePad::DragWndSize(int nMoveOffset)
{
	if (nMoveOffset == 0)
	{
		m_bSizingWnd = false;
		//m_MsgList.EnableTextLineShadow(true);
		m_nSizeBtnDragOffsetY = 0;
	}
	else
	{
		if (m_nSizeBtnDragOffsetY == 0)
		{
			m_bSizingWnd = true;
			m_bExpandWnd = false;
			//m_MsgList.EnableTextLineShadow(false);
		}
		int nMoveOffsetX = (short)(nMoveOffset & 0xffff);
		int nMoveOffsetY = (short)((nMoveOffset >> 16) & 0xffff);

		int x, y;
		m_SizeBtn.GetPosition(&x, &y);
		x -= nMoveOffsetX;
		y -= nMoveOffsetY;
		m_SizeBtn.SetPosition(x, y);	//°´Å¥¸ú×Å´°¿Ú±ä»¯£¬ËùÒÔÒªÏÈ±ä»ØÔ­Ñù

		m_nSizeBtnDragOffsetY += nMoveOffsetY;

		if (ChanageHeight(m_nSizeBtnDragOffsetY, m_bSizeUp))	//¸Ä±äHeight³É¹¦ºó£¬¾Í²»ÓÃ»ýÀÛÁË
			m_nSizeBtnDragOffsetY = 0;
	}
}

int KUiMsgCentrePad::ChanageHeight(int nOffsetY, bool bAtTop)
{
	if (bAtTop)
	{
		nOffsetY = m_MessageChannel[m_nCurChannelSel].HeightToLineHeight(-nOffsetY);
	}
	else
	{
		nOffsetY = m_MessageChannel[m_nCurChannelSel].HeightToLineHeight(nOffsetY);
	}


	if (nOffsetY != 0)
	{
		int x, y;
		GetSize(&x, &y);
		int nFontSize = m_MessageChannel[m_nCurChannelSel].GetFontSize();
		if (y + nOffsetY >= (nFontSize + 1) * nFontSize / 2)
		{
			if (bAtTop)
			{
				if (m_Top - nOffsetY < m_nMinTopPos)
					SetPosition(m_Left, m_nMinTopPos);
				else
					SetPosition(m_Left, m_Top - nOffsetY);
			}

			if (m_Top + y + nOffsetY > m_nMaxBottomPos)
				nOffsetY = m_nMaxBottomPos - m_Top - y;

			SetSize(x, y + nOffsetY);
		}
	}

	return nOffsetY;
}

void KUiMsgCentrePad::Clear()
{
	if (m_pSelf)
	{
		for (int i = 0; i < CHANNELTAB_NUM; i++)
			m_pSelf->m_MessageChannel[i].GetMessageListBox()->Clear();
		m_pSelf->m_Sys.m_SysRoom.Clear();
	}
}

void KUiMsgCentrePad::Expand()
{
	if (m_pSelf)
	{
		int x,y,w,h;
		m_pSelf->GetPosition(&x, &y);
		m_pSelf->GetSize(&w, &h);
		if (m_pSelf->m_bExpandWnd)
		{
			m_pSelf->SetPosition(m_pSelf->m_nLastWndSize.left, m_pSelf->m_nLastWndSize.top);
			m_pSelf->SetSize(m_pSelf->m_nLastWndSize.right, m_pSelf->m_nLastWndSize.bottom);			
			m_pSelf->m_bExpandWnd = false;
		}
		else
		{
			m_pSelf->m_nLastWndSize.left = x;
			m_pSelf->m_nLastWndSize.top = y;
			m_pSelf->m_nLastWndSize.right = w;
			m_pSelf->m_nLastWndSize.bottom = h;
			m_pSelf->SetPosition(x, m_pSelf->m_nMinTopPos);
			m_pSelf->SetSize(w, m_pSelf->m_nMaxBottomPos - m_pSelf->m_nMinTopPos);
			m_pSelf->m_bExpandWnd = true;
		}
	}
}

void KUiMsgCentrePad::PopupChannelMenu(int x, int y)
{
	if (m_pSelf == NULL)
		return;

	if (m_pSelf->m_nActivateChannels <= 0)
		return;

	int nChannelDataCount = m_pSelf->m_nActivateChannels;

	KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nChannelDataCount));
	if (pMenuData == NULL)
	{
		return;
	}

	KPopupMenu::InitMenuData(pMenuData, nChannelDataCount);
	pMenuData->usMenuFlag &= ~PM_F_HAVE_HEAD_TAIL_IMG;
	pMenuData->usMenuFlag |= (PM_F_AUTO_DEL_WHEN_HIDE | PM_F_CANCEL_BY_CALLER);
	pMenuData->nItemTitleIndent = 0;
	pMenuData->byItemTitleUpSpace = 0;

	pMenuData->nItemHeight = 0;
	WORD nHei = 0;
	char szText[64];
	int iC = 0;
	for (int i = 0; i < nChannelDataCount; i++)
	{
		WORD nPicIndex = -1;
		int nOffset = 0;
		KRColor uColor;
		KRColor uBKColor;
		short nCheckPicIndex = -1;
		if (GetChannelMenuinfo(iC, &nPicIndex, &nHei, &uColor, &uBKColor, szText, &nCheckPicIndex) && nCheckPicIndex != -1)
		{
			pMenuData->Items[iC].szData[0] = KTC_INLINE_PIC;
			*((WORD*)(pMenuData->Items[iC].szData + 1)) = nPicIndex;
			pMenuData->Items[iC].szData[3] = uColor.Color_b.a;
			pMenuData->Items[iC].szData[4] = uColor.Color_b.r;
			pMenuData->Items[iC].szData[5] = uColor.Color_b.g;
			pMenuData->Items[iC].szData[6] = uColor.Color_b.b;
			nOffset = 7;
			if (nHei > pMenuData->nItemHeight)
				pMenuData->nItemHeight = nHei;
			strcpy(pMenuData->Items[iC].szData + nOffset, szText);
			pMenuData->Items[iC].uDataLen = strlen(szText) + nOffset;
			iC++;
		}
		pMenuData->Items[i].uBgColor = uBKColor.Color_dw;
	}
	pMenuData->nNumItem = iC;

	pMenuData->nX = x;
	pMenuData->nY = y;
	KPopupMenu::Popup(pMenuData, (KWndWindow*)m_pSelf, SEL_CHANNEL_MENU);
}

int KUiMsgCentrePad::CheckChannel(int nChannelIndex, bool b)
{
	if (m_pSelf == NULL)
		return 0;

	if (nChannelIndex < 0 || nChannelIndex >= m_pSelf->m_nActivateChannels)
		return 0;
	if (m_pSelf->IsNeverCloseActivateChannel(nChannelIndex))	//²»ÄÜ¹Ø±ÕµÄÆµµÀ²»ÄÜ¸Ä±ä×´Ì¬
		return 0;

	if (m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe == b)
		return 1;
	m_pSelf->m_pActivateChannel[nChannelIndex].bSubscribe = b;
	m_pSelf->SendChannelSubscribe(nChannelIndex, b);
	int n = m_pSelf->m_pActivateChannel[nChannelIndex].ResourceIndex;
	if (n >= 0)
	{
		char Buffer[32];
		strncpy(Buffer, b ? m_pSelf->m_ChannelsResource[n].cMenuText : m_pSelf->m_ChannelsResource[n].cMenuDeactivateText, 32);
		strcat(Buffer, b ? " ®· ®­îc më l¹i" : " ®· bÞ ®ãng l¹i");
		KUiMsgCentrePad::SystemMessageArrival(Buffer, strlen(Buffer));
	}
	return 1;
}

int	KUiMsgCentrePad::FilterTextColor(char* pMsgBuff, unsigned short nMsgLength, const KRColor& uColor)
{
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR);
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_COLOR);
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_COLOR_RESTORE);
	nMsgLength = TClearSpecialCtrlInEncodedText(pMsgBuff, nMsgLength, KTC_BORDER_RESTORE);
	return TEncodeText(pMsgBuff, nMsgLength);
}

BOOL KUiMsgCentrePad::GetSelfPosition(int *pnX, int *pnY)
{
	if (m_pSelf)
	{
		if(m_pSelf->m_bExpandWnd)
		{
			*pnX = m_pSelf->m_nLastWndSize.left;
			*pnY = m_pSelf->m_nLastWndSize.top;
		}
		else
			m_pSelf->GetPosition(pnX, pnY);
	}
	return TRUE;
}

BOOL KUiMsgCentrePad::GetSelfSize(int *pnW, int *pnH)
{
	if (m_pSelf)
	{
		if(m_pSelf->m_bExpandWnd)
		{
			*pnW = m_pSelf->m_nLastWndSize.right;
			*pnH = m_pSelf->m_nLastWndSize.bottom;
		}
		else
			m_pSelf->GetSize(pnW, pnH);
	}
	return TRUE;
}

void KUiMsgCentrePad::SetFontSize(int nFontSize)
{
	if (m_pSelf)
	{
		for (int i = 0; i < CHANNELTAB_NUM; i++)
			m_pSelf->m_MessageChannel[i].GetMessageListBox()->SetFontSize(nFontSize);
	}
}

int KUiMsgCentrePad::GetFontSize()
{
	if (m_pSelf)
		return m_pSelf->m_MessageChannel[m_pSelf->m_nCurChannelSel].GetMessageListBox()->GetFontSize();

	return NORMAL_FONTSIZE;
}

int KUiMsgCentrePad::SetChannelTextColor(char* cTitle, DWORD uTextColor, DWORD uTextBorderColor)
{
	if (m_pSelf)
	{
		int nRIndex = m_pSelf->FindChannelResourceIndex(cTitle);
		if (nRIndex >= 0)
		{
			m_pSelf->m_ChannelsResource[nRIndex].uTextColor.Color_dw = uTextColor;
			m_pSelf->m_ChannelsResource[nRIndex].uTextColor.Color_b.a = KTC_COLOR;

			m_pSelf->m_ChannelsResource[nRIndex].uTextBorderColor.Color_dw = uTextBorderColor;
			m_pSelf->m_ChannelsResource[nRIndex].uTextBorderColor.Color_b.a = KTC_BORDER_COLOR;
		}

		return nRIndex;
	}

	return -1;
}

char* KUiMsgCentrePad::GetChannelTextColor(int nIndex, DWORD& uTextColor, DWORD& uTextBorderColor)
{
	if (m_pSelf && nIndex >= 0 && nIndex < m_pSelf->m_nChannelsResource)
	{
		uTextColor = m_pSelf->m_ChannelsResource[nIndex].uTextColor.Color_dw;
		uTextBorderColor = m_pSelf->m_ChannelsResource[nIndex].uTextBorderColor.Color_dw;
		return m_pSelf->m_ChannelsResource[nIndex].cTitle;
	}

	return NULL;
}

//nType 0 is me, 1 is friend, 2 is stranger
void KUiMsgCentrePad::SetMSNTextColor(int nType, DWORD uTextColor, DWORD uTextBorderColor)
{
	if (m_pSelf)
	{
		if (nType == 0)
		{
			m_pSelf->m_MSNInfo.uTextColorSelf.Color_dw = uTextColor;
			m_pSelf->m_MSNInfo.uTextColorSelf.Color_b.a = KTC_COLOR;

			m_pSelf->m_MSNInfo.uTextBorderColorSelf.Color_dw = uTextBorderColor;
			m_pSelf->m_MSNInfo.uTextBorderColorSelf.Color_b.a = KTC_BORDER_COLOR;
		}
		else if (nType == 1)
		{
			m_pSelf->m_MSNInfo.uTextFriendColor.Color_dw = uTextColor;
			m_pSelf->m_MSNInfo.uTextFriendColor.Color_b.a = KTC_COLOR;

			m_pSelf->m_MSNInfo.uTextBorderFriendColor.Color_dw = uTextBorderColor;
			m_pSelf->m_MSNInfo.uTextBorderFriendColor.Color_b.a = KTC_BORDER_COLOR;
		}
		else if (nType == 2)
		{
			m_pSelf->m_MSNInfo.uTextColorUnknown.Color_dw = uTextColor;
			m_pSelf->m_MSNInfo.uTextColorUnknown.Color_b.a = KTC_COLOR;

			m_pSelf->m_MSNInfo.uTextBorderColorUnknown.Color_dw = uTextBorderColor;
			m_pSelf->m_MSNInfo.uTextBorderColorUnknown.Color_b.a = KTC_BORDER_COLOR;
		}
	}
}

//nType 0 is me, 1 is friend, 2 is stranger
void KUiMsgCentrePad::GetMSNTextColor(int nType, DWORD& uTextColor, DWORD& uTextBorderColor)
{
	if (m_pSelf)
	{
		if (nType == 0)
		{
			uTextColor = m_pSelf->m_MSNInfo.uTextColorSelf.Color_dw;
			uTextBorderColor = m_pSelf->m_MSNInfo.uTextBorderColorSelf.Color_dw;
		}
		else if (nType == 1)
		{
			uTextColor = m_pSelf->m_MSNInfo.uTextFriendColor.Color_dw;
			uTextBorderColor = m_pSelf->m_MSNInfo.uTextBorderFriendColor.Color_dw;
		}
		else if (nType == 2)
		{
			uTextColor = m_pSelf->m_MSNInfo.uTextColorUnknown.Color_dw;
			uTextBorderColor = m_pSelf->m_MSNInfo.uTextBorderColorUnknown.Color_dw;
		}
	}
}
