/*******************Editer	: duccom0123 EditTime:	2024/06/12 11:48:44*********************
//	外界访问Core用到数据结构等的定义
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
	一些定义可能处于在游戏世界各模块的头文件中，请在此处包含那个头文件，并请那样的头文件
不要包含一些与游戏世界对外无关的内容。
    开发过程中游戏世界的外部客户在未获得游戏世界接口完整定义的情况下，会现先直接在此文件
定义它需要的数据定义，游戏世界各模块可根据自身需要与设计把定义作修改或移动到模块内的对外
头文件，并在此建立充要的包含。
*****************************************************************************************/
#ifndef GAMEDATADEF_H
#define GAMEDATADEF_H

#include "CoreObjGenreDef.h"
#include "CoreUseNameDef.h"

#define ITEM_VERSION 1

#define _CHAT_SCRIPT_OPEN

#ifdef _SERVER
#define MAX_NPC 48000
#define MAX_PLAYER 1200   // Max nguoi choi
#define MAX_ITEM 200000
#else
#define MAX_NPC 256
#define MAX_PLAYER 2
#define MAX_ITEM 512
#endif

#define MAX_TEAM_MEMBER 7
#define MAX_QUESTION_LENGTH 2056
#define MAX_SENTENCE_LENGTH 256
#define MAX_MESSAGE_LENGTH 512
#define MAX_ANSWERNUM 20

#define NORMAL_FONTSIZE 12
#define FILE_NAME_LENGTH 80
#define OBJ_NAME_LENGTH 40
#define PLAYER_PICKUP_CLIENT_DISTANCE 63
#define defMAX_EXEC_OBJ_SCRIPT_DISTANCE 200
#define defMAX_PLAYER_SEND_MOVE_FRAME 5
#define PLAYER_PICKUP_SERVER_DISTANCE 40000

#define MAX_PERCENT 100
#define MAX_INT 0x7fffffff
#define MONEY_FLOOR 10000

#define ROLE_NO 2
#define PLAYER_MALE_NPCTEMPLATEID -1
#define PLAYER_FEMALE_NPCTEMPLATEID -2

#define PLAYER_SHARE_EXP_DISTANCE 768

#define MAX_DEATH_PUNISH_PK_VALUE 10

// tạm thời chuyển về 800x600
#define RESOLUTION_WIDTH 800
#define RESOLUTION_HEIGHT 600

#define POISON_DAMAGE_TIME 60
#define POISON_DAMAGE_INTERVAL 10
#define COLD_DAMAGE_TIME 60
#define MAX_DEATLY_STRIKE_ENHANCEP 200
#define MIN_FATALLY_STRIKE_ENHANCEP 30
#define MAX_FATALLY_STRIKE_ENHANCEP 50

#define MAX_LEVEL 200
#define MAX_SERIES 5
#define MAX_LEAD_LEVEL 100

#define MAX_TONG_COUNT 50
#define MAX_TONG_LEVEL 10
#define MAX_TONG_NATIONALEMBLEM 6

#define MAX_COMPOUND_ITEM 3

#define OBJ_NAME_LENGHT 80   // TamLTM Kham nam xanh
#define MAX_PART_BUILD 9     //TamLTM Kham nam xanh // Max so part item kham nam TamLTM

#define REGION_PIXEL_WIDTH 512
#define REGION_PIXEL_HEIGHT 1024
#define REGION_CELL_SIZE_X 32
#define REGION_CELL_SIZE_Y 32
#define REGION_CELL_WIDTH (REGION_PIXEL_WIDTH / REGION_CELL_SIZE_X)
#define REGION_CELL_HEIGHT (REGION_PIXEL_HEIGHT / REGION_CELL_SIZE_Y)

#define GAME_FPS 18
#define GAME_UPDATE_TIME 10
#define GAME_SYNC_LOSS 100

#define MAX_TASK 255
#define MAX_TEMP_TASK 100

#define MAX_SCIRPTACTION_BUFFERNUM 2056

#define TRADEFOLKGAME_NEED_WIDTH 6
#define TRADEFOLKGAME_NEED_HEIGHT 5

#define MIN_ITEM_LEVEL 0
#define MAX_ITEM_LEVEL 10
#define MAX_ITEM_GENERATORLEVEL 10
#define MAX_ITEM_LUCK 10
#define MAX_ITEM_MAGICATTRIB 8
#define MAX_ITEM_MAGICLEVEL MAX_ITEM_MAGICATTRIB * 2
#define MAX_ITEM_NORMAL_MAGICATTRIB 6
#define MAX_ITEM_S3DBINTERFACE1 15
#define MAX_ITEM_S3DBINTERFACE2 20

#define FACTIONS_PRR_SERIES 2
#define HUASHANID 10
#define MAX_FACTION FACTIONS_PRR_SERIES *series_num + 1

#define MIN_PASSWORD_LENGTH 0
#define MAX_PASSWORD_LENGTH 16

enum LOCK_STATE
{
    LOCK_STATE_CHARACTER = -3,
    LOCK_STATE_FOREVER = -2,
    LOCK_STATE_LOCK,
    LOCK_STATE_NORMAL,
    LOCK_STATE_UNLOCK,
};

#define MAX_STATTASK 10
#define MAX_TRANSLIFE_VALUE 10
#define MAX_VIPRANK_VALUE 10

enum TASKVALUE_STATTASK
{
    TASKVALUE_STATTASK_BEGIN = 210,
    TASKVALUE_STATTASK_REPUTE = 210,
    TASKVALUE_STATTASK_FUYUAN,
    TASKVALUE_STATTASK_ACCUM,
    TASKVALUE_STATTASK_ACCUMSTAT1,
    TASKVALUE_STATTASK_ACCUMSTAT2,
    TASKVALUE_STATTASK_HONOR,
    TASKVALUE_STATTASK_HONORSTAT,
    TASKVALUE_STATTASK_RESPECT,
    TASKVALUE_STATTASK_TIMESTAT,
    TASKVALUE_STATTASK_VIPRANK,
    TASKVALUE_STATTASK_NUM
};

enum MAP_TYPE
{
    MAPID_UNKNOWN,
    MAPID_CITY,
    MAPID_CAPITAL,
    MAPID_CAVE,
    MAPID_BATTLEFIELD,
    MAPID_FIELD,
    MAPID_OTHERS,
    MAPID_COUNTRY,
    MAPID_TONG,
    MAPID_NUM,
};

enum ITEM_PART
{
    itempart_head = 0,
    itempart_body,
    itempart_belt,
    itempart_weapon,
    itempart_foot,
    itempart_cuff,
    itempart_amulet,
    itempart_ring1,
    itempart_ring2,
    itempart_pendant,
    itempart_horse,
    itempart_mask,
    itempart_mantle,
    itempart_signet,
    itempart_shipin,
    itempart_hoods,
    itempart_cloak,
    itempart_num,
};

//TamLTM kham nam code
enum BUILD_PART
{
    itempart_build1 = 0,
    itempart_build2,
    itempart_build3,
    itempart_build4,
    itempart_build5,
    itempart_build6,
    itempart_build7,
    itempart_build8,
    itempart_build9,
    buildpart_num,
};
//End code

enum COMPOUND_ACTION
{
    CPA_FUSION,
    CPA_CRYOLITE,
    CPA_PROPMINE,
    CPA_NUM,
};

enum ENCHASE_ACTION   //Do Tim
{
    ECA_DISTILL,
    ECA_FORGE,
    ECA_ENCHASE,
    ECA_ATLAS,
};

enum COMPOUND_PART   //Box Kham do tim
{
    compound_box1 = 0,
    compound_box2,
    compound_box3,
    compound_num,
};

//TamLTM kham nam xanh
enum ITEMKIND
{
    gold_item = 0,
    purple_item,
    normal_item,
    green_item,
    kind_number,
};

//End code

enum ITEMGENRE
{
    item_equip = 0,
    item_medicine,
    item_event,
    item_materials,
    item_task,
    item_townportal,
    item_magicscript,
    item_number,
    item_mine,   //TamLTM Kham nam xanh
};

enum EQUIPNATURE
{
    NATURE_NORMAL = 0,
    NATURE_VIOLET,
    NATURE_GOLD,
    NATURE_PLATINA,
    NATURE_NUM,
};

enum EQUIPLEVEL
{
    equip_normal = 0,
    equip_magic,
    equip_damage,
    equip_violet,
    equip_gold,
    equip_platina,
    equip_number,
};

enum EQUIPDETAILTYPE
{
    equip_meleeweapon = 0,
    equip_rangeweapon,
    equip_armor,
    equip_ring,
    equip_amulet,
    equip_boots,
    equip_belt,
    equip_helm,
    equip_cuff,
    equip_pendant,
    equip_horse,
    equip_mask,
    equip_mantle,
    equip_signet,
    equip_shipin,
    equip_hoods,
    equip_cloak,
    equip_detailnum,
};

enum MEDICINEDETAILTYPE
{
    medicine_blood = 0,
    medicine_mana,
    medicine_both,
    medicine_stamina,
    medicine_antipoison,
    medicine_detailnum,
};

//TamLTM code kham nam xanh
enum SPECIALITEM_TASK
{
    SP_TUTHUYTINH = 239,
    SP_LUCTHUYTINH = 240,
    SP_LAMTHUYTINH = 238,
};
//End code

enum TOWNPORTALDETAILTYPE
{
    townportal_normal,
    townportal_infinite,
    townportal_special,
};

typedef struct
{
    BOOL bIsSkill;
    int nIdx;
    int nPlace;
    int nX;
    int nY;
    void Release()
    {
        bIsSkill = FALSE;
        nIdx = 0;
        nPlace = 0;
        nX = 0;
        nY = 0;
    };
} PlayerItem;

enum INVENTORY_ROOM
{
    room_equipment = 0,
    room_repository,
    room_repository1,
    room_repository2,
    room_repository3,
    room_repository4,
    room_repository5,
    room_equipmentex,
    room_trade,
    room_tradeback,
    room_trade1,
    room_immediacy,
    room_give,
    room_builditem,   // TamLTM Kham nam
    room_compound,
    room_num,
};

enum ITEM_POSITION
{
    pos_hand = 1,
    pos_equip,
    pos_equiproom,
    pos_equiproomex,
    pos_repositoryroom,
    pos_repositoryroom1,
    pos_repositoryroom2,
    pos_repositoryroom3,
    pos_repositoryroom4,
    pos_repositoryroom5,
    pos_traderoom,
    pos_trade1,
    pos_immediacy,
    pos_give,
    pos_compound,
    pos_builditem,   //TamLTM Kham nam xanh
    pos_compoundroom,
    pos_num,
};

enum EXPAND_BOX
{
    REPOSITORY_ONE,
    REPOSITORY_TWO,
    REPOSITORY_THREE,
    REPOSITORY_FOUR,
    REPOSITORY_FIVE,
    REPOSITORY_NUM,
};

#define MAX_HAND_ITEM 1
#define EQUIPMENT_ROOM_WIDTH 6
#define EQUIPMENT_ROOM_HEIGHT 10
#define MAX_EQUIPMENT_ITEM (EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)
#define REPOSITORY_ROOM_WIDTH 6
#define REPOSITORY_ROOM_HEIGHT 10
#define MAX_REPOSITORY_ITEM ((REPOSITORY_ROOM_WIDTH * REPOSITORY_ROOM_HEIGHT) * (1 + REPOSITORY_NUM))
#define TRADE_ROOM_WIDTH 10
#define TRADE_ROOM_HEIGHT 4
#define MAX_TRADE_ITEM (TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
#define MAX_TRADE1_ITEM MAX_TRADE_ITEM
#define GIVE_ROOM_WIDTH 6
#define GIVE_ROOM_HEIGHT 4
#define MAX_GIVE_ITEM (GIVE_ROOM_WIDTH * GIVE_ROOM_HEIGHT)
#define MOSAICENCRUSTED_ROOM_WIDTH 2
#define MOSAICENCRUSTED_ROOM_HEIGHT 4
#define MAX_MOSAICENCRUSTED_ITEM (MOSAICENCRUSTED_ROOM_WIDTH * MOSAICENCRUSTED_ROOM_HEIGHT)
#define IMMEDIACY_ROOM_WIDTH 9
#define IMMEDIACY_ROOM_HEIGHT 1
#define MAX_IMMEDIACY_ITEM (IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
#define MAX_PLAYER_ITEM_RESERVED 32
#define MAX_PLAYER_ITEM (MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM + MAX_TRADE_ITEM + MAX_TRADE1_ITEM + MAX_IMMEDIACY_ITEM + itempart_num + MAX_HAND_ITEM + MAX_PLAYER_ITEM_RESERVED)
//#define		MAX_PLAYER_ITEM		420
#define REMOTE_REVIVE_TYPE 0
#define LOCAL_REVIVE_TYPE 1

#define MAX_MELEE_WEAPON 7
#define HAND_PARTICULAR 6
#define MAX_RANGE_WEAPON 3
#define MAX_WEAPON MAX_MELEE_WEAPON + MAX_RANGE_WEAPON

#define MAX_NPC_TYPE 300

#define MAX_NPC_DIR 64
#define MAX_NPCSKILL 80
#define MAX_NPCPARAM 4
#define MAX_NPC_HEIGHT 128

#define MAX_SKILL_STATE 18

#define MAX_RESIST 95
#define RESIST_PLUS_SCALE 10

#define MIN_HIT_PERCENT 5
#define MAX_HIT_PERCENT 95

#define LEVEL_EXPLOSIVE 120
#define LIFE_EXPLOSIVE 25

#define PLAYER_MOVE_DO_NOT_MANAGE_DISTANCE 5

#define NORMAL_NPC_PART_NO 5

#ifndef _SERVER
#define C_REGION_X(x) (LOWORD(SubWorld[0].m_Region[(x)].m_RegionID))
#define C_REGION_Y(y) (HIWORD(SubWorld[0].m_Region[(y)].m_RegionID))
#endif

enum PLAYER_INSTANT_STATE
{
    enumINSTANT_STATE_LEVELUP = 0,
    enumINSTANT_STATE_REVIVE,
    enumINSTANT_STATE_CREATE_TEAM,
    enumINSTANT_STATE_LOGIN,
    enumINSTANT_STATE_NUM,
};

enum OBJ_ATTRIBYTE_TYPE
{
    series_metal,   //	金系
    series_wood,    //	木系
    series_water,   //	水系
    series_fire,    //	火系
    series_earth,   //	土系
    series_nil,
    series_num = series_nil,
    series_minus,
};

enum OBJ_GENDER
{
    OBJ_G_MALE = 0,   //雄性，男的
    OBJ_G_FEMALE,     //雌的，女的
};

enum NPCCAMP
{
    camp_begin,
    camp_justice,
    camp_evil,
    camp_balance,
    camp_free,
    camp_animal,
    camp_event,
    camp_audience,
    camp_tongwar,
    camp_num,
};

enum NPCBOSS
{
    npc_normal,
    npc_blue,      // 新手阵营（加入门派前的玩家）
    npc_gold,      // 正派阵营
    npc_pink,      // 正派阵营
    npcboss_num,   // 阵营数
};

enum ITEM_IN_ENVIRO_PROP
{
    IIEP_NORMAL = 0,    //一般/正常/可用
    IIEP_NOT_USEABLE,   //不可用/不可装配
    IIEP_SPECIAL,       //特定的不同情况
};

#define GOD_MAX_OBJ_TITLE_LEN 2048   //128临时改为1024为了兼容旧代码 to be modified
#define GOD_MAX_OBJ_PROP_LEN 1024
#define GOD_MAX_OBJ_DESC_LEN 1024

//==================================
//	游戏对象的描述
//==================================
struct KGameObjDesc
{
    char szTitle[GOD_MAX_OBJ_TITLE_LEN];   //标题，名称
    char szProp[GOD_MAX_OBJ_PROP_LEN];     //属性，每行可以tab划分为靠左与靠右对齐两部分
    char szDesc[GOD_MAX_OBJ_DESC_LEN];     //描述
};

//==================================
//	问题与可选答案
//==================================
struct KUiAnswer
{
    char AnswerText[MAX_SENTENCE_LENGTH];   //可选答案文字（可以包含控制符）
    int AnswerLen;                          //可选答案存储长度（包括控制符，不包含结束符）
};

struct KUiQuestionAndAnswer
{
    int LeftTime;
    char Question[MAX_QUESTION_LENGTH];   //问题文字（可以包含控制符）
    int QuestionLen;                      //问题文字存储长度（包括控制符，不包含结束符）
    int AnswerCount;                      //可选答案的数目
    KUiAnswer Answer[1];                  //候选答案
};

struct KUiImage
{
    char Name[MAX_PATH];
    int Frame;
};

//==================================
//	简略表示游戏对象的结构
//==================================
struct KUiGameObject
{
    unsigned int uGenre;   //对象类属
    unsigned int uId;      //对象id
    //	int			 nData;		//与对象实例相关的某数据
};

//==================================
//	以坐标表示的一个区域范围
//==================================
struct KUiRegion
{
    int h;        //左上角起点横坐标
    int v;        //左上角起点纵坐标
    int Width;    //区域横宽
    int Height;   //区域纵宽
};

//==================================
//	可以游戏对象容纳的地方
//==================================
enum UIOBJECT_CONTAINER
{
    UOC_IN_HAND = 1,      //手中拿着
    UOC_GAMESPACE,        //游戏窗口
    UOC_IMMEDIA_ITEM,     //快捷物品
    UOC_IMMEDIA_SKILL,    //快捷武功0->右键武功，1,2...-> F1,F2...快捷武功
    UOC_ITEM_TAKE_WITH,   //随身携带
    UOC_ITEM_TAKE_WITH_EX,
    UOC_TO_BE_TRADE,         //要被买卖，买卖面板上
    UOC_OTHER_TO_BE_TRADE,   //买卖面板上，别人要卖给自己的，
    UOC_EQUIPTMENT,          //身上装备
    UOC_NPC_SHOP,            //npc买卖场所
    UOC_STORE_BOX,
    UOC_STORE_BOX1,   //储物箱
    UOC_STORE_BOX2,
    UOC_STORE_BOX3,
    UOC_STORE_BOX4,
    UOC_STORE_BOX5,
    UOC_SKILL_LIST,   //列出全部拥有技能的窗口，技能窗口
    UOC_SKILL_TREE,
    UOC_ITEM_GIVE,
    UOC_BUILD_ITEM,   //TamLTM kham nam xanh
    UOC_COMPOUND,
    UOC_COMPOUND_BOX,
    //左、右可用技能树
};

enum MONEYUNIT
{
    moneyunit_money = 0,   // 头
    moneyunit_extpoint,    // 身体
    moneyunit_fuyuan,      // 腰带
    moneyunit_repute,      // 武器
    moneyunit_accum,
    moneyunit_honor,
    moneyunit_respect,
    moneyunit_num,
};
//==================================
// iCoreShell::GetGameData函数调用,uDataId取值为GDI_TRADE_DATA时，
// uParam的许可取值列表
// 注释中的Return:行表示相关的GetGameData调用的返回值的含义
//==================================
enum UI_TRADE_OPER_DATA
{
    UTOD_IS_WILLING,   //是否交易意向(叫卖中)
    //Return: 返回自己是否处于叫卖中的布尔值
    UTOD_IS_LOCKED,   //自己是否处于已锁定状态
    //Return: 返回自己是否处于已锁定状态的布尔值
    UTOD_IS_TRADING,   //是否可以正在等待交易操作（交易是否已确定）
    //Return: 返回是否正在等待交易操作（交易是否已确定）
    UTOD_IS_OTHER_LOCKED,   //对方是否已经处于锁定状态
    //Return: 返回对方是否已经处于锁定状态的布尔值
    UTOD_IS_OTHER_TRADING,
};

//==================================
//	买卖物品
//==================================
struct KUiItemBuySelInfo
{
    int nItemNature;
    char szItemName[64];   //物品名称
    int nOldPrice;
    int nCurPrice;   //买卖价钱，正值为卖价格，负值表示买入的价格为(-nPrice)
    int nMoneyUnit;
    bool bNewArrival;
};

//==================================
//	表示某个游戏对象在坐标区域范围的信息结构
//==================================
struct KUiObjAtRegion
{
    KUiGameObject Obj;
    KUiRegion Region;
};

struct KUiObjAtContRegion : public KUiObjAtRegion
{
    union
    {
        UIOBJECT_CONTAINER eContainer;
        int nContainer;
    };
};

struct KUiMsgParam
{
    unsigned char eGenre;   //取值范围为枚举类型MSG_GENRE_LIST,见MsgGenreDef.h文件
    unsigned char cChatPrefixLen;
    unsigned short nMsgLength;
    char szName[32];
#define CHAT_MSG_PREFIX_MAX_LEN 16
    unsigned char cChatPrefix[CHAT_MSG_PREFIX_MAX_LEN];
};

struct KUiInformationParam
{
    char sInformation[MAX_MESSAGE_LENGTH];   //消息文字内容
    short nInforLen;                         //消息文字内容的存储长度
    BOOL bNeedConfirmNotify;
};

enum PLAYER_ACTION_LIST
{
    PA_NONE = 0,      //无动作
    PA_RUN = 0x01,    //跑
    PA_SIT = 0x02,    //打坐
    PA_RIDE = 0x04,   //骑（马）
};
//==================================
//	系统消息分类
//==================================
enum SYS_MESSAGE_TYPE
{
    SMT_NORMAL = 0,   //不参加分类的消息
    SMT_SYSTEM,       //系统，连接相关
    SMT_PLAYER,       //玩家相关
    SMT_TEAM,         //组队相关
    SMT_FRIEND,       //聊天好友相关
    SMT_MISSION,      //任务相关
    SMT_CLIQUE,       //帮派相关
    SMT_TRADE,        //帮派相关
    SMT_WAR,          //帮派相关
};

//==================================
//	系统消息响应方式
//==================================
enum SYS_MESSAGE_CONFIRM_TYPE
{
    SMCT_NONE,                  //在对话消息窗口直接掠过，不需要响应。
    SMCT_CLICK,                 //点击图标后立即删除。
    SMCT_MSG_BOX,               //点击图标后弹出消息框。
    SMCT_UI_RENASCENCE,         //选择重生
    SMCT_UI_ATTRIBUTE,          //打开属性页面
    SMCT_UI_SKILLS,             //打开技能页面
    SMCT_UI_ATTRIBUTE_SKILLS,   //打开属性页面技能页面
    SMCT_UI_TEAM_INVITE,        //答应或拒绝加入队伍的邀请,
    //						pParamBuf 指向一个KUiPlayerItem结构的数据，表示邀情人(队长)
    SMCT_UI_TEAM_APPLY,   //答应或拒绝加入队伍的申请,
    //						pParamBuf 指向一个KUiPlayerItem结构的数据，表示申请人
    SMCT_UI_TEAM,        //打开队伍管理面板
    SMCT_UI_INTERVIEW,   //打开聊天对话界面,
    //						pParamBuf 指向一个KUiPlayerItem结构的数据，表示发来消息的好友
    SMCT_UI_FRIEND_INVITE,   //批准或拒绝别人加自己为好友
    //						pParamBuf 指向一个KUiPlayerItem结构的数据，表示发出好友邀请的人
    SMCT_UI_TRADE,   //答应或拒绝交易的请求,
    SMCT_UI_TRADE_FOLKGAME,
    SMCT_UI_SPAR,
    //						pParamBuf 指向一个KUiPlayerItem结构的数据，表示发出交易邀请的人
    SMCT_DISCONNECT,           //断线
    SMCT_UI_TONG_JOIN,         //答应或拒绝加入帮会的申请
    SMCT_UI_TONG_JOIN_APPLY,   //答应或拒绝加入帮会的申请
};

//==================================
//	系统消息
//==================================
struct KSystemMessage
{
    char szMessage[256];           //消息文本
    unsigned int uReservedForUi;   //界面使用的数据域,core里填0即可
    unsigned char eType;           //消息分类取值来自枚举类型 SYS_MESSAGE_TYPE
    unsigned char byConfirmType;   //响应类型
    unsigned char byPriority;      //优先级,数值越大，表示优先级越高
    unsigned char byParamSize;     //伴随GDCNI_SYSTEM_MESSAGE消息的pParamBuf所指参数缓冲区空间的大小。
};

//==================================
//	聊天频道的描述
//==================================
struct KUiChatChannel
{
    int nChannelNo;
    unsigned int uChannelId;
    union
    {
        int nChannelIndex;
        int nIsSubscibed;   //是否被订阅
    };
    char cTitle[32];
};

//==================================
//	聊天好友的一个分组的信息
//==================================
struct KUiChatGroupInfo
{
    char szTitle[32];   //分组的名称
    int nNumFriend;     //组内好友的数目
};

//==================================
//	好友发来的聊天话语
//==================================
struct KUiChatMessage
{
    unsigned int uColor;
    short nContentLen;
    char szContent[256];
};

//==================================
//	主角的一些不易变的数据
//==================================
struct KUiPlayerBaseInfo
{
    char Name[32];   //名字
    char szTongName[32];
    char szMateName[32];
    char Title[128];     //称号
    int nCurFaction;     // 当前加入门派 id ，如果为 -1 ，当前没有在门派中
    int nMissionGroup;   // 当前加入门派 id ，如果为 -1 ，当前没有在门派中
    int nRoomId;
    int nRankInWorld;   //江湖排名值,值为0表示未上排名板
    int nRankInSect;
    int nPKValue;
    int nRepute;
    int nFuYuan;
    int nTranslife;

    char StatusDesc[16];     //状态描述
    unsigned int nCurTong;   // 当前加入帮派name id ，如果为 0 ，当前没有在帮派中
};

//==================================
//	主角的一些易变的数据
//==================================
struct KUiPlayerRuntimeInfo
{
    int nLifeFull;             //生命满值
    int nLife;                 //生命
    int nManaFull;             //内力满值
    int nMana;                 //内力
    int nStaminaFull;          //体力满值
    int nStamina;              //体力
    int nExperienceFull;       //经验满值
    int nExperience;           //当前经验值
    int nCurLevelExperience;   //当前级别升级需要的经验值

    unsigned char byAction;   //正在进行的行为动作，为枚举PLAYER_ACTION_LIST取值的组合
};

//==================================
//	主角的一些属性数据索引
//==================================
enum UI_PLAYER_ATTRIBUTE
{
    UIPA_STRENGTH = 0,   //力量
    UIPA_DEXTERITY,      //敏捷
    UIPA_VITALITY,       //活力
    UIPA_ENERGY,         //精力
};

//==================================
//	主角的一些易变的属性数据
//==================================
struct KUiPlayerAttribute
{
    int nMoney;            //银两
    int nLevel;            //等级
    char StatusDesc[16];   //状态描述

    int nBARemainPoint;   //基本属性剩余点数
    int nStrength;        //力量
    int nDexterity;       //敏捷
    int nVitality;        //活力
    int nEnergy;          //精力

    int nKillMAX;        //最大杀伤力
    int nKillMIN;        //最小杀伤力
    int nRightKillMax;   //右键最大杀伤力
    int nRightKillMin;   //右键最小杀伤力

    int nLeftAttack;    //攻击力
    int nRightAttack;   //攻击力
    int nDefence;       //防御力
    int nMoveSpeed;     //移动速度
    int nAttackSpeed;   //攻击速度
    int nCastSpeed;

    //Khang 5 hanh
    int nPhyDef;          //物理防御
    int nPhyDefPlus;      //物理防御
    int nCoolDef;         //冰冻防御
    int nCoolDefPlus;     //冰冻防御
    int nLightDef;        //闪电防御
    int nLightDefPlus;    //闪电防御
    int nFireDef;         //火焰防御
    int nFireDefPlus;     //火焰防御
    int nPoisonDef;       //毒素防御
    int nPoisonDefPlus;   //毒素防御

    int nPKValue;
    int nRepute;
    int nFuYuan;
    int nTranslife;
    int nRankInWorld;
    char Title[128];   //称号
};

//==================================
//	主角的立即使用物品与武功
//==================================
struct KUiPlayerImmedItemSkill
{
    KUiGameObject ImmediaItem[IMMEDIACY_ROOM_WIDTH];
    KUiGameObject IMmediaSkill[2];
};

//==================================
//	主角装备安换的位置
//==================================
enum UI_EQUIPMENT_POSITION
{
    UIEP_HEAD = 0,
    UIEP_HAND,
    UIEP_NECK,
    UIEP_FINESSE,
    UIEP_BODY,
    UIEP_WAIST,
    UIEP_FINGER1,
    UIEP_FINGER2,
    UIEP_WAIST_DECOR,
    UIEP_FOOT,
    UIEP_HORSE,
    UIEP_MASK,
    UIEP_MANTLE,
    UIEP_SIGNET,
    UIEP_SHIPIN,
    UIEP_HOODS,
    UIEP_CLOAK,
};

//TamLTM Code kham nam xanh
enum UI_BUILDITEM_POSITION
{
    UIEP_BUILDITEM1 = 0,
    UIEP_BUILDITEM2 = 1,
    UIEP_BUILDITEM3 = 2,
    UIEP_BUILDITEM4 = 3,
    UIEP_BUILDITEM5 = 4,
    UIEP_BUILDITEM6 = 5,
    UIEP_BUILDITEM7 = 6,
    UIEP_BUILDITEM8 = 7,
    UIEP_BUILDITEM9 = 8,
};
// End code

enum MOSAICENCRUSTED_POSITION
{
    MOSAICENCRUSTED_UIEP_BOX_1 = 0,
    MOSAICENCRUSTED_UIEP_BOX_2,
    MOSAICENCRUSTED_UIEP_BOX_3,
    MOSAICENCRUSTED_UIEP_BOX_4,
    MOSAICENCRUSTED_UIEP_BOX_5,
    MOSAICENCRUSTED_UIEP_BOX_6,
    MOSAICENCRUSTED_UIEP_BOX_7,
    MOSAICENCRUSTED_UIEP_BOX_8,
    MOSAICENCRUSTED_UIEP_BOX_9,
};

enum PLAYER_SECLECT_UI
{
    SELECT_TALKUI = 0,
    SELECT_SELECTUI,
    SELECT_NUM,
};

//==================================
//	单项技能数据
//==================================
struct KUiSkillData : public KUiGameObject
{
    union
    {
        int nLevel;
        int nData;
    };
};

//==================================
//	一个队伍中最多包含成员的数目
//==================================
#define PLAYER_TEAM_MAX_MEMBER 8

//==================================
//	统帅能力相关的数据
//==================================
struct KUiPlayerLeaderShip
{
    int nLeaderShipLevel;            //统帅力等级
    int nLeaderShipExperience;       //统帅力经验值
    int nLeaderShipExperienceFull;   //升到下级需要的经验值
};

//==================================
//	一个玩家角色项
//==================================
struct KUiPlayerItem
{
    char Name[32];      //玩家角色姓名
    unsigned int uId;   //玩家角色id
    int nIndex;         //玩家角色索引
    int nData;          //此玩家相关的一项数值，含义与具体的应用位置有关
    int nTeamID;
};

//==================================
//	组队信息的描述
//==================================
struct KUiTeamItem
{
    KUiPlayerItem Leader;
};

//==================================
//	队伍信息
//==================================
struct KUiPlayerTeam
{
    bool bTeamLeader;   //玩家自己是否队长
    char cNumMember;    //队员数目
    char cNumTojoin;    //欲加入的人员的数目
    bool bOpened;       //队伍是否允许其他人加入
    int nModePick;
    bool bPKFollowCaptain;   //队伍是否允许其他人加入
    int nTeamServerID;       //队伍在服务器上的id，用于标识该队伍，-1 为空
    int nCaptainPower;
};

//==================================
//	魔法属性
//==================================
#ifndef MAGICATTRIB
#define MAGICATTRIB
struct KMagicAttrib
{
    int nAttribType;   //属性类型
    int nValue[3];     //属性参数
    short nMin;        //luu gia tri min cua opt
    short nMax;        //luu gia tri max cua opt
    KMagicAttrib()
    {
        nValue[0] = nValue[1] = nValue[2] = nAttribType = nMin = nMax = 0;
    };
};

struct KMagicAutoSkill
{
    int nSkillId;
    int nSkillLevel;
    int nRate;   //属性参数
    DWORD dwNextCastTime;
    int nWaitCastTime;
    KMagicAutoSkill()
    {
        nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = 0;
    };
};
#else
struct KMagicAttrib;
struct KMagicAutoSkill;
#endif

struct KStateControl
{
    int nSkillId;
    int nLeftTime;
};

struct KMapPos
{
    int nSubWorld;
    int nRegion;
    int nMapX;
    int nMapY;
    int nOffX;
    int nOffY;
};

//==================================
//	选项设置项
//==================================
enum OPTIONS_LIST
{
    OPTION_PERSPECTIVE,   //透视模式  nParam = (int)(bool)bEnable 是否开启
    OPTION_DYNALIGHT,     //动态光影	nParam = (int)(bool)bEnable 是否开启
    OPTION_MUSIC_VALUE,   //音乐音量	nParam = 音量大小（取值为0到-10000）
    OPTION_SOUND_VALUE,   //音效音量	nParam = 音量大小（取值为0到-10000）
    OPTION_BRIGHTNESS,    //亮度调节	nParam = 亮度大小（取值为0到-100）
    OPTION_WEATHER,       //天气效果开关 nParam = (int)(bool)bEnable 是否开启
    OPTION_OTHER,         //TamLTM Toi uu hinh anh
};

//==================================
//	所处的地域时间环境信息
//==================================

struct KUiSceneTimeInfo
{
    char szSceneName[80];   //场景名
    int nSceneId;           //场景id
    int nScenePos0;         //场景当前坐标（东）
    int nScenePos1;         //场景当前坐标（南）
    int nGameSpaceTime;     //以分钟为单位

    //TamLTM Bang hoi chiem linh
    char szTongName[32];
    char szTongNameBC[32];
    int szTongT;
    int szTongVG;
    //end code
};

struct KUiSceneTimeInfoOften : public KUiSceneTimeInfo
{
};

struct KMapTraffic
{
    char Content[80];
    int Kind;
    POINT Point;
    POINT TPoint;
    int MapIndex;
    void Release()
    {
        memset(Content, 0, sizeof(Content));
        Kind = -1;
        Point.x = Point.y = TPoint.x = TPoint.y = MapIndex = 0;
    };
};
//==================================
//	光源信息
//==================================
//整数表示的三维点坐标
struct KPosition3
{
    int nX;
    int nY;
    int nZ;
};

struct KLightInfo
{
    KPosition3 oPosition;   // 光源位置
    DWORD dwColor;          // 光源颜色及亮度
    long nRadius;           // 作用半径
};

//小地图的显示内容项
enum SCENE_PLACE_MAP_ELEM
{
    SCENE_PLACE_MAP_ELEM_NONE = 0x00,        //无东西
    SCENE_PLACE_MAP_ELEM_PIC = 0x01,         //显示缩略图
    SCENE_PLACE_MAP_ELEM_CHARACTER = 0x02,   //显示人物
    SCENE_PLACE_MAP_ELEM_PARTNER = 0x04,     //显示同队伍人
};

//场景的地图信息
struct KSceneMapInfo
{
    int nScallH;   //真实场景相对于地图的横向放大比例
    int nScallV;   //真实场景相对于地图的纵向放大比例
    int nFocusMinH;
    int nFocusMinV;
    int nFocusMaxH;
    int nFocusMaxV;
    int nOrigFocusH;
    int nOrigFocusV;
    int nFocusOffsetH;
    int nFocusOffsetV;
};

enum NPC_RELATION
{
    relation_none = 1,
    relation_self = 2,   //Chinh minh
    relation_ally = 4,   //Dong minh team
    relation_enemy = 8,
    relation_dialog = 16,
    relation_all = relation_none | relation_ally | relation_enemy | relation_self | relation_dialog,
    relation_num,
};

enum NPCKIND
{
    kind_normal = 0,
    kind_player,
    kind_partner,
    kind_dialoger,   //对话者
    kind_bird,
    kind_mouse,
    kind_num
};

enum   // 物件类型
{
    Obj_Kind_MapObj = 0,   // 地图物件，主要用于地图动画
    Obj_Kind_Body,         // npc 的尸体
    Obj_Kind_Box,          // 宝箱
    Obj_Kind_Item,         // 掉在地上的装备
    Obj_Kind_Money,        // 掉在地上的钱
    Obj_Kind_LoopSound,    // 循环音效
    Obj_Kind_RandSound,    // 随机音效
    Obj_Kind_Light,        // 光源（3D模式中发光的东西）
    Obj_Kind_Door,         // 门类
    Obj_Kind_Trap,         // 陷阱
    Obj_Kind_Prop,         // 小道具，可重生
    Obj_Kind_Obstacle,
    Obj_Kind_Num,   // 物件的种类数
};

//主角身份地位等一些关键属性项
enum PLAYER_BRIEF_PROP
{
    PBP_LEVEL = 1,   //登级变化	nParam表示当前等级
    PBP_FACTION,     //门派		nParam表示门派属性，如果nParam为-1表示没有门派
    PBP_CLIQUE,      //帮派		nParam为非0值表示入了帮派，0值表示脱离了帮派
};

//新闻消息的类型定义
enum NEWS_MESSAGE_TYPE
{
    NEWSMESSAGE_NORMAL,     //一般消息，显示（一次）就消息消亡了
                            //无时间参数
    NEWSMESSAGE_COUNTING,   //倒计（秒）数消息，计数到0时，就消息就消亡了。
                            //时间参数中的数据结构中仅秒数据有效，倒计数以秒为单位。
    NEWSMESSAGE_TIMEEND,    //定时消息，定时到时，消息就消完了，否则每半分钟显示一次。
                            //时间参数表示消亡的指定时间。
};

struct KNewsMessage
{
    int nType;                       //消息类型
    char sMsg[MAX_MESSAGE_LENGTH];   //消息内容
    int nMsgLen;                     //消息内容存储长度
};

struct KRankIndex
{
    bool bValueAppened;         //每一项是否有没有额外数据
    bool bSortFlag;             //每一项是否有没有升降标记
    unsigned short usIndexId;   //排名项ID数值
};

#define MAX_RANK_MESSAGE_STRING_LENGTH 128

struct KRankMessage
{
    char szMsg[MAX_RANK_MESSAGE_STRING_LENGTH];   // 文字内容
    unsigned short usMsgLen;                      // 文字内容的长度
    short cSortFlag;                              // 旗标值，QOO_RANK_DATA的时候表示出升降，负值表示降，正值表示升，0值表示位置未变
    int nValueAppend;                             // 此项附带的值
};

struct KMissionRecord
{
    char szHeader[32];
    int nHeaderLen;
    int nRecordAct;
    char szContent[MAX_SCIRPTACTION_BUFFERNUM];   //存储任务提示信息的缓冲区，（字符串为控制符已经编码的字符串）
    int nContentLen;                              //sContent内有效内容的长度(单位：字节)，长度最大一定不超过256字节
    unsigned int uValue;                          //关联数值
};

enum TEAM_FIGURE
{
    TEAM_CAPTAIN,   // 队伍成员身份 队长
    TEAM_MEMBER,    // 队伍成员身份 队员
};

enum CHATROOM_FIGURE
{
    CHATROOM_NOTHING,
    CHATROOM_MEMBER,   // 队伍成员身份 队员
    CHATROOM_HOST,     // 队伍成员身份 队长
};
//---------------------------- 帮会相关 ------------------------

#define defTONG_MAX_DIRECTOR 2
#define defTONG_MAX_MANAGER 56
#define defTONG_ONE_PAGE_MAX_NUM 7
#define defTONG_NOTIFY_LENGTH 256
#define defTONG_NAME_LENGTH_32 32
#define defTONG_NAME_LENGTH_12 12
#define defTONG_EFF_LASTWEEK 1
#define defTONG_EFF_THISWEEK 2
#define defTONG_EFF_USEABLE 3

enum TONG_MEMBER_FIGURE
{
    enumTONG_FIGURE_MEMBER,     // 帮众
    enumTONG_FIGURE_MANAGER,    // 队长
    enumTONG_FIGURE_DIRECTOR,   // 长老
    enumTONG_FIGURE_MASTER,     // 帮主
    enumTONG_FIGURE_NUM,
};

enum
{
    enumTONG_APPLY_INFO_ID_SELF,        // 申请查询自身信息
    enumTONG_APPLY_INFO_ID_MASTER,      // 申请查询帮主信息
    enumTONG_APPLY_INFO_ID_DIRECTOR,    // 申请查询长老信息
    enumTONG_APPLY_INFO_ID_MANAGER,     // 申请查询队长信息
    enumTONG_APPLY_INFO_ID_MEMBER,      // 申请查询帮众信息(一批帮众)
    enumTONG_APPLY_INFO_ID_ONE,         // 申请查询某帮会成员信息(一个帮众)
    enumTONG_APPLY_INFO_ID_TONG_HEAD,   // 申请查询某帮会信息，用于申请加入帮会
    enumTONG_APPLY_INFO_ID_NUM,
};

enum
{
    enumTONG_CREATE_ERROR_ID1,    // Player[m_nPlayerIndex].m_nIndex <= 0
    enumTONG_CREATE_ERROR_ID2,    // 交易过程中
    enumTONG_CREATE_ERROR_ID3,    // 帮会名问题
    enumTONG_CREATE_ERROR_ID4,    // 帮会阵营问题
    enumTONG_CREATE_ERROR_ID5,    // 已经是帮会成员
    enumTONG_CREATE_ERROR_ID6,    // 自己的阵营问题
    enumTONG_CREATE_ERROR_ID7,    // 等级问题
    enumTONG_CREATE_ERROR_ID8,    // 钱问题
    enumTONG_CREATE_ERROR_ID9,    // 组队不能建帮会
    enumTONG_CREATE_ERROR_ID10,   // 帮会模块出错
    enumTONG_CREATE_ERROR_ID11,   // 名字字符串出错
    enumTONG_CREATE_ERROR_ID12,   // 名字字符串过长
    enumTONG_CREATE_ERROR_ID13,   // 帮会同名错误
    enumTONG_CREATE_ERROR_ID14,   // 帮会产生失败
    enumTONG_CREATE_ERROR_ID15,   // 帮会产生失败
};

enum
{
    enumTONG_CHANGE_CAMP_ERROR_ID1,
    enumTONG_CHANGE_CAMP_ERROR_ID2,
    enumTONG_CHANGE_CAMP_ERROR_ID3,
    enumTONG_CHANGE_CAMP_ERROR_ID4,
    enumTONG_CHANGE_CAMP_ERROR_ID5,
    enumTONG_CHANGE_CAMP_ERROR_ID6,
    enumTONG_CHANGE_CAMP_ERROR_ID7,
    enumTONG_CHANGE_CAMP_ERROR_ID8,
};

//帮派信息
struct KTongInfo
{
    char szName[defTONG_NAME_LENGTH_32];         //帮会名字
    char szMasterName[defTONG_NAME_LENGTH_32];   //帮主名字
    int nTongJiyuParam;
    char szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
    int nFaction;   //帮会阵营
    int nMoney;     //帮会资金
    BYTE btLevel;
    DWORD dwTotalEff;
    int nMasterJoinTm;
    int nMasterSaveEff;
    BOOL bRecruit;
    int nMemberCount;   //帮众数量
    int nManagerCount;
    int nDirectorCount;
};

struct KTongParam
{
    int m_nLevel;
    int m_nLeadLevel;
    int m_nMoney;
    int m_nItemCount;
    int m_nItemGenre;
    int m_nItemDetail;
    int m_nRepute;
    int m_nFuYuan;
    int m_nLeaveMoney;
    int m_nLeaveRepute;
    int m_nLeaveTime;
    int m_nMoneyChangeCamp;
    int m_nMoneySaveJiyu;
};

//对帮内成员做的动作，或改变自己与帮会的关系的动作
enum TONG_ACTION_TYPE
{
    TONG_ACTION_DISMISS,   //踢人
    TONG_ACTION_ASSIGN,    //任命
    TONG_ACTION_DEMISE,    //传位
    TONG_ACTION_LEAVE,     //离帮
    TONG_ACTION_RECRUIT,   //招人状态
    TONG_ACTION_APPLY,     //申请加入
    TONG_ACTION_CHANGE_AGNAME,
    TONG_ACTION_CHANGE_MALE_AGNAME,
    TONG_ACTION_CHANGE_FEMALE_AGNAME,
    TONG_ACTION_CHANGE_CAMP_JUSTIE,
    TONG_ACTION_CHANGE_CAMP_EVIL,
    TONG_ACTION_CHANGE_CAMP_BALANCE,
};

enum TONG_MONEY_ACTION_TYPE
{
    TONG_ACTION_SAVE,   //踢人
    TONG_ACTION_GET,    //任命
};

//对帮内成员做的动作，或改变自己与帮会的关系的动作时的参数
struct KTongOperationParam
{
    TONG_ACTION_TYPE eOper;   //操作项
    int nData[4];
    char szData[32];   //操作项需要的密码，如果对应的操作项不需要密码，则直接填密码为空。
};

//帮会成员项
struct KTongMemberItem : public KUiPlayerItem
{
    char szAgname[defTONG_NAME_LENGTH_32];   //在帮内的职务/称号
    BOOL bOnline;
    int nSaveEff;
    int nJoinTm;
};

//某个玩家与XX的关系，(XX可以是帮会，队伍等等)
struct KUiPlayerRelationWithOther : KUiPlayerItem
{
    int nRelation;
    int nParam;
};

//通用的带名称描述游戏对象的结构
struct KUiGameObjectWithName
{
    char szName[32];
    char szString[32];
    int nData;
    int nParam;
    unsigned int uParam;
};

enum
{
    enumLadderBegin = 0,
    enumTopTenMasterHand,
    enumTopTenRicher,
    enumTopTenKiller,
    enumTopTenRepute,
    enumTopTenFuYuan,
    enumTopTenAccumStat1,
    enumTopTenAccumStat2,
    enumTopTenHonorStat,
    enumTopTenTimeStat,
    enumTopTenTongLv,
    enumTopTenTongMn,
    enumTopTenTongEff,
    enumFacTopTenMasterHand,
    enumFacTopTenRicher = enumFacTopTenMasterHand + (MAX_FACTION + 1),
    enumLadderEnd = enumFacTopTenRicher + (MAX_FACTION + 1),
};

enum NPCFIGHT
{
    enumFightNone = 0,
    enumFightActive,
    enumFightNum,
};

enum
{
    enumPKNormal = 0,
    enumPKWar,
    enumPKMurder,
    enumPKTongWar,
    enumPKNum,
};

enum
{
    enumPKLogNothing,
    enumPKLogAll,
    enumPKLogTongWar,
    enumPKLogMurder,
    enumPKLogWar,
    enumPKLogNormal,
    enumPKLogNum,
};

struct KOneRoom
{
    char lpszRoomHost[32];
    char lpszRoomName[32];
    BYTE byRoomMemNum;
    int nRoomLeftTime;
    int nRoomParam;
};

struct KOneMem
{
    char lpszName[32];
    int nMemParam;
    int nMemIndex;
    BOOL bIsRoomHost;
};

struct KLockItem
{
    KLockItem()
    {
        Clear();
    };
    ~KLockItem()
    {
        Clear();
    };
    short nState;
    DWORD dwLockTime;
    void Clear()
    {
        nState = LOCK_STATE_NORMAL;
        dwLockTime = 0;
    };
    BOOL IsLock()
    {
        return (nState != LOCK_STATE_NORMAL);
    };
};

//#define MAX_DATAGROUP_VALUE 4
#define MAX_DATAGROUP_VALUE 4
struct KDataGroup
{
    int nNameId;
    int nValue[MAX_DATAGROUP_VALUE];
    char szName1[32];
    char szName2[32];
    void Clear()
    {
        nNameId = 0;
        memset(nValue, 0, sizeof(nValue));
        memset(szName1, 0, sizeof(szName1));
        memset(szName2, 0, sizeof(szName2));
    }
};

#define MAX_MISSION_PARAM 16
#define MAX_GLBMISSION_PARAM 3
#define MISSION_STATNUM 10
#define MISSION_PARAM_AVAILABLE 0
#define MISSION_AVAILABLE_VALUE 1
struct TMissionLadderSelfInfo
{
    char szMissionName[64];
    unsigned char ucGroup;
    int nGlbParam[MAX_GLBMISSION_PARAM];
    int nParam[MAX_MISSION_PARAM];
    void Clear()
    {
        memset(szMissionName, 0, sizeof(szMissionName));
        memset(nGlbParam, 0, sizeof(nGlbParam));
        memset(nParam, 0, sizeof(nParam));
    };
};

struct TMissionLadderInfo
{
    unsigned char ucGroup;
    char Name[32];
    int nParam[MAX_MISSION_PARAM];
    void Clear()
    {
        ucGroup = 0;
        memset(Name, 0, sizeof(Name));
        memset(nParam, 0, sizeof(nParam));
    };
};

#define MAX_SUPERSHOP_SHOPTAB 10
typedef struct
{
    int m_nCurShop;
    int m_nShopIdx[MAX_SUPERSHOP_SHOPTAB];
    int m_nShopNum;
    int m_nMoneyUnit;
    DWORD m_SubWorldID;
    int m_nMpsX;
    int m_nMpsY;
    int m_nBuyIdx;
    void Clear()
    {
        memset(m_nShopIdx, -1, sizeof(m_nShopIdx));
        m_nMoneyUnit = moneyunit_money;
        m_SubWorldID = -1;
        m_nMpsX = 0;
        m_nMpsY = 0;
    }
} BuySellInfo;

struct PLAYERTRADE
{
    char cName[32];
    BOOL nTrade;
    int nDest;
    void Release()
    {
        memset(cName, 0, sizeof(cName));
        nTrade = FALSE;
        nDest = 0;
    };
};

typedef struct
{
    char szName[64];
    DWORD dwColor;
    int nStateGraphics;
    DWORD dwLeftTime;
    void Release()
    {
        memset(szName, 0, sizeof(szName));
        dwColor = 0;
        dwLeftTime = 0;
        nStateGraphics = 0;
    };
} KExpandRank;

struct LockMoveItem
{
    BOOL bLock;
    int nPlace;
    void Release()
    {
        bLock = FALSE;
        nPlace = 0;
    };
};

#define MAX_AUTO_SIZE 50
#define MAX_AUTO_LIST 15
#define MAX_TIME_OVERLOOK 100
#define MAX_TIME_RESET_OVERLOOK 10000
#define MAX_TIME_SPACE 1500
#define MAX_AUTO_STATESKILL 3
#define MAX_AUTO_FILTERL 40

enum
{
    enumRoomNotEnough1,
    enumRoomNotEnough2,
    enumRoomNotEnough4,
    enumRoomNotEnough6,
};

enum
{
    enumPickEquipAll = 0,
    enumPickEquipJewelry,
    enumPickEquipHaveMagic,
    enumPickNum,
};

#endif