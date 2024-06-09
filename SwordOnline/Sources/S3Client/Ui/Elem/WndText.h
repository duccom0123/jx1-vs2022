/*****************************************************************************************
//	界面窗口体系结构--文本窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-23
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once
#include "WndWindow.h"

#define	WNDTEXT_ES_HALIGN_CENTRE	0x0001		//水平居中
#define	WNDTEXT_ES_HALIGN_RIGHT		0x0002		//水平靠右对齐
#define	WNDTEXT_ES_VALIGN_CENTRE	0x0004		//垂直居中
#define	WNDTEXT_ES_VALIGN_BOTTOM	0x0008		//垂直靠底对齐
#define	WNDTEXT_ES_HALIGN_FILTER	(WNDTEXT_ES_HALIGN_RIGHT | WNDTEXT_ES_HALIGN_CENTRE)
#define	WNDTEXT_ES_VALIGN_FILTER	(WNDTEXT_ES_VALIGN_BOTTOM | WNDTEXT_ES_VALIGN_CENTRE)
#define	WNDTEXT_ES_ALIGN_FILTER		(WNDTEXT_ES_HALIGN_FILTER | WNDTEXT_ES_VALIGN_FILTER)

#define	WNDTEXT_ES_MULTILINE		0x0010		//多行


//=================================
//		文本窗口基础类
//=================================
class KWndText : public KWndWindow
{
public:
	KWndText();
	virtual int		Init(KIniFile* pIniFile, const char* pSection);//初始化
	virtual int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
	virtual void	PaintWindow();							//绘制窗口
	int				GetText(char* pBuffer, int nSize);		//获取字符串内容
	void			Clear(bool bSafe = false);
	void			SetText(const char* pText, int nLen = -1);	//设置文本文字
	void			SetTextColor(unsigned int Color);		//设置文本文字颜色
	void			SetBorderTextColor(unsigned int Color);		//设置文本文字颜色
	void			SetIntText(int nNumber, char Separator = 0);
	void			SetMoneyUnitText(int nMoneyUnit);
	void			SetMoneyUnitPrice(int nPrice, int nMoneyUnit);
	void			SetExtPointText(int nNumber);
	void			Set3IntText(int nNumber, bool bRound = false, const char* szUnit="lng");
	void			Set4IntText(int nNumber);
	void			Set5IntText(int nNumber, bool bFlag = false);//设置文本串的内容为与表述所整数的字符串
	void			Set6IntText(int nNumber, char Separator = 0);
	void			Set6IntText(int nNumber1, int nNumber2, char Separator1 = 0, char Separator2 = 0);
	void			SetResistPlus(int nNumber, char Separator = 0);
	//设置文本串的内容为与表述所给两个整数的字符串，两个整数间以指定的字符分割
	void			Set2IntText(int nNumber1, int nNumber2, char Separator);
	void			Set2IntText2(int nNumber1, int nNumber2, char Separator1, char Separator2);
	int				SetTopLine(int nTopLine);				//设置从文本串的第几行开始显示
	int				GetLineCount() { return m_nLineCount; }	//get line count of the text content
	void			Clone(KWndText* pCopy);
	void SetHAlign(unsigned int nType)
	{
		m_Style |= nType;		
	};
	void SetFontSize(int nFontSize);
	int				GetFontSize() { return m_nFontSize; }
	virtual const char*	GetShortKey() {return NULL;}	//取快捷键,用于Tip显示
	static void		SetAllButtonTipTextColor(unsigned int uColor);	//设置所有按钮提示名称文字的颜色
	static void		EnableAllButtonTip(int bEnable);		
	virtual int		GetToolTipInfo(char* szTip, int nMax);
	int		SetToolTipInfo(char* szTip, int nMax);
	void	SetToolTipHover(bool bFlag) {m_bTipHover = bFlag;};
protected:
	void			SetTextPtr(char* pText, int nBuffLen);	//设置文本缓冲区指针

private:
	int				m_nFontSize;		//字体大小
	unsigned int	m_TextColor;		//文字颜色
	unsigned int	m_BorderColor;		//文字边缘颜色

	char*			m_pText;			//文本缓冲区
	int				m_nBuffLen;			//文本缓冲区大小
	int				m_nTextLen;			//字符串的存储长度	
	int				m_nTopLine;			//显示的头一行的行号，从0开始
	int				m_nLineCount;		//文本总的行数目
	char			m_szTip[256];
	int				m_nTipLen;
	bool			m_bTipHover;
private:
	static unsigned int	ms_uBtnTipTextColor;	//按钮提示名称文字的颜色
	static int			ms_nDisableBtnTip;		//是否禁止按钮的提示文字
};

//=================================
//	文本缓冲区长度为32的文本窗口类
//=================================
class KWndText32 : public KWndText
{
private:
	char			m_Text[32];
public:
	KWndText32();
};

//=================================
//	文本缓冲区长度为80的文本窗口类
//=================================
class KWndText80 : public KWndText
{
private:
	char			m_Text[80];
public:
	KWndText80();
};

//=================================
//	文本缓冲区长度为256的文本窗口类
//=================================
class KWndText256 : public KWndText
{
private:
	char			m_Text[256];
public:
	KWndText256();
};

//=================================
//	文本缓冲区长度为256的文本窗口类
//=================================
class KWndText512 : public KWndText
{
private:
	char			m_Text[512];
public:
	KWndText512();
};

