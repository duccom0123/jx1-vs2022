#include "../Elem/WndShadow.h"
#include "../Elem/WndText.h"

#define MAX_SELFINFO_ITEMNAME 5

enum selfInfoObj
{
	PkPlayer,
	NpcKill,
	MaxKill,
	GetItem,
	GetFlag,
};

class KUiRankData : KWndShadow
{
public:

public:
	//打开窗口，返回唯一的一个类对象实例
	static KUiRankData* OpenWindow();
	//关闭窗口，同时可以选则是否删除对象实例
	static void   CloseWindow(bool bDestory = TRUE); 
	static KUiRankData*	GetIfVisible();
public:

private:
	static  KUiRankData *m_pSelf;
private:
	KUiRankData();
	~KUiRankData();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void    Initialize();       
	void	LoadScheme(const char* pScheme);	//载入界面方案
	void	PaintWindow();
	void	Breathe();

private:
	KWndText32		m_BattleWarTxt;
	KWndText32		m_WinCondition;
	KWndText32		m_WinConditionVal;
	KWndText32		m_Proportion;
	KWndText32		m_ProportionVal;
	KWndText32		m_LeftTime;
	KWndText32		m_LeftTimeVal;
	
	KWndText32		m_ReportItem;
	KWndText32		m_ObjNum;
	KWndText32		m_SelfGrade;
	
	KWndText32		m_SelfInfoItemName[MAX_SELFINFO_ITEMNAME];
	KWndText32		m_SelfInfoObj[MAX_SELFINFO_ITEMNAME];
	KWndText32		m_SelfInfoGrade[MAX_SELFINFO_ITEMNAME];
	
	KWndText32		m_SelfCurGrade;
	KWndText32		m_SelfCurGradeVal;
	KWndText32		m_SelfCurDeath;
	KWndText32		m_SelfCurDeathVal;
	KWndText32		m_SelfCurMaxKill;
	KWndText32		m_SelfCurMaxKillVal;
	KWndText32		m_OperateRemind;
	
	KWndText32		m_TopTenPlayFaction[MISSION_STATNUM];
	KWndText32		m_TopTenPlayName;
	KWndText32		m_TopTenPlayNameTxt[MISSION_STATNUM];
	KWndText32		m_TopTenGrade;
	KWndText32		m_TopTenGradeVal[MISSION_STATNUM];
	KWndText32		m_TopTenKillNpc;
	KWndText32		m_TopTenKillNpcVal[MISSION_STATNUM];
	KWndText32		m_TopTenPkPlayer;
	KWndText32		m_TopTenPkPlayerVal[MISSION_STATNUM];
	KWndText32		m_TopTenMaxKill;
	KWndText32		m_TopTenMaxKillVal[MISSION_STATNUM];
	KWndText32		m_TopTenGetItem;
	KWndText32		m_TopTenGetItemVal[MISSION_STATNUM];
	KWndText32		m_TopTenGetFlag;
	KWndText32		m_TopTenGetFlagVal[MISSION_STATNUM];
	KWndText32		m_TopTenDeath;
	KWndText32		m_TopTenDeathVal[MISSION_STATNUM];
	
	int				m_nLeft_Right;
	int				m_nTitleFont;
	int				m_nValueFont;
	
	int				m_nTopInfoPos;
	int				m_nLinePos1;
	int				m_nSelfInfoPos;
	int				m_nSelfGradePos;
	int				m_nLinePos2;
	int				m_nTopTenPos;
	
	unsigned int	m_LineColor;
	char			m_szFaction[2][32];
};