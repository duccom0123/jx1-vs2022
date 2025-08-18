#include "KWin32.h"
#include "KCore.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiVirtualKeyboard.h"
#include "../../Login/Login.h"
#include "../UiSoundSetting.h"
#include "UiConnectInfo.h"
#include <crtdbg.h>
#include "../../../core/src/coreshell.h"

#define	SCHEME_INI				"UiVirtualKeyboard.ini"

extern iCoreShell*		g_pCoreShell;
KUiVirtualKeyboard* KUiVirtualKeyboard::m_pSelf = NULL;

static const char* szArrayKeyFormat[MAX_KEYNUM_FORMAT+MAX_KEYLETTER_FORMAT+MAX_KEYCHAR_FORMAT][2] = 
{
	{"49","33"},{"50","64"},{"51","35"},{"52","36"},{"53","37"},{"54","94"},{"55","38"},{"56","42"},
	{"57","40"},{"48","41"},{"97","65"},{"98","66"},{"99","67"},{"100","68"},{"101","69"},{"102","70"},
	{"103","71"},{"104","72"},{"105","73"},{"106","74"},{"107","75"},{"108","76"},{"109","77"},{"110","78"},
	{"111","79"},{"112","80"},{"113","81"},{"114","82"},{"115","83"},{"116","84"},{"117","85"},{"118","86"},
	{"119","87"},{"120","88"},{"121","89"},{"122","90"},{"96","126"},{"45","95"},{"61","43"},{"91","123"},
	{"93","125"},{"92","124"},{"59","58"},{"39","34"},{"44","60"},{"46","62"},{"47","63"},
};
//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KUiVirtualKeyboard::KUiVirtualKeyboard()
{
	m_pWndWindow = NULL;
	m_pWndEdit = NULL;
	m_bPressDownShift = false;
	m_nFrameFly = 0;
	m_bInsertCount = false;
	m_nInsertCount = 0;
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KUiVirtualKeyboard::~KUiVirtualKeyboard()
{
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiVirtualKeyboard* KUiVirtualKeyboard::OpenWindow(bool rd)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiVirtualKeyboard;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->m_nInsertCount = 0;
		m_pSelf->ArrangeKey(rd,m_pSelf->m_nFrameFly);
        m_pSelf->Show();
	}	
    return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiVirtualKeyboard::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

KUiVirtualKeyboard* KUiVirtualKeyboard::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiVirtualKeyboard::Initialize()
{
	int i;
	for (i=0; i<MAX_KEYNUM_FORMAT+MAX_KEYLETTER_FORMAT+MAX_KEYCHAR_FORMAT; i++)
		AddChild(&m_KeyNormal[i]);
	for (i=0; i<KeySpecialNum; i++)
		AddChild(&m_KeySpecial[i]);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiVirtualKeyboard::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		
		int i;
		char Key[128], TempKey[128];
		int nWidth, nHeight, nLeft, nTop, Width;
		GetAbsolutePos(&nLeft, &nTop);
		GetSize(&Width, NULL);
		Ini.GetInteger("Main", "FrameFly", 9, &m_nFrameFly);
		Ini.GetInteger("Key", "KeyWidth", 24, &nWidth);
		Ini.GetInteger("Key", "KeyHeight", 24, &nHeight);
		Ini.GetString("Key", "KeyNumFormat", "", Key, sizeof(Key));
		for (i=0; i<MAX_KEYNUM_FORMAT; i++)
		{
			sprintf(TempKey,Key, atoi(szArrayKeyFormat[i][0]));
			m_KeyNormal[i].Init(&Ini, "KeyDesc");
			m_KeyNormal[i].SetImage(ISI_T_SPR, TempKey, true);
		}

		Ini.GetString("Key", "KeyLetterFormat", "", Key, sizeof(Key));
		for (i; i<MAX_KEYNUM_FORMAT+MAX_KEYLETTER_FORMAT; i++)
		{
			sprintf(TempKey, Key, atoi(szArrayKeyFormat[i][0]));
			m_KeyNormal[i].Init(&Ini, "KeyDesc");
			m_KeyNormal[i].SetImage(ISI_T_SPR, TempKey, true);
		}
		
		Ini.GetString("Key", "KeyCharFormat", "", Key, sizeof(Key));
		int count =0;
		for (i; i<MAX_KEYNUM_FORMAT+MAX_KEYLETTER_FORMAT+MAX_KEYCHAR_FORMAT; i++)
		{
			sprintf(TempKey, Key, count);
			m_KeyNormal[i].Init(&Ini, "KeyDesc");
			m_KeyNormal[i].SetImage(ISI_T_SPR, TempKey, true);
			count++;
		}
		Ini.GetString("Key", "KeyShift", "", TempKey, sizeof(TempKey));
		m_KeySpecial[KeyShift].Init(&Ini, "KeyDesc");
		m_KeySpecial[KeyShift].SetImage(ISI_T_SPR, TempKey, true);
		Ini.GetString("Key", "KeyBack", "", TempKey, sizeof(TempKey));
		m_KeySpecial[KeyBack].Init(&Ini, "KeyDesc");
		m_KeySpecial[KeyBack].SetImage(ISI_T_SPR, TempKey, true);
		Ini.GetString("Key", "KeyRange", "", TempKey, sizeof(TempKey));
		m_KeySpecial[KeyRange].Init(&Ini, "KeyDesc");
		m_KeySpecial[KeyRange].SetImage(ISI_T_SPR, TempKey, true);
		Ini.GetString("Key", "ReRangeInfo", "", TempKey, sizeof(TempKey));
		m_KeySpecial[KeyRange].SetToolTipInfo(TempKey, strlen(TempKey));
	}
}

bool KUiVirtualKeyboard::FindSame(int a[],int n,int x)
{ 
    int i; 
    if(n==0)return false; 
    for(i=0;i<n;++i) 
        if(a[i]==x)return true; 
    return false; 
} 


void KUiVirtualKeyboard::Gen_Keyboard(bool rd, int t[],int n,int a,int b)
{ 
	
    int i, x, min, max; 
	if (b){
		min=a>0 && a<b ? a:0;
		max=b>a ? b:a;
	}
	else{
		min=0;
		max=a;
	}
	if (max<n) max=n;
	max-=1;
    x = rd?GetRandomNumber(min,max):0; 
    for(i = 0; i < n; ++i) 
    { 
		if(rd)
		{
        while (FindSame(t,i,x)) 
            x = GetRandomNumber(min,max); 
		}
		else
			x=i;
        t[i] = x; 
    }
}

void KUiVirtualKeyboard::ArrangeKey(bool rd, int nFrameFly)
{
	int uArrangeTab[][KeyFormatNum] = 
	{
		{KeyNormal,KeySpecial},
		{KeySpecial,KeyNormal},
	};

	int nArrange = rd?GetRandomNumber(0,sizeof(uArrangeTab)/sizeof(uArrangeTab[0])-1):0;
	int* a;
	int i, j, WIDTH, w, h;
	int nLeft = nFrameFly;
	int nTop = nFrameFly;
	GetSize(&WIDTH, NULL);
	for (j = 0; j < sizeof(uArrangeTab[nArrange])/sizeof(int); j++)
	{
		switch (uArrangeTab[nArrange][j])
		{
			case KeySpecial:
				a = new int[KeySpecialNum];
				Gen_Keyboard(rd,a,KeySpecialNum,KeySpecialNum);
				for (i = 0; i < KeySpecialNum; i ++)
				{
					m_KeySpecial[a[i]].SetPosition(nLeft, nTop);
					m_KeySpecial[a[i]].GetSize(&w, &h);
					m_KeySpecial[a[i]].SetValue(a[i]);
					nLeft+=w+(i == KeySpecialNum-1 ? 2:4);
					if (nLeft + nFrameFly*2 >= WIDTH)
					{
						nLeft = nFrameFly;
						nTop += h+4;
					}
				}
				break;
			case KeyNormal:
				int tong = MAX_KEYNUM_FORMAT+MAX_KEYLETTER_FORMAT+MAX_KEYCHAR_FORMAT;
				a = new int[tong];
				Gen_Keyboard(rd,a,tong,tong);
				for (i = 0; i < tong; i ++)
				{
					m_KeyNormal[a[i]].SetPosition(nLeft, nTop);
					m_KeyNormal[a[i]].GetSize(&w, &h);
					m_KeyNormal[a[i]].SetValue(a[i]);
					nLeft+=w+2;
					if (nLeft + nFrameFly*2 >= WIDTH)
					{
						nLeft = nFrameFly;
						nTop += h+2;
					}
				}
				delete[] a;
				break;
		}
	}
}

void KUiVirtualKeyboard::SetLinkToInsert(KWndWindow *pWndWindow, KWndEdit *pWndEdit, bool bInsert)
{
	if (m_pSelf && pWndWindow && pWndEdit)
	{
		m_pSelf->m_pWndWindow = pWndWindow;
		m_pSelf->m_pWndEdit = pWndEdit;
		m_pSelf->m_bInsertCount = bInsert;
	}
}

void KUiVirtualKeyboard::Breathe()
{
	if(!m_pWndWindow || m_pWndWindow->IsVisible()==false)
	{
		m_pWndWindow = NULL;
		CloseWindow(false);
	}
}

BOOL KUiVirtualKeyboard::VirtualKeyboardAgree(KWndWindow *pWnd)
{
	if (m_pSelf && pWnd)
	{
		if (m_pSelf->m_nInsertCount < 3)
			return FALSE;
	}
	return TRUE;
}
//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiVirtualKeyboard::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		int i;
		if (m_pWndWindow && m_pWndWindow->IsVisible() && m_pWndEdit)
		{
			for (i = 0; i < KeySpecialNum; i ++)
			{
				if (uParam == (unsigned int)&m_KeySpecial[i])
				{
					switch (m_KeySpecial[i].GetValue())
					{
						case KeyShift:
							m_bPressDownShift = !m_bPressDownShift;
							m_KeySpecial[KeyShift].CheckButton(m_bPressDownShift);
							break;
						case KeyBack:
							if (m_pWndEdit)
							{
								char szKeyString[32];
								m_pWndEdit->GetText(szKeyString, sizeof(szKeyString), true);
								if (szKeyString[0])
								{
									m_pWndEdit->SetText(szKeyString, strlen(szKeyString)-1);
									if (m_bInsertCount)
										m_nInsertCount--;
								}
							}								
							break;
						case KeyRange:
							ArrangeKey(true,m_nFrameFly);
							break;
					}
					Wnd_SetFocusWnd(m_pWndEdit);
				}
			}				
			for (i = 0; i < MAX_KEYNUM_FORMAT+MAX_KEYLETTER_FORMAT+MAX_KEYCHAR_FORMAT; i ++)
			{
				if (uParam == (unsigned int)&m_KeyNormal[i])
				{
					char Key[6];
					sprintf(Key,"%c", atoi(szArrayKeyFormat[m_KeyNormal[i].GetValue()][m_bPressDownShift]));
					m_pWndEdit->InsertString(Key, strlen(Key));
					if (m_bInsertCount)
						m_nInsertCount++;
					Wnd_SetFocusWnd(m_pWndEdit);
					return 0;
				}
			}
		}
		else
			CloseWindow(true);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}
