
#ifndef COREUSENAMEDEF_H
#define COREUSENAMEDEF_H

#define		MAX_PLAYER_IN_ACCOUNT			3

#define		SETTING_PATH					"\\settings"


#define		NPCNAME_KEY						"<npc>"
#define		PLAYERNAME_KEY					"<player>"
#define		SEXNAME_KEY						"<sex>"

#define 	STRANGER_UNITNAME 				"[Ng­êi l¹]"
#define 	FRIEND_INDEX					0
#define 	FRIEND_UNITNAME 				"H¶o h÷u"
#define 	ENEMY_INDEX						1
#define 	ENEMY_UNITNAME	 				"Cơu nh©n"
#define 	ADDENEMY_UNITNAME	 			"Cơu nh©n\n"
#define		BLACKLIST_UNITNAME				"Sæ ®en"
#define		BLACKLIST_INDEX					2
//---------------------------- npc res Ïà¹Ø ------------------------------
#define		RES_INI_FILE_PATH				"\\settings\\npcres"
#define		RES_NPC_STYLE_PATH				"\\spr\\npcres\\style"
#define		RES_SOUND_FILE_PATH				"sound"

#define		NPC_RES_KIND_FILE_NAME			"\\settings\\npcres\\ÈËÎïÀàĐÍ.txt"
#define		NPC_NORMAL_RES_FILE				"\\settings\\npcres\\ÆƠÍ¨npc×ÊÔ´.txt"
#define		NPC_NORMAL_SPRINFO_FILE			"\\settings\\npcres\\ÆƠÍ¨npc×ÊÔ´ĐÅÏ¢.txt"
#define		STATE_MAGIC_TABLE_NAME			"\\settings\\npcres\\×´̀¬Í¼ĐÎ¶ÔƠƠ±í.txt"
#define		PLAYER_RES_SHADOW_FILE			"\\settings\\npcres\\Ö÷½Ç¶¯×÷̉ơÓ°¶ÔÓ¦±í.txt"
#define		NPC_RES_SHADOW_FILE				"ÆƠÍ¨npc¶¯×÷̉ơÓ°¶ÔÓ¦±í.txt"
#define		PLAYER_SOUND_FILE				"\\settings\\npcres\\Ö÷½Ç¶¯×÷Éù̉ô±í.txt"
#define		NPC_SOUND_FILE					"\\settings\\npcres\\npc¶¯×÷Éù̉ô±í.txt"

#define		NPC_ACTION_NAME					"npc¶¯×÷±í.txt"
#define		ACTION_FILE_NAME				"¶¯×÷±àºÅ±í.txt"

#define		PLAYER_MENU_STATE_RES_FILE		"\\settings\\npcres\\½çĂæ×´̀¬ÓëÍ¼ĐÎ¶ÔƠƠ±í.txt"
#define		PLAYER_INSTANT_SPECIAL_FILE		"\\settings\\npcres\\Ë²¼ä̀ØĐ§.txt"

#define		SPR_INFO_NAME					"ĐÅÏ¢"
#define		SPR_EFFECT_NAME					"_effect"
#define		KIND_NAME_SECT					"CharacterType"
#define		KIND_NAME_SPECIAL				"SpecialNpc"
#define		KIND_NAME_NORMAL				"NormalNpc"
#define		KIND_FILE_SECT1					"PartFileName"
#define		KIND_FILE_SECT2					"WeaponActionTab1"
#define		KIND_FILE_SECT3					"WeaponActionTab2"
#define		KIND_FILE_SECT4					"ActionRenderOrderTab"
#define		KIND_FILE_SECT5					"ResFilePath"

//--------------------------- player ĂÅÅÉÏà¹Ø ------------------------------
// ×¢£ºÏÂĂæƠâ¸öÉè¶¨ÎÄ¼₫±ØĐë°´ ½đ ½đ Ä¾ Ä¾ Ë® Ë® »đ »đ ÍÁ ÍÁ µÄË³Đ̣ÅÅÁĐ
#define		FACTION_FILE					"\\settings\\faction\\ĂÅÅÉÉè¶¨.ini"

//--------------------------- player PKÏà¹Ø ------------------------------
#define		defPK_PUNISH_FILE				"\\settings\\npc\\player\\PKPunish.txt"

//--------------------------- player °ï»áÏà¹Ø ------------------------------
#define		defTONG_PARAM_FILE				"\\settings\\tong\\Tong_Setting.ini"
#define		defTONG_LEVEL_FILE				"\\settings\\tong\\Tong_Level.txt"
#define		defTONG_DATA_FILE				"\\settings\\tong\\Tong_Data.ini"

//---------------------------- player ÊưÖµÏà¹Ø ------------------------------
#define		PLAYER_LEVEL_EXP_FILE			"\\settings\\npc\\player\\level_exp.txt"
#define		PLAYER_LEVEL_ADD_FILE			"\\settings\\npc\\player\\level_add.txt"
#define		PLAYER_LEVEL_LEAD_EXP_FILE		"\\settings\\npc\\player\\level_lead_exp.txt"
#define		PLAYER_MAGIC_LEVEL_EXP_FILE		"\\settings\\npc\\player\\magic_level_exp.txt"
#define		BASE_ATTRIBUTE_FILE_NAME		"\\settings\\npc\\player\\NewPlayerBaseAttribute.ini"
#define		PLAYER_PK_SET_FILE				"\\settings\\npc\\PKSet.ini"
#define		PLAYER_BASE_VALUE				"\\settings\\npc\\player\\BaseValue.ini"
#define		PLAYER_STAMINA_FILE_NAME		"\\settings\\npc\\player\\stamina.ini"

// 00ÄĐ½đ 01Å®½đ 02ÄĐÄ¾ 03Å®Ä¾ 04ÄĐË® 05Å®Ë® 06ÄĐ»đ 07Å®»đ 08ÄĐÍÁ 09Å®ÍÁ
#define		NEW_PLAYER_INI_FILE_NAME		"\\settings\\npc\\player\\NewPlayerIni%02d.ini"

//---------------------------- player ½»̉×Ïà¹Ø ------------------------------
#define		BUYSELL_FILE					"\\settings\\buysell.txt"
#define		GOODS_FILE						"\\settings\\goods.txt"

//----------------------------- ÎïÆ·Ïà¹Ø ------------------------------
#define		CHANGERES_MELEE_FILE			"\\settings\\item\\MeleeRes.txt"
#define		CHANGERES_RANGE_FILE			"\\settings\\item\\RangeRes.txt"
#define		CHANGERES_ARMOR_FILE			"\\settings\\item\\ArmorRes.txt"
#define		CHANGERES_HELM_FILE				"\\settings\\item\\HelmRes.txt"
#define		CHANGERES_HORSE_FILE			"\\settings\\item\\HorseRes.txt"
#define		CHANGERES_GOLD_FILE				"\\settings\\item\\GoldEquipRes.txt"
#define		CHANGERES_MASK_FILE				"\\settings\\item\\MaskRes.txt"
#define		CHANGERES_HOODS_FILE			"\\settings\\item\\HoodsRes.txt"
#define		CHANGERES_CLOAK_FILE			"\\settings\\item\\CloakRes.txt"
#define		ITEM_ABRADE_FILE				"\\settings\\item\\AbradeRate.ini"

//--------------- npc skill missles Éè¶¨ÎÄ¼₫£¬ÓĂÓÚÉú³ÉÄ£°å -------------
#define		SKILL_SETTING_FILE				"\\settings\\Skills.txt"
#define		MISSLES_SETTING_FILE			"\\settings\\Missles.txt"
#define		NPC_SETTING_FILE				"\\settings\\NpcS.txt"
#define		NPC_NAME_FILE					"\\settings\\NpcName.txt"

//---------------------------- object Ïà¹Ø ------------------------------
#define		OBJ_DATA_FILE_NAME		"\\settings\\obj\\ObjData.txt"
#define		MONEY_OBJ_FILE_NAME		"\\settings\\obj\\MoneyObj.txt"
#define		OBJ_NAME_COLOR_FILE		"\\settings\\obj\\ObjNameColor.ini"

//---------------------------- Đ¡µØÍ¼Ïà¹Ø -------------------------------
#define		defLITTLE_MAP_SET_FILE	"\\Ui\\Default\\Đ¡µØÍ¼ÑƠÉ«.ini"

//----------------------------- Éù̉ôÏà¹Ø --------------------------------
#define		defINSTANT_SOUND_FILE	"\\settings\\SoundList.txt"
#define		defMUSIC_SET_FILE		"\\settings\\music\\MusicSet.txt"
#define		defMUSIC_FIGHT_SET_FILE	"\\settings\\music\\MusicFightSet.ini"

//------------------------------------------------------------------------
#define NPC_TEMPLATE_BINFILEPATH		"\\settings"
#define NPC_TEMPLATE_BINFILE			"NpcTemplate.Bin"
#define WEAPON_PHYSICSSKILLFILE			"\\settings\\ClientWeaponSkill.txt"				
#define WEAPON_PARTICULARTYPE			"ParticularType"
#define	WEAPON_DETAILTYPE				"DetailType"
#define	WEAPON_SKILLID					"PhysicsSkillID"
//-----------------------------------------------------------------------
#define	WORLD_WAYPOINT_TABFILE			"\\settings\\WayPoint.txt"
#define WORLD_STATION_TABFILE			"\\settings\\Station.txt"
#define WORLD_STATIONPRICE_TABFILE		"\\settings\\StationPrice.txt"
#define WORLD_WAYPOINTPRICE_TABFILE		"\\settings\\WayPointPrice.txt"
#define WORLD_DOCK_TABFILE				"\\settings\\Wharf.txt"
#define WORLD_DOCKPRICE_TABFILE			"\\settings\\WharfPrice.txt"
#define STRINGRESOURSE_TABFILE			"\\settings\\StringResource.txt"
#define PLAYER_RANK_SETTING_TABFILE		"\\settings\\RankSetting.txt"	
#define EVENTITEM_TABFILE				"\\settings\\item\\EventItem.txt"
#define QUESTITEM_TABFILE				"\\settings\\item\\QuestKey.txt"
#define TOWNPORTAL_TABFILE				"\\settings\\item\\TownPortal.txt"
#define MAGICSCRIPT_TABFILE				"\\settings\\item\\MagicScript.txt"

//---------------------------------ÈÎÎñ--------------------------
#define		MAGICATTRIB_LEVEL_FILE		"\\settings\\item\\magicattriblevel.txt"
#define		MAGICATTRIB_LVINDEX_FILE	"\\settings\\item\\magicattriblevel_index.txt"

#define		MAPLIST_SETTING_FILE			"\\settings\\MapList.ini"

#define		PLATINA_EQUIP_FILE			"\\settings\\item\\platinaequip.txt"
#define		GOLD_EQUIP_FILE				"\\settings\\item\\goldequip.txt"
#define		GOLD_EQUIP_MAGIC_FILE		"\\settings\\item\\magicattrib_ge.txt"

#define		GM_SCRIPT					"\\script\\gmscript.lua"
#define		SERVERTIMER_SCRIPT			"\\script\\servertimer.lua"
#define		SERVERSTARTUP_SCRIPT		"\\script\\serverscript.lua"

#define		LOGIN_SCRIPT				"\\script\\player\\playerlogin.lua"
#define		LEVELUP_SCRIPT				"\\script\\player\\playerlevelup.lua"
#define		LOGOUT_SCRIPT				"\\script\\player\\playerlogout.lua"
#define		SCRIPT_PROTOCOL_FILE		"\\script\\global\\script_protocol.lua"

#define		MINIMAP_SETTING_FILE_INI	"\\Settings\\MapTraffic.ini"
#define 	GAME_DATA_FILE_INI			"\\settings\\GameData.ini"
#define 	GAME_SETTING_FILE_INI		"\\settings\\GameSetting.ini"

#define		UNKNOWNITEM_SPR				"\\spr\\item\\unknownitem.spr"
#define		UNKNOWNITEM_SPR36			"\\spr\\item\\unknownitem36.spr"
#define		RESIZEITEM_SPR				"\\spr\\item\\twzhuanyun\\zhuanyunbao_big.spr"

#define		NORMAL_UNCLEAR_WORD			"Ch­a râ"
#define		NORMAL_FUNCTION_NAME		"main"
//---------------------------- ÏûÏ¢Ïà¹Ø ------------------------------
// ̀Ø±đ×¢̉â£º̉ÔÏÂ×Ö·û´®³¤¶È²»ÄÜ³¬¹ư32×Ö½Ú£¬°üÀ¨ %d %s µÈ½ÓÊƠ¾ß̀åÄÚÈỬÔºóµÄ³¤¶È
#define		MSG_GET_EXP						"B¹n nhËn ®­îc <color=green>%d <color>®iÓm kinh nghiÖm"
#define		MSG_DEC_EXP						"B¹n tæn thÊt <color=green>%d <color>®iÓm kinh nghiÖm"
#define		MSG_LEVEL_UP					"§¼ng cÊp ®¹t %d cÊp"
#define		MSG_LEADER_LEVEL_UP				"§¼ng cÊp thèng so¸i cña b¹n ®¹t cÊp %d"
#define		MSG_GET_ATTRIBUTE_POINT			"B¹n că %d ®iÓm tỉm n¨ng"
#define		MSG_GET_SKILL_POINT				"B¹n că %d ®iÓm kü n¨ng"
#define		MSG_GET_ATTRIBUTE_SKILL_POINT	"B¹n că %d ®iÓm tỉm n¨ng %d ®iÓm kü n¨ng"

#define		MSG_TEAM_SEND_INVITE			"B¹n göi ®Ơn %s lêi mêi gia nhËp tæ ®éi"
#define		MSG_TEAM_GET_INVITE				"§éi tr­ëng %s göi lêi mêi gia nhËp tæ ®éi ®Ơn b¹n"
#define		MSG_TEAM_REFUSE_INVITE			"Ng­êi ch¬i %s tơ chèi lêi gia nhËp tæ ®éi cña nhăm b¹n"
#define		MSG_TEAM_APPLY_INVITE_FAIL		"Nhăm mµ b¹n vơa ®èng ư gia nhËp kh«ng tån t¹i"
#define		MSG_TEAM_APPLY_INVITE_FAIL2		"Nhăm mµ b¹n vơa ®èng ư gia nhËp ®· ®ăng chøc n¨ng chiªu mé"
#define		MSG_TEAM_CREATE					"B¹n s¸ng lËp nªn mét nhăm míi"
#define		MSG_TEAM_CREATE_FAIL			"S¸ng lËp nhăm thÊt b¹i."
#define		MSG_TEAM_CREATE_FAIL1			"PK m« h×nh bang chiƠn kh«ng thÓ s¸ng lËp nhăm míi!"
#define		MSG_TEAM_CANNOT_CREATE			"B¹n hiÖn giê kh«ng thÓ lËp nhăm!"
#define		MSG_TEAM_TARGET_CANNOT_ADD_TEAM	"§èi ph­¬ng hiÖn giê kh«ng thÓ lËp nhăm!"
#define		MSG_TEAM_IS_FULL				"Nhăm mµ b¹n xin phĐp gia nhËp ®· ®ñ sè l­îng"
#define		MSG_TEAM_OPEN					"Nhăm cña b¹n hiÖn t¹i că thÓ tiƠp nhËn thµnh viªn míi"
#define		MSG_TEAM_CLOSE					"Nhăm cña b¹n hiÖn t¹i kh«ng thÓ tiƠp nhËn thµnh viªn míi"
#define		MSG_TEAM_MODEPICK_SELF			"Tù nhÆt"
#define		MSG_TEAM_MODEPICK_FREE			"Tù do nhÆt"
#define		MSG_TEAM_MODEPICK_CAPTAIN		"§éi tr­ëng nhÆt"
#define		MSG_TEAM_MODEPICK_ALTERNATE		"Lu©n phiªn nhÆt"
#define		MSG_TEAM_PKFOLLOW_OPEN			"B¹n më chƠ ®é PK theo nhăm"
#define		MSG_TEAM_PKFOLLOW_CLOSE			"B¹n ®ăng chƠ ®é PK theo nhăm"
#define		MSG_TEAM_ADD_MEMBER				"%s trë thµnh b»ng h÷u cña nhăm B¹n."
#define		MSG_TEAM_SELF_ADD				"B¹n ®· tham gia nhăm cña %s"
#define		MSG_TEAM_ADDMEMBER_NOTVALID		"B¹n kh«ng thÓ tham gia nhăm cña ng­êi ch¬i míi"
#define		MSG_TEAM_DISMISS_CAPTAIN		"B¹n ®· gi¶i t¸n nhăm cña m×nh"
#define		MSG_TEAM_DISMISS_MEMBER			"%s gi¶i t¸n nhăm!"
#define		MSG_TEAM_KICK_ONE				"%s b̃ khai trơ khái nhăm"
#define		MSG_TEAM_BE_KICKEN				"B¹n b̃ khai trơ khái nhăm"
#define		MSG_TEAM_APPLY_ADD				"%s xin phĐp gia nhËp nhăm!"
#define		MSG_TEAM_APPLY_ADD_SELF_MSG		"B¹n göi lêi xin phĐp gia nhËp tæ ®éi cña %s"
#define		MSG_TEAM_LEAVE					"%s rêi khái nhăm."
#define		MSG_TEAM_LEAVE_SELF_MSG			"B¹n ®· rêi khái nhăm cña %s"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL1	"§¶m nhËn nhăm tr­ëng thÊt b¹i!"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL2	"%s kh«ng ®ñ søc thèng so¸i!"
#define		MSG_TEAM_CHANGE_CAPTAIN_FAIL3	"Kh«ng thÓ giao chøc vô ®éi tr­ëng cho ng­êi míi ch¬i"
#define		MSG_TEAM_CHANGE_CAPTAIN			"%s ®­îc ®¶m nhËn lµ nhăm tr­ëng"
#define		MSG_TEAM_CHANGE_CAPTAIN_SELF	"B¹n ®­îc %s ®¶m nhËn lµ nhăm tr­ëng"
#define		MSG_TEAM_AUTO_CAPTAIN			"%s ®­îc hÖ thèng chØ ®̃nh lµ nhăm tr­ëng"
#define		MSG_TEAM_AUTO_CAPTAIN_SELF		"B¹n ®­îc hÖ thèng chØ ®̃nh lµ nhăm tr­ëng"


#define		MSG_CHATROOM_CREATE				"B¹n t¹o phßng míi %s"
#define		MSG_CHATROOM_CREATE_ERROR_SAMENAME	"Tªn nµy ®· tån t¹i"
#define		MSG_CHATROOM_CREATE_ERROR_SAMEHOST	"B¹n ®· t¹o mét phßng råi"
#define		MSG_CHATROOM_CREATE_ERROR_NAME	"Tªn kh«ng hîp lÖ"
#define		MSG_CHATROOM_CREATE_ERROR_PASS	"MËt m· phßng kh«ng hîp lÖ"
#define		MSG_CHATROOM_CREATE_ERROR_ROOMFULL	"Sè phßng t¸n gÉu cña m¸y chñ nµy ®· ®ñ, kh«ng thÓ t¹o thªm"
#define		MSG_CHATROOM_SELF_JOIN			"B¹n vµo phßng %s"
#define		MSG_CHATROOM_JOIN				"%s ®· vµo phßng"
#define		MSG_CHATROOM_JOIN_FAIL1			"B¹n nhËp sai mËt m· phßng %s"
#define		MSG_CHATROOM_JOIN_FAIL2			"B¹n b̃ liÖt vµo sæ ®en kh«ng thÓ ®¨ng nhËp vµo phßng %s"
#define		MSG_CHATROOM_JOIN_FAIL3			"Phßng %s ®· ®Çy"
#define		MSG_CHATROOM_SELF_LEAVE			"B¹n rêi khái phßng %s"
#define		MSG_CHATROOM_LEAVE				"%s rêi khái phßng"
#define		MSG_CHATROOM_SELF_DELETE		"B¹n ®· hñy phßng %s"
#define		MSG_CHATROOM_SELF_FORCELEAVE	"B¹n b̃ ®Èy khái phßng %s"
#define		MSG_CHATROOM_SELF_BEKICK		"B¹n b̃ khai trơ khái phßng %s"
#define		MSG_CHATROOM_BEKICK				"%s b̃ khai trơ khái phßng"
#define		MSG_CHATROOM_ADDBLACKLIST		"Thªm %s vµo sæ ®en"
#define		MSG_CHATROOM_ADDBLACKLIST_FAIL	"Thªm %s vµo sæ ®en thÊt b¹i"
#define		MSG_CHATROOM_REMOVEBLACKLIST	"L­îc bá %s khái sæ ®en"
#define		MSG_CHATROOM_SELD_CHANGEPW		"B¹n ®· ®æi mËt m· phßng %s"
#define		MSG_CHATROOM_OPENGAME			"L­ît ch¬i míi ®­îc më, lÇn nµy ®Æt tèi ®a %d ®iÓm"
#define		MSG_CHATROOM_REVERSE			"L­ît ch¬i cña phßng %s ®· kƠt thóc"

#define		MSG_CHAT_APPLY_ADD_FRIEND		"B¹n ®Ơn %s xin phĐp ®­îc kƠt giao."
#define		MSG_CHAT_FRIEND_HAD_IN			"%s ®· ®­îc nhËn lµm b¼ng h÷u."
#define		MSG_CHAT_GET_FRIEND_APPLY		"%s xin phĐp gia nhËp b»ng h÷u"
#define		MSG_CHAT_REFUSE_FRIEND			"%s tơ chèi yªu cÇu kƠt b¹n"
#define		MSG_CHAT_ADD_FRIEND_FAIL		"Thªm %s vµo danh s¸ch b»ng h÷u thÊt b¹i"
#define		MSG_CHAT_ADD_FRIEND_SUCCESS		"Thªm %s vµo danh s¸ch b»ng h÷u thµnh c«ng"
#define		MSG_CHAT_FRIEND_ONLINE			"H¶o h÷u %s lªn m¹ng råi!"
#define		MSG_CHAT_FRIEND_OFFLINE			"H¶o h÷u %s rêi m¹ng råi!"
#define		MSG_CHAT_ENEMY_ONLINE			"Cơu nh©n %s lªn m¹ng råi!"
#define		MSG_CHAT_ENEMY_OFFLINE			"Cơu nh©n %s rêi m¹ng råi!"
#define		MSG_CHAT_DELETE_FRIEND			"B¹n cïng %s hñy bá quan hÖ"
#define		MSG_CHAT_DELETED_FRIEND			"%s cïng B¹n hñy bá quan hÖ"

#define		MSG_SHOP_NO_ROOM				"Kho¶ng trèng trong tói kh«ng ®ñ"
#define		MSG_SHOP_NO_MONEY				"B¹n kh«ng că ®ñ ng©n l­îng"
#define		MSG_SHOP_NO_EXTPOINT			"B¹n kh«ng că ®ñ tỉn ®ång"
#define		MSG_SHOP_NO_FUYUAN				"B¹n kh«ng că ®ñ phóc duyªn"
#define		MSG_SHOP_NO_REPUTE				"B¹n kh«ng că ®ñ danh väng"
#define		MSG_SHOP_NO_ACCUM				"B¹n kh«ng că ®ñ tƯch ḷy"
#define		MSG_SHOP_NO_HONOR				"B¹n kh«ng că ®ñ vinh dù"
#define		MSG_SHOP_NO_RESPECT				"B¹n kh«ng că ®ñ uy danh"

#define		MSG_NPC_NO_MANA					"Néi lùc kh«ng ®ñ!"
#define		MSG_NPC_NO_STAMINA				"ThÓ lùc kh«ng ®ñ!"
#define		MSG_NPC_NO_LIFE					"Sinh lùc kh«ng ®ñ!"

#define		MSG_NPC_NOT_USE_TOWNPORTAL		"Kh«ng thÓ sö dông thæ ®̃a phï ë ®©y"
#define		MSG_NPC_NOT_PAY_FOR_SPEECH		"B¹n b̃ cÊm ph¸t ng«n ®Ơn %H:%M - %d/%m/%Y"
#define		MSG_NPC_NOT_USE_SKILL_SILENT	"B¹n kh«ng thÓ thi triÓn vâ c«ng lóc nµy"
#define		MSG_NPC_NOT_USE_SKILL_DISTANCE	"B¹n ®øng c¸ch tiªu ®iÓm qu¸ xa, tr­íc m¾t sö dông vâ c«ng nµy sÏ kh«ng ®ñ hiÖu nghiÖm"
#define		MSG_NPC_NOT_USE_SKILL_WEAPON	"Ṿ khƯ mµ b¹n ®ang sö dông kh«ng phï hîp víi lo¹i vâ c«ng mµ b¹n hiÖn ®ang sö dông"
#define		MSG_NPC_NOT_USE_SKILL_HORSE1	"Vâ c«ng mµ b¹n sö dông hiÖn giê kh«ng thÓ c­́i ngùa"
#define		MSG_NPC_NOT_USE_SKILL_HORSE2	"Vâ c«ng mµ b¹n sö dông hiÖn giê cÇn ph¶i c­́i ngùa"
#define		MSG_NPC_RENASCENCE_SOMEONE		"B¹n b̃ %s h¹ träng th­¬ng"
#define		MSG_NPC_RENASCENCE				"B¹n ®· b̃ träng th­¬ng"

#define		MSG_OBJ_CANNOT_PICKUP			"B¹n kh«ng thÓ lÊy vËt phÈm cña ng­êi kh¸c"
#define		MSG_MONEY_CANNOT_PICKUP			"B¹n kh«ng thÓ lÊy tỉn cña ng­êi kh¸c"
#define		MSG_OBJ_TOO_FAR					"VËt phÈm qu¸ xa kh«ng thÓ lÊy"
#define		MSG_DEC_MONEY					"B¹n b̃ mÊt <color=yellow>%d <color>l­îng b¹c"
#define		MSG_EARN_MONEY					"B¹n nhËn ®­îc <color=yellow>%d <color>l­îng b¹c"
#define		MSG_DEATH_LOSE_ITEM				"B¹n ®· b̃ mÊt <color=blue>%s<color>"
#define		MSG_ADD_ITEM					"B¹n nhËn ®­îc mét c¸i <color=blue>%s<color>"
#define		MSG_ADD_ITEM_STACK				"B¹n nhËn ®­îc %d c¸i <color=blue>%s<color>"

#define		MSG_ITEM_SAME_IN_IMMEDIATE		"HiÖn ®· că vËt phÈm ®ång lo¹i råi"
#define		MSG_ITEM_PUT_IN_IMMEDIATE		"Thanh phƯm t¾t kh«ng thÓ ®Æt vµo vËt phÈm nµy!"

#define		MSG_ITEM_DAMAGED				"Trang b̃ %s ®· b̃ háng råi"
#define		MSG_ITEM_NEARLY_DAMAGED			"Trang b̃ %s s¾p háng h·y ®Ơn thî r̀n ®Ó söa chöa l¹i"
#define		MSG_ITEM_REPAIRED				"Tiªu tèn %d l­îng b¹c ®Ó söa chöa %s"

#define		MSG_LOCK_NOT_THROW				"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng ®­îc tïy ư vøt bá"
#define		MSG_LOCK_NOT_TRADE				"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng thÓ giao d̃ch"
#define		MSG_LOCK_NOT_SETPRICE			"Nh»m b¶o vÖ tµi s¶n cho b¹n VËt phÈm nµy kh«ng thÓ ®̃nh gi¸"

#define		MSG_BROKEN_CANTREPAIR			"Trang b̃ ®· h­ háng hoµn toµn kh«ng thÓ söa ch÷a b»ng ph­¬ng ph¸p b×nh th­êng"

#define		MSG_TRADE_STATE_OPEN			"B¹n hiÖn giê că thÓ giao d̃ch"
#define		MSG_TRADE_STATE_CLOSE			"B¹n hiÖn giê kh«ng thÓ giao d̃ch"
#define		MSG_TRADE_SELF_LOCK				"B¹n ®· khăa chøc n¨ng giao d̃ch vËt phÈm, chê ®èi ph­¬ng tr¶ lêi"
#define		MSG_TRADE_DEST_LOCK				"Ng­¬i ch¬i %s ®· khăa chøc n¨ng giao d̃ch vËt phÈm"
#define		MSG_TRADE_SUCCESS				"B¹n cïng víi %s giao d̃ch thµnh c«ng"
#define		MSG_TRADE_FAIL					"B¹n cïng víi %s giao d̃ch thÊt b¹i"
#define		MSG_TRADE_SELF_ROOM_FULL		"Kho¶ng trèng trong tói cña B¹n kh«ng ®ñ!"
#define		MSG_TRADE_DEST_ROOM_FULL		"%s kho¶ng trèng trong tói kh«ng ®ñ!"
#define		MSG_TRADE_SEND_APPLY			"B¹n gÆp %s xin phĐp giao d̃ch!"
#define		MSG_TRADE_GET_APPLY				"%s xin phĐp cïng víi B¹n giao d̃ch!"
#define		MSG_TRADE_REFUSE_APPLY			"%s cù tuyÖt giao d̃ch víi B¹n!"
#define		MSG_TRADE_INPUT_MONEY			"§èi ph­¬ng ®Æt vµo sè tỉn %d l­îng"

#define		MSG_SPAR_SEND_APPLY				"B¹n göi lêi mêi tû thƯ vâ nghÖ ®Ơn %s"
#define		MSG_SPAR_GET_APPLY				"Ng­êi ch¬i %s göi lêi mêi tû thƯ vâ nghÖ ®Ơn víi b¹n"
#define		MSG_SPAR_REFUSE_APPLY			"Ng­êi ch¬i %s tơ chèi lêi mêi tû vâ cña b¹n"
#define		MSG_SPAR_VICTORY				"B¹n ®· chiƠn th¾ng trong lÇn tû thƯ vâ nghÖ nµy"
#define		MSG_SPAR_DEFEAT					"Tû vâ thÊt b¹i, xin ®ơng ng· lßng"

#define		MSG_TRADE_FOLKGAME_SEND_APPLY	"B¹n göi lêi mêi ®Êu ®è thuËt ®Ơn %s"
#define		MSG_TRADE_FOLKGAME_GET_APPLY	"Ng­êi ch¬i %s göi lêi mêi ®Êu ®è thuËt ®Ơn víi b¹n"
#define		MSG_TRADEFOLKGAME_SELF_ROOM_FULL	"Hµnh trang cña b¹n kh«ng ®ñ %dx%d « trèng"
#define		MSG_TRADEFOLKGAME_DEST_ROOM_FULL	"Hµnh trang cña %s kh«ng ®ñ %dx%d « trèng"

#define		MSG_PK_NORMAL_FLAG_OPEN			"Tr¹ng th¸i PK ë d¹ng chiƠn ®Êu"
#define		MSG_PK_NORMAL_FLAG_CLOSE		"Tr¹ng th¸i PK ë d¹ng luyÖn c«ng"
#define		MSG_PK_NORMAL_FLAG_DS			"Tr¹ng th¸i PK ë d¹ng thøc giƠt ng­êi"
#define		MSG_PK_NORMAL_FLAG_TONGWAR		"Tr¹ng th¸i PK ë d¹ng thøc m« h×nh bang chiƠn"
#define		MSG_PK_NORMAL_FLAG_UNKNOWN		"Tr¹ng th¸i PK ë d¹ng thøc ch­a râ"
#define		MSG_PK_HIGHT_LOCK_NOT_SWITCH	"HiÖn giê PK cña b¹n >= %d, tr­íc khi PK xin h·y hñy bá tr¹ng th¸i khăa b¶o hiÓm."
#define		MSG_PKSTATE_NOT_OPEN			"Tr¹ng th¸i PK nµy ch­a më"
#define		MSG_PK_VALUE					"Tr̃ sè PK hiÖn t¹i lµ %d"
#define		MSG_PK_ERROR_1					"B¹n hiÖn giê ®ang ë trong tr¹ng th¸i phi chiƠn ®Êu!"
#define		MSG_PK_ERROR_2					"Ng­êi míi kh«ng thÓ giƠt thï!"
#define		MSG_PK_ERROR_3					"B¹n ®ang cïng víi ng­êi kh¸c trao ®æi ư kiƠn!"
#define		MSG_PK_ERROR_4					"B¹n ®ang cïng víi ng­êi kh¸c giƠt thï!"
#define		MSG_PK_ERROR_5					"B¹n kh«ng thÓ giƠt ng­êi míi!"
#define		MSG_PK_ERROR_6					"§èi ph­¬ng ®ang ë trong tr¹ng th¸i phi chiƠn ®Êu!"
#define		MSG_PK_ERROR_7					"§èi ph­¬ng ®ang cïng víi ng­êi kh¸c giƠt thï!"
#define		MSG_PK_ERROR_8					"B¹n hiÖn giê kh«ng thÓ cơu s¸t!"
#define		MSG_PK_ERROR_9					"Ng­êi nµy ®ang trong tr¹ng th¸i ®ăng cơu s¸t"
#define		MSG_PK_ENMITY_SUCCESS_1			"%s cïng víi B¹n tiƠn hµnh cơu s¸t"
#define		MSG_PK_ENMITY_SUCCESS_2			"Sau 10 gi©y b¾t ®Çu tiƠn hµnh cơu s¸t"
#define		MSG_PK_SPAR_ENMITY_SUCCESS_1	"%s cïng víi B¹n tiƠn hµnh tû vâ"
#define		MSG_PK_SPAR_ENMITY_SUCCESS_2	"Sau 5 gi©y b¾t ®Çu tiƠn hµnh tû vâ"
#define		MSG_PK_ENMITY_CLOSE				"Tr¹ng th¸i cơu s¸t kƠt thóc"
#define		MSG_PK_SPAR_ENMITY_CLOSE		"Tr¹ng th¸i tû vâ kƠt thóc"
#define		MSG_PK_ENMITY_OPEN				"B¾t ®Çu tiƠn hµnh cơu s¸t"
#define		MSG_PK_SPAR_ENMITY_OPEN			"B¾t ®Çu tiƠn hµnh tû vâ"
#define		MGS_PK_IS_LOCK					"PK tơ phi chiƠn ®Êu chuyÓn sang luyÖn c«ng ph¶i mÊt %d phót"
#define		MGS_PK_IS_LOCK1					"Tr¹ng th¸i ®Æc biÖt kh«ng thÓ chuyÓn kiÓu chiƠn ®Êu"
#define		MGS_PK_IS_LOCK2					"Thµnh viªn bang héi míi că thÓ më chiƠn ®Êu bang héi"
#define		MGS_PK_IS_LOCK3					"ChƠ ®éi PK nhăm kh«ng thÓ tù ư chuyÓn tr¹ng th¸i chiƠn ®Êu"
#define		MGS_PK_IS_LOCK4					"Trong tæ ®éi kh«ng thÓ chuyÓn PK m« h×nh bang chiƠn"
#define		MGS_PK_IS_LOCK5					"§iÓm PK cña b¹n qu¸ nhỉu, kh«ng thÓ h¹ thñ ®­îc!"
#define		MGS_PK_IS_LOCK6					"§iÓm kinh nghiÖm ©m cña b¹n qu¸ nhỉu, kh«ng thÓ h¹ thñ ®­îc!"
#define		MGS_PK_IS_LOCK7					"PK chuyÓn gi÷a c¸c tr¹ng th¸i chiƠn ®Êu ph¶i mÊt %d gi©y"

#define		MSG_TONG_CREATE_ERROR01			"B¹n nhËp vµo tªn bang héi kh«ng hîp lÖ"
#define		MSG_TONG_CREATE_ERROR02			"Phe ph¸i trong bang héi kh«ng hîp lÖ"
#define		MSG_TONG_CREATE_ERROR03			"Thµnh viªn bang héi kh«ng thÓ kh«ng thÓ lËp mét bang héi kh¸c!"
#define		MSG_TONG_CREATE_ERROR04			"B¹n kh«ng ph¶i lµ s¸t thñ kh«ng thÓ lËp bang!"
#define		MSG_TONG_CREATE_ERROR05			"B¹n kh«ng ®ñ %d cÊp kh«ng thÓ lËp bang!"
#define		MSG_TONG_CREATE_ERROR06			"B¹n kh«ng ®ñ %d ®¼ng cÊp thèng so¸i!"
#define		MSG_TONG_CREATE_ERROR07			"B¹n kh«ng ®ñ %d ng©n l­îng b¹c ®Ó lËp bang!"
#define		MSG_TONG_CREATE_ERROR08			"B¹n ®ang trong ®éi ng̣, kh«ng thÓ lËp bang!"
#define		MSG_TONG_CREATE_ERROR09			"B¹n kh«ng că Nh¹c v­¬ng kiƠm!"
#define		MSG_TONG_CREATE_ERROR10			"B¹n ®ang bËn hoÆc ®ang giao d̃ch kh«ng thÓ lËp bang"
#define		MSG_TONG_CREATE_ERROR11			"B¹n kh«ng ®ñ %d danh väng"
#define		MSG_TONG_CREATE_ERROR12			"B¹n kh«ng ®ñ %d phóc duyªn"
#define		MSG_TONG_CREATE_ERROR13			"Sè bang héi cña server ®· ®¹t giíi h¹n kh«ng thÓ t¹o thªm"
#define		MSG_TONG_CREATE_ERROR14			"Tªn bang héi hoÆc tªn bang chñ b̃ lçi kh«ng thÓ t¹o"
#define		MSG_TONG_CREATE_ERROR15			"Bang héi ®· tån t¹i hoÆc b¹n ®ang lµ bang chñ cña mét bang héi kh¸c"
#define		MSG_TONG_APPLY_CREATE			"B¹n göi yªu cÇu thµnh lËp bang héi"
#define		MSG_TONG_CREATE_SUCCESS			"Thµnh lËp bang héi thµnh c«ng"
#define		MSG_TONG_SEND_APPLY				"B¹n göi lêi mêi gia nhËp bang héi ®Ơn %s"
#define		MSG_TONG_REV_APPLY				"%s göi lêi mêi gia nhËp bang héi ®Ơn b¹n"
#define		MSG_TONG_APPLY_ADD				"B¹n göi yªu cÇu xin gia nhËp bang héi!"
#define		MSG_TONG_APPLY_ADD_SELF_MSG		"Ng­êi ch¬i %s xin phĐp gia nhËp bang héi"
#define		MSG_TONG_APPLY_ADD_ERROR		"Ng­êi ch¬i %s kh«ng ®ñ ®ỉu kiÖn gia nhËp bang héi"
#define		MSG_TONG_APPLY_ADD_ERROR1		"Thµnh viªn bang héi kh«ng thÓ gia nhËp bang héi kh¸c"
#define		MSG_TONG_APPLY_ADD_ERROR2		"B¹n ph¶i lµ s¸t thñ míi că thÓ gia nhËp bang héi"
#define		MSG_TONG_APPLY_ADD_ERROR3		"B¹n ph¶i rêi khái tæ ®éi míi că thÓ gia nhËp bang héi"
#define		MSG_TONG_APPLY_ADD_ERROR4		"B¹n vơa chñ ®éng rêi khái bang héi, ®ang trong thêi gian thö th¸ch kh«ng thÓ göi yªu cÇu"
#define		MSG_TONG_APPLY_ADD_ERROR5		"Bang héi mµ b¹n vơa xin gia nhËp ®· ®ăng chøc n¨ng chiªu mé ®Ö tö"
#define		MSG_TONG_APPLY_ADD_ERROR6		"Bang héi ®· ®ăng chøc n¨ng chiªu mé ®Ö tö"
#define		MSG_TONG_APPLY_ADD_ERROR7		"Ng­êi nµy vơa rêi khái bang héi ch­a thÓ gia nhËp b©y giê"
#define		MSG_TONG_REFUSE_ADD				"Ng­êi ch¬i %s tơ chèi lêi mêi gia nhËp bang héi"
#define		MSG_TONG_ADD_SUCCESS			"B¹n ®· gia nhËp bang héi thµnh c«ng"
#define		MSG_TONG_CANNOT_LEAVE1			"Thµnh viªn chƯnh thøc kh«ng thÓ rêi khái bang"
#define		MSG_TONG_CANNOT_LEAVE2			"Tr­ëng l·o kh«ng thÓ rêi khái bang"
#define		MSG_TONG_BE_KICKED				"B¹n b̃ trôc xuÊt ra khái bang"
#define		MSG_TONG_KICK_ERROR1			"Thao t¸c phi ph¸p, b¹n kh«ng thuéc bÊt kú bang héi nµo"
#define		MSG_TONG_KICK_ERROR2			"Thao t¸c thÊt b¹i, ch­c Tr­ëng l·o trë lªn míi că thÓ trôc xuÊt thµnh viªn"
#define		MSG_TONG_KICK_ERROR3			"Tr­ëng l·o kh«ng thÓ trôc xuÊt trùc tiƠp"
#define		MSG_TONG_LEAVE_SUCCESS			"B¹n ®· rêi khái bang héi thµnh c«ng, că thÓ xin gia nhËp bang héi kh¸c sau %d phót"
#define		MSG_TONG_LEAVE_FAIL				"Rêi khái bang héi thÊt b¹i"
#define		MSG_TONG_NO_MONEY				"B¹n kh«ng că ®ñ ng©n l­îng kh«ng thÓ tù ư rêi khái"
#define		MSG_TONG_CHANGE_AS_MASTER		"B¹n b©y giê ®¶m nhËn chøc ṽ Bang chñ"
#define		MSG_TONG_CHANGE_MASTER_ERROR1	"ChuyÓn ṽ thÊt b¹i, ®èi ph­¬ng kh«ng trùc tuyƠn!"
#define		MSG_TONG_CHANGE_MASTER_ERROR2	"ChuyÓn ṽ thÊt b¹i, ®èi ph­¬ng n¨ng lùc kh«ng ®ñ!"
#define		MSG_TONG_CHANGE_MASTER_ERROR3	"ChuyÓn ṽ thÊt b¹i, trong bang kh«ng că ng­êi nµy!"
#define		MSG_TONG_CHANGE_AS_MEMBER		"B©y giê b¹n trë thµnh thµnh viªn b×nh th­êng"
#define		MSG_TONG_CHANGE_AGNAME_ERROR1	"Ng­êi ch¬i nµy hiÖn kh«ng trùc tuyƠn, thao t¸c thÊt b¹i"
#define		MSG_TONG_CHANGE_AGNAME_ERROR2	"Ng­êi ch¬i nµy kh«ng ®ñ tµi l·nh ®¹o, thao t¸c thÊt b¹i"
#define		MSG_TONG_CHANGE_AGNAME_ERROR3	"Ng­êi ch¬i nµy kh«ng tån t¹i, thao t¸c thÊt b¹i"
#define		MSG_TONG_CHANGE_CAMP			"TiƠn hµnh thay ®æi phe ph¸i bæn bang"
#define		MSG_TONG_CHANGE_CAMP_ERROR1		"ChØ că bang chñ míi sö dông ®­îc chøc n¨ng nµy"
#define		MSG_TONG_CHANGE_CAMP_ERROR2		"Vui lßng chän phe ph¸i thay ®æi ®Ơn"
#define		MSG_TONG_CHANGE_CAMP_ERROR3		"Ng©n khè kh«ng că ®ñ %d l­îng, kh«ng thÓ thay ®æi phe ph¸i bæn bang"
#define		MSG_TONG_CHANGE_SAVEJIYU_ERROR	"Ng©n khè kh«ng că ®ñ %d l­îng, kh«ng thÓ b¶o l­u"

#define		MSG_CANT_SIT_RIDE				"Lóc c­́i ngùa kh«ng thÓ thỉn täa"
#define		MSG_CANT_SWITCH_HORSE_SIT		"B¹n ®ang thỉn täa kh«ng thÓ lªn ngùa"
#define		MSG_CANT_SWITCH_HORSE			"B¹n qu¸ mÖt mái kh«ng thÓ liªn tôc lªn hoÆc xuèng ngùa"
#define		MSG_CANT_TRADE_BUZY				"Trong lóc ći ngùa hoÆc ®ang giao d̃ch víi tæ ®éi kh«ng thÓ bµy b¸n!"

#define		MESSAGE_BROADCAST_ANNOUCE_HEAD	"C«ng bè"
#define		MESSAGE_SYSTEM_ANNOUCE_HEAD		"HÖ thèng"
#define		MESSAGE_SYSTEM_ANNOUCE_SKILL	"HÖ thèng kü n¨ng"
#define		MESSAGE_SYSTEM_ANNOUCE_PROTECT	"HÖ thèng b¶o mËt"
#define		MESSAGE_TEAM_ANNOUCE_HEAD		"Tin ®éi"
#define		MESSAGE_TONG_ANNOUCE_HEAD		"Tin bang"
#define		MESSAGE_ROOM_ANNOUCE_HEAD		"Tin phßng"

#define		defTONG_VICEROY_AGNAME			"[Th¸i thó]"
#define		defTONG_MASTER_AGNAME			"Bang chñ"
#define		defTONG_DIRECTOR_AGNAME			"Tr­ëng l·o"
#define		defTONG_MANAGER_AGNAME			"§éi tr­ëng"
#define		defTONG_MEMBER_AGNAME			"§Ö tö"

#define		MSG_TRADE_COUNT					"B¹n ch­a ®̃nh gi¸ cho bÊt kú vËt phÈm nµo"
#define		MSG_TRADE_SELLED				"VËt phÈm nµy ®· ®­îc b¸n ra råi!"
#define		MSG_TRADE_CLOSED				"Ng­êi bµy b¸n ®· cÊt ®i råi"

#define		MSG_FIGHT_MODE_ERROR1			"Tr¹ng th¸i kh«ng ®­îc b¶o vÖ kh«ng thÓ thùc hiÖn"
#define		MSG_FIGHT_MODE_ERROR2			"B¹n trong tr¹ng th¸i kh«ng ®­îc b¶o vÖ kh«ng thÓ thùc hiÖn"
#define		MSG_FIGHT_MODE_ERROR3			"%s trong tr¹ng th¸i kh«ng ®­îc b¶o vÖ kh«ng thÓ thùc hiÖn"

#define		MSG_SUBWORLD_NOT_OPEN			"§­êng ®i phƯa tr­íc kh«ng th«ng"

#define		MSG_PASSWORD_ERROR1				"MËt khÈu ph¶i că ®é dµi tơ 4 ®Ơn 16 kó tù"
#define		MSG_PASSWORD_ERROR2				"MËt khÈu x¸c nhËn kh«ng trïng khíp. Xin mêi nhËp l¹i"
#define		MSG_PASSWORD_ERROR3				"MËt khÈu míi nhËp vµo kh«ng thÓ trïng víi mËt khÈu hiÖn t¹i"

#define		MSG_FUNCTION_FOBIDDENED			"Xin lçi, c«ng n¨ng nµy t¹m thêi ®· b̃ ®ăng!"
#define		MSG_FUNCTION_LOCKED				"Thao t¸c ®· b̃ khăa! vui lßng më khăa b¶o vÖ tr­íc khi thùc hiÖn."


enum enumMSG_ID
{
	enumMSG_ID_NONE = 0,
	enumMSG_ID_TEAM_KICK_ONE,
	enumMSG_ID_TEAM_DISMISS,
	enumMSG_ID_TEAM_LEAVE,
	enumMSG_ID_TEAM_REFUSE_INVITE,
	enumMSG_ID_TEAM_APPLY_INVITE_FAIL,
	enumMSG_ID_TEAM_APPLY_INVITE_FAIL2,
	enumMSG_ID_TEAM_SELF_ADD,
	enumMSG_ID_TEAM_ADDMEMBER_NOTVALID,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL,
	enumMSG_ID_TEAM_CHANGE_CAPTAIN_FAIL2,
	enumMSG_ID_OBJ_CANNOT_PICKUP,
	enumMSG_ID_OBJ_TOO_FAR,
	enumMSG_ID_DEC_MONEY,
	enumMSG_ID_SPAR_VICTORY,
	enumMSG_ID_SPAR_DEFEAT,
	enumMSG_ID_TRADE_SELF_ROOM_FULL,
	enumMSG_ID_TRADE_DEST_ROOM_FULL,
	enumMSG_ID_TRADE_REFUSE_APPLY,
	enumMSG_ID_TRADEFOLKGAME_SELF_ROOM_FULL,
	enumMSG_ID_TRADEFOLKGAME_DEST_ROOM_FULL,
	enumMSG_ID_SPAR_REFUSE_APPLY,
	enumMSG_ID_GET_ITEM,
	enumMSG_ID_MONEY_CANNOT_PICKUP,
	enumMSG_ID_CANNOT_ADD_TEAM,
	enumMSG_ID_TEAM_IS_FULL,
	enumMSG_ID_CANNOT_INVITE_TEAM,
	enumMSG_ID_TARGET_CANNOT_ADD_TEAM,
	enumMSG_ID_GET_INVITE_TEAM_REPLY,
	enumMSG_ID_PK_NORMAL_NOT_OPEN,
	enumMSG_ID_PK_HIGHT_LOCK_NOT_SWITCH,
	enumMSG_ID_PKSTATE_NOT_OPEN,
	enumMSG_ID_PK_ERROR_1,
	enumMSG_ID_PK_ERROR_2,
	enumMSG_ID_PK_ERROR_3,
	enumMSG_ID_PK_ERROR_4,
	enumMSG_ID_PK_ERROR_5,
	enumMSG_ID_PK_ERROR_6,
	enumMSG_ID_PK_ERROR_7,
	enumMSG_ID_PK_ERROR_8,
	enumMSG_ID_PK_ERROR_9,
	enumMSG_ID_PK_IS_LOCK,
	enumMSG_ID_PK_IS_LOCK1,
	enumMSG_ID_PK_IS_LOCK2,
	enumMSG_ID_PK_IS_LOCK3,
	enumMSG_ID_PK_IS_LOCK4,
	enumMSG_ID_PK_IS_LOCK5,
	enumMSG_ID_PK_IS_LOCK6,
	enumMSG_ID_PK_IS_LOCK7,
	enumMSG_ID_TRADE_BUZY,
	enumMSG_ID_TRADE_SELLED,
	enumMSG_ID_TRADE_CLOSED,
	enumMSG_ID_DEATH_LOSE_ITEM,
	enumMSG_ID_TONG_REFUSE_ADD,
	enumMSG_ID_TONG_BE_KICK,
	enumMSG_ID_TONG_LEAVE_SUCCESS,
	enumMSG_ID_TONG_LEAVE_FAIL,
	enumMSG_ID_TONG_NO_MONEY,
	enumMSG_ID_TONG_CHANGE_AS_MASTER,
	enumMSG_ID_TONG_CHANGE_AS_MEMBER,
	enumMSG_ID_TONG_APPLY_ADD_ERROR,
	enumMSG_ID_TONG_APPLY_ADD_ERROR1,
	enumMSG_ID_TONG_APPLY_ADD_ERROR2,
	enumMSG_ID_TONG_APPLY_ADD_ERROR3,
	enumMSG_ID_TONG_APPLY_ADD_ERROR4,
	enumMSG_ID_TONG_APPLY_ADD_ERROR5,
	enumMSG_ID_TONG_APPLY_ADD_ERROR6,
	enumMSG_ID_TONG_APPLY_ADD_ERROR7,
	enumMSG_ID_FIGHT_MODE_ERROR1,
	enumMSG_ID_FIGHT_MODE_ERROR2,
	enumMSG_ID_FIGHT_MODE_ERROR3,
	enumMSG_ID_SHOP_NO_ROOM,
	enumMSG_ID_SHOP_NO_MONEY,
	enumMSG_ID_SHOP_NO_EXTPOINT,
	enumMSG_ID_SHOP_NO_FUYUAN,
	enumMSG_ID_SHOP_NO_REPUTE,
	enumMSG_ID_SHOP_NO_ACCUM,
	enumMSG_ID_SHOP_NO_HONOR,
	enumMSG_ID_SHOP_NO_RESPECT,
	enumMSG_ID_FUNCTION_CHAT_FORBIDDENED,
	enumMSG_ID_FUNCTION_FOBIDDENED,
	enumMSG_ID_FUNCTION_LOCKED,
	enumMSG_ID_LOCK_NOT_TRADE,
	enumMSG_ID_LOCK_NOT_SETPRICE,
	enumMSG_ID_LOCK_NOT_THROW,
	enumMSG_ID_ITEM_TOWNPORTAL,
	enumMSG_ID_ITEM_CANTREPAIR,
	enumMSG_ID_ITEM_SAME_IMMEDIATE,
	enumMSG_ID_ITEM_CANTPUT_IMMEDIATE,
	enumMSG_ID_HORSE_CANT_SWITCH1,
	enumMSG_ID_HORSE_CANT_SWITCH2,
	enumMSG_ID_CHATROOM_JOIN,
	enumMSG_ID_CHATROOM_LEAVE,
	enumMSG_ID_CHATROOM_BEKICK,
	enumMSG_ID_CHATROOM_OPENGAME,
	enumMSG_ID_CHATROOM_REVERSE,
	enumMSG_ID_NPC_RENASCENCE_SOMEONE,
	enumMSG_ID_NPC_RENASCENCE,
	enumMSG_ID_NUM,
};

//---------------------------- Êó±êÖ¸ƠëÏà¹Ø ------------------------------
#define		MOUSE_CURSOR_NORMAL				0
#define		MOUSE_CURSOR_FIGHT				1
#define		MOUSE_CURSOR_DIALOG				2
#define		MOUSE_CURSOR_PICK				3
#define		MOUSE_CURSOR_USE				8
#define		MOUSE_CURSOR_TRADE				11
#define		MOUSE_CURSOR_FINGER				12
#define		MOUSE_CURSOR_HELP				13
#endif
