/* 
 * File:     UiTaskNote.h
 * Desc:     任务记事窗口
 * Author:   flying
 * Creation: 2003/7/18
 * 即将第二次参加新人培训
 */
//-----------------------------------------------------------------------------
#pragma once

#if !defined _UITASKNOTE
#define _UITASKNOTE

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndList.h"
#include "../Elem/WndShadow.h"
#include "../Elem/WndText.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndScrollBar.h"


#define MAX_TASK_PAGE 3
#define MAX_TASK_ON_PAGE 12
#define	MAX_TASK_HEAD MAX_TASK_PAGE*MAX_TASK_ON_PAGE

enum RECORD_ACTION
{
	REA_EXPLAIN = 0,
	REA_PERFORM,
	REA_FINISH,
};

struct KWndTaskButton
{
	KWndButton				btn;
	KWndImage				onimg;
	KWndImage				bdimg;
	unsigned				id;
};

class KUiTaskNote : public KWndShowAnimate
{
public:
	static KUiTaskNote*		OpenWindow();
	//关闭窗口，同时可以选则是否删除对象实例
	static void				CloseWindow(bool bDestroy);
	static KUiTaskNote*		GetIfVisible();
	// 由游戏循环唤醒
	static void				WakeUp(KMissionRecord* pRecord);
	//载入界面方案
	static void				LoadScheme(const char* pScheme);
private:
	KUiTaskNote();
	virtual ~KUiTaskNote();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	Initialize();
	void	OnClose();
	void	InitData();		//初始化相关存档数据
	void	SaveData();		//保存数据
	void	UpdateData(int nSel);
	void	UpdateList(int nId);
	void	OnDelete(int nIndex);
private:
	// here goes data.
	static KUiTaskNote*		m_pSelf;
	
	KWndButton				m_BtnGut, m_BtnTask, m_BtnEvent;
	KWndTaskButton			m_TBtnTaskSel[MAX_TASK_ON_PAGE];
	KWndButton				m_BtnClose;
	KWndImage				m_icon;
	KScrollMessageListBox	m_RecordList;
	KWndButton				m_BtnDelete;
	unsigned int			m_uNotOKPicIndex, m_uOKPicIndex;
	KRColor					m_uExplainColor,m_uNotOKColor, m_uOKColor, m_uFnColor;
	int						m_nTabSel, m_nTaskSel[MAX_TASK_PAGE];
	unsigned int			m_uCurHeaderId;
	int						m_nDirLeft, m_nDirTop;
	char					m_szTaskImage[MAX_TASK_HEAD][2][FILE_NAME_LENGTH], m_szBorderImage[FILE_NAME_LENGTH], m_szOnImage[FILE_NAME_LENGTH];
};

class KUiMissionNote : public KWndShadow
{
public:
	static KUiMissionNote*		OpenWindow(KMissionRecord* pRecord);
	//关闭窗口，同时可以选则是否删除对象实例
	static void				CloseWindow(bool bDestroy);
	static KUiMissionNote*		GetIfVisible();
	//载入界面方案
	static void				LoadScheme(const char* pScheme);
private:
	KUiMissionNote();
	virtual ~KUiMissionNote();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	Initialize();
	int		PtInWindow(int x, int y);				//判断一个点是否在窗口范围内,传入的是绝对坐标
private:
	// here goes data.
	static KUiMissionNote*		m_pSelf;


	KWndText32				m_Name;
	KWndMessageListBox		m_DestText;
	KWndPureTextBtn			m_CloseBtn;
};
#endif