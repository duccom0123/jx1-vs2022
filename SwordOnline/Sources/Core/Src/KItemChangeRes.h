//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemChangeRes.h
// Date:	2002.12
// Code:	Spe
// Desc:	Header File
//---------------------------------------------------------------------------

#ifndef	KItemChangeResH
#define	KItemChangeResH

#include "KTabFile.h"

class KItemChangeRes
{
private:
	KTabFile	m_MeleeWeapon;
	KTabFile	m_RangeWeapon;
	KTabFile	m_Armor;
	KTabFile	m_Helm;
	KTabFile	m_Horse;
	KTabFile	m_Gold;
	KTabFile	m_Hoods;
	KTabFile	m_Cloak;
public:
	BOOL		Init();
	int			GetWeaponRes(int nDetail, int nParti, int nLevel, unsigned int uFlash);
	int			GetArmorRes(int nParti, int nLevel);
	int			GetHelmRes(int nParti, int nLevel);
	int			GetHorseRes(int nParti, int nLevel);
	int			GetGoldItemRes(int nDetail, int nRow, unsigned int uFlash = 0);
	int			GetHoodsRes(int nParti);
	int			GetCloakRes(int nParti);
};

extern KItemChangeRes	g_ItemChangeRes;
#endif
