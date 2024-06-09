/*****************************************************************************************
//	ΩÁ√Ê--¥¢ŒÔœ‰ΩÁ√Ê
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiExpandItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "UiPlayerShop.h"
#include "UiSysMsgCentre.h"
#include "UiTradeConfirmWnd.h"
#include "UiSuperShop.h"
#include "UiBreakItem.h"
#include <crtdbg.h>

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI	"UiExpandItem.ini"
extern iRepresentShell*	g_pRepresentShell;
KUiExpandItem* KUiExpandItem::m_pSelf = NULL;


//--------------------------------------------------------------------------
//	π¶ƒ‹£∫»Áπ˚¥∞ø⁄’˝±ªœ‘ æ£¨‘Ú∑µªÿ µ¿˝÷∏’Î
//--------------------------------------------------------------------------
KUiExpandItem* KUiExpandItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫¥Úø™¥∞ø⁄£¨∑µªÿŒ®“ªµƒ“ª∏ˆ¿‡∂‘œÛ µ¿˝
//--------------------------------------------------------------------------
KUiExpandItem* KUiExpandItem::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiExpandItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫πÿ±’¥∞ø⁄
//--------------------------------------------------------------------------
void KUiExpandItem::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		m_pSelf = NULL;
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ≥ı ºªØ
// -------------------------------------------------------------------------
void KUiExpandItem::Initialize()
{
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBox);
	

	

	m_ItemBox.SetContainerId((int)UOC_ITEM_TAKE_WITH_EX);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

void KUiExpandItem::OnNpcTradeMode(bool bTrue)
{
	if (m_pSelf)
		m_pSelf->m_ItemBox.EnablePickPut(!bTrue);
}

//--------------------------------------------------------------------------
//	π¶ƒ‹£∫ππ‘Ï∫Ø ˝
//--------------------------------------------------------------------------
void KUiExpandItem::UpdateData()
{
	m_ItemBox.Clear();
	
	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_EXPANDITEM, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_EXPANDITEM, (unsigned int)pObjs, nCount);//µ•œﬂ≥Ã÷¥––£¨nCount÷µ≤ª±‰
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], true);
		free(pObjs);
		pObjs = NULL;
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ŒÔ∆∑±‰ªØ∏¸–¬
// -------------------------------------------------------------------------
void KUiExpandItem::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemBox.AddObject(&Obj, 1);
			else
				m_ItemBox.RemoveObject(&Obj);
		}
	}
	else
		UpdateData();
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ‘ÿ»ÎΩÁ√Ê∑Ω∞∏
// -------------------------------------------------------------------------
void KUiExpandItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
		m_pSelf->m_ItemBox.EnableTracePutPos(true);
	}
}

// -------------------------------------------------------------------------
// π¶ƒ‹	: ¥∞ø⁄∫Ø ˝
// -------------------------------------------------------------------------
int KUiExpandItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_LEFT_CLICK_ITEM:
		if (g_UiBase.GetStatus() == UIS_S_TRADE_SALE)
			OnClickItem((KUiDraggedObject*)uParam, true);
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC)
			OnClickItem((KUiDraggedObject*)uParam, false);
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_REPAIR)
			OnRepairItem((KUiDraggedObject*)uParam);
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		if (GetKeyState(VK_SHIFT) & 0x8000 && !g_UiBase.GetStatus())
			OnBreakItem((KUiDraggedObject*)uParam, true);
		else
			OnClickItem((KUiDraggedObject*)uParam, true);
		break;
	case WND_N_ITEM_PICKDROP:
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
			break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow();
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}


void KUiExpandItem::PaintWindow() 
{
	KWndShowAnimate::PaintWindow();
	if (g_pRepresentShell && g_pCoreShell->GetGameData(GDI_EQUIPEX_TIME, NULL, NULL) <= 0)
	{
		KRUShadow	bg;
		m_ItemBox.GetAbsolutePos(&bg.oPosition.nX, &bg.oPosition.nY);
		bg.oPosition.nX;
		bg.oPosition.nY;
		bg.Color.Color_dw = 0x191079a5;
		m_ItemBox.GetSize(&bg.oEndPos.nX, &bg.oEndPos.nY);
		bg.oEndPos.nX += bg.oPosition.nX;
		bg.oEndPos.nY += bg.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &bg, RU_T_SHADOW, true);	
	}
}


void KUiExpandItem::OnClickItem(KUiDraggedObject* pItem, bool bDoImmed)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_TAKE_WITH_EX;

	if (KUiPlayerShop::GetIfVisible())
		return;
	if (bDoImmed == false)
	{
		KUiItemBuySelInfo	Price = { 0 };
		if (KUiSuperShop::GetIfVisible() == NULL && 
			g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
			(unsigned int)(&Obj), (int)(&Price)))
		{
			KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_SALE);
		}	
	}
	else
	{
		UISYS_STATUS eStatus = g_UiBase.GetStatus();
		if (eStatus == UIS_S_TRADE_SALE)
		{
			if (KUiSuperShop::GetIfVisible() == NULL)
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
					(unsigned int)(&Obj), 1);				
		}
		else if (eStatus == UIS_S_TRADE_NPC)
		{
			if (KUiSuperShop::GetIfVisible() == NULL)
			{
				if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
				{
					g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
						(unsigned int)(&Obj), 1);			
				}
				else
				{
					KSystemMessage	Msg;
					Msg.byConfirmType = SMCT_NONE;
					Msg.byParamSize = 0;
					Msg.byPriority = 0;
					Msg.eType = SMT_NORMAL;
					Msg.uReservedForUi = 0;
					strcpy(Msg.szMessage, "Nh n gi˜ ph›m Shift ÆÂng thÍi nh n chuÈt ph∂i lÀp t¯c b∏n Æ≠Óc vÀt ph»m!");
					KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
				}
			}
		}
		else if (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))
		{
			g_pCoreShell->OperationRequest(GOI_USE_ITEM,
				(unsigned int)(&Obj), UOC_ITEM_TAKE_WITH_EX);
		}
	}
}

void KUiExpandItem::OnRepairItem(KUiDraggedObject* pItem)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_TAKE_WITH_EX;

	KUiItemBuySelInfo	Price = { 0 };
	if (g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,
		(unsigned int)(&Obj), (int)(&Price)))
	{
		KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_REPAIR);
	}
}

void KUiExpandItem::OnBreakItem(KUiDraggedObject* pItem, bool bDoImmed)
{
	if (pItem == NULL || g_pCoreShell == NULL || KUiBreakItem::GetIfVisible())
		return;
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_TAKE_WITH_EX;
	KUiItemBuySelInfo	Price = { 0 };
	
	if (bDoImmed)
		g_pCoreShell->BreakItem((unsigned int)(&Obj), 0, FALSE);
	else
	{
		int nNum = g_pCoreShell->GetStackNum(pItem->uId);
		if (nNum > 1)
		{
			if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
				(unsigned int)(&Obj), (int)(&Price)))
			KUiBreakItem::OpenWindow(&Obj, &Price, nNum);
		}
	}
}

void KUiExpandItem::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;

	UISYS_STATUS eStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		_ASSERT(pPickPos->pWnd);		
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(
			Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_ITEM_TAKE_WITH_EX;

		if (eStatus == UIS_S_TRADE_SALE)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
				(unsigned int)(&Pick), 1);
			return;			
		}
		else if (eStatus == UIS_S_TRADE_REPAIR)
		{
			if(g_pCoreShell->IsDamage(Obj.uId))
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,
					(unsigned int)(&Pick), 0);
			return;
		}
		else if (eStatus == UIS_S_TRADE_BUY)
		{
			return;
		}
		else if (eStatus == UIS_S_LOCK_ITEM)
		{
			g_pCoreShell->OperationRequest(GOI_LOCKITEM, (unsigned int)(&Pick), 1);
			return;
		}
		else if (eStatus == UIS_S_UNLOCK_ITEM)
		{
			g_pCoreShell->OperationRequest(GOI_LOCKITEM, (unsigned int)(&Pick), 0);
			return;
		}
		else if (GetKeyState(VK_SHIFT) & 0x8000 && !g_UiBase.GetStatus())
		{
			if (!KUiBreakItem::GetIfVisible())
				OnBreakItem(&Obj, false);
			return;
		}
	}

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_ITEM_TAKE_WITH_EX;	
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}