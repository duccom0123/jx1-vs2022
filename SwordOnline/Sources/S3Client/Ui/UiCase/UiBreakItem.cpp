// -------------------------------------------------------------------------
//	ÎÄ¼þÃû		£º	½»Ò×È·ÈÏ¿ò
//	´´½¨Õß		£º	Wooy(Wu yue)
//	´´½¨Ê±¼ä	£º	2003-5-28
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiBreakItem.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"UiBreakItem.ini"

KUiBreakItem* KUiBreakItem::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiBreakItem* KUiBreakItem::OpenWindow(KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pPriceInfo, int nStackNum)
{
	if (pObj == NULL)
		return NULL;
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiBreakItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_Title.SetText("NhËp sè l­îng");
		if (nStackNum > 0)
			m_pSelf->m_nMaxNum = nStackNum - 1;
		else
			m_pSelf->m_nMaxNum = 0;
		m_pSelf->m_Increase.Enable(true);
		m_pSelf->m_Decrease.Enable(true);
		m_pSelf->m_CheckButton.CheckButton(false);
		m_pSelf->m_ItemInfo = *pObj;
		m_pSelf->m_PriceInfo = *pPriceInfo;
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiBreakItem::KUiBreakItem()
{
	m_PriceInfo.szItemName[0] = 0;
	m_nMaxNum = 0;
	m_ItemInfo.Obj.uGenre = CGOG_NOTHING;
}

//Èç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
KUiBreakItem* KUiBreakItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±ÕÏú»Ù´°¿Ú
//--------------------------------------------------------------------------
void KUiBreakItem::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}


//ÏÔÊ¾´°¿Ú
void KUiBreakItem::Show()
{
	m_Number.SetIntText(1);
	m_ItemName.SetText(m_PriceInfo.szItemName);
	int Left, Top;
	ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	KWndImage::Show();
	Wnd_SetFocusWnd(&m_Number);
	Wnd_SetExclusive((KWndWindow*)this);
}

//Òþ²Ø´°¿Ú
void KUiBreakItem::Hide()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
int KUiBreakItem::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_ItemName);
	AddChild(&m_Increase);
	AddChild(&m_Decrease);		
	AddChild(&m_Number);
	AddChild(&m_CheckButton);
	AddChild(&m_Button);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	Wnd_AddWindow(this);
	
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	return true;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiBreakItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_Title.Init(&Ini, "Title");
		m_ItemName.Init(&Ini, "ItemName");
		m_Increase.Init(&Ini, "Increase");
		m_Decrease.Init(&Ini, "Decrease");
		m_CheckButton.Init(&Ini, "CheckButton");
		m_CheckButton.Init(&Ini, "Button");
		m_OkBtn.Init(&Ini, "OkBtn");
		m_CancelBtn.Init(&Ini, "CancelBtn");
		m_Number.Init(&Ini,"StringInput");
		m_Number.SetIntText(1);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿ÚÏûÏ¢º¯Êý
//--------------------------------------------------------------------------
int KUiBreakItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
		{
			OnOk();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Increase)
			OnIncrease();
		else if (uParam == (unsigned int)(KWndWindow*)&m_Decrease)
			OnDecrease();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CheckButton)
		{
		    if (m_CheckButton.IsButtonChecked())
			{
			    m_CheckButton.CheckButton(true);
			}	
			else
			{
			    m_CheckButton.CheckButton(false);
			}
		}	
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnOk();
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCheckInput();
		break;
	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiBreakItem::OnCancel()
{
	CloseWindow(false);
}

void KUiBreakItem::OnCheckInput()
{
	int nNumber = m_Number.GetIntNumber();
	if (nNumber <= 0)
		nNumber = 0;
	else if (nNumber >= m_nMaxNum)
		nNumber = m_nMaxNum;
	char	szBuff1[16], szBuff2[16];
	itoa(nNumber, szBuff1, 10);
	m_Number.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_Number.SetIntText(nNumber);
}


void KUiBreakItem::OnIncrease()
{	
	int nNumber = m_Number.GetIntNumber();
	nNumber++;
	if(nNumber > m_nMaxNum)
		nNumber = m_nMaxNum;
		
	m_Number.SetIntText(nNumber);
	Wnd_SetFocusWnd(&m_Number);
}

//-----------------------
void KUiBreakItem::OnDecrease()
{	
	int nNumber = m_Number.GetIntNumber();
	nNumber--;
	if (nNumber < 0)
		nNumber = m_nMaxNum;
	m_Number.SetIntText(nNumber);
	Wnd_SetFocusWnd(&m_Number);
}

void KUiBreakItem::OnOk()
{
	if (g_pCoreShell)
	{
		int nNumber = m_Number.GetIntNumber();
		g_pCoreShell->BreakItem((unsigned int)(&m_ItemInfo), nNumber, m_CheckButton.IsButtonChecked());
	}
	CloseWindow(false);
}