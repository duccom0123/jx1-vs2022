/*****************************************************************************************
//	界面--技能窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-28
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/wndpage.h"
#include "../Elem/WndImagePart.h"
#include "../elem/WndLabeledButton.h"
#include "../elem/WndEdit.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndList.h"
#include "../../../core/src/gamedatadef.h"


#define	FIGHT_SKILL_COUNT			50
#define FIGHT_SKILL_COUNT_PER_PAGE	25
#define	FIGHT_SKILL_SUB_PAGE_COUNT	FIGHT_SKILL_COUNT / FIGHT_SKILL_COUNT_PER_PAGE

class KUiFightSkillSubPage : public KWndPage
{
public:
	KUiFightSkillSubPage();
	void	Initialize(/*int nSubPageIndex*/);				//初始化
	void	LoadScheme(const char* pScheme);			//载入界面方案
	void	UpdateRemainPoint(int nPoint);				//更新升级点数
	void	UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新单个技能
	void	UpdateData(KUiSkillData* pSkills);			//更新数据
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	PaintWindow();								//绘制窗口
private:
//	int			m_nSubPagIndex;
	int			m_nRemainSkillPoint;
	KWndObjectBox	m_FightSkills[FIGHT_SKILL_COUNT_PER_PAGE];
//	KUiSkillData	m_Skills[FIGHT_SKILL_COUNT_PER_PAGE];
	struct
	{
		int		nFont;
		SIZE	Offset;
		unsigned int Color;
	}			m_SkillTextParam;
};

class KUiFightSkillSub : public KWndPageSet
{
public:	
	void	Initialize();								//初始化
	void	LoadScheme(const char* pScheme);			//载入界面方案
	void	UpdateRemainPoint(int nPoint);				//更新升级点数
	void	UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新技能
	void	UpdateData();								//更新数据
private:
	KUiFightSkillSubPage	m_SubPages[FIGHT_SKILL_SUB_PAGE_COUNT];
	KWndButton				m_SubPageBtn[FIGHT_SKILL_SUB_PAGE_COUNT];
};

class KUiFightSkill : public KWndPage
{
public:
	void	Initialize();								//初始化
	void	LoadScheme(const char* pScheme);			//载入界面方案
	void	UpdateRemainPoint(int nPoint);				//更新升级点数
	void	UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新技能
	void	UpdateData();								//更新数据	
private:
	KWndText32			m_RemainSkillPoint;
	KUiFightSkillSub	m_InternalPad;
};

class KUiSkills : public KWndPageSet
{
public:
	static KUiSkills* OpenWindow();							//打开窗口，返回唯一的一个类对象实例
	static KUiSkills* GetIfVisible();						//如果窗口正被显示，则返回实例指针
	static void		 CloseWindow(bool bDestroy);			//关闭窗口，同时可以选则是否删除对象实例
	static void		 LoadScheme(const char* pScheme);		//载入界面方案
	static void		 UpdateSkill(KUiSkillData* pSkill, int nIndex);	//更新技能

	void	UpdateFightRemainPoint(int nPoint);				//更新战斗技能升级点数

private:
	KUiSkills(){}
	~KUiSkills() {}
	void	Initialize();							//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
private:
	static KUiSkills* m_pSelf;
private:
	KUiFightSkill	m_FightSkillPad;
	KWndButton		m_FightSkillPadBtn;
	KWndButton		m_Close;
};
