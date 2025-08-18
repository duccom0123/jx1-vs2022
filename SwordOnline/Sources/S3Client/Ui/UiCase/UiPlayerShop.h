// -------------------------------------------------------------------------
//	文件名		：	UiTrade.h
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2002-12-21
//	功能描述	：	npc交易系统界面
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../../../core/src/gamedatadef.h"

struct KUiObjAtContRegion;

class KUiPlayerShop : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiPlayerShop*		OpenWindow(KUiPlayerItem* pDest);				//打开窗口，返回唯一的一个类对象实例
	static KUiPlayerShop*		GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void			CloseWindow();				//关闭窗口，同时可以选则是否删除对象实例
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CancelTrade();
	void				UpdateData(KUiPlayerItem* pDest);	
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void			UpdateItem();

private:
	KUiPlayerShop();
	~KUiPlayerShop() {}
	void	Clear();
	void	OnClickButton(KWndButton* pWnd, int bCheck);
	void	OnBuyItem(KUiDraggedObject* pItem);
	void	Initialize();						//初始化
private:
	static KUiPlayerShop*		m_pSelf;
private:
	KUiObjAtContRegion*		m_pObjsList;
	int					m_nObjCount;
	int					m_nPageCount;
	int					m_nCurrentPage;
	KWndObjectMatrix	m_ItemsBox;		//物品栏
	KWndButton			m_CloseBtn;
	KUiPlayerItem		m_Dest;
	KWndText32			m_Name;
	int					m_nCount;
};