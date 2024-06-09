#pragma once
#include "../Elem/WndShowAnimate.h"
#include "../elem/wndpage.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndLabeledButton.h"
#include "../elem/wndbutton.h"
#include "../elem/WndList.h"

class KUiChooseFace : public KWndPageSet
{
public:
	static KUiChooseFace* OpenWindow(int uParam);	
	static KUiChooseFace* GetIfVisible();	
	static void			 CloseWindow();
private:

	void	Initialize();										
	void	LoadScheme(const char* pScheme);	
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	ShowWindow(int nNum);
	void 	OnOK();
	void 	PaintWindow();
private:
	static KUiChooseFace* m_pSelf;
private:
	KWndButton	m_OneFemaleBtn, m_TwoFemaleBtn, m_ThreeFemaleBtn, m_FourFemaleBtn, m_FiveFemaleBtn;
	KWndButton	m_SixFemaleBtn, m_SevenFemaleBtn, m_EightFemaleBtn, m_NineFemaleBtn, m_TenFemaleBtn;
	KWndButton	m_ElevenFemaleBtn, m_TwelveFemaleBtn;
	KWndButton	m_OneMaleBtn, m_TwoMaleBtn, m_ThreeMaleBtn, m_FourMaleBtn, m_FiveMaleBtn;
	KWndButton	m_SixMaleBtn, m_SevenMaleBtn, m_EightMaleBtn, m_NineMaleBtn, m_TenMaleBtn;
	KWndButton	m_ElevenMaleBtn, m_TwelveMaleBtn, m_ThirteenMaleBtn, m_FourteenMaleBtn, m_FifteenMaleBtn;
	KWndButton 	m_CloseBtn, m_OkBtn;
	KWndList				m_List;			// ÉêÇë¼ÓÈëÁÐ±í
	KWndScrollBar			m_Scroll;			// ³ÉÔ±ÁÐ±í¹ö¶¯Ìõ
	char			m_szLoginBg[32];
	int 		nCheck;
};