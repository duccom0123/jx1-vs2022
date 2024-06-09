#ifndef	_KNPCTEMPLATE_H
#define	_KNPCTEMPLATE_H

#include "KCore.h"
#include "KNpc.h"

class KNpcTemplate
{
public:
private:
public:
	char	Name[32];
	DWORD	m_Kind;
	int		m_Camp;
	int		m_Series;
	int		m_HeadImage;
	int		m_bClientOnly;
	int		m_CorpseSettingIdx;
	int		m_DeathFrame;
	int		m_WalkFrame;
	int		m_StandFrame;
	int		m_StandFrame1;
	int		m_RunFrame;
	int		m_HurtFrame;
	int		m_WalkSpeed;
	int		m_AttackFrame;
	int		m_CastFrame;
	int		m_RunSpeed;
	
#ifdef _SERVER	
	int		m_AiMode;
	int		m_AiParam[MAX_AI_PARAM - 1];
	int		m_FireResistMax;
	int		m_ColdResistMax;
	int		m_LightResistMax;
	int		m_PoisonResistMax;
	int		m_PhysicsResistMax;
	int		m_ActiveRadius;
	int		m_VisionRadius;
	BYTE	m_AIMAXTime;
	int		m_HitRecover;
	int		m_ReviveFrame;

	int		m_Experience;
	int		m_LifeReplenish;
	int		m_AttackRating;
	int		m_Defend;
	KMagicAttrib		m_PhysicsDamage;
	int		m_RedLum;
	int		m_GreenLum;
	int		m_BlueLum;
	int		m_FireResist;
	int		m_ColdResist;
	int		m_LightResist;
	int		m_PoisonResist;
	int		m_PhysicsResist;
	KSkillList m_SkillList;
#endif
	
#ifndef _SERVER	
	int		m_ArmorType;
	int		m_HelmType;
	int		m_WeaponType;
	int		m_HorseType;
	int		m_bRideHorse;
#endif
	
	int		m_NpcSettingIdx;
	BOOL	m_bHaveLoadedFromTemplate;
	int		m_nStature;	
	
public:
	void	Init(int nNpcTemplateId);
	KNpcTemplate(){	m_bHaveLoadedFromTemplate = FALSE;};
};

extern KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE];

#endif

