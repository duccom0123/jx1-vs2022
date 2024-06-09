/*****************************************************************************************
*****************************************************************************************/
#pragma once

struct KPopupMenuItem
{
	char			szData[64];
	unsigned int	uDataLen;
	unsigned int	uBgColor;
	unsigned short	uID;
};

#define MENU_DEFAULT_POS				-607
#define	MENU_ITEM_DEFAULT_WIDTH			0
#define	MENU_ITEM_DEFAULT_HEIGHT		0
#define	MENU_ITEM_DEFAULT_RIGHT_WIDTH	-1
#define MENU_ITEM_DEFAULT_INDENT		255
#define MENU_ITEM_DEFAULT_TITLEUPSPACE	1

enum POPUPMENU_FLAG
{
	PM_F_HAVE_HEAD_TAIL_IMG		= 0x0001,
	PM_F_ZOOM_SELECTED_ITEM		= 0x0002,
	PM_F_TAB_SPLIT_ITEM_TEXT	= 0x0004,
	PM_F_HAVE_ITEM_SEPARATOR	= 0x0008,
	PM_F_AUTO_DEL_WHEN_HIDE		= 0x0010,
	PM_F_CANCEL_BY_CALLER		= 0x0020,
};

struct KPopupMenuData
{
	short			nX;
	short			nY;
	short			nXX;
	short			nYY;

	short			nItemWidth;
	short			nItemRightWidth;
	short			nItemHeight;
	short			nNumItem;
	short			nSelectedItem;
	unsigned short	usMenuFlag;

	unsigned char	byFontSize;
	unsigned char	byItemTitleUpSpace;
	short			nItemTitleIndent;

//	bool			bZoomSelectedItem;
//	bool			bHaveHeadTailImg;
//	bool			bHaveSeparator;
//	bool			bAutoDelete;
//	bool			bManualCancel;

	unsigned int	uBorderLineColor;
	unsigned int	uSelItemBgColor;
	unsigned int	uTextColor;	
	unsigned int	uTextBorderColor;
	unsigned int	uSelTextColor;
	unsigned int	uSelTextBorderColor;

	KPopupMenuItem	Items[1];
};

#define	MENU_DATA_SIZE(nNumItem)	(sizeof(KPopupMenuData) + (nNumItem - 1) * sizeof(KPopupMenuItem))

class KWndWindow;
class KPopupMenu
{
public:
	static void	Popup(KPopupMenuData* pMenu, KWndWindow* pCaller, unsigned int uParam);
	static void Cancel();
	static void	PaintMenu();
	static void	LoadTheme(const char* pScheme);
	static int	HandleInput(unsigned int uMsg, unsigned int uParam, int nParam);//´°¿Úº¯Êý
	static void OnWndDelete(KWndWindow* pWnd);
	static void	InitMenuData(KPopupMenuData* pMenu, int nNumItem);
	static KPopupMenuData*	GetMenuData() {	return m_pMenu;}
private:
	static bool SelectItem(int x, int y, bool bApply, bool bRight = false);
	static bool SelectItem(int nIndex, bool bApply);
private:
	static KPopupMenuData*  m_pMenu;
	static KWndWindow*		m_pCaller;
	static unsigned int		m_uCallerParam;
	
	static short			m_nImgWidth;
	static short			m_nImgHeight;
	static short			m_nIndent;
    static short			m_nFrame;
	static int				m_nMenuHeight;
	static char				m_szImage[128];

	static unsigned int	m_uDefBorderLineColor;
	static unsigned int	m_uDefItemBgColor;
	static unsigned int	m_uDefSelItemBgColor;
	static unsigned int	m_uDefTextColor;
	static unsigned int	m_uDefTextBorderColor;
	static unsigned int	m_uDefSelTextColor;
	static unsigned int	m_uDefSelTextBorderColor;
};
