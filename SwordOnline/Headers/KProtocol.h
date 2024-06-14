#ifndef KPROTOCOL_H
#define KPROTOCOL_H

#ifndef __linux
#ifdef _STANDALONE
#include "GameDataDef.h"
#else
#include "../Sources/Core/src/GameDataDef.h"
#endif
#else
#include "GameDataDef.h"
#include <string.h>
#endif

#include "KProtocolDef.h"
#include "KRelayProtocol.h"
#include <memory>
#pragma pack(push, enter_protocol)
#pragma	pack(1)

#define	PROTOCOL_MSG_TYPE	BYTE
#define PROTOCOL_MSG_SIZE	(sizeof(PROTOCOL_MSG_TYPE))
#define	MAX_PROTOCOL_NUM	207

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		TeamServerID;
	BYTE	HelmType;
	BYTE	ArmorType;
	BYTE	WeaponType;
	BYTE	HorseType;
	BYTE	RankID;
	KExpandRank		ExpandRank;
	DWORD	TongNameID;
	char	TongName[16];
	int		TongNationalEmblem;
	char	TongAgname[16];
	BYTE	m_Figure;
	BYTE	m_btSomeFlag;
	BYTE	Translife;
	BYTE	Viprank;
	int		Repute;
	int		FuYuan;
	BYTE	PKFlag;
	BYTE	PKValue;
	BYTE	MantleLevel;
	int		MaskType;
	BYTE	MantleType;
	int		HelmPlayer;
	PLAYERTRADE	Trade;
	char	MateName[16];
	int 	RankInWorld;
	BYTE	PacePer;
	int		FactionNumber;
	BYTE	ImagePlayer;
} PLAYER_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		TeamServerID;
	BYTE	HelmType;
	BYTE	ArmorType;
	BYTE	WeaponType;
	BYTE	HorseType;
	BYTE	RankID;
	KExpandRank		ExpandRank;
	DWORD	TongNameID;
	char	TongName[16];
	int		TongNationalEmblem;
	char	TongAgname[16];
	BYTE	m_Figure;
	BYTE	m_btSomeFlag;
	BYTE	Translife;
	BYTE	Viprank;
	int		Repute;
	int		FuYuan;
	BYTE	PKFlag;
	BYTE	PKValue;
	BYTE	MantleLevel;
	int		MaskType;
	BYTE	MantleType;
	PLAYERTRADE	Trade;
	char	MateName[16];
	int 	RankInWorld;
	BYTE	PacePer;
	BYTE	ImagePlayer;
	int		FactionNumber;
} PLAYER_NORMAL_SYNC;


typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	Camp;				// 阵营
	BYTE	CurrentCamp;		// 当前阵营
	BYTE	m_bySeries;			// 五行系
	int		CurrentLife;
	int		CurrentLifeMax;
	BYTE	m_btMenuState;		// 组队、交易等状态
	BYTE	m_Doing;			// 行为
	BYTE	m_btKind;			// npc类型
	BYTE	Special;			// 加强的NPC（后面的bit表示加强类型，名字是否金色之类的由加强的数目在客户端确定）
	int		MissionGroup;
	int		Dir;
	DWORD	MapX;				// 位置信息
	DWORD	MapY;				// 位置信息
	DWORD	ID;					// Npc的唯一ID
	int		NpcSettingIdx;		// 客户端用于加载玩家资源及基础数值设定
	char	m_szName[64];		// 名字
} NPC_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		MapX;
	int		MapY;
	BYTE	Camp;
	int		CurrentLife;
	int		CurrentLifeMax;
	BYTE	Doing;
	BYTE	State;
	BYTE	WalkSpeed;
	BYTE	RunSpeed;
	BYTE	AttackSpeed;
	BYTE	CastSpeed;
	BYTE	StateInfo[MAX_SKILL_STATE];
} NPC_NORMAL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		MapX;
	int		MapY;
	int		OffX;
	int		OffY;
} NPC_PLAYER_TYPE_NORMAL_SYNC;
                 
typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
} NPC_SIT_SYNC, NPC_DEATH_SYNC, NPC_REQUEST_COMMAND, NPC_REQUEST_FAIL, NPC_MASK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BOOL	Rv;
} NPC_REMOVE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Type;
} NPC_REVIVE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_JUMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nMpsX;
	int		nMpsY;
} NPC_RUN_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nFrames;
	int		nX;
	int		nY;
} NPC_HURT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Camp;
} NPC_CHGCURCAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	BYTE	Camp;
} NPC_CHGCAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		nSkillID;
	int		nSkillLevel;
	int		nMpsX;
	int		nMpsY;
} NPC_SKILL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	char	szMsg[MAX_SENTENCE_LENGTH];
	int		nMsgLen;
} NPC_CHAT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	int		nSkillID;
	int		nMpsX;
	int		nMpsY;
} NPC_SKILL_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMpsX;
	int		nMpsY;
} NPC_WALK_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
} NPC_REVIVE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	int		nMpsX;
	int		nMpsY;
} NPC_RUN_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	BuySellInfo m_BuySellInfo;
} SALE_BOX_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BOOL	bEquipEx;
	int		nBoxIndex;
} SOME_BOX_SYNC;

typedef struct 
{
	BYTE	ProtocolType;
	int		nNpcId;
} PLAYER_DIALOG_NPC_COMMAND; //主角与nNpcId对话的请求

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nExp;				// 当前经验
} PLAYER_EXP_SYNC;				// 玩家同步经验

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_CREATE_TEAM;		// 客户端玩家创建队伍，向服务器发请求

struct PLAYER_SEND_CREATE_TEAM_SUCCESS
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	nTeamServerID;		// 队伍在服务器上的唯一标识
	PLAYER_SEND_CREATE_TEAM_SUCCESS() {nTeamServerID = -1;}
};	// 服务器通知玩家队伍创建成功

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btErrorID;		// 队伍创建不成功原因：0 同名 1 玩家本身已经属于某一支队伍 3 当前处于不能组队状态
} PLAYER_SEND_CREATE_TEAM_FALSE;// 服务器通知客户端队伍创建不成功

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwTarNpcID;		// 查询目标 npc id
} PLAYER_APPLY_TEAM_INFO;		// 客户端向服务器申请查询某个npc的组队情况

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_TEAM_INFO_FALSE;	// 服务器告知客户端申请查询某个npc的组队情况失败

typedef struct PLAYER_SEND_TEAM_INFO_DATA
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nCaptain;			// 队长 npc id
	int		m_nMember[MAX_TEAM_MEMBER];	// 所有队员 npc id
	DWORD	nTeamServerID;		// 队伍在服务器上的唯一标识
	PLAYER_SEND_TEAM_INFO_DATA() {nTeamServerID = -1;};
} PLAYER_SEND_TEAM_INFO;		// 服务器向客户端发送某个队伍的信息数据

typedef struct PLAYER_SEND_SELF_TEAM_INFO_DATA
{
	BYTE	ProtocolType;							// 协议名称
	BYTE	m_btState;								// 队伍状态
	DWORD	m_dwNpcID[MAX_TEAM_MEMBER + 1];			// 每名成员的npc id （队长放在第一位）
	char	m_szNpcName[MAX_TEAM_MEMBER + 1][32];	// 每名成员的名字（队长放在第一位）
	DWORD	nTeamServerID;							// 队伍在服务器上的唯一标识
	DWORD	m_nLeadExp;							// 玩家的统率力经验
	BYTE	m_btLevel[MAX_TEAM_MEMBER + 1];			// 每名成员的等级（队长放在第一位）
	PLAYER_SEND_SELF_TEAM_INFO_DATA() {memset(m_szNpcName, 0, 32 * (MAX_TEAM_MEMBER + 1)); nTeamServerID = -1;};
} PLAYER_SEND_SELF_TEAM_INFO;						// 服务器向客户端发送客户端自身队伍的信息数据

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btState;
	BYTE	m_btFlag;		// 打开或关闭
} PLAYER_TEAM_CHANGE_STATE;		// 队伍队长向服务器申请开放、关闭队伍是否允许接收成员状态

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwTarNpcID;		// 目标队伍队长npc id 或者 申请人 npc id
} PLAYER_APPLY_ADD_TEAM;		// 玩家向服务器申请加入某个队伍或者服务器向某个队长转发某个玩家的加入申请

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 被接受入队伍的npc id
} PLAYER_ACCEPT_TEAM_MEMBER;	// 玩家通知服务器接受某个玩家入队伍

typedef struct PLAYER_TEAM_ADD_MEMBER_DATA
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btLevel;			// 加入者等级
	DWORD	m_dwNpcID;			// 加入者npc id
	char	m_szName[32];		// 加入者姓名
	PLAYER_TEAM_ADD_MEMBER_DATA() {memset(m_szName, 0, 32);};
} PLAYER_TEAM_ADD_MEMBER;		// 服务器通知队伍中的各个玩家有新成员加入

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BOOL	bMySelf;
} PLAYER_APPLY_LEAVE_TEAM;		// 客户端玩家申请离队

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 离队npc id
} PLAYER_LEAVE_TEAM;			// 服务器通知各队员某人离队

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 离队npc id
} PLAYER_TEAM_KICK_MEMBER;		// 队长踢除某个队员

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwNpcID;			// 目标npc id
	BOOL	m_bMySelf;
} PLAYER_APPLY_TEAM_CHANGE_CAPTAIN;// 队长向服务器申请把自己的队长身份交给别的队员

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	DWORD	m_dwCaptainID;		// 新队长npc id
	DWORD	m_dwMemberID;		// 新队员npc id
	BOOL	m_bMySelf;
} PLAYER_TEAM_CHANGE_CAPTAIN;	// 服务器通知各队员更换队长

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
} PLAYER_APPLY_TEAM_DISMISS;	// 向服务器申请解散队伍

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btPKFlag;			// pk 开关
} PLAYER_SET_PK;				// 向服务器申请打开、关闭PK

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
	BYTE	m_btCamp;				// 新阵营
	BYTE	m_btCurFaction;			// 当前门派
	BYTE	m_btFirstFaction;		// 首次加入门派
	int		m_nAddTimes;			// 加入门派次数
} PLAYER_FACTION_DATA;				// 服务器发给客户端门派信息

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
} PLAYER_LEAVE_FACTION;				// 服务器通知玩家离开门派

typedef struct
{
	BYTE	ProtocolType;			// 协议名称
	int		m_nMissionGroup;				// 新阵营
} PLAYER_MISSION_DATA;				// 服务器发给客户端门派信息

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btState;
	int		m_nID;
	int		m_nDataID;
	int		m_nXpos;
	int		m_nYpos;
	int		m_nMoneyNum;
	int		m_nItemID;
	BYTE	m_btDir;
	BYTE	m_btItemWidth;
	WORD	m_wCurFrame;
	BYTE	m_btItemHeight;
	BYTE	m_btColorID;
	int		m_nGenre;
	int		m_nDetailType;
	BYTE	m_btFlag;
	char	m_szName[OBJ_NAME_LENGTH];
} OBJ_ADD_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btState;
	int		m_nID;
} OBJ_SYNC_STATE;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btDir;
	int		m_nID;
} OBJ_SYNC_DIR;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
	BYTE	m_btSoundFlag;
} OBJ_SYNC_REMOVE;

typedef struct
{
	BYTE	ProtocolType;
	int		m_nID;
} OBJ_CLIENT_SYNC_ADD;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nLeadLevel;		// 统率力经验值
	int		m_nLeadExp;		// 统率力经验值
} PLAYER_LEAD_EXP_SYNC;			// 同步统率力经验值

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BOOL	m_bSetLevel;
	BYTE	m_btLevel;			// 当前等级
	int		m_nExp;				// 当前经验
	int		m_nAttributePoint;	// 剩余属性点
	int		m_nSkillPoint;		// 剩余技能点
	int		m_nBaseLifeMax;		// 当前最大生命值
	int		m_nBaseStaminaMax;	// 当前最大体力值
	int		m_nBaseManaMax;		// 当前最大内力值
} PLAYER_LEVEL_UP_SYNC;			// 玩家升级

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btLevel;			// 当前等级
	DWORD	m_dwTeammateID;		// 队友 npc id
} PLAYER_TEAMMATE_LEVEL_SYNC;	// 玩家升级的时候通知队友

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btAttribute;		// 属性(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nAddNo;			// 加的点数
} PLAYER_ADD_BASE_ATTRIBUTE_COMMAND;	// 玩家添加基本属性点

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nSkillID;			// 技能id
	int		m_nAddPoint;		// 要加的点数
} PLAYER_ADD_SKILL_POINT_COMMAND;// 玩家申请增加某个技能的点数

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btAttribute;		// 属性(0=Strength 1=Dexterity 2=Vitality 3=Engergy)
	int		m_nBasePoint;		// 基本点数
	int		m_nCurPoint;		// 当前点数
	int		m_nLeavePoint;		// 剩余未分配属性点
} PLAYER_ATTRIBUTE_SYNC;		// 玩家同步属性点

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nSkillID;			// 技能id
	int		m_nSkillLevel;		// 技能等级
	int		m_nAddLevel;		// 装备添加的技能点
	int		m_nSkillExp;		// 技能等级
	BOOL	m_bTempSkill;
	int		m_nLeavePoint;		// 剩余未分配技能点
} PLAYER_SKILL_LEVEL_SYNC;		// 玩家同步技能点

//typedef struct
//{
//	BYTE	ProtocolType;		// 协议名称
//	int		m_nItemID;			// 物品id
//	int		m_nSourcePos;		// 来源位置
//	int		m_nTargetPos;		// 目的位置
//} PLAYER_EQUIP_ITEM_COMMAND;	// 玩家鼠标右键点击使用物品(装备)

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btPlace;			// 药品位置
	BYTE	m_btX;				// 药品位置
	BYTE	m_btY;				// 药品位置
	int		m_nItemID;			// 物品id
} PLAYER_EAT_ITEM_COMMAND;		// 玩家鼠标右键点击使用物品(吃药)

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	int		m_nObjID;			// 鼠标点击的obj的id
	BYTE	m_btPosType;		// 位置类型
	BYTE	m_btPosX;			// 坐标 x
	BYTE	m_btPosY;			// 坐标 y
} PLAYER_PICKUP_ITEM_COMMAND;		// 玩家获得物品（鼠标点击地图上的obj）

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BOOL			m_bIsNew;
	BOOL			m_bTemp;
	DWORD			m_dwOwner;
	int				m_TradePrice;
	int				m_ID;				// 物品的ID
	BYTE			m_Nature;
	BYTE			m_Genre;			// 物品的类型
	int				m_Detail;			// 物品的类别
	int				m_Particur;			// 物品的详细类别
	BYTE			m_Series;			// 物品的五行
	BYTE			m_Level;			// 物品的等级
	BYTE			m_btPlace;			// 坐标
	BYTE			m_btX;				// 坐标
	BYTE			m_btY;				// 坐标
	PlayerItem		m_BackLocal;
	int				m_ItemX;
	int				m_ItemY;
	int				m_Luck;				// MF
	int				m_MagicLevel[MAX_ITEM_MAGICLEVEL];	// 生成参数
	WORD			m_Version;			// 装备版本
	int				m_Durability;		// 耐久度
	UINT			m_RandomSeed;		// 随机种子
	BYTE			m_bStack;
	int				m_StackNum;
	int				m_ExpireTime;
	KLockItem		m_LockItem;
	BOOL			m_bLockSell;
	BOOL			m_bLockTrade;
	BOOL			m_bLockDrop;
	int				m_Param;
	int				m_Mantle;
	int				m_Width;
	int				m_Height;
	int				m_Fortune;
} ITEM_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD			m_dwID;
	int				m_MagicLevel[MAX_ITEM_MAGICLEVEL];
	KMagicAttrib	m_MagicAttrib[MAX_ITEM_MAGICATTRIB];
} ITEM_SYNC_MAGIC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
} ITEM_REMOVE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_ID;				// 物品的ID
	BYTE			m_Number;			
} PLAYER_SELL_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_Shop;
	BYTE			m_BuyIdx;			// 买第几个东西
	BYTE			m_Number;			// new add
} PLAYER_BUY_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney1;			// 装备栏
	int				m_nMoney2;			// 贮物箱
	int				m_nMoney3;			// 交易栏
} PLAYER_MONEY_SYNC;					// 服务器通知客户端钱的数量

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDownPos;
	BYTE			m_btDownX;
	BYTE			m_btDownY;
	BYTE			m_btUpPos;
	BYTE			m_btUpX;
	BYTE			m_btUpY;
} PLAYER_MOVE_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDownPos;
	BYTE			m_btDownX;
	BYTE			m_btDownY;
	BYTE			m_btUpPos;
	BYTE			m_btUpX;
	BYTE			m_btUpY;
} PLAYER_MOVE_ITEM_SYNC;


typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btSrcPos;
	BYTE			m_btSrcX;
	BYTE			m_btSrcY;
	BYTE			m_btDestPos;
	BYTE			m_btDestX;
	BYTE			m_btDestY;
} ITEM_AUTO_MOVE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
} PLAYER_THROW_AWAY_ITEM_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int 			m_shLife;
	int 			m_shStamina;
	int 			m_shMana;
	short			m_shAngry;
	BYTE			m_btTeamData;
} CURPLAYER_NORMAL_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	// npc部分
	DWORD			m_dwID;				// Npc的ID
	BYTE			m_btLevel;			// Npc的等级
	BYTE			m_btSex;			// 性别
	BYTE			m_btKind;			// Npc的类型
	BYTE			m_btSeries;			// Npc的五行系
	int 			m_wLifeMax;			// Npc的最大生命
	int 			m_wStaminaMax;		// Npc的最大体力
	int 			m_wManaMax;			// Npc的最大内力
	int				m_HeadImage;
	// player 部分
	WORD			m_wAttributePoint;	// 未分配属性点
	WORD			m_wSkillPoint;		// 未分配技能点
	WORD			m_wStrength;		// 玩家的基本力量（决定基本伤害）
	WORD			m_wDexterity;		// 玩家的基本敏捷（决定命中、体力）
	WORD			m_wVitality;		// 玩家的基本活力（决定生命、体力）
	WORD			m_wEngergy;			// 玩家的基本精力（决定内力）
	WORD			m_wLucky;			// 玩家的基本幸运值
	int				m_nExp;				// 当前经验值(当前等级在npc身上)
	BYTE			m_byTranslife;
	int				m_nLeadExp;		// 统率力经验值

	// 门派
	BYTE			m_btCurFaction;		// 当前门派
	BYTE			m_btFirstFaction;	// 第一次加入的是哪个门派
	int				m_nFactionAddTimes;	// 加入各种门派的总次数
	
	int				m_nMissionGroup;
	int				m_nRoomId;
	// 排名
	WORD			m_wWorldStat;		// 世界排名
	WORD			m_wSectStat;		// 门派排名
	int				m_nKillPeopleNumber;
	// 钱
	int				m_nMoney1;
	int				m_nMoney2;
	DWORD			m_dwEquipExpandTime;
	BYTE			m_btRepositoryNum;
	DWORD			m_dwLeaveTongTime;
	BYTE			m_btImagePlayer;
} CURPLAYER_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wProtocolLong;
	BYTE	m_nOperateType;				//操作类型
	BYTE	m_bUIId, m_bOptionNum, m_bParam1, m_bParam2;// m_bParam1,主信息是数字标识还是字符串标识, m_bParam2,是否是与服务器交互的选择界面
	BOOL	m_bParam;
	int		m_nParam, m_nParam1;
	int		m_nBufferLen;
	char	m_szKey[MAX_PATH];
	char	m_pContent[MAX_SCIRPTACTION_BUFFERNUM];				//带控制符
} PLAYER_SCRIPTACTION_SYNC;

typedef struct
{
	WORD	SkillId;
	BYTE	SkillLevel;
	int		SkillExp;
} SKILL_SEND_ALL_SYNC_DATA;

typedef struct
{
	BYTE						ProtocolType;
	WORD						m_wProtocolLong;
	SKILL_SEND_ALL_SYNC_DATA	m_sAllSkill[MAX_NPCSKILL];
} SKILL_SEND_ALL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	WeatherID;
} SYNC_WEATHER;

typedef struct defWORLD_SYNC
{
	BYTE	ProtocolType;
	int		SubWorld;
	int		Region;
	BYTE	Weather;
	DWORD	Frame;
} WORLD_SYNC;

typedef struct 
{
	BYTE	ProtocolType;
	int		nSelectIndex;
	int		nSelectUi;
}PLAYER_SELECTUI_COMMAND;

typedef struct
{
	BYTE			ProtocolType;	// c2s_viewequip
	DWORD			m_dwNpcID;
} VIEW_EQUIP_COMMAND;

typedef struct
{
	BYTE			ProtocolType;	// c2s_viewequip
	DWORD			m_dwNpcID;
} PLAYER_TRADE_VIEW_COMMAND;

//此结构已经被tagDBSelPlayer结构替换
/*typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nSelect;			// 
} DB_PLAYERSELECT_COMMAND;*/

/*
 * { Add by liupeng 2003.05.10
 *
 * #pragma pack( push, 1 )
*/

/*
 * Nonstandard extension used : zero-sized array in struct/union
 */
#pragma warning(disable: 4200)

#define KSG_PASSWORD_MAX_SIZE   64

typedef struct tagKSG_PASSWORD
{
    char szPassword[KSG_PASSWORD_MAX_SIZE];    // 现在采用MD5的字符串，由于是32个字符，加上末尾'\0'，需要至少33个空间，因此使用64
} KSG_PASSWORD;

#define _NAME_LEN	32

struct tagProtoHeader
{
	BYTE	cProtocol;
};

struct tagResult : public tagProtoHeader
{
	BYTE	cResult;
};

struct tagDBSelPlayer : public tagProtoHeader
{
	char	szRoleName[_NAME_LEN];
};

struct tagDBDelPlayer : public tagProtoHeader
{
	char	        szAccountName[_NAME_LEN];
    KSG_PASSWORD    Password;
	char	        szRoleName[_NAME_LEN];
};

//删除与新建角色的返回消息带的数据
struct tagNewDelRoleResponse : public tagDBSelPlayer
{
	bool	bSucceeded;		//是否成功
};

// 2003.05.11
struct tagDBSyncPlayerInfo : public tagProtoHeader
{
	size_t	dataLength;
	char	szData[0];
};

/*
 * 2003.06.27
 * s2c_gateway_broadcast
*/

#define	AP_WARNING_ALL_PLAYER_QUIT			1
#define	AP_NOTIFY_GAMESERVER_SAFECLOSE		2
#define	AP_NOTIFY_ALL_PLAYER				3

#define	AP_PUNISH_LOCK						1
#define	AP_PUNISH_CHAT						2
#define	AP_LOCK_ACCOUNTNAME					1
#define	AP_UNLOCK_ACCOUNTNAME				0
#define	AP_LOCK_CHAT						2
#define	AP_UNLOCK_CHAT						3
#define	MAX_GATEWAYBROADCAST_LEN	260
struct tagGatewayBroadCast : public tagProtoHeader
{
	UINT	uCmdType;
	char	szData[MAX_GATEWAYBROADCAST_LEN];
};

/*
 * 2003.05.22
 * s2c_syncgamesvr_roleinfo_cipher
*/
struct tagGuidableInfo : public tagProtoHeader
{
	GUID guid;
	WORD nExtPoint;			//可用的附送点
	WORD nChangePoint;		//变化的附送点
	size_t	datalength;
	char	szData[0];

};

/*
 * c2s_permitplayerlogin
 */
struct tagPermitPlayerLogin : public tagProtoHeader
{
	GUID guid;

	BYTE szRoleName[_NAME_LEN];
	BYTE szAccountName[_NAME_LEN];

	/*
	 * Succeeded : true
	 * Failed	 : false
	 */
	bool bPermit;
};

struct tagPermitPlayerExchange
{
	BYTE cProtocol;
	GUID guid;
	DWORD dwIp;
	WORD wPort;
	bool bPermit;
};
/*
 * c2s_notifyplayerlogin
 */
struct tagNotifyPlayerLogin : public tagPermitPlayerLogin
{
	UINT			nIPAddr;
	unsigned short	nPort;
};

/*
 * s2c_querymapinfo
 */
struct tagQueryMapInfo : public tagProtoHeader
{	
};

/*
 * s2c_querygameserverinfo
 */
struct tagQueryGameSvrInfo : public tagProtoHeader
{	
};

/*
 * s2c_notifysvrip
 */
struct tagNotifySvrIp : public tagProtoHeader
{
	WORD	pckgID;

	BYTE	cIPType;
	DWORD	dwMapID;

	DWORD	dwSvrIP;
};

/*
 * s2c_notifyplayerexchange
 */
struct tagNotifyPlayerExchange : public tagProtoHeader
{
	GUID			guid;
	UINT			nIPAddr;
	unsigned short	nPort;
};

/*
 * c2s_requestsvrip
 */

/*
 * BYTE	cIPType
 */
#define INTRANER_IP	0
#define INTERNET_IP 1

struct tagRequestSvrIp : public tagProtoHeader
{
	WORD	pckgID;

	BYTE	cIPType;
	DWORD	dwMapID;
};

/*
 * c2c_notifyexchange
 */
struct tagSearchWay : public tagProtoHeader
{
	int		lnID;
	int		nIndex;
	DWORD	dwPlayerID;
};

/*
 * c2s_updatemapinfo
 */
struct tagUpdateMapID : public tagProtoHeader
{
	/*
	 * For example : Are your clear older information when it 
	 *		update local informatin
	 */
	BYTE cReserve;

	int cMapCount;

	BYTE szMapID[0];	// C4200 warning
};

/*
 * c2s_updategameserverinfo
 */
struct tagGameSvrInfo : public tagProtoHeader
{
	UINT			nIPAddr_Intraner;
	UINT			nIPAddr_Internet;

	unsigned short	nPort;
	WORD			wCapability;
};

/*
 * s2c_identitymapping
 */
struct tagIdentityMapping : public tagGameSvrInfo
{
	GUID guid;
};

/*
 * c2s_logiclogin
 * s2c_gmgateway2relaysvr
 * s2c_gmnotify
 */
struct tagLogicLogin : public tagProtoHeader
{
	GUID guid;
};

/*
 * s2c_logiclogout
 */
struct tagLogicLogout : public tagProtoHeader
{
	BYTE szRoleName[_NAME_LEN];
};

/*
 * c2s_registeraccount
 */
struct tagRegisterAccount : public tagProtoHeader
{
	BYTE szAccountName[_NAME_LEN];
};

/*
 * c2s_entergame
 */
struct tagEnterGame : public tagProtoHeader
{
	/*
	 * Succeeded : content is account name
	 * Failed	 : content is null
	 */

	BYTE szAccountName[_NAME_LEN];
};

struct tagEnterGame2 : public EXTEND_HEADER
{
	char szAccountName[_NAME_LEN];
	char szCharacterName[_NAME_LEN];
	DWORD	dwNameID;
	unsigned long	lnID;
};

/*
 * c2s_leavegame
 */

/*
 * BYTE cCmdType
 */
#define NORMAL_LEAVEGAME	0x0		// lock account
#define HOLDACC_LEAVEGAME	0x1A	// clear resource but don't to unlock account 

struct tagLeaveGame : public tagProtoHeader
{
	BYTE cCmdType;
	WORD nExtPoint;        //将要扣除的附送点
	/*
	 * Succeeded : content is account name
	 * Failed	 : content is null
	 */

	char szAccountName[_NAME_LEN];
};

struct tagLeaveGame2 : public EXTEND_HEADER
{
	BYTE cCmdType;
	char szAccountName[_NAME_LEN];
};
/*
*  c2s_registerfamily
*/
struct tagRegisterFamily : public tagProtoHeader
{
	BYTE bRegister;		//1 is Register, 0 is unRegister
	BYTE nFamily;
	BYTE RelayMethod;
};

/*
 * c2s_gmsvr2gateway_saverole
 */
struct tagGS2GWSaveRole : public tagProtoHeader
{
	size_t	datalength;
	BYTE	szData[0];
};

/*
 * #pragma pack( pop )
 *
 * } End of the struct define
 */

typedef struct
{
	char	szName[32];
	BYTE	Sex;
	BYTE	Series;
	int		Faction;
	int		Level;
} RoleBaseInfo/* client */, S3DBI_RoleBaseInfo /* server */;

typedef struct
{
	BYTE				ProtocolType;
	RoleBaseInfo		m_RoleList[MAX_PLAYER_IN_ACCOUNT];
} ROLE_LIST_SYNC;

//移自RoleDBManager/kroledbheader.h
//用来替换上面的ROLE_LIST_SYNC,ROLE_LIST_SYNC结构不再需要了
struct TProcessData
{
	unsigned char	nProtoId;
	size_t			nDataLen;//TRoleNetMsg时表示该Block的实际数据长度,TProcessData时表示Stream的实际数据长度
	unsigned long	ulIdentity;
	bool			bLeave;
	char			szName[32];
	char			pDataBuffer[1];//实际的数据
};

struct tagRoleEnterGame
{
	BYTE			ProtocolType;
	bool			bLock;
	char			Name[_NAME_LEN];
};

//新建角色的信息结构
//注释：新建决消息c2s_newplayer，传送的参数为TProcessData结构描述的数据，其中TProcessData::pDataBuffer要扩展为NEW_PLAYER_COMMAND
struct NEW_PLAYER_COMMAND
{
	BYTE			m_btRoleNo;			// 角色编号
	BYTE			m_btSeries;			// 五行系
	unsigned short	m_NativePlaceId;	//出生地ID
	char			m_szName[32];		// 姓名
};

typedef struct 
{
	BYTE			ProtocolType;
	BYTE			m_LogoutType;
} LOGOUT_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			szAccName[32];
} LOGIN_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
} TRADE_APPLY_OPEN_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
} TRADE_APPLY_CLOSE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btState;			// if == 0 close if == 1 open if == 2 trading
	DWORD			m_dwNpcID;			// 如果是开始交易，对方的 npc id
	BOOL			m_bFolkGame;
} TRADE_CHANGE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;
	DWORD			m_dwID;
	BYTE			m_btState;
	char			m_szSentence[MAX_SENTENCE_LENGTH];
} NPC_SET_MENU_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwID;
	BOOL			m_bFolkGame;
} TRADE_APPLY_START_COMMAND;

// 服务器转发交易申请
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nDestIdx;			// 申请者在服务器端的player idx
	DWORD			m_dwNpcId;			// 申请者的 npc id
	BOOL			m_bFolkGame;
} TRADE_APPLY_START_SYNC;

// 接受或拒绝别人的交易申请
typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_bDecision;		// 同意 1 不同意 0
	int				m_nDestIdx;			// 交易对方在服务器端的player idx
	BOOL			m_bFolkGame;
} TRADE_REPLY_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney;
} TRADE_MOVE_MONEY_COMMAND;		// c2s_trademovemoney

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nMoney;
} TRADE_MONEY_SYNC;				// s2c_trademoneysync

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDecision;		// 确定交易 1  退出交易 0  取消确定 4  锁定交易 2  取消锁定 3
	BYTE			m_btFolkGame;
} TRADE_DECISION_COMMAND;				// 交易执行或取消 c2s_tradedecision

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	BYTE			m_btDecision;		// 交易ok 1  交易取消 0  锁定 2  取消锁定 3
} TRADE_DECISION_SYNC;					// s2c_tradedecision

typedef struct
{
	BYTE			ProtocolType;		
	BYTE			m_byDir;			// 取钱的方向（0存，1取）
	DWORD			m_dwMoney;			// 钱数
} STORE_MONEY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwNpcID;
} TEAM_INVITE_ADD_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	WORD			m_wLength;			// 长度
	int				m_nIdx;
	char			m_szName[32];
} TEAM_INVITE_ADD_SYNC;

typedef struct
{
	BYTE			ProtocolType;		//
	int				m_nAuraSkill;
} SKILL_CHANGEAURASKILL_COMMAND;		//更换光环技能

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btResult;
	int				m_nIndex;
} TEAM_REPLY_INVITE_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BOOL			m_bSelfAskFolkGame;
	BOOL			m_bDestReplyFolkGame;
	BYTE			m_btSelfLock;
	BYTE			m_btDestLock;
	BYTE			m_btSelfOk;
	BYTE			m_btDestOk;
} TRADE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	DWORD			m_dwSkillID;		// 技能
	int				m_nLevel;
	int				m_nTime;			// 时间
	BOOL			m_bOverLook;
	KMagicAttrib	m_MagicAttrib[MAX_SKILL_STATE];
} STATE_EFFECT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	BOOL	bNegative;
} IGNORE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwTime;
} PING_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwReplyServerTime;
	DWORD			m_dwClientTime;
} PING_CLIENTREPLY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btSitFlag;
} NPC_SIT_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
} NPC_HORSE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	m_dwID;
	BOOL	m_bRideHorse;
}	NPC_HORSE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_dwRegionID;
	int				m_nObjID;
} OBJ_MOUSE_CLICK_SYNC;

//typedef struct tagSHOW_MSG_SYNC
//{
//	BYTE			ProtocolType;
//	WORD			m_wLength;
//	WORD			m_wMsgID;
//	LPVOID			m_lpBuf;
//	tagSHOW_MSG_SYNC() {m_lpBuf = NULL;};
//	~tagSHOW_MSG_SYNC() {Release();}
//	void	Release() {
//		if (m_lpBuf) 
//			m_lpBuf = NULL;
//	}
//} SHOW_MSG_SYNC;

typedef struct tagSHOW_MSG_SYNC {
	BYTE ProtocolType;
	WORD m_wLength;
	WORD m_wMsgID;
	std::unique_ptr<BYTE[]> *m_lpBuf;

	tagSHOW_MSG_SYNC() : ProtocolType(0), m_wLength(0), m_wMsgID(0), m_lpBuf(nullptr) {}

	~tagSHOW_MSG_SYNC() = default;

	void Release() {m_lpBuf->reset(); }

	void AllocateBuffer(std::size_t size) {m_lpBuf = &std::make_unique<BYTE[]>(size);}
} SHOW_MSG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btFlag;
} PK_APPLY_NORMAL_FLAG_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	BYTE			m_btFlag;
} PK_NORMAL_FLAG_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwNpcID;
	BOOL			m_bRefuse;
	BOOL			m_bSpar;
} PK_APPLY_ENMITY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	DWORD			m_dwNpcID;
	BOOL			m_bAim;
	BOOL			m_bSpar;
	char			m_szName[32];
} PK_ENMITY_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	BYTE			m_btState;
	DWORD			m_dwNpcID;
	char			m_szName[32];
} PK_EXERCISE_STATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	int				m_nPKValue;
} PK_VALUE_SYNC;

typedef struct
{
	int		m_nID;
	BYTE	m_btNature;
	BYTE	m_btGenre;			// 物品的类型
	int		m_btDetail;			// 物品的类别
	int		m_btParticur;		// 物品的详细类别
	BYTE	m_btSeries;			// 物品的五行
	BYTE	m_btLevel;			// 物品的等级
	BYTE	m_btLuck;			// MF
	int		m_btMagicLevel[MAX_ITEM_MAGICLEVEL];	// 生成参数
	WORD	m_wVersion;			// 装备版本
	DWORD	m_dwRandomSeed;		// 随机种子
} SViewItemInfo;

typedef struct
{
	BYTE			ProtocolType;
	DWORD			m_dwNpcID;
	BYTE			m_Avatar;
	SViewItemInfo	m_sInfo[itempart_num];
} VIEW_EQUIP_SYNC;				// s2c_viewequip

typedef struct//该结构是所统计的玩家的基本数据
{
	char	Name[20];
	int		nValue;
	BYTE	bySort;
}TRoleList;

// 游戏统计结构
typedef struct
{
	TRoleList MoneyStat[10];			//金钱最多排名列表（十个玩家，最多可达到100个）
	TRoleList LevelStat[10];			//级别最多排名列表（十个玩家，最多可达到100个）
	TRoleList KillerStat[10];			//杀人最多排名列表
	TRoleList ReputeStat[10];
	TRoleList FuYuanStat[10];
	TRoleList AccumStat1[10];
	TRoleList AccumStat2[10];
	TRoleList HonorStat[10];
	TRoleList TimeStat[10];
	TRoleList TongLvStat[10];
	TRoleList TongMnStat[10];
	TRoleList TongEffStat[10];
	//[门派号][玩家数]，其中[0]是没有加入门派的玩家
	TRoleList MoneyStatBySect[MAX_FACTION+1][10];	//各门派金钱最多排名列表
	TRoleList LevelStatBySect[MAX_FACTION+1][10];	//各门派级别最多排名列表

	//[门派号]，其中[0]是没有加入门派的玩家
	int SectPlayerNum[MAX_FACTION+1];				//各个门派的玩家数
	int SectMoneyMost[MAX_FACTION+1];				//财富排名前一百玩家中各门派所占比例数
	int SectLevelMost[MAX_FACTION+1];				//级别排名前一百玩家中各门派所占比例数
}  TGAME_STAT_DATA;

typedef struct
{
	BYTE	ProtocolType;
	BYTE	bSleep;
	DWORD	NpcID;
} NPC_SLEEP_SYNC;

//////////////
//排名相关
typedef struct
{
	BYTE		ProtocolType;
	DWORD		dwLadderID;
	TRoleList	StatData[10];
} LADDER_DATA;

typedef struct
{
	BYTE		ProtocolType;
	WORD		wSize;
	int			nCount;
	DWORD		dwLadderID[0];
} LADDER_LIST;

typedef struct
{
	BYTE		ProtocolType;
	DWORD		dwLadderID;
} LADDER_QUERY;


///////////////////
//chat 相关

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	packageID;
	char	someone[_NAME_LEN];
	BYTE	sentlen;
	char	item[MAX_SENTENCE_LENGTH];
} CHAT_SOMEONECHAT_CMD, CHAT_SOMEONECHAT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	packageID;
	BYTE	filter;
	DWORD	channelid;
	BYTE	cost;	//0: 无限制，1: 10元/句，2: <10Lv ? 不能说 : MaxMana/2/句, 3: MaxMana/10/句,4: <20Lv ? 不能说 : MaxMana*4/5/句
	BYTE	sentlen;
	char	item[MAX_SENTENCE_LENGTH];
	BYTE	someflag;
} CHAT_CHANNELCHAT_CMD;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	packageID;
	char	someone[_NAME_LEN];
	DWORD	channelid;
	BYTE	sentlen;
	char	item[MAX_SENTENCE_LENGTH];
	BYTE	someflag;
} CHAT_CHANNELCHAT_SYNC;

enum {codeSucc, codeFail, codeStore};
typedef struct
{
	BYTE	ProtocolType;
	DWORD	packageID;
	BYTE	code;
	char	item[MAX_SENTENCE_LENGTH];
	BYTE	someflag;
} CHAT_FEEDBACK;

typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	WORD	wChatLength;
} CHAT_EVERYONE;


typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	WORD	wChatLength;
	BYTE	byHasIdentify;
	WORD	wPlayerCount;
} CHAT_GROUPMAN;


typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	nameid;
	unsigned long lnID;
	WORD	wChatLength;
} CHAT_SPECMAN;


enum { tgtcls_team, tgtcls_fac, tgtcls_tong, tgtcls_msgr, tgtcls_cr, tgtcls_scrn, tgtcls_bc};
typedef struct
{
	BYTE	ProtocolType;
	WORD	wSize;
	DWORD	nFromIP;
	DWORD	nFromRelayID;
	DWORD	channelid;
	BYTE	TargetCls;
	DWORD	TargetID;
	WORD	routeDateLength;
} CHAT_RELEGATE;


///////////////////////////////////////
// tong 相关

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} S2C_TONG_HEAD;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} STONG_PROTOCOL_HEAD;

typedef struct
{
	int		m_nPlayerIdx;
	int		m_nCamp;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_APPLY_CREATE;

typedef struct
{
	int		m_nPlayerIdx;
	DWORD	m_dwNpcID;
} STONG_SERVER_TO_CORE_APPLY_ADD;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nTargetIdx;
	DWORD	m_dwNameID;
} STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION;

typedef struct
{
	int		m_nCamp;
	int		m_nPlayerIdx;
	DWORD	m_dwPlayerNameID;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CREATE_SUCCESS;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nTargetIdx;
	DWORD	m_dwNameID;
} STONG_SERVER_TO_CORE_REFUSE_ADD;

typedef struct
{
	int		m_nSelfIdx;
	int		m_nInfoID;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
	char	m_szName[32];
} STONG_SERVER_TO_CORE_GET_INFO;

typedef struct
{
	int		m_nPlayerIdx;
	DWORD	m_dwPlayerNameID;
	BYTE	m_btCamp;
	DWORD	m_dwMoney;
	BYTE	m_btLevel;
	DWORD	m_dwTotalEff;
	BOOL	m_bRecruit;
	int		m_nMemberNum;
	int		m_nTongParam;
	int 	m_nTongJiyuParam;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szMasterName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_ADD_SUCCESS;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_BE_INSTATED;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_BE_KICKED;

typedef struct
{
	int		m_nPlayerIdx;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_BE_CHANGED_AGNAME;

typedef struct
{
	DWORD	m_dwTongNameID;
	int		m_nMoney;
	int		m_nCamp;
} STONG_SERVER_TO_CORE_BE_CHANGED_CAMP;

typedef struct
{
	DWORD	m_dwTongNameID;
	BYTE 	m_btLevel;
} STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL;

typedef struct
{
	DWORD	m_dwTongNameID;
	int  	m_nMoney;
} STONG_SERVER_TO_CORE_BE_CHANGED_MONEY;

typedef struct
{
	DWORD	m_dwTongNameID;
	int  	m_nEff;
} STONG_SERVER_TO_CORE_BE_CHANGED_TONG_EFF;

typedef struct
{
	DWORD	m_dwTongNameID;
	BOOL 	m_bRecruit;
} STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT;

typedef struct
{
	DWORD	m_dwTongNameID;
	int 	m_nTongParam;
} STONG_SERVER_TO_CORE_BE_CHANGED_TONGPARAM;

typedef struct
{
	DWORD	m_dwTongNameID;
	int  	m_nMoney;
	int 	m_nTongJiyuParam;
	char	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} STONG_SERVER_TO_CORE_BE_CHANGED_JIYU;

typedef struct
{
	int		m_nPlayerIdx;
	int 	m_nTotalEff;
	DWORD	m_dwTotalEff;
	DWORD	m_dwTongNameID;
} STONG_SERVER_TO_CORE_BE_CHANGED_EFF;

typedef struct
{
	int		m_nPlayerIdx;
	BOOL	m_bSuccessFlag;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_LEAVE;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHECK_GET_AGNAME_POWER;

typedef struct
{
	int		m_nPlayerIdx;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	DWORD	m_dwTongNameID;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHANGE_AS;

typedef struct
{
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} STONG_SERVER_TO_CORE_CHANGE_MASTER;

typedef struct
{
	DWORD	m_dwTongNameID;
	DWORD	m_dwMoney;
	DWORD	m_nMoney;
	BYTE	nType;
	int		m_nPlayerIdx;
	int		m_nSelect;
	int 	m_nNumMember;
} STONG_SERVER_TO_CORE_MONEY;

typedef struct
{
	DWORD	m_dwTongNameID;
	int 	m_nValue;
	int 	m_nValueSync;
	BYTE	nType;
	int		m_nPlayerIdx;
	int		m_nSelect;
} STONG_SERVER_TO_CORE_EFF;

typedef struct
{
	DWORD	m_dwParam;
	int		m_nFlag;
	int		m_nCamp;
	BYTE	m_btLevel;
	int		m_nFigure;
	DWORD	m_dwMemberNum;
	BYTE	m_btManagerNum;
	BYTE	m_btDirectorNum;
	int		m_nPos;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szMaster[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
	int		m_nJoinTm;
	DWORD	m_nMoney;
	DWORD	m_dwTotalEff;
	int 	m_nSaveEff;
	BOOL 	m_bRecruit;
	int		m_nTongParam;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} STONG_SERVER_TO_CORE_LOGIN;

// 玩家申请建立帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
	BYTE 	m_btSex;
	BYTE 	m_btLevel;
	char	m_szName[defTONG_NAME_LENGTH_32 + 1];
} TONG_APPLY_CREATE_COMMAND;

// 玩家申请加入帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwNpcID;
} TONG_APPLY_ADD_COMMAND;

// 玩家申请加入帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btCurFigure;
	BYTE	m_btCurPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_INSTATE_COMMAND;

// 帮会建立失败 扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btFailId;
} TONG_CREATE_FAIL_SYNC;

// 转发加入帮会申请 扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nPlayerIdx;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_ADD_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	char	m_cTongName[defTONG_NAME_LENGTH_32];
} JOIN_TONG_SYNC;

// 通知玩家建立帮会成功 用普通协议
typedef struct
{
	BYTE	ProtocolType;
	BYTE	m_btCamp;
	char	m_szName[defTONG_NAME_LENGTH_32 + 1];
} TONG_CREATE_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwID;
} TONG_JOIN_REPLY;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	char			m_cTongName[defTONG_NAME_LENGTH_32];
} S2C_JOIN_TONG;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_Index;
	DWORD			m_dwID;
} S2C_TONG_JOIN_REPLY;

// 通知玩家加入帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szMaster[defTONG_NAME_LENGTH_32];
} TONG_ADD_SYNC;

// 玩家申请解散帮会 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
} TONG_APPLY_DISMISS_COMMAND;

// 玩家帮会被解散 用普通协议
typedef struct
{
	BYTE	ProtocolType;
} TONG_DISMISS_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nPlayerIdx;
	DWORD	m_dwNameID;
	BYTE	m_btFlag;			// 是否接受 TRUE 接受 FALSE 不接受
} TONG_ACCEPT_MEMBER_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btInfoID;
	int		m_nParam1;
	int		m_nParam2;
	int		m_nParam3;
	char	m_szBuf[64];
} TONG_APPLY_INFO_COMMAND;

typedef struct
{
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
	int 	m_nMasterJoinTm;
	int 	m_nDirectorJoinTm[defTONG_MAX_DIRECTOR];
	int 	m_nManagerJoinTm[defTONG_MAX_MANAGER];
	BOOL	m_bOnline;
	int		m_nMasterEff;
	int		m_nDirectorEff[defTONG_MAX_DIRECTOR];
	int 	m_nManagerEff[defTONG_MAX_MANAGER];
} TONG_ONE_LEADER_INFO;

typedef struct
{
	char	m_szName[defTONG_NAME_LENGTH_32];
	int 	m_nMemberJoinTm;
	BOOL	m_bOnline;
	BYTE	m_btSex;
	int 	m_nMemberEff;
} TONG_ONE_MEMBER_INFO;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwNpcID;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	DWORD	m_dwTotalEff;
	int 	m_nSaveEff;
	BOOL	m_bRecruit;
	int 	m_nTongParam;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	TONG_ONE_LEADER_INFO	m_sMember[1 + defTONG_MAX_DIRECTOR];
} TONG_HEAD_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	BYTE	m_btStateNo;
	BYTE	m_btCurNum;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	TONG_ONE_LEADER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
} TONG_MANAGER_INFO_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwMoney;
	int		m_nCredit;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	BYTE	m_btStateNo;
	BYTE	m_btCurNum;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szMaleAgname[defTONG_NAME_LENGTH_32];
	char	m_szFemaleAgname[defTONG_NAME_LENGTH_32];
	TONG_ONE_MEMBER_INFO	m_sMember[defTONG_ONE_PAGE_MAX_NUM];
} TONG_MEMBER_INFO_SYNC;

// 玩家自身在帮会中的信息 用扩展协议
typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btJoinFlag;
	BYTE	m_btFigure;
	BYTE	m_btCamp;
	BYTE	m_btLevel;
	BYTE	m_btDirectorNum;
	BYTE	m_btManagerNum;
	DWORD	m_dwMemberNum;
	char	m_szTongName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szMasterName[defTONG_NAME_LENGTH_32];
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
	DWORD	m_dwMoney;
	DWORD	m_dwTotalEff;
	BOOL	m_bRecruit;
	int		m_nTongParam;
} TONG_SELF_INFO_SYNC;


typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btOldFigure;
	BYTE	m_btOldPos;
	BYTE	m_btNewFigure;
	BYTE	m_btNewPos;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_INSTATE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_KICK_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btSuccessFlag;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_KICK_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_LEAVE_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_CHANGE_MASTER_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btPos;
	char	m_szName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} TONG_APPLY_CHANGE_AGNAME_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	int		m_btSex;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFigure;
	BYTE	m_btCamp;
	int 	m_nMoney;
} TONG_APPLY_CHANGE_CAMP_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nExtPoint;
} APPLY_GET_EXTPOINT_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nParam;
} APPLY_GET_PARAM_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_MASTER_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_AGNAME_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_btFailID;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_CAMP_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btCamp;
} TONG_CHANGE_CAMP_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_btLevel;
} TONG_CHANGE_LEVEL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	BYTE	m_bRecruit;
} TONG_CHANGE_RECRUIT_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nTongParam;
} TONG_CHANGE_TONGPARAM_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	BYTE	m_bRecruit;
} TONG_CHANGE_RECRUIT_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	int		m_nTongParam;
} TONG_CHANGE_TONGPARAM_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} TONG_CHANGE_JIYU_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	int 	m_nSaveEff;
	int 	m_dwTotalEff;
} TONG_CHANGE_EFF_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
	int		m_nMoney;
	int		m_nTongJiyuParam;
	char 	m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
} TONG_APPLY_CHANGE_INFO_COMMAND;

// tong 相关 end

typedef struct
{
	BYTE	ProtocolType;
	DWORD	dwItemID;
	int		nChange;
} ITEM_DURABILITY_CHANGE;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	int		Rank;
}	TITLE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	ID;
	KExpandRank		ExpandRank;
}	EXPANDTITLE_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	dwItemID;
	BYTE	m_Number;
} ITEM_REPAIR;

typedef struct
{
	BYTE	ProtocolType;
	WORD	nTaskId;
	char	szTaskValue[16];
}S2C_SYNCTASKVALUE;

typedef struct tagS2C_PLAYER_SYNC
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBuf;
	tagS2C_PLAYER_SYNC() {m_lpBuf = NULL;};
	~tagS2C_PLAYER_SYNC() {Release();}
	void	Release() {if (m_lpBuf) delete []m_lpBuf; m_lpBuf = NULL;}
} S2C_PLAYER_SYNC;

typedef struct tagPLAYER_COMMAND
{
	BYTE			ProtocolType;
	WORD			m_wLength;
	WORD			m_wMsgID;
	LPVOID			m_lpBuf;
	tagPLAYER_COMMAND() {m_lpBuf = NULL;};
	~tagPLAYER_COMMAND() {Release();}
	void	Release() {if (m_lpBuf) delete []m_lpBuf; m_lpBuf = NULL;}
} PLAYER_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	char	szBuf[32];
} C2S_BUF_COMMAND;

typedef struct
{
	BYTE			ProtocolType;	
	int				nExtPoint;	
	int				nChangeExtPoint;	
} S2C_EXTPOINT;

typedef struct
{	
	BYTE	ProtocolType;
	char	m_szName[FILE_NAME_LENGTH];
	char	m_szInitString[MAX_MESSAGE_LENGTH];
}PLAYER_GIVE;

typedef struct
{
	BYTE	ProtocolType;
	DWORD	dwItemID;
	int		nNum;
	BOOL	bIsBreakAll;
}PLAYER_BREAK_COMMAND;

typedef struct
{
	int		m_nID;				// 物品的ID
	BYTE	m_btNature;
	BYTE	m_btGenre;			// 物品的类型
	int		m_btDetail;			// 物品的类别
	int		m_btParticur;		// 物品的详细类别
	BYTE	m_btSeries;			// 物品的五行
	BYTE	m_btLevel;			// 物品的等级
	BYTE	m_btLuck;			// MF
	int		m_btMagicLevel[MAX_ITEM_MAGICLEVEL];	// 生成参数
	WORD	m_wVersion;			// 装备版本
	DWORD	m_dwRandomSeed;		// 随机种子
	int		m_nIdx;
	UINT	m_uPrice;
	int		m_bX;
	int		m_bY;
	unsigned short 	m_nStackNum;
	int		m_ExpireTime;
	int		m_nParam;
	int		m_nFortune;
} SViewSellItemInfo;

typedef struct
{
	BYTE			ProtocolType;
	SViewSellItemInfo	m_sInfo;
} PLAYER_TRADE_ITEM_SYNC;

typedef struct
{
	BYTE			ProtocolType;
	LockMoveItem	m_LockMove;
}PLAYER_LOCKMOVE;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			dwID;
	int				nParam;
}PLAYER_TRADE;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			dwID;
	BOOL			bOpen;
}PLAYER_TRADE_ACTION_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nIdx;			// 买第几个东西
	DWORD			m_dwID;
	BYTE			m_Place;			// 放在身上哪个地方
	BYTE			m_X;				// 坐标X
	BYTE			m_Y;				// 坐标Y
} PLAYER_TRADE_BUY_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwID;				// 物品的ID
	int				m_nPrice;
}PLAYER_TRADE_SET_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	char			Name[32];
}PLAYER_TRADE_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
}PLAYER_TRADE_START_FAIL_SYNC;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	char	m_szName[defTONG_NAME_LENGTH_32];
	char	m_szAgname[defTONG_NAME_LENGTH_32];
} TONG_CHANGE_AGNAME_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	int		m_nEff;
	int 	m_nSelect;
	char	m_szName[defTONG_NAME_LENGTH_32];
	BYTE	m_btFigure;
} TONG_APPLY_SAVE_EFF_COMMAND;

typedef struct
{
	BYTE	ProtocolType;
	WORD	m_wLength;
	BYTE	m_btMsgId;
	DWORD	m_dwTongNameID;
	DWORD	m_dwMoney;
	char	m_szName[defTONG_NAME_LENGTH_32];
} TONG_APPLY_SAVE_COMMAND;

typedef struct
{
	DWORD	m_dwTongNameID;
	char	m_szAgname[defTONG_NAME_LENGTH_32];
	char	m_szName[defTONG_NAME_LENGTH_32];
	int		m_nPlayerIdx;
} STONG_SERVER_TO_CORE_AGNAME;

typedef struct
{
	BYTE	ProtocolType;
	char	MissionName[64];
	TMissionLadderSelfInfo SelfData;
	TMissionLadderInfo MissionRank[MISSION_STATNUM];
} PLAYER_MISSION_RANKDATA;

typedef struct
{
	BYTE	ProtocolType;
	int			m_nSaleType;
	BuySellInfo	m_BuySellInfo;
} S2C_SUPERSHOP;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	DWORD			m_dwID;
} SPAR_APPLY_START_COMMAND;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nDestIdx;			// 申请者在服务器端的player idx
	DWORD			m_dwNpcId;			// 申请者的 npc id
} SPAR_APPLY_START_SYNC;

typedef struct
{
	BYTE			ProtocolType;		// 协议类型
	int				m_nDecision;
	BOOL			m_bRoomList;
} PLAYER_CHATROOM_DECISION_COMMAND;

typedef struct
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btErrorID;		// 队伍创建不成功原因：0 同名 1 玩家本身已经属于某一支队伍 3 当前处于不能组队状态
} PLAYER_SEND_CREATE_CHATROOM_FALSE;// 服务器通知客户端队伍创建不成功

struct PLAYER_SEND_CHATROOM_STATE
{
	BYTE	ProtocolType;		// 协议名称
	BYTE	m_btMsgId;
	int		nId;
	char	lpszName[16];
	PLAYER_SEND_CHATROOM_STATE() {lpszName[0]=0;}
};	// 服务器通知玩家队伍创建成功

struct PLAYER_REQUEST_CHATROOM_LIST
{
	BYTE	ProtocolType;		// 协议名称
	char	lpszRoomHost[16];
	char	lpszRoomName[10];
	BYTE	byRoomMemNum;
	int		nRoomLeftTime;
	int		nRoomParam;
	PLAYER_REQUEST_CHATROOM_LIST() {nRoomParam=0;}
};	// 服务器通知玩家队伍创建成功

struct PLAYER_REQUEST_LIST
{
	BYTE	ProtocolType;		// 协议名称
	char	lpszName[_NAME_LEN];
	int		nMemIndex;
	int		nMemParam;
	BOOL	bIsRoomHost;
	PLAYER_REQUEST_LIST() {nMemParam=0;}
};	// 服务器通知玩家队伍创建成功

typedef struct	
{
	BYTE	ProtocolType;
	int		int_ID;
} CP_DATAU;

// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdSell(int nID, int nNumber);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdBuy(int nShop, int nBuyIdx, BYTE nNumber);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdRun(int nX, int nY);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdWalk(int nX, int nY);
// 在调用这支函数之前必须判断是否处于交易状态，如果正在交易，不能调用这支函数
void SendClientCmdSkill(int nSkillID, int nX, int nY);
void SendClientCmdSit(bool bFlag);
void SendClientCmdMoveItem(void* pDownPos, void* pUpPos);
void SendClientCmdQueryLadder(DWORD	dwLadderID);
void SendClientCmdRequestNpc(int nID);
void SendClientCmdStoreMoney(int nDir, int nMoney);
void SendClientCmdRevive();
void SendObjMouseClick(int nObjID, DWORD dwRegionID);
void SendClientCmdRepair(DWORD dwID);
void SendClientCmdRide();
void SendClientCmdBreak(DWORD dwID, int nNum, BOOL bIsBreakAll);
void SendClientCPSetImageCmd(int ID);
extern	int	g_nProtocolSize[MAX_PROTOCOL_NUM];
#pragma pack(pop, enter_protocol)
#endif
