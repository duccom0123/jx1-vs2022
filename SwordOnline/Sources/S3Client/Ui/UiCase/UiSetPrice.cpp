#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiSetPrice.h"
#include "../UiSoundSetting.h"
#include "UiPlayerShop.h"
#include "UiInformation.h"
#include "../Elem/WndText.h"
#include "../../../core/src/coreshell.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"UiSetPrice.ini"

KUiSetPrice* KUiSetPrice::m_pSelf = NULL;

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiSetPrice* KUiSetPrice::OpenWindow(int nMoney, KUiObjAtContRegion* pObj, KUiItemBuySelInfo* pPriceInfo)
{
	if (pPriceInfo == NULL)
		return NULL;
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSetPrice;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_ItemInfo = *pObj;
		m_pSelf->m_PriceInfo = *pPriceInfo;
		m_pSelf->m_Number.SetIntText(nMoney);
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();

		Wnd_SetFocusWnd(&m_pSelf->m_Number);
	}
	return m_pSelf;
}

KUiSetPrice::KUiSetPrice()
{
	m_PriceInfo.szItemName[0] = 0;
	m_PriceInfo.nCurPrice = 0;
	m_ItemInfo.Obj.uGenre = CGOG_NOTHING;
}

KUiSetPrice* KUiSetPrice::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiSetPrice::CloseWindow(bool bDestroy)
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


void KUiSetPrice::Show()
{
    m_ItemName.SetText(m_PriceInfo.szItemName);
    int Left, Top;
    ALW_GetWndPosition(Left, Top, m_Width, m_Height);
    SetPosition(Left, Top);
    KWndImage::Show();
    Wnd_SetExclusive((KWndWindow*)this);

}

void KUiSetPrice::Hide()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int KUiSetPrice::Initialize()
{
	AddChild(&m_ItemName);
	AddChild(&m_Number);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	Wnd_AddWindow(this);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	return true;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiSetPrice::LoadScheme(const char* pScheme)
{
    char        Buff[128];
    KIniFile    Ini;
    sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
    if (Ini.Load(Buff))
    {
        Init(&Ini, "Main");
        m_ItemName.Init(&Ini, "ItemName");
        m_Number.Init(&Ini, "BuyNumber");      
        m_OkBtn.Init(&Ini, "OkBtn");
        m_CancelBtn.Init(&Ini, "CancelBtn");
    }
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int KUiSetPrice::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
			OnSetPrice();
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			OnSetPrice();
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

void KUiSetPrice::OnCheckInput()
{	
	DWORD nMoney = m_Number.GetIntNumber();
	if (nMoney < 0)
		nMoney = 0;
	else if (nMoney > 200000000)
		nMoney = 200000000;
	char	szBuff1[16], szBuff2[16];
	itoa(nMoney, szBuff1, 10);
	m_Number.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
		m_Number.SetIntText(nMoney);
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

void KUiSetPrice::OnSetPrice()
{
	int nMoney = m_Number.GetIntNumber();

	g_pCoreShell->OperationRequest(GOI_PLAYER_SETPRICE,
					(unsigned int)(&m_ItemInfo), nMoney);

	CloseWindow(true);
}

void KUiSetPrice::OnCancel()
{
	g_pCoreShell->OperationRequest(GOI_PLAYER_SETPRICE,
					(unsigned int)(&m_ItemInfo), 0);

	CloseWindow(true);
}