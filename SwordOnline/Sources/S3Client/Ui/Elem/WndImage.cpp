/*******************Editer	: duccom0123 EditTime:	2024/06/12 11:48:43*********************
//	界面窗口体系结构--图形窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-25
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "WndImage.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../engine/src/KDebug.h"
extern iRepresentShell*	g_pRepresentShell;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndImage::KWndImage()
{
	IR_InitUiImageRef(m_Image);
	IR_InitUiImageRef(m_EffectImage);
}

void KWndImage::Clone(KWndImage* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_Image	= m_Image;
		pCopy->m_EffectImage	= m_EffectImage;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndImage::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		int		nValue = 0;
		pIniFile->GetInteger(pSection, "Trans", 0, &nValue);
		if (nValue)
			m_Style |= WNDIMG_ES_EXCLUDE_TRANS;
		else
			m_Style &= ~WNDIMG_ES_EXCLUDE_TRANS;
		m_Image.nFlipTime = 0;
		m_Image.nInterval = 0;
		m_Image.nNumFrames = 0;
		pIniFile->GetInteger(pSection, "ImgType", 0, &nValue);
		if (nValue == 1)
		{
			m_Image.nType = ISI_T_BITMAP16;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
			m_Style &= ~WNDIMG_ES_EXCLUDE_TRANS;
		}
		else
		{
			m_Image.nType = ISI_T_SPR;
			m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
		}
		pIniFile->GetString(pSection, "Image", "" , m_Image.szImage, sizeof(m_Image.szImage));
		m_Image.uImage = 0;
		m_Image.nISPosition = IMAGE_IS_POSITION_INIT;

		if (pIniFile->GetInteger(pSection, "Frame", -1, &nValue) && nValue != -1)
			m_Image.nFrame = nValue;

		if ((m_Width == 0 || m_Height == 0) && g_pRepresentShell)
		{
			KImageParam	Param = { 0 };
			if (g_pRepresentShell->GetImageParam(m_Image.szImage, &Param, m_Image.nType))
			{
				if (m_Width == 0)
					m_Width = Param.nWidth;
				if (m_Height == 0)
					m_Height = Param.nHeight;
			}
		}
		
		m_EffectImage.nFlipTime = 0;
		m_EffectImage.nInterval = 0;
		m_EffectImage.nNumFrames = 0;	
		m_EffectImage.nType = ISI_T_SPR;		
		m_EffectImage.uImage = 0;
		m_EffectImage.nISPosition = IMAGE_IS_POSITION_INIT;
		pIniFile->GetString(pSection, "EffectImage", "" , m_EffectImage.szImage, sizeof(m_EffectImage.szImage));
		m_EffectImage.nFrame = 0;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：判断一个点是否在窗口范围内,传入的是绝对坐标
//--------------------------------------------------------------------------
int KWndImage::PtInWindow(int x, int y)
{
	int	bIn = KWndWindow::PtInWindow(x, y);
	
	if ((bIn &&
			(m_Style & WND_S_SIZE_WITH_ALL_CHILD) == 0) || 
		((m_Style & WND_S_VISIBLE) &&
			bIn == false &&
			(m_Style & WND_S_SIZE_WITH_ALL_CHILD)))
	{
		if ((m_Style & WNDIMG_ES_EXCLUDE_TRANS) && g_pRepresentShell)
				bIn =  g_pRepresentShell->GetImagePixelAlpha(m_Image.szImage, m_Image.nFrame, x - m_nAbsoluteLeft, y - m_nAbsoluteTop, m_Image.nType);
	}
	return bIn;
}

//--------------------------------------------------------------------------
//	功能：设置图形帧
//--------------------------------------------------------------------------
void KWndImage::SetFrame(int nFrame)
{
	m_Image.nFrame = nFrame;
}

void KWndImage::SetImage(short nType, const char* pszImgName, bool bAdjustWndSize)
{
	m_Image.nType = nType;
	strncpy(m_Image.szImage, pszImgName, sizeof(m_Image.szImage));
	m_Image.szImage[sizeof(m_Image.szImage) - 1] = 0;
	m_Image.nFrame = 0;
	m_Image.nNumFrames = 0;
	m_Image.uImage = 0;
	m_Image.nFlipTime = IR_GetCurrentTime();
	if (bAdjustWndSize && g_pRepresentShell)
	{
		KImageParam	Param;
		if (g_pRepresentShell->GetImageParam(m_Image.szImage, &Param, nType))
		{
			SetSize(Param.nWidth, Param.nHeight);
			m_Image.nNumFrames = Param.nNumFrames;
		}
	}
}

char* KWndImage::GetImage()
{
	return m_Image.szImage;
}

void KWndImage::RemoveImage()
{
	memset(m_Image.szImage, 0, sizeof(m_Image.szImage));
	m_Image.nNumFrames = 0;
	m_Image.uImage = 0;
	m_Image.nFlipTime = IR_GetCurrentTime();
}

void KWndImage::UpdateTimer()
{
	m_Image.nFlipTime = IR_GetCurrentTime();
}

void KWndImage::SetMaxFrame(int nMaxFrame) 
{ 
    m_Image.nNumFrames = nMaxFrame; 
}  
int KWndImage::GetMaxFrame() 
{ 
    return m_Image.nNumFrames; 
}  
//--------------------------------------------------------------------------
//	功能：图形换帧
//--------------------------------------------------------------------------
int KWndImage::NextFrame()
{
	return IR_NextFrame(m_Image);
}

//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndImage::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (g_pRepresentShell)
	{
		m_Image.oPosition.nX = m_nAbsoluteLeft;
		m_Image.oPosition.nY = m_nAbsoluteTop;
		g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
	}
}