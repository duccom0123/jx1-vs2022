/*******************Editer	: duccom0123 EditTime:	2024/06/12 11:48:44*********************
//	������Core�õ����ݽṹ�ȵĶ���
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-12
------------------------------------------------------------------------------------------
	һЩ������ܴ�������Ϸ�����ģ���ͷ�ļ��У����ڴ˴������Ǹ�ͷ�ļ�������������ͷ�ļ�
��Ҫ����һЩ����Ϸ��������޹ص����ݡ�
    ������������Ϸ������ⲿ�ͻ���δ�����Ϸ����ӿ��������������£�������ֱ���ڴ��ļ�
��������Ҫ�����ݶ��壬��Ϸ�����ģ��ɸ���������Ҫ����ưѶ������޸Ļ��ƶ���ģ���ڵĶ���
ͷ�ļ������ڴ˽�����Ҫ�İ�����
*****************************************************************************************/
#ifndef GAMEDATADEF_H
#define GAMEDATADEF_H

#include "CoreObjGenreDef.h"
#include "CoreUseNameDef.h"

#define		ITEM_VERSION						1

#define		_CHAT_SCRIPT_OPEN

#ifdef	_SERVER
#define	MAX_NPC			48000
#define	MAX_PLAYER		1200
#define	MAX_ITEM		160000
#else
#define	MAX_NPC			256
#define	MAX_PLAYER		2
#define	MAX_ITEM		512
#endif

#define		MAX_TEAM_MEMBER						7
#define		MAX_QUESTION_LENGTH					2056
#define		MAX_SENTENCE_LENGTH					256
#define		MAX_MESSAGE_LENGTH					512
#define		MAX_ANSWERNUM						20

#define 	NORMAL_FONTSIZE						12
#define		FILE_NAME_LENGTH					80
#define		OBJ_NAME_LENGTH						40
#define		PLAYER_PICKUP_CLIENT_DISTANCE		63
#define		defMAX_EXEC_OBJ_SCRIPT_DISTANCE		200
#define		defMAX_PLAYER_SEND_MOVE_FRAME		5
#define		PLAYER_PICKUP_SERVER_DISTANCE		40000

#define		MAX_PERCENT							100
#define		MAX_INT								0x7fffffff
#define		MONEY_FLOOR							10000

#define		ROLE_NO								2
#define		PLAYER_MALE_NPCTEMPLATEID			-1
#define		PLAYER_FEMALE_NPCTEMPLATEID			-2

#define		PLAYER_SHARE_EXP_DISTANCE			768

#define		MAX_DEATH_PUNISH_PK_VALUE			10

#define		RESOLUTION_WIDTH				1900
#define		RESOLUTION_HEIGHT				800

#define		POISON_DAMAGE_TIME				60
#define		POISON_DAMAGE_INTERVAL			10
#define		COLD_DAMAGE_TIME				60
#define		MAX_DEATLY_STRIKE_ENHANCEP		200
#define		MIN_FATALLY_STRIKE_ENHANCEP		30
#define		MAX_FATALLY_STRIKE_ENHANCEP		50

#define		MAX_LEVEL						200
#define		MAX_SERIES						5
#define		MAX_LEAD_LEVEL					100

#define		MAX_TONG_COUNT					50
#define		MAX_TONG_LEVEL					10
#define		MAX_TONG_NATIONALEMBLEM			6

#define		MAX_COMPOUND_ITEM				3

#define REGION_PIXEL_WIDTH	512
#define REGION_PIXEL_HEIGHT	1024
#define REGION_CELL_SIZE_X	32
#define REGION_CELL_SIZE_Y	32
#define REGION_CELL_WIDTH	(REGION_PIXEL_WIDTH/REGION_CELL_SIZE_X)
#define REGION_CELL_HEIGHT	(REGION_PIXEL_HEIGHT/REGION_CELL_SIZE_Y)

#define		GAME_FPS					18
#define		GAME_UPDATE_TIME			10
#define		GAME_SYNC_LOSS				100

#define		MAX_TASK					255
#define		MAX_TEMP_TASK				100

#define		MAX_SCIRPTACTION_BUFFERNUM	2056

#define 	TRADEFOLKGAME_NEED_WIDTH	6
#define 	TRADEFOLKGAME_NEED_HEIGHT	5

#define 	MIN_ITEM_LEVEL				0
#define 	MAX_ITEM_LEVEL				10
#define 	MAX_ITEM_GENERATORLEVEL		10
#define 	MAX_ITEM_LUCK				10
#define 	MAX_ITEM_MAGICATTRIB		8
#define 	MAX_ITEM_MAGICLEVEL			MAX_ITEM_MAGICATTRIB * 2
#define 	MAX_ITEM_NORMAL_MAGICATTRIB	6
#define		MAX_ITEM_S3DBINTERFACE1		15
#define		MAX_ITEM_S3DBINTERFACE2		20

#define		FACTIONS_PRR_SERIES				2
#define		HUASHANID						10
#define		MAX_FACTION						FACTIONS_PRR_SERIES * series_num + 1

#define		MIN_PASSWORD_LENGTH		0
#define		MAX_PASSWORD_LENGTH		16

enum LOCK_STATE
{
	LOCK_STATE_CHARACTER = -3,
	LOCK_STATE_FOREVER = -2,
	LOCK_STATE_LOCK,
	LOCK_STATE_NORMAL,
	LOCK_STATE_UNLOCK,
};


#define		MAX_STATTASK					10
#define		MAX_TRANSLIFE_VALUE				10
#define		MAX_VIPRANK_VALUE				10

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

enum COMPOUND_ACTION
{
	CPA_FUSION,
	CPA_CRYOLITE,
	CPA_PROPMINE,
	CPA_NUM,
};

enum ENCHASE_ACTION
{
	ECA_DISTILL,
	ECA_FORGE,
	ECA_ENCHASE,
	ECA_ATLAS,
};

enum COMPOUND_PART
{
	compound_box1 = 0,
	compound_box2,
	compound_box3,
	compound_num,
};

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

enum TOWNPORTALDETAILTYPE
{
	townportal_normal,
	townportal_infinite,
	townportal_special,
};

typedef struct
{
	BOOL	bIsSkill;
	int		nIdx;
	int		nPlace;
	int		nX;
	int		nY;
	void	Release() {
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

#define		MAX_HAND_ITEM				1
#define		EQUIPMENT_ROOM_WIDTH		6
#define		EQUIPMENT_ROOM_HEIGHT		10
#define		MAX_EQUIPMENT_ITEM			(EQUIPMENT_ROOM_WIDTH * EQUIPMENT_ROOM_HEIGHT)
#define		REPOSITORY_ROOM_WIDTH		6
#define		REPOSITORY_ROOM_HEIGHT		10
#define		MAX_REPOSITORY_ITEM			((REPOSITORY_ROOM_WIDTH * REPOSITORY_ROOM_HEIGHT) * (1 + REPOSITORY_NUM))
#define		TRADE_ROOM_WIDTH			10
#define		TRADE_ROOM_HEIGHT			4
#define		MAX_TRADE_ITEM				(TRADE_ROOM_WIDTH * TRADE_ROOM_HEIGHT)
#define		MAX_TRADE1_ITEM				MAX_TRADE_ITEM
#define		GIVE_ROOM_WIDTH				6
#define		GIVE_ROOM_HEIGHT			4
#define		MAX_GIVE_ITEM				(GIVE_ROOM_WIDTH * GIVE_ROOM_HEIGHT)
#define		MOSAICENCRUSTED_ROOM_WIDTH	2
#define		MOSAICENCRUSTED_ROOM_HEIGHT	4
#define		MAX_MOSAICENCRUSTED_ITEM	(MOSAICENCRUSTED_ROOM_WIDTH * MOSAICENCRUSTED_ROOM_HEIGHT)
#define		IMMEDIACY_ROOM_WIDTH		9
#define		IMMEDIACY_ROOM_HEIGHT		1
#define		MAX_IMMEDIACY_ITEM			(IMMEDIACY_ROOM_WIDTH * IMMEDIACY_ROOM_HEIGHT)
#define		MAX_PLAYER_ITEM_RESERVED	32
#define		MAX_PLAYER_ITEM				(MAX_EQUIPMENT_ITEM + MAX_REPOSITORY_ITEM + MAX_TRADE_ITEM + MAX_TRADE1_ITEM + MAX_IMMEDIACY_ITEM + itempart_num + MAX_HAND_ITEM + MAX_PLAYER_ITEM_RESERVED)

#define		REMOTE_REVIVE_TYPE		0
#define		LOCAL_REVIVE_TYPE		1

#define		MAX_MELEE_WEAPON		7
#define		HAND_PARTICULAR			6
#define		MAX_RANGE_WEAPON		3
#define		MAX_WEAPON				MAX_MELEE_WEAPON + MAX_RANGE_WEAPON

#define		MAX_NPC_TYPE			300

#define		MAX_NPC_DIR				64
#define		MAX_NPCSKILL			80
#define		MAX_NPCPARAM			4
#define		MAX_NPC_HEIGHT			128

#define		MAX_SKILL_STATE			18

#define		MAX_RESIST				95
#define		RESIST_PLUS_SCALE		10

#define		MIN_HIT_PERCENT			5
#define		MAX_HIT_PERCENT			95

#define		LEVEL_EXPLOSIVE			120
#define		LIFE_EXPLOSIVE			25

#define		PLAYER_MOVE_DO_NOT_MANAGE_DISTANCE	5

#define	NORMAL_NPC_PART_NO		5

#ifndef _SERVER
#define		C_REGION_X(x)	(LOWORD(SubWorld[0].m_Region[ (x) ].m_RegionID))
#define		C_REGION_Y(y)	(HIWORD(SubWorld[0].m_Region[ (y) ].m_RegionID))
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
	series_metal,			//	��ϵ
	series_wood,			//	ľϵ
	series_water,			//	ˮϵ
	series_fire,			//	��ϵ
	series_earth,			//	��ϵ
	series_nil,
	series_num = series_nil,
	series_minus,
};

enum OBJ_GENDER
{
	OBJ_G_MALE	= 0,	//���ԣ��е�
	OBJ_G_FEMALE,		//�Ƶģ�Ů��
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
	npc_blue,				// ������Ӫ����������ǰ����ң�
	npc_gold,			// ������Ӫ
	npcboss_num,				// ��Ӫ��
};

enum ITEM_IN_ENVIRO_PROP
{
	IIEP_NORMAL = 0,	//һ��/����/����
	IIEP_NOT_USEABLE,	//������/����װ��
	IIEP_SPECIAL,		//�ض��Ĳ�ͬ���
};

#define	GOD_MAX_OBJ_TITLE_LEN	2048	//128��ʱ��Ϊ1024Ϊ�˼��ݾɴ��� to be modified
#define	GOD_MAX_OBJ_PROP_LEN	1024
#define	GOD_MAX_OBJ_DESC_LEN	1024

//==================================
//	��Ϸ���������
//==================================
struct KGameObjDesc
{
	char	szTitle[GOD_MAX_OBJ_TITLE_LEN];	//���⣬����
	char	szProp[GOD_MAX_OBJ_PROP_LEN];	//���ԣ�ÿ�п���tab����Ϊ�����뿿�Ҷ���������
	char	szDesc[GOD_MAX_OBJ_DESC_LEN];	//����
};

//==================================
//	�������ѡ��
//==================================
struct KUiAnswer
{
	char	AnswerText[MAX_SENTENCE_LENGTH];	//��ѡ�����֣����԰������Ʒ���
	int		AnswerLen;			//��ѡ�𰸴洢���ȣ��������Ʒ�����������������
};

struct KUiQuestionAndAnswer
{
	int			LeftTime;
	char		Question[MAX_QUESTION_LENGTH];	//�������֣����԰������Ʒ���
	int			QuestionLen;	//�������ִ洢���ȣ��������Ʒ�����������������
	int			AnswerCount;	//��ѡ�𰸵���Ŀ
	KUiAnswer	Answer[1];		//��ѡ��
};

struct KUiImage 
{ 
    char		Name[MAX_PATH]; 
    int			Frame; 
};  

//==================================
//	���Ա�ʾ��Ϸ����Ľṹ
//==================================
struct KUiGameObject
{
	unsigned int uGenre;	//��������
	unsigned int uId;		//����id
//	int			 nData;		//�����ʵ����ص�ĳ����
};

//==================================
//	�������ʾ��һ������Χ
//==================================
struct KUiRegion
{
	int		h;		//���Ͻ���������
	int		v;		//���Ͻ����������
	int		Width;	//������
	int		Height;	//�����ݿ�
};

//==================================
//	������Ϸ�������ɵĵط�
//==================================
enum UIOBJECT_CONTAINER
{
	UOC_IN_HAND	= 1,		//��������
	UOC_GAMESPACE,			//��Ϸ����
	UOC_IMMEDIA_ITEM,		//�����Ʒ
	UOC_IMMEDIA_SKILL,		//����书0->�Ҽ��书��1,2...-> F1,F2...����书
	UOC_ITEM_TAKE_WITH,		//����Я��
	UOC_ITEM_TAKE_WITH_EX,
	UOC_TO_BE_TRADE,		//Ҫ�����������������
	UOC_OTHER_TO_BE_TRADE,	//��������ϣ�����Ҫ�����Լ��ģ�
	UOC_EQUIPTMENT,			//����װ��
	UOC_NPC_SHOP,			//npc��������
	UOC_STORE_BOX,
	UOC_STORE_BOX1,//������
	UOC_STORE_BOX2,
	UOC_STORE_BOX3,
	UOC_STORE_BOX4,
	UOC_STORE_BOX5,
	UOC_SKILL_LIST,			//�г�ȫ��ӵ�м��ܵĴ��ڣ����ܴ���
	UOC_SKILL_TREE,
	UOC_ITEM_GIVE,
	UOC_COMPOUND,
	UOC_COMPOUND_BOX,
	//���ҿ��ü�����
};


enum MONEYUNIT
{
	moneyunit_money = 0,	// ͷ
	moneyunit_extpoint,		// ����
	moneyunit_fuyuan,		// ����
	moneyunit_repute,	// ����
	moneyunit_accum,
	moneyunit_honor,
	moneyunit_respect,
	moneyunit_num,
};
//==================================
// iCoreShell::GetGameData��������,uDataIdȡֵΪGDI_TRADE_DATAʱ��
// uParam������ȡֵ�б�
// ע���е�Return:�б�ʾ��ص�GetGameData���õķ���ֵ�ĺ���
//==================================
enum UI_TRADE_OPER_DATA
{
	UTOD_IS_WILLING,		//�Ƿ�������(������)
	//Return: �����Լ��Ƿ��ڽ����еĲ���ֵ
	UTOD_IS_LOCKED,			//�Լ��Ƿ���������״̬
	//Return: �����Լ��Ƿ���������״̬�Ĳ���ֵ
	UTOD_IS_TRADING,		//�Ƿ�������ڵȴ����ײ����������Ƿ���ȷ����
	//Return: �����Ƿ����ڵȴ����ײ����������Ƿ���ȷ����
	UTOD_IS_OTHER_LOCKED,	//�Է��Ƿ��Ѿ���������״̬
	//Return: ���ضԷ��Ƿ��Ѿ���������״̬�Ĳ���ֵ
	UTOD_IS_OTHER_TRADING,
};

//==================================
//	������Ʒ
//==================================
struct KUiItemBuySelInfo
{
	int				nItemNature;
	char			szItemName[64];	//��Ʒ����
	int				nOldPrice;
	int				nCurPrice;			//������Ǯ����ֵΪ���۸񣬸�ֵ��ʾ����ļ۸�Ϊ(-nPrice)
	int				nMoneyUnit;
	bool			bNewArrival;
};

//==================================
//	��ʾĳ����Ϸ��������������Χ����Ϣ�ṹ
//==================================
struct KUiObjAtRegion
{
	KUiGameObject	Obj;
	KUiRegion		Region;
};

struct KUiObjAtContRegion : public KUiObjAtRegion
{
	union
	{
		UIOBJECT_CONTAINER	eContainer; 
		int					nContainer;
	};
};

struct KUiMsgParam
{
	unsigned char	eGenre;	//ȡֵ��ΧΪö������MSG_GENRE_LIST,��MsgGenreDef.h�ļ�
	unsigned char	cChatPrefixLen;
	unsigned short	nMsgLength;
	char			szName[32];
#define	CHAT_MSG_PREFIX_MAX_LEN	16
	unsigned char	cChatPrefix[CHAT_MSG_PREFIX_MAX_LEN];
};

struct KUiInformationParam
{
	char	sInformation[MAX_MESSAGE_LENGTH];	//��Ϣ��������
	short	nInforLen;			//��Ϣ�������ݵĴ洢����
	BOOL	bNeedConfirmNotify;
};

enum PLAYER_ACTION_LIST
{
	PA_NONE = 0,	//�޶���
	PA_RUN  = 0x01,	//��
	PA_SIT  = 0x02,	//����
	PA_RIDE = 0x04,	//�����
};
//==================================
//	ϵͳ��Ϣ����
//==================================
enum SYS_MESSAGE_TYPE
{
	SMT_NORMAL = 0,	//���μӷ������Ϣ
	SMT_SYSTEM,		//ϵͳ���������
	SMT_PLAYER,		//������
	SMT_TEAM,		//������
	SMT_FRIEND,		//����������
	SMT_MISSION,	//�������
	SMT_CLIQUE,		//�������
	SMT_TRADE,		//�������
	SMT_WAR,		//�������
};

//==================================
//	ϵͳ��Ϣ��Ӧ��ʽ
//==================================
enum SYS_MESSAGE_CONFIRM_TYPE
{
	SMCT_NONE,				//�ڶԻ���Ϣ����ֱ���ӹ�������Ҫ��Ӧ��
	SMCT_CLICK,				//���ͼ�������ɾ����
	SMCT_MSG_BOX,			//���ͼ��󵯳���Ϣ��
	SMCT_UI_RENASCENCE,		//ѡ������
	SMCT_UI_ATTRIBUTE,		//������ҳ��
	SMCT_UI_SKILLS,			//�򿪼���ҳ��
	SMCT_UI_ATTRIBUTE_SKILLS,//������ҳ�漼��ҳ��
	SMCT_UI_TEAM_INVITE,	//��Ӧ��ܾ�������������,
	//						pParamBuf ָ��һ��KUiPlayerItem�ṹ�����ݣ���ʾ������(�ӳ�)
	SMCT_UI_TEAM_APPLY,		//��Ӧ��ܾ�������������,
	//						pParamBuf ָ��һ��KUiPlayerItem�ṹ�����ݣ���ʾ������
	SMCT_UI_TEAM,			//�򿪶���������
	SMCT_UI_INTERVIEW,		//������Ի�����,
	//						pParamBuf ָ��һ��KUiPlayerItem�ṹ�����ݣ���ʾ������Ϣ�ĺ���
	SMCT_UI_FRIEND_INVITE,	//��׼��ܾ����˼��Լ�Ϊ����
	//						pParamBuf ָ��һ��KUiPlayerItem�ṹ�����ݣ���ʾ���������������
	SMCT_UI_TRADE,			//��Ӧ��ܾ����׵�����,
	SMCT_UI_TRADE_FOLKGAME,
	SMCT_UI_SPAR,
	//						pParamBuf ָ��һ��KUiPlayerItem�ṹ�����ݣ���ʾ���������������
	SMCT_DISCONNECT,		//����
	SMCT_UI_TONG_JOIN,//��Ӧ��ܾ������������
	SMCT_UI_TONG_JOIN_APPLY,//��Ӧ��ܾ������������
};

//==================================
//	ϵͳ��Ϣ
//==================================
struct KSystemMessage
{
	char			szMessage[256];	//��Ϣ�ı�
	unsigned int	uReservedForUi;	//����ʹ�õ�������,core����0����
	unsigned char	eType;			//��Ϣ����ȡֵ����ö������ SYS_MESSAGE_TYPE
	unsigned char	byConfirmType;	//��Ӧ����
	unsigned char	byPriority;		//���ȼ�,��ֵԽ�󣬱�ʾ���ȼ�Խ��
	unsigned char	byParamSize;	//����GDCNI_SYSTEM_MESSAGE��Ϣ��pParamBuf��ָ�����������ռ�Ĵ�С��
};

//==================================
//	����Ƶ��������
//==================================
struct KUiChatChannel
{
	int			 nChannelNo;
	unsigned int uChannelId;
	union
	{
		int		 nChannelIndex;
		int		 nIsSubscibed;	//�Ƿ񱻶���
	};
	char		 cTitle[32];
};

//==================================
//	������ѵ�һ���������Ϣ
//==================================
struct KUiChatGroupInfo
{
	char	szTitle[32];	//���������
	int		nNumFriend;		//���ں��ѵ���Ŀ
};

//==================================
//	���ѷ��������컰��
//==================================
struct KUiChatMessage
{
	unsigned int uColor;
	short	nContentLen;
	char	szContent[256];
};

//==================================
//	���ǵ�һЩ���ױ������
//==================================
struct KUiPlayerBaseInfo
{
	char	Name[32];	//����
	char	szTongName[32];
	char	szMateName[32];
	char	Title[128];	//�ƺ�
	int		nCurFaction;// ��ǰ�������� id �����Ϊ -1 ����ǰû����������
	int		nMissionGroup;// ��ǰ�������� id �����Ϊ -1 ����ǰû����������
	int		nRoomId;
	int		nRankInWorld;//��������ֵ,ֵΪ0��ʾδ��������
	int		nRankInSect;
	int		nPKValue;
	int		nRepute;
	int		nFuYuan;
	int		nTranslife;

	char	StatusDesc[16];		//״̬����
	unsigned int nCurTong;// ��ǰ�������name id �����Ϊ 0 ����ǰû���ڰ�����
};

//==================================
//	���ǵ�һЩ�ױ������
//==================================
struct KUiPlayerRuntimeInfo
{
	int		nLifeFull;			//������ֵ
	int		nLife;				//����
	int		nManaFull;			//������ֵ
	int		nMana;				//����
	int		nStaminaFull;		//������ֵ
	int		nStamina;			//����
	int		nExperienceFull;	//������ֵ
	int		nExperience;		//��ǰ����ֵ
	int		nCurLevelExperience;//��ǰ����������Ҫ�ľ���ֵ

	unsigned char	byAction;	//���ڽ��е���Ϊ������Ϊö��PLAYER_ACTION_LISTȡֵ�����
};

//==================================
//	���ǵ�һЩ������������
//==================================
enum UI_PLAYER_ATTRIBUTE
{
	UIPA_STRENGTH = 0,			//����
	UIPA_DEXTERITY,				//����
	UIPA_VITALITY,				//����
	UIPA_ENERGY,				//����
};

//==================================
//	���ǵ�һЩ�ױ����������
//==================================
struct KUiPlayerAttribute
{
	int		nMoney;				//����
	int		nLevel;				//�ȼ�
	char	StatusDesc[16];		//״̬����

	int		nBARemainPoint;		//��������ʣ�����
	int		nStrength;			//����
	int		nDexterity;			//����
	int		nVitality;			//����
	int		nEnergy;			//����

	int		nKillMAX;			//���ɱ����
	int		nKillMIN;			//��Сɱ����
	int		nRightKillMax;		//�Ҽ����ɱ����
	int		nRightKillMin;		//�Ҽ���Сɱ����

	int		nLeftAttack;			//������
	int		nRightAttack;			//������
	int		nDefence;			//������
	int		nMoveSpeed;			//�ƶ��ٶ�
	int		nAttackSpeed;		//�����ٶ�
	int		nCastSpeed;

	int		nPhyDef;			//��������
	int		nPhyDefPlus;			//��������
	int		nCoolDef;			//��������
	int		nCoolDefPlus;			//��������
	int		nLightDef;			//�������
	int		nLightDefPlus;			//�������
	int		nFireDef;			//�������
	int		nFireDefPlus;			//�������
	int		nPoisonDef;			//���ط���
	int		nPoisonDefPlus;			//���ط���

	int		nPKValue;
	int		nRepute;
	int		nFuYuan;
	int		nTranslife;
	int		nRankInWorld;
	char	Title[128];	//�ƺ�
};

//==================================
//	���ǵ�����ʹ����Ʒ���书
//==================================
struct KUiPlayerImmedItemSkill
{
	KUiGameObject	ImmediaItem[IMMEDIACY_ROOM_WIDTH];
	KUiGameObject	IMmediaSkill[2];
};

//==================================
//	����װ��������λ��
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
//	���������
//==================================
struct KUiSkillData : public KUiGameObject
{
	union
	{
		int		nLevel;
		int		nData;
	};
};

//==================================
//	һ����������������Ա����Ŀ
//==================================
#define	PLAYER_TEAM_MAX_MEMBER	8

//==================================
//	ͳ˧������ص�����
//==================================
struct KUiPlayerLeaderShip
{
	int		nLeaderShipLevel;			//ͳ˧���ȼ�
	int		nLeaderShipExperience;		//ͳ˧������ֵ
	int		nLeaderShipExperienceFull;	//�����¼���Ҫ�ľ���ֵ
};

//==================================
//	һ����ҽ�ɫ��
//==================================
struct KUiPlayerItem
{
	char			Name[32];	//��ҽ�ɫ����
	unsigned int	uId;		//��ҽ�ɫid
	int				nIndex;		//��ҽ�ɫ����
	int				nData;		//�������ص�һ����ֵ������������Ӧ��λ���й�
	int				nTeamID;
};

//==================================
//	�����Ϣ������
//==================================
struct KUiTeamItem
{
	KUiPlayerItem	Leader;
};

//==================================
//	������Ϣ
//==================================
struct KUiPlayerTeam
{
	bool			bTeamLeader;			//����Լ��Ƿ�ӳ�
	char			cNumMember;				//��Ա��Ŀ
	char			cNumTojoin;				//���������Ա����Ŀ
	bool			bOpened;				//�����Ƿ����������˼���
	int				nModePick;
	bool			bPKFollowCaptain;				//�����Ƿ����������˼���
	int				nTeamServerID;			//�����ڷ������ϵ�id�����ڱ�ʶ�ö��飬-1 Ϊ��
	int				nCaptainPower;
};

//==================================
//	ħ������
//==================================
#ifndef MAGICATTRIB
#define MAGICATTRIB
struct KMagicAttrib
{
	int				nAttribType;					//��������
	int				nValue[3];						//���Բ���
	KMagicAttrib(){nValue[0] = nValue[1] = nValue[2] = nAttribType = 0;};
};

struct KMagicAutoSkill
{
	int				nSkillId;
	int				nSkillLevel;
	int				nRate;						//���Բ���
	DWORD			dwNextCastTime;
	int				nWaitCastTime;
	KMagicAutoSkill(){nSkillId = nSkillLevel = nRate = dwNextCastTime = nWaitCastTime = 0;};
};
#else
struct KMagicAttrib;
struct KMagicAutoSkill;
#endif


struct KStateControl
{
	int				nSkillId;
	int				nLeftTime;
};

struct KMapPos
{
	int		nSubWorld;
	int		nRegion;
	int		nMapX;
	int		nMapY;
	int		nOffX;
	int		nOffY;
};

//==================================
//	ѡ��������
//==================================
enum OPTIONS_LIST
{
	OPTION_PERSPECTIVE,		//͸��ģʽ  nParam = (int)(bool)bEnable �Ƿ���
	OPTION_DYNALIGHT,		//��̬��Ӱ	nParam = (int)(bool)bEnable �Ƿ���
	OPTION_MUSIC_VALUE,		//��������	nParam = ������С��ȡֵΪ0��-10000��
	OPTION_SOUND_VALUE,		//��Ч����	nParam = ������С��ȡֵΪ0��-10000��
	OPTION_BRIGHTNESS,		//���ȵ���	nParam = ���ȴ�С��ȡֵΪ0��-100��
	OPTION_WEATHER,			//����Ч������ nParam = (int)(bool)bEnable �Ƿ���
};

//==================================
//	�����ĵ���ʱ�价����Ϣ
//==================================

struct KUiSceneTimeInfo
{
	char	szSceneName[80];		//������
	int		nSceneId;				//����id
	int		nScenePos0;				//������ǰ���꣨����
	int		nScenePos1;				//������ǰ���꣨�ϣ�
	int		nGameSpaceTime;			//�Է���Ϊ��λ
};


struct KUiSceneTimeInfoOften : public KUiSceneTimeInfo
{
};

struct KMapTraffic
{
	char	Content[80];
	int		Kind;
	POINT	Point;
	POINT	TPoint;
	int		MapIndex;
	void	Release() {
	memset(Content, 0, sizeof(Content));
	Kind = -1;
	Point.x = Point.y = TPoint.x = TPoint.y = MapIndex = 0;
	};
};
//==================================
//	��Դ��Ϣ
//==================================
//������ʾ����ά������
struct KPosition3
{
	int nX;
	int nY;
	int nZ;
};

struct KLightInfo
{
	KPosition3 oPosition;			// ��Դλ��
	DWORD dwColor;					// ��Դ��ɫ������
	long  nRadius;					// ���ð뾶
};


//С��ͼ����ʾ������
enum SCENE_PLACE_MAP_ELEM
{ 
	SCENE_PLACE_MAP_ELEM_NONE		= 0x00,		//�޶���
	SCENE_PLACE_MAP_ELEM_PIC		= 0x01,		//��ʾ����ͼ
	SCENE_PLACE_MAP_ELEM_CHARACTER	= 0x02,		//��ʾ����
	SCENE_PLACE_MAP_ELEM_PARTNER	= 0x04,		//��ʾͬ������
};

//�����ĵ�ͼ��Ϣ
struct KSceneMapInfo
{
	int	nScallH;		//��ʵ��������ڵ�ͼ�ĺ���Ŵ����
	int nScallV;		//��ʵ��������ڵ�ͼ������Ŵ����
	int	nFocusMinH;
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
	relation_none	= 1,
	relation_self	= 2,
	relation_ally	= 4,
	relation_enemy	= 8,
	relation_dialog	= 16,
	relation_all	= relation_none | relation_ally | relation_enemy | relation_self | relation_dialog,	
	relation_num,
};

enum NPCKIND
{
	kind_normal = 0	,		
	kind_player,
	kind_partner,
	kind_dialoger,	//�Ի���
	kind_bird,
	kind_mouse,
    kind_num
};

enum	// �������
{
	Obj_Kind_MapObj = 0,		// ��ͼ�������Ҫ���ڵ�ͼ����
	Obj_Kind_Body,				// npc ��ʬ��
	Obj_Kind_Box,				// ����
	Obj_Kind_Item,				// ���ڵ��ϵ�װ��
	Obj_Kind_Money,				// ���ڵ��ϵ�Ǯ
	Obj_Kind_LoopSound,			// ѭ����Ч
	Obj_Kind_RandSound,			// �����Ч
	Obj_Kind_Light,				// ��Դ��3Dģʽ�з���Ķ�����
	Obj_Kind_Door,				// ����
	Obj_Kind_Trap,				// ����
	Obj_Kind_Prop,				// С���ߣ�������
	Obj_Kind_Obstacle,
	Obj_Kind_Num,				// �����������
};

//�������ݵ�λ��һЩ�ؼ�������
enum PLAYER_BRIEF_PROP
{
	PBP_LEVEL = 1,	//�Ǽ��仯	nParam��ʾ��ǰ�ȼ�
	PBP_FACTION,	//����		nParam��ʾ�������ԣ����nParamΪ-1��ʾû������
	PBP_CLIQUE,		//����		nParamΪ��0ֵ��ʾ���˰��ɣ�0ֵ��ʾ�����˰���
};

//������Ϣ�����Ͷ���
enum NEWS_MESSAGE_TYPE
{
	NEWSMESSAGE_NORMAL,			//һ����Ϣ����ʾ��һ�Σ�����Ϣ������
								//��ʱ�����
	NEWSMESSAGE_COUNTING,		//���ƣ��룩����Ϣ��������0ʱ������Ϣ�������ˡ�
								//ʱ������е����ݽṹ�н���������Ч������������Ϊ��λ��
	NEWSMESSAGE_TIMEEND,		//��ʱ��Ϣ����ʱ��ʱ����Ϣ�������ˣ�����ÿ�������ʾһ�Ρ�
								//ʱ�������ʾ������ָ��ʱ�䡣
};

struct KNewsMessage
{
	int		nType;						//��Ϣ����
	char	sMsg[MAX_MESSAGE_LENGTH];	//��Ϣ����
	int		nMsgLen;					//��Ϣ���ݴ洢����
};

struct KRankIndex
{
	bool			bValueAppened;	//ÿһ���Ƿ���û�ж�������
	bool			bSortFlag;		//ÿһ���Ƿ���û���������
	unsigned short	usIndexId;		//������ID��ֵ
};

#define MAX_RANK_MESSAGE_STRING_LENGTH 128

struct KRankMessage
{
	char szMsg[MAX_RANK_MESSAGE_STRING_LENGTH];	// ��������
	unsigned short		usMsgLen;				// �������ݵĳ���
	short				cSortFlag;				// ���ֵ��QOO_RANK_DATA��ʱ���ʾ����������ֵ��ʾ������ֵ��ʾ����0ֵ��ʾλ��δ��
	int					nValueAppend;			// �������ֵ

};

struct KMissionRecord
{
	char			szHeader[32];
	int				nHeaderLen;
	int				nRecordAct;
	char			szContent[MAX_SCIRPTACTION_BUFFERNUM];	//�洢������ʾ��Ϣ�Ļ����������ַ���Ϊ���Ʒ��Ѿ�������ַ�����
	int				nContentLen;	//sContent����Ч���ݵĳ���(��λ���ֽ�)���������һ��������256�ֽ�
	unsigned int	uValue;			//������ֵ
};

enum TEAM_FIGURE
{
	TEAM_CAPTAIN,				// �����Ա���� �ӳ�
	TEAM_MEMBER,				// �����Ա���� ��Ա
};

enum CHATROOM_FIGURE
{
	CHATROOM_NOTHING,
	CHATROOM_MEMBER,				// �����Ա���� ��Ա
	CHATROOM_HOST,				// �����Ա���� �ӳ�
};
//---------------------------- ������ ------------------------

#define		defTONG_MAX_DIRECTOR				7
#define		defTONG_MAX_MANAGER					56
#define		defTONG_ONE_PAGE_MAX_NUM			7
#define		defTONG_NOTIFY_LENGTH				256
#define		defTONG_NAME_LENGTH_32				32
#define		defTONG_NAME_LENGTH_12				12
#define		defTONG_EFF_LASTWEEK				1
#define		defTONG_EFF_THISWEEK				2
#define		defTONG_EFF_USEABLE					3

enum TONG_MEMBER_FIGURE
{
	enumTONG_FIGURE_MEMBER,				// ����
	enumTONG_FIGURE_MANAGER,			// �ӳ�
	enumTONG_FIGURE_DIRECTOR,			// ����
	enumTONG_FIGURE_MASTER,				// ����
	enumTONG_FIGURE_NUM,
};

enum
{
	enumTONG_APPLY_INFO_ID_SELF,		// �����ѯ������Ϣ
	enumTONG_APPLY_INFO_ID_MASTER,		// �����ѯ������Ϣ
	enumTONG_APPLY_INFO_ID_DIRECTOR,	// �����ѯ������Ϣ
	enumTONG_APPLY_INFO_ID_MANAGER,		// �����ѯ�ӳ���Ϣ
	enumTONG_APPLY_INFO_ID_MEMBER,		// �����ѯ������Ϣ(һ������)
	enumTONG_APPLY_INFO_ID_ONE,			// �����ѯĳ����Ա��Ϣ(һ������)
	enumTONG_APPLY_INFO_ID_TONG_HEAD,	// �����ѯĳ�����Ϣ���������������
	enumTONG_APPLY_INFO_ID_NUM,
};

enum
{
	enumTONG_CREATE_ERROR_ID1,	// Player[m_nPlayerIndex].m_nIndex <= 0
	enumTONG_CREATE_ERROR_ID2,	// ���׹�����
	enumTONG_CREATE_ERROR_ID3,	// ���������
	enumTONG_CREATE_ERROR_ID4,	// �����Ӫ����
	enumTONG_CREATE_ERROR_ID5,	// �Ѿ��ǰ���Ա
	enumTONG_CREATE_ERROR_ID6,	// �Լ�����Ӫ����
	enumTONG_CREATE_ERROR_ID7,	// �ȼ�����	
	enumTONG_CREATE_ERROR_ID8,	// Ǯ����
	enumTONG_CREATE_ERROR_ID9,	// ��Ӳ��ܽ����
	enumTONG_CREATE_ERROR_ID10,	// ���ģ�����
	enumTONG_CREATE_ERROR_ID11,	// �����ַ�������
	enumTONG_CREATE_ERROR_ID12,	// �����ַ�������
	enumTONG_CREATE_ERROR_ID13,	// ���ͬ������
	enumTONG_CREATE_ERROR_ID14,	// ������ʧ��
	enumTONG_CREATE_ERROR_ID15,	// ������ʧ��
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

//������Ϣ
struct KTongInfo
{
	char  szName[defTONG_NAME_LENGTH_32];			//�������
	char  szMasterName[defTONG_NAME_LENGTH_32];		//��������
	int	  nTongJiyuParam;
	char  szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
	int   nFaction;				//�����Ӫ
	int   nMoney;				//����ʽ�
	BYTE  btLevel;
	DWORD dwTotalEff;
	int	  nMasterJoinTm;
	int   nMasterSaveEff;
	BOOL  bRecruit;
	int	  nMemberCount;         //��������
	int	  nManagerCount;
	int	  nDirectorCount;
};

struct KTongParam
{
	int		m_nLevel;
	int		m_nLeadLevel;
	int		m_nMoney;
	int		m_nItemCount;
	int		m_nItemGenre;
	int		m_nItemDetail;
	int		m_nRepute;
	int		m_nFuYuan;
	int		m_nLeaveMoney;
	int		m_nLeaveRepute;
	int		m_nLeaveTime;
	int		m_nMoneyChangeCamp;
	int		m_nMoneySaveJiyu;
};

//�԰��ڳ�Ա���Ķ�������ı��Լ�����Ĺ�ϵ�Ķ���
enum TONG_ACTION_TYPE
{
	TONG_ACTION_DISMISS,       //����
	TONG_ACTION_ASSIGN,        //����
	TONG_ACTION_DEMISE,        //��λ
	TONG_ACTION_LEAVE,         //���
	TONG_ACTION_RECRUIT,       //����״̬
	TONG_ACTION_APPLY,         //�������
	TONG_ACTION_CHANGE_AGNAME,
	TONG_ACTION_CHANGE_MALE_AGNAME,
	TONG_ACTION_CHANGE_FEMALE_AGNAME,
	TONG_ACTION_CHANGE_CAMP_JUSTIE,
	TONG_ACTION_CHANGE_CAMP_EVIL,
	TONG_ACTION_CHANGE_CAMP_BALANCE,
};


enum TONG_MONEY_ACTION_TYPE
{
	TONG_ACTION_SAVE,       //����
	TONG_ACTION_GET,        //����
};

//�԰��ڳ�Ա���Ķ�������ı��Լ�����Ĺ�ϵ�Ķ���ʱ�Ĳ���
struct KTongOperationParam
{
	TONG_ACTION_TYPE	eOper;				//������
	int                 nData[4];
	char				szData[32];		//��������Ҫ�����룬�����Ӧ�Ĳ������Ҫ���룬��ֱ��������Ϊ�ա�
};

//����Ա��
struct KTongMemberItem : public KUiPlayerItem
{
	char szAgname[defTONG_NAME_LENGTH_32];	//�ڰ��ڵ�ְ��/�ƺ�
	BOOL bOnline;
	int  nSaveEff;
	int	 nJoinTm;
};

//ĳ�������XX�Ĺ�ϵ��(XX�����ǰ�ᣬ����ȵ�)
struct KUiPlayerRelationWithOther : KUiPlayerItem
{
	int		nRelation;
	int		nParam;
};

//ͨ�õĴ�����������Ϸ����Ľṹ
struct KUiGameObjectWithName
{
	char			szName[32];
	char			szString[32];
	int				nData;
	int				nParam;
	unsigned int 	uParam;
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
	enumFacTopTenRicher = enumFacTopTenMasterHand + (MAX_FACTION+1),
	enumLadderEnd = enumFacTopTenRicher + (MAX_FACTION+1),
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
	char	lpszRoomHost[32];
	char	lpszRoomName[32];
	BYTE	byRoomMemNum;
	int		nRoomLeftTime;
	int		nRoomParam;
};

struct KOneMem
{
	char	lpszName[32];
	int		nMemParam;
	int		nMemIndex;
	BOOL	bIsRoomHost;
};

struct KLockItem
{
	KLockItem() {Clear();};
	~KLockItem() {Clear();};
	short nState;
	DWORD dwLockTime;
	void Clear() {
		nState = LOCK_STATE_NORMAL;
		dwLockTime = 0;
	};
	BOOL IsLock() {
		return (nState!=LOCK_STATE_NORMAL);
	};
};

#define MAX_DATAGROUP_VALUE 4
struct KDataGroup
{
	int		nNameId;
	int		nValue[MAX_DATAGROUP_VALUE];
	char	szName1[32];
	char	szName2[32];
	void Clear() {
		nNameId = 0;
		memset(nValue, 0, sizeof(nValue));
		memset(szName1, 0, sizeof(szName1));
		memset(szName2, 0, sizeof(szName2));
	}
};

#define MAX_MISSION_PARAM			16
#define MAX_GLBMISSION_PARAM		3
#define MISSION_STATNUM				10
#define MISSION_PARAM_AVAILABLE		0
#define MISSION_AVAILABLE_VALUE		1
struct TMissionLadderSelfInfo
{
	char		  szMissionName[64];
	unsigned char ucGroup;
	int			  nGlbParam[MAX_GLBMISSION_PARAM];
	int			  nParam[MAX_MISSION_PARAM];
	void Clear() {
		memset(szMissionName, 0, sizeof(szMissionName));
		memset(nGlbParam, 0, sizeof(nGlbParam));
		memset(nParam, 0, sizeof(nParam));
	};
};

struct TMissionLadderInfo
{
	unsigned char ucGroup;
	char		  Name[32];
	int			  nParam[MAX_MISSION_PARAM];
	void Clear() {
		ucGroup = 0;
		memset(Name, 0, sizeof(Name));
		memset(nParam, 0, sizeof(nParam));
	};
};

#define	MAX_SUPERSHOP_SHOPTAB 10
typedef struct
{
	int		m_nCurShop;
	int		m_nShopIdx[MAX_SUPERSHOP_SHOPTAB];
	int		m_nShopNum;
	int		m_nMoneyUnit;
	DWORD	m_SubWorldID;
	int		m_nMpsX;
	int		m_nMpsY;
	void	Clear() {
		memset(m_nShopIdx, -1, sizeof(m_nShopIdx)); 
		m_nMoneyUnit = moneyunit_money; m_SubWorldID = -1; m_nMpsX = 0; m_nMpsY = 0;
	}
} BuySellInfo;


struct PLAYERTRADE
{
	char cName[32];
	BOOL nTrade;
	int nDest;
	void Release() {
		memset(cName, 0, sizeof(cName));
		nTrade = FALSE;
		nDest = 0;
	};
};

typedef struct
{
	char	szName[64];
	DWORD	dwColor;
	int		nStateGraphics;
	DWORD	dwLeftTime;
	void	Release() {
		memset(szName, 0, sizeof(szName));
		dwColor = 0;
		dwLeftTime = 0;
		nStateGraphics = 0;
	};
} KExpandRank;

struct	LockMoveItem
{
	BOOL bLock;
	int	nPlace;
	void	Release() {
		bLock = FALSE;
		nPlace = 0;
	};
};


#define		MAX_AUTO_SIZE				50
#define		MAX_AUTO_LIST				15
#define		MAX_TIME_OVERLOOK			100
#define		MAX_TIME_RESET_OVERLOOK		10000
#define		MAX_TIME_SPACE				1500
#define		MAX_AUTO_STATESKILL			3
#define		MAX_AUTO_FILTERL			40

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
