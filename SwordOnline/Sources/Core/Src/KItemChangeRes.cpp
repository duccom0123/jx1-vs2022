//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemChangeRes.h
// Date:	2002.12
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------
#include "KCore.h"
#include "KEngine.h"
#include "KItemChangeRes.h"
#include "KItem.h"
#include "CoreUseNameDef.h"

typedef struct _KFlashTable
{
	char			Name[64];
	int				NormalRes;
	int				FlashRes;
}KFlashTable;

static	const KFlashTable	s_FlashTable[] =
{
	{"Quy“n ThÒ",				0,		45	},
	{"Y”m Nguy÷t ßao",			6,		62	},
	{"Th t X›c Ki’m",			3,		63	},
	{"Thanh Long ßao",			6,		64	},
	{"Thi™n NhÀn Ki’m",			3,		65	},
	{"HÁn Thi’t BÊng",			12,		66	},
	{"Nhπn Linh Th≠¨ng",		9,		67	},
	{"Kim Qua ChÔy",			18,		68	},
	{"HÂ ßi÷p Song ßao",		15,		69	},
	{"§ Kim Tri“n ThÒ",			19,		45	},
	{"Huy“n Thi’t Tri“n ThÒ",	19,		45	},
	{"Loπi ∏m Kh›",				19,		46	},
	{"ßπi Phong ßao",			22,		73	},
	{"Huy“n Thi’t Ki’m",		20,		74	},
	{"ßπi Phong ßao",			22,		75	},
	{"Huy“n Thi’t Ki’m",		20,		76	},
	{"Kim C´ BÊng",				26,		77	},
	{"Ph∏ Thi™n K›ch",			24,		78	},
	{"Ph∏ Thi™n ChÔy",			30,		79	},
	{"Th´n NhÀt Tr∂m",			28,		80	},
	{"MÀt V©n Tri“n ThÒ",		19,		45	},
	{"MÀt V©n Tri“n ThÒ",		19,		45	},
	{"Loπi ∏m Kh›",				19,		46	},
};

static	const int	s_nFlashCount = sizeof(s_FlashTable)/sizeof(KFlashTable);

KItemChangeRes	g_ItemChangeRes;
BOOL KItemChangeRes::Init()
{
	if (!m_MeleeWeapon.Load(CHANGERES_MELEE_FILE))
		return FALSE;
	if (!m_RangeWeapon.Load(CHANGERES_RANGE_FILE))
		return FALSE;
	if (!m_Armor.Load(CHANGERES_ARMOR_FILE))
		return FALSE;
	if (!m_Helm.Load(CHANGERES_HELM_FILE))
		return FALSE;
	if (!m_Horse.Load(CHANGERES_HORSE_FILE))
		return FALSE;
	if (!m_Gold.Load(CHANGERES_GOLD_FILE))
		return FALSE;
	if (!m_Hoods.Load(CHANGERES_HOODS_FILE))
		return FALSE;
	if (!m_Cloak.Load(CHANGERES_CLOAK_FILE))
		return FALSE;
	return TRUE;
}

int	KItemChangeRes::GetWeaponRes(int nDetail, int nParti, int nLevel, unsigned uFlash)
{
	int nRet;
	if (nLevel == 0)
	{
		m_MeleeWeapon.GetInteger(2, 2, 2, &nRet);
		return nRet - 2;
	}

	int nRow = nParti * 10 + nLevel + 2;
	switch(nDetail)
	{
	case equip_meleeweapon:
		m_MeleeWeapon.GetInteger(nRow, 2, 2, &nRet);
		break;
	case equip_rangeweapon:
		m_RangeWeapon.GetInteger(nRow - 1, 2, 2, &nRet);	// √ª”–ƒ¨»œ◊¥Ã¨
		break;
	}
	nRet -= 2;
	if(uFlash)
	{
		for(int i=0; i< s_nFlashCount; i++)
		{
			if(nRet == s_FlashTable[i].NormalRes)
				nRet = s_FlashTable[i].FlashRes;
		}
	}
	return nRet;
}

int	KItemChangeRes::GetArmorRes(int nParti, int nLevel)
{
	int nRet;
	if (nLevel == 0)
	{
		m_Armor.GetInteger(2, 2, 19, &nRet);
		return nRet - 2;
	}
	int nRow = nParti * 10 + nLevel + 2;
	m_Armor.GetInteger(nRow, 2, 19, &nRet);
	return nRet - 2;
}

int	KItemChangeRes::GetHelmRes(int nParti, int nLevel)
{
	int nRet;
	if (nLevel == 0)
	{
		m_Helm.GetInteger(2, 2, 19, &nRet);
		return nRet - 2;
	}
	int nRow = nParti * 10 + nLevel + 2;
	m_Helm.GetInteger(nRow, 2, 19, &nRet);
	return nRet - 2;
}

int KItemChangeRes::GetHorseRes(int nParti, int nLevel)
{
	int nRet;
	if (nLevel == 0)
	{
		return -1;
	}
	int nRow = nParti * 10 + nLevel + 2;
	m_Horse.GetInteger(nRow, 2, 2, &nRet);
	return nRet - 2;
}

int	KItemChangeRes::GetGoldItemRes(int nDetail, int nRow, unsigned uFlash/* = 0*/)
{
	int nRet;
	char szRow[16];
	char pszInfo[6];
	itoa(nRow + 1, szRow, 10);
	m_Gold.GetString(szRow, "≤øº˛±‡∫≈", "19", pszInfo, sizeof(pszInfo));
	nRet = atoi(pszInfo);
	nRet -= 2;

	if(uFlash)
	{
		switch(nDetail)
		{
			case equip_meleeweapon:
			case equip_rangeweapon:
				for(int i=0; i< s_nFlashCount; i++)
				{
					if(nRet == s_FlashTable[i].NormalRes)
						nRet = s_FlashTable[i].FlashRes;
				}
				break;
		}
	}
	return nRet;
}

int	KItemChangeRes::GetHoodsRes(int nParti)
{
	int nRet;
	int nRow = nParti + 2;
	m_Hoods.GetInteger(nRow, 2, 2, &nRet);

	return nRet - 2;
}

int	KItemChangeRes::GetCloakRes(int nParti)
{
	int nRet;
	int nRow = nParti + 2;
	m_Cloak.GetInteger(nRow, 2, 2, &nRet);

	return nRet - 2;
}