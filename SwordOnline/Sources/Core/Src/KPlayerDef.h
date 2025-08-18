#ifndef KPLAYERDEF_H
#define KPLAYERDEF_H

//数据库加载列表数据时每帧加载数量
#define DBLOADPERTIME_ITEM		10
#define DBLOADPERTIME_FRIEND	100
#define	DBLOADPERTIME_TASK		500



#define SCRIPT_PLAYERID				"PlayerId"		//脚本中的玩家Id全局变量名
#define SCRIPT_PLAYERINDEX			"PlayerIndex"	//脚本中的玩家的编号全局变量名
#define SCRIPT_OBJINDEX				"ObjIndex"		//触发脚本的物件的INDEX
#define SCRIPT_SUBWORLDINDEX		"SubWorld"		//脚本中当前世界的Index的全局变量名

#define	CLIENT_PLAYER_INDEX	1

#define		MAX_TEAM_APPLY_LIST		32		// 每个队伍的最大当前申请加入人数

//---------------------------------------------------------------------------
#define	QUEST_BEFORE	0x0000
#define	QUEST_START		0x0001
#define	QUEST_FINISHED	0xffff
//---------------------------------------------------------------------------
#ifndef _SERVER
enum MOUSE_BUTTON
{
	button_left  = 0,
	button_right,
};
#endif

enum SCRIPTACTION  
{
	SCRIPTACTION_UISHOW,			//显示某个UI界面
	SCRIPTACTION_EXESCRIPT,			//执行脚本
};
/*
enum UISEL_DATATYPE
{
	UISEL_nInfo_sOption,		//主信息以数字标识，选项为字符串
	UISEL_nInfo_sOption,		//主信息以数字标识，选项为数字标识	
	
	UISEL_sInfo_nOption,		
	UISEL_sInfo_sOption,
};
*/
enum
{
	ATTRIBUTE_STRENGTH = 0,
	ATTRIBUTE_DEXTERITY,
	ATTRIBUTE_VITALITY,
	ATTRIBUTE_ENGERGY,
	ATTRIBUTE_RESET_ALL,
};

enum	// 队伍创建失败原因
{
	Team_Create_Error_InTeam = 0,	// 已经在队伍中
	Team_Create_Error_SameName,		// 同名错误
	Team_Create_Error_Name,			// 队伍名字非法
	Team_Create_Error_TeamFull,		// 队伍数量太多，容纳不下
	Team_Create_Error_CannotCreate,	// 当前处于不能组队状态
	Team_Create_Error_NormalPK,	// 当前处于不能组队状态
};

enum	// 队伍创建失败原因
{
	ChatRoom_Create_Error_Name,
	ChatRoom_Create_Error_Pass,
	ChatRoom_Create_Error_SameName,		// 同名错误
	ChatRoom_Create_Error_SameHost,		// 队伍数量太多，容纳不下
	ChatRoom_Create_Error_RoomFull,		// 队伍数量太多，容纳不下
};

enum	// 队伍创建失败原因
{
	ChatRoom_Create_Success,
	ChatRoom_Join_Host,		// 同名错误
	ChatRoom_Join_Member,		// 同名错误
	ChatRoom_Join_Fail1,
	ChatRoom_Join_Fail2,
	ChatRoom_Join_Fail3,
	ChatRoom_Leave,		// 同名错误
	ChatRoom_Delete,
	ChatRoom_ForceLeave,
	ChatRoom_BeKick,		// 队伍数量太多，容纳不下
	ChatRoom_AddBlackList,
	ChatRoom_AddBlackList_Fail,
	ChatRoom_RemoveBlackList,
	ChatRoom_ChangePw,
};

enum SYNC_STEP
{
	STEP_BASE_INFO = 0,
	STEP_FIGHT_SKILL_LIST,
	STEP_STATE_SKILL_LIST,
	STEP_TASK_LIST,
	STEP_ITEM_LIST,
	STEP_SYNC_END
};

enum SAVE_STATUS
{
	SAVE_IDLE = 0,
	SAVE_REQUEST,
	SAVE_DOING,
};

enum LIXIAN_STATUS
{
	LIXIAN_IDLE = 0,
	LIXIAN_REQUEST,
	LIXIAN_DOING,
};
#endif
