/*****************************************************************************************
//	界面--选择游戏服务器窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-12
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../../../Core/Src/GameDatadef.h"

class KUiPlayerLock : protected KWndShowAnimate
{
public:
	static KUiPlayerLock* OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static void			 CloseWindow();//关闭窗口
	static KUiPlayerLock*	GetIfVisible();		//如果窗口正被显示，则返回实例指针
private:
	KUiPlayerLock();
	~KUiPlayerLock();
	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	Breathe();									//活动函数
	void	GetList();

private:
	static KUiPlayerLock* m_pSelf;
	void	UpdateNotice();
private:
    void	OnClickButton(KWndWindow* pBtn);	
	KWndText256		m_List;
	KWndButton		m_Close;
	KWndButton	m_UnlockBtn;
	KWndText256		m_Notice;
	KWndText256		m_ListMS;
	KWndMessageListBox			m_MessageList;
//	KWndScrollBar				m_MessageScroll;
	char			m_szNotice[MAX_SENTENCE_LENGTH];
	char			m_szMessage[MAX_MESSAGE_LENGTH];
};
