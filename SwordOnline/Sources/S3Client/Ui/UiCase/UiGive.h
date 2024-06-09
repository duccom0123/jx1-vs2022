// -------------------------------------------------------------------------
//	文件名		：	UiGive.h
//	功能描述	：	获得字串界面
// -------------------------------------------------------------------------
#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../elem/wndbutton.h"
#include "../Elem/WndText.h"
#include "../elem/wndObjContainer.h"
#include "../Elem/WndMessageListBox.h"
#include "../../../core/src/gamedatadef.h"
struct KUiObjAtRegion;
class KUiGive : protected KWndImage
{
public:
	//----界面面板统一的接口函数----
	static KUiGive*	OpenWindow(const char* szName, char* szInitString);	//打开窗口，返回唯一的一个类对象实例
	static KUiGive*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);	//载入界面方案
	static void			CloseWindow(bool bDestroy);		//关闭窗口
	void	OnClickItem(KUiDraggedObject* pItem);
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	void	UpdateItem(KUiObjAtRegion* pItem, int bAdd);//物品变化更新
private:
	KUiGive();
	~KUiGive() {}
	void	Initialize();								//初始化
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData(const char* szName, char* szInitString);
private:
	static KUiGive*	m_pSelf;
	KWndText32			m_Title;
	KWndMessageListBox	m_ContentList;
	KWndScrollBar		m_ContentScroll;	//备选文字和滚动条
	KWndObjectMatrix	m_ItemBox;
	KWndButton			m_Confirm;
	KWndButton			m_Cancel;
};

