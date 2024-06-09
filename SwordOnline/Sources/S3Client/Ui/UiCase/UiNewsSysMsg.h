/*****************************************************************************************
//	界面--新闻消息窗口
//	Copyright : Kingsoft 2003
//	Author	:   Fyt(Fan Zhanpeng)
//	CreateTime:	2003-08-01
------------------------------------------------------------------------------------------
*****************************************************************************************/
#ifndef _UINEWSSYSMSG_H
#define _UINEWSSYSMSG_H    1

#include "KEngine.h"
#include "KIniFile.h"
#include "../Elem/WndText.h"
#include "../Elem/WndShadow.h"
#include "../../../Core/Src/GameDataDef.h"

struct KNewsSysMsg
{
	char	sMsg[MAX_MESSAGE_LENGTH];	//消息内容
	int		nMsgLen;					//消息内容存储长度
	int		nCurLen;
	unsigned uTime;
};

struct KNewsSysMsgNode
{
	KNewsSysMsgNode*	pNext;
	KNewsSysMsg			Msg;
};

class KUiNewsSysMsg : public KWndWindow
{
public:
	//----界面面板统一的接口函数----
	static KUiNewsSysMsg*	OpenWindow();						// 打开窗口，返回唯一的一个类对象实例
	static KUiNewsSysMsg*  GetIfVisible();                     //如果窗口正被显示，则返回实例指针
	static void				CloseWindow(BOOL bDestory = FALSE);	// 关闭窗口
	static void				LoadScheme(const char* pszScheme);	// 载入界面方案
	static void				MessageArrival(const char* pMsg, unsigned short nMsgLength);

private:
    static KUiNewsSysMsg *m_pSelf;
private:
	KUiNewsSysMsg();
	~KUiNewsSysMsg() {}
	void	Initialize();
	void	LoadScheme(KIniFile* pIni);			// 载入界面方案
	virtual int		PtInWindow(int x, int y);	// 穿透！！！
	virtual void	PaintWindow();
	void	ClearAll();
	bool	AddMessage(KNewsSysMsg* pMsg);
												// 在消息队列-__-||b的最前面增加一条消息-__-||b
	int		GetSystemMessageCount();
	bool	RemoveSystemMessage(int nIndex);
	const KNewsSysMsg*	GetSystemMessage(int nIndex);
private:

	KNewsSysMsgNode*	m_pHead;			// 消息链表中的头一个消息
	static	int			ms_nSystemMessageCount;

	int				m_nIndentH;				// 前缀和显示消息部份分隔多少
	int				m_nIndentV;				// 文字相对于窗口顶部的向下缩近（单位：像素点）
	int				m_nFontSize;			// 字体大小
	unsigned int	m_uTextColor;			// 前景字符的颜色
	unsigned int	m_uTextBorderColor;		// 文字边缘颜色
	unsigned int	m_uShadowColor;			// 前景字符的颜色
	unsigned int	m_uShowInterval;		// 同一个消息两次显示之间的间隔
};

#endif
