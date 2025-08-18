// -------------------------------------------------------------------------
//	文件名		：	交易确认框
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-5-28
// -------------------------------------------------------------------------
#ifndef __UiBreakItem_H__
#define __UiBreakItem_H__

#include "../Elem/WndButton.h"
#include "../Elem/WndText.h"
#include "../Elem/WndEdit.h"
#include "../../../core/src/gamedatadef.h"

class KUiBreakItem : protected KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiBreakItem*	OpenWindow(KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pPriceInfo, int nStackNum);	//打开窗口，返回唯一的一个类对象实例
	static KUiBreakItem*	GetIfVisible();				//如果窗口正被显示，则返回实例指针
	static void			CloseWindow(bool bDestroy);		//关闭窗口
private:
	KUiBreakItem();
	~KUiBreakItem() {}
	int		Initialize();								//初始化
	void	Show();
	void	Hide();
	void	LoadScheme(const char* pScheme);			//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnCancel();
	void	OnOk();
	void	OnCheckInput();
	void	OnIncrease();
	void	OnDecrease();
private:
	static KUiBreakItem*	m_pSelf;
	KWndText32			m_Title;
	KWndText32			m_ItemName;
	KWndEdit32			m_Number;
	KWndButton			m_Increase;
	KWndButton			m_Decrease;
	int					m_nMaxNum;
	KWndButton			m_OkBtn;
	KWndButton			m_CancelBtn;
	KWndButton			m_Button;
	KWndButton			m_CheckButton;
	KUiObjAtContRegion	m_ItemInfo;
	KUiItemBuySelInfo	m_PriceInfo;
};


#endif // __UiTradeConfirm_H__