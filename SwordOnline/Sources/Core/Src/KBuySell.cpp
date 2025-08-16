#include "KEngine.h"
#include "KCore.h"
#include "KTabFile.h"
#include "KPlayer.h"
#include "KPlayerSet.h"
#include "KTongData.h"
#include "KNpc.h"
#include "KItemGenerator.h"
#include "KSubWorldSet.h"
#include "KItemSet.h"
#include "KBuySell.h"
#include "KObjSet.h"
#include "KTongProtocol.h"
#ifndef _STANDALONE
#include "CoreShell.h"
#include "crtdbg.h"
#endif
#include "CoreUseNameDef.h"
#ifdef _SERVER
//#include "../MultiServer/Heaven/Interface/iServer.h"
#endif

#include	<time.h>

#define	SHOP_BOX_WIDTH		6
#define	SHOP_BOX_HEIGHT		10
#define	SSHOP_BOX_WIDTH		12
#define	SSHOP_BOX_HEIGHT	10

KBuySell	BuySell;

KBuySell::KBuySell()
{
#ifndef _SERVER
	m_pShopRoom = NULL;
	m_pSShopRoom = NULL;
#endif
	m_Item = NULL;
	m_SellItem = NULL;
	m_Width = 0;
	m_Height = 0;
	m_ItemNum = 0;
}

KBuySell::~KBuySell()
{
#ifndef _SERVER
	if (m_pShopRoom)
	{
		delete m_pShopRoom;
		m_pShopRoom = NULL;
	}
	if (m_pSShopRoom)
	{
		delete m_pSShopRoom;
		m_pSShopRoom = NULL;
	}
#endif
	if (m_Item)
	{
		delete [] m_Item;
		m_Item = NULL;
	}
	if (m_SellItem)
	{
		for (int i = 0; i < m_Height; i++)
		{
			if (m_SellItem[i])
			{
				delete[] m_SellItem[i];
				m_SellItem[i] = NULL;
			}
		}
		delete[] m_SellItem;
		m_SellItem = NULL;
	}
	m_Width = 0;
	m_Height = 0;
	m_ItemNum = 0;
}

int KBuySell::Init()
{
	KTabFile		GoodsFile;
	KTabFile		BuySellFile;
//	g_SetFilePath("\\");
	if (!BuySellFile.Load(BUYSELL_FILE) || !GoodsFile.Load(GOODS_FILE))
		return 0;

	int nHeight = GoodsFile.GetHeight() - 1;
	int nWidth = GoodsFile.GetWidth() - 1;
	if (nWidth <= 0 || nHeight <= 0)
		return 0;

	m_Item = (KItem *)new KItem[nHeight];
	if (!m_Item)
		return 0;
	ItemGenerate ItemGenerator;
	for (int k = 0; k < nHeight; k++)
	{
		GoodsFile.GetInteger(k + 2, 2, -1, &ItemGenerator.nItemNature);
		GoodsFile.GetInteger(k + 2, 3, -1, &ItemGenerator.nGenre);
		GoodsFile.GetInteger(k + 2, 4, -1, &ItemGenerator.nDetailType);
		GoodsFile.GetInteger(k + 2, 5, -1, &ItemGenerator.nParticularType);
		GoodsFile.GetInteger(k + 2, 6, 0, &ItemGenerator.nLevel);
		GoodsFile.GetInteger(k + 2, 7, 5, &ItemGenerator.nSeriesReq);
		GoodsFile.GetInteger(k + 2, 8, 0, &ItemGenerator.nNewPrice);
		GoodsFile.GetInteger(k + 2, 9, -1, &ItemGenerator.nParam);
		GoodsFile.GetInteger(k + 2, 10, 0, &ItemGenerator.nExpirePoint);
		GoodsFile.GetInteger(k + 2, 11, 0, &ItemGenerator.nLock);
		GoodsFile.GetInteger(k + 2, 12, 0, &ItemGenerator.nNewArrival);
		switch(ItemGenerator.nGenre)
		{
		case item_equip:
			ItemGen.Gen_Equipment(ItemGenerator.nItemNature,
					ItemGenerator.nDetailType,
					ItemGenerator.nParticularType,
					ItemGenerator.nSeriesReq,
					ItemGenerator.nLevel,
					NULL,
					ItemGenerator.nItemNature>=NATURE_GOLD?MAKELONG(ItemGenerator.nLevel,ItemGenerator.nSeriesReq):0,
					g_SubWorldSet.GetGameVersion(),
					&m_Item[m_ItemNum]);
			break;
		case item_medicine:
			ItemGen.Gen_Medicine(ItemGenerator.nDetailType, 
				ItemGenerator.nLevel, 
				g_SubWorldSet.GetGameVersion(),
				&m_Item[m_ItemNum]);
			break;
		case item_event:
			ItemGen.Gen_Event(ItemGenerator.nDetailType, &m_Item[m_ItemNum]);
			break;
		case item_materials:
			break;
		case item_task:
			ItemGen.Gen_Quest(ItemGenerator.nDetailType, &m_Item[m_ItemNum]);
			break;
		case item_townportal:
			ItemGen.Gen_TownPortal(ItemGenerator.nDetailType, &m_Item[m_ItemNum]);
			break;
		case item_magicscript:
			ItemGen.Gen_MagicScript(ItemGenerator.nDetailType, &m_Item[m_ItemNum], ItemGenerator.nLevel, ItemGenerator.nSeriesReq, 0);
			break;
		default:
			break;
		}
		if (ItemGenerator.nNewPrice)
			m_Item[m_ItemNum].SetNewPrice(ItemGenerator.nNewPrice);
		m_Item[m_ItemNum].SetParam(ItemGenerator.nParam);
		if (ItemGenerator.nExpirePoint)
			m_Item[m_ItemNum].SetExpirePoint(ItemGenerator.nExpirePoint);
		if (ItemGenerator.nLock)
			m_Item[m_ItemNum].LockItem(ItemGenerator.nLock);
		m_Item[k].SetNewArrival(ItemGenerator.nNewArrival > 0);
		m_Item[m_ItemNum].GetBackLocal()->Release();
		
		m_ItemNum++;
	}

	m_Height = BuySellFile.GetHeight() - 1;
	m_Width  = BuySellFile.GetWidth();
	
	if (m_Width <= 0 || m_Height <= 0)
		return 0;

	m_SellItem = (int **)new int*[m_Height];
	if (!m_SellItem)
		return 0;
	for (int i = 0; i < m_Height; i++)
	{
		m_SellItem[i] = NULL;
		m_SellItem[i] = (int *)new int[m_Width];
		if (!m_SellItem[i])
			return 0;
		for (int j = 0; j < m_Width; j++)
		{
			BuySellFile.GetInteger(i + 2, j + 2, -1, &m_SellItem[i][j]);
			if (m_SellItem[i][j] == -1)
			{
				continue;
			}
			_ASSERT(m_SellItem[i][j] > 0);		// 策划是从1开始的
			if (m_SellItem[i][j] > 0)
				m_SellItem[i][j] -= 1;			// 为了策划从1开始填表
		}
	}
#ifndef _SERVER
	if (!m_pShopRoom)
	{
		m_pShopRoom = new KInventory;
		m_pShopRoom->Init(SHOP_BOX_WIDTH, SHOP_BOX_HEIGHT);
	}
	if (!m_pSShopRoom)
	{
		m_pSShopRoom = new KInventory;
		m_pSShopRoom->Init(SSHOP_BOX_WIDTH, SSHOP_BOX_HEIGHT);
	}
#endif
	return m_ItemNum;
}

KItem* KBuySell::GetItem(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_ItemNum || !m_Item)
		return NULL;

	return &m_Item[nIndex];
}

int KBuySell::GetItemIndex(int nShop, int nIndex)
{
	if (!m_SellItem || nShop < 0 || nShop >= m_Height || nIndex < 0 || nIndex >= m_Width)
		return -1;

	if (!m_SellItem[nShop])
		return -1;

	return m_SellItem[nShop][nIndex];
}

BOOL KBuySell::BuySellCheck(int nBuy, int nBuyIdx)
{
	if (nBuyIdx < 0 || nBuyIdx >= m_Height)
		return FALSE;
	if (nBuy < 0 || nBuy >= m_Width)
		return FALSE;
	int nIdx = m_SellItem[nBuy][nBuyIdx];
	if (nIdx < 0 || nIdx >= m_ItemNum)
		return FALSE;

	return TRUE;
}

#ifdef _SERVER
BOOL KBuySell::CanBuy(int nPlayerIdx, int nBuy, int nBuyIdx, int nBuyNumber)
{
	BOOL bShowMsg = TRUE;
	if (nPlayerIdx < 0 || nPlayerIdx >= MAX_PLAYER)
	{
		_ASSERT(0);
		return FALSE;
	}
	if (Player[nPlayerIdx].CheckTrading())
		return FALSE;

	if (BuySellCheck(nBuy, nBuyIdx) == FALSE)
		return FALSE;

	if (nBuy == -1)
	{
		printf("[error]BuySell: %s buy idx error (%d)!", Npc[Player[nPlayerIdx].m_nIndex].Name, nBuy);
		return FALSE;
	}

	if (nBuyNumber <= 0)
		return FALSE;

	int nIdx = m_SellItem[nBuy][nBuyIdx];
	int nPrice = m_Item[nIdx].GetCurPrice() * nBuyNumber;
	SHOW_MSG_SYNC	sMsg;
	sMsg.ProtocolType = s2c_msgshow;
	sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
	if (Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
	{
		bShowMsg = FALSE;		
		sMsg.m_wMsgID = enumMSG_ID_FIGHT_MODE_ERROR1;
	}
	else
	{
		switch (Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit)
		{
		case moneyunit_money:
			if (Player[nPlayerIdx].m_ItemList.GetEquipmentMoney() < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_MONEY;
			}
			break;
		case moneyunit_extpoint:
			if (Player[nPlayerIdx].GetExtPoint() < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_EXTPOINT;
			}
			break;
		case moneyunit_fuyuan:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_FUYUAN;
			}
			break;
		case moneyunit_repute:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_REPUTE;
			}
			break;
		case moneyunit_accum:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_ACCUM;
			}	
			break;
		case moneyunit_honor:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_HONOR;
			}
			break;
		case moneyunit_respect:
			if (Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT) < nPrice)
			{
				bShowMsg = FALSE;		
				sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_RESPECT;
			}
			break;
		default:
			return FALSE;
		}
	}

	ItemPos	Pos;
	if (Player[nPlayerIdx].m_ItemList.SearchPosition(m_Item[nIdx].GetWidth(), m_Item[nIdx].GetHeight(), &Pos, true) == FALSE)
	{
		bShowMsg = FALSE;		
		sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_ROOM;
	}

	if (!bShowMsg)
		g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);	

	return bShowMsg;
}

BOOL KBuySell::Buy(int nPlayerIdx, int nBuy, int nBuyIdx, BYTE nBuyNumber)
{
	KASSERT(nPlayerIdx >= 0 && nPlayerIdx < MAX_PLAYER);

	if (nBuy == -1)
	{
		printf("BuySell: %s buy idx error!", Npc[Player[nPlayerIdx].m_nIndex].Name);
		return FALSE;
	}
	if (nBuyIdx >= m_Width)
		return FALSE;

	if (m_SellItem[nBuy][nBuyIdx] < 0 || m_SellItem[nBuy][nBuyIdx] >= m_ItemNum)
		return FALSE;

	if (CanBuy(nPlayerIdx, nBuy, nBuyIdx, nBuyNumber) == FALSE)
		return FALSE;
		
	int nIdx = m_SellItem[nBuy][nBuyIdx];
	int nWidth = m_Item[nIdx].GetWidth();
	int nHeight = m_Item[nIdx].GetHeight();

	if (m_Item[nIdx].GetExpirePoint())
	{
		m_Item[nIdx].SetExpireTime(KSG_GetCurSec() + m_Item[nIdx].GetExpirePoint());
	}

	int nPrice = m_Item[nIdx].GetCurPrice();
	for(int i = 0; i < nBuyNumber; i++)
	{
		int nItemIdx = ItemSet.Add(&m_Item[nIdx]);

		if (nItemIdx <= 0)
			return FALSE;	

		ItemPos	Pos;
		if (Player[nPlayerIdx].m_ItemList.SearchPosition(nWidth, nHeight, &Pos, true))
		{
			Player[nPlayerIdx].m_ItemList.Add(nItemIdx, pos_equiproom, Pos.nX, Pos.nY);
		}
		else
		{
			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_SHOP_NO_ROOM;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
			break;
		}
	}
	int i = 1;
	if (i)
	{
	switch (Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit)
	{
	case moneyunit_money:
		Player[nPlayerIdx].Pay(nPrice*i);	
		break;
	case moneyunit_extpoint:
		Player[nPlayerIdx].PayExtPoint(nPrice*i);
		break;
	case moneyunit_fuyuan:
		Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_FUYUAN, 
			Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_FUYUAN)-(nPrice*i));
		break;
	case moneyunit_repute:
		Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_REPUTE, 
			Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_REPUTE)-(nPrice*i));
		break;
	case moneyunit_accum:
		Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_ACCUM, 
			Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_ACCUM)-(nPrice*i));
		break;
	case moneyunit_honor:
		Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_HONOR, 
			Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_HONOR)-(nPrice*i));
		break;
	case moneyunit_respect:
		Player[nPlayerIdx].m_cTask.SetSaveVal(TASKVALUE_STATTASK_RESPECT, 
			Player[nPlayerIdx].m_cTask.GetSaveVal(TASKVALUE_STATTASK_RESPECT)-(nPrice*i));
		break;
	default:
		printf("[error]BuySell %s moneyunit (%d)", Npc[Player[nPlayerIdx].m_nIndex].Name, Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit);
		break;
	}
	}
	return TRUE;
}
/*******************************************************************************
参数 nIdx 指游戏里Item数组的编号
*******************************************************************************/
BOOL KBuySell::Sell(int nPlayerIdx, int nBuy, int nIdx, int nBuyNumber)
{
	KASSERT(nPlayerIdx >= 0 && nPlayerIdx < MAX_PLAYER);
	KASSERT(nIdx >= 0 && nIdx < MAX_ITEM);
	
	if (Npc[Player[nPlayerIdx].m_nIndex].m_FightMode)
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_FIGHT_MODE_ERROR1;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		if(g_pServer && Player[nPlayerIdx].m_nNetConnectIdx != -1)
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}	
	
	if (nBuy == -1)
	{
		printf("BuySell: %s buy idx error!", Npc[Player[nPlayerIdx].m_nIndex].Name);
		return FALSE;
	}

	if (Item[nIdx].GetLock()->IsLock() || 
		Item[nIdx].GetLockSell())
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_LOCK_NOT_TRADE;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
		if(g_pServer && Player[nPlayerIdx].m_nNetConnectIdx != -1)
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		return FALSE;
	}

	int nMoney = Item[nIdx].GetSalePrice() * Item[nIdx].GetStackNum();

	if (nMoney)
		Player[nPlayerIdx].Earn(nMoney);
	Player[nPlayerIdx].m_ItemList.Remove(nIdx);
	ItemSet.Remove(nIdx);	

	return TRUE;
}
#endif

#ifndef _SERVER
void KBuySell::PaintItem(int nIdx, int nX, int nY, BOOL bStack)
{
	int nShop = Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nShopIdx[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop];
	if (nShop < 0 || nShop >= m_Height)
		return;
	int nItemIdx = GetItemIndex(nShop, nIdx);

	int x = nX;
	int y = nY;

	KItem* pItem = GetItem(nItemIdx);

	if (pItem)
	{
		pItem->Paint(x, y, false, false);
	}
}

void KBuySell::OpenSale(BuySellInfo *pInfo)
{
	if (pInfo->m_nShopIdx[0] < 0 || pInfo->m_nShopIdx[0] >= m_Height)
		return;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.Clear();
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo = *pInfo;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = 0;
	CoreDataChanged(GDCNI_NPC_TRADE, NULL, TRUE);
}

void KBuySell::OpenSale(int nSaleType, BuySellInfo *pInfo)
{
	if(nSaleType < 0 || nSaleType > 1)
		return;

	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.Clear();
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo = *pInfo;
	Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nCurShop = 0;
	CoreDataChanged(GDCNI_SUPERSHOP, (unsigned int)pInfo, nSaleType);
}
#endif

#ifdef _SERVER
void KBuySell::OpenSale(int nPlayerIdx, int nShop, int nShopMoneyUnit)
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
		return;
	if (nShop < 0 || nShop >= m_Height)
		return;
	Player[nPlayerIdx].m_BuyInfo.Clear();
	Player[nPlayerIdx].m_BuyInfo.m_nShopIdx[0] = nShop;
	Player[nPlayerIdx].m_BuyInfo.m_nShopNum = 1;
	Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit = nShopMoneyUnit;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsY);

	SALE_BOX_SYNC saleSync;
	saleSync.ProtocolType = s2c_opensalebox;
	memcpy(&saleSync.m_BuySellInfo, &Player[nPlayerIdx].m_BuyInfo, sizeof(BuySellInfo));
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &saleSync, sizeof(SALE_BOX_SYNC));
}

void KBuySell::OpenSale(int nPlayerIdx, int nSaleType, int nMoneyUnit, int nShopNum, int *nShopId)
{
	if (nPlayerIdx <= 0 || nPlayerIdx > MAX_PLAYER)
		return;
	if(nSaleType < 0 || nSaleType > 1)
		return;
	if(nMoneyUnit < moneyunit_money || nMoneyUnit >= moneyunit_num)
		return;
	if(nShopNum <= 0 || nShopNum >= MAX_SUPERSHOP_SHOPTAB)
		return;

	Player[nPlayerIdx].m_BuyInfo.Clear();
	memcpy(&Player[nPlayerIdx].m_BuyInfo.m_nShopIdx, nShopId, sizeof(int) * MAX_SUPERSHOP_SHOPTAB);
	Player[nPlayerIdx].m_BuyInfo.m_nShopNum = nShopNum;
	Player[nPlayerIdx].m_BuyInfo.m_nMoneyUnit = nMoneyUnit;
	Player[nPlayerIdx].m_BuyInfo.m_SubWorldID = Npc[Player[nPlayerIdx].m_nIndex].m_SubWorldIndex;
	Npc[Player[nPlayerIdx].m_nIndex].GetMpsPos(
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsX,
		&Player[nPlayerIdx].m_BuyInfo.m_nMpsY);

	S2C_SUPERSHOP	sSale;
	sSale.ProtocolType = s2c_syncsupershop;
	sSale.m_nSaleType = nSaleType;
	memcpy(&sSale.m_BuySellInfo, &Player[nPlayerIdx].m_BuyInfo, sizeof(BuySellInfo));
	g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, (BYTE*)&sSale, sizeof(S2C_SUPERSHOP));
}
#endif
