// DBBackup.h: interface for the CDBBackup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(DBBACKUP_H)
#define DBBACKUP_H

#include "DBTable.h"
#include "S3DBInterface.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define DEFAULTSTATNUM 10
#define LEVELSTATNUM 1000
#define MONEYSTATNUM 10
#define SECTLEVELSTATNUM 10
#define SECTMONEYSTATNUM 10
class CDBBackup  
{
public:
	struct TRoleList
	{
		char Name[20];
		//char Account[32];
		int Level;
		int	Translife;
		int Money;
		int Sect;			//门派
		int FightExp;
		int KillNum;
		int Repute;
		int	FuYuan;
		int	AccumStat1;
		int	AccumStat2;
		int	HonorStat;
		int	TimeStat;
		BYTE TongLv;
		int	TongMn;
		DWORD	TongEff;
		BYTE Sort;
	};
	struct TStatData
	{
		TRoleList LevelStat[LEVELSTATNUM];			//级别最多排名列表
		TRoleList MoneyStat[MONEYSTATNUM];			//金钱最多排名列表
		TRoleList KillerStat[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList ReputeStat[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList FuYuanStat[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList AccumStat1[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList AccumStat2[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList HonorStat[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList TimeStat[DEFAULTSTATNUM];
		TRoleList TongLvStat[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList TongMnStat[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList TongEffStat[DEFAULTSTATNUM];			//杀人最多排名列表
		TRoleList MoneyStatBySect[MAX_FACTION+1][SECTMONEYSTATNUM];	//各门派金钱最多排名列表[门派][玩家数]
		TRoleList LevelStatBySect[MAX_FACTION+1][SECTLEVELSTATNUM];	//各门派级别最多排名列表[门派][玩家数]
		int SectPlayerNum[MAX_FACTION+1];				//各个门派的玩家数
		int SectMoneyMost[MAX_FACTION+1];				//财富排名前MONEYSTATNUM玩家中各门派所占比例数
		int SectLevelMost[MAX_FACTION+1];				//级别排名前LEVELSTATNUM玩家中各门派所占比例数
		
	};
private:
	enum StatType{stMoney, stLevel, stKiller, stRepute, stFuYuan, stAccumStat1, stAccumStat2, stHonorStat, stTimeStat, stTongLv, stTongMn, stTongEff};//需要判断的类别（stMoney, stLevel）

	static TRoleList* GetMin(			//查找出列表中钱/级别（或其他）最少的一个元素
		TRoleList* const aRoleList,	//列表
		const int n,				//列表大小
		const StatType aType,
		const char* TongName = 0);		//需要判断的类别（stMoney, stLevel）

	static TRoleList* GetMax(			//查找出列表中钱/级别（或其他）最少的一个元素
		TRoleList* const aRoleList,	//列表
		const int n,				//列表大小
		const StatType aType,
		const char* TongName = 0);		//需要判断的类别（stMoney, stLevel）

	static void ListSort(			//对列表做特定的排序
		TRoleList* const aRoleList,	
		const int n,
		const StatType aType,
		const bool bSortZA = false);
	static int GetIndexByName(char* aName, TRoleList* aList, int aListSize);
	static void RoleDataCopy(TRoleList* Desc, TRoleData* Source, bool bTong= false);//把RoleData有用的结构复制到RoleList结构中
private:
	HANDLE m_hThread; 
	HANDLE m_hManualThread; 
	static void Backup();			//备份和写入统计数据
	static void MakeSendStatData();	//生成发送给客户端的统计数据结构
	static DWORD WINAPI TimerThreadFunc( LPVOID lpParam );	//备份计时线程
	static DWORD WINAPI ManualThreadFunc( LPVOID lpParam );	//手工备份线程
public:
	CDBBackup(char* aPath, char* aName, ZDBTable* aRunTable);
	virtual ~CDBBackup(){};
	bool Open(int aTime, DWORD bTime);		//启动备份计时
	bool Suspend();						//挂起
	bool Resume();						//继续执行线程
	bool Close();						//关闭
	TGAME_STAT_DATA GetSendStatData();	//取得发送给客户端的统计数据结构
	bool IsWorking();					//线程是否在工作
	bool IsBackuping();				//备份是否在工作
	bool ManualBackup();				//手工备份
	static void SaveStatInfo();			//把游戏世界等级排名写到指定玩家角色中
};

#endif // !defined(DBBACKUP_H)
