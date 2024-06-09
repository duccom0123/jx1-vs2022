/*****************************************************************************************
//	界面窗口体系结构-纯文字按钮
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-1-19
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "WndWindow.h"
#include "../../../core/src/gamedatadef.h"

#define	WNDPTB_MAX_TEXT_LEN	64

#define	WNDPTB_S_CENTRE_ALIGN	0x00008000

#define	WNDPTB_F_BE_PRESSEDDOWN	0x00000001
#define	WNDPTB_F_OVER			0x00000002
#define	WNDPTB_F_CHECK			0x00000004

class KWndPureTextBtn : public KWndWindow
{
public:
	KWndPureTextBtn();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);	//初始化
	virtual void	PaintWindow();									//绘制窗口
	void			SetText(const char* pText, int nLen = -1);		//设置文本文字
	int				GetTextLen();
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	void			CheckButton(int bChecked);
	void			Clone(KWndPureTextBtn* pCopy);
	BOOL            IsChecked();
	void			Set3IntText(int nNumber1, int nNumber2, char Separator, char* Find);
	virtual int		GetToolTipInfo(char* szTip, int nMax);
	int				SetToolTipInfo(char* szTip, int nMax);	
	virtual const char*	GetShortKey() {return NULL;}	//取快捷键,用于Tip显示
	void SetTextColor(unsigned int Color)
	{
		m_NormalColor = Color;
	}	
	void SetColor(unsigned int NormalColor, unsigned int OverColor, unsigned int PressedColor)
	{
		m_NormalColor = NormalColor;
		m_OverColor = OverColor;
		m_PressedColor = PressedColor;
	}	
private:
	void			AdjustPosition();
private:
	unsigned int	m_Flag;
	char			m_sText[WNDPTB_MAX_TEXT_LEN];
	int				m_nTextLen;			//字符串的存储长度
	unsigned int	m_NormalColor;
	unsigned int	m_OverColor;
	unsigned int	m_PressedColor;
	unsigned int	m_NormalBorderColor;
	unsigned int	m_OverBorderColor;
	unsigned int	m_PressedBorderColor;
	int				m_nFontSize;
	char			m_szTip[256];
	int				m_nTipLen;
	static unsigned int	ms_uBtnTipTextColor;	//按钮提示名称文字的颜色
public:
	char			m_pItem[MAX_SENTENCE_LENGTH];
	int				m_X;
	int				m_Y;
	bool			m_bHaveItem;//
	void			SetItem(const char* pItem)
	{
		strcpy(m_pItem, pItem);
	};
};
