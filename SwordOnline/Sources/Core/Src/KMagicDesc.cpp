#include "KCore.h"

#include "KEngine.h"
#include "KMagicAttrib.h"
#include "KMagicDesc.h"
#include "GameDataDef.h"
#include "KFaction.h"

#define		MAGICDESC_FILE		"\\settings\\MagicDesc.Ini"
extern const char * g_MagicID2String(int nAttrib);
const char MAGIC_ATTRIB_STRING[][100] = 
{
	"skill_begin",						//0  
	"skill_cost_v",						//1  
	"skill_costtype_v",					//2  
	"skill_mintimepercast_v",			//3  
	"skill_misslenum_v",				//4  
	"skill_misslesform_v",				//5  
	"skill_param1_v",					//6  
	"skill_param2_v",					//7  
	"skill_attackradius",				//8  
	"skill_eventskilllevel",			//9  
	"skill_showevent",					//10 
	"skill_appendskill",				//11 
	"skill_reserve3",					//12 
	"skill_end",						//13 
	"missle_begin",						//14 
	"missle_movekind_v",				//15 
	"missle_speed_v",					//16 
	"missle_lifetime_v",				//17 
	"missle_height_v",					//18 
	"missle_damagerange_v",				//19 
	"missle_radius_v",					//20 
	"missle_missrate",					//21 
	"missle_hitcount",					//22 
	"missle_reserve3",					//23 
	"missle_reserve4",					//24 
	"missle_reserve5",					//25 
	"missle_end",						//26 
	"item_begin",						//27 
	"weapondamagemin_v",				//28 
	"weapondamagemax_v",				//29 
	"armordefense_v",					//30 
	"durability_v",						//31 
	"requirestr",						//32 
	"requiredex",						//33 
	"requirevit",						//34 
	"requireeng",						//35 
	"requirelevel",						//36 
	"requireseries",					//37 
	"requiresex",						//38 
	"requiremenpai",					//39 
	"weapondamageenhance_p",			//40 
	"armordefenseenhance_p",			//41 
	"requirementreduce_p",				//42 
	"indestructible_b",					//43 
	"married",							//44 
	"dec_percasttime",					//45 
	"require_translife",				//46 
	"require_fortune_value",			//47 
	"item_reserve2",					//48 
	"item_reserve3",					//49 
	"item_purple",						//50 
	"add_damage_p",						//51 
	"not_add_pkvalue_p",				//52 
	"add_boss_damage",					//53 
	"item_end",							//54 
	"damage_begin",						//55 
	"attackrating_v",					//56 
	"attackrating_p",					//57 
	"ignoredefense_p",					//58 
	"magicdamage_v",					//59 
	"seriesdamage_p",					//60 
	"physicsdamage_v",					//61 
	"colddamage_v",						//62 
	"firedamage_v",						//63 
	"lightingdamage_v",					//64 
	"poisondamage_v",					//65 
	"physicsenhance_p",					//66 
	"steallife_p",						//67 
	"stealmana_p",						//68 
	"stealstamina_p",					//69 
	"knockback_p",						//70 
	"deadlystrike_p",					//71 
	"fatallystrike_p",					//72 
	"stun_p",							//73 
	"trong",							//74 
	"randmove",							//75 
	"damage_reserve2",					//76 
	"damage_reserve3",					//77 
	"damage_reserve4",					//78 
	"damage_reserve5",					//79 
	"damage_reserve6",					//80 
	"damage_reserve7",					//81 
	"damage_reserve8",					//82 
	"damage_end",						//83 
	"normal_begin",						//84 
	"lifemax_v",						//85 
	"lifemax_p",						//86 
	"life_v",							//87 
	"lifereplenish_v",					//88 
	"manamax_v",						//89 
	"manamax_p",						//90 
	"mana_v",							//91 
	"manareplenish_v",					//92 
	"staminamax_v",						//93 
	"staminamax_p",						//94 
	"stamina_v",						//95 
	"staminareplenish_v",				//96 
	"strength_v",						//97 
	"dexterity_v",						//98 
	"vitality_v",						//99 
	"energy_v",							//100
	"poisonres_p",						//101
	"fireres_p",						//102
	"lightingres_p",					//103
	"physicsres_p",						//104
	"coldres_p",						//105
	"freezetimereduce_p",				//106
	"burntimereduce_p",					//107
	"poisontimereduce_p",				//108
	"poisondamagereduce_v",				//109
	"stuntimereduce_p",					//110
	"fastwalkrun_p",					//111
	"visionradius_p",					//112
	"fasthitrecover_v",					//113
	"allres_p",							//114
	"attackspeed_v",					//115
	"castspeed_v",						//116
	"meleedamagereturn_v",				//117
	"meleedamagereturn_p",				//118
	"rangedamagereturn_v",				//119
	"rangedamagereturn_p",				//120
	"addphysicsdamage_v",				//121
	"addfiredamage_v",					//122
	"addcolddamage_v",					//123
	"addlightingdamage_v",				//124
	"addpoisondamage_v",				//125
	"addphysicsdamage_p",				//126
	"slowmissle_b",						//127
	"changecamp_b",						//128
	"physicsarmor_v",					//129
	"coldarmor_v",						//130
	"firearmor_v",						//131
	"poisonarmor_v",					//132
	"lightingarmor_v",					//133
	"damage2addmana_p",					//134
	"lucky_v",							//135
	"steallifeenhance_p",				//136
	"stealmanaenhance_p",				//137
	"stealstaminaenhance_p",			//138
	"allskill_v",						//139
	"addphysicsmagic_v",				//140
	"addcoldmagic_v",					//141
	"addfiremagic_v",					//142
	"addlightingmagic_v",				//143
	"addpoisonmagic_v",					//144
	"knockbackenhance_p",				//145
	"deadlystrikeenhance_p",			//146
	"stunenhance_p",					//147
	"badstatustimereduce_v",			//148
	"manashield_p",						//149
	"adddefense_v",						//150
	"adddefense_p",						//151
	"fatallystrikeenhance_p",			//152
	"lifepotion_v",						//153
	"manapotion_v",						//154
	"physicsresmax_p",					//155
	"coldresmax_p",						//156
	"fireresmax_p",						//157
	"lightingresmax_p",					//158
	"poisonresmax_p",					//159
	"allresmax_p",						//160
	"coldenhance_p",					//161
	"fireenhance_p",					//162
	"lightingenhance_p",				//163
	"poisonenhance_p",					//164
	"magicenhance_p",					//165
	"attackratingenhance_v",			//166
	"attackratingenhance_p",			//167
	"metalskill_v",						//168
	"woodskill_v",						//169
	"waterskill_v",						//170
	"fireskill_v",						//171
	"earthskill_v",						//172
	"fatallystrikeres_p",				//173
	"dynamicmagicshield_v",				//174
	"staticmagicshield_p",				//175
	"expenhance_p",						//176
	"ignoreskill_p",					//177
	"poisondamagereturn_v",				//178
	"poisondamagereturn_p",				//179
	"returnskill_p",					//180
	"skill_mintimepercastonhorse_v",	//181
	"poison2decmana_p",					//182
	"hide",								//183
	"autodeathskill",					//184
	"clearnegativestate",				//185
	"returnres_p",						//186
	"autoattackskill",					//187
	"dec_percasttimehorse",				//188
	"normal_reserve4",					//189
	"lifereplenish_p",					//190
	"frozen_action",					//191
	"normal_reserve5",					//192
	"sorbdamage_p",						//193
	"autorescueskill",					//194
	"autoreplyskill",					//195
	"anti_hitrecover",					//196
	"magicdamage_p",					//197
	"anti_poisonres_p",					//198
	"anti_fireres_p",					//199
	"anti_lightingres_p",				//200
	"anti_physicsres_p",				//201
	"anti_coldres_p",					//202
	"fastwalkrun_yan_p",				//203
	"anti_poisontimereduce_p",			//204
	"do_hurt_p",						//205
	"me2wooddamage_p",					//206
	"skill_desc",						//207
	"skill_collideevent",				//208
	"skill_vanishedevent",				//209
	"skill_startevent",					//210
	"skill_flyevent",					//211
	"skill_dohurt",						//212
	"skill_bymissle",					//213
	"candetonate1",						//214
	"candetonate2",						//215
	"candetonate3",						//216
	"anti_fireres_yan_p",				//217
	"anti_maxres_p",					//218
	"anti_do_stun_p",					//219
	"anti_stuntimereduce_p",			//220
	"anti_poisonres_yan_p",				//221
	"anti_coldres_yan_p",				//222
	"anti_do_hurt_p",					//223
	"anti_allres_yan_p",				//224
	"pk_punish_weaken",					//225
	"block_rate",						//226
	"enhancehit_rate",					//227
	"poisonres_yan_p",					//228
	"lightingres_yan_p",				//229
	"fireres_yan_p",					//230
	"physicsres_yan_p",					//231
	"coldres_yan_p",					//232
	"lifemax_yan_v",					//233
	"lifemax_yan_p",					//234
	"manamax_yan_v",					//235
	"manamax_yan_p",					//236
	"sorbdamage_yan_p",					//237
	"pk_punish_enhance",				//238
	"attackspeed_yan_v",				//239
	"castspeed_yan_v",					//240
	"allres_yan_p",						//241
	"me2earthdamage_p",					//242
	"skill_enhance",					//243
	"anti_physicsres_yan_p",			//244
	"fasthitrecover_yan_v",				//245
	"five_elements_enhance_v",			//246
	"five_elements_resist_v",			//247
	"anti_lightingres_yan_p",			//248
	"metal2medamage_p",					//249
	"reduceskillcd3",					//250
	"clearallcd",						//251
	"anti_enhancehit_rate",				//252
	"me2metaldamage_p",					//253
	"normal_reserve6",					//254
	"normal_reserve7",					//255
	"wood2medamage_p",					//256
	"me2waterdamage_p",					//257
	"water2medamage_p",					//258
	"me2firedamage_p",					//259
	"fire2medamage_p",					//260
	"do_stun_p",						//261
	"earth2medamage_p",					//262
	"normal_reserve8",					//263
	"enhancehiteffect_rate",			//264
	"meleedamagereturnmana_p",			//265
	"rangedamagereturnmana_p",			//266
	"reduceskillcd1",					//267
	"reduceskillcd2",					//268
	"anti_sorbdamage_yan_p",			//269
	"anti_block_rate",					//270
	"addblockrate",						//271
	"walkrunshadow",					//272
	"returnskill2enemy",				//273
	"manatoskill_enhance",				//274
	"melee_returnres_p",				//275
	"range_returnres_p",				//276
	"addskilldamage1", 					//277
	"addskilldamage2", 					//278
	"addskilldamage3", 					//279
	"addskilldamage4", 					//280
	"addskilldamage5", 					//281
	"addskilldamage6",					//282
	"addskilldamage7", 					//283
	"addskilldamage8", 					//284
	"addskilldamage9",					//285
	"missle_exp_begin",					//286
	"missle_range",						//287
	"missle_dmginterval",				//288
	"missle_zspeed",					//289
	"missle_ablility",					//290
	"missle_param",						//291
	"missle_wait",						//292
	"missle_fly",						//293
	"missle_collide",					//294
	"missle_vanish",					//295
	"missle_exp_rev1",					//296
	"missle_exp_rev2",					//297
	"missle_exp_rev3",					//298
	"missle_exp_rev4",					//299
	"missle_exp_rev5",					//300
	"missle_exp_end",					//301
	"ignorenegativestate_p",			//302
	"normal_end",						//303
};



KMagicDesc	g_MagicDesc;
KMagicDesc::KMagicDesc()
{
	m_szDesc[0] = 0;
}

KMagicDesc::~KMagicDesc()
{
}

BOOL KMagicDesc::Init()
{
//	g_SetFilePath("\\");
	return (m_IniFile.Load(MAGICDESC_FILE));
}

const char* KMagicDesc::GetDesc(void *pData)
{
	
	char	szTempDesc[256];
	char*	pTempDesc = szTempDesc;
	int		i = 0;

	ZeroMemory(m_szDesc, 256);
	
	if (!pData)
		return NULL;

	KMagicAttrib* pAttrib = (KMagicAttrib *)pData;

	const char	*pszKeyName = g_MagicID2String(pAttrib->nAttribType);
	m_IniFile.GetString("Descript", pszKeyName, "", szTempDesc, 256);
	while(*pTempDesc)
	{
		if (*pTempDesc == '#')
		{
			int	nDescAddType = 0;
			switch(*(pTempDesc + 3))
			{
			case '+':
				nDescAddType = 1;
				break;
			case '~':
				nDescAddType = 2;
				break;
			default:
				nDescAddType = 0;
				break;
			}
			int nValue = 0;
			switch(*(pTempDesc + 2))
			{
			case 'A':
				nValue = (int)(pAttrib->nValue[0] / 256);
				break;			
			case '1':
				nValue = pAttrib->nValue[0];
				break;
			case '2':
				nValue = pAttrib->nValue[1];
				break;
			case '3':
				nValue = pAttrib->nValue[2];
				break;
			case '6':
				nValue = (int)(pAttrib->nValue[2] / 256);
				break;
			case '7':
				nValue = (int)(pAttrib->nValue[0] % 256);
				break;
			case '9':
				nValue = (int)(pAttrib->nValue[2] % 256);
				break;
			default:
				nValue = pAttrib->nValue[0];
				break;
			}
			switch(*(pTempDesc+1))
			{
			case 'm':		// ÃÅÅÉ
				strcat(m_szDesc, g_Faction.m_sAttribute[nValue].m_szShowName);
				i += strlen(g_Faction.m_sAttribute[nValue].m_szShowName);
				break;
			case 's':		// ÎåÐÐ
				switch(nValue)
				{
				case series_metal:
					strcat(m_szDesc, "HÖ Kim");
					break;
				case series_wood:
					strcat(m_szDesc, "HÖ Méc");
					break;
				case series_water:
					strcat(m_szDesc, "HÖ Thñy");
					break;
				case series_fire:
					strcat(m_szDesc, "HÖ Háa");
					break;
				case series_earth:
					strcat(m_szDesc, "HÖ Thæ ");
					break;
				default:
					strcat(m_szDesc, "V« HÖ ");
					break;
				}
				i += 4;
				break;
			case 'k':		// ÏûºÄÀàÐÍ
				switch(nValue)
				{
				case 0:
					strcat(m_szDesc, "Néi Lùc\n");
					break;
				case 1:
					strcat(m_szDesc, "Sinh Lùc\n");
					break;
				case 2:
					strcat(m_szDesc, "ThÓ Lùc\n");
					break;
				case 3:
					strcat(m_szDesc, "TiÒn\n");
					break;
				default:
					strcat(m_szDesc, "Néi Lùc\n");
					break;
				}
				i += 4;
				break;
			case 'd':		// Êý×Ö
				{
					switch(nDescAddType)
					{
					case 1:
						if (nValue > 0)
						{
							strcat(m_szDesc, "+");
							i += 1;
						}
						else
						{
							nValue = -nValue;
							strcat(m_szDesc, "-");
							i += 1;
						}
						break;
					case 2:
						if (nValue > 0)
						{
							strcat(m_szDesc, "-");
							i += 1;
						}
						break;
					default:
						break;
					}
					char	szMsg[16];
					sprintf(szMsg, "%d", nValue);
					strcat(m_szDesc, szMsg);
					i += strlen(szMsg);
				}
				break;
			case 'l':
				{
					char szMsg[80];
					KSkill * pSkill = NULL;
					if (nValue > 0)
					{
					pSkill = (KSkill *)g_SkillManager.GetSkill(nValue,1);
					sprintf(szMsg,"[ %s ]",pSkill->GetSkillName());
					}
					else
					{
						sprintf(szMsg, "Vâ c«ng vèn cã");
					}
					strcat(m_szDesc,szMsg);
					i += strlen(szMsg);
				}
				break;
			case 'f':		// Êý×Ö
					char szMsg[80];
					sprintf(szMsg, "%d", (int)(nValue/18));
					strcat(m_szDesc,szMsg);
					i += strlen(szMsg);
				break;
			case 'x':		// ÐÔ±ð
				if (nValue)
				{
					strcat(m_szDesc, "N÷");
					i += 2;
				}
				else
				{
					strcat(m_szDesc, "Nam");
					i += 3;
				}
				break;
			case 'w':
				{
					char szMsg[32];
					switch(nValue)
					{
					case 9:
						strcpy(szMsg, "tµn ¶nh kÐo dµi");
						break;
					default:
						strcpy(szMsg, "tµn ¶nh");
						break;
					}
					strcat(m_szDesc,szMsg);
					i += strlen(szMsg);
				}
				break;
			default:
				break;
			}
			pTempDesc += 4;
		}
		else
		{
			m_szDesc[i] = *pTempDesc;
			pTempDesc++;
			i++;
		}
	}
	return m_szDesc;
}

const char * g_MagicID2String(int nAttrib)
{
	if ((nAttrib < 0) || nAttrib >= magic_normal_end) return MAGIC_ATTRIB_STRING[magic_normal_end];
	return 	MAGIC_ATTRIB_STRING[nAttrib];
}

int	g_String2MagicID(char * szMagicAttribName)
{
	if ((!szMagicAttribName) || (!szMagicAttribName[0])) return -1;

	//nValue2 µ±ÖµÎª-1Ê±ÎªÓÀ¾ÃÐÔ×´Ì¬£¬0Îª·Ç×´Ì¬£¬ÆäËüÖµÎªÓÐÊ±Ð§ÐÔ×´Ì¬Ä§·¨Ð§¹û
	//ÐèÒª½«×´Ì¬Êý¾ÝÓë·Ç×´Ì¬Êý¾Ý·ÖÀë³öÀ´£¬·ÅÈëÏàÓ¦µÄÊý×éÄÚ£¬²¢¼ÇÂ¼×ÜÊýÁ¿
	
	for (int i  = 0 ; i <= magic_normal_end; i ++)
	{
		if (!strcmp(szMagicAttribName, g_MagicID2String(i)))
			return i;
	}
	return -1;
}

int	KMagicDesc::String2MagicID(char * szMagicAttribName)
{
	if ((!szMagicAttribName) || (!szMagicAttribName[0])) return -1;

	//nValue2 µ±ÖµÎª-1Ê±ÎªÓÀ¾ÃÐÔ×´Ì¬£¬0Îª·Ç×´Ì¬£¬ÆäËüÖµÎªÓÐÊ±Ð§ÐÔ×´Ì¬Ä§·¨Ð§¹û
	//ÐèÒª½«×´Ì¬Êý¾ÝÓë·Ç×´Ì¬Êý¾Ý·ÖÀë³öÀ´£¬·ÅÈëÏàÓ¦µÄÊý×éÄÚ£¬²¢¼ÇÂ¼×ÜÊýÁ¿
	
	for (int i  = 0 ; i <= magic_normal_end; i ++)
	{
		if (!strcmp(szMagicAttribName, g_MagicID2String(i)))
			return i;
	}
	return -1;
}

