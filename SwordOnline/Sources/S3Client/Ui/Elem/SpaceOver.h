/*****************************************************************************************
//	浮动提示窗口
//	Copyright : Kingsoft 2002-2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-23
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "WndMessage.h"
#include "UiImage.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../core/src/GameDataDef.h"

class KSpaceOver
{
public:
	KSpaceOver();
	int		IsSpaceOverWndObj(void* pWnd, int nObj);
	void    SetSpaceOverInfo(void* pWnd, int nObj, int x, int y, unsigned int uHoverTime, bool LAlign = false, bool bFollowCursor = false);
	void    SetSpaceOverImage(bool bAdd);
    void    SetSpaceOverTitle(const char *pcszTitleText, int nTitleTextLen, UINT uColor);
	void	CancelSpaceOverInfo();
	void	PaintSpaceOverInfo();
	void	LoadScheme(const char* pScheme);		//载入界面方案
	void	UpdateCursorPos(int nX, int nY);		//鼠标的位置更新了
	void	OnWndClosed(void* pWnd);
private:
	void	Update(int nX, int nY);
private:
	int		m_nLeft;		        //窗口左上角横坐标
	int		m_nTop;			        //窗口左上角纵坐标
	int		m_nWndWidth;	        //动态的窗口水平宽
	int		m_nWndHeight;	        //动态的窗口垂直宽
	int		m_nWndWidthReduce;	        //动态的窗口水平宽
	int		m_nWndHeightReduce;	        //动态的窗口垂直宽
	int		m_nWidthReduce;
	int		m_nHeightReduce;
	int		m_nMaxWidthReduce;
	int		m_nMaxHeightReduce;
	int		m_nIndent;		        //文字缩进（与边框的水平相隔距离）
	int		m_nFontSize;	        //文字字体(大小)

	int     m_nApplyX;              //外部提交的浮动窗口的X坐标
	int     m_nApplyY;              //外部提交的浮动窗口的Y坐标
	int     m_nTitleLineNum;        //标题(名字)部分的行数
	int     m_nMaxLineLen;          //三部分一起算最长的行的长度

	unsigned int m_uTitleBgColor;   //标题(名字)背景颜色
	unsigned int m_uTitleColor;     //标题(名字)文字颜色
	/////////////////////////
	unsigned int m_uBoderShadowColor;      //属性文字颜色
	/////////////////////////
	KRUImagePart m_Image;	        //边框图形

	void*	m_pSpaceOverWnd;		//鼠标停留的窗口
	int		m_nObj;
	char    m_ObjTitle[GOD_MAX_OBJ_TITLE_LEN];
	int		m_nTitleLen;
	bool	m_bTempImg;			//是否显示提头与结尾边框图
	bool	m_LAlign;
	bool	m_bFollowCursor;		//是否跟踪鼠标指针的位置，一起移动，（同时没有横宽最短限制）
	BOOL	m_bShow;
	unsigned int	m_uHoverTime;
	unsigned int	m_uLastHoverTime;

public:
	KUiImageRef	 m_HoverImage;
};

extern KSpaceOver	g_SpaceOver;

void SetSpaceOverObjDescColor(unsigned int uColor);
void SetSpaceOverObjectDesc(void* pWnd, int nObj, unsigned int uGenre,
			unsigned int uId, int nContainer, int x, int y, unsigned int uHoverTime, bool LAlign = false);
//绘制被拖动游戏对象的函数
