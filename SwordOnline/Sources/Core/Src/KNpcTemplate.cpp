#include "KCore.h"

#include "KNpcTemplate.h"

void	KNpcTemplate::Init(int nNpcTemplateId)
{
	if (nNpcTemplateId < 0 ) return;
	int nNpcTempRow = nNpcTemplateId + 2;

	m_NpcSettingIdx = nNpcTemplateId;

	g_NpcSetting.GetString(nNpcTempRow, "Name", "", Name, sizeof(Name));
	g_NpcSetting.GetInteger(nNpcTempRow, "Kind", 0, (int *)&m_Kind);
	g_NpcSetting.GetInteger(nNpcTempRow, "Camp", 0, &m_Camp);
	g_NpcSetting.GetInteger(nNpcTempRow, "Series", 0, &m_Series);
	
	
	g_NpcSetting.GetInteger(nNpcTempRow, "HeadImage",	0, &m_HeadImage);
	g_NpcSetting.GetInteger(nNpcTempRow, "ClientOnly",	0, &m_bClientOnly);
	g_NpcSetting.GetInteger(nNpcTempRow, "CorpseIdx",	0, &m_CorpseSettingIdx);
	
	g_NpcSetting.GetInteger(nNpcTempRow, "DeathFrame",	12, &m_DeathFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkFrame",	15, &m_WalkFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "RunFrame",	15, &m_RunFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "HurtFrame",	10, &m_HurtFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "WalkSpeed",	5, &m_WalkSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "AttackSpeed",	20, &m_AttackFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "CastSpeed",	20, &m_CastFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "RunSpeed",	10, &m_RunSpeed);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame",	15, &m_StandFrame);
	g_NpcSetting.GetInteger(nNpcTempRow, "StandFrame1", 15, &m_StandFrame1);
	g_NpcSetting.GetInteger(nNpcTempRow, "Stature",		0,  &m_nStature);
	
#ifdef _SERVER	
	g_NpcSetting.GetInteger(nNpcTempRow, "AIMode",	0, &m_AiMode);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam1",	0, &m_AiParam[0]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam2",	0, &m_AiParam[1]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam3",	0, &m_AiParam[2]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam4",	0, &m_AiParam[3]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam5",	0, &m_AiParam[4]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam6",	0, &m_AiParam[5]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam7",	0, &m_AiParam[6]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam8",	0, &m_AiParam[7]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam9",	0, &m_AiParam[8]);
	g_NpcSetting.GetInteger(nNpcTempRow, "AIParam10",	5, &m_AiParam[9]);

	g_NpcSetting.GetInteger(nNpcTempRow, "FireResistMax",	0, &m_FireResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "ColdResistMax",	0, &m_ColdResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "LightResistMax",	0, &m_LightResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "PoisonResistMax",	0, &m_PoisonResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "PhysicsResistMax",	0, &m_PhysicsResistMax);
	g_NpcSetting.GetInteger(nNpcTempRow, "ActiveRadius", 30, &m_ActiveRadius);
	g_NpcSetting.GetInteger(nNpcTempRow, "VisionRadius", 40, &m_VisionRadius);
	
	int nAIMaxTime = 0;
	g_NpcSetting.GetInteger(nNpcTempRow, "AIMaxTime", 25, (int*)&nAIMaxTime);
	m_AIMAXTime = (BYTE)nAIMaxTime;
	
	g_NpcSetting.GetInteger(nNpcTempRow, "HitRecover", 0, &m_HitRecover);
	g_NpcSetting.GetInteger(nNpcTempRow, "ReviveFrame", 2400, &m_ReviveFrame);
#else
	g_NpcSetting.GetInteger(nNpcTempRow, "ArmorType", 0, &m_ArmorType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HelmType", 0, &m_HelmType);
	g_NpcSetting.GetInteger(nNpcTempRow, "WeaponType", 0, &m_WeaponType);
	g_NpcSetting.GetInteger(nNpcTempRow, "HorseType", -1, &m_HorseType);
	g_NpcSetting.GetInteger(nNpcTempRow, "RideHorse",0, &m_bRideHorse);
#endif

	int nParam;
	int nParam2;
#ifdef _SERVER				
	g_NpcSetting.GetInteger(nNpcTempRow, "Skill1",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level1", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(1, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "Skill2",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level2", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(2, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "Skill3",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level3", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(3, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "Skill4",	0, &nParam);
	g_NpcSetting.GetInteger(nNpcTempRow, "Level4", 0, &nParam2);
	if (nParam && nParam2)
		m_SkillList.SetNpcSkill(4, nParam, nParam2);

	g_NpcSetting.GetInteger(nNpcTempRow, "ExpParam", 1, &nParam);
	m_Experience = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "LifeReplenish", 0, &nParam);
	m_LifeReplenish = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "ARParam", 1, &nParam);	
	m_AttackRating = nParam;
	if (m_AttackRating == 0) 
		m_AttackRating = 100;

	g_NpcSetting.GetInteger(nNpcTempRow, "DefenseParam", 1, &nParam);
	m_Defend = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "MinDamageParam", 1, &nParam);
	m_PhysicsDamage.nValue[0] = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "MaxDamageParam", 1, &nParam);
	m_PhysicsDamage.nValue[2] = nParam;

	
	g_NpcSetting.GetInteger(nNpcTempRow, "RedLum", 0, &nParam);
	m_RedLum = nParam;
	
	g_NpcSetting.GetInteger(nNpcTempRow, "GreenLum", 0, &nParam);
	m_GreenLum = nParam;
	
	g_NpcSetting.GetInteger(nNpcTempRow, "BlueLum", 0, &nParam);
	m_BlueLum = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "FireResist", 0, &nParam);
	m_FireResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "ColdResist", 0, &nParam);
	m_ColdResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "LightResist", 0, &nParam);
	m_LightResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "PoisonResist", 0, &nParam);
	m_PoisonResist = nParam;

	g_NpcSetting.GetInteger(nNpcTempRow, "PhysicsResist", 0, &nParam);
	m_PhysicsResist = nParam;
#endif
}
