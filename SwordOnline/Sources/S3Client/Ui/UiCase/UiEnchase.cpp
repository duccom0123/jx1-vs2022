#pragma once

#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../UiSoundSetting.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../UiBase.h"
#include <crtdbg.h>
#include "UiSysMsgCentre.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/PopupMenu.h"
#include "UiItem.h"
#include "UiEnchase.h"

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;

#define 	SCHEME_INI			"UiEnchase.ini"

static struct UE_CTRL_MAP
{
	int				nPosition;
	const char*		pIniSection;
}CtrlItemMap[MAX_COMPOUND_ITEM] =
{
	{ MOSAICENCRUSTED_UIEP_BOX_1,		"Box1"		},
	{ MOSAICENCRUSTED_UIEP_BOX_2,		"Box2"		},
	{ MOSAICENCRUSTED_UIEP_BOX_3,		"Box3"		}
};


KUiEnchase*	KUiEnchase::m_pSelf = NULL;

KUiEnchase::KUiEnchase()
{
	m_nCurPage = UPGRADE_PAGE;
}

KUiEnchase::~KUiEnchase()
{
}

KUiEnchase* KUiEnchase::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiEnchase;
		if (m_pSelf)
		{
			m_pSelf->Initialize();
		}
	}
	if (m_pSelf)
	{
		if(KUiItem::GetIfVisible() ==FALSE)
			KUiItem::OpenWindow();
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->m_nCurPage = UPGRADE_PAGE;
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

void KUiEnchase::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);

		if(bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
		{
			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_LOCKMOVE, 0, 0);
		}
	}
}

KUiEnchase* KUiEnchase::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiEnchase::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_UpgradeBtn.Init(&Ini, "UpgradeBtn");
			m_pSelf->m_CompoundBtn.Init(&Ini, "CompoundBtn");
			m_pSelf->m_DistillBtn.Init(&Ini, "DistillBtn");
			m_pSelf->m_ForgeBtn.Init(&Ini, "ForgeBtn");
			m_pSelf->m_EnchaseBtn.Init(&Ini, "EnchaseBtn");
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		}
	}
}

void KUiEnchase::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, sizeof(Scheme));
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", Scheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Ini.GetString("RuleInfo", "Compound", "", m_CompoundPad.m_szTip[CPA_FUSION], sizeof(m_CompoundPad.m_szTip[CPA_FUSION]));
		Ini.GetString("RuleInfo", "UpCryolite", "", m_CompoundPad.m_szTip[CPA_CRYOLITE], sizeof(m_CompoundPad.m_szTip[CPA_CRYOLITE]));
		Ini.GetString("RuleInfo", "UpPropMine", "", m_CompoundPad.m_szTip[CPA_PROPMINE], sizeof(m_CompoundPad.m_szTip[CPA_PROPMINE]));
		Ini.GetString("RuleInfo", "Distill", "", m_DistillPad.m_szTip, sizeof(m_DistillPad.m_szTip));
		Ini.GetString("RuleInfo", "Forge", "", m_ForgePad.m_szTip, sizeof(m_ForgePad.m_szTip));
		Ini.GetString("RuleInfo", "Enchase", "", m_EnchasePad.m_szTip, sizeof(m_EnchasePad.m_szTip));
		Ini.GetString("RuleInfo", "Upgrade", "", m_UpgradePad.m_szTip, sizeof(m_UpgradePad.m_szTip));
	}
	m_UpgradePad.Initialize();
	AddPage(&m_UpgradePad, &m_UpgradeBtn);

	m_CompoundPad.Initialize();
	AddPage(&m_CompoundPad, &m_CompoundBtn);

	m_DistillPad.Initialize();
	AddPage(&m_DistillPad, &m_DistillBtn);
	
	m_ForgePad.Initialize();
	AddPage(&m_ForgePad, &m_ForgeBtn);
	
	m_EnchasePad.Initialize();
	AddPage(&m_EnchasePad, &m_EnchaseBtn);

	AddChild(&m_UpgradeBtn);
	AddChild(&m_CompoundBtn);
	AddChild(&m_DistillBtn);
	AddChild(&m_ForgeBtn);
	AddChild(&m_EnchaseBtn);
	AddChild(&m_CloseBtn);
	
	Wnd_AddWindow(this);
	
	LoadScheme(Scheme);
}

int KUiEnchase::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	if (uMsg == WND_N_BUTTON_CLICK)
	{
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
		{
			CloseWindow(true);
			nResult = true;
		}
		else
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_UpgradeBtn)
			{
				m_nCurPage = UPGRADE_PAGE;	
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_CompoundBtn)
			{
				m_nCurPage = COMPOUND_PAGE;
				m_CompoundPad.PopUp();
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_DistillBtn)
			{
				m_nCurPage = DISTILL_PAGE;
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_ForgeBtn)
			{
				m_nCurPage = FORGE_PAGE;
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_EnchaseBtn)
			{	
				m_nCurPage = ENCHASE_PAGE;
			}
			nResult = KWndPageSet::WndProc(uMsg, uParam, nParam);
		}
	}
	else
	{
		nResult = KWndPageSet::WndProc(uMsg, uParam, nParam);
	}
	return nResult;
}

void KUiEnchase::UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd)
{
	if(m_pSelf)
	{
		switch(m_pSelf->m_nCurPage)
		{
			case UPGRADE_PAGE:
				m_pSelf->m_UpgradePad.UpdateItemObjBox(pItem, bAdd);
				break;
			case COMPOUND_PAGE:
				m_pSelf->m_CompoundPad.UpdateItemObjBox(pItem, bAdd);
				break;
			case DISTILL_PAGE:
				m_pSelf->m_DistillPad.UpdateItemObjBox(pItem, bAdd);
				break;
			case FORGE_PAGE:
				m_pSelf->m_ForgePad.UpdateItemObjBox(pItem, bAdd);
				break;
			case ENCHASE_PAGE:
				m_pSelf->m_EnchasePad.UpdateItemObjBox(pItem, bAdd);
				break;
		}
	}
}

void KUiEnchase::UpdateItemMatrixBox(KUiObjAtRegion* pItem, int bAdd)
{
	if(m_pSelf)
	{
		switch(m_pSelf->m_nCurPage)
		{
			case UPGRADE_PAGE:
				break;
			case COMPOUND_PAGE:
				break;
			case DISTILL_PAGE:
				m_pSelf->m_DistillPad.UpdateItemMatrixBox(pItem, bAdd);
				break;
			case FORGE_PAGE:
				break;
			case ENCHASE_PAGE:
				m_pSelf->m_EnchasePad.UpdateItemMatrixBox(pItem, bAdd);
				break;
		}
	}
}

void KUiEnchase::Breathe()
{
	Wnd_GameSpaceHandleInput(false);

	if(m_nCurPage == COMPOUND_PAGE)
	{
		m_CompoundPad.Breathe();
	}
	else if(m_nCurPage == DISTILL_PAGE)
	{
		m_DistillPad.Breathe();
	}
	else if(m_nCurPage == FORGE_PAGE)
	{
		m_ForgePad.Breathe();
	}
	else if(m_nCurPage == ENCHASE_PAGE)
	{
		m_EnchasePad.Breathe();
	}
	else if(m_nCurPage == UPGRADE_PAGE)
	{
		m_UpgradePad.Breathe();
	}
}


#define 	SCHEME_INI_UPGRADE		"·ÖÒ³Í¼Æ×.ini"


KUiEnchase_Upgrade* KUiEnchase_Upgrade::ms_pSelf = NULL;

KUiEnchase_Upgrade::KUiEnchase_Upgrade()
{
	m_nStatus = STATUS_WAIT;
}

KUiEnchase_Upgrade::~KUiEnchase_Upgrade()
{
}

KUiEnchase_Upgrade* KUiEnchase_Upgrade::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiEnchase_Upgrade;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->m_nStatus = STATUS_WAIT;
		ms_pSelf->BringToTop();
		ms_pSelf->UpdateData();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}

KUiEnchase_Upgrade* KUiEnchase_Upgrade::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiEnchase_Upgrade::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiEnchase_Upgrade::Show()
{
	UpdateDataObjBox();
	m_nStatus = STATUS_WAIT;
	m_GuideList.Clear();
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);
	KWndPage::Show();		
}

void KUiEnchase_Upgrade::Hide()
{
	KWndPage::Hide();
}

void KUiEnchase_Upgrade::Initialize()
{
	AddChild(&m_EquipPos);
	m_EquipPos.SetText("Trang bÞ");
	AddChild(&m_CryolitePos);
	m_CryolitePos.SetText("HuyÒn tinh");
	AddChild(&m_ConsumeEffect);
	m_ConsumeEffect.Hide();
	for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
	{
		m_UpgradeBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_UpgradeBox[i]);
		m_UpgradeBox[i].SetContainerId((int)UOC_COMPOUND);
	}
	AddChild(&m_GuideList);
	AddChild(&m_GuideList_Scroll);
	AddChild(&m_UpgradeBtn);
	AddChild(&m_CancleBtn);
	m_GuideList.SetScrollbar(&m_GuideList_Scroll);
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);	

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	Wnd_AddWindow(this);
}

void KUiEnchase_Upgrade::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_UPGRADE);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		int nPosX = 0, nPosY = 0;
		m_EquipPos.Init(&Ini, "EquipPos");
		Ini.GetInteger2("EquipPos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_EquipPos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_EquipPos.SetBorderTextColor(GetColor(Buff));
		m_EquipPos.SetPosition(nPosX-60, nPosY-5);
		
		m_CryolitePos.Init(&Ini, "CryolitePos");
		Ini.GetInteger2("CryolitePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetBorderTextColor(GetColor(Buff));
		m_CryolitePos.SetPosition(nPosX-30, nPosY-5);

		for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
		{
			m_UpgradeBox[i].Init(&Ini, CtrlItemMap[i].pIniSection);
		}
		m_GuideList.Init(&Ini, "GuideList");
		m_GuideList_Scroll.Init(&Ini, "GuideList_Scroll");
		m_UpgradeBtn.Init(&Ini, "UpgradeBtn");
		m_CancleBtn.Init(&Ini, "CancleBtn");
		m_ConsumeEffect.Init(&Ini, "ConsumeEffect");
	}
}

int KUiEnchase_Upgrade::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch(uMsg)
	{
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))
		{
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		{
			if(uParam == (unsigned int)&m_UpgradeBtn)
			{
				if (m_nStatus != STATUS_WAIT)
					return 0;		
				
				m_nStatus = STATUS_BEGIN;

				nResult = true;
			}
			else if (uParam == (unsigned int)&m_CancleBtn)
			{
				if (m_nStatus != STATUS_WAIT)
				{
					m_nStatus = STATUS_WAIT;
					return 0;
				}
				nResult = true;
			}
		}
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_GuideList_Scroll)
			m_GuideList.SetFirstShowLine(nParam);
		break;
	default:
			nResult = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiEnchase_Upgrade::UpdateDataObjBox()
{
	KUiObjAtRegion	Equips[MAX_COMPOUND_ITEM];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPOUND, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < MAX_COMPOUND_ITEM; i++)
		m_UpgradeBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItemObjBox(&Equips[i], true);
	}
}

void KUiEnchase_Upgrade::UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
		{
			if (CtrlItemMap[i].nPosition == pItem->Region.v)
			{
				if (bAdd)
					m_UpgradeBox[i].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,pItem->Region.Width, pItem->Region.Height);
				else
					m_UpgradeBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}

	}
}

void KUiEnchase_Upgrade::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (m_nStatus != STATUS_WAIT)
		return;
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;
	
	if (pPickPos)
	{
		//_ASSERT(pPickPos->pWnd);
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = 0;
		Pick.eContainer = UOC_COMPOUND;
		pWnd = pPickPos->pWnd;
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;
	
	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = 0;
		Drop.eContainer = UOC_COMPOUND;
	}
	
	for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
	{
		if (pWnd == (KWndWindow*)&m_UpgradeBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}
	
	//_ASSERT(i < _ITEM_COUNT);
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiEnchase_Upgrade::Breathe()
{
	if (m_nStatus == STATUS_WAIT)
	{
		m_ConsumeEffect.Hide();
		m_ConsumeEffect.SetFrame(-1);
	}
	else if(m_nStatus == STATUS_BEGIN)
	{
		m_ConsumeEffect.Show();
		m_ConsumeEffect.SetFrame(0);
		m_nStatus = STATUS_DOING;
	}
	else if (m_nStatus == STATUS_DOING)
	{
		if (!PlayEffect())
		{
			m_nStatus = STATUS_CHANGE;
			m_ConsumeEffect.Hide();
		}
	}
	else if (m_nStatus == STATUS_CHANGE)
	{
		m_ConsumeEffect.Hide();
		m_ConsumeEffect.SetFrame(-1);
		m_nStatus = STATUS_FINISH;
	}
	else if (m_nStatus == STATUS_FINISH)
	{
		m_nStatus = STATUS_WAIT;
	}		
}

int KUiEnchase_Upgrade::PlayEffect()
{
	if(m_ConsumeEffect.GetCurrentFrame() >= 26)
	{
		m_ConsumeEffect.SetFrame(0);
		return 0;
	}
	else
	{
		m_ConsumeEffect.NextFrame();
		return 1;
	}
}


int KGetFarmImage_Upgrade::GetMaxFrame()
{
	return m_Image.nNumFrames;
}


int KGetFarmImage_Upgrade::GetCurrentFrame()
{
	return m_Image.nFrame;
}

#define 	SCHEME_INI_COMPOUND		"·ÖÒ³ºÏ³É.ini"

char szArray_Compound[][64] = 
{
	"LuyÖn chÕ HuyÒn tinh kho¸ng th¹ch",
	"Th¨ng cÊp HuyÒn tinh kho¸ng th¹ch",
	"Th¨ng cÊp Kho¸ng th¹ch thuéc tÝnh",
};

KUiEnchase_Compound* KUiEnchase_Compound::ms_pSelf = NULL;


KUiEnchase_Compound::KUiEnchase_Compound()
{
	m_nStatus = STATUS_WAIT;
}

KUiEnchase_Compound::~KUiEnchase_Compound()
{
}

KUiEnchase_Compound* KUiEnchase_Compound::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiEnchase_Compound;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		ms_pSelf->m_nStatus = STATUS_WAIT;
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->BringToTop();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}

KUiEnchase_Compound* KUiEnchase_Compound::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiEnchase_Compound::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiEnchase_Compound::Show()
{
	ApplyShow(CPA_FUSION);
	m_nStatus = STATUS_WAIT;
	KWndPage::Show();		
}

void KUiEnchase_Compound::Hide()
{
	KWndPage::Hide();
}

void KUiEnchase_Compound::Initialize()
{
	AddChild(&m_Box_0);
	m_Box_0.SetText("NhÉn");

	AddChild(&m_Box_1);
	m_Box_1.SetText("D©y chuyÒn/hé th©n phï");

	AddChild(&m_Box_2);
	m_Box_2.SetText("Ngäc béi/h­¬ng nang");
	
	AddChild(&m_Effect_0);
	m_Effect_0.Hide();
	AddChild(&m_Effect_1);
	m_Effect_1.Hide();
	AddChild(&m_Effect_2);
	m_Effect_2.Hide();
	
	AddChild(&m_GuideList);
	AddChild(&m_GuideList_Scroll);
	AddChild(&m_CompoundBtn);
	AddChild(&m_CancleBtn);
	m_GuideList.SetScrollbar(&m_GuideList_Scroll);
	int nLength = TEncodeText(m_szTip[CPA_FUSION], strlen(m_szTip[CPA_FUSION]));
	m_GuideList.AddOneMessage(m_szTip[CPA_FUSION], nLength);

	for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
	{
		m_CompoundBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_CompoundBox[i]);
		m_CompoundBox[i].SetContainerId((int)UOC_COMPOUND);
	}

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	Wnd_AddWindow(this);
}

void KUiEnchase_Compound::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_COMPOUND);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		int nPosX = 0, nPosY = 0;
		m_Box_0.Init(&Ini, "Box_0");
		Ini.GetInteger2("Box_0", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_Box_0.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_Box_0.SetBorderTextColor(GetColor(Buff));
		m_Box_0.SetPosition(nPosX-10, nPosY-5);
		
		m_Box_1.Init(&Ini, "Box_1");
		Ini.GetInteger2("Box_1", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_Box_1.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_Box_1.SetBorderTextColor(GetColor(Buff));
		m_Box_1.SetPosition(nPosX-75, nPosY-5);
		
		m_Box_2.Init(&Ini, "Box_2");
		Ini.GetInteger2("Box_2", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_Box_2.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_Box_2.SetBorderTextColor(GetColor(Buff));
		m_Box_2.SetPosition(nPosX-60, nPosY-5);

		m_GuideList.Init(&Ini, "GuideList");
		m_GuideList_Scroll.Init(&Ini, "GuideList_Scroll");
		m_CompoundBtn.Init(&Ini, "CompoundBtn");
		m_CancleBtn.Init(&Ini, "CancleBtn");
		
		for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
		{
			m_CompoundBox[i].Init(&Ini, CtrlItemMap[i].pIniSection);
		}
		
		m_Effect_0.Init(&Ini, "Effect_0");
		m_Effect_1.Init(&Ini, "Effect_1");
		m_Effect_2.Init(&Ini, "Effect_2");
	}
}

int KUiEnchase_Compound::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch(uMsg)
	{
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))
		{
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		{
			if(uParam == (unsigned int)&m_CompoundBtn)
			{
				if (m_nStatus != STATUS_WAIT)
					return 0;

				m_nStatus = STATUS_BEGIN;

				nResult = true;
			}
			else if (uParam == (unsigned int)&m_CancleBtn)
			{
				if (m_nStatus != STATUS_WAIT)
				{
					m_nStatus = STATUS_WAIT;
					return 0;
				}
				nResult = true;
			}
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_GuideList_Scroll)
			m_GuideList.SetFirstShowLine(nParam);
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			g_DebugLog("%d",nParam);
			if ((short)(LOWORD(nParam)) >= 0)
			{
				ApplyShow((short)(LOWORD(nParam)));
			}
		}
		break;
	default:
		nResult = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiEnchase_Compound::UpdateData()
{
	KUiObjAtRegion	Equips[MAX_COMPOUND_ITEM];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPOUND, (unsigned int)&Equips, 0);
	int i;
	for (i = 0; i < MAX_COMPOUND_ITEM; i++)
		m_CompoundBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItemObjBox(&Equips[i], true);
	}
}

void KUiEnchase_Compound::UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
		{
			if (CtrlItemMap[i].nPosition == pItem->Region.v)
			{
				if (bAdd)
					m_CompoundBox[i].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,
					pItem->Region.Width, pItem->Region.Height);
				else
					m_CompoundBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}
	}
}

void KUiEnchase_Compound::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (m_nStatus != STATUS_WAIT)
		return;
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;
	
	if (pPickPos)
	{
		//_ASSERT(pPickPos->pWnd);
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = 0;
		Pick.eContainer = UOC_COMPOUND;
		pWnd = pPickPos->pWnd;
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = 0;
		Drop.eContainer = UOC_COMPOUND;
	}
	
	for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
	{
		if (pWnd == (KWndWindow*)&m_CompoundBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}

	//_ASSERT(i < _ITEM_COUNT);

	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiEnchase_Compound::PopUp()
{
	int nActionDataCount = sizeof(szArray_Compound) / sizeof(szArray_Compound[0]);
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	
	for (int i = 0; i < nActionDataCount; i++)
	{
		strncpy(pSelUnitMenu->Items[i].szData, szArray_Compound[i], sizeof(szArray_Compound[i]));
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	int Left, Top;
	GetAbsolutePos(&Left, &Top);
	pSelUnitMenu->nX = Left;
	pSelUnitMenu->nY = Top;
	KPopupMenu::Popup(pSelUnitMenu, this, 0);
}

void KUiEnchase_Compound::ApplyShow(int nAction)
{
	switch(nAction)
	{
	case CPA_FUSION:
		{
			m_Box_0.SetText("NhÉn");
			m_Box_0.SetPosition(105,44);
			
			m_Box_1.SetText("D©y chuyÒn/hé th©n phï");
			m_Box_1.SetPosition(110,154);
			
			m_Box_2.SetText("Ngäc béi/h­¬ng nang");
			m_Box_2.SetPosition(-8,154);
			m_GuideList.Clear();
			int nLength = TEncodeText(m_szTip[CPA_FUSION], strlen(m_szTip[CPA_FUSION]));
			m_GuideList.AddOneMessage(m_szTip[CPA_FUSION], nLength);
		}
		break;
	case CPA_CRYOLITE:
		{
			m_Box_0.SetText("HuyÒn tinh 1");
			m_Box_0.SetPosition(80,44);
			
			m_Box_1.SetText("HuyÒn tinh 2");
			m_Box_1.SetPosition(146,154);
			
			m_Box_2.SetText("HuyÒn tinh 3");
			m_Box_2.SetPosition(18,154);
			m_GuideList.Clear();
			int nLength = TEncodeText(m_szTip[CPA_CRYOLITE], strlen(m_szTip[CPA_CRYOLITE]));
			m_GuideList.AddOneMessage(m_szTip[CPA_CRYOLITE], nLength);
		}
		break;
	case CPA_PROPMINE:
		{
			m_Box_0.SetText("Kho¸ng th¹ch 1");
			m_Box_0.SetPosition(71,44);
			
			m_Box_1.SetText("Kho¸ng th¹ch 2");
			m_Box_1.SetPosition(137,154);
			
			m_Box_2.SetText("Kho¸ng th¹ch 3");
			m_Box_2.SetPosition(9,154);
			m_GuideList.Clear();
			int nLength = TEncodeText(m_szTip[CPA_PROPMINE], strlen(m_szTip[CPA_PROPMINE]));
			m_GuideList.AddOneMessage(m_szTip[CPA_PROPMINE], nLength);
		}
		break;
	}
}

void KUiEnchase_Compound::Breathe()
{
	if (m_nStatus == STATUS_WAIT)
	{
		m_Effect_0.Hide();
		m_Effect_0.SetFrame(-1);
		m_Effect_1.Hide();
		m_Effect_1.SetFrame(-1);
		m_Effect_2.Hide();
		m_Effect_2.SetFrame(-1);
	}
	else if (m_nStatus == STATUS_BEGIN)
	{
		m_Effect_0.Show();
		m_Effect_0.SetFrame(0);
		m_Effect_1.Show();
		m_Effect_1.SetFrame(0);
		m_Effect_2.Show();
		m_Effect_2.SetFrame(0);
		m_nStatus = STATUS_DOING;
	}
	else if (m_nStatus == STATUS_DOING)
	{
		if (!PlayEffect())
		{
			m_nStatus = STATUS_CHANGE;
			m_Effect_0.Hide();
			m_Effect_1.Hide();
			m_Effect_2.Hide();
		}
	}
	else if (m_nStatus == STATUS_CHANGE)
	{
		m_Effect_0.Hide();
		m_Effect_0.SetFrame(-1);
		m_Effect_1.Hide();
		m_Effect_1.SetFrame(-1);
		m_Effect_2.Hide();
		m_Effect_2.SetFrame(-1);
		m_nStatus = STATUS_FINISH;
	}
	else if (m_nStatus == STATUS_FINISH)
	{
		m_nStatus = STATUS_WAIT;
	}
}

int KUiEnchase_Compound::PlayEffect()
{
	if (((m_Effect_0.GetMaxFrame() == 0) || (m_Effect_0.GetCurrentFrame() < (m_Effect_0.GetMaxFrame() - 1))) && 
		((m_Effect_1.GetMaxFrame() == 0) || (m_Effect_1.GetCurrentFrame() < (m_Effect_1.GetMaxFrame() - 1))) && 
		((m_Effect_2.GetMaxFrame() == 0) || (m_Effect_2.GetCurrentFrame() < (m_Effect_2.GetMaxFrame() - 1)))
		)
	{
		m_Effect_0.NextFrame();
		m_Effect_1.NextFrame();
		m_Effect_2.NextFrame();
		return 1;
	}
	else
	{
		m_Effect_0.SetFrame(0);
		m_Effect_1.SetFrame(0);
		m_Effect_2.SetFrame(0);
		return 0;
	}
}

int KGetFrameImage_Compound::GetMaxFrame()
{
	return m_Image.nNumFrames;
}


int KGetFrameImage_Compound::GetCurrentFrame()
{
	return m_Image.nFrame;
}

#define 	SCHEME_INI_DISTILL		"·ÖÒ³ÌáÈ¡.ini"


KUiEnchase_Distill* KUiEnchase_Distill::ms_pSelf = NULL;

KUiEnchase_Distill::KUiEnchase_Distill()
{
	m_nStatus = STATUS_WAIT;
}

KUiEnchase_Distill::~KUiEnchase_Distill()
{
}

KUiEnchase_Distill* KUiEnchase_Distill::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiEnchase_Distill;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->m_nStatus = STATUS_WAIT;
		ms_pSelf->BringToTop();
		ms_pSelf->UpdateDataObjBox();
		ms_pSelf->UpdateDataMatrixBox();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}

KUiEnchase_Distill* KUiEnchase_Distill::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiEnchase_Distill::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiEnchase_Distill::Show()
{
	UpdateDataObjBox();
	//UpdateDataMatrixBox();
	m_nStatus = STATUS_WAIT;
	m_GuideList.Clear();
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);	
	KWndPage::Show();		
}

void KUiEnchase_Distill::Hide()
{
	KWndPage::Hide();
}

void KUiEnchase_Distill::Initialize()
{
	//UpdateAllData();
	AddChild(&m_EquipPos);
	m_EquipPos.SetText("Trang bÞ mµu xanh");
	AddChild(&m_CryolitePos);
	m_CryolitePos.SetText("HuyÒn tinh");
	AddChild(&m_PropMinePos);
	m_PropMinePos.SetText("Nguyªn kho¸ng");
	AddChild(&m_ConsumePos);
	m_ConsumePos.SetText("Cã thÓ chän nguyªn liÖu");
	AddChild(&m_EquipEffect);
	m_EquipEffect.Hide();
	AddChild(&m_ConsumeEffect);
	m_ConsumeEffect.Hide();
	for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
	{
		m_DistillBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_DistillBox[i]);
		m_DistillBox[i].SetContainerId((int)UOC_COMPOUND);
	}
	AddChild(&m_ItemBox);
	m_ItemBox.SetContainerId((int)UOC_COMPOUND_BOX);
	AddChild(&m_GuideList);
	AddChild(&m_GuideList_Scroll);
	AddChild(&m_DistillBtn);
	AddChild(&m_CancleBtn);
	m_GuideList.SetScrollbar(&m_GuideList_Scroll);
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);	

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	Wnd_AddWindow(this);
}

void KUiEnchase_Distill::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_DISTILL);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		int nPosX = 0, nPosY = 0;
		m_EquipPos.Init(&Ini, "EquipPos");
		Ini.GetInteger2("EquipPos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_EquipPos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_EquipPos.SetBorderTextColor(GetColor(Buff));
		m_EquipPos.SetPosition(nPosX-50, nPosY-5);
		
		m_CryolitePos.Init(&Ini, "CryolitePos");
		Ini.GetInteger2("CryolitePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetBorderTextColor(GetColor(Buff));
		m_CryolitePos.SetPosition(nPosX-30, nPosY-5);
		
		m_PropMinePos.Init(&Ini, "PropMinePos");
		Ini.GetInteger2("PropMinePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_PropMinePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_PropMinePos.SetBorderTextColor(GetColor(Buff));
		m_PropMinePos.SetPosition(nPosX-40, nPosY-5);
		
		m_ConsumePos.Init(&Ini, "ConsumePos");
		Ini.GetInteger2("ConsumePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_ConsumePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_ConsumePos.SetBorderTextColor(GetColor(Buff));
		m_ConsumePos.SetPosition(nPosX-70, nPosY-5);

		for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
		{
			m_DistillBox[i].Init(&Ini, CtrlItemMap[i].pIniSection);
		}
		m_ItemBox.Init(&Ini, "ItemBox");
		m_ItemBox.EnableTracePutPos(true);			
		m_GuideList.Init(&Ini, "GuideList");
		m_GuideList_Scroll.Init(&Ini, "GuideList_Scroll");
		m_DistillBtn.Init(&Ini, "DistillBtn");
		m_CancleBtn.Init(&Ini, "CancleBtn");
		m_EquipEffect.Init(&Ini, "EquipEffect");
		m_ConsumeEffect.Init(&Ini, "ConsumeEffect");
	}
}

int KUiEnchase_Distill::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch(uMsg)
	{
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))
		{
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		{
			if(uParam == (unsigned int)&m_DistillBtn)
			{
				if (m_nStatus != STATUS_WAIT)
					return 0;			

				m_nStatus = STATUS_BEGIN;

				nResult = true;				
			}
			else if (uParam == (unsigned int)&m_CancleBtn)
			{
				if (m_nStatus != STATUS_WAIT)
				{
					m_nStatus = STATUS_WAIT;
					return 0;
				}

				nResult = true;
			}
		}
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_GuideList_Scroll)
			m_GuideList.SetFirstShowLine(nParam);
		break;
	default:
			nResult = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiEnchase_Distill::UpdateDataObjBox()
{
	KUiObjAtRegion	Equips[MAX_COMPOUND_ITEM];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPOUND, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < MAX_COMPOUND_ITEM; i++)
		m_DistillBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItemObjBox(&Equips[i], true);
	}
}

void KUiEnchase_Distill::UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
		{
			if (CtrlItemMap[i].nPosition == pItem->Region.v)
			{
				if (bAdd)
					m_DistillBox[i].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,pItem->Region.Width, pItem->Region.Height);
				else
					m_DistillBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}

	}
}

void KUiEnchase_Distill::UpdateDataMatrixBox()
{
	m_ItemBox.Clear();
	KUiObjAtRegion* pObjs = NULL;
	
	int nCount = g_pCoreShell->GetGameData(GDI_COMPOUND_BOX, 0, 0);
	if (nCount == 0)
		return;
	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_COMPOUND_BOX, (unsigned int)pObjs, nCount);
		for (int i = 0; i < nCount; i++)
			UpdateItemMatrixBox(&pObjs[i], true);
		free(pObjs);
		pObjs = NULL;
	}
}

void KUiEnchase_Distill::UpdateItemMatrixBox(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemBox.AddObject(&Obj, 1);
			else
				m_ItemBox.RemoveObject(&Obj);
		}
	}
	else
		UpdateDataMatrixBox();
}

void KUiEnchase_Distill::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (m_nStatus != STATUS_WAIT)
		return;
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;
	
	if (pPickPos)
	{
		if (pPickPos->TypeItemBox == 1)
		{
			//_ASSERT(pPickPos->pWnd);
			((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
			Pick.Obj.uGenre = Obj.uGenre;
			Pick.Obj.uId = Obj.uId;
			Pick.Region.Width = Obj.DataW;
			Pick.Region.Height = Obj.DataH;
			Pick.Region.h = 0;
			Pick.eContainer = UOC_COMPOUND;
			pWnd = pPickPos->pWnd;
		}
		else if (pPickPos->TypeItemBox == 0)
		{
			_ASSERT(pPickPos->pWnd);
			((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(Obj, pPickPos->h, pPickPos->v);
			Pick.Obj.uGenre = Obj.uGenre;
			Pick.Obj.uId = Obj.uId;
			Pick.Region.Width = Obj.DataW;
			Pick.Region.Height = Obj.DataH;
			Pick.Region.h = Obj.DataX;
			Pick.Region.v = Obj.DataY;
			Pick.eContainer = UOC_COMPOUND_BOX;
		}
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;
	
	if (pDropPos)
	{
		if (pDropPos->TypeItemBox == 1)
		{
			Wnd_GetDragObj(&Obj);
			Drop.Obj.uGenre = Obj.uGenre;
			Drop.Obj.uId = Obj.uId;
			Drop.Region.Width = Obj.DataW;
			Drop.Region.Height = Obj.DataH;
			Drop.Region.h = 0;
			Drop.eContainer = UOC_COMPOUND;
		}
		else if (pDropPos->TypeItemBox == 0)
		{
			Wnd_GetDragObj(&Obj);
			Drop.Obj.uGenre = Obj.uGenre;
			Drop.Obj.uId = Obj.uId;
			Drop.Region.Width = Obj.DataW;
			Drop.Region.Height = Obj.DataH;
			Drop.Region.h = pDropPos->h;
			Drop.Region.v = pDropPos->v;
			Drop.eContainer = UOC_COMPOUND_BOX;
		}
	}
	
	for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
	{
		if (pWnd == (KWndWindow*)&m_DistillBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}
	
	//_ASSERT(i < _ITEM_COUNT);
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiEnchase_Distill::Breathe()
{
	if (m_nStatus == STATUS_WAIT)
	{
		m_EquipEffect.Hide();
		m_EquipEffect.SetFrame(-1);
		m_ConsumeEffect.Hide();
		m_ConsumeEffect.SetFrame(-1);
	}
	else if(m_nStatus == STATUS_BEGIN)
	{
		m_EquipEffect.Show();
		m_EquipEffect.SetFrame(0);
		m_ConsumeEffect.Show();
		m_ConsumeEffect.SetFrame(0);
		m_nStatus = STATUS_DOING;
	}
	else if (m_nStatus == STATUS_DOING)
	{
		if (!PlayEffect())
		{
			m_nStatus = STATUS_CHANGE;
			m_EquipEffect.Hide();
			m_ConsumeEffect.Hide();
		}
	}
	else if (m_nStatus == STATUS_CHANGE)
	{
		m_EquipEffect.Hide();
		m_EquipEffect.SetFrame(-1);
		m_ConsumeEffect.Hide();
		m_ConsumeEffect.SetFrame(-1);
		m_nStatus = STATUS_FINISH;
	}
	else if (m_nStatus == STATUS_FINISH)
	{
		m_nStatus = STATUS_WAIT;
	}
}

int KUiEnchase_Distill::PlayEffect()
{
	if(m_EquipEffect.GetCurrentFrame() >= 26 || m_ConsumeEffect.GetCurrentFrame() >= 26)
	{
		m_EquipEffect.SetFrame(0);
		m_ConsumeEffect.SetFrame(0);
		return 0;
	}
	else
	{
		m_EquipEffect.NextFrame();
		m_ConsumeEffect.NextFrame();
		return 1;
	}
}

int KGetFarmImage_Distill::GetMaxFrame()
{
	return m_Image.nNumFrames;
}


int KGetFarmImage_Distill::GetCurrentFrame()
{
	return m_Image.nFrame;
}

#define 	SCHEME_INI_FORGE		"·ÖÒ³´òÔì.ini"

KUiEnchase_Forge*	KUiEnchase_Forge::ms_pSelf = NULL;

KUiEnchase_Forge::KUiEnchase_Forge()
{
	m_nStatus = STATUS_WAIT;
}

KUiEnchase_Forge::~KUiEnchase_Forge()
{
}

KUiEnchase_Forge* KUiEnchase_Forge::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiEnchase_Forge;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->m_nStatus = STATUS_WAIT;
		ms_pSelf->BringToTop();
		ms_pSelf->UpdateData();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}

KUiEnchase_Forge* KUiEnchase_Forge::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiEnchase_Forge::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiEnchase_Forge::Show()
{
	UpdateData();
	m_GuideList.Clear();
	m_nStatus = STATUS_WAIT;
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);
	KWndPage::Show();		
}

void KUiEnchase_Forge::Hide()
{
	KWndPage::Hide();
}

void KUiEnchase_Forge::Initialize()
{
	AddChild(&m_EquipPos);
	m_EquipPos.SetText("Trang bÞ xanh tr¾ng");
	AddChild(&m_CryolitePos);
	m_CryolitePos.SetText("HuyÒn tinh");
	AddChild(&m_EquipEffect);
	m_EquipEffect.Hide();	
	for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
	{
		m_ForgeBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_ForgeBox[i]);
		m_ForgeBox[i].SetContainerId((int)UOC_COMPOUND);
	}
	AddChild(&m_GuideList);
	AddChild(&m_GuideList_Scroll);
	AddChild(&m_ForgeBtn);
	AddChild(&m_CancleBtn);
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);	

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	Wnd_AddWindow(this);
}

void KUiEnchase_Forge::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_FORGE);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		int nPosX = 0, nPosY = 0;
		m_EquipPos.Init(&Ini, "EquipPos");
		Ini.GetInteger2("EquipPos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_EquipPos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_EquipPos.SetBorderTextColor(GetColor(Buff));
		m_EquipPos.SetPosition(nPosX-60, nPosY-5);
		
		m_CryolitePos.Init(&Ini, "CryolitePos");
		Ini.GetInteger2("CryolitePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetBorderTextColor(GetColor(Buff));
		m_CryolitePos.SetPosition(nPosX-30, nPosY-5);
		for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
		{
			m_ForgeBox[i].Init(&Ini, CtrlItemMap[i].pIniSection);
		}
		m_GuideList.Init(&Ini, "GuideList");
		m_GuideList_Scroll.Init(&Ini, "GuideList_Scroll");
		m_ForgeBtn.Init(&Ini, "ForgeBtn");
		m_CancleBtn.Init(&Ini, "CancleBtn");
		m_EquipEffect.Init(&Ini, "EquipEffect");
	}
}

int KUiEnchase_Forge::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch(uMsg)
	{
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))
		{
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		{
			if(uParam == (unsigned int)&m_ForgeBtn)
			{
				if (m_nStatus != STATUS_WAIT)
					return 0;			

				m_nStatus = STATUS_BEGIN;

				nResult = true;
			}
			else if (uParam == (unsigned int)&m_CancleBtn)
			{
				if (m_nStatus != STATUS_WAIT)
				{
					m_nStatus = STATUS_WAIT;
					return 0;
				}
				nResult = true;
			}
		}
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_GuideList_Scroll)
			m_GuideList.SetFirstShowLine(nParam);
		break;
	default:
			nResult = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiEnchase_Forge::UpdateData()
{
	KUiObjAtRegion	Equips[MAX_COMPOUND_ITEM];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPOUND, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < MAX_COMPOUND_ITEM; i++)
		m_ForgeBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItemObjBox(&Equips[i], true);
	}
}

void KUiEnchase_Forge::UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
		{
			if (CtrlItemMap[i].nPosition == pItem->Region.v)
			{
				if (bAdd)
					m_ForgeBox[i].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,
					pItem->Region.Width, pItem->Region.Height);
				else
					m_ForgeBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}
	}
}

void KUiEnchase_Forge::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (m_nStatus != STATUS_WAIT)
		return;
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;
	
	if (pPickPos)
	{
		//_ASSERT(pPickPos->pWnd);
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = 0;
		Pick.eContainer = UOC_COMPOUND;
		pWnd = pPickPos->pWnd;
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;
	
	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = 0;
		Drop.eContainer = UOC_COMPOUND;
	}
	
	for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
	{
		if (pWnd == (KWndWindow*)&m_ForgeBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}
	
	//_ASSERT(i < _ITEM_COUNT);
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiEnchase_Forge::Breathe()
{	
	if (m_nStatus == STATUS_WAIT)
	{
		m_EquipEffect.Hide();
		m_EquipEffect.SetFrame(-1);
	}
	else if(m_nStatus == STATUS_BEGIN)
	{
		m_EquipEffect.Show();
		m_EquipEffect.SetFrame(0);
		m_nStatus = STATUS_DOING;
	}
	else if (m_nStatus == STATUS_DOING)
	{
		if (!PlayEffect())
		{
			m_nStatus = STATUS_CHANGE;
			m_EquipEffect.Hide();
		}
	}
	else if (m_nStatus == STATUS_CHANGE)
	{
		m_EquipEffect.Hide();
		m_EquipEffect.SetFrame(-1);
		m_nStatus = STATUS_FINISH;
	}
	else if (m_nStatus == STATUS_FINISH)
	{
		m_nStatus = STATUS_WAIT;
	}		
}

int KUiEnchase_Forge::PlayEffect()
{
	if(m_EquipEffect.GetCurrentFrame() >= 26)
	{
		m_EquipEffect.SetFrame(0);
		return 0;
	}
	else
	{
		m_EquipEffect.NextFrame();
		return 1;
	}
}

int KGetFarmImage_Forge::GetMaxFrame()
{
	return m_Image.nNumFrames;
}

int KGetFarmImage_Forge::GetCurrentFrame()
{
	return m_Image.nFrame;
}

#define 	SCHEME_INI_ENCHASE		"·ÖÒ³ÏâÇ¶.ini"

KUiEnchase_Enchase*	KUiEnchase_Enchase::ms_pSelf = NULL;

KUiEnchase_Enchase::KUiEnchase_Enchase()
{
	m_nStatus = STATUS_WAIT;
}

KUiEnchase_Enchase::~KUiEnchase_Enchase()
{
}

KUiEnchase_Enchase* KUiEnchase_Enchase::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiEnchase_Enchase;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->m_nStatus = STATUS_WAIT;
		ms_pSelf->BringToTop();
		ms_pSelf->UpdateData();
		ms_pSelf->Show();
	}
	return ms_pSelf;
}

KUiEnchase_Enchase* KUiEnchase_Enchase::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

void KUiEnchase_Enchase::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}

void KUiEnchase_Enchase::Show()
{
	UpdateDataObjBox();
	//UpdateDataMatrixBox();
	m_nStatus = STATUS_WAIT;
	m_GuideList.Clear();
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);	
	KWndPage::Show();		
}

void KUiEnchase_Enchase::Hide()
{
	KWndPage::Hide();
}

void KUiEnchase_Enchase::Initialize()
{
	AddChild(&m_PurpleEquipPos);
	m_PurpleEquipPos.SetText("Trang bÞ HuyÒn Tinh");
	AddChild(&m_CryolitePos);
	m_CryolitePos.SetText("HuyÒn tinh");
	AddChild(&m_PropMinePos);
	m_PropMinePos.SetText("Nguyªn kho¸ng");
	AddChild(&m_ConsumePos);
	m_ConsumePos.SetText("Cã thÓ chän nguyªn liÖu");
	AddChild(&m_EquipEffect);
	m_EquipEffect.Hide();
	AddChild(&m_ConsumeEffect);
	m_ConsumeEffect.Hide();
	
	for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
	{
		m_EnchaseBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_EnchaseBox[i]);
		m_EnchaseBox[i].SetContainerId((int)UOC_COMPOUND);
	}
	AddChild(&m_ItemBox);
	m_ItemBox.SetContainerId((int)UOC_COMPOUND_BOX);
	AddChild(&m_GuideList);
	AddChild(&m_GuideList_Scroll);
	AddChild(&m_EnchaseBtn);
	AddChild(&m_CancleBtn);
	m_GuideList.SetScrollbar(&m_GuideList_Scroll);
	int nLength = TEncodeText(m_szTip, strlen(m_szTip));
	m_GuideList.AddOneMessage(m_szTip, nLength);	

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	Wnd_AddWindow(this);
}

void KUiEnchase_Enchase::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ENCHASE);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		int nPosX = 0, nPosY = 0;
		m_PurpleEquipPos.Init(&Ini, "PurpleEquipPos");
		Ini.GetInteger2("PurpleEquipPos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_PurpleEquipPos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_PurpleEquipPos.SetBorderTextColor(GetColor(Buff));
		m_PurpleEquipPos.SetPosition(nPosX-60, nPosY-5);
		
		m_CryolitePos.Init(&Ini, "CryolitePos");
		Ini.GetInteger2("CryolitePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_CryolitePos.SetBorderTextColor(GetColor(Buff));
		m_CryolitePos.SetPosition(nPosX-30, nPosY-5);
		
		m_PropMinePos.Init(&Ini, "PropMinePos");
		Ini.GetInteger2("PropMinePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_PropMinePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_PropMinePos.SetBorderTextColor(GetColor(Buff));
		m_PropMinePos.SetPosition(nPosX-40, nPosY-5);
		
		m_ConsumePos.Init(&Ini, "ConsumePos");
		Ini.GetInteger2("ConsumePos", "Pos", &nPosX, &nPosY);
		Ini.GetString("TextColor", "Font", "0,0,0", Buff, sizeof(Buff));
		m_ConsumePos.SetTextColor(GetColor(Buff));
		Ini.GetString("TextColor", "Border", "0,0,0", Buff, sizeof(Buff));
		m_ConsumePos.SetBorderTextColor(GetColor(Buff));
		m_ConsumePos.SetPosition(nPosX-70, nPosY-5);
		for (int i = 0; i < MAX_COMPOUND_ITEM; i ++)
		{
			m_EnchaseBox[i].Init(&Ini, CtrlItemMap[i].pIniSection);
		}
		m_ItemBox.Init(&Ini, "ItemBox");
		m_ItemBox.EnableTracePutPos(true);		
		m_GuideList.Init(&Ini, "GuideList");
		m_GuideList_Scroll.Init(&Ini, "GuideList_Scroll");
		m_EnchaseBtn.Init(&Ini, "EnchaseBtn");
		m_CancleBtn.Init(&Ini, "CancelBtn");
		m_EquipEffect.Init(&Ini, "EquipEffect");
		m_ConsumeEffect.Init(&Ini, "ConsumeEffect");
	}
}

int KUiEnchase_Enchase::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch(uMsg)
	{
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))
		{
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		}
		break;
	case WND_N_BUTTON_CLICK:
		{
			if(uParam == (unsigned int)&m_EnchaseBtn)
			{
				if (m_nStatus != STATUS_WAIT)
					return 0;			

				m_nStatus = STATUS_BEGIN;

				nResult = true;
			}
			else if (uParam == (unsigned int)&m_CancleBtn)
			{
				if (m_nStatus != STATUS_WAIT)
				{
					m_nStatus = STATUS_WAIT;
					return 0;
				}
				nResult = true;
			}
		}
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_GuideList_Scroll)
			m_GuideList.SetFirstShowLine(nParam);
		break;
	default:
			nResult = KWndPage::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiEnchase_Enchase::UpdateDataObjBox()
{
	KUiObjAtRegion	Equips[MAX_COMPOUND_ITEM];
	int nCount = g_pCoreShell->GetGameData(GDI_COMPOUND, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < MAX_COMPOUND_ITEM; i++)
		m_EnchaseBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateItemObjBox(&Equips[i], true);
	}
}

void KUiEnchase_Enchase::UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
		{
			if (CtrlItemMap[i].nPosition == pItem->Region.v)
			{
				if (bAdd)
					m_EnchaseBox[i].HoldObject(pItem->Obj.uGenre, pItem->Obj.uId,pItem->Region.Width, pItem->Region.Height);
				else
					m_EnchaseBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}
	}
}

void KUiEnchase_Enchase::UpdateDataMatrixBox()
{
	m_ItemBox.Clear();
	KUiObjAtRegion* pObjs = NULL;
	
	int nCount = g_pCoreShell->GetGameData(GDI_COMPOUND_BOX, 0, 0);
	if (nCount == 0)
		return;
	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_COMPOUND_BOX, (unsigned int)pObjs, nCount);
		for (int i = 0; i < nCount; i++)
			UpdateItemMatrixBox(&pObjs[i], true);
		free(pObjs);
		pObjs = NULL;
	}
}

void KUiEnchase_Enchase::UpdateItemMatrixBox(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemBox.AddObject(&Obj, 1);
			else
				m_ItemBox.RemoveObject(&Obj);
		}
	}
	else
		UpdateDataMatrixBox();
}

void KUiEnchase_Enchase::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (m_nStatus != STATUS_WAIT)
		return;
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;
	
	if (pPickPos)
	{
		if (pPickPos->TypeItemBox == 1)
		{
			//_ASSERT(pPickPos->pWnd);
			((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
			Pick.Obj.uGenre = Obj.uGenre;
			Pick.Obj.uId = Obj.uId;
			Pick.Region.Width = Obj.DataW;
			Pick.Region.Height = Obj.DataH;
			Pick.Region.h = 0;
			Pick.eContainer = UOC_COMPOUND;
			pWnd = pPickPos->pWnd;
		}
		else if (pPickPos->TypeItemBox == 0)
		{
			_ASSERT(pPickPos->pWnd);
			((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(Obj, pPickPos->h, pPickPos->v);
			Pick.Obj.uGenre = Obj.uGenre;
			Pick.Obj.uId = Obj.uId;
			Pick.Region.Width = Obj.DataW;
			Pick.Region.Height = Obj.DataH;
			Pick.Region.h = Obj.DataX;
			Pick.Region.v = Obj.DataY;
			Pick.eContainer = UOC_COMPOUND_BOX;
		}
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;
	
	if (pDropPos)
	{
		if (pDropPos->TypeItemBox == 1)
		{
			Wnd_GetDragObj(&Obj);
			Drop.Obj.uGenre = Obj.uGenre;
			Drop.Obj.uId = Obj.uId;
			Drop.Region.Width = Obj.DataW;
			Drop.Region.Height = Obj.DataH;
			Drop.Region.h = 0;
			Drop.eContainer = UOC_COMPOUND;
		}
		else if (pDropPos->TypeItemBox == 0)
		{
			Wnd_GetDragObj(&Obj);
			Drop.Obj.uGenre = Obj.uGenre;
			Drop.Obj.uId = Obj.uId;
			Drop.Region.Width = Obj.DataW;
			Drop.Region.Height = Obj.DataH;
			Drop.Region.h = pDropPos->h;
			Drop.Region.v = pDropPos->v;
			Drop.eContainer = UOC_COMPOUND_BOX;
		}
	}
	
	for (int i = 0; i < MAX_COMPOUND_ITEM; i++)
	{
		if (pWnd == (KWndWindow*)&m_EnchaseBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}
	
	//_ASSERT(i < _ITEM_COUNT);
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiEnchase_Enchase::Breathe()
{
	if (m_nStatus == STATUS_WAIT)
	{
		m_EquipEffect.Hide();
		m_EquipEffect.SetFrame(-1);
		m_ConsumeEffect.Hide();
		m_ConsumeEffect.SetFrame(-1);
	}
	else if(m_nStatus == STATUS_BEGIN)
	{
		m_EquipEffect.Show();
		m_EquipEffect.SetFrame(0);
		m_ConsumeEffect.Show();
		m_ConsumeEffect.SetFrame(0);
		m_nStatus = STATUS_DOING;
	}
	else if (m_nStatus == STATUS_DOING)
	{
		if (!PlayEffect())
		{
			m_nStatus = STATUS_CHANGE;
			m_EquipEffect.Hide();
			m_ConsumeEffect.Hide();
		}
	}
	else if (m_nStatus == STATUS_CHANGE)
	{
		m_EquipEffect.Hide();
		m_EquipEffect.SetFrame(-1);
		m_ConsumeEffect.Hide();
		m_ConsumeEffect.SetFrame(-1);
		m_nStatus = STATUS_FINISH;
	}
	else if (m_nStatus == STATUS_FINISH)
	{
		m_nStatus = STATUS_WAIT;
	}
}

int KUiEnchase_Enchase::PlayEffect()
{
	if(m_EquipEffect.GetCurrentFrame() >= 26 || m_ConsumeEffect.GetCurrentFrame() >= 26)
	{
		m_EquipEffect.SetFrame(0);
		m_ConsumeEffect.SetFrame(0);
		return 0;
	}
	else
	{
		m_EquipEffect.NextFrame();
		m_ConsumeEffect.NextFrame();
		return 1;
	}
}

int KGetFarmImage_Enchase::GetMaxFrame()
{
	return m_Image.nNumFrames;
}


int KGetFarmImage_Enchase::GetCurrentFrame()
{
	return m_Image.nFrame;
}
