#include "KWin32.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "UiSuperShop.h"
#include "UiItem.h"
#include "../../../Core/Src/CoreObjGenreDef.h"
#include "../../../Core/Src/CoreShell.h"
#include "../../../Engine/Src/Text.h"
#include "../../../Engine/Src/KDebug.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "UiInformation.h"
#include "UiSysMsgCentre.h"
#include "UiTradeConfirmWnd.h"
#include "KTabFile.h"
#include <crtdbg.h>
#include "../../../Represent/iRepresent/iRepresentShell.h"


#include "../ShortcutKey.h"
#include "GameDataDef.h"
extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI							"UiSuperShop.ini"
#define SCHEME_SHOPPING_INI					"UiShopPing.ini"
#define UI_WAIT_CLEAR_SHOPPINGCART			1
#define MARKET_DISCOUNT_PIC					"\\spr\\Ui3\\ÂòÂô\\ÐÂÆæÕä¸ó½çÃæ\\%d_vn.spr"

KUiShoppingCart* KUiShoppingCart::m_pSelf = NULL;

KUiShoppingCart::KUiShoppingCart()
{
	m_nBuyValue = 1;
	m_nCostValue = 0;
	m_nOldValue = 0;
	m_nSaveValue = 0;
	m_nOwnValue = 0;
	m_PriceInfo.szItemName[0] = 0;
	m_PriceInfo.nOldPrice = 0;
	m_PriceInfo.nCurPrice = 0;
	m_PriceInfo.bNewArrival = FALSE;
	m_PriceInfo.nMoneyUnit = 0;	
	m_ItemInfo.Obj.uGenre = CGOG_NOTHING;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiShoppingCart* KUiShoppingCart::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiShoppingCart* KUiShoppingCart::OpenWindow(KUiObjAtContRegion* pObj,
			KUiItemBuySelInfo* pPriceInfo)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiShoppingCart;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if (m_pSelf->m_ItemInfo.Obj.uId != pObj->Obj.uId)
			m_pSelf->m_nBuyValue = 1;
		m_pSelf->m_ItemInfo = *pObj;
		m_pSelf->m_PriceInfo = *pPriceInfo;
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiShoppingCart::Breathe()
{
	m_GoodsInfo_CountEdit.SetIntText(m_nBuyValue);
	m_TotalCostValueText.SetMoneyUnitPrice(m_nOwnValue, m_PriceInfo.nMoneyUnit);
	m_GoodsInfo_TotalPrice.SetMoneyUnitPrice(m_nCostValue, m_PriceInfo.nMoneyUnit);
	m_TotalSaveValueText.SetMoneyUnitPrice(m_nCostValue * m_nBuyValue, m_PriceInfo.nMoneyUnit);
	m_OwnValueText.SetMoneyUnitPrice(m_nOwnValue - m_nCostValue * m_nBuyValue, m_PriceInfo.nMoneyUnit);
	
	if (m_nOwnValue >= m_nCostValue * m_nBuyValue && m_nBuyValue > 0)
		m_ConfirmBuy.Enable(true);
	else
		m_ConfirmBuy.Enable(false);
		
	m_OwnValueText.SetTextColor((m_nOwnValue - m_nCostValue * m_nBuyValue < 0)?0x008785:0xffd94f);
}

void KUiShoppingCart::AddCount()
{	
	int nNumber = m_GoodsInfo_CountEdit.GetIntNumber();
	nNumber ++;

	m_nBuyValue = nNumber;
	char	szBuff1[16], szBuff2[16];
	itoa(m_nBuyValue, szBuff1, 10);
	m_GoodsInfo_CountEdit.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_GoodsInfo_CountEdit.SetIntText(m_nBuyValue);
	Wnd_SetFocusWnd(&m_pSelf->m_GoodsInfo_CountEdit);
}

void KUiShoppingCart::DelCount()
{	
	int nNumber = m_GoodsInfo_CountEdit.GetIntNumber();
	nNumber --;
	
	if (nNumber <= 0)
		nNumber = 0;
	
	m_nBuyValue = nNumber;
	char	szBuff1[16], szBuff2[16];
	itoa(m_nBuyValue, szBuff1, 10);
	m_GoodsInfo_CountEdit.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_GoodsInfo_CountEdit.SetIntText(m_nBuyValue);
	Wnd_SetFocusWnd(&m_pSelf->m_GoodsInfo_CountEdit);
}

void KUiShoppingCart::OnCheckInput()
{	
	int nNumber = m_GoodsInfo_CountEdit.GetIntNumber();
	m_nBuyValue = nNumber;
	
	char	szBuff1[16], szBuff2[16];
	itoa(m_nBuyValue, szBuff1, 10);
	m_GoodsInfo_CountEdit.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_GoodsInfo_CountEdit.SetIntText(m_nBuyValue);
}

void KUiShoppingCart::Show()
{
	Wnd_SetFocusWnd(&m_pSelf->m_GoodsInfo_CountEdit);
	KWndShowAnimate::Show();
	Wnd_SetExclusive((KWndWindow*)this);
}

void KUiShoppingCart::UpdateData()
{
	if (g_pCoreShell)
	{
		m_GoodsInfo_ItemBox.Celar();

		m_GoodsInfo_ItemBox.HoldObject(m_ItemInfo.Obj.uGenre, m_ItemInfo.Obj.uId, m_ItemInfo.Region.Width, m_ItemInfo.Region.Height);
		m_GoodsInfo_GoodsName.SetText(m_PriceInfo.szItemName);
		
		m_nOwnValue = g_pCoreShell->GetOwnValue(m_PriceInfo.nMoneyUnit);
		
		m_nCostValue = m_PriceInfo.nCurPrice;
		m_nOldValue = m_PriceInfo.nOldPrice;
		if (m_nCostValue < m_nOldValue)
			m_nSaveValue = m_nOldValue - m_nCostValue;
		else
			m_nSaveValue = 0;
	}
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiShoppingCart::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}


void KUiShoppingCart::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_TotalCostTitleText);
	AddChild(&m_TotalSaveTitleText);
	AddChild(&m_OwnTitleText);
	AddChild(&m_TotalCostValueText);
	AddChild(&m_TotalSaveValueText);
	AddChild(&m_OwnValueText);
	AddChild(&m_GoodsInfo);
	AddChild(&m_GoodsInfo_ItemBox);
	m_GoodsInfo_ItemBox.SetContainerId((int)UOC_NPC_SHOP);
	AddChild(&m_GoodsInfo_GoodsName);
	AddChild(&m_GoodsInfo_TotalPrice);
	AddChild(&m_GoodsInfo_DelItem);
	AddChild(&m_GoodsInfo_AddCount);
	AddChild(&m_GoodsInfo_DelCount);
	AddChild(&m_GoodsInfo_CountEdit);
	AddChild(&m_ScrollBar);
	AddChild(&m_ConfirmBuy);
	AddChild(&m_PrePaidBtn);
	AddChild(&m_CloseBtn);
	Wnd_AddWindow(this); 
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}


void KUiShoppingCart::LoadScheme(const char* pScheme)
{	
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\" SCHEME_SHOPPING_INI, pScheme);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_Title.Init(&Ini, "Title");
		m_TotalCostTitleText.Init(&Ini, "TotalCostTitleText");
		m_TotalSaveTitleText.Init(&Ini, "TotalSaveTitleText");
		m_OwnTitleText.Init(&Ini, "OwnTitleText");
		m_TotalCostValueText.Init(&Ini, "TotalCostValueText");
		m_TotalSaveValueText.Init(&Ini, "TotalSaveValueText");
		m_OwnValueText.Init(&Ini, "OwnValueText");
		m_GoodsInfo.Init(&Ini, "GoodsInfo");
		m_GoodsInfo_ItemBox.Init(&Ini, "GoodsInfo_ItemBox");
		m_GoodsInfo_GoodsName.Init(&Ini, "GoodsInfo_GoodsName");
		m_GoodsInfo_TotalPrice.Init(&Ini, "GoodsInfo_TotalPrice");
		m_GoodsInfo_DelItem.Init(&Ini, "GoodsInfo_DelItem");
		m_GoodsInfo_AddCount.Init(&Ini, "GoodsInfo_AddCount");
		m_GoodsInfo_DelCount.Init(&Ini, "GoodsInfo_DelCount");
		m_GoodsInfo_CountEdit.Init(&Ini, "GoodsInfo_CountEdit");
		m_ScrollBar.Init(&Ini, "Scroll");
		m_ConfirmBuy.Init(&Ini, "ConfirmBuy");
		m_PrePaidBtn.Init(&Ini, "PrePaid");
		m_CloseBtn.Init(&Ini, "CloseBtn");
	}
}

void KUiShoppingCart::CartRelease()
{
	m_nBuyValue = 1;
	m_nCostValue = 0;
	m_nOldValue = 0;
	m_nSaveValue = 0;
	m_nOwnValue = 0;
	CloseWindow(true);
}

int	KUiShoppingCart::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK )
	{
		if ((KWndWindow*)uParam == (KWndWindow*)&m_CloseBtn)
			CloseWindow(true);
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_GoodsInfo_DelItem)
		{
			CartRelease();
			KUiSuperShop::PutItem(NULL);
		}			
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_GoodsInfo_AddCount)
			AddCount();
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_GoodsInfo_DelCount)
			DelCount();
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_ConfirmBuy)
		{
			KUiSuperShop::BuyItem(m_nBuyValue);
			CartRelease();
		}
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_PrePaidBtn)
		{
			KIniFile Ini;
			if (Ini.Load(GAME_SETTING_FILE_INI))
			{
				char szFilePath[MAX_PATH];
				Ini.GetString("URL", "PrePaid", "", szFilePath, sizeof(szFilePath));
				ShellExecute(NULL,"open",szFilePath,NULL,NULL,SW_SHOWNORMAL); 
			}
		}	
	}
	else if (uMsg == WND_N_SCORLLBAR_POS_CHANGED)
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_ScrollBar)
			return 0;
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if (uParam == VK_RETURN && m_ConfirmBuy.IsVisible())
		{
			if (m_nOwnValue >= m_nCostValue * m_nBuyValue && m_nBuyValue > 0)
			{
				KUiSuperShop::BuyItem(m_nBuyValue);
				CartRelease();
			}
		}			
		else if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC)
				Hide();
			else
				g_UiBase.SetStatus(UIS_S_TRADE_NPC);
		}
	}
	else if (uMsg == WND_N_EDIT_CHANGE)
		OnCheckInput();
	return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
}


////////////////////////////////////////////////////
void KWndSellItem::Initialize()
{
	AddChild(&m_MarketGoods);
	AddChild(&m_MarketGoods_DisCount);
	m_MarketGoods_DisCount.Hide();
	AddChild(&m_MarketGoods_imgNewArrival);
	m_MarketGoods_imgNewArrival.Hide();
	AddChild(&m_GoodsNameText);
	AddChild(&m_OriginalPriceText);
	AddChild(&m_OriginalPrice_NumberText);
	AddChild(&m_PriceText);
	AddChild(&m_Price_NumberText);
	AddChild(&m_ItemBox);
	AddChild(&m_MarketGoods_Buy);
	m_MarketGoods_Buy.Enable(false);
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KWndSellItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if ( Ini.Load(Buff))
	{
		KWndSellItem::Init(&Ini, "MarketGoods");
        m_MarketGoods.Init(&Ini, "MarketGoods");
		m_GoodsNameText.Init(&Ini,"MarketGoods_GoodsName");//
		m_MarketGoods_DisCount.Init(&Ini,"MarketGoods_DisCount");//
		m_MarketGoods_imgNewArrival.Init(&Ini,"MarketGoods_imgNewArrival");//
		m_OriginalPriceText.Init(&Ini,"MarketGoods_OriginalPrice" );
		m_OriginalPrice_NumberText.Init(&Ini,"MarketGoods_OriginalPrice_Number" );//
		m_PriceText.Init(&Ini,"MarketGoods_Price" );
		m_Price_NumberText.Init(&Ini,"MarketGoods_Price_Number" );//
        m_ItemBox.Init(&Ini,"MarketGoods_ItemBox");
		m_MarketGoods_Buy.Init(&Ini,"MarketGoods_Buy");
	}
}
void KWndSellItem::Clear()
{
	m_MarketGoods_DisCount.Hide();
	m_MarketGoods_imgNewArrival.Hide();
	m_GoodsNameText.Clear();//ÎïÆ·Ãû
	m_OriginalPriceText.Clear();//Ô­¼Û
	m_OriginalPrice_NumberText.Clear();//Ô­¼Û
	m_PriceText.Clear();//ÐÂ¼Û
	m_Price_NumberText.Clear();//ÐÂ¼Û
	m_ItemBox.Celar();
	m_MarketGoods_Buy.Enable(false);
}
void KWndSellItem::PaintWindow()
{
//	KWndObjectBox::PaintWindow();
}
int		KWndSellItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)//´°¿Úº¯Êý
{
	if (uMsg == WND_N_BUTTON_CLICK )
	{
		if( (KWndWindow*)uParam == (KWndWindow*)&m_MarketGoods_Buy )
		{
			KUiDraggedObject	pItem;
			KUiObjAtContRegion	Obj;
			m_ItemBox.GetObject(pItem);
			
			Obj.Obj.uGenre = pItem.uGenre;
			Obj.Obj.uId = pItem.uId;
			Obj.Region.h = 0;
			Obj.Region.v = 0;
			Obj.Region.Width  = pItem.DataW;
			Obj.Region.Height = pItem.DataH;
			Obj.eContainer = UOC_NPC_SHOP;
			KUiItemBuySelInfo	Price = { 0 };
			if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE, (unsigned int)(&Obj), (int)(&Price)))
			{
				KUiShoppingCart::OpenWindow(&Obj, &Price);
				KUiSuperShop::PutItem(&Obj);
			}						
		}
	}
	return KWndObjectBox::WndProc(uMsg, uParam, nParam);
}
//////////////////////////////////////////////////

KUiSuperShop* KUiSuperShop::m_pSelf = NULL;

KUiSuperShop::KUiSuperShop()
{
	m_pBSinfo.Clear();
	m_nStoreActive = 0;
	m_nSellTypeCount = 0;
	m_nSellTypeStart = 0;
	m_nCurrentPage = 0;
	m_nCurrentShopId = 0;
	m_nPageCount = 0;
	m_uEnableTextColor = 0;
	m_uInvalidTextColor = 0;
	m_pItemInfo.Obj.uGenre = CGOG_NOTHING;
	m_pItemInfo.Obj.uId = -1;
	m_pObjsList = NULL;
	m_nObjCount = 0;
}

KUiSuperShop::~KUiSuperShop()
{
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiSuperShop* KUiSuperShop::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiSuperShop* KUiSuperShop::OpenWindow(BuySellInfo* pBSinfo)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSuperShop;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{	
		if(KUiItem::GetIfVisible() ==FALSE)
			KUiItem::OpenWindow();
		KUiItem::OnNpcTradeMode(true);
		g_UiBase.SetStatus(UIS_S_TRADE_NPC);
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateShop(pBSinfo);
		m_pSelf->BringToTop();
		m_pSelf->Show();		
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiSuperShop::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		//if (m_pSelf->m_pItemInfo.Obj.uGenre != CGOG_NOTHING)
		//{
		//	UIMessageBox("Giá hµng ch­a thanh to¸n. B¹n muèn hñy kh«ng?", m_pSelf, "X¸c nhËn", "Hñy bá", UI_WAIT_CLEAR_SHOPPINGCART);
		//}
		//else
		//{	
			KUiItem::OnNpcTradeMode(false);
			g_UiBase.SetStatus(UIS_S_IDLE);
			if (bDestroy == false)
				m_pSelf->Hide();
			else
			{
				m_pSelf->Destroy();
				m_pSelf = NULL;
			}
		//}
	}
}

void KUiSuperShop::Initialize()
{
	for (int i = 0; i < MAX_SUPERSHOP_PAGETAB; i ++)
	{
		AddChild(&m_SellType[i]);
		m_SellType[i].Hide();
	}
	for(int i =0; i< 15; i++ )
	{
		m_WndSellItem[i].Initialize();
		AddChild(&m_WndSellItem[i]);
	}
	AddChild(&m_NextBtn);
	AddChild(&m_LastBtn);
	AddChild(&m_SellTypeGuide);
	AddChild(&m_PrePaidBtn);
	AddChild(&m_NextPageBtn);
	AddChild(&m_LastPageBtn);
	AddChild(&m_CurrentPageText);
//	AddChild(&m_ShoppingCartBtn);
	AddChild(&m_CloseBtn);
	Wnd_AddWindow(this); 
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}


void KUiSuperShop::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{		
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\" SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->KWndShowAnimate::Init(&Ini, "Main");
			m_pSelf->m_NextBtn.Init(&Ini, "Right");
			m_pSelf->m_LastBtn.Init(&Ini, "Left");
			m_pSelf->m_SellTypeGuide.Init(&Ini, "SellTypeGuide");
			m_pSelf->m_PrePaidBtn.Init(&Ini, "PrePaid");
			m_pSelf->m_NextPageBtn.Init(&Ini, "RightBtn");
			m_pSelf->m_LastPageBtn.Init(&Ini, "LeftBtn");
			m_pSelf->m_CurrentPageText.Init(&Ini, "PageInfo");
			m_pSelf->m_ShoppingCartBtn.Init(&Ini, "ShoppingCart");
			
			int nLeft, nStartX, nStartY, nDiffW, nDiffH, nWidth, nHeight, i;
			int nCount = 0;
			for (i = 0; i < MAX_SUPERSHOP_PAGETAB; i ++)
			{
				m_pSelf->m_SellType[i].Init(&Ini, "SellType");
				if (i > 0)
				{
					m_pSelf->m_SellType[i - 1].GetAbsolutePos(&nLeft, NULL);
					m_pSelf->m_SellType[i - 1].GetSize(&nWidth, &nHeight);
					m_pSelf->m_SellType[i].SetPosition(nLeft + nWidth + 1, 52);
				}
			}
			for (i = 0; i < 15; i ++)
			{
				m_pSelf->m_WndSellItem[i].LoadScheme(pScheme);
			}	   
			Ini.GetInteger2("MarketGoods", "Start", &nStartX, &nStartY);
			Ini.GetInteger2("MarketGoods", "Diff", &nDiffW, &nDiffH);
			Ini.GetInteger("MarketGoods", "Width", 0, &nWidth);
			Ini.GetInteger("MarketGoods", "Height", 0, &nHeight);
			int nStartXS = nStartX;
			for (i = 0; i < 15; i ++)
			{
				m_pSelf->m_WndSellItem[i].SetPosition(nStartX, nStartY);
				nCount ++;
				nStartX += nWidth + nDiffW;
				if (nCount == MAX_ITEM_PER_ROW)
				{
					nCount = 0;
					nStartX = nStartXS;
					nStartY += nHeight + nDiffH;
				}
			}			
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
			Ini.GetString("SellType", "Color", "", Buff, sizeof(Buff));
			m_pSelf->m_uEnableTextColor = GetColor(Buff);
			Ini.GetString("SellType", "SelectColor", "", Buff, sizeof(Buff));
			m_pSelf->m_uInvalidTextColor = GetColor(Buff);
		}
	}
}


void KUiSuperShop::UpdateShop(BuySellInfo* pBSinfo)
{
	if (m_pSelf)
	{
		m_pSelf->m_pBSinfo = *pBSinfo;
		m_pSelf->m_nSellTypeCount = pBSinfo->m_nShopNum;
		m_pSelf->m_nStoreActive = m_pSelf->m_nSellTypeCount;
		if (m_pSelf->m_nStoreActive > MAX_SUPERSHOP_PAGETAB)
			m_pSelf->m_nStoreActive = MAX_SUPERSHOP_PAGETAB;
		for (int i = 0; i < m_pSelf->m_nStoreActive; i++)
			m_pSelf->m_SellType[i].Show();
		m_pSelf->SetSellTypeStart(0);
		m_pSelf->SellSortChange(m_pSelf->m_nCurrentShopId - m_pSelf->m_nSellTypeStart);
		m_pSelf->PutItem(NULL);
		g_UiBase.SetStatus(UIS_S_TRADE_NPC);
	}
}

void KUiSuperShop::SetSellTypeStart(int nStart)
{
	m_NextBtn.Enable(true);
	m_LastBtn.Enable(true);
	
	m_nSellTypeStart = nStart;
	if (m_nSellTypeStart + m_nStoreActive >= m_nSellTypeCount)
	{
		m_nSellTypeStart = m_nSellTypeCount - m_nStoreActive;
		m_NextBtn.Enable(false);
	}

	if (m_nSellTypeStart <= 0)
	{
		m_nSellTypeStart = 0;
		m_LastBtn.Enable(false);
	}
	for (int i = m_nSellTypeStart; i < m_nStoreActive + m_nSellTypeStart; i ++)
	{
		char szName[80];
		KTabFile	cTabFile;
		if (cTabFile.Load(BUYSELL_FILE)) {
			cTabFile.GetString((m_pBSinfo.m_nShopIdx[i]+2), "µÀ¾ßÒ»", "", szName, sizeof(szName));
		} else {
			itoa(m_pBSinfo.m_nShopIdx[i], szName, 10);
		}
		m_pSelf->m_SellType[i - m_nSellTypeStart].SetLabel(szName);
	}

	for (int i = 0; i < m_nStoreActive; i ++)
	{
		m_SellType[i].CheckButton((m_nCurrentShopId - m_nSellTypeStart) == i);
		m_SellType[i].SetLabelColor((m_nCurrentShopId - m_nSellTypeStart) == i ?
				m_uInvalidTextColor : m_uEnableTextColor);
	}	
}

void KUiSuperShop::SetCurrSellSort(int nSaleId)
{
	m_nCurrentShopId = m_nSellTypeStart + nSaleId;
	char szName[6];
	itoa(m_pBSinfo.m_nShopIdx[m_nCurrentShopId], szName, 10);
	m_SellTypeGuide.SetText(szName, strlen(szName));		
	m_nCurrentPage = 0;
	UpdateData();
	PageChangeInfo();
}

void KUiSuperShop::PageChangeInfo()
{
	char Buff[32];
	if (m_nPageCount <= 0)
		m_nPageCount = 1;
	if (m_nObjCount)
		sprintf(Buff, "%d/%d", m_nCurrentPage + 1, m_nPageCount);
	else
		strcpy(Buff, "_/_");
	m_CurrentPageText.SetText(Buff);
	
	m_NextPageBtn.Enable(true);
	m_LastPageBtn.Enable(true);
	if ((m_nCurrentPage + 1) >= m_nPageCount)	
		m_NextPageBtn.Enable(false);
	if (m_nCurrentPage <= 0)
		m_LastPageBtn.Enable(false);
}

void KUiSuperShop::ClearPage()
{
	for(int i =0; i< 15; i++ )
		m_WndSellItem[i].Clear();
}

void KUiSuperShop::BuyItem(int nNumber)
{
	if (m_pSelf)
	{
		if (m_pSelf->m_pItemInfo.Obj.uGenre == CGOG_NPCSELLITEM)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY,
				(unsigned int)(&m_pSelf->m_pItemInfo), nNumber);
			PutItem(NULL);
		}
	}
}


void KUiSuperShop::SetPage(int nIndex)
{
	ClearPage();
	int nAdd = 0;
	if (nIndex >= 0 && nIndex < m_nPageCount && m_pObjsList)
	{
		for (int i = 0; i < m_nObjCount; i++)
		{
			if (m_pObjsList[i].nContainer == nIndex)
			{
				UpdateItem(&m_pObjsList[i], nAdd);
				nAdd ++; 
			}
		}
		m_nCurrentPage = nIndex;
		PageChangeInfo();
	}
}

void KUiSuperShop::UpdateItem(KUiObjAtContRegion* pItem, int nAdd)
{
	if (pItem)
	{
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiObjAtContRegion	Obj;
			Obj.Obj.uGenre = pItem->Obj.uGenre;
			Obj.Obj.uId = pItem->Obj.uId;
			Obj.Region.h = 0;
			Obj.Region.v = 0;
			Obj.Region.Width  = pItem->Region.Width;
			Obj.Region.Height = pItem->Region.Height;
			Obj.eContainer = UOC_NPC_SHOP;
			KUiItemBuySelInfo	Price = { 0 };
			if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
			(unsigned int)(&Obj), (int)(&Price)))
			{
				m_WndSellItem[nAdd].m_GoodsNameText.SetText(Price.szItemName);
				m_WndSellItem[nAdd].m_OriginalPriceText.SetText("Gi¸ gèc:");
				m_WndSellItem[nAdd].m_PriceText.SetText("Gi¸ thùc:");
				if (Price.nCurPrice < Price.nOldPrice)
				{
					m_WndSellItem[nAdd].m_MarketGoods_DisCount.Show();
					int nDiscountVal = 100 - (Price.nCurPrice * 100 / Price.nOldPrice);
					if (nDiscountVal % 10 == 0 && (nDiscountVal > 0 && nDiscountVal < 100))
					{
						char szBuffer[128];
						sprintf(szBuffer, MARKET_DISCOUNT_PIC, nDiscountVal);
						m_WndSellItem[nAdd].m_MarketGoods_DisCount.SetImage(ISI_T_SPR, szBuffer);
					}
				}
				if (Price.bNewArrival)
					m_WndSellItem[nAdd].m_MarketGoods_imgNewArrival.Show();
				m_WndSellItem[nAdd].m_Price_NumberText.SetMoneyUnitPrice(Price.nCurPrice, Price.nMoneyUnit);
				m_WndSellItem[nAdd].m_OriginalPrice_NumberText.SetMoneyUnitPrice(Price.nOldPrice, Price.nMoneyUnit);
				m_WndSellItem[nAdd].m_MarketGoods_Buy.Enable(true);
				m_WndSellItem[nAdd].m_ItemBox.HoldObject(pItem->Obj.uGenre, pItem->Obj.uId, pItem->Region.Width, pItem->Region.Height);
			}
		}
	}
	else
		SetPage(m_nCurrentPage);
}



void KUiSuperShop::UpdateData()
{
	Clear();
	m_nObjCount = g_pCoreShell->GetDataSuperShop(m_nCurrentShopId, 0, 0);
	if (m_nObjCount == 0)
		return;

	if (m_pObjsList = (KUiObjAtContRegion*)malloc(sizeof(KUiObjAtContRegion) * m_nObjCount))
	{
		m_nPageCount = g_pCoreShell->GetDataSuperShop(m_nCurrentShopId, (unsigned int)m_pObjsList, m_nObjCount) + 1;
		SetPage(0);
	}
	else
		m_nObjCount = 0;
}

void KUiSuperShop::Clear()
{
	m_nObjCount  = 0;
	m_nPageCount = 0;
	if (m_pObjsList)
	{
		free(m_pObjsList);
		m_pObjsList = NULL;
	}
	ClearPage();
}

void KUiSuperShop::SellSortChange(int i)
{
	if (g_pCoreShell)
	{
		SetCurrSellSort(i);
		for (int j = 0; j < m_nStoreActive; j ++)
		{
			m_SellType[j].CheckButton(i == j);
			m_SellType[j].SetLabelColor(i == j ?
					m_uInvalidTextColor : m_uEnableTextColor);
		}
	}
}

void KUiSuperShop::CancelTrade()
{
	CloseWindow(true);
}

int	KUiSuperShop::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK )
	{	
		for (int i = 0; i < MAX_SUPERSHOP_PAGETAB; i ++)
		{
			if ((KWndWindow*)uParam == (KWndWindow*)&m_SellType[i])
				SellSortChange(i);
		}	
		if( (KWndWindow*)uParam == (KWndWindow*)&m_CloseBtn )
		{
		  CancelTrade();
		  return 0;
		}
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_PrePaidBtn)
		{
			KIniFile Ini;
			if (Ini.Load(GAME_SETTING_FILE_INI))
			{
				char szFilePath[MAX_PATH];
				Ini.GetString("URL", "PrePaid", "", szFilePath, sizeof(szFilePath));
				ShellExecute(NULL,"open",szFilePath,NULL,NULL,SW_SHOWNORMAL); 
			}
		}	
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_NextBtn)
			SetSellTypeStart(m_nSellTypeStart + 1);
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_LastBtn)
			SetSellTypeStart(m_nSellTypeStart - 1);
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_NextPageBtn)
			SetPage(m_nCurrentPage + 1);
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_LastPageBtn)
			SetPage(m_nCurrentPage - 1);
		else if ((KWndWindow*)uParam == (KWndWindow*)&m_ShoppingCartBtn)
		{
			if (m_pItemInfo.Obj.uGenre != CGOG_NOTHING)
			{
				KUiItemBuySelInfo	Price = { 0 };
				if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE, (unsigned int)(&m_pItemInfo), (int)(&Price)))
				{
					KUiShoppingCart::OpenWindow(&m_pItemInfo, &Price);
				}
			}
		}
	}
	else if (uMsg == WM_KEYDOWN)
	{
		if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC)
			{
				CancelTrade();
			}
			else
				g_UiBase.SetStatus(UIS_S_TRADE_NPC);
		}
	}	
	else if (uMsg == WND_M_OTHER_WORK_RESULT)
	{
		if (uParam == UI_WAIT_CLEAR_SHOPPINGCART)
		{
			if (!nParam)
			{
				m_pItemInfo.Obj.uGenre = CGOG_NOTHING;
				m_pItemInfo.Obj.uId = -1;
				CancelTrade();
			}
		}
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

void KUiSuperShop::PutItem(KUiObjAtContRegion* pObj)
{
	if (m_pSelf && pObj != NULL)
	{
		m_pSelf->m_pItemInfo = *pObj;
		m_pSelf->m_ShoppingCartBtn.Enable(true);
	}
	else
	{
		m_pSelf->m_pItemInfo.Obj.uGenre = CGOG_NOTHING;
		m_pSelf->m_pItemInfo.Obj.uId = -1;
		m_pSelf->m_ShoppingCartBtn.Enable(false);	
	}
}

void KUiSuperShop::Breathe()
{
	for (int i = 0; i < 15; i ++)
	if (m_WndSellItem[i].m_MarketGoods_imgNewArrival.IsVisible())
	{
		if (m_WndSellItem[i].m_MarketGoods_imgNewArrival.GetCurrentFrame() >= 2)
			m_WndSellItem[i].m_MarketGoods_imgNewArrival.SetFrame(0);
		else
			m_WndSellItem[i].m_MarketGoods_imgNewArrival.NextFrame();
	}
}

int KCanGetNumFrame::GetMaxFrame()
{
	return m_Image.nNumFrames;
}


int KCanGetNumFrame::GetCurrentFrame()
{
	return m_Image.nFrame;
}


#define SCHEME_INI_VN		"¾«Á¶Ê¯ÉÌµêvn.ini"

KUiDynamicShop* KUiDynamicShop::m_pSelf = NULL;

KUiDynamicShop::KUiDynamicShop()
{
	m_pBSinfo.Clear();
	m_nCurrentShopId = 0;
	m_pObjsList = NULL;
	m_nObjCount = 0;
	m_nPageCount = 0;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
//--------------------------------------------------------------------------
KUiDynamicShop* KUiDynamicShop::GetIfVisible()
{
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiDynamicShop* KUiDynamicShop::OpenWindow(BuySellInfo *pBSinfo)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiDynamicShop;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{	
		if(KUiItem::GetIfVisible() ==FALSE)
			KUiItem::OpenWindow();
		KUiItem::OnNpcTradeMode(true);
		g_UiBase.SetStatus(UIS_S_TRADE_NPC);
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateShop(pBSinfo);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiDynamicShop::CloseWindow()
{
	if (m_pSelf)
	{
		KUiItem::OnNpcTradeMode(false);
		KUiTradeConfirm::CloseWindow(true);
		g_UiBase.SetStatus(UIS_S_IDLE);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

//³õÊ¼»¯
void KUiDynamicShop::Initialize()
{
	for (int i = 0; i < MAX_SUPERSHOP_PAGETAB; i++)
	{
		AddChild(&m_SellType[i]);
		m_SellType[i].Hide();
	}
	AddChild(&m_ItemsBox);
	AddChild(&m_BuyBtn);
	AddChild(&m_SellBtn);
	AddChild(&m_RepairBtn);
	AddChild(&m_PreBtn);
	AddChild(&m_NextBtn);
	AddChild(&m_CloseBtn);
	AddChild(&m_CurPageTxt);
	AddChild(&m_TitleImage);
	
	m_ItemsBox.SetContainerId((int)UOC_NPC_SHOP);
	Wnd_AddWindow(this);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

//ÔØÈë½çÃæ·½°¸
void KUiDynamicShop::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{		
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\" SCHEME_INI_VN, pScheme);
		if (Ini.Load(Buff))
		{
			char szKey[16];
			m_pSelf->KWndShowAnimate::Init(&Ini, "Main");
			for (int i = 0; i < MAX_SUPERSHOP_PAGETAB; i++)
			{
				sprintf(szKey, "TypeBtn_%d", i);
				m_pSelf->m_SellType[i].Init(&Ini, szKey);
			}
			m_pSelf->m_ItemsBox.Init(&Ini, "ItemBox");
			m_pSelf->m_BuyBtn.Init(&Ini, "BuyBtn");
			m_pSelf->m_SellBtn.Init(&Ini, "SellBtn");
			m_pSelf->m_RepairBtn.Init(&Ini, "RepairBtn");
			m_pSelf->m_PreBtn.Init(&Ini, "LeftBtn");
			m_pSelf->m_NextBtn.Init(&Ini,"RightBtn");
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
			m_pSelf->m_CurPageTxt.Init(&Ini, "CurPageTxt");
			m_pSelf->m_TitleImage.Init(&Ini, "TitleImage");
			
			m_pSelf->m_ItemsBox.EnablePickPut(false);
		}
	}
}

void KUiDynamicShop::CancelTrade()
{
	CloseWindow();
}

//´°¿Úº¯Êý
int	KUiDynamicShop::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam, nParam);
		break;
	case WND_N_LEFT_CLICK_ITEM:
		OnBuyItem((KUiDraggedObject*)uParam,
			g_UiBase.GetStatus() == UIS_S_TRADE_BUY);
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		if (nParam == (int)(KWndWindow*)&m_ItemsBox)
			OnBuyItem((KUiDraggedObject*)uParam, true);
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() == UIS_S_TRADE_NPC)
			{
				CloseWindow();
			}
			else
			{
				m_BuyBtn.CheckButton(false);
				m_SellBtn.CheckButton(false);
				m_RepairBtn.CheckButton(false);
				g_UiBase.SetStatus(UIS_S_TRADE_NPC);
			}
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiDynamicShop::OnBuyItem(KUiDraggedObject* pItem, bool bDoImmed)
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
	Obj.eContainer = UOC_NPC_SHOP;

	UISYS_STATUS eStatus = g_UiBase.GetStatus();
		if (bDoImmed == false)
		{		
			KUiItemBuySelInfo	Price = { 0 };
			if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
				(unsigned int)(&Obj), (int)(&Price)) && eStatus != UIS_S_TRADE_SALE && eStatus != UIS_S_TRADE_REPAIR)
			{
				KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_BUY);
			}		
		}
		else
		{
			if (eStatus == UIS_S_TRADE_BUY)
			{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY,
				(unsigned int)(&Obj), 1);	
				return;
			}		
			else
			{
				if ((GetKeyState(VK_SHIFT) & 0x8000) != 0)
				{
				g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY,
					(unsigned int)(&Obj), 1);	
					return;
				}
					else
					{
						KSystemMessage	Msg;
						Msg.byConfirmType = SMCT_NONE;
						Msg.byParamSize = 0;
						Msg.byPriority = 0;
						Msg.eType = SMT_NORMAL;
						Msg.uReservedForUi = 0;
						strcpy(Msg.szMessage, "NhÊn gi÷ phÝm Shift ®ång thêi nhÊn chuét ph¶i lËp tøc mua ®­îc vËt phÈm!");
						KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
						return;
					}	
			}	
		}
}

void KUiDynamicShop::OnClickButton(KWndButton* pWnd, int bCheck)
{
	if (Wnd_GetDragObj(NULL))
		return;
	if (pWnd == &m_BuyBtn)
	{
		if (bCheck)
		{
			m_SellBtn.CheckButton(false);
			m_RepairBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_BUY);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC);
	}
	else if (pWnd == &m_SellBtn)
	{
		if (bCheck)
		{
			m_BuyBtn.CheckButton(false);
			m_RepairBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_SALE);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC);
	}
	else if (pWnd == &m_RepairBtn)
	{
		if (bCheck)
		{
			m_BuyBtn.CheckButton(false);
			m_SellBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_REPAIR);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC);
	}
	else if (pWnd == &m_PreBtn)
		SetPage(m_nCurrentPage - 1);
	else if(pWnd == &m_NextBtn)
		SetPage(m_nCurrentPage + 1);
	else if (pWnd == &m_CloseBtn)
		CloseWindow();
	else
	{
		for (int i = 0; i < MAX_SUPERSHOP_PAGETAB; i ++)
		{
			if (pWnd == &m_SellType[i])
				SellSortChange(i);
		}	
	}
}

void KUiDynamicShop::SellSortChange(int i)
{
	m_nCurrentShopId = i;
	UpdateData();
	for (int j = 0; j < MAX_SUPERSHOP_PAGETAB; j ++)
	{
		m_SellType[j].CheckButton(i == j);
	}
}

void KUiDynamicShop::SetPage(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nPageCount && m_pObjsList)
	{
		m_ItemsBox.Clear();
		for (int i = 0; i < m_nObjCount; i++)
		{
			if (m_pObjsList[i].nContainer == nIndex)
				UpdateItem(&m_pObjsList[i], true);
		}
		m_nCurrentPage = nIndex;
	}
	char Buff[32];
	if (m_nPageCount <= 0)
		m_nPageCount = 1;
	if (m_nObjCount)
		sprintf(Buff, "%d/%d", m_nCurrentPage + 1, m_nPageCount);
	else
		strcpy(Buff, "_/_");
	m_CurPageTxt.SetText(Buff);
	
	m_NextBtn.Enable(true);
	m_PreBtn.Enable(true);
	if ((m_nCurrentPage + 1) >= m_nPageCount)	
		m_NextBtn.Enable(false);
	if (m_nCurrentPage <= 0)
		m_PreBtn.Enable(false);
}

void KUiDynamicShop::UpdateItem(KUiObjAtContRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
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
				m_ItemsBox.AddObject(&Obj, 1);
			else
				m_ItemsBox.RemoveObject(&Obj);
		}
		
	}
	else
		SetPage(m_nCurrentPage);
}

void KUiDynamicShop::UpdateShop(BuySellInfo *pBSinfo)
{
	m_pBSinfo = *pBSinfo;
	for (int i = 0; i < pBSinfo->m_nShopNum; i ++)
	{
		char szName[6];
		itoa(pBSinfo->m_nShopIdx[i], szName, 10);
		m_SellType[i].SetLabel(szName);

		m_SellType[i].Show();
	}
	SellSortChange(m_nCurrentShopId);
	UpdateData();
}

void KUiDynamicShop::UpdateData()
{
	Clear();
	m_nObjCount = g_pCoreShell->GetDataDynamicShop(m_nCurrentShopId, 0, 0);
	if (m_nObjCount == 0)
		return;

	if (m_pObjsList = (KUiObjAtContRegion*)malloc(sizeof(KUiObjAtContRegion) * m_nObjCount))
	{
		g_pCoreShell->GetDataDynamicShop(m_nCurrentShopId, (unsigned int)m_pObjsList, m_nObjCount);//µ¥Ïß³ÌÖ´ÐÐ£¬nCountÖµ²»±ä
		m_nPageCount = m_pObjsList[m_nObjCount - 1].nContainer + 1;
		SetPage(0);
	}
	else
		m_nObjCount = 0;
}

void KUiDynamicShop::Clear()
{
	m_nObjCount  = 0;
	m_nPageCount = 0;
	if (m_pObjsList)
	{
		free(m_pObjsList);
		m_pObjsList = NULL;
	}
}