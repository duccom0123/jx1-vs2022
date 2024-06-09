#ifndef	KSkillListH
#define	KSkillListH
//#include "Windows.h"
#include "GameDataDef.h"


typedef struct tagNpcSkill
{
	int		SkillId;
	int		SkillLevel;
	int		AddLevel;	
	int		CurrentSkillLevel;
	int		SkillExp;
	int		NextSkillExp;
	BOOL	TempSkill;
	int		MaxTimes;
	int		RemainTimes;
	DWORD	NextCastTime;		//下次可发送时的最小时间
	int		WaitCastTime;
#ifndef _SERVER
	DWORD	TotalTime;//tong thoi gian
#endif
} NPCSKILL, *LPNPCSKILL;
struct TDBPlayerData;
#ifndef TOOLVERSION
class KSkillList
#else 
class CORE_API KSkillList
#endif
{
public:
	NPCSKILL	m_Skills[MAX_NPCSKILL];//下标从1开始，0为无效index
	int			m_nAllSkillV;
	int			m_nNpcIndex;
public:
	KSkillList();
	~KSkillList();
	int			Add(int nSkillID, int nLevel = 1, int nExp = 0, BOOL bTempSkill = FALSE, int nMaxTimes = 0, int RemainTimes = 0);
	void		Remove(int nSkillID)
	{
		if (nSkillID <= 0 || nSkillID >= MAX_SKILL) 
			return ;
		int i = FindSame(nSkillID);
		if (i)
		{
			m_Skills[i].SkillId = 0;
			m_Skills[i].SkillLevel = 0;
			m_Skills[i].AddLevel = m_nAllSkillV;
			m_Skills[i].CurrentSkillLevel = 0;
			m_Skills[i].SkillExp = 0;
			m_Skills[i].NextSkillExp = 0;
			m_Skills[i].TempSkill = FALSE;
			m_Skills[i].MaxTimes = 0;
			m_Skills[i].RemainTimes = 0;
			m_Skills[i].NextCastTime = 0;
			m_Skills[i].WaitCastTime = 0;
		}

	};//Question :需要加！
	void		RemoveIdx(int nIdx)
	{
		if (nIdx <= 0 || nIdx >= MAX_NPCSKILL) 
			return ;

		m_Skills[nIdx].SkillId = 0;
		m_Skills[nIdx].SkillLevel = 0;
		m_Skills[nIdx].AddLevel = m_nAllSkillV;
		m_Skills[nIdx].CurrentSkillLevel = 0;
		m_Skills[nIdx].SkillExp = 0;
		m_Skills[nIdx].NextSkillExp = 0;
		m_Skills[nIdx].TempSkill = FALSE;
		m_Skills[nIdx].MaxTimes = 0;
		m_Skills[nIdx].RemainTimes = 0;
		m_Skills[nIdx].NextCastTime = 0;
		m_Skills[nIdx].WaitCastTime = 0;

	};//Question :需要加！
	int			GetSkillId(unsigned long ulSkillIdx) const
	{
		if (ulSkillIdx >= MAX_NPCSKILL || ulSkillIdx <= 0) 
			return 0;
		else 
			return m_Skills[ulSkillIdx].SkillId;
	};


	int	SetSkillLevelDirectlyUsingIndex(unsigned long ulSkillIdx, unsigned long ulLevel)/*请慎用该函数，因为目前未加任何被动技能降级对数值的影响*/
	{
		if (ulSkillIdx >= MAX_NPCSKILL ||  ulSkillIdx == 0 || ulLevel >= MAX_SKILLLEVEL)
			return 0;
		
		if (m_Skills[ulSkillIdx].SkillId)
		{
			m_Skills[ulSkillIdx].SkillLevel = ulLevel;
			m_Skills[ulSkillIdx].CurrentSkillLevel = m_Skills[ulSkillIdx].SkillLevel + m_Skills[ulSkillIdx].AddLevel;
			return 1;
		}	
		return 0;
	}

	int	SetSkillLevelDirectlyUsingId(unsigned long ulSkillId, unsigned long ulLevel)/*请慎用该函数，因为目前未加任何被动技能降级对数值的影响*/
	{
		if (ulSkillId >= MAX_SKILL || ulSkillId <= 0 ||  ulLevel >= MAX_SKILLLEVEL)
			return 0;
		unsigned long  ulSkillIdx = FindSame(ulSkillId);
		if (ulSkillIdx)
		{
			if (m_Skills[ulSkillIdx].SkillId)
			{
				m_Skills[ulSkillIdx].SkillLevel = ulLevel;
				m_Skills[ulSkillIdx].CurrentSkillLevel = m_Skills[ulSkillIdx].SkillLevel + m_Skills[ulSkillIdx].AddLevel;
				return 1;
			}	
		}
		return 0;
	}

	void		Clear(){memset(m_Skills, 0, sizeof(m_Skills));};
#ifndef _SERVER
	void		SetSkillLevel(int nId, int nLevel);
	BOOL		SetLevel(int nIndex, int nLevel);		// 把某个编号技能设为某一级
	void		SetAddLevel( int nId, int nAdd);
	void		SetCurLevel( int nId, int nAdd);
	BOOL		SetExp(int nIndex, int nExp);		// 把某个编号技能设为某一级
	int			GetSkillSortList(KUiSkillData *);		// 获得角色当前所有技能排序列表
	int			GetSkillPosition(int nSkillId);
	int			GetLeftSkillSortList(KUiSkillData*);	// 获得角色当前左键技能排序列表
	int			GetRightSkillSortList(KUiSkillData*);	// 获得角色当前右键技能排序列表
	int			GetNextSkillState(int nIndex = 0);
	int			GetNextSkillFight(int nIndex = 0);
	int			GetNextSkillAura(int nIndex = 0);
	int			FindSkillLifeReplenish();
#endif
	BOOL		IsBaseSkill(int nSkillId);
	void		AllSkillV(int nSkillId, int nLevel);
	void		SeriesSkillV(int nSeries, int nLevel);
	BOOL		IncreaseLevel(int nIdx, int nLvl);
	BOOL		IncreaseExp(int nIdx, int nExp);
	BOOL		SetNextExp(int nIdx, int nExp);
	BOOL		SetTempSkill(int nIdx, BOOL bTempSkill);
	int			GetLevel(int nSkillID);
	int			GetExp(int nSkillID);
	int			GetNextExp(int nSkillID);
	BOOL		IsTempSkill(int nSkillID);
	int			GetSkillIdxLevel(int nIdx) 
	{
		if (nIdx <= 0) return 0;
		return m_Skills[nIdx].SkillLevel;
	};
    int            GetSkillIdx(int nIdx)  
    { 
        if (nIdx <= 0) return 0; 
        for (int i = 1; i < MAX_NPCSKILL; i++) 
        { 
            if (m_Skills[i].SkillId == nIdx) 
				return i;
        }     
        return 0; 
    };
	int			GetCurrentLevel(int nSkillID);
	int			GetCurrentLevelIndex(int nListIndex)
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return m_Skills[nListIndex].CurrentSkillLevel;
		else 
			return 0;
	};
	int			GetCount();
	int			FindSame(int nSkillID);
	int			FindFree();
	BOOL		CanCast(int nSkillID, DWORD dwTime);
	void		SetNextCastTime(int nSkillID, DWORD dwCurrentTime, DWORD dwNextCastTime);
	int			GetSkillId(int nListIndex)
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return m_Skills[nListIndex].SkillId;
		else 
			return -1;
	};
	int			GetAddSkillDamage(int nSkillID);

	int GetAddLevel(int SkillId)
	{
		for(int i = 1; i < MAX_NPCSKILL; i++)
		{
			if (m_Skills[i].SkillId == SkillId)
			{
				return m_Skills[i].AddLevel;
			}
		}
		return 0;
	}	
	int	GetAddLevelIndex(int nListIndex)
	{
		if (nListIndex < MAX_NPCSKILL && nListIndex > 0)
			return m_Skills[nListIndex].AddLevel;
		else 
			return 0;
	};
#ifdef _SERVER
	int		UpdateDBSkillList(BYTE *);
	void	SetNpcSkill(int nSkillNo, int nSkillId, int nSkillLevel);
	void	RemoveNpcSkill(int nSkillNo);
	int		RollBackSkills(bool bRbAll);
#endif

};
#endif
