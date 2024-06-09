/*****************************************************************************************
//	界面--消息窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-14
------------------------------------------------------------------------------------------
*****************************************************************************************/
#pragma once

#include "../Elem/WndText.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndShowAnimate.h"

struct KUiInformationParam;

class KUiInformation2 : protected KWndShowAnimate
{
public:
	void	Initialize();							//初始化
	void	LoadScheme(const char* pScheme);		//载入界面方案
	void	Show(const char* pInformation, int nInforLen, BOOL bNeedConfirmNotify = TRUE);//显示窗口

	void	SpeakWords(KUiInformationParam* pWordDataList, int nCount);	//显示对话内容
	KWndWindow* TopChildFromPoint(int x, int y) {
		return this;
	}
	void	Hide();									//隐藏窗口
	void	Close();
	KUiInformation2();
	~KUiInformation2();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);//窗口函数
private:
	KWndText512			m_Information;

	KUiInformationParam*	m_pWordDataList;
	int						m_nNumWordData;
	int						m_nCurrentWord;
	BOOL					m_bNeedConfirmNotify;
};

void UIMessageBox2(const char* pMsg, int nMsgLen = -1);

extern KUiInformation2	g_UiInformation2;