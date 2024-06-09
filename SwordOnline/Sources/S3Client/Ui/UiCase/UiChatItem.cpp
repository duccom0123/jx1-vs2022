#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiChatItem.h"
#include "UiSysMsgCentre.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iCoreShell*	g_pCoreShell;
extern iRepresentShell *g_pRepresentShell;


KUiChatItem*			KUiChatItem::m_pSelf = NULL;


#define		SCHEME_INI			"UiChatItem.ini"

KUiChatItem::KUiChatItem()
{
	m_uBoderColor = 0;
	m_pItemLen = 0;
	m_pItemDesc[0] = 0;
	m_nNumLine = 0;
	m_nMaxLineLen = 0;
	memset(&m_Image, 0, sizeof(KRUImage));
	m_nWidth = 0;
	m_nHeight = 0;
}

KUiChatItem::~KUiChatItem()
{
}

KUiChatItem* KUiChatItem::OpenWindow(int nIdx)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiChatItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData(nIdx);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiChatItem::CloseWindow()
{
	if (m_pSelf)
	{
		KUiDraggedObject	Obj;
		m_pSelf->m_ItemBox.GetObject(Obj, 0, 0);
		if(Obj.uId)
			g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, Obj.uId);
//		m_pSelf->Destroy();
//		m_pSelf = NULL;
		m_pSelf->Hide();
	}
}

KUiChatItem* KUiChatItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiChatItem::Initialize()
{	
	AddChild(&m_ItemBox);
	m_ItemBox.SetContainerId((int)UOC_GAMESPACE);
	m_ItemBox.EnablePickPut(false);
	m_ItemBox.EnableMouseHover(false);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


void KUiChatItem::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;

		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);

		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
			Ini.GetString("Main", "BgColor", "0,0,0", Buff, 128);
			m_pSelf->m_uBoderColor = (GetColor(Buff) & 0x00ffffff);
			int nAlpha;
			Ini.GetInteger("Main", "BgAlpha", 0, (int*)&nAlpha);
			m_pSelf->m_uBoderColor |= (nAlpha << 24);	
			
			Ini.GetInteger("Main", "ImgWidth",    0, (int*)&m_pSelf->m_nWidth);
			Ini.GetInteger("Main", "ImgHeight",   0, (int*)&m_pSelf->m_nHeight);
			if (m_pSelf->m_nWidth < 0)
				m_pSelf->m_nWidth = 0;
			if (m_pSelf->m_nHeight < 0)
				m_pSelf->m_nHeight = 0;
			m_pSelf->m_Image.nType = ISI_T_SPR;
			m_pSelf->m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
			m_pSelf->m_Image.Color.Color_b.a = 255;
			m_pSelf->m_Image.uImage = 0;
			m_pSelf->m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
			Ini.GetString("Main", "Image", "" , m_pSelf->m_Image.szImage, sizeof(m_pSelf->m_Image.szImage));
			m_pSelf->m_Image.nFrame = 0;

			Ini.GetString("Main", "RectColor", "0,0,0", Buff, sizeof(Buff));
			m_pSelf->m_nRectColor = (GetColor(Buff) & 0x00ffffff);
		}
	}
}


int KUiChatItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_LEFT_CLICK_ITEM:
		CloseWindow();
		break;
	case WM_LBUTTONDOWN:
		if (HIWORD(nParam) - m_nAbsoluteTop > m_nHeight && HIWORD(nParam) < m_nAbsoluteTop + m_Height - m_nHeight)
			CloseWindow();
		break;
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}
#define PREFIX_SIZE 20
void KUiChatItem::UpdateData(int nIdx)
{
	if (nIdx > 0)
	{
		g_pCoreShell->GetGameData(GDI_ITEM_DESC, (unsigned int)&m_pItemDesc, nIdx);
		m_pItemLen = TEncodeText(m_pItemDesc, strlen(m_pItemDesc));

		int w,h;
		g_pCoreShell->GetSizeItem(nIdx, &w, &h);
		m_ItemBox.Clear();
		KUiDraggedObject Obj;
		Obj.uGenre = CGOG_ITEM;
		Obj.uId = nIdx;
		Obj.DataX = 0;
		Obj.DataY = 0;
		Obj.DataW = w;
		Obj.DataH = h;
		m_ItemBox.AddObject(&Obj, 1);
		w *= 28;
		h *= 28;
		m_ItemBox.SetSize(w,h);
		m_nNumLine = TGetEncodedTextLineCount(m_pItemDesc, m_pItemLen, 0, (int&)m_nMaxLineLen, NORMAL_FONTSIZE);
		m_Width = NORMAL_FONTSIZE * m_nMaxLineLen / 2 + m_nWidth/2 + w + PREFIX_SIZE;
		m_Height = (NORMAL_FONTSIZE + 1) * m_nNumLine + (m_nHeight*2) + PREFIX_SIZE;

		if (m_Height < h + NORMAL_FONTSIZE + m_nHeight)
			m_Height = h + NORMAL_FONTSIZE + m_nHeight + PREFIX_SIZE + 12;
				
		int nLeft = RESOLUTION_WIDTH/2 - m_Width/2;
		int nTop = RESOLUTION_HEIGHT/2 - m_Height/2;
		SetPosition(nLeft < 0 ? 0 : nLeft, nTop < 0 ? 0 : nTop);
	}
}

void KUiChatItem::PaintWindow()
{
	if (g_pRepresentShell)
	{
		KRUShadow	Shadow;
		KOutputTextParam	Param;
		Param.BorderColor = 0xff000000;
		Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

		Shadow.Color.Color_dw = m_uBoderColor;
		Shadow.oPosition.nX = m_nAbsoluteLeft;
		
		Shadow.oPosition.nY = m_nAbsoluteTop;
		Shadow.oEndPos.nX = m_nAbsoluteLeft + m_Width;
		Shadow.oEndPos.nY = m_nAbsoluteTop + m_Height;
		Shadow.oEndPos.nZ = Shadow.oPosition.nZ = 0;
		int m_nWndWidth = Shadow.oEndPos.nX - Shadow.oPosition.nX;
		if (m_nWndWidth % m_nWidth != 0)
		{
			m_Width += (m_nWidth - (m_nWndWidth % m_nWidth));	
			Shadow.oEndPos.nX += (m_nWidth - (m_nWndWidth % m_nWidth));	
		}	
		Param.nSkipLine = 0;
		Param.nNumLine = 1;
		Param.Color = 0xffffffff;
		Param.nY = Shadow.oPosition.nY + m_nHeight;
		
		if (m_pItemLen)
		{
			g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);		
			int w;
			m_ItemBox.GetSize(&w, NULL);
			int nLineLen;
			while(true)
			{
				if (TGetEncodedTextLineCount(m_pItemDesc, m_pItemLen , 0, nLineLen, NORMAL_FONTSIZE, Param.nSkipLine, 1) == 0)
					break;
				Param.nX = 10 + (m_nAbsoluteLeft + (m_Width - m_nWidth/2 - w) / 2 - (nLineLen * NORMAL_FONTSIZE) / 4 + w);
				g_pRepresentShell->OutputRichText(NORMAL_FONTSIZE, &Param, m_pItemDesc, m_pItemLen , 0);
				Param.nSkipLine ++;
				Param.nY += NORMAL_FONTSIZE + 1;
			};
			m_Image.oPosition.nX = Shadow.oPosition.nX;
			while(m_Image.oPosition.nX < Shadow.oPosition.nX + m_nWndWidth)
			{
				m_Image.oPosition.nY = Shadow.oPosition.nY;
				g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
				m_Image.oPosition.nY = Shadow.oEndPos.nY - m_nHeight;
				g_pRepresentShell->DrawPrimitives(1, &m_Image, RU_T_IMAGE, true);
				m_Image.oPosition.nX += m_nWidth;
			};
			KRURect	rect;
			rect.Color.Color_dw = 0xff00ffff;
			m_ItemBox.GetAbsolutePos(&rect.oPosition.nX, &rect.oPosition.nY);
			rect.oPosition.nX += 1;
			rect.oPosition.nY += 1;
			rect.oEndPos.nZ = rect.oPosition.nZ = 0;
			m_ItemBox.GetSize(&rect.oEndPos.nX, &rect.oEndPos.nY);
			rect.oEndPos.nX += rect.oPosition.nX - 3;
			rect.oEndPos.nY += rect.oPosition.nY - 3;
			g_pRepresentShell->DrawPrimitives(1, &rect, RU_T_RECT, true);
		}
	}
}