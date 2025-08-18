// -------------------------------------------------------------------------
//	ÎÄ¼þÃû		£º	UiItem.cpp
//	Author		£º	ÂÀ¹ð»ª, Wooy(Wu yue)
//	´´½¨Ê±¼ä	£º	2002-9-16 11:26:43
//	¹¦ÄÜÃèÊö	£º	
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiTrade.h"
#include "UiItem.h"
#include "UiGetMoney.h"
#include "UiGetString.h"
#include "UiStoreBox.h"
#include "UiTradeConfirmWnd.h"
#include "UiSysMsgCentre.h"
#include "UiShop.h"
#include "UiBreakItem.h"
#include "UiPlayerShop.h"
#include "UiStoreBox.h"
#include "UiSetPrice.h"
#include "UiStatus.h"
#include "UiStoreBox.h"
#include "UiExpandItem.h"
#include "UiSuperShop.h"
#include "UiGive.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../Engine/src/KDebug.h"
#include "../UiBase.h"
#include "../UiShell.h"
#include "../ShortcutKey.h"
#include <crtdbg.h>
#include "../UiSoundSetting.h"

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI	"UiItem.ini"

KUiItem* KUiItem::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UIITEM_WAIT_GETMONEY,
	UIITEM_WAIT_GETADV,
};


//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
//--------------------------------------------------------------------------
KUiItem* KUiItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiItem* KUiItem::OpenWindow(bool bFlag)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf && bFlag)
	{
		if (g_pCoreShell && g_pCoreShell->GetGameData(GDI_EQUIPEX_TIME, NULL, NULL) > 0)
		{
			if(KUiExpandItem::GetIfVisible() == NULL)
				KUiExpandItem::OpenWindow();
		}
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiItem::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if(KUiExpandItem::GetIfVisible())
			KUiExpandItem::CloseWindow();
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ³õÊ¼»¯
// -------------------------------------------------------------------------
void KUiItem::Initialize()
{
	AddChild(&m_TitleIcon);
	AddChild(&m_Money);
	AddChild(&m_Gold);
	AddChild(&m_MoneyIcon);
	AddChild(&m_GoldIcon);
	AddChild(&m_GetMoneyBtn);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBox);
	AddChild(&m_OpenStatusPadBtn);
	AddChild(&m_MakeAdvBtn);
	AddChild(&m_MarkPriceBtn);
	AddChild(&m_MakeStallBtn);
	m_byMark = 0;
	m_szAdvStr[0] = 0;

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_ItemBox.SetContainerId((int)UOC_ITEM_TAKE_WITH);
	m_nMoney = 0;
	Wnd_AddWindow(this);
}

//»î¶¯º¯Êý
void KUiItem::Breathe()
{
	if (!g_pCoreShell->GetTradeState())
	{
		m_MakeStallBtn.CheckButton(FALSE);
		if (m_byMark == 1)
			m_byMark = 0;
	}
	else if (m_byMark == 0 && g_pCoreShell->GetTradeState())
	{
		if (g_UiBase.GetStatus() != UIS_S_IDLE)
			g_UiBase.SetStatus(UIS_S_IDLE);
		m_MakeStallBtn.CheckButton(TRUE);
		m_byMark = 1;
	}
	m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	m_Money.Set3IntText(m_nMoney);	
	m_Gold.SetExtPointText(g_pCoreShell->GetExtPoint());
}

void KUiItem::OnNpcTradeMode(bool bTrue)
{
	KUiExpandItem::OnNpcTradeMode(bTrue);
	if (m_pSelf)
		m_pSelf->m_ItemBox.EnablePickPut(!bTrue);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹¹Ôìº¯Êý
//--------------------------------------------------------------------------
void KUiItem::UpdateData()
{
	m_ItemBox.Clear();

	m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	m_Money.Set3IntText(m_nMoney);

	KUiObjAtRegion* pObjs = NULL;
	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);//µ¥Ïß³ÌÖ´ÐÐ£¬nCountÖµ²»±ä
		for (int i = 0; i < nCount; i++)
		{
			KUiDraggedObject no;
			no.uGenre = pObjs[i].Obj.uGenre;
			no.uId = pObjs[i].Obj.uId;
			no.DataX = pObjs[i].Region.h;
			no.DataY = pObjs[i].Region.v;
			no.DataW = pObjs[i].Region.Width;
			no.DataH = pObjs[i].Region.Height;
			m_ItemBox.AddObject(&no, 1);
		}
		free(pObjs);
		pObjs = NULL;
	}
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ÎïÆ·±ä»¯¸üÐÂ
// -------------------------------------------------------------------------
void KUiItem::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
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

		UiSoundPlay(UI_SI_PICKPUT_ITEM);
	}
	else
		UpdateData();
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ÔØÈë½çÃæ·½°¸
// -------------------------------------------------------------------------
void KUiItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_TitleIcon.Init(&Ini, "TitleIcon");
		m_pSelf->m_Money.Init(&Ini, "Money");
		m_pSelf->m_Gold.Init(&Ini, "Gold");
		m_pSelf->m_MoneyIcon.Init(&Ini, "MoneyIcon");
		m_pSelf->m_GoldIcon.Init(&Ini, "GoldIcon");
		m_pSelf->m_GetMoneyBtn.Init(&Ini, "GetMoneyBtn");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
		m_pSelf->m_OpenStatusPadBtn.Init(&Ini, "OpenStatus");
		m_pSelf->m_ItemBox.EnableTracePutPos(true);
		m_pSelf->m_MakeAdvBtn.Init(&Ini,"MakeAdvBtn");
		m_pSelf->m_MarkPriceBtn.Init(&Ini,"MarkPriceBtn");
		m_pSelf->m_MakeStallBtn.Init(&Ini,"MakeStallBtn");
	}
}

void KUiItem::OnClickItem(KUiDraggedObject* pItem, bool bDoImmed)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

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
			if(KUiSuperShop::GetIfVisible() == NULL)
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
					(unsigned int)(&Obj), 1);				
		}
		else if (eStatus == UIS_S_TRADE_NPC)
		{
			if(KUiSuperShop::GetIfVisible() == NULL)
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
					strcpy(Msg.szMessage, "NhÊn gi÷ phÝm Shift ®ång thêi nhÊn chuét ph¶i lËp tøc b¸n ®­îc vËt phÈm!");
					KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
				}
			}
		}
		else if (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))
		{
			g_pCoreShell->OperationRequest(GOI_USE_ITEM,
				(unsigned int)(&Obj), UOC_ITEM_TAKE_WITH);
		}
	}
}

void KUiItem::OnRepairItem(KUiDraggedObject* pItem)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	KUiItemBuySelInfo	Price = { 0 };
	if (g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,
		(unsigned int)(&Obj), (int)(&Price)))
	{
		KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_REPAIR);
	}
}

// -------------------------------------------------------------------------
// ¹¦ÄÜ	: ´°¿Úº¯Êý
// -------------------------------------------------------------------------
int KUiItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
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
		else if (g_UiBase.GetStatus() == UIS_S_TRADE_SETPRICE)
			OnSetPrice((KUiDraggedObject*)uParam);
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
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn && g_UiBase.GetStatus() != UIS_S_TRADE_SETPRICE)
			CloseWindow(false);
		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenStatusPadBtn)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_STATUS);
		else if (uParam == (unsigned int)(KWndWindow*)&m_GetMoneyBtn)
		{
			if (KUiStoreBox::GetIfVisible())
				KUiGetMoney::OpenWindow(0, m_nMoney, this, UIITEM_WAIT_GETMONEY, &m_Money);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MakeAdvBtn && m_MakeStallBtn.IsButtonChecked() == 0 && !g_UiBase.GetStatus())
				KUiGetString::OpenWindow(GSA_ADV, "", m_szAdvStr, this, UIITEM_WAIT_GETADV, 0, 4, 16);
		else if (uParam == (unsigned int)(KWndWindow*)&m_MarkPriceBtn && m_MakeStallBtn.IsButtonChecked() == 0)
		{
			if (g_UiBase.GetStatus() == UIS_S_IDLE)
			{
				g_UiBase.SetStatus(UIS_S_TRADE_SETPRICE);
				KUiItem::OnNpcTradeMode(true);
			}
			else if (g_UiBase.GetStatus() == UIS_S_TRADE_SETPRICE)
			{
				g_UiBase.SetStatus(UIS_S_IDLE);
				KUiItem::OnNpcTradeMode(false);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MakeStallBtn && !g_UiBase.GetStatus())
		{
			if (g_pCoreShell)
			{
				if (m_szAdvStr[0])
				{
					if (!g_pCoreShell->OperationRequest(GOI_PLAYER_DOTRADE, (unsigned int)(&m_szAdvStr), 0))
						m_MakeStallBtn.CheckButton(false);
				}
				else
					KUiGetString::OpenWindow(GSA_ADV, "", m_szAdvStr, this, UIITEM_WAIT_GETADV, 0, 4, 16);
			}
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		if (uParam == UIITEM_WAIT_GETMONEY)
		{
			OnGetMoney(nParam);
		}
		else if(uParam == UIITEM_WAIT_GETADV)
		{
			strcpy(m_szAdvStr, (char*)nParam);
			if (m_szAdvStr[0])
			{
				if (!g_pCoreShell->OperationRequest(GOI_PLAYER_DOTRADE, (unsigned int)(&m_szAdvStr), 0))
					m_MakeStallBtn.CheckButton(false);
			}
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiItem::OnGetMoney(int nMoney)
{	
	if (nMoney > 0 && KUiStoreBox::GetIfVisible())
	{
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,
			true, nMoney);
	}
}

void KUiItem::OnBreakItem(KUiDraggedObject* pItem, bool bDoImmed)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;
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

void KUiItem::OnSetPrice(KUiDraggedObject* pItem)
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
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	KUiItemBuySelInfo	Price = { 0 };
	if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
		(unsigned int)(&Obj), (int)(&Price)))
	{
		g_DebugLog("xxx");
		KUiSetPrice::OpenWindow(g_pCoreShell->GetTradePrice(pItem->uId), &Obj, &Price);
	}	
}

void KUiItem::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
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
		Pick.eContainer = UOC_ITEM_TAKE_WITH;

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
		Drop.eContainer = UOC_ITEM_TAKE_WITH;	
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}