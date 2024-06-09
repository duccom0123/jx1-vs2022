//---------------------------------------------------------------------------
// Sword3 Engine (c) 2002 by Kingsoft
//
// File:	KPlayerFaction.cpp
// Date:	2002.09.26
// Code:	±ﬂ≥«¿À◊”
// Desc:	PlayerFaction Class
//---------------------------------------------------------------------------

#include	"KCore.h"
#include	"MyAssert.H"
#include	"GameDataDef.h"
#include	"KFaction.h"
#include	"KPlayerFaction.h"

#define		FACTION_NEW			"MÌi nhÀp giang hÂ "
#define		FACTION_OLD			"Giang hÂ hi÷p kh∏ch"

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫ππ‘Ï∫Ø ˝
//---------------------------------------------------------------------------
KPlayerFaction::KPlayerFaction()
{
	Release();
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫«Âø’
//---------------------------------------------------------------------------
void	KPlayerFaction::Release()
{
	m_nCurFaction = -1;
	m_nFirstAddFaction = -1;
	m_nAddTimes = 0;
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫∏˘æ›ÕÊº“µƒŒÂ–– Ù–‘»∑∂®ÕÊº“√≈≈… ˝æ›
//---------------------------------------------------------------------------
void	KPlayerFaction::SetSeries(int nSeries)
{
	return;
/*
	if (nSeries < series_metal || nSeries >= series_num)
		return;
	int		nID;
	for (int i = 0; i < FACTIONS_PRR_SERIES; i++)
	{
		nID = g_Faction.GetID(nSeries, i);
		_ASSERT(nID >= 0);
		if (m_sSkillOpen[i].m_nID != nID)
		{
			m_sSkillOpen[i].m_nID = nID;
			m_sSkillOpen[i].m_nOpenLevel = 0;
		}
	}
*/
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫º”»Î√≈≈…
//---------------------------------------------------------------------------
BOOL	KPlayerFaction::AddFaction(int nSeries, char *lpszFactionName)
{
	int		nID;

	nID = g_Faction.GetID(nSeries, lpszFactionName);
	if (nID < 0)
		return FALSE;
	return AddFaction(nSeries, nID);
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫º”»Î√≈≈…
//---------------------------------------------------------------------------
BOOL	KPlayerFaction::AddFaction(int nSeries, int nFactionID)
{
	if (nSeries < series_metal || nSeries >= series_num)
		return FALSE;
	if (nFactionID < HUASHANID && (nFactionID < nSeries * FACTIONS_PRR_SERIES || nFactionID >= (nSeries + 1) * FACTIONS_PRR_SERIES))
		return FALSE;
	if (nFactionID == HUASHANID && nSeries != series_water)
		return FALSE;
	SetSeries(nSeries);
	m_nCurFaction = nFactionID;
	m_nAddTimes++;
	if (m_nAddTimes == 1)
		m_nFirstAddFaction = nFactionID;

	return TRUE;
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫¿Îø™µ±«∞√≈≈…
//---------------------------------------------------------------------------
void	KPlayerFaction::LeaveFaction()
{
	m_nCurFaction = -1;
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫ªÒµ√µ±«∞√≈≈…’Û”™
//---------------------------------------------------------------------------
int		KPlayerFaction::GetGurFactionCamp()
{
	if (m_nCurFaction < 0)
	{
		if (m_nAddTimes == 0)
			return camp_begin;
		else
			return camp_free;
	}
	else
	{
		if (g_Faction.GetCamp(m_nCurFaction) >= 0)
			return g_Faction.GetCamp(m_nCurFaction);
		else
			return camp_begin;
	}
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫ªÒµ√µ±«∞√≈≈…
//---------------------------------------------------------------------------
int		KPlayerFaction::GetCurFactionNo()
{
	return m_nCurFaction;
}

//---------------------------------------------------------------------------
//	π¶ƒ‹£∫ªÒµ√µ±«∞√≈≈…
//---------------------------------------------------------------------------
void	KPlayerFaction::GetCurFactionName(char *lpszGetName)
{
	if (!lpszGetName)
		return;

	if (this->m_nCurFaction == -1)
	{
		if (this->m_nAddTimes == 0)
		{
			lpszGetName[0] = 0;
		}
		else
		{
			strcpy(lpszGetName, FACTION_NEW);
		}
	}
	else
	{
		strcpy(lpszGetName, g_Faction.m_sAttribute[m_nCurFaction].m_szName);
	}

	return;
}


void	KPlayerFaction::GetCurFactionShowName(char *lpszGetName)
{
	if (!lpszGetName)
		return;

	if (this->m_nCurFaction == -1)
	{
		if (this->m_nAddTimes == 0)
		{
			lpszGetName[0] = 0;
		}
		else
		{
			strcpy(lpszGetName, FACTION_NEW);
		}
	}
	else
	{
		strcpy(lpszGetName, g_Faction.m_sAttribute[m_nCurFaction].m_szShowName);
	}

	return;
}



