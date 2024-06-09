/* 
 * File:     UiCaveList.h
 * Desc:     世界地图
 * Author:   flying
 * Creation: 2003/7/22
 */
//-----------------------------------------------------------------------------
#pragma once

#include "../Elem/WndImage.h"
#include "../Elem/WndButton.h"
#include "../Elem/UiImage.h"

class KUiCaveList : protected KWndImage
{
public:
	static KUiCaveList* OpenWindow();		//打开窗口，返回唯一的一个类对象实例
	static void			CloseWindow();		//关闭窗口
	static KUiCaveList*	GetIfVisible();
	void				LoadScheme(const char* pScheme);	//载入界面方案

private:
	KUiCaveList() {}
	~KUiCaveList() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	Breathe();				//活动函数
	void	UpdateData();
private:
	static KUiCaveList* m_pSelf;
	KWndImage			m_Sign;
};