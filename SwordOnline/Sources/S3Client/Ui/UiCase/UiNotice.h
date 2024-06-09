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

class KUiNotice : protected KWndShowAnimate
{
public:
	static KUiNotice* OpenWindow();				//打开窗口，返回唯一的一个类对象实例
	static void			 CloseWindow(bool bDestroy);//关闭窗口
private:
	KUiNotice();
	~KUiNotice();
	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);	//窗口函数
	void	OnLogin();
	void	OnCancel();
	int		OnKeyDown(unsigned int uKey);
	void	OnClickButton(KWndButton* pBtn);
	void	GetList();

private:
	static KUiNotice* m_pSelf;
	void	UpdateNotice();
private:
	KWndText256		m_List;
	KWndButton		m_Close;
	KWndText256		m_Notice;
	KWndMessageListBox			m_MessageList;
	KWndScrollBar				m_MessageScroll;
	char			m_szNotice[MAX_SENTENCE_LENGTH];
	char			m_szMessage[MAX_MESSAGE_LENGTH];
};
