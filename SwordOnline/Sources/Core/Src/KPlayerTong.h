//---------------------------------------------------------------------------
// Sword3 Engine (c) 2003 by Kingsoft
//
// File:	KPlayerTong.h
// Date:	2003.08.12
// Code:	边城浪子
// Desc:	KPlayerTong Class
//---------------------------------------------------------------------------


#ifndef KPLAYERTONG_H
#define KPLAYERTONG_H

class KPlayerTong
{
	friend class KPlayer;
private:
	int			m_nPlayerIndex;		// 指向对应的player数组的位置
	int			m_nApplyTo;			// 如果已经向某个帮会申请加入，帮主的 playerindex

public:
	DWORD		m_dwTongNameID;		// 帮会名字符串转换成的 dword
	int			m_nFlag;			// 是否加入帮会
	int			m_nFigure;			// 帮会中的身份
	int			m_nCamp;			// 帮会阵营
	BYTE		m_btDirectorNum;
	BYTE		m_btManagerNum;
	DWORD		m_dwMemberNum;
	DWORD		m_dwLeaveTime;
	BYTE		m_btLevel;
	char		m_szName[defTONG_NAME_LENGTH_32];		// 帮会名称
	char		m_szMasterName[defTONG_NAME_LENGTH_32];	// 帮主名字
	char		m_szAgname[defTONG_NAME_LENGTH_32];		// 当前称号
	int			m_nTongJiyuParam;
	char 		m_szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
	DWORD		m_dwMoney;
	DWORD		m_dwTotalEff;
	int 		m_nSaveEff;
	BOOL		m_bRecruit;
	int 		m_nTongParam;
	int			m_nJoinTm;
#ifdef _SERVER
	int			m_nTongNationalEmblem;
#endif
private:
	BOOL		CheckAcceptAddApplyCondition();	// 判断是否可以转发别人的加入帮会申请

public:
	// 初始化
	void		Init(int nPlayerIdx);
	void		Clear();

	void		GetTongName(char *lpszGetName);
	void		GetTongAgname(char *lpszGetAgname);
	DWORD		GetTongNameID();
	void		SetTongNameID(DWORD dwID) { m_dwTongNameID = dwID; };
	int			CheckIn() {return m_nFlag;};
	// 是否有权力查询队长信息
	BOOL		CanGetManagerInfo(DWORD dwTongNameID);
	// 是否有权力查询帮众信息
	BOOL		CanGetMemberInfo(DWORD dwTongNameID);

#ifndef _SERVER
	// 申请创建帮会
	int			ApplyCreateTong(int nCamp, char *lpszTongName);
	void		ForceCreateTong(BYTE* pMsg);
	// 得到服务器通知创建帮会
	void		Create(TONG_CREATE_SYNC *psCreate);
	// 申请加入帮会
	BOOL		ApplyAddTong(DWORD dwNpcID);
	// 是否接受成员 bFlag == TRUE 接受 == FALSE 不接受
	void		JoinTongReply(int nNpcIdx);

	void		s2cJoinTong(BYTE* pMsg);
	void		s2cJoinTongReply(BYTE* pMsg);

	void		AcceptMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag);
	void		ReplyMember(int nPlayerIdx, DWORD dwNameID, BOOL bFlag);
	// 加入帮会，成为普通帮众
	BOOL		AddTong(int nCamp, char *lpszTongName, char *lpszAgname, char *lpszMaster);
	// 申请任命
	BOOL		ApplyInstate(int nCurFigure, int nCurPos, int nNewFigure, int nNewPos, char *lpszName);
	// 申请踢人
	BOOL		ApplyKick(int nCurFigure, int nCurPos, char *lpszName);
	// 申请传位
	BOOL		ApplyChangeMaster(int nCurFigure, int nPos, char *lpszName);
	// 申请离开帮会
	BOOL		ApplyLeave();

	BOOL		ApplySaveMoney(DWORD nMoney);

	BOOL		ApplyGetMoney(DWORD nMoney);

	BOOL		ApplyChangeJiyu(char* szMasterName, int nParam, char* szNotify);
	BOOL		ApplyRecruit(BOOL bRecruit);
	
	/* 申请获得帮会信息
	if nInfoID == enumTONG_APPLY_INFO_ID_SELF 不用参数
	if nInfoID == enumTONG_APPLY_INFO_ID_MASTER nParam1 帮会名id
	if nInfoID == enumTONG_APPLY_INFO_ID_DIRECTOR nParam1 帮会名id
	if nInfoID == enumTONG_APPLY_INFO_ID_MANAGER nParam1 帮会名id nParam2 从开始有效第几个 nParam3 连续几个
	if nInfoID == enumTONG_APPLY_INFO_ID_MEMBER nParam1 帮会名id nParam2 从开始有效第几个 nParam3 连续几个
	if nInfoID == enumTONG_APPLY_INFO_ID_ONE lpszName 人名
	if nInfoID == enumTONG_APPLY_INFO_ID_TONG_HEAD nParam1 NpcIndex
	*/
	BOOL		ApplyInfo(int nInfoID, int nParam1, int nParam2, int nParam3, char *lpszName = NULL);
	// 设定自身帮会信息
	void		SetSelfInfo(TONG_SELF_INFO_SYNC *pInfo);
	
	void        OpenCreateInterface();

	void		ChangeMoney(DWORD dwMoney) {m_dwMoney = dwMoney;};
	
	void		ChangeTongEff(DWORD dwTotalEff) {m_dwTotalEff = dwTotalEff;};
	
	BOOL		ApplyChangeAgname(int nCurFigure, int nCurPos, char *lpszName, char *lpszAgname);
	
	BOOL		ApplyChangeSexAgname(char *lpszAgname, int nSex);
	
	BOOL		ApplyTongChangeCamp(int nCamp);
	
	BOOL		ChangeCamp(int nCamp);
	BOOL		ChangeLevel(BYTE btLevel);
	BOOL		ChangeRecruit(BOOL bRecruit);
	BOOL		ChangeTongParam(int nTongParam);
	BOOL		ChangeJiyu(int nParam, char* szNotify);
	BOOL		ChangeEff(int nSaveEff, DWORD dwTotalEff);
#endif

#ifdef _SERVER
	// 判断创建帮会条件是否成立 if 成功 return == 0 else return error id
	int			CheckCreateCondition(int nCamp, char *lpszTongName);
	// 得到relay通知，帮会创建成功，处理相应数据
	BOOL		Create(int nCamp, char *lpszTongName);
	// 头上是否需要顶找人标志
	// 转发加入帮会申请给对方客户端
	BOOL		TransferAddApply(DWORD dwNpcID);
	// 发消息通知拒绝某人申请
	void		SendRefuseMessage(int nPlayerIdx, DWORD dwNameID);
	
	void		JoinTong(char* cTongName);
	void		JoinTongReply(BYTE* pProtocol);
	// 判断别人加入自己帮会条件是否成立
	BOOL		CheckJoinTong();
	BOOL		CheckAddCondition(int nPlayerIdx);
	BOOL		CheckAddConditionReply(int nPlayerIdx);
	// 加入帮会，成为普通帮众
	BOOL		AddTong(int nCamp, int nMemberNum, DWORD dwMoney, int nLevel, DWORD dwTotalEff, BOOL bRecruit, int nTongParam, int nTongJiyuParam, char *lpszTongName, char *lpszMasterName, char *lpszAgname);
	// 给客户端发送自己在帮会中的信息
	void		SendSelfInfo();
	// 检测是否有任命权利
	BOOL		CheckInstatePower(TONG_APPLY_INSTATE_COMMAND *pApply);
	// 被任命
	void		BeInstated(STONG_SERVER_TO_CORE_BE_INSTATED *pSync);
	// 检测是否有踢人权利
	BOOL		CheckKickPower(TONG_APPLY_KICK_COMMAND *pKick);
	// 被踢出帮会
	void		BeKicked(STONG_SERVER_TO_CORE_BE_KICKED *pSync);
	// 检测是否有离开权利
	BOOL		CheckLeavePower(TONG_APPLY_LEAVE_COMMAND *pLeave);

	BOOL		CheckMoneyPower(TONG_APPLY_SAVE_COMMAND *pSave);

	BOOL		CheckSaveEffPower(TONG_APPLY_SAVE_EFF_COMMAND *pSave);
	// 离开帮会
	void		Leave(STONG_SERVER_TO_CORE_LEAVE *pLeave);
	// 检测是否有权利换帮主
	BOOL		CheckChangeMasterPower(TONG_APPLY_CHANGE_MASTER_COMMAND *pChange);
	// 检测是否有能力接受传位
	BOOL		CheckGetMasterPower();
	// 传位导致身份改变
	BOOL		CheckGetMasterPower(STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER *pCheck);

	void		ChangeAs(STONG_SERVER_TO_CORE_CHANGE_AS *pAs);
	// 换帮主
	void		ChangeMaster(char *lpszMaster);
	// 登陆时候获得帮会信息
	void		Login(STONG_SERVER_TO_CORE_LOGIN *pLogin);

	void		DBSetTongNameID(DWORD dwID) { m_dwTongNameID = dwID; if (dwID) m_nFlag = 1;};

	void		ChangeMoney(DWORD dwMoney);

	void		ChangeTongEff(DWORD dwTotalEff);
	
	BOOL		CheckChangeAgnamePower(TONG_APPLY_CHANGE_AGNAME_COMMAND *pChange);

	BOOL		CheckGetAgnamePower(STONG_SERVER_TO_CORE_CHECK_GET_AGNAME_POWER *pCheck);	
	
	void		BeChangedAgname(STONG_SERVER_TO_CORE_BE_CHANGED_AGNAME *pSync);
	
	BOOL		CheckChangeSexAgnamePower(TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND *pChange);
	
	int			CheckChangeCampCondition(TONG_APPLY_CHANGE_CAMP_COMMAND *pChange);
	
	BOOL		CheckChangeRecruit(TONG_CHANGE_RECRUIT_COMMAND *pChange);
	
	BOOL		CheckChangeInfo(TONG_APPLY_CHANGE_INFO_COMMAND *pChange);
	
	void		BeChangedCamp(STONG_SERVER_TO_CORE_BE_CHANGED_CAMP *pSync);	
	void		BeChangedLevel(STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL *pSync);
	void		BeChangedRecruit(STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT *pSync);
	void		BeChangedTongParam(STONG_SERVER_TO_CORE_BE_CHANGED_TONGPARAM *pSync);
	void		BeChangedJiyu(STONG_SERVER_TO_CORE_BE_CHANGED_JIYU *pSync);
	void		BeChangedEff(STONG_SERVER_TO_CORE_BE_CHANGED_EFF *pSync);
#endif
};
#endif
