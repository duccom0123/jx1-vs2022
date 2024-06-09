// -------------------------------------------------------------------------
//	文件名		：	UiInit.h
//	创建者		：	彭建波
//	创建时间	：	2002-9-10 11:25:36
//	功能描述	：	初始界面	
//
// -------------------------------------------------------------------------
#ifndef __UIINIT_H__
#define __UIINIT_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndShowAnimate.h"

class KUiInit : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiInit*		OpenWindow(bool bStartMusic = true, bool bJustLauched = false);	//打开窗口，返回唯一的一个类对象实例
	static void			CloseWindow();							//关闭窗口
	static void			PlayTitleMusic();
	static void			StopTitleMusic();

private:
	KUiInit() {}
	~KUiInit() {}
    void    ShowCompleted();
	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickButton(KWndButton* pWnd);	//响应点击按钮
	KWndButton*	GetActiveBtn();
	void	OnAutoLogin();
	int		OnKeyDown(unsigned int uKey);
private:
	static KUiInit* m_pSelf;
private:
	char		m_szLoginBg[32];

	KWndButton m_EnterGame;					// 进入游戏
	KWndButton m_GameConfig;				// 游戏设置
	KWndButton m_OpenRep;				// 制作人员名单
	KWndButton m_DesignerList;				// 制作人员名单
	KWndButton m_ExitGame;					// 退出游戏
	KWndButton m_EnterGameBorder;
	KWndButton m_GameConfigBorder;
	KWndButton m_OpenRepBorder;
	KWndButton m_ExitGameBorder;
};

#endif // __UIINIT_H__