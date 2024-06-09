// -------------------------------------------------------------------------
//UiToolsControlBar.h
// -------------------------------------------------------------------------
#ifndef __UITOOLSCONTROLBAR_H__
#define __UITOOLSCONTROLBAR_H__

#pragma once
#include "../Elem/WndToolBar.h"
#include "../Elem/WndButton.h"

class KUiToolsControlBar : public KWndToolBar
{
public:
	static KUiToolsControlBar* OpenWindow();
	static void				CloseWindow();
	static void				LoadScheme(const char* pScheme);
	static void				DefaultScheme(const char* pScheme);
	static KUiToolsControlBar* GetSelf()	{return m_pSelf;}
	static void	SwitchPK();
	static void	SwitchFastPK();
private:
	~KUiToolsControlBar() {}
	void	Initialize();
	void	Breathe();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	SwitchPK(int nAction);	
private:
	static KUiToolsControlBar*	m_pSelf;
	KWndButton	m_Friend;
	KWndButton	m_Options;
	enum CHOICE_PK
	{
		Exercises = 0,
		Fighting,
		Murder,
		Tongwar,
	};
};
#endif // __UITOOLSCONTROLBAR_H__