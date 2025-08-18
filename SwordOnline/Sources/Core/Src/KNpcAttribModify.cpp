#include "KCore.h"
#include "KNpc.h"
#include "KPlayer.h"
#include "KMath.h"
#include "KNpcAttribModify.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

KNpcAttribModify	g_NpcAttribModify;
KNpcAttribModify::KNpcAttribModify()
{
	ZeroMemory(ProcessFunc, sizeof(ProcessFunc));
	ProcessFunc[magic_physicsresmax_p] = &KNpcAttribModify::PhysicsResMaxP;
	ProcessFunc[magic_coldresmax_p] = &KNpcAttribModify::ColdResMaxP;
	ProcessFunc[magic_fireresmax_p] = &KNpcAttribModify::FireResMaxP;
	ProcessFunc[magic_lightingresmax_p] = &KNpcAttribModify::LightingResMaxP;
	ProcessFunc[magic_poisonresmax_p] = &KNpcAttribModify::PoisonResMaxP;
	ProcessFunc[magic_allresmax_p] = &KNpcAttribModify::AllResMaxP;
	ProcessFunc[magic_lifepotion_v] = &KNpcAttribModify::LifePotionV;
	ProcessFunc[magic_manapotion_v] = &KNpcAttribModify::ManaPotionV;
	ProcessFunc[magic_meleedamagereturn_v] = &KNpcAttribModify::MeleeDamageReturnV;
	ProcessFunc[magic_meleedamagereturn_p] = &KNpcAttribModify::MeleeDamageReturnP;
	ProcessFunc[magic_rangedamagereturn_v] = &KNpcAttribModify::RangeDamageReturnV;
	ProcessFunc[magic_rangedamagereturn_p] = &KNpcAttribModify::RangeDamageReturnP;
	ProcessFunc[magic_damage2addmana_p] = &KNpcAttribModify::Damage2AddManaP;
	ProcessFunc[magic_adddefense_v] = &KNpcAttribModify::ArmorDefenseV;			// 与装备加防御统一处理
	ProcessFunc[magic_poisonenhance_p] = &KNpcAttribModify::PoisonEnhanceP;
	ProcessFunc[magic_lightingenhance_p] = &KNpcAttribModify::LightingEnhanceP;
	ProcessFunc[magic_fireenhance_p] = &KNpcAttribModify::FireEnhanceP;
	ProcessFunc[magic_coldenhance_p] = &KNpcAttribModify::ColdEnhanceP;
	ProcessFunc[magic_armordefense_v] = &KNpcAttribModify::ArmorDefenseV;
	ProcessFunc[magic_lifemax_v] = &KNpcAttribModify::LifeMaxV;
	ProcessFunc[magic_lifemax_yan_v] = &KNpcAttribModify::LifeMaxV;
	ProcessFunc[magic_lifemax_p] = &KNpcAttribModify::LifeMaxP;
	ProcessFunc[magic_lifemax_yan_p] = &KNpcAttribModify::LifeMaxP;
	ProcessFunc[magic_life_v] = &KNpcAttribModify::LifeV;
	ProcessFunc[magic_lifereplenish_v] = &KNpcAttribModify::LifeReplenishV;
	ProcessFunc[magic_manamax_v] = &KNpcAttribModify::ManaMaxV;
	ProcessFunc[magic_manamax_yan_v] = &KNpcAttribModify::ManaMaxV;
	ProcessFunc[magic_manamax_p] = &KNpcAttribModify::ManaMaxP;
	ProcessFunc[magic_manamax_yan_p] = &KNpcAttribModify::ManaMaxP;
	ProcessFunc[magic_mana_v] = &KNpcAttribModify::ManaV;
	ProcessFunc[magic_manareplenish_v] = &KNpcAttribModify::ManaReplenishV;
	ProcessFunc[magic_staminamax_v] = &KNpcAttribModify::StaminaMaxV;
	ProcessFunc[magic_staminamax_p] = &KNpcAttribModify::StaminaMaxP;
	ProcessFunc[magic_stamina_v] = &KNpcAttribModify::StaminaV;
	ProcessFunc[magic_staminareplenish_v] = &KNpcAttribModify::StaminaReplenishV;
	ProcessFunc[magic_strength_v] = &KNpcAttribModify::StrengthV;
	ProcessFunc[magic_dexterity_v] = &KNpcAttribModify::DexterityV;
	ProcessFunc[magic_vitality_v] = &KNpcAttribModify::VitalityV;
	ProcessFunc[magic_energy_v] = &KNpcAttribModify::EnergyV;
	ProcessFunc[magic_poisonres_p] = &KNpcAttribModify::PoisonresP;
	ProcessFunc[magic_poisonres_yan_p] = &KNpcAttribModify::PoisonresP;
	ProcessFunc[magic_fireres_p] = &KNpcAttribModify::FireresP;
	ProcessFunc[magic_fireres_yan_p] = &KNpcAttribModify::FireresP;
	ProcessFunc[magic_lightingres_p] = &KNpcAttribModify::LightingresP;
	ProcessFunc[magic_lightingres_yan_p] = &KNpcAttribModify::LightingresP;
	ProcessFunc[magic_physicsres_p] = &KNpcAttribModify::PhysicsresP;
	ProcessFunc[magic_physicsres_yan_p] = &KNpcAttribModify::PhysicsresP;
	ProcessFunc[magic_coldres_p] = &KNpcAttribModify::ColdresP;
	ProcessFunc[magic_coldres_yan_p] = &KNpcAttribModify::ColdresP;
	ProcessFunc[magic_freezetimereduce_p] = &KNpcAttribModify::FreezeTimeReduceP;
	ProcessFunc[magic_burntimereduce_p] = &KNpcAttribModify::BurnTimeReduceP;
	ProcessFunc[magic_poisontimereduce_p] = &KNpcAttribModify::PoisonTimeReduceP;
	ProcessFunc[magic_poisondamagereduce_v] = &KNpcAttribModify::PoisonDamageReduceV;
	ProcessFunc[magic_stuntimereduce_p] = &KNpcAttribModify::StunTimeReduceP;
	ProcessFunc[magic_fastwalkrun_p] = &KNpcAttribModify::FastWalkRunP;
	ProcessFunc[magic_fastwalkrun_yan_p] = &KNpcAttribModify::FastWalkRunP;
	ProcessFunc[magic_visionradius_p] = &KNpcAttribModify::VisionRadiusP;
	ProcessFunc[magic_fasthitrecover_v] = &KNpcAttribModify::FastHitRecoverV;
	ProcessFunc[magic_fasthitrecover_yan_v] = &KNpcAttribModify::FastHitRecoverV;
	ProcessFunc[magic_allres_p] = &KNpcAttribModify::AllresP;
	ProcessFunc[magic_allres_yan_p] = &KNpcAttribModify::AllresP;
	ProcessFunc[magic_attackrating_v] = &KNpcAttribModify::AttackRatingV;
	ProcessFunc[magic_attackratingenhance_v] = &KNpcAttribModify::AttackRatingV;
	ProcessFunc[magic_attackrating_p] = &KNpcAttribModify::AttackRatingP;
	ProcessFunc[magic_attackratingenhance_p] = &KNpcAttribModify::AttackRatingP;
	ProcessFunc[magic_attackspeed_v] = &KNpcAttribModify::AttackSpeedV;
	ProcessFunc[magic_attackspeed_yan_v] = &KNpcAttribModify::AttackSpeedV;
	ProcessFunc[magic_castspeed_v] = &KNpcAttribModify::CastSpeedV;
	ProcessFunc[magic_castspeed_yan_v] = &KNpcAttribModify::CastSpeedV;
	ProcessFunc[magic_addphysicsdamage_v] = &KNpcAttribModify::AddPhysicsDamageV;
	ProcessFunc[magic_addfiredamage_v] = &KNpcAttribModify::AddFireDamageV;
	ProcessFunc[magic_addcolddamage_v] = &KNpcAttribModify::AddColdDamageV;
	ProcessFunc[magic_addlightingdamage_v] = &KNpcAttribModify::AddLightingDamageV;
	ProcessFunc[magic_addpoisondamage_v] = &KNpcAttribModify::AddPoisonDamageV;
	ProcessFunc[magic_addphysicsdamage_p] = &KNpcAttribModify::AddPhysicsDamageP;
	ProcessFunc[magic_slowmissle_b] = &KNpcAttribModify::SlowMissleB;
	ProcessFunc[magic_changecamp_b] = &KNpcAttribModify::ChangeCampV;
	ProcessFunc[magic_physicsarmor_v] = &KNpcAttribModify::PhysicsArmorV;
	ProcessFunc[magic_coldarmor_v] = &KNpcAttribModify::ColdArmorV;
	ProcessFunc[magic_firearmor_v] = &KNpcAttribModify::FireArmorV;
	ProcessFunc[magic_poisonarmor_v] = &KNpcAttribModify::PoisonArmorV;
	ProcessFunc[magic_lightingarmor_v] = &KNpcAttribModify::LightingArmorV;
	ProcessFunc[magic_lucky_v] = &KNpcAttribModify::LuckyV;
	ProcessFunc[magic_steallife_p] = &KNpcAttribModify::StealLifeP;
	ProcessFunc[magic_steallifeenhance_p] = &KNpcAttribModify::StealLifeP;
	ProcessFunc[magic_stealstamina_p] = &KNpcAttribModify::StealStaminaP;
	ProcessFunc[magic_stealstaminaenhance_p] = &KNpcAttribModify::StealStaminaP;
	ProcessFunc[magic_stealmana_p] = &KNpcAttribModify::StealManaP;
	ProcessFunc[magic_stealmanaenhance_p] = &KNpcAttribModify::StealManaP;
	ProcessFunc[magic_allskill_v] = &KNpcAttribModify::AllSkillV;
	ProcessFunc[magic_metalskill_v] = &KNpcAttribModify::MetalSkillV;
	ProcessFunc[magic_woodskill_v] = &KNpcAttribModify::WoodSkillV;
	ProcessFunc[magic_waterskill_v] = &KNpcAttribModify::WaterSkillV;
	ProcessFunc[magic_fireskill_v] = &KNpcAttribModify::FireSkillV;
	ProcessFunc[magic_earthskill_v] = &KNpcAttribModify::EarthSkillV;
	ProcessFunc[magic_fatallystrikeres_p] = &KNpcAttribModify::FatallyStrikeResP;
	ProcessFunc[magic_deadlystrikeenhance_p] = &KNpcAttribModify::DeadlyStrikeEnhanceP;
	ProcessFunc[magic_fatallystrikeenhance_p] = &KNpcAttribModify::FatallyStrikeEnhanceP;
	ProcessFunc[magic_manashield_p] = &KNpcAttribModify::ManaShieldP;
	ProcessFunc[magic_addcoldmagic_v] = &KNpcAttribModify::AddColdMagicV;
	ProcessFunc[magic_addfiremagic_v] = &KNpcAttribModify::AddFireMagicV;
	ProcessFunc[magic_addlightingmagic_v] = &KNpcAttribModify::AddLightingMagicV;
	ProcessFunc[magic_addpoisonmagic_v] = &KNpcAttribModify::AddPoisonMagicV;
	ProcessFunc[magic_addphysicsmagic_v] = &KNpcAttribModify::AddPhysicsMagicV;

	ProcessFunc[magic_dynamicmagicshield_v] = &KNpcAttribModify::DynamicMagicShieldV;
	ProcessFunc[magic_staticmagicshield_p] = &KNpcAttribModify::StaticMagicShieldP;
	ProcessFunc[magic_expenhance_p] = &KNpcAttribModify::ExpEnhanceP;
	ProcessFunc[magic_lifereplenish_p] = &KNpcAttribModify::LifeReplenishP;
	ProcessFunc[magic_ignoreskill_p] = &KNpcAttribModify::IgnoreSkillP;
	ProcessFunc[magic_poisondamagereturn_p] = &KNpcAttribModify::PoisonDamageReturnP;
	ProcessFunc[magic_returnskill_p] = &KNpcAttribModify::ReturnSkillP;
	ProcessFunc[magic_poison2decmana_p] = &KNpcAttribModify::Poison2DecManaP;
	ProcessFunc[magic_autodeathskill] = &KNpcAttribModify::AutoDeathSkill;
	ProcessFunc[magic_autoattackskill] = &KNpcAttribModify::AutoAttackSkill;
	ProcessFunc[magic_hide] = &KNpcAttribModify::Hide;
	ProcessFunc[magic_ignorenegativestate_p] = &KNpcAttribModify::IgnoreNegativeStateP;
	ProcessFunc[magic_autoreplyskill] = &KNpcAttribModify::AutoReplySkill;
	ProcessFunc[magic_autorescueskill] = &KNpcAttribModify::AutoRescueSkill;
	ProcessFunc[magic_returnres_p] = &KNpcAttribModify::ReturnResP;
	ProcessFunc[magic_skill_enhance] = &KNpcAttribModify::SkillEnhanceP;
	ProcessFunc[magic_five_elements_enhance_v] = &KNpcAttribModify::FiveElementsEnhanceV;
	ProcessFunc[magic_five_elements_resist_v] = &KNpcAttribModify::FiveElementsResistV;	
	ProcessFunc[magic_frozen_action] = &KNpcAttribModify::FrozenAction;
	ProcessFunc[magic_walkrunshadow] = &KNpcAttribModify::WalkRunShadow;
	ProcessFunc[magic_randmove] = &KNpcAttribModify::RandMove;
	ProcessFunc[magic_manatoskill_enhance] = &KNpcAttribModify::ManaToSkillEnhanceP;
}

KNpcAttribModify::~KNpcAttribModify()
{
}

void KNpcAttribModify::ModifyAttrib(KNpc* pNpc, void* pData)
{
	if (!pData || !pNpc)
		return;

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nAttribType < 0 || pMagic->nAttribType >= magic_normal_end || NULL == ProcessFunc[pMagic->nAttribType])
		return;
	
	(this->*ProcessFunc[pMagic->nAttribType])(pNpc, pData);
}

void KNpcAttribModify::AddColdDamageV(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentColdDamage.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentColdDamage.nValue[0] > 0 && pNpc->m_CurrentColdDamage.nValue[2] > 0)
		pNpc->m_CurrentColdDamage.nValue[1] = COLD_DAMAGE_TIME;
	else
	{
		pNpc->m_CurrentColdDamage.nValue[0] = 0;
		pNpc->m_CurrentColdDamage.nValue[1] = 0;
		pNpc->m_CurrentColdDamage.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddColdMagicV(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdMagic.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentColdMagic.nValue[2] += pMagic->nValue[0];
	if (pNpc->m_CurrentColdMagic.nValue[0] > 0 && pNpc->m_CurrentColdMagic.nValue[2] > 0)
		pNpc->m_CurrentColdMagic.nValue[1] = COLD_DAMAGE_TIME;
	else
	{
		pNpc->m_CurrentColdMagic.nValue[0] = 0;
		pNpc->m_CurrentColdMagic.nValue[1] = 0;
		pNpc->m_CurrentColdMagic.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddFireDamageV(KNpc* pNpc, void* pData)
{	
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentFireDamage.nValue[2] += pMagic->nValue[0];
}

void KNpcAttribModify::AddFireMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireMagic.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentFireMagic.nValue[2] += pMagic->nValue[0];
}


void KNpcAttribModify::AddLightingDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightDamage.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentLightDamage.nValue[2] += pMagic->nValue[0];
}

void KNpcAttribModify::AddLightingMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightMagic.nValue[0] += pMagic->nValue[0];
	pNpc->m_CurrentLightMagic.nValue[2] += pMagic->nValue[0];
}

void KNpcAttribModify::AddPhysicsDamageP(KNpc* pNpc, void* pData)
{
#define WEAPON_ALL			((MAX_MELEE_WEAPON) + 0)
#define	WEAPON_RANGE_ALL	((MAX_MELEE_WEAPON) + 1)
#define	WEAPON_MELEE_ALL	((MAX_MELEE_WEAPON) + 2)
#define	WEAPON_NONE			((MAX_MELEE_WEAPON) + 3)

	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	int nType = abs(pMagic->nValue[2]);
	
	if (nType == WEAPON_ALL)
	{
		for (int i = 0; i < MAX_MELEE_WEAPON; i++)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_RANGE_ALL)
	{
		pNpc->m_CurrentRangeEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_NONE)
	{
		pNpc->m_CurrentHandEnhance += pMagic->nValue[0];
	}
	else if (nType == WEAPON_MELEE_ALL)
	{
		for (int i = 0; i < MAX_MELEE_WEAPON; i++)
		{
			pNpc->m_CurrentMeleeEnhance[i] += pMagic->nValue[0];
		}
	}
	else if (nType >= 0 && nType < MAX_MELEE_WEAPON)
	{
		pNpc->m_CurrentMeleeEnhance[nType] += pMagic->nValue[0];
	}
}

void KNpcAttribModify::AddPoisonDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonDamage.nValue[0] += pMagic->nValue[0];

	if (pNpc->m_CurrentPoisonDamage.nValue[0] > 0)
	{
		pNpc->m_CurrentPoisonDamage.nValue[1] = POISON_DAMAGE_TIME;//pMagic->nValue[1];
		pNpc->m_CurrentPoisonDamage.nValue[2] = POISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
	}
	else
	{
		pNpc->m_CurrentPoisonDamage.nValue[0] = 0;
		pNpc->m_CurrentPoisonDamage.nValue[1] = 0;
		pNpc->m_CurrentPoisonDamage.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddPoisonMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonMagic.nValue[0] += pMagic->nValue[0];

	if (pNpc->m_CurrentPoisonMagic.nValue[0] > 0)
	{
		pNpc->m_CurrentPoisonMagic.nValue[1] = POISON_DAMAGE_TIME;//pMagic->nValue[1];
		pNpc->m_CurrentPoisonMagic.nValue[2] = POISON_DAMAGE_INTERVAL;//pMagic->nValue[2];
	}
	else
	{
		pNpc->m_CurrentPoisonMagic.nValue[0] = 0;
		pNpc->m_CurrentPoisonMagic.nValue[1] = 0;
		pNpc->m_CurrentPoisonMagic.nValue[2] = 0;
	}
}

void KNpcAttribModify::AddPhysicsDamageV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAddPhysicsDamage += pMagic->nValue[0];
}

void KNpcAttribModify::AddPhysicsMagicV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAddPhysicsMagic += pMagic->nValue[0];
}


void KNpcAttribModify::AllresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResist += pMagic->nValue[0];
	pNpc->m_CurrentColdResist += pMagic->nValue[0];
	pNpc->m_CurrentLightResist += pMagic->nValue[0];
	pNpc->m_CurrentPoisonResist += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsResist += pMagic->nValue[0];
}

void KNpcAttribModify::AllSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	ISkill * pSkill = NULL;
	int nSkillId = abs(pMagic->nValue[2]);
	int nAddLevel = pMagic->nValue[0];
	pNpc->m_SkillList.AllSkillV(nSkillId, nAddLevel);
}

void KNpcAttribModify::AttackRatingP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRating += pNpc->m_AttackRating * pMagic->nValue[0] / 100;
}

void KNpcAttribModify::AttackRatingV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackRating += pMagic->nValue[0];
}

void KNpcAttribModify::AttackSpeedV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentAttackSpeed += pMagic->nValue[0];
}

void KNpcAttribModify::BadStatusTimeReduceV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	return;
}

void KNpcAttribModify::BurnTimeReduceP(KNpc* pNpc, void* pData)
{
	return;
}

void KNpcAttribModify::CastSpeedV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentCastSpeed += pMagic->nValue[0];
}

void KNpcAttribModify::ChangeCampV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
	{
		if (pMagic->nValue[0] > 0 && pMagic->nValue[0] < camp_num)
			pNpc->SetCurrentCamp(pMagic->nValue[0]);
		else
			pNpc->SetCurrentCamp(pNpc->m_Camp);
	}
	
}

void KNpcAttribModify::ColdArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_ColdArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::ColdresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdResist += pMagic->nValue[0];
}

void KNpcAttribModify::DeadlyStrikeEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDeadlyStrikeEnhanceP += pMagic->nValue[0];
}

void KNpcAttribModify::DexterityV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	Player[pNpc->m_nPlayerIdx].ChangeCurDexterity(pMagic->nValue[0]);
}

void KNpcAttribModify::EarthSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_earth, pMagic->nValue[0]);
}

void KNpcAttribModify::FatallyStrikeResP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFatallyStrikeResP += pMagic->nValue[0];	
}

void KNpcAttribModify::EnergyV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	Player[pNpc->m_nPlayerIdx].ChangeCurEngergy(pMagic->nValue[0]);
}

void KNpcAttribModify::FastHitRecoverV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentHitRecover += pMagic->nValue[0];
}

void KNpcAttribModify::FastWalkRunP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentWalkSpeed += ((float)(pNpc->m_WalkSpeed * pMagic->nValue[0]) / 100);
	pNpc->m_CurrentRunSpeed += ((float)(pNpc->m_RunSpeed * pMagic->nValue[0]) / 100);
}

void KNpcAttribModify::FireArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_FireArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::FireSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_fire, pMagic->nValue[0]);
}

void KNpcAttribModify::FreezeTimeReduceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFreezeTimeReducePercent += pMagic->nValue[0];
}

void KNpcAttribModify::LifeMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeMax += pNpc->m_LifeMax * pMagic->nValue[0] / 100;
}

void KNpcAttribModify::LifeMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeMax += pMagic->nValue[0];
}

void KNpcAttribModify::LifeReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeReplenish += pMagic->nValue[0];
}

void KNpcAttribModify::LifeV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLife += pMagic->nValue[0];
}

void KNpcAttribModify::LightingArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_LightArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::LightingresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightResist += pMagic->nValue[0];
}

void KNpcAttribModify::LuckyV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->IsPlayer())
	{
		pNpc->m_CurrentLucky += pMagic->nValue[0];
	}
}

void KNpcAttribModify::ManaMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaMax += pNpc->m_ManaMax * pMagic->nValue[0] / 100;
}

void KNpcAttribModify::ManaMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaMax += pMagic->nValue[0];
}

void KNpcAttribModify::ManaReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaReplenish += pMagic->nValue[0];
}

void KNpcAttribModify::ManaV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMana += pMagic->nValue[0];
}

void KNpcAttribModify::ManaShieldP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_ManaShield.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::MeleeDamageReturnP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMeleeDmgRetPercent += pMagic->nValue[0];
}

void KNpcAttribModify::MeleeDamageReturnV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentMeleeDmgRet += pMagic->nValue[0];
}

void KNpcAttribModify::MetalSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_metal, pMagic->nValue[0]);
}

void KNpcAttribModify::PhysicsArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_PhysicsArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::PhysicsresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPhysicsResist += pMagic->nValue[0];
}

void KNpcAttribModify::Damage2AddManaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDamage2Mana += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonArmorV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_PoisonArmor.nValue[0] += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonDamageReduceV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_PoisonState.nValue[0] -= pMagic->nValue[0];
	if (pNpc->m_PoisonState.nValue[0] <= 0)
	{
		pNpc->m_PoisonState.nTime = 0;
	}
}

void KNpcAttribModify::PoisonresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonResist += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonTimeReduceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonTimeReducePercent += pMagic->nValue[0];
}

void KNpcAttribModify::RangeDamageReturnV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentRangeDmgRet += pMagic->nValue[0];
}

void KNpcAttribModify::RangeDamageReturnP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentRangeDmgRetPercent += pMagic->nValue[0];
}

void KNpcAttribModify::SlowMissleB(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	pNpc->m_CurrentSlowMissle += pMagic->nValue[0];

	if (pNpc->m_CurrentSlowMissle > 0)
		pNpc->m_CurrentSlowMissle = 1;
	else
		pNpc->m_CurrentSlowMissle = 0;
}

void KNpcAttribModify::StaminaMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaMax += pNpc->m_StaminaMax * pMagic->nValue[0] / 100;
}

void KNpcAttribModify::StaminaMaxV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaMax += pMagic->nValue[0];
}

void KNpcAttribModify::StaminaReplenishV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaGain += pMagic->nValue[0];
}

void KNpcAttribModify::StaminaV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStamina += pMagic->nValue[0];
}


void KNpcAttribModify::StealLifeP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeStolen += pMagic->nValue[0];
}

void KNpcAttribModify::StealManaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaStolen += pMagic->nValue[0];
}

void KNpcAttribModify::StealStaminaP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaminaStolen += pMagic->nValue[0];
}

void KNpcAttribModify::StrengthV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	Player[pNpc->m_nPlayerIdx].ChangeCurStrength(pMagic->nValue[0]);
}

void KNpcAttribModify::StunTimeReduceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStunTimeReducePercent += pMagic->nValue[0];
}

void KNpcAttribModify::VisionRadiusP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentVisionRadius += pMagic->nValue[0];
}

void KNpcAttribModify::VitalityV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->m_Kind != kind_player)
		return;
	if (pNpc->m_nPlayerIdx <= 0)
		return;
	Player[pNpc->m_nPlayerIdx].ChangeCurVitality(pMagic->nValue[0]);
}

void KNpcAttribModify::WaterSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_water, pMagic->nValue[0]);
}

void KNpcAttribModify::WoodSkillV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_SkillList.SeriesSkillV(series_wood, pMagic->nValue[0]);	
}

void KNpcAttribModify::FireresP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResist += pMagic->nValue[0];
}

void KNpcAttribModify::ArmorDefenseV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentDefend += pMagic->nValue[0];
}

void KNpcAttribModify::ColdEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::FireEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::LightingEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::LifePotionV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[1] <= 0)
		return;
	int nX1, nY1, nX2, nY2;
	nX1 = pNpc->m_LifeState.nValue[0];
	nY1 = pNpc->m_LifeState.nTime;
	nX2 = pMagic->nValue[0];
	nY2 = pMagic->nValue[1];
	pNpc->m_LifeState.nTime = max(nY1, nY2);
	pNpc->m_LifeState.nValue[0] = (nX1 * nY1 + nX2 * nY2) / pNpc->m_LifeState.nTime;
}

void KNpcAttribModify::ManaPotionV(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;

	if (pMagic->nValue[1] <= 0)
		return;
	int nX1, nY1, nX2, nY2;
	nX1 = pNpc->m_ManaState.nValue[0];
	nY1 = pNpc->m_ManaState.nTime;
	nX2 = pMagic->nValue[0];
	nY2 = pMagic->nValue[1];
	pNpc->m_ManaState.nTime = max(nY1, nY2);
	pNpc->m_ManaState.nValue[0] = (nX1 * nY1 + nX2 * nY2) / pNpc->m_ManaState.nTime;
}

void KNpcAttribModify::PhysicsResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPhysicsResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::ColdResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentColdResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::FireResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::LightingResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLightResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::AllResMaxP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFireResistMax += pMagic->nValue[0];
	pNpc->m_CurrentColdResistMax += pMagic->nValue[0];
	pNpc->m_CurrentLightResistMax += pMagic->nValue[0];
	pNpc->m_CurrentPoisonResistMax += pMagic->nValue[0];
	pNpc->m_CurrentPhysicsResistMax += pMagic->nValue[0];
}

void KNpcAttribModify::FatallyStrikeEnhanceP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFatallyStrikeEnhanceP += pMagic->nValue[0];	
}

void KNpcAttribModify::MixPoisonDamage(KMagicAttrib* pDes, KMagicAttrib* pSrc)
{
	int d1, d2, t1, t2, c1, c2;
	d1 = pDes->nValue[0];
	d2 = pSrc->nValue[0];
	t1 = pDes->nValue[1];
	t2 = pSrc->nValue[1];
	c1 = pDes->nValue[2];
	c2 = pSrc->nValue[2];
	if (c1 == 0 || d1 == 0)
	{
		memcpy(pDes, pSrc, sizeof(KMagicAttrib));
		return;
	}
	if (c2 == 0 || d2 == 0)
	{
		return;
	}
	pDes->nValue[0] = ((c1 + c2) * d1 / c1 + (c1 + c2) * d2 / c2) / 2;
	pDes->nValue[1] = (t1 * d1 * c2 + t2 *d2 * c1) /(d1 * c2 + d2 * c1);
	pDes->nValue[2] = (c1 + c2) / 2;
}


void KNpcAttribModify::DynamicMagicShieldV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaShield += pMagic->nValue[0];
}

void KNpcAttribModify::StaticMagicShieldP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentStaticMagicShieldP += pMagic->nValue[0];
}

void KNpcAttribModify::ExpEnhanceP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pNpc->IsPlayer())
	{
		pNpc->m_CurrentExpEnhance += pMagic->nValue[0];
	}
}

void KNpcAttribModify::LifeReplenishP(KNpc* pNpc, void* pData)
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentLifeReplenishPercent += pMagic->nValue[0];
}

void KNpcAttribModify::IgnoreSkillP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentIgnoreSkillPercent += pMagic->nValue[0];
}

void KNpcAttribModify::PoisonDamageReturnP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentPoisonDamageReturnPercent += pMagic->nValue[0];
}

void KNpcAttribModify::ReturnSkillP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentReturnSkillPercent += pMagic->nValue[0];
}

void KNpcAttribModify::Poison2DecManaP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	
	if (pMagic->nValue[1] <= 0)
		return;
	pNpc->m_LoseMana.nTime = pMagic->nValue[1];
	pNpc->m_LoseMana.nValue[0] = pNpc->m_CurrentMana * pMagic->nValue[0] / MAX_PERCENT / (pNpc->m_LoseMana.nTime / GAME_FPS);
}

void KNpcAttribModify::AutoDeathSkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_DeathSkill[i].nSkillId <= 0)
			{
				pNpc->m_DeathSkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_DeathSkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_DeathSkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_DeathSkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_DeathSkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::AutoAttackSkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_AttackSkill[i].nSkillId <= 0)
			{
				pNpc->m_AttackSkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_AttackSkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_AttackSkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_AttackSkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_AttackSkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::Hide( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_HideState.nTime = pMagic->nValue[1];
}

void KNpcAttribModify::IgnoreNegativeStateP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentIgnoreNegativeStateP += pMagic->nValue[0];
}

void KNpcAttribModify::AutoReplySkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_ReplySkill[i].nSkillId <= 0)
			{
				pNpc->m_ReplySkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_ReplySkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_ReplySkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_ReplySkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_ReplySkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::AutoRescueSkill( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if (pMagic->nValue[0])
	{
		for (int i = 0; i < MAX_AUTOSKILL; i ++)
		{
			if (pNpc->m_RescueSkill[i].nSkillId <= 0)
			{
				pNpc->m_RescueSkill[i].nSkillId = pMagic->nValue[0] / 256;
				pNpc->m_RescueSkill[i].nSkillLevel = pMagic->nValue[0] % 256;
				pNpc->m_RescueSkill[i].dwNextCastTime = SubWorld[pNpc->m_SubWorldIndex].m_dwCurrentTime;
				pNpc->m_RescueSkill[i].nWaitCastTime = pMagic->nValue[2] / 256;
				pNpc->m_RescueSkill[i].nRate = pMagic->nValue[2] % 256;
				break;
			}
		}
	}
}

void KNpcAttribModify::ReturnResP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentReturnResPercent += pMagic->nValue[0];
}

void KNpcAttribModify::SkillEnhanceP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentSkillEnhancePercent += pMagic->nValue[0];
}

void KNpcAttribModify::FiveElementsEnhanceV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFiveElementsEnhance += pMagic->nValue[0];
}

void KNpcAttribModify::FiveElementsResistV( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentFiveElementsResist += pMagic->nValue[0];
}

void KNpcAttribModify::FrozenAction( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_FrozenAction.nTime = pMagic->nValue[1];
	else
		pNpc->m_FrozenAction.nTime = 0;
}

void KNpcAttribModify::WalkRunShadow( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_WalkRun.nTime += pMagic->nValue[1];
	else
		pNpc->m_WalkRun.nTime = 0;
}

void KNpcAttribModify::RandMove( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	if(pMagic->nValue[1] > 0)
		pNpc->m_RandMove.nTime += pMagic->nValue[1];
	else
		pNpc->m_RandMove.nTime = 0;
}

void KNpcAttribModify::ManaToSkillEnhanceP( KNpc* pNpc, void* pData )
{
	KMagicAttrib* pMagic = (KMagicAttrib *)pData;
	pNpc->m_CurrentManaToSkillEnhanceP += pMagic->nValue[0];
}