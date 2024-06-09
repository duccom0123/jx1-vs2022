#ifndef KPLAYERAI_H
#define KPLAYERAI_H
#include "KLinkArray.h"

enum
{
	Blood,
	Mana,
	TownPortal,
	TownPortal_l,
	AntiPoison,
	EnchaseExp,
	MecidineBag,
};

enum
{
	OverLookNothing,
	OverLookAllBoss,
	OverLookBlueBoss,
	OverLookGoldBoss,
};

enum
{
	Step_Training,
	Step_Repair,
	Step_InventoryMoney,
	Step_InventoryItem,
	Step_Return,
};

#ifndef _SEVER
class KEquipmentArray
{
public:
	KLinkArray	m_Link;
	int		FindSame(int nIdx);
};

class KPlayerAI
{
	friend class KPlayer;
public:
	unsigned int	m_uTotalTime;
	BOOL			m_bIsActive;
	int				m_nLastFightMode;
	BOOL			m_bPriorityUseMouse;
	BOOL			m_bFightUseSB;
	BOOL			m_bHoldSpaceBar;
	BOOL			m_bEatLife;
	BOOL			m_bEatMana;
	unsigned int	m_uLifeTimeUse;
	unsigned int	m_uManaTimeUse;
	int				m_nLifeAutoV;
	int				m_nLifeAutoP;
	int				m_nManaAutoV;
	int				m_nManaAutoP;
	BOOL			m_bTPLife;
	int				m_nTPLifeV;
	BOOL			m_bTPMana;
	int				m_nTPManaV;
	BOOL			m_bTPNotMedicineBlood;
	BOOL			m_bTPNotMedicineMana;
	BOOL			m_bTPHightMoney;
	int				m_nTPHightMoneyV;
	BOOL			m_bTPDamageEquip;
	int				m_nTPDamageEquipV;	
	BOOL			m_bTPNotEquipment;
	int				m_nTPNotEquipmentV;	
	BOOL			m_bAntiPoison;
	BOOL			m_bEnchaseExp;
	BOOL			m_bLifeReplenish;
	int				m_nLifeReplenishP;
	BOOL			m_bInventoryMoney;
	BOOL			m_bInventoryItem;
	BOOL			m_bReturnPortal;
	int				m_nReturnPortalSec;
	int				m_nCurReturnPortalSec;
	BOOL			m_bFightNear;
	int				m_nRadiusAuto;
	int				m_nDistanceAuto;
	int				m_nSupportSkills[MAX_AUTO_STATESKILL];
	int				m_nFightSkill;
	int				m_nFightBack;
	int				m_nFightBoss;
	int				m_nActiveAuraID1;
	int				m_nActiveAuraID2;
	int				m_nTargetIdx;
	unsigned int	m_uLifeCountDown;
	unsigned int	m_uManaCountDown;
	unsigned int	m_uPortalCountDown;
	unsigned int	m_uAntiPoisonCountDown;
	unsigned int	m_uEnchaseExpCountDown;
	unsigned int	m_uOpenMedicineCountDown;
	int				m_nArrayNpcNeast[MAX_AUTO_SIZE];
	int				m_nArrayNpcOverLook[MAX_AUTO_SIZE];
	int				m_nCount_Attack_OverLook;
	int				m_nArrayTimeNpcOverLook[MAX_AUTO_SIZE];
	int				m_nArrayTimeObjectOverLook[MAX_AUTO_SIZE];
	int				m_nArrayInvitePlayer[MAX_AUTO_SIZE];
	int				m_nArrayTimeInvitePlayer[MAX_AUTO_SIZE];
	int				m_nLifeOverLook;
	int				m_nCount_Run_OverLook;
	int				m_nObject;
	int				m_nArrayObjectNeast[MAX_AUTO_SIZE];
	int				m_nArrayObjectOverLook[MAX_AUTO_SIZE];
	BOOL			m_bPickFightNone;
	BOOL			m_bPickMoney;
	BOOL			m_bPickNotEquip;
	BOOL			m_bPickEquip;
	BOOL			m_bFollowPick;
	int				m_nPickEquipKind;
	BOOL			m_bFilterEquipment;
	int				m_nFilterMagic[MAX_AUTO_FILTERL][2];
	BOOL			m_bSaveTrash;
	int				m_nSaveTrashPrice;
	BOOL			m_bSaveJewelry;
	BOOL			m_bAttackNpc;
	BOOL			m_bFollowPeople;
	char			m_szFollowName[32];
	int				m_nFollowRadius;
	BOOL			m_bAutoParty;
	BOOL			m_bAutoInvite;
	BOOL			m_bAutoInviteList;
	char			m_szPartyList[MAX_AUTO_LIST][32];
	BOOL			m_bOverTarget;
	BOOL			m_bFollowTarget;
	BOOL			m_bOpenMedicine;
	BOOL 			m_bRepairEquip;
	BOOL			m_bSortEquipment;
	KEquipmentArray	m_sListEquipment;
	
	BOOL			m_bMoveMps;
	int				m_nMoveMps[MAX_AUTO_LIST][3];
	int				m_nMoveStep;
	int				m_nMoveCount;	
	
	int				m_nReturnPortalStep;
public:
	KPlayerAI() {};	
	~KPlayerAI() {}
	void	Init();		
	void	Release();
	void	Active();

	void			ClearArrayNpcOverLook(){memset(m_nArrayNpcOverLook, 0, sizeof(m_nArrayNpcOverLook));};
	void			ClearArrayObjectOverLook(){memset(m_nArrayObjectOverLook, 0, sizeof(m_nArrayObjectOverLook));};
	void			ClearArrayTimeNpcOverLook(){memset(m_nArrayTimeNpcOverLook, 0, sizeof(m_nArrayTimeNpcOverLook));};
	void			ClearArrayTimeObjectOverLook(){memset(m_nArrayTimeObjectOverLook, 0, sizeof(m_nArrayTimeObjectOverLook));};
	void			ClearArrayNpcNeast(){memset(m_nArrayNpcNeast, 0, sizeof(m_nArrayNpcNeast));};
	void			ClearArrayObjectNeast(){memset(m_nArrayObjectNeast, 0, sizeof(m_nArrayObjectNeast));};
	void			ClearArrayInvitePlayer(){memset(m_nArrayInvitePlayer, 0, sizeof(m_nArrayInvitePlayer));};
	void			ClearArrayTimeInvitePlayer(){memset(m_nArrayTimeInvitePlayer, 0, sizeof(m_nArrayTimeInvitePlayer));};

	int			FindFreeInvitePlayer()
	{
		for (int i=0; i < MAX_AUTO_SIZE; i++)
		{
			if (m_nArrayInvitePlayer[i] <= 0)
				return i;
		}		
		return -1;
	};	
	
	BOOL			FindSameInvitePlayer(int uID)
	{
		for (int i=0; i < MAX_AUTO_SIZE; i++)
		{
			if (m_nArrayInvitePlayer[i] == uID)
				return TRUE;
		}		
		return FALSE;
	};
	void			MoveTo(int nX, int nY);
	BOOL			FollowAttack(int i);
	BOOL			CheckNpc(int nIndex);
	BOOL			CheckObject(int nObject);
	BOOL			CheckEquip(BYTE btDetail);
	BOOL			CheckPTList(const char *Name);
	BOOL			CheckEquipMagic(int nIdx);
	void			SetHorse();
	void			SwitchAura();
	void			EatItem();
	BOOL			CastStateSkill();
	void			SetSortEquipment(BOOL bFlag);
	void			SortEquipment();
	void			FilterEquip();
	BOOL			MoveMps();
	void			InviteParty();
	BOOL			AddNpc2Array(int nRelation);
	int				GetNearNpcArray(int nRelation);
	BOOL			AddObject2Array();
	int				GetNearObjectArray();
	BOOL			FollowObject(int nObject);
	BOOL			UseItem(int type);
	BOOL			ReturnFromPortal();
	BOOL			InventoryItem();
};
#endif
#endif