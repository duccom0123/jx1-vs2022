/*****************************************************************************************
//	界面--消息窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-14
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiInformation2.h"
#include "../../../core/src/coreshell.h"
#include "../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI	"UiInformation2.ini"

KUiInformation2	g_UiInformation2;

void UIMessageBox2(const char* pMsg, int nMsgLen, BOOL bNeedConfirmNotify)
{
	g_UiInformation2.Show(pMsg, nMsgLen, bNeedConfirmNotify);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiInformation2::KUiInformation2()
{
	m_pWordDataList = NULL;
	m_nNumWordData = 0;
	m_nCurrentWord = 0;
}

//显示对话内容
void KUiInformation2::SpeakWords(KUiInformationParam* pWordDataList, int nCount)
{
	if (pWordDataList == NULL || nCount <= 0)
		return;
	KUiInformationParam* pNewList = (KUiInformationParam*)malloc(sizeof(KUiInformationParam) *(nCount + m_nNumWordData - m_nCurrentWord));
	if (pNewList == NULL)
		return;
	if (m_nNumWordData > m_nCurrentWord)
	{
		memcpy(pNewList, m_pWordDataList, sizeof(KUiInformationParam) * (m_nNumWordData - m_nCurrentWord));
		m_nCurrentWord = m_nNumWordData - m_nCurrentWord;
	}
	else
		m_nCurrentWord = 0;
	memcpy(&pNewList[m_nCurrentWord], pWordDataList, sizeof(KUiInformationParam) * nCount);
	m_nNumWordData = m_nCurrentWord + nCount;

	if (m_pWordDataList)
		free(m_pWordDataList);
	m_pWordDataList = pNewList;

	if (IsVisible() == false)
	{
		Show(m_pWordDataList->sInformation, m_pWordDataList->nInforLen, m_pWordDataList->bNeedConfirmNotify);
		m_nCurrentWord ++;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiInformation2::Initialize()
{
	AddChild(&m_Information);
	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiInformation2::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		KWndShowAnimate::Init(&Ini, "Main");
		m_Information .Init(&Ini, "Info");
	}
}

//--------------------------------------------------------------------------
//	功能：显示窗口
//--------------------------------------------------------------------------
void KUiInformation2::Show(const char* pInformation, int nInforLen, BOOL bNeedConfirmNotify)
{
	if (pInformation)
	{
		m_Information.SetText(pInformation, nInforLen);
		m_bNeedConfirmNotify = bNeedConfirmNotify;
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		BringToTop();
		KWndShowAnimate::Show();
		Wnd_SetExclusive((KWndWindow*)this);
	}
}

//--------------------------------------------------------------------------
//	功能：隐藏窗口
//--------------------------------------------------------------------------
void KUiInformation2::Hide()
{
	if (m_bNeedConfirmNotify)
	{
		g_pCoreShell->OperationRequest(GOI_INFORMATION_CONFIRM_NOTIFY, 0, 0);
		m_bNeedConfirmNotify = FALSE;
	}
	Wnd_ReleaseExclusive((KWndWindow*)this);
	if (m_nCurrentWord < m_nNumWordData)
	{
		Show(m_pWordDataList[m_nCurrentWord].sInformation,
			m_pWordDataList[m_nCurrentWord].nInforLen, m_pWordDataList[m_nCurrentWord].bNeedConfirmNotify);
		m_nCurrentWord ++;
	}
	else
	{
		KWndShowAnimate::Hide();
	}
	if (m_pWordDataList && m_nCurrentWord >= m_nNumWordData)
	{
		m_nNumWordData = 0;
		m_nCurrentWord = 0;
		free(m_pWordDataList);
		m_pWordDataList = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiInformation2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
	case WM_LBUTTONDOWN:
//		if (uParam == (unsigned int)(KWndWindow*)&m_OKBtn)
		Hide();
		break;
	case WM_KEYDOWN:
		if (uParam  == VK_RETURN || uParam == VK_SPACE)
		{
			Hide();
			nRet = true;
		}
		else if (uParam == VK_ESCAPE)
		{
			while (IsVisible())
				Hide();
			Hide();
			nRet = true;
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

//关闭窗口，不通知调用窗口
void KUiInformation2::Close()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndShowAnimate::Hide();
	if (m_pWordDataList)
	{
		free(m_pWordDataList);
		m_pWordDataList = NULL;
	}
	m_nNumWordData = 0;
	m_nCurrentWord = 0;
}

KUiInformation2::~KUiInformation2()
{
	Close();
}