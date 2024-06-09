/*****************************************************************************************
//	浮动提示窗口
//	Copyright : Kingsoft 2002-2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-23
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "KTabFile.h"
#include "KStrBase.h"
#include "GameDataDef.h"
#include "KCore.h"
#include "SpaceOver.h"
#include "AutoLocateWnd.h"
#include "WndWindow.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Engine/Src/Text.h"
#include "SpecialFuncs.h"
extern iRepresentShell*	g_pRepresentShell;

#include "../../../core/src/CoreShell.h"
extern iCoreShell*	g_pCoreShell;

KSpaceOver	g_SpaceOver;
#define	SCHEME_INI				"弹出说明文字.ini"
#define	INFO_MIN_LEN			26
#define FOLLOW_CURSOR_OFFSET_X	16
#define FOLLOW_CURSOR_OFFSET_Y	8

static unsigned int	s_uHoverObjDestTextColor = 0xffffffff;	//对象提示文字的颜色

void SetSpaceOverObjDescColor(unsigned int uColor)
{
	s_uHoverObjDestTextColor = uColor;
}

void SetSpaceOverObjectDesc(void* pWnd, int nObj, unsigned int uGenre,
			unsigned int uId, int nContainer, int x, int y, unsigned int uHoverTime, bool LAlign)
{
	char szTitle[GOD_MAX_OBJ_TITLE_LEN];

	int		nLenTitle = 0;
	
	g_SpaceOver.CancelSpaceOverInfo();
	if (g_pCoreShell)
	{
		KUiObjAtContRegion	Obj;
		Obj.Obj.uGenre = uGenre;
		Obj.Obj.uId = uId;
		Obj.Region.h = Obj.Region.v = 0;
		Obj.Region.Width = Obj.Region.Height = 0;
		Obj.nContainer = nContainer;
		szTitle[0] = 0;
		unsigned uIndex = GDI_GAME_OBJ_DESC;
		if (g_UiBase.GetStatus() == UIS_S_TRADE_REPAIR)
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_REPAIRINFO;
		else if (g_UiBase.GetStatus() != UIS_S_IDLE)
			uIndex = GDI_GAME_OBJ_DESC_INCLUDE_TRADEINFO;
	
		g_pCoreShell->GetGameData(uIndex, (unsigned int)&Obj, (int)&szTitle);
		g_SpaceOver.SetSpaceOverInfo(pWnd, nObj, x, y, uHoverTime, LAlign, false);
		g_SpaceOver.SetSpaceOverImage(g_pCoreShell->GetGameData(GDI_GAME_OBJ_DESC_INCLUDE_MOUSEHOVER, (unsigned int)&Obj, (int)&g_SpaceOver.m_HoverImage.szImage) == 1);

		if (szTitle[0])
		{
			nLenTitle = TEncodeText(szTitle, strlen(szTitle));
			g_SpaceOver.SetSpaceOverTitle(szTitle, nLenTitle,s_uHoverObjDestTextColor);
		}
	}	
}

KSpaceOver::KSpaceOver()
{
	m_nLeft      = 0;
	m_nTop       = 0;
	m_nWndWidth  = 0;
	m_nWndHeight = 0;
	m_nWndWidthReduce = 0;
	m_nWndHeightReduce = 0;
	m_nWidthReduce = 0;
	m_nHeightReduce = 0;
	m_nMaxWidthReduce = 0;
	m_nMaxHeightReduce = 0;
	m_nIndent    = 0;
	m_nFontSize  = 12;
	m_pSpaceOverWnd = NULL;
	m_nObj = 0;
	m_ObjTitle[0]  =0;
	m_nTitleLen    = 0;
	m_uTitleColor  = 0;
	m_uTitleBgColor= 0;
	//////////////////
	m_uBoderShadowColor = 0;      //属性文字颜色
	//////////////////
	memset(&m_Image, 0, sizeof(KRUImage));
	IR_InitUiImageRef(m_HoverImage);
	m_bTempImg = false;
	m_bFollowCursor = false;
	m_LAlign = false;
	m_bShow = false;
	m_uHoverTime = 0;
	m_uLastHoverTime = IR_GetCurrentTime();
}

int  KSpaceOver::IsSpaceOverWndObj(void* pWnd, int nObj)
{
	return ((pWnd == m_pSpaceOverWnd) && (nObj == m_nObj));
}

void KSpaceOver::CancelSpaceOverInfo()
{
	m_pSpaceOverWnd = 0;
	m_nObj = 0;
	m_ObjTitle[0] = 0;
	m_nTitleLen = 0;
	m_bShow = false;
	if (m_bTempImg)
		SetSpaceOverImage(false);
}

/***********************************************************************
*功能：设置鼠标提示窗口的基本信息
************************************************************************/
void KSpaceOver::SetSpaceOverInfo(void* pWnd, int nObj, int x, int y, unsigned int uHoverTime, bool LAlign, bool bFollowCursor)
{
    m_pSpaceOverWnd = pWnd;
	m_nObj = nObj;
	m_bFollowCursor = bFollowCursor;
	m_nApplyX = x;
	m_nApplyY = y;
	m_ObjTitle[0] = 0;
	m_nTitleLen = 0;
	m_uHoverTime = uHoverTime;
	m_uLastHoverTime = IR_GetCurrentTime();
	m_LAlign = LAlign;
	m_bShow = false;
}

void KSpaceOver::SetSpaceOverImage(bool bAdd)
{
	KImageParam	Param;
	if (bAdd)
	{
		if (g_pRepresentShell->GetImageParam(m_HoverImage.szImage, &Param, ISI_T_SPR) == true)
		{
			m_HoverImage.nFlipTime = IR_GetCurrentTime();
			//m_HoverImage.nInterval = Param.nInterval;
			m_HoverImage.nInterval = 80; //true
			m_HoverImage.nNumFrames = Param.nNumFrames;
			m_HoverImage.nType = ISI_T_SPR;
			m_HoverImage.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_HoverImage.Color.Color_b.a = 255;
			m_HoverImage.uImage = 0;
			m_HoverImage.nISPosition = IMAGE_IS_POSITION_INIT;
			m_HoverImage.nFrame = 0;

			m_nWndWidthReduce = Param.nWidth / 2;
			m_nWndHeightReduce = Param.nHeight / 4;
		}
		m_bTempImg = true;
	}
	else
	{
		IR_InitUiImageRef(m_HoverImage);
		m_nWndWidthReduce = 0;
		m_nWndHeightReduce = 0;
		m_nHeightReduce = 0;
		m_nMaxWidthReduce = 0;
		m_nMaxHeightReduce = 0;
		m_bTempImg = false;
	}
}

/***********************************************************************
*功能：设置鼠标提示窗口的标题(物件的名字)
************************************************************************/
void KSpaceOver::SetSpaceOverTitle(const char *pTitleText, int nTitleTextLen, UINT uColor)
{
	if (nTitleTextLen > 0 && pTitleText && nTitleTextLen <= GOD_MAX_OBJ_TITLE_LEN)
	{
		memcpy(m_ObjTitle, pTitleText, nTitleTextLen);
		m_nTitleLen = nTitleTextLen;
		m_uTitleColor = uColor;
		Update(m_nApplyX, m_nApplyY);
	}
	else
	{
		m_ObjTitle[0] = 0;
		m_nTitleLen = 0;
	}
}

void KSpaceOver::Update(int nX, int nY)
{
	m_bShow = false;

	if (g_pRepresentShell == NULL)
		return;

	int nMaxTitleLen;

	m_nMaxLineLen = 0;
	if (m_nTitleLen > 0)
	{
		m_nTitleLineNum = TGetEncodedTextLineCount(
			m_ObjTitle, m_nTitleLen, 0, nMaxTitleLen, m_nFontSize);
		m_nMaxLineLen = nMaxTitleLen;
	}
	else
	{
		m_nTitleLineNum = 0;
	}

	int nNumLine = m_nTitleLineNum;
	if (nNumLine == 0)
		return;

	if (m_bFollowCursor == false && m_nMaxLineLen < INFO_MIN_LEN)
		m_nMaxLineLen = INFO_MIN_LEN;

	if (m_nMaxWidthReduce < m_nWndWidthReduce)
		m_nMaxWidthReduce = m_nWndWidthReduce;
	if (m_nMaxHeightReduce < m_nWndHeightReduce)
		m_nMaxHeightReduce = m_nWndHeightReduce;

	m_nWndWidth = m_nMaxWidthReduce + m_nFontSize * m_nMaxLineLen / 2 + m_nIndent * 2;
	m_nWndHeight = (m_nFontSize + 1) * nNumLine + 5;

	if (m_nWndHeight < m_nMaxHeightReduce)
		m_nWndHeight = m_nMaxHeightReduce;

	if (m_bFollowCursor)
	{
		m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
		m_nTop  = nY + FOLLOW_CURSOR_OFFSET_Y;
	}
	else
	{
		//ALW_GetWndPosition(m_nLeft, m_nTop, m_nWndWidth, m_nWndHeight);
		m_nLeft = nX;
		m_nTop = nY;
	}

	//可以显示浮动窗口啦!
	m_bShow = true;
}


//鼠标的位置更新了
void KSpaceOver::UpdateCursorPos(int nX, int nY)
{
	if (m_bFollowCursor && m_bShow)
	{
		m_nLeft = nX + FOLLOW_CURSOR_OFFSET_X;
		m_nTop  = nY + FOLLOW_CURSOR_OFFSET_Y;
	}
}


void KSpaceOver::OnWndClosed(void* pWnd)
{
	if (pWnd && pWnd == m_pSpaceOverWnd)
		CancelSpaceOverInfo();
}

//载入界面方案
void KSpaceOver::LoadScheme(const char* pScheme)
{
	if (pScheme == NULL)
		return;
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Ini.GetInteger("MouseOverWnd", "Indent",      0, &m_nIndent);
		Ini.GetInteger("MouseOverWnd", "Font",        0, &m_nFontSize);
		
		if(m_nFontSize < 8)
			m_nFontSize = 8;
		int nValue;
		Ini.GetInteger("Main", "ImgType", 0, &nValue);
		if (nValue == 1)
		{
			m_Image.nType = ISI_T_BITMAP16;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
		}
		else
		{
			m_Image.nType = ISI_T_SPR;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_Image.Color.Color_b.a = 255;
		}
		m_Image.uImage = 0;
		m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
		Ini.GetString("Main", "Image", "" , m_Image.szImage, sizeof(m_Image.szImage));
		Ini.GetInteger("Main", "Frame", 0, &nValue);
		m_Image.nFrame = nValue;

		Ini.GetString("Main", "TitleBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uTitleBgColor = (GetColor(Buff) & 0xffffff) | ((220 << 21) & 0x0a000000);
		
		////////////////////////////////////////
		Ini.GetString("Main", "BorderBgColor", "0, 0, 0", Buff, sizeof(Buff));
		m_uBoderShadowColor = (GetColor(Buff) & 0xffffff) | ((180 << 21) & 0xff000000);
		////////////////////////////////////////

		Update((m_nLeft + m_nWndWidth) / 2, m_nTop);
	}
}


void KSpaceOver::PaintSpaceOverInfo()
{
	if (m_bShow == false || g_pRepresentShell == NULL)
		return;

	if (m_uHoverTime > 0 && IR_IsTimePassed(m_uHoverTime, m_uLastHoverTime))
		m_bShow = false;
	//画背景阴影和边框
	KRUShadow Shadow;
	//写文字初始化
	KOutputTextParam	Param;
	Param.BorderColor = 0;
	Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

	//====标题(名字)部分====
	if (m_nLeft + m_nWndWidth <= RESOLUTION_WIDTH)
		Shadow.oPosition.nX = m_nLeft;
	else
	{
		if (m_nLeft - m_nWndWidth  - 10 < 0)
			Shadow.oPosition.nX = 0;
		else
			Shadow.oPosition.nX = m_nLeft - m_nWndWidth  - 10;
	}
		
	Shadow.oPosition.nY = m_nTop - 5;
		
	if (m_nLeft + m_nWndWidth <= RESOLUTION_WIDTH)
		Shadow.oEndPos.nX = m_nLeft + m_nWndWidth;
	else
	{
		if (m_nLeft - m_nWndWidth  - 10 < 0)
			Shadow.oEndPos.nX = m_nWndWidth;
		else
			Shadow.oEndPos.nX = m_nLeft - 10;
			
	}
	Shadow.oEndPos.nY = Shadow.oPosition.nY + m_nWndHeight;

	if (m_nTitleLen > 0)
	{
		Shadow.Color.Color_dw = m_uTitleBgColor;
		//Shadow.oEndPos.nY += (m_nFontSize + 1) * m_nTitleLineNum;
		if (!m_LAlign)
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

		Param.nSkipLine = 0;
		Param.nNumLine = 1;
		Param.Color = m_uTitleColor;
		Param.nY = Shadow.oPosition.nY + 5;
		int nLineLen;
		while(true)
		{
			if (TGetEncodedTextLineCount(m_ObjTitle, m_nTitleLen, 0, nLineLen, m_nFontSize, Param.nSkipLine, 1) == 0)
				break;
			if (m_nLeft + m_nWndWidth <= RESOLUTION_WIDTH)
			{
				if (m_LAlign)
					Param.nX = m_nLeft + m_nMaxWidthReduce + m_nIndent;
				else
					Param.nX = m_nLeft + (m_nWndWidth + m_nMaxWidthReduce) / 2 - (nLineLen * m_nFontSize) / 4;
			}
			else
			{
				if (m_nLeft - (m_nWndWidth + m_nMaxWidthReduce)  - 10 < 0)
				{
					if (m_LAlign)
						Param.nX = m_nLeft + m_nMaxWidthReduce + m_nIndent;
					else
						Param.nX = (m_nWndWidth + m_nMaxWidthReduce) / 2 - (nLineLen * m_nFontSize) / 4;
					
				}
				else
				{
					if (m_LAlign)
						Param.nX = Shadow.oPosition.nX  - 10 - m_nMaxWidthReduce + m_nIndent;
					else
						Param.nX = m_nLeft  - 10 - (m_nWndWidth + m_nMaxWidthReduce) / 2 - (nLineLen * m_nFontSize) / 4;
				}
			}
			g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_ObjTitle, m_nTitleLen, 0);
			Param.nSkipLine ++;
			Param.nY += m_nFontSize + 1;
		};
	}

	if (m_bTempImg)
	{
		m_HoverImage.oPosition.nX = m_nLeft - m_nWndWidthReduce / 2;
		m_HoverImage.oPosition.nY = m_nTop - m_nWndHeightReduce - m_nWndHeight / 2 ;
		IR_NextFrame(m_HoverImage);
		g_pRepresentShell->DrawPrimitives(1, &m_HoverImage, RU_T_IMAGE, true);
	}
}