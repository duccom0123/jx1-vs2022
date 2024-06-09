/*****************************************************************************************
//	界面--道具界面
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../elem/WndButton.h"
#include "../elem/WndImage.h"
#include "../elem/WndText.h"
#include "../elem/WndObjContainer.h"
#include "../Elem/WndShowAnimate.h"
#include "../../../core/src/GameDataDef.h"

struct KUiObjAtRegion;

class KUiItem : public KWndShowAnimate
{
public:
	static KUiItem* OpenWindow(bool bFlag = true);				//打开窗口，返回唯一的一个类对象实例
	static KUiItem* GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void		CloseWindow(bool bDestroy);	//关闭窗口，同时可以选则是否删除对象实例
	static void		LoadScheme(const char* pScheme);//载入界面方案
	static void		OnNpcTradeMode(bool bTrue);
	void			UpdateItem(KUiObjAtRegion* pItem, int bAdd);//物品变化更新
	void			Breathe();									//活动函数
private:
	KUiItem() {}
	~KUiItem() {}
	void	Initialize();							//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void	UpdateData();
	void	OnClickItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	OnRepairItem(KUiDraggedObject* pItem);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	OnGetMoney(int nMoney);
	void	OnBreakItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	OnSetPrice(KUiDraggedObject* pItem);
private:
	static KUiItem*		m_pSelf;
private:
	int					m_nMoney;
	KWndText256			m_Money;
	KWndText256			m_Gold;
	KWndButton			m_GetMoneyBtn;
	KWndImage			m_TitleIcon, m_MoneyIcon, m_GoldIcon;
	KWndButton			m_CloseBtn;
	KWndObjectMatrix	m_ItemBox;
	KWndButton			m_OpenStatusPadBtn;
	KWndButton			m_MakeAdvBtn;
	KWndButton			m_MarkPriceBtn;
	KWndButton			m_MakeStallBtn;
	BYTE				m_byMark;

	char				m_szAdvStr[16];
};