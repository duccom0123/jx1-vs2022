/*****************************************************************************************
//	Hop thoai Chon chan dung nguoi choi
//	Author	:   Doan Van Kiem
//	CreateTime:	2013/10/01
------------------------------------------------------------------------------------------
*****************************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiShell.h"
#include "../../../core/src/coreshell.h"
#include "UiInit.h"
#include "UiLoginBg.h"
#include "../UiSoundSetting.h"
#include <crtdbg.h>
#include "UiChooseFace.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "UiSysMsgCentre.h"
#include "../../../engine/src/KDebug.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*	g_pCoreShell;

#define SCHEME_INI_SELECT_FACE		"UiChooseFace.ini"

KUiChooseFace* KUiChooseFace::m_pSelf = NULL;

KUiChooseFace* KUiChooseFace::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}
//--------------------------------------------------------------------------
//	Mo hop thoai, tra ve con tro hop thoai
//--------------------------------------------------------------------------
KUiChooseFace* KUiChooseFace::OpenWindow(int uParam)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiChooseFace;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->Show();
		m_pSelf->ShowWindow(uParam);
		m_pSelf->nCheck = 0;
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
// Dong hop thoai 
//--------------------------------------------------------------------------
void KUiChooseFace::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ShowCursor(true);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}	
}

// -------------------------------------------------------------------------
// Khoi tao hop thoai
// -------------------------------------------------------------------------
void KUiChooseFace::Initialize()
{
	AddChild(&m_OneFemaleBtn);
	AddChild(&m_TwoFemaleBtn);
	AddChild(&m_ThreeFemaleBtn);
	AddChild(&m_FourFemaleBtn);
	AddChild(&m_FiveFemaleBtn);
	AddChild(&m_SixFemaleBtn);	
	AddChild(&m_SevenFemaleBtn);	
	AddChild(&m_EightFemaleBtn);	
	AddChild(&m_NineFemaleBtn);	
	AddChild(&m_TenFemaleBtn);	
	AddChild(&m_ElevenFemaleBtn);	
	AddChild(&m_TwelveFemaleBtn);	
	AddChild(&m_OneMaleBtn);
	AddChild(&m_TwoMaleBtn);
	AddChild(&m_ThreeMaleBtn);
	AddChild(&m_FourMaleBtn);
	AddChild(&m_FiveMaleBtn);
	AddChild(&m_SixMaleBtn);	
	AddChild(&m_SevenMaleBtn);	
	AddChild(&m_EightMaleBtn);	
	AddChild(&m_NineMaleBtn);	
	AddChild(&m_TenMaleBtn);	
	AddChild(&m_ElevenMaleBtn);	
	AddChild(&m_TwelveMaleBtn);	
	AddChild(&m_ThirteenMaleBtn);	
	AddChild(&m_FourteenMaleBtn);	
	AddChild(&m_FifteenMaleBtn);	
	AddChild(&m_CloseBtn);	
	AddChild(&m_OkBtn);
    AddChild(&m_Scroll);
	AddChild(&m_List);
	m_List.SetScrollbar(&m_Scroll);	
	char schemePath[256];
	g_UiBase.GetCurSchemePath(schemePath, 256);
	LoadScheme(schemePath);
	Wnd_AddWindow(this, WL_TOPMOST);
}
// -------------------------------------------------------------------------
// Tao layout hop thoai
// -------------------------------------------------------------------------
void KUiChooseFace::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SELECT_FACE);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_CloseBtn.Init(&Ini, "CloseBtn");
		m_OkBtn.Init(&Ini, "OkBtn");
		m_Scroll.Init(&Ini, "Scroll");
    	m_List	.Init(&Ini, "List");		
		if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0)){	
		m_OneMaleBtn	.Init(&Ini, "OneMaleBtn");
		m_TwoMaleBtn.Init(&Ini, "TwoMaleBtn");
		m_ThreeMaleBtn.Init(&Ini, "ThreeMaleBtn");	
		m_FourMaleBtn.Init(&Ini, "FourMaleBtn");
		m_FiveMaleBtn.Init(&Ini, "FiveMaleBtn");
		m_SixMaleBtn.Init(&Ini, "SixMaleBtn");
		m_SevenMaleBtn.Init(&Ini, "SevenMaleBtn");
		m_EightMaleBtn.Init(&Ini, "EightMaleBtn");
		m_NineMaleBtn.Init(&Ini, "NineMaleBtn");
		m_TenMaleBtn.Init(&Ini, "TenMaleBtn");
		m_ElevenMaleBtn.Init(&Ini, "ElevenMaleBtn");
		m_TwelveMaleBtn.Init(&Ini, "TwelveMaleBtn");
		m_ThirteenMaleBtn.Init(&Ini, "ThirteenMaleBtn");
		m_FourteenMaleBtn.Init(&Ini, "FourteenMaleBtn");
		m_FifteenMaleBtn.Init(&Ini, "FifteenMaleBtn");		
		}
		else
		{

		m_OneFemaleBtn	.Init(&Ini, "OneFemaleBtn");
		m_TwoFemaleBtn.Init(&Ini, "TwoFemaleBtn");
		m_ThreeFemaleBtn.Init(&Ini, "ThreeFemaleBtn");	
		m_FourFemaleBtn.Init(&Ini, "FourFemaleBtn");
		m_FiveFemaleBtn.Init(&Ini, "FiveFemaleBtn");
		m_SixFemaleBtn.Init(&Ini, "SixFemaleBtn");
		m_SevenFemaleBtn.Init(&Ini, "SevenFemaleBtn");
		m_EightFemaleBtn.Init(&Ini, "EightFemaleBtn");
		m_NineFemaleBtn.Init(&Ini, "NineFemaleBtn");
		m_TenFemaleBtn.Init(&Ini, "TenFemaleBtn");
		m_ElevenFemaleBtn.Init(&Ini, "ElevenFemaleBtn");
		m_TwelveFemaleBtn.Init(&Ini, "TwelveFemaleBtn");		
		}
		Ini.GetString("Main", "StringQuest", "", m_szLoginBg, sizeof(m_szLoginBg));
	}
}

void KUiChooseFace::ShowWindow(int nNum)
{
	switch(nNum)
	{
		case 1:
			m_pSelf->m_OneFemaleBtn.Show();
			m_pSelf->m_TwoFemaleBtn.Show();
			m_pSelf->m_ThreeFemaleBtn.Show();
			m_pSelf->m_FourFemaleBtn.Show();
			m_pSelf->m_FiveFemaleBtn.Show();
			m_pSelf->m_SixFemaleBtn.Show();
			m_pSelf->m_SevenFemaleBtn.Show();
			m_pSelf->m_EightFemaleBtn.Show();
			m_pSelf->m_NineFemaleBtn.Show();
			m_pSelf->m_TenFemaleBtn.Show();
			m_pSelf->m_ElevenFemaleBtn.Show();
			m_pSelf->m_TwelveFemaleBtn.Show();
			m_pSelf->m_OneMaleBtn.Hide();
			m_pSelf->m_TwoMaleBtn.Hide();
			m_pSelf->m_ThreeMaleBtn.Hide();
			m_pSelf->m_FourMaleBtn.Hide();
			m_pSelf->m_FiveMaleBtn.Hide();
			m_pSelf->m_SixMaleBtn.Hide();
			m_pSelf->m_SevenMaleBtn.Hide();
			m_pSelf->m_EightMaleBtn.Hide();
			m_pSelf->m_NineMaleBtn.Hide();
			m_pSelf->m_TenMaleBtn.Hide();
			m_pSelf->m_ElevenMaleBtn.Hide();
			m_pSelf->m_TwelveMaleBtn.Hide();
			m_pSelf->m_ThirteenMaleBtn.Hide();
			m_pSelf->m_FourteenMaleBtn.Hide();
			m_pSelf->m_FifteenMaleBtn.Hide();
			break;
		case 2:
			m_pSelf->m_OneFemaleBtn.Hide();
			m_pSelf->m_TwoFemaleBtn.Hide();
			m_pSelf->m_ThreeFemaleBtn.Hide();
			m_pSelf->m_FourFemaleBtn.Hide();
			m_pSelf->m_FiveFemaleBtn.Hide();
			m_pSelf->m_SixFemaleBtn.Hide();
			m_pSelf->m_SevenFemaleBtn.Hide();
			m_pSelf->m_EightFemaleBtn.Hide();
			m_pSelf->m_NineFemaleBtn.Hide();
			m_pSelf->m_TenFemaleBtn.Hide();
			m_pSelf->m_ElevenFemaleBtn.Hide();
			m_pSelf->m_TwelveFemaleBtn.Hide();
			m_pSelf->m_OneMaleBtn.Show();
			m_pSelf->m_TwoMaleBtn.Show();
			m_pSelf->m_ThreeMaleBtn.Show();
			m_pSelf->m_FourMaleBtn.Show();
			m_pSelf->m_FiveMaleBtn.Show();
			m_pSelf->m_SixMaleBtn.Show();
			m_pSelf->m_SevenMaleBtn.Show();
			m_pSelf->m_EightMaleBtn.Show();
			m_pSelf->m_NineMaleBtn.Show();
			m_pSelf->m_TenMaleBtn.Show();
			m_pSelf->m_ElevenMaleBtn.Show();
			m_pSelf->m_TwelveMaleBtn.Show();
			m_pSelf->m_ThirteenMaleBtn.Show();
			m_pSelf->m_FourteenMaleBtn.Show();
			m_pSelf->m_FifteenMaleBtn.Show();
			break;
	}
}

int KUiChooseFace::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch(uMsg)
	{
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_Scroll)
			m_List.SetTopItemIndex(nParam);
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow();
		else if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnOK();
		else if (uParam == (unsigned int)(KWndWindow*)&m_OneFemaleBtn)
		{
			nCheck = 1;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TwoFemaleBtn)
		{
			nCheck = 2;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ThreeFemaleBtn)
		{
			nCheck = 3;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FourFemaleBtn)
		{
			nCheck = 4;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FiveFemaleBtn)
		{
			nCheck = 5;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SixFemaleBtn)
		{
			nCheck = 6;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SevenFemaleBtn)
		{
			nCheck = 7;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_EightFemaleBtn)
		{
			nCheck = 8;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_NineFemaleBtn)
		{
			nCheck = 9;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TenFemaleBtn)
		{
			nCheck = 10;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ElevenFemaleBtn)
		{
			nCheck = 11;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TwelveFemaleBtn)
		{
			nCheck = 12;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OneMaleBtn)
		{
			nCheck = 13;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TwoMaleBtn)
		{
			nCheck = 14;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ThreeMaleBtn)
		{
			nCheck = 15;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FourMaleBtn)
		{
			nCheck = 16;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FiveMaleBtn)
		{
			nCheck = 17;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SixMaleBtn)
		{
			nCheck = 18;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SevenMaleBtn)
		{
			nCheck = 19;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_EightMaleBtn)
		{
			nCheck = 20;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_NineMaleBtn)
		{
			nCheck = 21;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TenMaleBtn)
		{
			nCheck = 22;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ElevenMaleBtn)
		{
			nCheck = 23;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TwelveMaleBtn)
		{
			nCheck = 24;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ThirteenMaleBtn)
		{
			nCheck = 25;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FourteenMaleBtn)
		{
			nCheck = 26;
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FifteenMaleBtn)
		{
			nCheck = 27;
		}
		break;
	default:
		nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiChooseFace::OnOK()
{
	if (nCheck <= 0)
	{
		KSystemMessage	Msg;
		Msg.byConfirmType = SMCT_NONE;
		Msg.byParamSize = 0;
		Msg.byPriority = 0;
		Msg.eType = SMT_NORMAL;
		Msg.uReservedForUi = 0;
		strcpy(Msg.szMessage, "B¹n ch­a chän ch©n dung!");
		KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
	}
	else
	{
		g_pCoreShell->OperationRequest(GOI_CP_SET_IMAGE_PLAYER, nCheck, 0);
		CloseWindow();
	}
}

void KUiChooseFace::PaintWindow()
{
	KWndPageSet::PaintWindow();
	if (g_pRepresentShell)
	{
		KRUShadow	Shadow;
		Shadow.Color.Color_dw = 0x20000000;
		Shadow.oPosition.nX = m_nAbsoluteLeft;
		Shadow.oPosition.nY = m_nAbsoluteTop;
		Shadow.oEndPos.nX = m_nAbsoluteLeft + m_Width + 10;
		Shadow.oEndPos.nY = m_nAbsoluteTop + m_Height - 15;
		Shadow.oEndPos.nZ = Shadow.oPosition.nZ = 0;
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
		if (nCheck > 0)
		{
			if (nCheck == 1 || nCheck == 13)
			{
				KRURect	rect1;
				rect1.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect1.oPosition.nX = Shadow.oPosition.nX +15;
				rect1.oPosition.nY = Shadow.oPosition.nY +59;
				rect1.oEndPos.nZ = rect1.oPosition.nZ = 0;
				rect1.oEndPos.nX = Shadow.oEndPos.nX - 343;
				rect1.oEndPos.nY = rect1.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect1, RU_T_RECT, true);
			}
			else if (nCheck == 2 || nCheck == 14)
			{
				KRURect	rect2;
				rect2.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect2.oPosition.nX = Shadow.oPosition.nX +95;
				rect2.oPosition.nY = Shadow.oPosition.nY +59;
				rect2.oEndPos.nZ = rect2.oPosition.nZ = 0;
				rect2.oEndPos.nX = Shadow.oEndPos.nX - 263;
				rect2.oEndPos.nY = rect2.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect2, RU_T_RECT, true);
			}
			else if (nCheck == 3 || nCheck == 15)
			{
				KRURect	rect3;
				rect3.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect3.oPosition.nX = Shadow.oPosition.nX +175;
				rect3.oPosition.nY = Shadow.oPosition.nY +59;
				rect3.oEndPos.nZ = rect3.oPosition.nZ = 0;
				rect3.oEndPos.nX = Shadow.oEndPos.nX - 183;
				rect3.oEndPos.nY = rect3.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect3, RU_T_RECT, true);
			}
			else if (nCheck == 4 || nCheck == 16)
			{
				KRURect	rect4;
				rect4.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect4.oPosition.nX = Shadow.oPosition.nX +255;
				rect4.oPosition.nY = Shadow.oPosition.nY +59;
				rect4.oEndPos.nZ = rect4.oPosition.nZ = 0;
				rect4.oEndPos.nX = Shadow.oEndPos.nX - 103;
				rect4.oEndPos.nY = rect4.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect4, RU_T_RECT, true);
			}
			else if (nCheck == 5 || nCheck == 17)
			{
				KRURect	rect5;
				rect5.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect5.oPosition.nX = Shadow.oPosition.nX +335;
				rect5.oPosition.nY = Shadow.oPosition.nY +59;
				rect5.oEndPos.nZ = rect5.oPosition.nZ = 0;
				rect5.oEndPos.nX = Shadow.oEndPos.nX - 23;
				rect5.oEndPos.nY = rect5.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect5, RU_T_RECT, true);
			}
			else if (nCheck == 6 || nCheck == 18)
			{
				KRURect	rect6;
				rect6.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect6.oPosition.nX = Shadow.oPosition.nX +15;
				rect6.oPosition.nY = Shadow.oPosition.nY +152;
				rect6.oEndPos.nZ = rect6.oPosition.nZ = 0;
				rect6.oEndPos.nX = Shadow.oEndPos.nX - 343;
				rect6.oEndPos.nY = rect6.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect6, RU_T_RECT, true);
			}
			else if (nCheck == 7 || nCheck == 19)
			{
				KRURect	rect7;
				rect7.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect7.oPosition.nX = Shadow.oPosition.nX +95;
				rect7.oPosition.nY = Shadow.oPosition.nY +152;
				rect7.oEndPos.nZ = rect7.oPosition.nZ = 0;
				rect7.oEndPos.nX = Shadow.oEndPos.nX - 263;
				rect7.oEndPos.nY = rect7.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect7, RU_T_RECT, true);
			}
			else if (nCheck == 8 || nCheck == 20)
			{
				KRURect	rect8;
				rect8.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect8.oPosition.nX = Shadow.oPosition.nX +175;
				rect8.oPosition.nY = Shadow.oPosition.nY +152;
				rect8.oEndPos.nZ = rect8.oPosition.nZ = 0;
				rect8.oEndPos.nX = Shadow.oEndPos.nX - 183;
				rect8.oEndPos.nY = rect8.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect8, RU_T_RECT, true);
			}
			else if (nCheck == 9 || nCheck == 21)
			{
				KRURect	rect9;
				rect9.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect9.oPosition.nX = Shadow.oPosition.nX +255;
				rect9.oPosition.nY = Shadow.oPosition.nY +152;
				rect9.oEndPos.nZ = rect9.oPosition.nZ = 0;
				rect9.oEndPos.nX = Shadow.oEndPos.nX - 103;
				rect9.oEndPos.nY = rect9.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect9, RU_T_RECT, true);
			}
			else if (nCheck == 10 || nCheck == 22)
			{
				KRURect	rect10;
				rect10.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect10.oPosition.nX = Shadow.oPosition.nX +335;
				rect10.oPosition.nY = Shadow.oPosition.nY +152;
				rect10.oEndPos.nZ = rect10.oPosition.nZ = 0;
				rect10.oEndPos.nX = Shadow.oEndPos.nX - 23;
				rect10.oEndPos.nY = rect10.oPosition.nY + 71; 
				g_pRepresentShell->DrawPrimitives(1, &rect10, RU_T_RECT, true);
			}
			else if (nCheck == 11 || nCheck == 23)
			{
				KRURect	rect11;
				rect11.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect11.oPosition.nX = Shadow.oPosition.nX +15;
				rect11.oPosition.nY = Shadow.oPosition.nY +245;
				rect11.oEndPos.nZ = rect11.oPosition.nZ = 0;
				rect11.oEndPos.nX = Shadow.oEndPos.nX - 343;
				rect11.oEndPos.nY = rect11.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect11, RU_T_RECT, true);
			}
			else if (nCheck == 12 || nCheck == 24)
			{
				KRURect	rect12;
				rect12.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect12.oPosition.nX = Shadow.oPosition.nX +95;
				rect12.oPosition.nY = Shadow.oPosition.nY +245;
				rect12.oEndPos.nZ = rect12.oPosition.nZ = 0;
				rect12.oEndPos.nX = Shadow.oEndPos.nX - 263;
				rect12.oEndPos.nY = rect12.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect12, RU_T_RECT, true);
			}
			else if (nCheck == 25)
			{
				KRURect	rect13;
				rect13.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect13.oPosition.nX = Shadow.oPosition.nX +175;
				rect13.oPosition.nY = Shadow.oPosition.nY +245;
				rect13.oEndPos.nZ = rect13.oPosition.nZ = 0;
				rect13.oEndPos.nX = Shadow.oEndPos.nX - 183;
				rect13.oEndPos.nY = rect13.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect13, RU_T_RECT, true);
			}
			else if (nCheck == 26)
			{
				KRURect	rect14;
				rect14.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect14.oPosition.nX = Shadow.oPosition.nX +255;
				rect14.oPosition.nY = Shadow.oPosition.nY +245;
				rect14.oEndPos.nZ = rect14.oPosition.nZ = 0;
				rect14.oEndPos.nX = Shadow.oEndPos.nX - 103;
				rect14.oEndPos.nY = rect14.oPosition.nY + 71;
				g_pRepresentShell->DrawPrimitives(1, &rect14, RU_T_RECT, true);
			}
			else if (nCheck == 27)
			{
				KRURect	rect15;
				rect15.Color.Color_dw = 0xFF000000 | 242 << 16 | 255 << 8 | 242;
				rect15.oPosition.nX = Shadow.oPosition.nX +335;
				rect15.oPosition.nY = Shadow.oPosition.nY +245;
				rect15.oEndPos.nZ = rect15.oPosition.nZ = 0;
				rect15.oEndPos.nX = Shadow.oEndPos.nX - 23;
				rect15.oEndPos.nY = rect15.oPosition.nY + 71; 
				g_pRepresentShell->DrawPrimitives(1, &rect15, RU_T_RECT, true);
			}
		}
	}
}