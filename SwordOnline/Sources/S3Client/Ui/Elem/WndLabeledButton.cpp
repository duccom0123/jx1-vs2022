/*****************************************************************************************
//	界面窗口体系结构--带文字按键窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-10-4
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include "../Elem/WndMessage.h"
#include "WndLabeledButton.h"
#include "Wnds.h"
#include "../../../Engine/Src/Text.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;

//===========================================
//	简单的带文字按钮窗口。
//	文字单行，居中处理，文字中不包含控制符。
//===========================================

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndLabeledButton::KWndLabeledButton()
{
	m_Label[0]  = 0;
	m_Label[31] = 0;
	m_nLabelLen = 0;
	m_nFontSize = 16;
	m_nLabelXOffset = 0;
	m_nLabelYOffset = 0;
	m_FontColor = 0;
	m_FontBorderColor = 0;
	m_OverColor = 0;
	m_OverBorderColor = 0;
	m_EnableColor = 0;
	m_EnableBdColor = 0;
	m_DisableColor = 0;
	m_DisableBdColor = 0;
}

void KWndLabeledButton::Clone(KWndLabeledButton* pCopy)
{
	if (pCopy)
	{
		KWndButton::Clone(pCopy);
		pCopy->m_nFontSize = m_nFontSize;
		pCopy->m_FontColor = m_FontColor;
		pCopy->m_OverColor = m_OverColor;
		pCopy->m_nLabelXOffset = m_nLabelXOffset;
		pCopy->m_nLabelYOffset = m_nLabelYOffset;
		pCopy->m_FontBorderColor = m_FontBorderColor;
		pCopy->m_OverBorderColor = m_OverBorderColor;
		pCopy->m_DisableColor = m_DisableColor;
		pCopy->m_DisableBdColor = m_DisableBdColor;
		strcpy(pCopy->m_Label, m_Label);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndLabeledButton::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndButton::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "Font", 16, &m_nFontSize);
		pIniFile->GetInteger(pSection, "LabelXOffset", 0, &m_nLabelXOffset);
		pIniFile->GetInteger(pSection, "LabelYOffset", 0, &m_nLabelYOffset);
		if (m_nFontSize < 12)
			m_nFontSize = 16;
		//===读取文字====
		char	Buff[32];
		pIniFile->GetString(pSection, "Color", "", Buff, sizeof(Buff));
		m_FontColor = GetColor(Buff);
		m_EnableColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "", Buff, sizeof(Buff));
		m_FontBorderColor = GetColor(Buff);
		m_EnableBdColor = GetColor(Buff);
		pIniFile->GetString(pSection, "OverColor", "", Buff, sizeof(Buff));
		m_OverColor = GetColor(Buff);
		pIniFile->GetString(pSection, "OverBorderColor", "", Buff, sizeof(Buff));
		m_OverBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "DisableColor", "", Buff, sizeof(Buff));
		m_DisableColor = GetColor(Buff);
		pIniFile->GetString(pSection, "DisableBorderColor", "", Buff, sizeof(Buff));
		m_DisableBdColor = GetColor(Buff);
		pIniFile->GetString(pSection, "Label", "", Buff, sizeof(Buff));
		SetLabel(Buff);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：设置按钮标题文字
//--------------------------------------------------------------------------
void KWndLabeledButton::SetLabel(const char* pLabel)
{
	if (pLabel)
	{
		m_nLabelLen = strlen(pLabel);
		if (m_nLabelLen > 31)
			m_nLabelLen = 31;
		memcpy(m_Label, pLabel, m_nLabelLen);
		m_Label[m_nLabelLen] = 0;
	}
}

void KWndLabeledButton::Set3IntLabel(int nNumber, char Separator)
{
	char	Buff[32];
	int		nLen;
	if (Separator)
		nLen = sprintf(Buff, "%d%c", nNumber, Separator);
	else
	{
		int vNumber = nNumber / MONEY_FLOOR;
		int lNumber = nNumber % MONEY_FLOOR;
		if (nNumber < MONEY_FLOOR)
			nLen = sprintf(Buff, "%d lng", nNumber);
		else
		{
			if(lNumber == 0)
				nLen = sprintf(Buff, "%d v筺 lng", vNumber);
			else
				nLen = sprintf(Buff, "%d v筺 %d lng", vNumber, lNumber);
		}
	}
	SetLabel(Buff);
}

void KWndLabeledButton::Enable(int bEnable)
{
	KWndButton::Enable(bEnable);
	if (bEnable)
	{
		SetLabelColor(m_EnableColor);
		if (m_EnableBdColor)
			SetLabelBorderColor(m_EnableBdColor);
		m_Style &= ~WND_S_DISABLE;
	}
	else
	{
		if (m_DisableColor)
			SetLabelColor(m_DisableColor);
		if (m_DisableBdColor)
			SetLabelBorderColor(m_DisableBdColor);
		m_Style |= WND_S_DISABLE;
	}
}

int KWndLabeledButton::GetLabel(char* pLabel, int nSize)
{
	int nRet = 0;
	if (pLabel)
	{		
		if (m_nLabelLen < nSize)
		{			
			memcpy(pLabel, m_Label, m_nLabelLen + 1);
			nRet = m_nLabelLen;
		}
		else
		{
			pLabel[0] = 0;
			nRet = 0;
		}
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：设置按钮标题文字颜色
//--------------------------------------------------------------------------
void KWndLabeledButton::SetLabelColor(unsigned int Color)
{
	m_FontColor = Color;
}

void KWndLabeledButton::SetLabelBorderColor(unsigned int Color)
{
	m_FontBorderColor = Color;
}


//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndLabeledButton::PaintWindow()
{
	KWndButton::PaintWindow();
	if (g_pRepresentShell)
	{
		char	Buffer[32];
		int nMaxLen = m_Width * 2 / m_nFontSize;
		const char* pShowString = TGetLimitLenString(m_Label, -1, Buffer, nMaxLen);
		if (pShowString)
		{
			int nLen = strlen(pShowString);
			unsigned int uColor = m_FontColor;
			unsigned int uBorderColor = m_FontBorderColor;
			if (m_Flag & WNDBTN_F_OVER)
			{
				uColor = m_OverColor;
				uBorderColor = m_OverBorderColor;
			}
			int nX = m_nAbsoluteLeft;
			int nY = m_nAbsoluteTop;
			g_pRepresentShell->OutputText(m_nFontSize, pShowString, nLen,
				nX + m_nLabelXOffset + (m_Width - nLen * m_nFontSize / 2) / 2,
				nY + m_nLabelYOffset,
				uColor , 0,
				TEXT_IN_SINGLE_PLANE_COORD, uBorderColor);
		}
	}
}
