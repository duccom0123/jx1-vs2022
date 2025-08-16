#include	"KCore.h"
#include	"MyAssert.H"
#include	"KItem.h"
#include	"KItemSet.h"
#include	"KNpc.h"
#include	"KMath.h"
#include	"KPlayer.h"
#include	"KPlayerSet.h"
#include	"KItemList.h"
#include	"KItemChangeRes.h"
#include	"KSortScript.h"
#include	<time.h>
#include    "../../Engine/Src/KSG_StringProcess.h"
#ifdef _SERVER
//#include	"../../Headers/IServer.h"
#include	"KObjSet.h"
#endif
#ifndef _SERVER
#include	"CoreShell.h"
#include	"../../Headers/IClient.h"
#endif


// PK死亡惩罚掉穿的装备，各装备掉落的权值
#define		defEQUIP_POWER
#ifdef defEQUIP_POWER
	int		g_nEquipPower[itempart_num] =
	{2, 4, 2, 4, 1, 1, 1, 1, 1, 1, 0};
#endif

int KItemList::ms_ActiveEquip[itempart_num][MAX_ITEM_ACTIVE] = 
{
	{ itempart_foot, itempart_ring1	},	//	itempart_head = 0,
	{ itempart_weapon, itempart_head },	//	itempart_body,
	{ itempart_body, itempart_amulet },	//	itempart_belt,
	{ itempart_ring1, itempart_foot },	//	itempart_weapon,
	{ itempart_cuff, itempart_pendant },//	itempart_foot,
	{ itempart_belt, itempart_ring2 },	//	itempart_cuff,
	{ itempart_head, itempart_weapon },	//	itempart_amulet,
	{ itempart_cuff, itempart_pendant },//	itempart_ring1,
	{ itempart_amulet, itempart_body },	//	itempart_ring2,
	{ itempart_belt, itempart_ring2 },	//	itempart_pendant,
	{ itempart_horse, itempart_horse },	//	itempart_horse,
	{ itempart_mask, itempart_mask },	//	itempart_mask,
	{ itempart_mantle, itempart_mantle },//	itempart_mantle,
	{ itempart_signet, itempart_signet },	//	itempart_signet,
	{ itempart_shipin, itempart_shipin },		//	itempart_shipin,
	{ itempart_hoods, itempart_hoods },//	itempart_hoods,
	{ itempart_cloak, itempart_cloak },//	itempart_cloak,
};

int KItemList::ms_ActivedEquip[itempart_num][MAX_ITEM_ACTIVE] =
{
	{ itempart_body, itempart_amulet },	//	itempart_head = 0,
	{ itempart_ring2, itempart_belt },	//	itempart_body,
	{ itempart_pendant, itempart_cuff },//	itempart_belt,
	{ itempart_amulet, itempart_body },	//	itempart_weapon,
	{ itempart_weapon, itempart_head },	//	itempart_foot,
	{ itempart_foot, itempart_ring1 },	//	itempart_cuff,
	{ itempart_belt, itempart_ring2 },	//	itempart_amulet,
	{ itempart_weapon, itempart_head },	//	itempart_ring1,
	{ itempart_cuff, itempart_pendant },//	itempart_ring2,
	{ itempart_foot, itempart_ring1 },	//	itempart_pendant,
	{ itempart_horse, itempart_horse },	//	itempart_horse,
	{ itempart_mask, itempart_mask },	//	itempart_mask,
	{ itempart_mantle, itempart_mantle },//	itempart_mantle,
	{ itempart_signet, itempart_signet },	//	itempart_signet,
	{ itempart_shipin, itempart_shipin },		//	itempart_shipin,
	{ itempart_hoods, itempart_hoods },//	itempart_hoods,
	{ itempart_cloak, itempart_cloak },//	itempart_cloak,
};

KItemList::KItemList()
{
	m_PlayerIdx = 0;
	m_nListCurIdx = 0;
}

KItemList::~KItemList()
{

}


/*!*****************************************************************************
// Function		: KItemList::GetWeaponType
// Purpose		: 取得玩家装备的武器类型
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::GetWeaponType()
{
	if (m_EquipItem[itempart_weapon])
		return Item[m_EquipItem[itempart_weapon]].GetDetailType();
	else
		return -1;
}

void KItemList::GetWeaponDamage(int* nMin, int* nMax)
{
	int nWeaponIdx = m_EquipItem[itempart_weapon];
	if (nWeaponIdx)
	{
		_ASSERT(Item[nWeaponIdx].m_CommonAttrib.nItemGenre == item_equip 
			&& (Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_meleeweapon
			|| Item[nWeaponIdx].m_CommonAttrib.nDetailType == equip_rangeweapon));
		int nMinDamage, nMaxDamage, nEnhance;
		int nDamageMinBase = Item[nWeaponIdx].m_aryBaseAttrib[0].nValue[0];
		int	nDamageMaxBase = Item[nWeaponIdx].m_aryBaseAttrib[1].nValue[0];
		nMinDamage = 0;
		nMaxDamage = 0;
		nEnhance = 0;
		for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			switch(Item[nWeaponIdx].m_aryMagicAttrib[i].nAttribType)
			{
			case magic_weapondamagemin_v:
				nMinDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamagemax_v:
				nMaxDamage += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			case magic_weapondamageenhance_p:
				nEnhance += Item[nWeaponIdx].m_aryMagicAttrib[i].nValue[0];
				break;
			default:
				break;
			}
		}
		*nMin = (nDamageMinBase + nMinDamage) * (100 + nEnhance) / 100;
		*nMax = (nDamageMaxBase + nMaxDamage) * (100 + nEnhance) / 100;
	}
	else	// 空手
	{
		/*
		int nDamageBase = Player[m_PlayerIdx].m_nCurStrength * Player[m_PlayerIdx].m_nCurDexterity;
		*nMin = nDamageBase >> 9;
		*nMax = nDamageBase >> 8;
		*/
		// 数值计算方法修改（孙英要求） by Spe 03/06/11
		_ASSERT(STRENGTH_SET_DAMAGE_VALUE > 0);
		*nMin = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
		*nMax = Player[m_PlayerIdx].m_nCurStrength / STRENGTH_SET_DAMAGE_VALUE;
	}
}

BOOL KItemList::SearchPosition(int nWidth, int nHeight, ItemPos* pPos, bool bOverLookHand)
{
	if (nWidth < 0 || nHeight < 0 || NULL == pPos)
	{
		return FALSE;
	}

	POINT	pPt;
	if (!m_Room[room_equipment].FindRoom(nWidth, nHeight, &pPt))
	{
		if((Player[m_PlayerIdx].m_dwEquipExpandTime - KSG_GetCurSec() > 0) && m_Room[room_equipmentex].FindRoom(nWidth, nHeight, &pPt))
		{
			pPos->nPlace = pos_equiproomex;
			pPos->nX = pPt.x;
			pPos->nY = pPt.y;
		}
		else if (!bOverLookHand)
		{
			if (0 != m_Hand)
			{
				return FALSE;
			}
			pPos->nPlace = pos_hand;
			pPos->nX = 0;
			pPos->nY = 0;
		}
		else
			return FALSE;
	}
	else
	{
		pPos->nPlace = pos_equiproom;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
	}
	return TRUE;
}

BOOL KItemList::SearchPosition(POINT ItemSize, ItemPos* pPos)
{
	POINT	pPt;
	
	if (m_Room[room_equipment].FindRoom((int)ItemSize.x, (int)ItemSize.y, &pPt))
	{
		pPos->nPlace = pos_equiproom;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	else if((Player[m_PlayerIdx].m_dwEquipExpandTime - KSG_GetCurSec() > 0) && m_Room[room_equipmentex].FindRoom((int)ItemSize.x, (int)ItemSize.y, &pPt))
	{
		pPos->nPlace = pos_equiproomex;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	return FALSE;
}

int KItemList::Add(int nIdx, POINT ItemSize, bool bAutoStack)
{
	ItemPos pos;
//	pos.nPlace = pos_equiproom;
	if(SearchPosition(ItemSize, &pos) == false)
		return 0;

	return Add(nIdx, pos.nPlace, pos.nX, pos.nY, bAutoStack);
}
/*!*****************************************************************************
// Function		: KItemList::Add
// Purpose		: 玩家得到一个装备
// Return		: int 
// Argumant		: int nIdx		Item数组的idx
// Argumant		: int nPlace
// Argumant		: int nX
// Argumant		: int nY
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::Add(int nIdx, int nPlace, int nX, int nY, bool bAutoStack)
{
	if (nIdx <= 0)
		return 0;

	if (Item[nIdx].GetID() == 0)
		return 0;
#ifdef _SERVER
	int pnX, pnY=0;
	if (Item[nIdx].IsStack() && bAutoStack)
	{
		int pnIdx = 0;
		if (Player[m_PlayerIdx].m_ItemList.m_Room[PositionToRoom(nPlace)].FindSameItemToStack(nIdx, &pnIdx, &pnX, &pnY))
		{
			int nNum = Item[pnIdx].AddStackNum(Item[nIdx].GetStackNum());
			SyncItem(pnIdx);
			if (nNum)
			{
				Item[nIdx].SetStackNum(nNum);
				return Add(nIdx, nPlace, nX, nY, bAutoStack);
			}
			return FindSame(pnIdx);
		}
	}

	if (Item[nIdx].GetOwner() != Player[m_PlayerIdx].m_dwID)
	{
		Item[nIdx].SetOwner(Player[m_PlayerIdx].m_dwID);
		Item[nIdx].SetTradePrice(0);
	}
#endif

	int i = FindFree();
	if (!i)
		return 0;

	switch(nPlace)
	{
	case pos_hand:
		if (m_Hand)
			return 0;
		m_Items[i].nPlace = pos_hand;
		m_Items[i].nX = 0;
		m_Items[i].nY = 0;
		m_Hand = nIdx;
		break;
	case pos_equip:
		if (nX < 0 || nX >= itempart_num)
			return 0;
		if (m_EquipItem[nX])
			return 0;
//	有可能是条件不够，被其他装备凑上的。是否能装备应该放到Equip时检查
//		if (!CanEquip(nIdx, nX))
//			return 0;
		m_Items[i].nPlace = pos_equip;
		m_Items[i].nX = nX;
		m_Items[i].nY = 0;
		break;
	case pos_equiproom:
		if (!m_Room[room_equipment].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_equiproom;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
#ifndef _SERVER
	case pos_trade1:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
			return 0;
		if (!m_Room[room_trade1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_trade1;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
#endif
	case pos_repositoryroom:
		if (!m_Room[room_repository].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_repositoryroom;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_repositoryroom1:
		if (!m_Room[room_repository1].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_repositoryroom1;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_repositoryroom2:
		if (!m_Room[room_repository2].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_repositoryroom2;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_repositoryroom3:
		if (!m_Room[room_repository3].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_repositoryroom3;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_repositoryroom4:
		if (!m_Room[room_repository4].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_repositoryroom4;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_repositoryroom5:
		if (!m_Room[room_repository5].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_repositoryroom5;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_equiproomex:
		if (!m_Room[room_equipmentex].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_equiproomex;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;
		break;
	case pos_immediacy:
		if (!m_Room[room_immediacy].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_immediacy;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;	
	case pos_give:
		if (!m_Room[room_give].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_give;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	case pos_compound:
		if (nX < 0 || nX >= MAX_COMPOUND_ITEM)
			return 0;
		if (m_CompoundItem[nX])
			return 0;
		m_Items[i].nPlace = pos_compound;
		m_Items[i].nX = nX;
		m_Items[i].nY = 0;
		break;
	case pos_compoundroom:
		if (!m_Room[room_compound].PlaceItem(nX, nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
			return 0;
		m_Items[i].nPlace = pos_compoundroom;
		m_Items[i].nX = nX;
		m_Items[i].nY = nY;		
		break;
	default:
		return 0;
	}

	m_Items[i].nIdx = nIdx;
	m_FreeIdx.Remove(i);
	m_UseIdx.Insert(i);

	if (m_Items[i].nPlace == pos_equip)
	{
		Equip(m_Items[i].nIdx, nX);
	}
	if (m_Items[i].nPlace == pos_compound)
	{
		PutCompound(m_Items[i].nIdx, nX);
	}
#ifdef _SERVER
	SyncItem(nIdx, TRUE, m_Items[i].nPlace, m_Items[i].nX, m_Items[i].nY, m_PlayerIdx);
	
	Player[m_PlayerIdx].m_uMustSave = SAVE_REQUEST;
#endif

#ifndef _SERVER
	KUiObjAtContRegion	pInfo;

	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,
		UIEP_MANTLE,
		UIEP_SIGNET,
		UIEP_SHIPIN,
		UIEP_HOODS,
		UIEP_CLOAK,
	};

	int PartCompoundConvert[MAX_COMPOUND_ITEM] =
	{
		MOSAICENCRUSTED_UIEP_BOX_1,
		MOSAICENCRUSTED_UIEP_BOX_2,
		MOSAICENCRUSTED_UIEP_BOX_3,
	};

	pInfo.Obj.uGenre = CGOG_ITEM;	//源装备
	pInfo.Obj.uId = nIdx;
	pInfo.Region.Width = Item[nIdx].GetWidth();
	pInfo.Region.Height = Item[nIdx].GetHeight();

	switch(nPlace)
	{
	case pos_immediacy:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	case pos_hand:
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_equiproomex:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH_EX;
		break;
	case pos_equiproom:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		break;
	case pos_repositoryroom:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_repositoryroom1:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX1;
		break;
	case pos_repositoryroom2:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX2;
		break;
	case pos_repositoryroom3:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX3;
		break;
	case pos_repositoryroom4:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX4;
		break;
	case pos_repositoryroom5:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_STORE_BOX5;
		break;
	case pos_trade1:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		break;
	case pos_give:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_ITEM_GIVE;
		break;
	case pos_compound:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartCompoundConvert[nX];
		pInfo.eContainer = UOC_COMPOUND;
		break;
	case pos_compoundroom:
		pInfo.Region.h = nX;
		pInfo.Region.v = nY;
		pInfo.eContainer = UOC_COMPOUND_BOX;
		break;
	}
	if (nPlace != pos_trade1)
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 1);
	else
	{
		CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 1);
	}
#endif
	return i;
}

/*!*****************************************************************************
// Function		: KItemList::Remove
// Purpose		: 玩家失去一个装备
// Return		: int 
// Argumant		: int nGameIdx为游戏世界中道具数组的编号
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Remove(int nGameIdx)
{
	if (!nGameIdx)
		return FALSE;

	int nIdx = FindSame(nGameIdx);

	if (!nIdx)
		return FALSE;

#ifdef _SERVER
	ITEM_REMOVE_SYNC	sRemove;
	sRemove.ProtocolType = s2c_removeitem;
	sRemove.m_ID = Item[nGameIdx].m_dwID;
#endif

	switch(m_Items[nIdx].nPlace)
	{
	case pos_hand:
		m_Hand = 0;
		break;
	case pos_equip:
		UnEquip(m_Items[nIdx].nIdx);
		break;
	case pos_immediacy:
		m_Room[room_immediacy].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_equiproom:
		m_Room[room_equipment].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_repositoryroom:
		m_Room[room_repository].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_repositoryroom1:
		m_Room[room_repository1].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_repositoryroom2:
		m_Room[room_repository2].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_repositoryroom3:
		m_Room[room_repository3].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_repositoryroom4:
		m_Room[room_repository4].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_repositoryroom5:
		m_Room[room_repository5].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_equiproomex:
		m_Room[room_equipmentex].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_traderoom:
		m_Room[room_trade].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	case pos_give:
		m_Room[room_give].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
#ifndef _SERVER
	case pos_trade1:
		if ( !Player[CLIENT_PLAYER_INDEX].CheckTrading() )
		{
			_ASSERT(0);
		}
		m_Room[room_trade1].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
#endif
	case pos_compound:
		DropCompound(m_Items[nIdx].nIdx);
		break;
	case pos_compoundroom:
		m_Room[room_compound].PickUpItem(
			nGameIdx,
			m_Items[nIdx].nX,
			m_Items[nIdx].nY,
			Item[m_Items[nIdx].nIdx].GetWidth(),
			Item[m_Items[nIdx].nIdx].GetHeight());
		break;
	default:
		return FALSE;
	}
#ifndef _SERVER
	// 客户端从玩家身上去除装备就应该从装备表中去除掉。
	ItemSet.Remove(m_Items[nIdx].nIdx);

	// 界面处理
	KUiObjAtContRegion pInfo;

	int PartConvert[itempart_num] = 
	{
		UIEP_HEAD,
		UIEP_BODY,
		UIEP_WAIST,
		UIEP_HAND,
		UIEP_FOOT,
		UIEP_FINESSE,
		UIEP_NECK,
		UIEP_FINGER1,
		UIEP_FINGER2,
		UIEP_WAIST_DECOR,
		UIEP_HORSE,
		UIEP_MASK,
		UIEP_MANTLE,
		UIEP_SIGNET,
		UIEP_SHIPIN,
		UIEP_HOODS,
		UIEP_CLOAK,
	};

	int PartCompoundConvert[MAX_COMPOUND_ITEM] =
	{
		MOSAICENCRUSTED_UIEP_BOX_1,
		MOSAICENCRUSTED_UIEP_BOX_2,
		MOSAICENCRUSTED_UIEP_BOX_3,
	};
	pInfo.Obj.uGenre = CGOG_ITEM;
	pInfo.Obj.uId = m_Items[nIdx].nIdx;
	pInfo.Region.Width = Item[m_Items[nIdx].nIdx].GetWidth();
	pInfo.Region.Height = Item[m_Items[nIdx].nIdx].GetHeight();

	switch(m_Items[nIdx].nPlace)
	{
	case pos_hand:
		pInfo.Obj.uGenre = CGOG_NOTHING;
		pInfo.Region.h = 0;
		pInfo.Region.v = 0;
		pInfo.eContainer = UOC_IN_HAND;
		break;
	case pos_equiproom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH;
		break;
	case pos_equiproomex:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_ITEM_TAKE_WITH_EX;
		break;
	case pos_repositoryroom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX;
		break;
	case pos_repositoryroom1:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX1;
		break;
	case pos_repositoryroom2:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX2;
		break;
	case pos_repositoryroom3:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX3;
		break;
	case pos_repositoryroom4:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX4;
		break;
	case pos_repositoryroom5:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_STORE_BOX5;
		break;
	case pos_immediacy:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_IMMEDIA_ITEM;
		break;
	case pos_equip:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartConvert[m_Items[nIdx].nX];
		pInfo.eContainer = UOC_EQUIPTMENT;
		break;
	case pos_trade1:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		break;
	case pos_give:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_ITEM_GIVE;
		break;
	case pos_compound:
		pInfo.Region.h = 0;
		pInfo.Region.v = PartCompoundConvert[m_Items[nIdx].nX];
		pInfo.eContainer = UOC_COMPOUND;
		break;
	case pos_compoundroom:
		pInfo.Region.h = m_Items[nIdx].nX;
		pInfo.Region.v = m_Items[nIdx].nY;
		pInfo.eContainer = UOC_COMPOUND_BOX;
		break;
	}
	if (m_Items[nIdx].nPlace != pos_trade1)
	{
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo, 0);
	}
	else
	{
		CoreDataChanged(GDCNI_TRADE_DESIRE_ITEM, (DWORD)&pInfo, 0);
	}
#endif
	m_Items[nIdx].nIdx = 0;
	m_Items[nIdx].nPlace = 0;
	m_Items[nIdx].nX = 0;
	m_Items[nIdx].nY = 0;
	m_FreeIdx.Insert(nIdx);
	m_UseIdx.Remove(nIdx);

#ifdef _SERVER
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
#endif
	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::FindFree
// Purpose		: 查找可用空索引
// Return		: int 
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

/*!*****************************************************************************
// Function		: KItemList::FindSame
// Purpose		: 
// Return		: int 
// Argumant		: int nGameIdx
// Comments		:
// Author		: Spe
*****************************************************************************/
int KItemList::FindSame(int nGameIdx)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;

		if (m_Items[nIdx].nIdx == nGameIdx)
			return nIdx;
	}
	return 0;
}


int KItemList::FindSame(DWORD dwID)
{
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;

		if (Item[m_Items[nIdx].nIdx].GetID() == dwID)
			return nIdx;
	}
	return 0;
}
/*!*****************************************************************************
// Function		: KItemList::Init
// Purpose		: 初始化玩家装备列表
// Return		: BOOL
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Init(int nPlayerIdx)
{
	m_PlayerIdx = nPlayerIdx;
	m_Hand = 0;
	m_nBackHand = 0;
	m_bMaskLock = FALSE;
#ifndef _SERVER
	m_bLockOperation = FALSE;
#endif
	m_Room[room_equipment].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_repository].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT);
	m_Room[room_repository1].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT);
	m_Room[room_repository2].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT);
	m_Room[room_repository3].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT);
	m_Room[room_repository4].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT);
	m_Room[room_repository5].Init(REPOSITORY_ROOM_WIDTH, REPOSITORY_ROOM_HEIGHT);
	m_Room[room_equipmentex].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_trade].Init(TRADE_ROOM_WIDTH, TRADE_ROOM_HEIGHT);
	m_Room[room_tradeback].Init(EQUIPMENT_ROOM_WIDTH, EQUIPMENT_ROOM_HEIGHT);
	m_Room[room_immediacy].Init(IMMEDIACY_ROOM_WIDTH, IMMEDIACY_ROOM_HEIGHT);
	m_Room[room_give].Init(GIVE_ROOM_WIDTH, GIVE_ROOM_HEIGHT);
	m_Room[room_compound].Init(MOSAICENCRUSTED_ROOM_WIDTH, MOSAICENCRUSTED_ROOM_HEIGHT);
#ifndef _SERVER
	m_Room[room_trade1].Init(TRADE_ROOM_WIDTH, TRADE_ROOM_HEIGHT);		// 这个的大小必须与 room_trade 的大小一样
#endif
	ZeroMemory(m_EquipItem, sizeof(m_EquipItem));				// 玩家装备的道具（对应游戏世界中道具数组的索引）
	ZeroMemory(m_Items, sizeof(m_Items));						// 玩家拥有的所有道具（包括装备着的和箱子里放的，对应游戏世界中道具数组的索引）
	ZeroMemory(m_CompoundItem, sizeof(m_CompoundItem));
	m_nListCurIdx = 0;											// 用于 GetFirstItem 和 GetNextItem
	
	m_FreeIdx.Init(MAX_PLAYER_ITEM);
	m_UseIdx.Init(MAX_PLAYER_ITEM);

	for (int i = MAX_PLAYER_ITEM - 1; i > 0 ; i--)
	{
		m_FreeIdx.Insert(i);
	}
	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::CanEquip
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::CanEquip(int nIdx, int nPlace)
{
	if (m_PlayerIdx <= 0 || nIdx <= 0 || nIdx >= MAX_ITEM || Item[nIdx].GetGenre() != item_equip)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if ((Item[nIdx].GetDurability() == 0) || (nPlace != -1 && !Fit(nIdx, nPlace)))
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)Item[nIdx].GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData))
		{
			return FALSE;
		}
		nCount++;
	} while (pData != NULL);
	return TRUE;
}

BOOL KItemList::CanEquip(KItem* pItem, int nPlace /* = -1 */)
{
	if (m_PlayerIdx <= 0 || !pItem)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	KMagicAttrib* pData = NULL;

	if ((pItem->GetDurability() == 0) || (nPlace != -1 && !Fit(pItem, nPlace)))
	{
		return FALSE;
	}

	int nCount = 0;
	do
	{
		pData = (KMagicAttrib*)pItem->GetRequirement(nCount);
		if (pData && !EnoughAttrib(pData))
		{
			return FALSE;
		}
		nCount++;
	} while (pData != NULL);

	return TRUE;
}

BOOL KItemList::EnoughAttrib(void* pAttrib)
{
	KMagicAttrib*	pData = (KMagicAttrib *)pAttrib;
	_ASSERT(pData);
	switch(pData->nAttribType)
	{
	case magic_requirestr:
		if (Player[m_PlayerIdx].m_nCurStrength < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiredex:
		if (Player[m_PlayerIdx].m_nCurDexterity < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirevit:
		if (Player[m_PlayerIdx].m_nCurVitality < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requireeng:
		if (Player[m_PlayerIdx].m_nCurEngergy < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requirelevel:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Level < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiremenpai:
		if (Player[m_PlayerIdx].m_cFaction.m_nCurFaction != pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requireseries:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_Series != pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_requiresex:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_nSex != pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_require_translife:
		if (Npc[Player[m_PlayerIdx].m_nIndex].m_byTranslife < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	case magic_require_fortune_value:
		if (GetPlayerFortune() < pData->nValue[0])
		{
			return FALSE;
		}
		break;
	default:
		break;
	}
	return TRUE;
}


int KItemList::GetPlayerFortune()
{
	int nFortune = 0;
	for (int i = 0; i < itempart_num; i ++)
	{
		int nIdx = GetEquipment(i);
		nFortune += Item[nIdx].GetFortune();
	}
	return nFortune;
}

int KItemList::HaveDamageItem(int nDur)
{
	int nIndex = 0;
	for (int i = 0; i < itempart_num; i ++)
	{
		nIndex = GetEquipment(i);
		if (Item[nIndex].GetDurability() >= 0 && 
			Item[nIndex].GetMaxDurability() > 0 && 
			Item[nIndex].GetDurability() < nDur)
		{
			return nIndex;
		}
	}
	return 0;
}

/*!*****************************************************************************
// Function		: KItemList::Equip
// Purpose		: 
// Return		: BOOL
// Argumant		: int nIdx，游戏世界中的道具数组编号，要求一定是装备类道具
// Argumant		: int nPlace，身上装备的位置，-1自动找位置
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Equip(int nIdx, int nPlace /* = -1 */)
{
	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (m_PlayerIdx <= 0 || nIdx <= 0 || nNpcIdx <= 0 || item_equip != Item[nIdx].GetGenre())
		return FALSE;

	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
	{
		_ASSERT(0);
		return FALSE;
	}

	int nEquipPlace = nPlace;
	if (-1 == nEquipPlace)
	{
		nEquipPlace = GetEquipPlace(Item[nIdx].GetDetailType());
	}
	else if (!Fit(nIdx, nEquipPlace))
	{
		return FALSE;
	}
	int nCurIdx = 0;
	switch(nEquipPlace)
	{
#ifdef _SERVER
	case itempart_head:
		nCurIdx = GetEquipment(itempart_hoods);
		if (nCurIdx)
		{
			if (Item[nCurIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetGoldItemRes(Item[nCurIdx].GetDetailType(), Item[nCurIdx].GetRow());
			else
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHoodsRes(Item[nCurIdx].GetParticular());
		}
		else
		{
			if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
			else
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());			
		}
		break;
	case itempart_body:
		{
			nCurIdx = GetEquipment(itempart_cloak);
			if (nCurIdx)
			{
				if (Item[nCurIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
					Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetGoldItemRes(Item[nCurIdx].GetDetailType(), Item[nCurIdx].GetRow());
				else
					Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetCloakRes(Item[nCurIdx].GetParticular());
			}
			else
			{
				if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
					Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
				else
					Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
			}
			nCurIdx = GetEquipment(itempart_mantle);
			if (nCurIdx <= 0)
				Npc[nNpcIdx].m_MantleType = Item[nIdx].GetMantle();
		}
		break;
	case itempart_weapon:
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
			Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow(), Item[nIdx].GetFlash());
		else
			Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(Item[nIdx].GetDetailType(), Item[nIdx].GetParticular(), Item[nIdx].GetLevel(), Item[nIdx].GetFlash());
		break;
	case itempart_mask:
		if (!m_bMaskLock)
		{
			if (Npc[nNpcIdx].m_bMaskFeature)
			{
				Npc[nNpcIdx].m_MaskType = 0;
			}
			else
			{
				if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
					Npc[nNpcIdx].m_MaskType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
				else
					g_MaskChangeRes.GetInteger(Item[nIdx].GetParticular() + 2, 2, 0, &Npc[nNpcIdx].m_MaskType);
			}
		}
		break;
	case itempart_mantle:
		Npc[nNpcIdx].m_byMantleLevel = Item[nIdx].GetLevel();
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
			Npc[nNpcIdx].m_MantleType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else
			Npc[nNpcIdx].m_MantleType = 0;
		break;
	case itempart_hoods:
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHoodsRes(Item[nIdx].GetParticular());
		break;
	case itempart_cloak:
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetCloakRes(Item[nIdx].GetParticular());
		break;
#endif
	case itempart_horse:
		if (Item[nIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
			Npc[nNpcIdx].m_HorseType = g_ItemChangeRes.GetGoldItemRes(Item[nIdx].GetDetailType(), Item[nIdx].GetRow());
		else
			Npc[nNpcIdx].m_HorseType = g_ItemChangeRes.GetHorseRes(Item[nIdx].GetParticular(), Item[nIdx].GetLevel());
		break;
	default:
		break;
	}
	// 更新装备自身坐标
	m_EquipItem[nEquipPlace] = nIdx;
	m_Items[nItemListIdx].nPlace = pos_equip;
	m_Items[nItemListIdx].nX = nEquipPlace;
	m_Items[nItemListIdx].nY = 0;

	if(nEquipPlace == itempart_horse)
	{
		if (Npc[nNpcIdx].m_bRideHorse)
		{
			//do
		}
		else
		{
			Npc[nNpcIdx].m_bRideHorse = TRUE;
		}
	}

	InfectionNextEquip(nEquipPlace, TRUE);
	
	if (itempart_weapon == nEquipPlace)
	{		
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();
	}
	Player[m_PlayerIdx].UpdataCurData();
	return TRUE;
}

/*!*****************************************************************************
// Function		: KItemList::InfectionNextEquip
// Purpose		: 装备影响其他装备
// Return		: void 
// Argumant		: int nEquipPlace
// Argumant		: BOOL bEquipUp 是装上（TRUE）还是卸下（FALSE）
// Comments		:
// Author		: Spe
*****************************************************************************/
void KItemList::InfectionNextEquip(int nEquipPlace, BOOL bEquip/* = FALSE */)
{
	if (m_PlayerIdx <= 0)
		return;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	for (int i = 0; i < MAX_ITEM_ACTIVE; i++)
	{
		int nInfectionEquip = ms_ActiveEquip[nEquipPlace][i];
		if (!m_EquipItem[nInfectionEquip])
			continue; 
		if (g_IsAccrue(Item[m_EquipItem[nEquipPlace]].GetSeries(),
			Item[m_EquipItem[nInfectionEquip]].GetSeries()))
		{
			int nActive = GetEquipEnhance(nInfectionEquip);
			_ASSERT(nActive > 0);
			if (bEquip)
				Item[m_EquipItem[nInfectionEquip]].ApplyHiddenMagicAttribToNPC(&Npc[nNpcIdx], nActive);
			else
				Item[m_EquipItem[nInfectionEquip]].RemoveHiddenMagicAttribFromNPC(&Npc[nNpcIdx], nActive);
		}
	}
}
/*!*****************************************************************************
// Function		: KItemList::UnEquip
// Purpose		: 移除装备
// Return		: BOOL 
// Argumant		: int nIdx 游戏世界中的道具数组索引
// Comments		: 
// Author		: Spe
*****************************************************************************/
BOOL KItemList::UnEquip(int nIdx, int nPos/* = -1*/)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	if (nIdx <= 0)
		return FALSE;

	if (nPos <= 0)
	{
		for (i = 0; i < itempart_num; i++)
		{
			if (m_EquipItem[i] == nIdx)
			{
				break;
			}
		}
		
		if (i == itempart_num)
			return FALSE;

	}
	else
	{
		if (m_EquipItem[nPos] != nIdx)	
			return FALSE;
		i = nPos;
	}

	if(i == itempart_horse)
	{
		if (Npc[nNpcIdx].m_bRideHorse)
			Npc[nNpcIdx].m_bRideHorse = FALSE;
	}
	InfectionNextEquip(i, FALSE);

	m_EquipItem[i] = 0;	
	int nCurIdx = 0;
	switch(i)
	{
#ifdef _SERVER
	case itempart_head:
		nCurIdx = GetEquipment(itempart_hoods);
		if (nCurIdx)
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHoodsRes(Item[nCurIdx].GetParticular());
		else
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
		break;
	case itempart_body:
		nCurIdx = GetEquipment(itempart_mantle);
		if (nCurIdx <= 0)
			Npc[nNpcIdx].m_MantleType = 0;

		nCurIdx = GetEquipment(itempart_cloak);
		if (nCurIdx)
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetCloakRes(Item[nCurIdx].GetParticular());
		else
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
		break;
	case itempart_weapon:
		Npc[nNpcIdx].m_WeaponType = g_ItemChangeRes.GetWeaponRes(0, 0, 0, 0);
		break;
	case itempart_mask:
		if (!m_bMaskLock)
			Npc[nNpcIdx].m_MaskType = 0;
		break;
	case itempart_mantle:
		Npc[nNpcIdx].m_byMantleLevel = 0;
		nCurIdx = GetEquipment(itempart_body);
		if (Item[nCurIdx].GetMantle() > 0)
			Npc[nNpcIdx].m_MantleType = Item[nCurIdx].GetMantle();
		else
			Npc[nNpcIdx].m_MantleType = 0;
		break;
	case itempart_cloak:
		nCurIdx = GetEquipment(itempart_body);
		if (nCurIdx)
		{
			if (Item[nCurIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
				Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetGoldItemRes(Item[nCurIdx].GetDetailType(), Item[nCurIdx].GetRow());
			else
				Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(Item[nCurIdx].GetParticular(), Item[nCurIdx].GetLevel());
		}
		else
			Npc[nNpcIdx].m_ArmorType = g_ItemChangeRes.GetArmorRes(0, 0);
		break;
	case itempart_hoods:
		nCurIdx = GetEquipment(itempart_head);
		if (nCurIdx)
		{
			if (Item[nCurIdx].m_CommonAttrib.nItemNature == NATURE_GOLD)
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetGoldItemRes(Item[nCurIdx].GetDetailType(), Item[nCurIdx].GetRow());
			else
				Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(Item[nCurIdx].GetParticular(), Item[nCurIdx].GetLevel());
		}
		else
			Npc[nNpcIdx].m_HelmType = g_ItemChangeRes.GetHelmRes(0, 0);
		break;
#endif
	case itempart_horse:
		Npc[nNpcIdx].m_HorseType = -1;
		break;
	default:
		break;
	}

	if (itempart_weapon == i)
	{
#ifndef _SERVER
		Player[CLIENT_PLAYER_INDEX].UpdateWeaponSkill();
#endif
		Player[m_PlayerIdx].SetNpcDamageAttrib();
	}

	Player[m_PlayerIdx].UpdataCurData();
	return TRUE;
}

int KItemList::GetEquipPlace(int nType)
{
	int nRet = -1;
	switch(nType)
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		nRet = itempart_weapon;
		break;
	case equip_armor:
		nRet = itempart_body;
		break;
	case equip_helm:
		nRet = itempart_head;
		break;
	case equip_boots:
		nRet = itempart_foot;
		break;
	case equip_ring:
		nRet = itempart_ring1;
		break;
	case equip_amulet:
		nRet = itempart_amulet;
		break;
	case equip_belt:
		nRet = itempart_belt;
		break;
	case equip_cuff:
		nRet = itempart_cuff;
		break;
	case equip_pendant:
		nRet = itempart_pendant;
		break;
	case equip_horse:
		nRet = itempart_horse;
		break;
	case equip_mask:
		nRet = itempart_mask;
		break;
	case equip_mantle:
		nRet = itempart_mantle;
		break;
	case equip_signet:
		nRet = itempart_signet;
		break;
	case equip_shipin:
		nRet = itempart_shipin;
		break;
	case equip_hoods:
		nRet = itempart_hoods;
		break;
	case equip_cloak:
		nRet = itempart_cloak;
		break;
	default:
		break;
	}
	return nRet;
}

/*!*****************************************************************************
// Function		: KItemList::Fit
// Purpose		: 
// Return		: BOOL 
// Argumant		: int nIdx
// Argumant		: int nPlace
// Comments		:
// Author		: Spe
*****************************************************************************/
BOOL KItemList::Fit(int nIdx, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(Item[nIdx].GetGenre() == item_equip);
	switch(Item[nIdx].GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_mantle:
		if (nPlace == itempart_mantle)
			bRet = TRUE;
		break;
	case equip_signet:
		if (nPlace == itempart_signet)
			bRet = TRUE;
		break;
	case equip_shipin:
		if (nPlace == itempart_shipin)
			bRet = TRUE;
		break;
	case equip_hoods:
		if (nPlace == itempart_hoods)
			bRet = TRUE;
		break;
	case equip_cloak:
		if (nPlace == itempart_cloak)
			bRet = TRUE;
		break;
	}
	return bRet;
}

BOOL KItemList::Fit(KItem* pItem, int nPlace)
{
	BOOL	bRet = FALSE;
	_ASSERT(pItem->GetGenre() == item_equip);
	switch(pItem->GetDetailType())
	{
	case equip_meleeweapon:
	case equip_rangeweapon:
		if (nPlace == itempart_weapon)
			bRet = TRUE;
		break;
	case equip_armor:
		if (nPlace == itempart_body)
			bRet = TRUE;
		break;
	case equip_belt:
		if (nPlace == itempart_belt)
			bRet = TRUE;
		break;
	case equip_boots:
		if (nPlace == itempart_foot)
			bRet = TRUE;
		break;
	case equip_cuff:
		if (nPlace == itempart_cuff)
			bRet = TRUE;
		break;
	case equip_ring:
		if (nPlace == itempart_ring1 || nPlace == itempart_ring2)
			bRet = TRUE;
		break;
	case equip_amulet:
		if (nPlace == itempart_amulet)
			bRet = TRUE;
		break;
	case equip_pendant:
		if (nPlace == itempart_pendant)
			bRet = TRUE;
		break;
	case equip_helm:
		if (nPlace == itempart_head)
			bRet = TRUE;
		break;
	case equip_horse:
		if (nPlace == itempart_horse)
			bRet = TRUE;
		break;
	case equip_mask:
		if (nPlace == itempart_mask)
			bRet = TRUE;
		break;
	case equip_mantle:
		if (nPlace == itempart_mantle)
			bRet = TRUE;
		break;
	case equip_signet:
		if (nPlace == itempart_signet)
			bRet = TRUE;
		break;
	case equip_shipin:
		if (nPlace == itempart_shipin)
			bRet = TRUE;
		break;
	case equip_hoods:
		if (nPlace == itempart_hoods)
			bRet = TRUE;
		break;
	case equip_cloak:
		if (nPlace == itempart_cloak)
			bRet = TRUE;
		break;
	}
	return bRet;
}

int KItemList::GetEquipEnhance(int nPlace)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (nPlace < 0 || nPlace >= itempart_num)
	{
		_ASSERT(0);
		return 0;
	}
	int nRet = 0, nCnt =0, nNum=1;

	int nSeries = Item[m_EquipItem[nPlace]].GetSeries();
	if (g_IsAccrue(Npc[nNpcIdx].m_Series, nSeries))
		nRet ++;
	for (int i = 0; i < MAX_ITEM_ACTIVE; i++)
	{
		// 看看激活这个装备的装备中是否相生这个装备
		int nActivedEquip = m_EquipItem[ms_ActivedEquip[nPlace][i]];
		if (nActivedEquip)
		{
			if (g_IsAccrue(Item[nActivedEquip].GetSeries(), nSeries))
				nRet++;
		}
	}
	if (nPlace >= itempart_horse || IsEnoughToActive())
	{
		nRet = MAX_ITEM_MAGICATTRIB / 2;
	}
	return nRet;
}


int KItemList::GetGoldEquipEnhance(int nPlace)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (nPlace < 0 || nPlace >= itempart_num)
	{
		_ASSERT(0);
		return 0;
	}
	int nRet = 0, nCnt =0, nNum=1;

	if (nPlace >= itempart_horse || IsEnoughToActive())
	{
		nRet = MAX_ITEM_MAGICATTRIB - MAX_ITEM_NORMAL_MAGICATTRIB;
	}
	else
	{
		if (Item[m_EquipItem[nPlace]].m_CommonAttrib.nItemNature >= NATURE_GOLD)
		{
			nNum = Item[m_EquipItem[nPlace]].GetNeedToActive1();
			int nGroup = Item[m_EquipItem[nPlace]].GetGroup();
			int nSetID = Item[m_EquipItem[nPlace]].GetSetID();

			for (int i = 0;i < itempart_horse; i++)
			{
				if (m_EquipItem[i] && Item[m_EquipItem[i]].m_CommonAttrib.nItemNature >= NATURE_GOLD)
				{
					if(i == itempart_ring1)
					{
						if (m_EquipItem[itempart_ring1] && 
							m_EquipItem[itempart_ring2] && 
							Item[m_EquipItem[itempart_ring1]].GetGroup() == nGroup && 
							Item[m_EquipItem[itempart_ring1]].GetSetID() == nSetID)
							continue;
					}
					if (Item[m_EquipItem[i]].GetGroup() == nGroup)
						nCnt++;
				}
			}
		}
		nRet = (nCnt/nNum);
	}

	return nRet;
}

BOOL KItemList::NowEatItem(int nIdx)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;
	int nItemGenre = Item[nIdx].GetGenre();
	int	nDetailType = Item[nIdx].GetDetailType();

	_ASSERT(nItemGenre == item_equip || 
			nItemGenre == item_medicine || 
			nItemGenre == item_event || 
			nItemGenre == item_materials || 
			nItemGenre == item_task || 
			nItemGenre == item_townportal || 
			nItemGenre == item_magicscript);

	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendCommand(do_stand);
	}
	if (nItemGenre == item_equip)
		return TRUE;

	else if (nItemGenre == item_medicine)
		Item[nIdx].ApplyMagicAttribToNPC(&Npc[nNpcIdx], MAX_ITEM_MAGICATTRIB / 2);
	else
	{
		if (Player[m_PlayerIdx].GetLockMove()->bLock)
		{
			if (Player[m_PlayerIdx].GetLockMove()->nPlace <= 0)
				return FALSE;
		}
#ifdef _SERVER
		ExecuteScript(nIdx);
#endif
		return TRUE;
	}
#ifdef _SERVER
	RemoveItem(nIdx, 1);
#endif
	return TRUE;
}

#ifndef _SERVER
int KItemList::UseItem(int nIdx)
{
	if (m_PlayerIdx <= 0)
		return FALSE;

	int nNpcIdx = Player[m_PlayerIdx].m_nIndex;

	if (0 == FindSame(nIdx))
		return 0;

	int		nRet = 0;
	switch(Item[nIdx].GetGenre())
	{
	case item_medicine:
		if (NowEatItem(nIdx))
			nRet = REQUEST_EAT_MEDICINE;
		break;
	default:
		nRet = REQUEST_EAT_OTHER;
		break;
	}
	return nRet;
}
#endif

#ifndef _SERVER
BOOL KItemList::SearchEquipment(int nWidth, int nHeight)
{
	if (nWidth < 0 || nHeight < 0)
	{
		return FALSE;
	}

	POINT	pPt;
	if (!m_Room[room_equipment].FindRoom(nWidth, nHeight, &pPt))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL KItemList::SearchStoreBox(int nRepositoryNum, int nWidth, int nHeight, ItemPos* pPos)
{
	if (nWidth < 0 || nHeight < 0 || NULL == pPos)
	{
		return FALSE;
	}

	POINT	pPt;
	if (m_Room[room_repository].FindRoom(nWidth, nHeight, &pPt))
	{
		pPos->nPlace = pos_repositoryroom;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	if (nRepositoryNum > REPOSITORY_ONE && m_Room[room_repository1].FindRoom(nWidth, nHeight, &pPt))
	{
		pPos->nPlace = pos_repositoryroom1;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	if (nRepositoryNum > REPOSITORY_TWO && m_Room[room_repository2].FindRoom(nWidth, nHeight, &pPt))
	{
		pPos->nPlace = pos_repositoryroom2;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	if (nRepositoryNum > REPOSITORY_THREE && m_Room[room_repository3].FindRoom(nWidth, nHeight, &pPt))
	{
		pPos->nPlace = pos_repositoryroom3;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	if (nRepositoryNum > REPOSITORY_FOUR && m_Room[room_repository4].FindRoom(nWidth, nHeight, &pPt))
	{
		pPos->nPlace = pos_repositoryroom4;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	if (nRepositoryNum > REPOSITORY_FIVE && m_Room[room_repository5].FindRoom(nWidth, nHeight, &pPt))
	{
		pPos->nPlace = pos_repositoryroom5;
		pPos->nX = pPt.x;
		pPos->nY = pPt.y;
		return TRUE;
	}
	return FALSE;
}
#endif

int	KItemList::SearchID(int nID)
{
	if (m_PlayerIdx <= 0)
		return 0;
	int nIdx = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (!nIdx)
			break;
		if (Item[m_Items[nIdx].nIdx].GetID() == (DWORD)nID)
			return m_Items[nIdx].nIdx;
	}
	return 0;
}

void KItemList::ExchangeMoney(int pos1, int pos2, int nMoney)
{
	if (pos1 < 0 || pos2 < 0 || pos1 > room_trade || pos2 > room_trade)
		return;

	if(Npc[Player[m_PlayerIdx].m_nIndex].m_FightMode)
		return;

#ifdef _SERVER
	if (m_Room[pos1].AddMoney(-nMoney))		// 源位置能拿出这么多钱来
	{
		if (!m_Room[pos2].AddMoney(nMoney))	// 目的地能放不下去
		{
			m_Room[pos1].AddMoney(nMoney);	// 还原源位置的钱
		}
	}
	else
	{
		return;
	}
#endif

#ifndef _SERVER
	if (pos1 == room_equipment && pos2 == room_repository)
		SendClientCmdStoreMoney(0, nMoney);
	else if (pos1 == room_repository && pos2 == room_equipment)
		SendClientCmdStoreMoney(1, nMoney);
#endif
#ifdef _SERVER
	SendMoneySync();
#endif	
}

//----------------------------------------------------------------------
//	功能：得到物品栏和储物箱的总钱数
//----------------------------------------------------------------------
int KItemList::GetMoneyAmount()
{
	return (m_Room[room_equipment].GetMoney() + m_Room[room_repository].GetMoney());
}

//----------------------------------------------------------------------
//	功能：得到物品栏和储物箱的钱数
//----------------------------------------------------------------------
int KItemList::GetRepositoryMoney()
{
	return m_Room[room_repository].GetMoney();
}

int KItemList::GetEquipmentMoney()
{
	return m_Room[room_equipment].GetMoney();
}

int KItemList::GetTradeMoney()
{
	return m_Room[room_trade].GetMoney();
}

BOOL KItemList::AddMoney(int nRoom, int nMoney)
{
	if (nRoom < 0 || nRoom >= room_num)
		return FALSE;

	if (!m_Room[nRoom].AddMoney(nMoney))
		return FALSE;

#ifdef _SERVER
	SendMoneySync();
#endif

	return TRUE;
}

BOOL KItemList::CostMoney(int nMoney)
{
	if (nMoney > GetEquipmentMoney())
		return FALSE;

	if ( !m_Room[room_equipment].AddMoney(-nMoney) )
		return FALSE;

#ifdef _SERVER
	SendMoneySync();
#endif

	return TRUE;
}

BOOL KItemList::DecMoney(int nMoney)
{
	if (nMoney < 0)
		return FALSE;

	if (nMoney > m_Room[room_equipment].GetMoney())
	{
		nMoney -= m_Room[room_equipment].GetMoney();
		SetRoomMoney(room_equipment, 0);
		if (nMoney > m_Room[room_repository].GetMoney())
			SetRoomMoney(room_repository, 0);
		else
			AddMoney(room_repository, -nMoney);
	}
	else
	{
		AddMoney(room_equipment, -nMoney);
	}

#ifdef _SERVER
	SendMoneySync();
#endif

	return TRUE;
}

#ifdef _SERVER
//----------------------------------------------------------------------------------
//	功能：调用此接口必须保证传入的nMoney是一个有效数(正数且不超过所有钱数)
//----------------------------------------------------------------------------------
void	KItemList::TradeMoveMoney(int nMoney)
{
	// 自己钱的处理
	m_Room[room_trade].SetMoney(nMoney);
	SendMoneySync();
	char szMsg[128];
	sprintf(szMsg, MSG_TRADE_INPUT_MONEY, nMoney);
	KPlayerChat::SendSystemInfo(1, Player[m_PlayerIdx].m_cTrade.m_nTradeDest, MESSAGE_SYSTEM_ANNOUCE_HEAD, (char *) szMsg, strlen(szMsg) );	
	// 给对方发消息
	TRADE_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_trademoneysync;
	sMoney.m_nMoney = nMoney;
	g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(TRADE_MONEY_SYNC));
}
#endif

#ifdef _SERVER
//----------------------------------------------------------------------------------
//	功能：服务器发money同步信息给客户端
//----------------------------------------------------------------------------------
void	KItemList::SendMoneySync()
{
	PLAYER_MONEY_SYNC	sMoney;
	sMoney.ProtocolType = s2c_syncmoney;
	sMoney.m_nMoney1 = m_Room[room_equipment].GetMoney();
	sMoney.m_nMoney2 = m_Room[room_repository].GetMoney();
	sMoney.m_nMoney3 = m_Room[room_trade].GetMoney();
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMoney, sizeof(PLAYER_MONEY_SYNC));
}
#endif

void KItemList::SetMoney(int nMoney1, int nMoney2, int nMoney3)
{
	m_Room[room_equipment].SetMoney(nMoney1);
	m_Room[room_repository].SetMoney(nMoney2);
	m_Room[room_trade].SetMoney(nMoney3);
#ifndef _SERVER
	KUiObjAtContRegion	sMoney;
	sMoney.Obj.uGenre = CGOG_MONEY;
	sMoney.Obj.uId = nMoney2;
	sMoney.eContainer = UOC_STORE_BOX;
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sMoney, 1);
#endif
}

void KItemList::SetRoomMoney(int nRoom, int nMoney)
{
	if (nRoom >= 0 && nRoom < room_num)
		m_Room[nRoom].SetMoney(nMoney);
}

void KItemList::ExchangeItem(ItemPos* SrcPos, ItemPos* DesPos)
{
#ifdef _SERVER
	if(Player[m_PlayerIdx].m_PTrade.nTrade)
#else
	if(Npc[Player[m_PlayerIdx].m_nIndex].m_PTrade.nTrade)
#endif
		return;

	if (SrcPos->nPlace != DesPos->nPlace)
		return;

	int nTempHand = m_Hand;
	int	nEquipIdx1 = 0;
	
#ifdef _SERVER
	PLAYER_MOVE_ITEM_SYNC	sMove;
	sMove.ProtocolType = s2c_playermoveitem;
	sMove.m_btDownPos = SrcPos->nPlace;
	sMove.m_btDownX = SrcPos->nX;
	sMove.m_btDownY = SrcPos->nY;
	sMove.m_btUpPos = DesPos->nPlace;
	sMove.m_btUpX = DesPos->nX;
	sMove.m_btUpY = DesPos->nY;
#endif

	// 源装备从SrcPos到手上，手上的装备到DesPos
	if (Player[m_PlayerIdx].GetLockMove()->bLock)
	{
		if (DesPos->nPlace != pos_equiproom && 
			Player[m_PlayerIdx].GetLockMove()->nPlace >= pos_hand && 
			Player[m_PlayerIdx].GetLockMove()->nPlace < pos_num && 
			DesPos->nPlace != Player[m_PlayerIdx].GetLockMove()->nPlace)
			return;
	}
	if(Npc[Player[m_PlayerIdx].m_nIndex].m_FightMode && DesPos->nPlace >= pos_repositoryroom && DesPos->nPlace <= pos_repositoryroom5)
		return;

	switch(SrcPos->nPlace)
	{
	case pos_hand:
		g_DebugLog("%s exchange item error", Npc[Player[m_PlayerIdx].m_nIndex].Name);
		return;
		break;
	case pos_equip:
		if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (SrcPos->nX < 0 || SrcPos->nX >= itempart_num || DesPos->nX < 0 || DesPos->nX >= itempart_num)
			return;
		nEquipIdx1 = m_EquipItem[SrcPos->nX];
		if (nEquipIdx1)
		{
			UnEquip(nEquipIdx1, SrcPos->nX);
		}
		if (m_Hand)
		{
			if (CanEquip(m_Hand, DesPos->nX))
			{
				Equip(m_Hand, DesPos->nX);
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else if (nEquipIdx1)
			{
				Equip(nEquipIdx1, SrcPos->nX);
			}
		}
		else
		{
			m_Hand = nEquipIdx1;
			m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_immediacy:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		// 判断是否有同类型的物品，有的话不让放下去
		if (m_Hand)
		{
			if (Item[m_Hand].CanShortKey())
			{
				if (m_Room[room_immediacy].CheckSameImmediacyItem(m_Hand))
				{
#ifdef _SERVER
					SHOW_MSG_SYNC	sMsg;
					sMsg.ProtocolType = s2c_msgshow;
					sMsg.m_wMsgID = enumMSG_ID_ITEM_SAME_IMMEDIATE;
					sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
					if(g_pServer && Player[m_PlayerIdx].m_nNetConnectIdx != -1)
						g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
#endif
					return;
				}
			}
			else
			{
#ifdef _SERVER
				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_ITEM_CANTPUT_IMMEDIATE;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID);
				if(g_pServer && Player[m_PlayerIdx].m_nNetConnectIdx != -1)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
#endif
				return;
			}

		}
		nEquipIdx1 = m_Room[room_immediacy].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;
		
		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_immediacy].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
	if (m_Hand)
		{
			if (m_Room[room_immediacy].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_immediacy;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_immediacy].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_repositoryroom:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_repository].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_repository].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_repositoryroom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_repository].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	//====================================================================================
	case pos_repositoryroom1:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (m_Hand && Player[m_PlayerIdx].m_btRepositoryNum <= REPOSITORY_ONE)
			return;
		nEquipIdx1 = m_Room[room_repository1].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository1].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_repository1].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_repository1].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_repositoryroom1;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_repository1].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
//---------------------------------------------------
	case pos_repositoryroom2:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (m_Hand && Player[m_PlayerIdx].m_btRepositoryNum <= REPOSITORY_TWO)
			return;
		nEquipIdx1 = m_Room[room_repository2].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository2].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_repository2].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_repository2].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_repositoryroom2;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_repository2].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
//----------------------------------------------------------------------
	case pos_repositoryroom3:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (m_Hand && Player[m_PlayerIdx].m_btRepositoryNum <= REPOSITORY_THREE)
			return;
		nEquipIdx1 = m_Room[room_repository3].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository3].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_repository3].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_repository3].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_repositoryroom3;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_repository3].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	//====================================================================================
	case pos_repositoryroom4:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (m_Hand && Player[m_PlayerIdx].m_btRepositoryNum <= REPOSITORY_FOUR)
			return;
		nEquipIdx1 = m_Room[room_repository4].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository4].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_repository4].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_repository4].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_repositoryroom4;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_repository4].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	//====================================================================================
	case pos_repositoryroom5:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (m_Hand && Player[m_PlayerIdx].m_btRepositoryNum <= REPOSITORY_FIVE)
			return;
		nEquipIdx1 = m_Room[room_repository5].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_repository5].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_repository5].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_repository5].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_repositoryroom5;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_repository5].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
//--------------------------------------------------------------------
	case pos_equiproomex:
		if (Player[m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (m_Hand && Player[m_PlayerIdx].m_dwEquipExpandTime <= KSG_GetCurSec())
			return;
		nEquipIdx1 = m_Room[room_equipmentex].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_equipmentex].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_equipmentex].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_equipmentex].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_equiproomex;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_equipmentex].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	//--------------------------------------------------------------------------
	case pos_equiproom:
		nEquipIdx1 = m_Room[room_equipment].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_equipment].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}

		if (m_Hand)
		{
			if (CanCombie(nEquipIdx1, m_Hand))
			{
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				int nNum = Item[nEquipIdx1].AddStackNum(Item[m_Hand].GetStackNum());
				if (nNum)
					Item[m_Hand].SetStackNum(nNum);
				else
				{
					Remove(m_Hand);
					ItemSet.Remove(m_Hand);
				}
				m_Room[room_equipment].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
				
				return;				
			}
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_equipment].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				if (Player[m_PlayerIdx].GetLockMove()->bLock)
				{
					if (Item[m_Hand].GetBackLocal()->nPlace < pos_hand || 
						Item[m_Hand].GetBackLocal()->nPlace >= pos_num)
						return;

					Item[m_Hand].GetBackLocal()->Release();
					int nListIdx = FindSame(nEquipIdx1);
					Item[nEquipIdx1].SetBackLocal(&m_Items[nListIdx]);
				}
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_equiproom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				if (Player[m_PlayerIdx].GetLockMove()->bLock)
				{
					if (Item[m_Hand].GetBackLocal()->nPlace < pos_hand || 
						Item[m_Hand].GetBackLocal()->nPlace >= pos_num)
						return;
				}
				m_Room[room_equipment].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				if (Player[m_PlayerIdx].GetLockMove()->bLock)
					Item[nEquipIdx1].SetBackLocal(&m_Items[nListIdx]);			
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_traderoom:
		if ( !Player[m_PlayerIdx].CheckTrading() )	// 不在交易
			return;
		nEquipIdx1 = m_Room[room_trade].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_trade].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		//char szMsg[128];
		if (m_Hand)
		{
			if (m_Room[room_trade].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_traderoom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
#ifdef _SERVER
				// 发消息给交易对方
				if (nEquipIdx1)	// 交易栏有东西被捡起
				{
					ITEM_REMOVE_SYNC	sRemove;
					sRemove.ProtocolType = s2c_removeitem;
					sRemove.m_ID = Item[nEquipIdx1].m_dwID;
					g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
				}
				// 手上的东西放在了交易栏
				SyncItem(m_Hand, TRUE, pos_trade1, DesPos->nX, DesPos->nY, Player[m_PlayerIdx].m_cTrade.m_nTradeDest);
				
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
				m_Hand = nEquipIdx1;
				if (FindSame(nEquipIdx1))
					m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_trade].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
#ifdef _SERVER
			// 发消息给交易对方
			if (nEquipIdx1)	// 交易栏有东西被捡起
			{
				ITEM_REMOVE_SYNC	sRemove;
				sRemove.ProtocolType = s2c_removeitem;
				sRemove.m_ID = Item[nEquipIdx1].m_dwID;
				g_pServer->PackDataToClient(Player[Player[m_PlayerIdx].m_cTrade.m_nTradeDest].m_nNetConnectIdx, (BYTE*)&sRemove, sizeof(ITEM_REMOVE_SYNC));
			}
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
		}
		break;
	case pos_give:
		nEquipIdx1 = m_Room[room_give].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_give].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_give].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_give;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_give].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}

#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	case pos_compound:
		if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		if (SrcPos->nX < 0 || SrcPos->nX >= MAX_COMPOUND_ITEM || DesPos->nX < 0 || DesPos->nX >= MAX_COMPOUND_ITEM)
			return;

		nEquipIdx1 = m_CompoundItem[SrcPos->nX];
		if (m_Hand)
		{
		    if(PutCompound(m_Hand, DesPos->nX) == TRUE)
			{
				if (nEquipIdx1)
				{
					DropCompound(nEquipIdx1, SrcPos->nX);
				}
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			else if (nEquipIdx1)
			{
				PutCompound(nEquipIdx1, SrcPos->nX);
			}	
		}
		else
		{
			if (nEquipIdx1)
			{
				DropCompound(nEquipIdx1, SrcPos->nX);
			}
			m_Hand = nEquipIdx1;
			m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif		
			
		}
		break;
	case pos_compoundroom:
		if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
			return;
		nEquipIdx1 = m_Room[room_compound].FindItem(SrcPos->nX, SrcPos->nY);
		if (nEquipIdx1 < 0)
			return;

		// 先把东西拣起来
		if (nEquipIdx1)
		{
			if (!m_Room[room_compound].PickUpItem(nEquipIdx1, SrcPos->nX, SrcPos->nY, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight()))
				return;
		}
		// 如果鼠标上有东西，先试试能不能把他放下去，如果不行的话，还得把原来的东西放回去
		if (m_Hand)
		{
			if (m_Room[room_compound].PlaceItem(DesPos->nX, DesPos->nY, m_Hand, Item[m_Hand].GetWidth(), Item[m_Hand].GetHeight()))
			{
				int nListIdx = FindSame(m_Hand);
				m_Items[nListIdx].nPlace = pos_compoundroom;
				m_Items[nListIdx].nX = DesPos->nX;
				m_Items[nListIdx].nY = DesPos->nY;
				m_Hand = nEquipIdx1;
				m_Items[FindSame(nEquipIdx1)].nPlace = pos_hand;
#ifdef _SERVER
				g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
			}
			// 把原来的东西放回去
			else
			{
				m_Room[room_compound].PlaceItem(SrcPos->nX, SrcPos->nY, nEquipIdx1, Item[nEquipIdx1].GetWidth(), Item[nEquipIdx1].GetHeight());
			}
		}
		// 如果鼠标上没有东西，只需要把拣起来的东西放到鼠标上
		else
		{
			int nListIdx = FindSame(nEquipIdx1);
			if (nEquipIdx1 && nListIdx)
			{
				m_Items[nListIdx].nPlace = pos_hand;
				m_Hand = nEquipIdx1;
			}
#ifdef _SERVER
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(PLAYER_MOVE_ITEM_SYNC));
#endif
		}
		break;
	}


#ifndef _SERVER
	// Notify to Menu
	if (m_Hand != nTempHand)	// 交换成功了
	{
	//uParam = (KUiObjAtRegion*)pInfo -> 物品数据及其放置区域位置信息
	//nParam = bAdd -> 0值表示减少这个物品，非0值表示增加这个物品
		KUiObjAtContRegion pInfo1, pInfo2;
		if (nTempHand == 0)
		{
			pInfo2.Obj.uGenre = CGOG_NOTHING;
			pInfo2.Obj.uId = 0;
			pInfo2.Region.Width = 0;
			pInfo2.Region.Height = 0;
		}
		else
		{
			pInfo2.Obj.uGenre = CGOG_ITEM;	//源装备
			pInfo2.Obj.uId = nTempHand;
			pInfo2.Region.Width = Item[nTempHand].GetWidth();
			pInfo2.Region.Height = Item[nTempHand].GetHeight();
		}
		
		if (m_Hand == 0)
		{
			pInfo1.Obj.uGenre = CGOG_NOTHING;
			pInfo1.Obj.uId = 0;
			pInfo1.Region.Width = 0;
			pInfo1.Region.Height = 0;
		}
		else
		{
			pInfo1.Obj.uGenre = CGOG_ITEM;
			pInfo1.Obj.uId = m_Hand;
			pInfo1.Region.Width = Item[m_Hand].GetWidth();
			pInfo1.Region.Height = Item[m_Hand].GetHeight();
		}

		int PartConvert[itempart_num] = 
		{
			UIEP_HEAD,
			UIEP_BODY,
			UIEP_WAIST,
			UIEP_HAND,
			UIEP_FOOT,
			UIEP_FINESSE,
			UIEP_NECK,
			UIEP_FINGER1,
			UIEP_FINGER2,
			UIEP_WAIST_DECOR,
			UIEP_HORSE,
			UIEP_MASK,
			UIEP_MANTLE,
			UIEP_SIGNET,
			UIEP_SHIPIN,
			UIEP_HOODS,
			UIEP_CLOAK,
		};

		int PartCompoundConvert[MAX_COMPOUND_ITEM] =
		{
			MOSAICENCRUSTED_UIEP_BOX_1,
			MOSAICENCRUSTED_UIEP_BOX_2,
			MOSAICENCRUSTED_UIEP_BOX_3,
		};
		switch(SrcPos->nPlace)
		{
		case pos_immediacy:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_IMMEDIA_ITEM;
			pInfo2.eContainer = UOC_IMMEDIA_ITEM;
			break;
		case pos_equiproom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_TAKE_WITH;
			pInfo2.eContainer = UOC_ITEM_TAKE_WITH;
			break;
		case pos_equiproomex:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_TAKE_WITH_EX;
			pInfo2.eContainer = UOC_ITEM_TAKE_WITH_EX;
			break;
		case pos_equip:
			pInfo1.Region.h = 0;
			pInfo1.Region.v = PartConvert[SrcPos->nX];
			pInfo2.Region.h = 0;
			pInfo2.Region.v = PartConvert[DesPos->nX];
			pInfo1.eContainer = UOC_EQUIPTMENT;
			pInfo2.eContainer = UOC_EQUIPTMENT;
			break;
		case pos_repositoryroom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX;
			pInfo2.eContainer = UOC_STORE_BOX;
			break;
		case pos_repositoryroom1:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX1;
			pInfo2.eContainer = UOC_STORE_BOX1;
			break;
		case pos_repositoryroom2:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX2;
			pInfo2.eContainer = UOC_STORE_BOX2;
			break;
		case pos_repositoryroom3:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX3;
			pInfo2.eContainer = UOC_STORE_BOX3;
			break;
		case pos_repositoryroom4:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX4;
			pInfo2.eContainer = UOC_STORE_BOX4;
			break;
		case pos_repositoryroom5:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_STORE_BOX5;
			pInfo2.eContainer = UOC_STORE_BOX5;
			break;
		case pos_traderoom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_TO_BE_TRADE;
			pInfo2.eContainer = UOC_TO_BE_TRADE;
			break;
		case pos_give:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_ITEM_GIVE;
			pInfo2.eContainer = UOC_ITEM_GIVE;
			break;
		case pos_compound:
			pInfo1.Region.h = 0;
			pInfo1.Region.v = PartCompoundConvert[SrcPos->nX];
			pInfo2.Region.h = 0;
			pInfo2.Region.v = PartCompoundConvert[DesPos->nX];
			pInfo1.eContainer = UOC_COMPOUND;
			pInfo2.eContainer = UOC_COMPOUND;
			break;
		case pos_compoundroom:
			pInfo1.Region.h = SrcPos->nX;
			pInfo1.Region.v = SrcPos->nY;
			pInfo2.Region.h = DesPos->nX;
			pInfo2.Region.v = DesPos->nY;
			pInfo1.eContainer = UOC_COMPOUND_BOX;
			pInfo2.eContainer = UOC_COMPOUND_BOX;
			break;
		}
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo1, 0);
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo2, 0);
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&pInfo1, 1);
		CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&pInfo2, 1);
	}
#endif
	if (Npc[Player[m_PlayerIdx].m_nIndex].m_Doing == do_sit)
	{
		Npc[Player[m_PlayerIdx].m_nIndex].SendCommand(do_stand);
	}
}

#ifndef	_SERVER
//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
BOOL	KItemList::AutoMoveItem(ItemPos SrcPos,ItemPos DesPos)
{
	if (Player[this->m_PlayerIdx].CheckTrading())	// 如果正在交易
		return FALSE;

	BOOL	bMove = FALSE;
	int		nIdx, nListIdx;

	// 目前只支持从room_equipment到room_immediacy
	switch (SrcPos.nPlace)
	{
	case pos_equiproom:
		{
			switch (DesPos.nPlace)
			{
			case pos_immediacy:
				{
					nIdx = m_Room[room_equipment].FindItem(SrcPos.nX, SrcPos.nY);
					if (nIdx <= 0)
						return FALSE;
					if (Item[nIdx].GetGenre() == item_equip)
					//if (Item[nIdx].GetGenre() != item_medicine)
					{
						_ASSERT(0);
						return FALSE;
					}
					if (!m_Room[room_equipment].PickUpItem(nIdx, SrcPos.nX, SrcPos.nY, Item[nIdx].GetWidth(), Item[nIdx].GetHeight()))
						return FALSE;
					m_Room[room_immediacy].PlaceItem(DesPos.nX, DesPos.nY, nIdx, Item[nIdx].GetWidth(), Item[nIdx].GetHeight());
					nListIdx = FindSame(nIdx);
					if (nListIdx <= 0)
						return FALSE;
					m_Items[nListIdx].nPlace = pos_immediacy;
					m_Items[nListIdx].nX = DesPos.nX;
					m_Items[nListIdx].nY = DesPos.nY;
					bMove = TRUE;
				}
				break;
			}
		}
		break;
	}

	if (!bMove)
		return bMove;

	// 通知界面
	KUiObjAtContRegion sSrcInfo, sDestInfo;

	sSrcInfo.Obj.uGenre		= CGOG_ITEM;
	sSrcInfo.Obj.uId		= nIdx;
	sSrcInfo.Region.Width	= Item[nIdx].GetWidth();
	sSrcInfo.Region.Height	= Item[nIdx].GetHeight();
	sSrcInfo.Region.h		= SrcPos.nX;
	sSrcInfo.Region.v		= SrcPos.nY;
	sSrcInfo.eContainer		= UOC_ITEM_TAKE_WITH;

	sDestInfo.Obj.uGenre	= CGOG_ITEM;
	sDestInfo.Obj.uId		= nIdx;
	sDestInfo.Region.Width	= Item[nIdx].GetWidth();
	sDestInfo.Region.Height	= Item[nIdx].GetHeight();
	sDestInfo.Region.h		= DesPos.nX;
	sDestInfo.Region.v		= DesPos.nY;
	sDestInfo.eContainer	= UOC_IMMEDIA_ITEM;

	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sSrcInfo, 0);
	CoreDataChanged(GDCNI_OBJECT_CHANGED, (DWORD)&sDestInfo, 1);

	return bMove;
}
#endif

#ifndef	_SERVER
//---------------------------------------------------------------------
//	功能：物品从一个地方直接移动到另一个地方，不经过鼠标这个中间过程
//---------------------------------------------------------------------
void	KItemList::MenuSetMouseItem()
{
	KUiObjAtContRegion	sInfo;
	if (!m_Hand)
	{
		CoreDataChanged(GDCNI_HOLD_OBJECT, 0, 0);
	}
	else
	{
		sInfo.Obj.uGenre = CGOG_ITEM;
		sInfo.Obj.uId = m_Hand;
		sInfo.Region.Width = Item[m_Hand].GetWidth();
		sInfo.Region.Height = Item[m_Hand].GetHeight();
		sInfo.Region.h = 0;
		sInfo.Region.v = 0;
		sInfo.eContainer = UOC_IN_HAND;
		CoreDataChanged(GDCNI_HOLD_OBJECT, (DWORD)&sInfo, 0);
	}
}
#endif

#ifdef _SERVER
BOOL KItemList::EatItem(int nPlace, int nX, int nY)
{
	int nItemIdx = 0;
	switch(nPlace)
	{
	case pos_equiproom:
		nItemIdx = m_Room[room_equipment].FindItem(nX, nY);
		if (nItemIdx > 0)
			return NowEatItem(nItemIdx);
		break;
	case pos_equiproomex:
		nItemIdx = m_Room[room_equipmentex].FindItem(nX, nY);
		if (nItemIdx > 0)
			return NowEatItem(nItemIdx);
		break;
	case pos_immediacy:
		nItemIdx = m_Room[room_immediacy].FindItem(nX, nY);
		if (nItemIdx > 0)
			return NowEatItem(nItemIdx);
		break;
	}

	return FALSE;
}
#endif

PlayerItem* KItemList::GetFirstItem()
{
	m_nListCurIdx = m_UseIdx.GetNext(0);
	return &m_Items[m_nListCurIdx];
}

PlayerItem* KItemList::GetNextItem()
{
	if ( !m_nListCurIdx )
		return NULL;
	m_nListCurIdx = m_UseIdx.GetNext(m_nListCurIdx);
	return &m_Items[m_nListCurIdx];
}

void	KItemList::ClearRoom(int nRoom)
{
	if (nRoom >= 0 && nRoom < room_num)
		this->m_Room[nRoom].Clear();
}

void	KItemList::BackupTrade()
{
	if ( !m_Room[room_tradeback].m_pArray )
		m_Room[room_tradeback].Init(m_Room[room_equipment].m_nWidth, m_Room[room_equipment].m_nHeight);
	memcpy(m_Room[room_tradeback].m_pArray, m_Room[room_equipment].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);

	memcpy(this->m_sBackItems, this->m_Items, sizeof(PlayerItem) * MAX_PLAYER_ITEM);

	m_nBackHand = m_Hand;
}

void	KItemList::RecoverTrade()
{
	memcpy(m_Room[room_equipment].m_pArray, m_Room[room_tradeback].m_pArray, sizeof(int) * m_Room[room_tradeback].m_nWidth * m_Room[room_tradeback].m_nHeight);

#ifndef _SERVER
	int nIdx = 0;
	while((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (m_Items[nIdx].nPlace == pos_trade1)
			Remove(m_Items[nIdx].nIdx);
	}
/*	for (int i = 0; i < MAX_PLAYER_ITEM; i++)
	{
		if (m_Items[i].nIdx && m_Items[i].nPlace == pos_trade1)
			Remove(m_Items[i].nIdx);
	}*/
#endif
	memcpy(m_Items, m_sBackItems, sizeof(PlayerItem) * MAX_PLAYER_ITEM);
	m_Hand = m_nBackHand;
}

void	KItemList::StartTrade()
{
	BackupTrade();
	ClearRoom(room_trade);
	ClearRoom(room_trade1);
}

/*!*****************************************************************************
// Function		: KItemList::RemoveAll
// Purpose		: 退出时清除所有的装备
// Return		: void
// Comments		: 会实际地从游戏世界中的道具数组中去掉
// Author		: Spe
*****************************************************************************/
void KItemList::RemoveAll()
{
	int nIdx = m_UseIdx.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdx.GetNext(nIdx);
		int nGameIdx = m_Items[nIdx].nIdx;
		Remove(m_Items[nIdx].nIdx);
#ifdef _SERVER
		// 客户端在上面KItemList::Remove()已经做了ItemSet.Remove()
		ItemSet.Remove(nGameIdx);
#endif
		nIdx = nIdx1;
	}
}

int KItemList::GetWeaponParticular()
{
	if (m_EquipItem[itempart_weapon])
		return Item[m_EquipItem[itempart_weapon]].GetParticularMelee();
	return -1;
}

int		KItemList::CountCommonItem(int nItemNature, int nItemGenre, int nDetailType, int nLevel, int nSeries, int P)
{
	int		nIdx = 0;
	int nResult = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;

		if (nItemNature != Item[nGameIdx].GetNature())
			continue;

		if (nItemGenre != Item[nGameIdx].GetGenre())
			continue;

		if (nDetailType > -1 && (nDetailType != Item[nGameIdx].GetDetailType()))
			continue;

		if (nLevel > -1 && (nLevel != Item[nGameIdx].GetLevel()))
			continue;

		if (nSeries > -1 && (nSeries != Item[nGameIdx].GetSeries()))
			continue;

		if (m_Items[nIdx].nPlace != P)
			continue;

		if (Item[nGameIdx].IsStack())
			nResult += Item[nGameIdx].GetStackNum();
		else
			nResult++;
	}
	return nResult;
}


int		KItemList::GetItemCountRoom(int P)
{
	int		nNo = 0;
	int		nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		if (m_Items[nIdx].nPlace != P)
			continue;
		nNo++;
	}
	return nNo;
}

int		KItemList::FindItem(int nItemNature, int nItemGenre, int nDetailType, int nLevel, int nSeries)
{
	int		nIdx = 0;
	int nCount = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;

		if (nItemNature != Item[nGameIdx].GetNature())
			continue;

		if (nItemGenre != Item[nGameIdx].GetGenre())
			continue;

		if (nDetailType > -1 && (nDetailType != Item[nGameIdx].GetDetailType()))
			continue;

		if (nLevel > -1 && (nLevel != Item[nGameIdx].GetLevel()))
			continue;

		if (nSeries > -1 && (nSeries != Item[nGameIdx].GetSeries()))
			continue;

		return nIdx;
	}
	return 0;
}

int		KItemList::FindItemByIndex(int nItemNature, int nIndex)
{
	int		nIdx = 0;
	int nCount = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (nItemNature == Item[nGameIdx].GetNature() && 
			nIndex == Item[nGameIdx].GetRow())
			return nIdx;
	}
	return 0;
}

#ifdef _SERVER
int KItemList::RemoveCommonItem(int nCount, int nItemNature, int nItemGenre, int nDetailType, int nLevel, int nSeries, int P)
{
	int nIdx, nResult = 0;
	for (int i = 0; i < nCount; i++)
	{
		if (!FindSameToRemove(nItemNature, nItemGenre, nDetailType, nLevel, nSeries, P, &nIdx))
			break;

		if (nIdx)
		{
			if (Item[nIdx].IsStack() && 
				Item[nIdx].GetStackNum() > 1)
			{
				Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - 1);
				this->SyncItem(nIdx);
				nResult ++;
				continue;
			}
			else
			{
				this->Remove(nIdx);
				ItemSet.Remove(nIdx);
				nResult ++;
				continue;
			}
		}
	}
	return nResult;
}
#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中把 trade room 中的 item 的 idx width height 信息写入 itemset 中的 m_psItemInfo 中去
//--------------------------------------------------------------------------
void	KItemList::GetTradeRoomItemInfo()
{
	_ASSERT(ItemSet.m_psItemInfo);
//	if (!ItemSet.m_psItemInfo)
//	{
//		ItemSet.m_psItemInfo = new TRADE_ITEM_INFO[TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT];
//	}
	memset(ItemSet.m_psItemInfo, 0, sizeof(TRADE_ITEM_INFO) * TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	while (1)
	{
		nItemIdx = m_Room[room_trade].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		_ASSERT(nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT);

		ItemSet.m_psItemInfo[nPos].m_nIdx = nItemIdx;
		ItemSet.m_psItemInfo[nPos].m_nWidth = Item[nItemIdx].GetWidth();
		ItemSet.m_psItemInfo[nPos].m_nHeight = Item[nItemIdx].GetHeight();
		nPos++;
	}

	// 从大到小排序
	TRADE_ITEM_INFO	sTemp;
	for (int i = nPos - 1; i >= 0; i--)
	{
		for (int j = 0; j < i; j++)
		{
			if (ItemSet.m_psItemInfo[j].m_nWidth * ItemSet.m_psItemInfo[j].m_nHeight < 
				ItemSet.m_psItemInfo[j + 1].m_nWidth * ItemSet.m_psItemInfo[j + 1].m_nHeight)
			{
				sTemp = ItemSet.m_psItemInfo[j];
				ItemSet.m_psItemInfo[j] = ItemSet.m_psItemInfo[j + 1];
				ItemSet.m_psItemInfo[j + 1] = sTemp;
			}
		}
	}
}
#endif

#ifdef _SERVER
//--------------------------------------------------------------------------
//	功能：交易中判断买进的物品能不能完全放进自己的物品栏
//--------------------------------------------------------------------------
BOOL	KItemList::TradeCheckCanPlace()
{
	LPINT	pnTempRoom;
	pnTempRoom = new int[EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT];
	memcpy(pnTempRoom, m_Room[room_equipment].m_pArray, sizeof(int) * EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT);

	int		nPos, i, j, a, b, nFind, nNext;
	for (nPos = 0; nPos < TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT; nPos++)
	{
		if (!ItemSet.m_psItemInfo[nPos].m_nIdx)
			break;
		nFind = 0;
		for (i = 0; i < EQUIPMENT_ROOM_HEIGHT - ItemSet.m_psItemInfo[nPos].m_nHeight + 1; i++)
		{
			for (j = 0; j < EQUIPMENT_ROOM_WIDTH - ItemSet.m_psItemInfo[nPos].m_nWidth + 1; j++)
			{
				nNext = 0;
				for (a = 0; a < ItemSet.m_psItemInfo[nPos].m_nHeight; a++)
				{
					for (b = 0; b < ItemSet.m_psItemInfo[nPos].m_nWidth; b++)
					{
						if (pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b])
						{
							nNext = 1;
							break;
						}
					}
					if (nNext)
						break;
				}
				// 找到一个位置
				if (!nNext)
				{
					// 数据处理
					ItemSet.m_psItemInfo[nPos].m_nX = j;
					ItemSet.m_psItemInfo[nPos].m_nY = i;
					for (a = 0; a < ItemSet.m_psItemInfo[nPos].m_nHeight; a++)
					{
						for (b = 0; b < ItemSet.m_psItemInfo[nPos].m_nWidth; b++)
							pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b] = ItemSet.m_psItemInfo[nPos].m_nIdx;
					}

					nFind = 1;
					break;
				}
			}
			if (nFind)
				break;
		}
		if (!nFind)
		{
			delete []pnTempRoom;
			return FALSE;
		}
	}

	delete []pnTempRoom;
	return TRUE;
}
#endif

//--------------------------------------------------------------------------
//	功能：判断一定长宽的物品能否放进物品栏 (为了服务器效率，本函数里面没有调用其他函数)
//--------------------------------------------------------------------------
BOOL	KItemList::CheckCanPlaceInEquipment(int nWidth, int nHeight, int *pnP, int *pnX, int *pnY)
{
	if (nWidth <= 0 || nHeight <= 0 || !pnX || !pnY)
		return FALSE;

	_ASSERT(m_Room[room_equipment].m_pArray);

	LPINT	pnTempRoom;
	int		i, j, a, b, nNext;

	pnTempRoom = m_Room[room_equipment].m_pArray;

	for (i = 0; i < EQUIPMENT_ROOM_HEIGHT - nHeight + 1; i++)
	{
		for (j = 0; j < EQUIPMENT_ROOM_WIDTH - nWidth + 1; j++)
		{
			nNext = 0;
			for (a = 0; a < nHeight; a++)
			{
				for (b = 0; b < nWidth; b++)
				{
					if (pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b])
					{
						nNext = 1;
						break;
					}
				}
				if (nNext)
					break;
			}
			if (!nNext)
			{
				*pnP = pos_equiproom;
				*pnX = j;
				*pnY = i;
				return TRUE;
			}
		}
	}

	if (Player[m_PlayerIdx].m_dwEquipExpandTime - KSG_GetCurSec() > 0)
	{
		_ASSERT(m_Room[room_equipmentex].m_pArray);

		pnTempRoom = m_Room[room_equipmentex].m_pArray;

		for (i = 0; i < EQUIPMENT_ROOM_HEIGHT - nHeight + 1; i++)
		{
			for (j = 0; j < EQUIPMENT_ROOM_WIDTH - nWidth + 1; j++)
			{
				nNext = 0;
				for (a = 0; a < nHeight; a++)
				{
					for (b = 0; b < nWidth; b++)
					{
						if (pnTempRoom[(i + a) * EQUIPMENT_ROOM_WIDTH + j + b])
						{
							nNext = 1;
							break;
						}
					}
					if (nNext)
						break;
				}
				if (!nNext)
				{
					*pnP = pos_equiproomex;
					*pnX = j;
					*pnY = i;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

#ifdef _SERVER
BOOL	KItemList::FindSameToRemove(int nItemNature, int nItemGenre, int nDetailType, int nLevel, int nSeries, int P, int *pnIdx)
{
	return m_Room[PositionToRoom(P)].FindSameToRemove(nItemNature, nItemGenre, nDetailType, nLevel, nSeries, pnIdx);
}
//------------------------------------------------------------------------------
//	功能：自动把一个药品从room_equipment移动到room_immediacy
//------------------------------------------------------------------------------
BOOL	KItemList::AutoMoveItemFromEquipmentRoom(int nItemIdx, int nSrcX, int nSrcY, int nDestX, int nDestY)
{
	if (!m_Room[room_equipment].m_pArray || !m_Room[room_immediacy].m_pArray)
		return FALSE;
	if (nSrcX < 0 || nSrcX >= m_Room[room_equipment].m_nWidth || nSrcY < 0 || nSrcY >= m_Room[room_equipment].m_nHeight)
		return FALSE;
	if (nDestX < 0 || nDestX >= m_Room[room_immediacy].m_nWidth || nDestY < 0 || nDestY >= m_Room[room_immediacy].m_nHeight)
		return FALSE;
	if (nItemIdx != m_Room[room_equipment].m_pArray[nSrcY * m_Room[room_equipment].m_nWidth + nSrcX] ||
		0 != m_Room[room_immediacy].m_pArray[nDestY * m_Room[room_immediacy].m_nWidth + nDestX])
		return FALSE;

	_ASSERT(Item[nItemIdx].GetWidth() == 1 && Item[nItemIdx].GetHeight() == 1);
	if (!m_Room[room_equipment].PickUpItem(nItemIdx, nSrcX, nSrcY, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
		return FALSE;
	if (!m_Room[room_immediacy].PlaceItem(nDestX, nDestY, nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight()))
	{
		m_Room[room_equipment].PlaceItem(nSrcX, nSrcY, nItemIdx, Item[nItemIdx].GetWidth(), Item[nItemIdx].GetHeight());
		return FALSE;
	}

	int nListIdx = FindSame(nItemIdx);
	_ASSERT(nListIdx > 0);
	m_Items[nListIdx].nPlace = pos_immediacy;
	m_Items[nListIdx].nX = nDestX;
	m_Items[nListIdx].nY = nDestY;

	ITEM_AUTO_MOVE_SYNC	sMove;
	sMove.ProtocolType = s2c_ItemAutoMove;
	sMove.m_btSrcPos = pos_equiproom;
	sMove.m_btSrcX = nSrcX;
	sMove.m_btSrcY = nSrcY;
	sMove.m_btDestPos = pos_immediacy;
	sMove.m_btDestX = nDestX;
	sMove.m_btDestY = nDestY;
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sMove, sizeof(ITEM_AUTO_MOVE_SYNC));

	return TRUE;
}
#endif

void	KItemList::RemoveRoom(int nRoom)
{
	if (nRoom < 0 || nRoom >= room_num)
		return;

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	while (1)
	{
		nItemIdx = m_Room[nRoom].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		_ASSERT(nPos < m_Room[nRoom].m_nWidth * m_Room[nRoom].m_nHeight);

		Remove(nItemIdx);
#ifdef _SERVER
		ItemSet.Remove(nItemIdx);
#endif
		nPos++;
	}
	ClearRoomItemOnly(nRoom);
}

int KItemList::CalcFreeItemCellCount(int nWidth, int nHeight, int nRoom)
{
	_ASSERT(m_Room[nRoom].m_pArray);

	return m_Room[nRoom].FindFreeCell(nWidth, nHeight);
}


#ifndef _SERVER
void KItemList::LockOperation()
{
	if (IsLockOperation())
	{
		_ASSERT(0);
		return;
	}
	m_bLockOperation = TRUE;
}
#endif

#ifndef _SERVER
void KItemList::UnlockOperation()
{
	if (!IsLockOperation())
	{
		return;
	}
	m_bLockOperation = FALSE;
}
#endif

int KItemList::GetActiveAttribNum(int nIdx)
{
	for (int i = 0; i < itempart_num; i++)
	{
		if (i >= itempart_horse)
		{
			return MAX_ITEM_MAGICATTRIB / 2;
		}
		if (nIdx == m_EquipItem[i])
		{
			return GetEquipEnhance(i);
		}
	}
	return 0;
}

int KItemList::GetGoldActiveAttribNum(int nIdx)
{
	for (int i = 0; i < itempart_num; i++)
	{
		if (i >= itempart_horse)
		{
			return MAX_ITEM_MAGICATTRIB / 2;
		}
		if (nIdx == m_EquipItem[i])
		{
			return GetGoldEquipEnhance(i);
		}
	}
	return 0;
}

#ifdef _SERVER
//-------------------------------------------------------------------------------
//	功能：丢失随身物品
//-------------------------------------------------------------------------------
void	KItemList::AutoLoseItemFromEquipmentRoom(int nRate)
{
	if (Player[m_PlayerIdx].CheckTrading())
		return;
	if (nRate <= 0 || nRate > 100)
		return;

	int		nItemIdx, nXpos, nYpos, nPos;

	nItemIdx = 0;
	nXpos = 0;
	nYpos = 0;
	nPos = 0;

	// 先统计所有物品，排除任务物品
	while (1)
	{
		nItemIdx = m_Room[room_equipment].GetNextItem(nItemIdx, nXpos, nYpos, &nXpos, &nYpos);
		if (nItemIdx == 0)
			break;
		if (Item[nItemIdx].GetLockDrop() || 
			Item[nItemIdx].GetLock()->IsLock())
			continue;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nIdx = nItemIdx;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nPlace = pos_equiproom;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nX = nXpos;
		ItemSet.m_sLoseItemFromEquipmentRoom[nPos].nY = nYpos;
		nPos++;
	}
	if (nPos == 0)
		return;

	KMapPos			sMapPos;
	int				nSelect;
	int				nObj;
	KObjItemInfo	sInfo;

	for (int i = 0; i < nPos; i++)
	{
		if (g_Random(100) >= nRate)
			continue;
		nItemIdx = ItemSet.m_sLoseItemFromEquipmentRoom[i].nIdx;
		Player[m_PlayerIdx].GetAboutPos(&sMapPos);
		// 丢弃
		if (Remove(nItemIdx))
		{
			sInfo.m_nItemID = nItemIdx;
			sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
			sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
			sInfo.m_nMoneyNum = 0;
			strcpy(sInfo.m_szName, Item[nItemIdx].GetName());
			sInfo.m_nColorID = Item[nItemIdx].GetQuality();
			sInfo.m_nGenre = Item[nItemIdx].GetGenre();
			sInfo.m_nDetailType = Item[nItemIdx].GetDetailType();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_bOverLook = FALSE;

			nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				Object[nObj].SetItemBelong(-1);
			}

			SHOW_MSG_SYNC	sMsg;
			sMsg.ProtocolType = s2c_msgshow;
			sMsg.m_wMsgID = enumMSG_ID_DEATH_LOSE_ITEM;
			sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(sInfo.m_szName);
			sMsg.AllocateBuffer(sMsg.m_wLength + 1);
			memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
			memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), sInfo.m_szName, sizeof(sInfo.m_szName));
			g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
		}
	}
}
#endif

#ifdef _SERVER
//-------------------------------------------------------------------------------
//	功能：丢失一个穿在身上的装备(马不会掉)
//-------------------------------------------------------------------------------
void	KItemList::AutoLoseEquip()
{
#ifndef defEQUIP_POWER
	int		i, nPos = 0;

	for (i = 0; i < itempart_num; i++)
	{
		if (m_EquipItem[i] <= 0)
			continue;
		if (i == itempart_horse)
			continue;
		if (Item[ItemSet.m_sLoseEquipItem[nPos].nIdx].GetLockDrop() || 
			Item[ItemSet.m_sLoseEquipItem[nPos].nIdx].GetLock()->IsLock())
			continue;
		ItemSet.m_sLoseEquipItem[nPos].nIdx = m_EquipItem[i];
		ItemSet.m_sLoseEquipItem[nPos].nPlace = nPos;
		nPos++;
	}
	if (nPos == 0)
		return;

	int		nSelect = g_Random(nPos);
#endif

#ifdef defEQUIP_POWER
	int		i, nPos = 0, nTotalPower = 0;

	for (i = 0; i < itempart_num; i++)
	{
		if (m_EquipItem[i] <= 0)
			continue;
		nTotalPower += g_nEquipPower[i];
		ItemSet.m_sLoseEquipItem[nPos].nIdx = m_EquipItem[i];
		ItemSet.m_sLoseEquipItem[nPos].nPlace = nPos;
		ItemSet.m_sLoseEquipItem[nPos].nX = nTotalPower;	// nX 借用一下
		nPos++;
	}
	if (nTotalPower == 0)
		return;
	int		nSelect = g_Random(nTotalPower);
	for (i = 0; i < nPos; i++)
	{
		if (ItemSet.m_sLoseEquipItem[i].nX > nSelect)
		{
			nSelect = i;
			break;
		}
	}
	if (i >= nPos)
		return;
#endif

	int			 nItemIdx;
	KMapPos		sMapPos;

	nItemIdx = ItemSet.m_sLoseEquipItem[nSelect].nIdx;
	Player[m_PlayerIdx].GetAboutPos(&sMapPos);
	if (Remove(nItemIdx))
	{
		int		nObj;
		KObjItemInfo	sInfo;
		sInfo.m_nItemID = nItemIdx;
		sInfo.m_nItemWidth = Item[nItemIdx].GetWidth();
		sInfo.m_nItemHeight = Item[nItemIdx].GetHeight();
		sInfo.m_nMoneyNum = 0;
		strcpy(sInfo.m_szName, Item[nItemIdx].GetName());
		sInfo.m_nColorID = Item[nItemIdx].GetQuality();
		sInfo.m_nGenre = Item[nItemIdx].GetGenre();
		sInfo.m_nDetailType = Item[nItemIdx].GetDetailType();
		sInfo.m_nMovieFlag = 1;
		sInfo.m_nSoundFlag = 1;
		sInfo.m_bOverLook = FALSE;

		nObj = ObjSet.Add(Item[nItemIdx].GetObjIdx(), sMapPos, sInfo);
		if (nObj >= 0)
		{
			Object[nObj].SetItemBelong(-1);
		}

		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_DEATH_LOSE_ITEM;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + sizeof(sInfo.m_szName);
		sMsg.AllocateBuffer(sMsg.m_wLength + 1);
		memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
		memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), sInfo.m_szName, sizeof(sInfo.m_szName));
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------------
//	功能：
//-------------------------------------------------------------------------------
int		KItemList::GetSameDetailItemNum(int nImmediatePos)
{
	if (nImmediatePos < 0 || nImmediatePos >= IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
		return 0;
	int		nIdx = m_Room[room_immediacy].FindItem(nImmediatePos, 0);
	if (nIdx <= 0)
		return 0;
	return m_Room[room_equipment].CalcSameDetailType(Item[nIdx].GetNature(), Item[nIdx].GetGenre(), Item[nIdx].GetDetailType(), Item[nIdx].GetLevel(), Item[nIdx].GetSeries()) + Item[nIdx].GetStackNum();
}
#endif

#ifdef _SERVER
void KItemList::Abrade(int nType)
{
	int nItemIdx = 0;
	for (int i = 0; i < itempart_num; i++)
	{
		nItemIdx = m_EquipItem[i];
		if (nItemIdx)
		{
			int nOldDur = Item[nItemIdx].GetDurability();
			if(nOldDur ==0)
				continue;
			int nDur = Item[nItemIdx].Abrade(nType == enumAbradeDefend ? PlayerSet.m_sPKPunishParam[Player[m_PlayerIdx].m_cPK.GetPKValue()].m_nAbradeP : 0, 
				ItemSet.GetAbradeRange(nType, i));
			if(nDur == -1)
				continue;
			if (nOldDur != nDur)
			{
				ITEM_DURABILITY_CHANGE sIDC;
				sIDC.ProtocolType = s2c_itemdurabilitychange;
				sIDC.dwItemID = Item[nItemIdx].GetID();
				sIDC.nChange = nDur - nOldDur;
				if (g_pServer)
					g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, &sIDC, sizeof(ITEM_DURABILITY_CHANGE));
			}
			if (nDur == 0)
			{
				Remove(nItemIdx);
				InsertEquipment(nItemIdx);
			}
		}
	}
}
#endif

BOOL KItemList::CanCombie(int Source, int Dest)
{
	if (Player[m_PlayerIdx].CheckTrading() )	// 不在交易
		return FALSE;
	if (Player[m_PlayerIdx].GetLockMove()->bLock)
		return FALSE;
	if  (Item[Source].GetLock()->IsLock())
		return FALSE;
	if (Item[Source].IsStack() && Item[Dest].IsStack() && 
		Item[Source].GetNature() == Item[Dest].GetNature() && 
		Item[Source].GetGenre() == Item[Dest].GetGenre() && 
		Item[Source].GetDetailType() == Item[Dest].GetDetailType() && 
		Item[Source].GetParticular() == Item[Dest].GetParticular() && 
		Item[Source].GetSeries() == Item[Dest].GetSeries() && 
		Item[Source].GetLevel() == Item[Dest].GetLevel() && 
		Item[Source].GetMantle() == Item[Dest].GetMantle() && 
		Item[Source].IsStack() == Item[Dest].IsStack() && 
		Item[Dest].GetStackNum() < Item[Dest].GetMaxStackNum() && 
		Item[Source].GetStackNum() < Item[Source].GetMaxStackNum() && 
		Item[Source].GetExpireTime() == Item[Dest].GetExpireTime() && 
		Item[Source].GetLockSell() == Item[Dest].GetLockSell() &&
		Item[Source].GetLockTrade() == Item[Dest].GetLockTrade() &&
		Item[Source].GetLockDrop() == Item[Dest].GetLockDrop() &&
		Item[Source].GetParam() == Item[Dest].GetParam()
	)
		return TRUE;

	return FALSE;
}


BOOL KItemList::CompareRemoveItem(int Source, int Dest)
{
	if (Player[m_PlayerIdx].CheckTrading() )	// 不在交易
		return FALSE;
	if (Item[Source].GetNature() == Item[Dest].GetNature() && 
		Item[Source].GetGenre() == Item[Dest].GetGenre() && 
		Item[Source].GetDetailType() == Item[Dest].GetDetailType() && 
		Item[Source].GetParticular() == Item[Dest].GetParticular() && 
		Item[Source].GetSeries() == Item[Dest].GetSeries() && 
		Item[Source].GetLevel() == Item[Dest].GetLevel() && 
		Item[Source].GetMantle() == Item[Dest].GetMantle() && 
		Item[Source].GetRow() == Item[Dest].GetRow() && 
		Item[Source].IsStack() == Item[Dest].IsStack() && 
		Item[Source].GetExpireTime() == Item[Dest].GetExpireTime() && 
		Item[Source].GetLockSell() == Item[Dest].GetLockSell() &&
		Item[Source].GetLockTrade() == Item[Dest].GetLockTrade() &&
		Item[Source].GetLockDrop() == Item[Dest].GetLockDrop() &&
		Item[Source].GetParam() == Item[Dest].GetParam()
		)
		return TRUE;
	return FALSE;
}

#ifdef _SERVER
void KItemList::SyncItem(int nIdx, BOOL bIsNew, int nPlace, int nX, int nY, int nPlayerIndex)
{
	ITEM_SYNC	sItem;
	sItem.ProtocolType = s2c_syncitem;
	sItem.m_bIsNew = bIsNew;
	sItem.m_bTemp = Item[nIdx].IsTemp();
	sItem.m_dwOwner = Item[nIdx].GetOwner();
	sItem.m_TradePrice = Item[nIdx].GetTradePrice();
	sItem.m_ID = Item[nIdx].GetID();
	sItem.m_Nature = Item[nIdx].GetNature();
	sItem.m_Genre = Item[nIdx].GetGenre();
	if(Item[nIdx].GetNature() >= NATURE_GOLD)
		sItem.m_Detail = Item[nIdx].GetRow();
	else
		sItem.m_Detail = Item[nIdx].GetDetailType();
	sItem.m_Particur = Item[nIdx].GetParticular();
	sItem.m_Series = Item[nIdx].GetSeries();
	sItem.m_Level = Item[nIdx].GetLevel();
	sItem.m_Luck = Item[nIdx].m_GeneratorParam.nLuck;
	sItem.m_BackLocal = *Item[nIdx].GetBackLocal();
	sItem.m_btPlace = nPlace;
	sItem.m_btX = nX;
	sItem.m_btY = nY;
	sItem.m_StackNum	= Item[nIdx].GetStackNum();
	sItem.m_ExpireTime = Item[nIdx].GetExpireTime();
	sItem.m_LockItem = *Item[nIdx].GetLock();
	sItem.m_bLockSell = Item[nIdx].GetLockSell();
	sItem.m_bLockTrade = Item[nIdx].GetLockTrade();
	sItem.m_bLockDrop = Item[nIdx].GetLockDrop();
	sItem.m_Param = Item[nIdx].GetParam();
	sItem.m_Mantle	= Item[nIdx].GetMantle();
	sItem.m_Width	= Item[nIdx].GetWidth();
	sItem.m_Height	= Item[nIdx].GetHeight();	
	sItem.m_Fortune	= Item[nIdx].GetFortune();
	for (int j = 0; j < MAX_ITEM_MAGICLEVEL; j++)
		sItem.m_MagicLevel[j] = (int)Item[nIdx].m_GeneratorParam.nGeneratorLevel[j];
	sItem.m_RandomSeed = Item[nIdx].m_GeneratorParam.uRandomSeed;
	sItem.m_Version = Item[nIdx].m_GeneratorParam.nVersion;
	sItem.m_Durability = Item[nIdx].GetDurability();
	
	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
	else
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC));
}

void KItemList::SyncItemMagicAttrib(int nIdx)
{
	ITEM_SYNC_MAGIC sItem;
	sItem.ProtocolType		= s2c_syncmagic;
	sItem.m_dwID 			= Item[nIdx].GetID();
	memcpy(sItem.m_MagicLevel, Item[nIdx].m_GeneratorParam.nGeneratorLevel, sizeof(int)*MAX_ITEM_MAGICLEVEL);
	memcpy(sItem.m_MagicAttrib, Item[nIdx].m_aryMagicAttrib, sizeof(sItem.m_MagicAttrib));
	if (g_pServer)
		g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&sItem, sizeof(ITEM_SYNC_MAGIC));
}


BOOL KItemList::RemoveItem(int nItemGenre,int nDetailType,int nParticularType,int nLevel)
{
	int nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		int nGameIdx = m_Items[nIdx].nIdx;
		if (	nItemGenre == Item[nGameIdx].GetGenre()
			&&	nDetailType == Item[nGameIdx].GetDetailType()
			&&  nParticularType == Item[nGameIdx].GetParticular()
			&&  nLevel == Item[nGameIdx].GetLevel()			
			)
		{
			Remove(nGameIdx);
			ItemSet.Remove(nGameIdx);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KItemList::RemoveItem(int nIdx, int nNum)
{
	if(nIdx<=0)
		return FALSE;
	if(nNum > Item[nIdx].GetStackNum())
		return FALSE;

	int nListIndex = FindSame(nIdx);
	if (!nListIndex)
	{
		_ASSERT(0);
		return FALSE;
	}

	if (nNum == 0 || nNum == Item[nIdx].GetStackNum())
	{
		if(m_Items[nListIndex].nPlace == pos_immediacy)
			goto REMOVE_IMMEDIACY;
		else
		{
			Remove(nIdx);
			ItemSet.Remove(nIdx);
			return TRUE;
		}
	}
	else
	{
		if (Item[nIdx].IsStack())
		{
			Item[nIdx].SetStackNum(Item[nIdx].GetStackNum() - nNum);
			this->SyncItem(nIdx);
			return TRUE;
		}
		else
		{
			if(m_Items[nListIndex].nPlace == pos_immediacy)
				goto REMOVE_IMMEDIACY;
			else
			{
				Remove(nIdx);
				ItemSet.Remove(nIdx);
				return TRUE;
			}
		}
	}

REMOVE_IMMEDIACY:
	int pnIdx,pnX,pnY;
	if (Player[m_PlayerIdx].m_ItemList.m_Room[room_equipment].FindSameItemToMove(nIdx, &pnIdx, &pnX, &pnY))
	{
		int nX = m_Items[nListIndex].nX;
		int nY = m_Items[nListIndex].nY;
		Remove(nIdx);
		ItemSet.Remove(nIdx);
		this->AutoMoveItemFromEquipmentRoom(pnIdx, pnX, pnY, nX, nY);
	}
	else
	{
		Remove(nIdx);
		ItemSet.Remove(nIdx);
	}
	return TRUE;
}

BOOL KItemList::Lock(int nIdx, BOOL bLock)
{
	if(nIdx<=0)
		return FALSE;
	if(Item[nIdx].GetLock()->nState == LOCK_STATE_FOREVER || 
	Item[nIdx].GetLock()->nState == LOCK_STATE_CHARACTER)
		return FALSE;
	if(bLock)
	{
		if(Item[nIdx].GetLock()->nState == LOCK_STATE_LOCK)
			return FALSE;
		Item[nIdx].LockItem(LOCK_STATE_LOCK);
	}
	else
	{
		if(Item[nIdx].GetLock()->nState == LOCK_STATE_UNLOCK)
			return FALSE;
		Item[nIdx].LockItem(259200);
	}

	SyncItem(nIdx);

	return TRUE;
}
#endif
		

int		KItemList::GetItemNum(int nItemGenre, int nDetailType, int nParticular, int nLevel)
{
	int		nNo = 0, nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		{	
				if (nItemGenre == Item[m_Items[nIdx].nIdx].GetGenre() && 
					nDetailType == Item[m_Items[nIdx].nIdx].GetDetailType() && 
					nParticular == Item[m_Items[nIdx].nIdx].GetParticular() &&
					nLevel == Item[m_Items[nIdx].nIdx].GetLevel())
				{
					if (Item[m_Items[nIdx].nIdx].IsStack())
					{
						nNo += Item[m_Items[nIdx].nIdx].GetStackNum();
						break;
					}
					else
						nNo++;
				}
		}
	}
	return nNo;
}


BOOL KItemList::PutCompound(int nIdx, int nPlace /* = -1 */)
{
	if (m_PlayerIdx <= 0 || nIdx <= 0)
		return FALSE;

	
	int nItemListIdx = FindSame(nIdx);
	if (!nItemListIdx)
	{
		_ASSERT(0);
		return FALSE;
	}

	m_CompoundItem[nPlace] = nIdx;
	m_Items[nItemListIdx].nPlace = pos_compound;
	m_Items[nItemListIdx].nX = nPlace;
	m_Items[nItemListIdx].nY = 0;
	return TRUE;
}


void KItemList::DropCompound(int nIdx, int nPos/* = -1*/)
{
	int i = 0;
	if (m_PlayerIdx <= 0)
		return;

	if (nIdx <= 0)
		return;

	if (nPos <= 0)
	{
		for (i = 0; i < MAX_COMPOUND_ITEM; i++)
		{
			if (m_CompoundItem[i] == nIdx)
				break;
		}
		if (i == MAX_COMPOUND_ITEM)
			return;

	}
	else
	{
		if (m_CompoundItem[nPos] != nIdx)	// 东西不对
			return;
		i = nPos;
	}

	m_CompoundItem[i] = 0;
	return;
}

void KItemList::SetMaskLock( BOOL bFlag )
{
	m_bMaskLock = bFlag;
#ifdef _SERVER
	NPC_MASK_SYNC Sync;
	Sync.ProtocolType = s2c_syncmasklock;
	Sync.ID = m_bMaskLock;
	g_pServer->PackDataToClient(Player[m_PlayerIdx].m_nNetConnectIdx, (BYTE*)&Sync, sizeof(NPC_MASK_SYNC));
#endif
}


BOOL KItemList::IsEnoughToActive()
{
	int nArray[itempart_horse][3], nCnt = 0, nFlg=FALSE;
	ZeroMemory(nArray, sizeof(nArray));

	for (int i = 0;i < itempart_horse; i++)
	{
		nFlg = FALSE;
		if (m_EquipItem[i] && Item[m_EquipItem[i]].m_CommonAttrib.nItemNature >= NATURE_GOLD)
		{
			if(i == itempart_ring1)
			{
				if (m_EquipItem[itempart_ring1] && 
					m_EquipItem[itempart_ring2] && 
					/*Item[m_EquipItem[itempart_ring1]].GetNature() == Item[m_EquipItem[itempart_ring2]].GetNature() && */
					Item[m_EquipItem[itempart_ring1]].GetGroup() == Item[m_EquipItem[itempart_ring2]].GetGroup() && 
					Item[m_EquipItem[itempart_ring1]].GetSetID() == Item[m_EquipItem[itempart_ring2]].GetSetID())
					continue;
			}
			for (int j = 0; j < itempart_horse; j++)
			{
				if (/*nArray[j][0] == Item[m_EquipItem[i]].GetNature() && */
					nArray[j][1] == Item[m_EquipItem[i]].GetGroup())
				{
					nArray[j][2]++;
					nFlg = TRUE;
					if (nArray[j][2] >= Item[m_EquipItem[i]].GetNeedToActive2())
						return TRUE;
				}
			}
			if (nFlg==FALSE)
			{
				nArray[nCnt][0] = Item[m_EquipItem[i]].GetNature();
				nArray[nCnt][1] = Item[m_EquipItem[i]].GetGroup();
				nArray[nCnt][2] = 1;
				
				if (nArray[nCnt][2] >= Item[m_EquipItem[i]].GetNeedToActive2())
					return TRUE;
				nCnt++;
			}
		}
	}
	return FALSE;
}

#ifdef _SERVER
void KItemList::BackLocal()
{
	int		nIdx = 0;
	while ((nIdx = m_UseIdx.GetNext(nIdx)))
	{
		PlayerItem TempBackLocal = *Item[nIdx].GetBackLocal();
		if((TempBackLocal.nPlace < pos_hand) || (TempBackLocal.nPlace >= pos_num))
			continue;

		this->Remove(nIdx);

		Item[nIdx].GetBackLocal()->Release();

		if(m_Room[PositionToRoom(TempBackLocal.nPlace)].FindItem(TempBackLocal.nX, TempBackLocal.nY)<= 0)
		{
			Add(nIdx, TempBackLocal.nPlace, TempBackLocal.nX, TempBackLocal.nY, false);
		}
		else
		{
			InsertEquipment(nIdx);
		}
	}
}

void KItemList::InsertEquipment(int nIdx, bool bAutoStack)
{
	int p,x, y;
	if (CheckCanPlaceInEquipment(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &p,&x, &y))
	{
		Add(nIdx, p, x, y, bAutoStack);
	}
	else
	{
		if  (Item[nIdx].GetLock()->IsLock())
		{
			POINT pPos;
			for(int nRoom = room_repository; nRoom <= room_repository+Player[m_PlayerIdx].m_btRepositoryNum && nRoom <= room_repository5; nRoom++)
			{
				if(m_Room[nRoom].FindRoom(Item[nIdx].GetWidth(), Item[nIdx].GetHeight(), &pPos))
				{
					Add(nIdx, pos_repositoryroom+nRoom-room_repository, pPos.x ,pPos.y);
					return;
				}
			}
		}

		int	nIndex = m_Hand;
		if (nIndex)
		{
			Remove(nIndex);
			
			KMapPos sMapPos;
			KObjItemInfo	sInfo;
			
			Player[m_PlayerIdx].GetAboutPos(&sMapPos);
			
			sInfo.m_nItemID = nIndex;
			sInfo.m_nItemWidth = Item[nIndex].GetWidth();
			sInfo.m_nItemHeight = Item[nIndex].GetHeight();
			sInfo.m_nMoneyNum = 0;
			strcpy(sInfo.m_szName, Item[nIndex].GetName());
			sInfo.m_nColorID = Item[nIndex].GetQuality();
			sInfo.m_nGenre = Item[nIndex].GetGenre();
			sInfo.m_nDetailType = Item[nIndex].GetDetailType();
			sInfo.m_nMovieFlag = 1;
			sInfo.m_nSoundFlag = 1;
			sInfo.m_bOverLook = FALSE;
			
			int nObj = ObjSet.Add(Item[nIndex].GetObjIdx(), sMapPos, sInfo);
			if (nObj >= 0)
			{
				if (Item[nIndex].LockPick())
				{
					Object[nObj].SetEntireBelong(this->m_PlayerIdx);
				}
				else
				{
					Object[nObj].SetItemBelong(this->m_PlayerIdx);
				}
			}
		}
		Add(nIdx, pos_hand, 0 ,0);
	}
}

void KItemList::ExecuteScript(int nIdx)
{
	if (nIdx <= 0 || nIdx >= MAX_ITEM)
		return;
	
	switch (Item[nIdx].GetGenre())
	{
		case item_equip:
			break;
		case item_medicine:
			Item[nIdx].ApplyMagicAttribToNPC(&Npc[Player[m_PlayerIdx].m_nIndex], MAX_ITEM_MAGICATTRIB / 2);
			break;
		default:
			char* szScriptFile = Item[nIdx].GetScript();
			if(szScriptFile[0])
				Player[m_PlayerIdx].ExecuteScript(szScriptFile, NORMAL_FUNCTION_NAME, nIdx);
			break;
	}
}
#endif

int KItemList::PositionToIndex(int P, int i)
{
	if (P >= pos_hand && P < pos_num)
	{
		switch (P)
		{
		case pos_equip:
			return m_EquipItem[i];
		}
	}
	return 0;
}