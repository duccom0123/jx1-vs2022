/*****************************************************************************************
//	界面窗口体系结构-纯文字按钮
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-1-19
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndPureTextBtn.h"
#include "Text.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "MouseHover.h"
#include "../UiCase/UiChatItem.h"

#include "../../../Engine/src/Text.h"
#include    "../../Engine/Src/KSG_StringProcess.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;
unsigned int KWndPureTextBtn::ms_uBtnTipTextColor = 0xffffffff;	//按钮提示名称文字的颜色

KWndPureTextBtn::KWndPureTextBtn()
{
	m_Flag = 0;
	m_sText[0] = 0;
	m_nTextLen = 0;
	m_nFontSize = 12;
	m_NormalColor = m_OverColor = m_PressedColor = 0;
	m_NormalBorderColor = m_OverBorderColor = m_PressedBorderColor = 0;
	m_szTip[0] = 0;
	m_nTipLen = 0;	
	m_pItem[0] = 0;
	m_X = 0;
	m_Y = 0;
	m_bHaveItem = FALSE;
}

void KWndPureTextBtn::Clone(KWndPureTextBtn* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_NormalColor = m_NormalColor;
		pCopy->m_OverColor = m_OverColor;
		pCopy->m_PressedColor = m_PressedColor;
		pCopy->m_nFontSize = m_nFontSize;
		pCopy->m_NormalBorderColor = m_NormalBorderColor;
		pCopy->m_OverBorderColor = m_OverBorderColor;
		pCopy->m_PressedBorderColor = m_PressedBorderColor;
	}
}

int KWndPureTextBtn::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "Font", 12, &m_nFontSize);
		if (m_nFontSize < 8)
			m_nFontSize = 8;
		int	nValue;
		pIniFile->GetInteger(pSection, "CentreAlign", 1, &nValue);
		if (nValue)
			m_Style |= WNDPTB_S_CENTRE_ALIGN;
		else
			m_Style &= ~WNDPTB_S_CENTRE_ALIGN;
		
		char	Buff[256];
		pIniFile->GetString(pSection, "Color", "", Buff, sizeof(Buff));
		m_NormalColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "0,0,0", Buff, 16);
		m_NormalBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "OverColor", "", Buff, sizeof(Buff));
		m_OverColor = GetColor(Buff);
		pIniFile->GetString(pSection, "OverBorderColor", "0,0,0", Buff, 16);
		m_OverBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelColor", "", Buff, sizeof(Buff));
		m_PressedColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelBorderColor", "0,0,0", Buff, 16);
		m_PressedBorderColor = GetColor(Buff);
		if (m_sText && m_sText[0] == 0)
		{
			if (pIniFile->GetString(pSection, "Text", "", Buff, 256))
			{
				strncpy(m_sText, Buff, WNDPTB_MAX_TEXT_LEN - 1);
				m_sText[WNDPTB_MAX_TEXT_LEN - 1] = 0;
				m_nTextLen = strlen(m_sText);
			}
		}
		pIniFile->GetString(pSection, "Tip", "", m_szTip, sizeof(m_szTip));
		m_nTipLen = TEncodeText(m_szTip, strlen(m_szTip));
//		AdjustPosition();
	}
	return false;
}

void KWndPureTextBtn::AdjustPosition()
{
	int nTextWidth = m_nTextLen * m_nFontSize / 2;
	if (m_Style & WNDPTB_S_CENTRE_ALIGN)
		SetPosition(m_Left + (m_Width - nTextWidth) / 2, m_Top);
	SetSize(nTextWidth, m_Height);

}

//绘制窗口
void KWndPureTextBtn::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (m_sText == NULL || m_sText[0] == 0 || g_pRepresentShell == NULL)
		return;
	KOutputTextParam param;
	if (m_Flag & WNDPTB_F_BE_PRESSEDDOWN ||
		m_Flag & WNDPTB_F_CHECK)
	{
		param.Color = m_PressedColor;
		param.BorderColor = m_PressedBorderColor;
	}
	else if (m_Flag & WNDPTB_F_OVER)
	{
		param.Color = m_OverColor;
		param.BorderColor = m_OverBorderColor;
	}
	else
	{
		param.Color = m_NormalColor;
		param.BorderColor = m_NormalBorderColor;
	}
	param.nX = m_nAbsoluteLeft + (m_Width - m_nTextLen * m_nFontSize / 2) / 2;
	param.nY = m_nAbsoluteTop;
	param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
	param.nSkipLine = 0;
	param.nNumLine = 100;
	g_pRepresentShell->OutputRichText(m_nFontSize, &param, m_sText, m_nTextLen);
}

//设置文本文字
void KWndPureTextBtn::SetText(const char* pText, int nLen)
{
	if (pText == NULL)
		nLen = 0;
	else if (nLen < 0)
		nLen = strlen(pText);
	if (nLen <= WNDPTB_MAX_TEXT_LEN)
		m_nTextLen = nLen;
	else
		m_nTextLen = WNDPTB_MAX_TEXT_LEN;

	if (m_nTextLen > 0)
		memcpy(m_sText, pText, m_nTextLen);
//	AdjustPosition();
}

int KWndPureTextBtn::GetTextLen()
{
	int nRet = 0;
	if (m_sText)
		nRet = m_nTextLen;
	return nRet;
}

void KWndPureTextBtn::CheckButton(int bChecked)
{
	if (bChecked)
		m_Flag |= WNDPTB_F_CHECK;
	else
		m_Flag &= ~WNDPTB_F_CHECK;
}

//--------------------------------------------------------------------------
//	功能：窗口函数（处理消息）
//--------------------------------------------------------------------------
int KWndPureTextBtn::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (IsDisable())
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	switch(uMsg)
	{
	case WM_LBUTTONUP:
		//if (g_pCoreShell->CheckMouse())
		//	return 0;
		if (m_Flag & WNDPTB_F_BE_PRESSEDDOWN)
		{
			m_Flag &= ~WNDPTB_F_BE_PRESSEDDOWN;
			if(m_bHaveItem && g_pCoreShell)
			{
				if(KUiChatItem::GetIfVisible()) KUiChatItem::CloseWindow();

				char m_pTItem[MAX_SENTENCE_LENGTH];
				memset(m_pTItem, 0, sizeof(m_pTItem));
				memcpy(m_pTItem, m_pItem, sizeof(m_pItem));
				int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&m_pTItem);
				if(nIdx)
				{
					KUiChatItem::OpenWindow(nIdx);
				}
			}
			else if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_BUTTON_CLICK,
					(unsigned int)(KWndWindow*)this, 0);
		}
		break;
	case WM_LBUTTONDOWN:
		m_Flag |= WNDPTB_F_BE_PRESSEDDOWN;
		break;
	case WM_MOUSEMOVE:
		m_Flag |= WNDPTB_F_OVER;
		break;
	case WND_M_MOUSE_LEAVE:
		m_Flag &= ~WNDPTB_F_OVER;
		m_Flag &= ~WNDPTB_F_BE_PRESSEDDOWN;
		KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	case WM_MOUSEHOVER:
		if (m_szTip[0] && !g_MouseOver.IsMoseHoverWndObj((void*)(KWndWindow*)this, 0))
		{
			char szTip[256];
			
			int nTipLen = GetToolTipInfo(szTip, 255);
			szTip[nTipLen] = 0;
			
			g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this, 0,
				LOWORD(nParam), HIWORD(nParam), false, true);
			g_MouseOver.SetMouseHoverTitle(szTip, nTipLen, ms_uBtnTipTextColor);
		}
		break;		
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

int	KWndPureTextBtn::GetToolTipInfo(char* szTip, int nMax)
{
	int nTipLen = min(m_nTipLen, nMax);
	memcpy(szTip, m_szTip, nTipLen);
	const char* pKey = GetShortKey();
	if (pKey && pKey[0] != 0 && (nTipLen + (int)strlen(pKey)) < nMax)
	{
		szTip[nTipLen++] =  '(';
		strcpy(szTip + nTipLen, pKey);
		nTipLen += strlen(pKey);
		szTip[nTipLen++] =  ')';
	}

	return nTipLen;
}

int KWndPureTextBtn::SetToolTipInfo(char* szTip, int nMax)
{
	m_nTipLen = min(nMax, sizeof(m_szTip) - 1);
	memcpy(m_szTip, szTip, m_nTipLen);
	m_szTip[m_nTipLen] = 0;
	m_nTipLen = TEncodeText(m_szTip, strlen(m_szTip));
	return m_nTipLen;
}

BOOL KWndPureTextBtn::IsChecked() 
{ 
    if (m_Flag & WNDPTB_F_CHECK) 
        return true; 
    else 
        return false; 
} 

void KWndPureTextBtn::Set3IntText(int nNumber1, int nNumber2, char Separator, char* Find)
{
	char	Buff[32];
	int		nLen;
	if (m_pParentWnd)
	{
		nLen = sprintf(Buff, "%d%c%d %s", nNumber1, Separator, nNumber2, Find);
		SetText(Buff, nLen);
	}
}