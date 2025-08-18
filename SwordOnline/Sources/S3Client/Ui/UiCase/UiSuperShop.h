// -------------------------------------------------------------------------
//	UiMarketNew.h
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndObjContainer.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndScrollBar.h"

#define	MAX_SUPERSHOP_PAGETAB 				8
#define MAX_ITEM_PER_ROW 					3
#define MAX_ITEM_PER_COLUMN 				5

class KUiShoppingCart : protected KWndShowAnimate
{
public:
	static KUiShoppingCart*	OpenWindow(KUiObjAtContRegion* pObj,
			KUiItemBuySelInfo* pPriceInfo);
	static KUiShoppingCart*	GetIfVisible();
	static void				CloseWindow(bool bDestroy);
	void					Show();
private:
	KUiShoppingCart();
	~KUiShoppingCart() {}
	void					Initialize();
	void					LoadScheme(const char* pScheme);
	int						WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void					UpdateData();	//更新技能
	void					Breathe();
	void					OnCheckInput();
	void					CartRelease();
	void					AddCount();
	void					DelCount();
private:
	static KUiShoppingCart*		m_pSelf;
public:
	KUiObjAtContRegion	m_ItemInfo;
	KUiItemBuySelInfo	m_PriceInfo;
private:
	
	KWndText80					m_Title;
	KWndText80					m_TotalCostTitleText;
	KWndText80					m_TotalSaveTitleText;
	KWndText80					m_OwnTitleText;
	KWndText80					m_TotalCostValueText;
	KWndText80					m_TotalSaveValueText;
	KWndText80					m_OwnValueText;
	KWndScrollBar				m_ScrollBar;
	
	KWndImage					m_GoodsInfo;
	KWndObjectBox				m_GoodsInfo_ItemBox;
	KWndText80					m_GoodsInfo_GoodsName;
	KWndText80					m_GoodsInfo_TotalPrice;
	KWndLabeledButton			m_GoodsInfo_DelItem;
	KWndButton					m_GoodsInfo_AddCount;
	KWndButton					m_GoodsInfo_DelCount;
	KWndEdit32					m_GoodsInfo_CountEdit;
	
	KWndLabeledButton			m_ConfirmBuy;
	KWndLabeledButton			m_PrePaidBtn;
	KWndButton					m_CloseBtn;

	int							m_nBuyValue;
	int							m_nCostValue;
	int							m_nOldValue;
	int							m_nSaveValue;
	int							m_nOwnValue;
};


class KCanGetNumFrame : public KWndImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KWndSellItem  : public KWndObjectBox
{
public:
	void Initialize();
	void LoadScheme(const char* pScheme);
	virtual void	PaintWindow();
	void Clear();
	KWndImage					m_MarketGoods;
	KWndObjectBox	     	 	m_ItemBox;
	KWndImage					m_MarketGoods_DisCount;
	KCanGetNumFrame 			m_MarketGoods_imgNewArrival;
	KWndText80            		m_GoodsNameText;
	KWndText80            		m_OriginalPriceText;
	KWndText80            		m_OriginalPrice_NumberText;
	KWndText80            		m_PriceText;
	KWndText80            		m_Price_NumberText;
	KWndLabeledButton			m_MarketGoods_Buy;
private: 
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
    
};

class KUiSuperShop : protected KWndShowAnimate
{
public:
	static KUiSuperShop*	OpenWindow(BuySellInfo* pBSinfo);
	static KUiSuperShop*	OpenWindow();
	static KUiSuperShop*	GetIfVisible();
	static void				CloseWindow(bool bDestroy);
	static void				LoadScheme(const char* pScheme);
	static void				UpdateShop(BuySellInfo* pBSinfo);
	static void				PutItem(KUiObjAtContRegion* pObj);
	static void				BuyItem(int nNumber);
private:
	KUiSuperShop();
	~KUiSuperShop();
	void					Initialize();
	int						WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void					UpdateData();
	void					SetSellTypeStart(int nStart);
	void					SetCurrSellSort(int nSaleId);	
	void					CancelTrade();
	void					ClearPage();
	void					SellSortChange(int i);
	void					PageChangeInfo();
	void					Clear();
	void					SetPage(int nIndex);
	void					UpdateItem(KUiObjAtContRegion* pItem, int nAdd);
	void					Breathe();
private:
	static KUiSuperShop*		m_pSelf;
private:

	KWndSellItem          		m_WndSellItem[15];
	KWndLabeledButton			m_SellType[MAX_SUPERSHOP_PAGETAB];	
	KWndLabeledButton			m_NextBtn;
	KWndLabeledButton			m_LastBtn;
	KWndLabeledButton			m_PrePaidBtn;
	KWndText256					m_SellTypeGuide;
	KWndLabeledButton			m_NextPageBtn;
	KWndLabeledButton			m_LastPageBtn;
	KWndLabeledButton			m_ShoppingCartBtn;	
	KWndButton					m_CloseBtn;
	
	
	BuySellInfo					m_pBSinfo;
	KWndText32        			m_CurrentPageText;
	BYTE						m_nStoreActive;
	int		 					m_nSellTypeCount;
	int		 					m_nSellTypeStart;
	int      					m_nCurrentPage;
	int     					m_nCurrentShopId;
	int      					m_nPageCount;
	unsigned int				m_uEnableTextColor;		//m_ToggleBtn标题文字的颜色
	unsigned int				m_uInvalidTextColor;	//m_ToggleBtn标题文字的颜色	
	
	KUiObjAtContRegion*	m_pObjsList;
	int					m_nObjCount;
	KUiObjAtContRegion	m_pItemInfo;
};


class KUiDynamicShop : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiDynamicShop*		OpenWindow(BuySellInfo* pBSinfo);				//打开窗口，返回唯一的一个类对象实例
	static KUiDynamicShop*		GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void			CloseWindow();				//关闭窗口，同时可以选则是否删除对象实例
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CancelTrade();
	void				UpdateShop(BuySellInfo* pBSinfo);
	void				UpdateData();
	void				UpdateItem(KUiObjAtContRegion* pItem, int bAdd);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
private:
	KUiDynamicShop();
	~KUiDynamicShop() {}
	void	SellSortChange(int i);
	void	SetPage(int nIndex);
	void	Clear();
	void	OnClickButton(KWndButton* pWnd, int bCheck);
	void	OnBuyItem(KUiDraggedObject* pItem, bool bDoImmed);
	void	Initialize();						//初始化
private:
	static KUiDynamicShop*		m_pSelf;
private:
	KUiObjAtContRegion*			m_pObjsList;
	int							m_nObjCount;
	int							m_nPageCount;
	int							m_nCurrentPage;
	int     					m_nCurrentShopId;
	
	BuySellInfo					m_pBSinfo;
	KWndLabeledButton			m_SellType[MAX_SUPERSHOP_PAGETAB];	
	KWndObjectMatrix			m_ItemsBox;		//物品栏
	KWndLabeledButton			m_BuyBtn;
	KWndLabeledButton			m_SellBtn;
	KWndLabeledButton			m_RepairBtn;
	KWndLabeledButton			m_PreBtn;
	KWndLabeledButton			m_NextBtn;
	KWndButton					m_CloseBtn;
	KWndText32					m_CurPageTxt;
	KWndImage					m_TitleImage;
};