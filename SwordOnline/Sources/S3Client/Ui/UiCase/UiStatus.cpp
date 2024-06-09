/*****************************************************************************************
//	½çÃæ--×´Ì¬½çÃæ
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "KDebug.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../elem/popupmenu.h"
#include "UiShop.h"
#include "UiSuperShop.h"
#include "UiStatus.h"
#include "UiGetString.h"
#include "UiItem.h"
#include "UiGive.h"
#include "UiExpandItem.h"
#include "UiStoreBox.h"
#include "UiSysMsgCentre.h"
#include "../ShortcutKey.h"
#include "../../../Core/Src/CoreShell.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "UiTradeConfirmWnd.h"
#include "UiChooseFace.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"

extern iRepresentShell*	g_pRepresentShell;

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"UiStatus.ini"
#define IMAGE_PLAYER	"\\Settings\\AvatarPlayer.ini"

enum WAIT_OTHER_WND_OPER_PARAM
{
	UIITEM_WAIT_GETSTR,
	UIITEM_WAIT_GETDEX,
	UIITEM_WAIT_GETVIT,
	UIITEM_WAIT_GETENE,
};

KUiStatus* KUiStatus::m_pSelf = NULL;
// -------------------------------------------------------------------------
// ---> ½¨Á¢¿Ø¼þÓëUIEP_*ÒÔ¼°¿É½ÓÄÉÎïÆ·µÄÀàÐÍµÄ¶ÔÓ¦¹ØÏµ
static struct UE_CTRL_MAP
{
	int				nPosition;
	const char*		pIniSection;
}CtrlItemMap[_ITEM_COUNT] =
{
	{ UIEP_HEAD,		"Cap"		},
	{ UIEP_HAND,		"Weapon"	},
	{ UIEP_NECK,		"Necklace"	},
	{ UIEP_FINESSE,		"Bangle"	},
	{ UIEP_BODY,		"Cloth"		},
	{ UIEP_WAIST,		"Sash"		},
	{ UIEP_FINGER1,		"Ring1"		},
	{ UIEP_FINGER2,		"Ring2"		},
	{ UIEP_WAIST_DECOR,	"Pendant"	},
	{ UIEP_FOOT,		"Shoes"		},
	{ UIEP_HORSE,		"Horse"		},
	{ UIEP_MASK,		"Mask"		},
	{ UIEP_MANTLE,		"Mantle"	},	//×°±¸-Âí
	{ UIEP_SIGNET,		"Signet"	},	//×°±¸-Âí
	{ UIEP_SHIPIN,		"Shipin"	},	//×°±¸-Âí
	{ UIEP_HOODS,		"Hoods"		},	//×°±¸-Âí
	{ UIEP_CLOAK,		"Cloak"		},	//×°±¸-Âí
};



//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÈç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
//--------------------------------------------------------------------------
KUiStatus* KUiStatus::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiStatus* KUiStatus::OpenWindow(bool bShow)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStatus;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf && bShow)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiStatus::CloseWindow(bool bDestroy)
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

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
void KUiStatus::Initialize()
{
	AddChild(&m_Name);
	AddChild(&m_Title);

	AddChild(&m_Life);
	AddChild(&m_Mana);
	AddChild(&m_Stamina);
	AddChild(&m_Experience);

	AddChild(&m_RemainPoint);
	AddChild(&m_Strength);
	AddChild(&m_Dexterity);
	AddChild(&m_Vitality);
	AddChild(&m_Energy);

	AddChild(&m_AddStrength);
	AddChild(&m_AddDexterity);
	AddChild(&m_AddVitality);
	AddChild(&m_AddEnergy);

	AddChild(&m_LeftDamage);	
	AddChild(&m_RightDamage);
	AddChild(&m_Attack);
	AddChild(&m_Defence);
	AddChild(&m_MoveSpeed);
	AddChild(&m_AttackSpeed);

	AddChild(&m_PhyDef);
	AddChild(&m_CoolDef);
	AddChild(&m_LightDef);
	AddChild(&m_FireDef);
	AddChild(&m_PoisonDef);
	AddChild(&m_PhyDefPlus);
	AddChild(&m_CoolDefPlus);
	AddChild(&m_LightDefPlus);
	AddChild(&m_FireDefPlus);
	AddChild(&m_PoisonDefPlus);
	AddChild(&m_Level);
	AddChild(&m_StatusDesc);
	AddChild(&m_PKValue);
	AddChild(&m_Repute);
	AddChild(&m_FuYuan);
	AddChild(&m_TransLife);
	AddChild(&m_WorldRank);

	AddChild(&m_UnlockBtn);
	AddChild(&m_BtnBind);
	AddChild(&m_BtnUnBind);	

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		if(i==UIEP_MASK)
			AddChild(&m_EquipExpandImg);
		m_EquipBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_EquipBox[i]);
		m_EquipBox[i].SetContainerId((int)UOC_EQUIPTMENT);
	}
	AddChild(&m_EquipExpandBtn);
	AddChild(&m_MaskFeature);

	AddChild(&m_OpenItemPad);
	AddChild(&m_Close);
	
	AddChild(&m_chooseavatar);
	AddChild(&m_Avatar);	

	SwitchExpand(FALSE);
	Wnd_AddWindow(this);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

void KUiStatus::SwitchExpand(BOOL bShow)
{
	m_EquipExpandBtn.CheckButton(bShow);
		bShow?m_MaskFeature.Show():m_MaskFeature.Hide();
		bShow?m_EquipExpandImg.Show():m_EquipExpandImg.Hide();
			for (int i = UIEP_MANTLE; i < _ITEM_COUNT; i ++)
			bShow?m_EquipBox[i].Show():m_EquipBox[i].Hide();
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiStatus::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
			m_pSelf->LoadScheme(&Ini);	
	}
}

//ÔØÈë½çÃæ·½°¸
void KUiStatus::LoadScheme(class KIniFile* pIni)
{
	if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0))
		Init(pIni, "Male");
	else
		Init(pIni, "Female");
	
	m_Avt	  .Init(pIni, "Avt");
	m_Name    .Init(pIni, "Name");
	m_Title   .Init(pIni, "Title");

	m_Life		.Init(pIni, "Life");
	m_Mana		.Init(pIni, "Mana");
	m_Stamina	.Init(pIni, "Stamina");
	m_Experience.Init(pIni, "Exp");

	m_RemainPoint.Init(pIni, "RemainPoint");
	m_Strength   .Init(pIni, "Strength");
	m_Dexterity  .Init(pIni, "Dexterity");
	m_Vitality   .Init(pIni, "Vitality");
	m_Energy     .Init(pIni, "Energy");
		
	m_AddStrength .Init(pIni, "AddStrength");
	m_AddDexterity.Init(pIni, "AddDexterity");
	m_AddVitality .Init(pIni, "AddVitality");
	m_AddEnergy   .Init(pIni, "AddEnergy");
		
	m_LeftDamage .Init(pIni, "LeftDamage");
	m_RightDamage.Init(pIni, "RightDamage");
	m_Attack     .Init(pIni, "Attack");
	m_Defence    .Init(pIni, "Defense");
	m_MoveSpeed  .Init(pIni, "MoveSpeed");
	m_AttackSpeed.Init(pIni, "AttackSpeed");

	m_PhyDef	 .Init(pIni, "ResistPhy");
	m_CoolDef    .Init(pIni, "ResistCold");
	m_LightDef   .Init(pIni, "ResistLighting");
	m_FireDef    .Init(pIni, "ResistFire");
	m_PoisonDef  .Init(pIni, "ResistPoison");
	m_PhyDefPlus .Init(pIni, "ResistPhyPlus");
	m_CoolDefPlus.Init(pIni, "ResistColdPlus");
	m_LightDefPlus.Init(pIni, "ResistLightingPlus");
	m_FireDefPlus .Init(pIni, "ResistFirePlus");
	m_PoisonDefPlus.Init(pIni, "ResistPoisonPlus");

	m_Level		 .Init(pIni, "Level");
	m_StatusDesc .Init(pIni, "Status");
	m_PKValue	 .Init(pIni, "PKValue");
	m_Repute    .Init(pIni, "Prestige");
	m_FuYuan   .Init(pIni, "FuYuan");
	m_TransLife    .Init(pIni, "TransLife");
	m_WorldRank  .Init(pIni, "WorldRank");
	
	m_UnlockBtn.Init(pIni, "BtnLock");
	m_BtnBind.Init(pIni, "BtnBind");
	m_BtnUnBind.Init(pIni, "BtnUnBind");

	m_chooseavatar.Init(pIni, "ClickHere");
	m_Avatar  .Init(pIni, "ClickHere");
	

	m_Close	 .Init(pIni, "Close");

	m_OpenItemPad.Init(pIni, "Item");

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].Init(pIni, CtrlItemMap[i].pIniSection);
	}
	m_EquipExpandBtn.Init(pIni, "EquipExpandBtn");
	m_EquipExpandImg.Init(pIni, "EquipExpandImg");
	m_MaskFeature.Init(pIni, "MaskFeature");
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿Úº¯Êý
//--------------------------------------------------------------------------
int KUiStatus::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close && !g_UiBase.GetStatus())
			CloseWindow(false);
		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenItemPad)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_ITEMS);
		
		else if (uParam == (unsigned int)(KWndWindow*)&m_Avatar)
		{
			if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0))
			{
				KUiChooseFace::OpenWindow(2);
			}
			else
			{	
				KUiChooseFace::OpenWindow(1);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnBind && g_pCoreShell->GetTradeState() == 0)
		{
			if (g_UiBase.GetStatus() == UIS_S_IDLE || g_UiBase.GetStatus() == UIS_S_UNLOCK_ITEM)
				g_UiBase.SetStatus(UIS_S_LOCK_ITEM);
			else if (g_UiBase.GetStatus() == UIS_S_LOCK_ITEM)
				g_UiBase.SetStatus(UIS_S_IDLE);
				
			if (KUiItem::GetIfVisible() == NULL)
				KUiItem::OpenWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnUnBind && g_pCoreShell->GetTradeState() == 0)
		{
			if (g_UiBase.GetStatus() == UIS_S_IDLE || g_UiBase.GetStatus() == UIS_S_LOCK_ITEM)
				g_UiBase.SetStatus(UIS_S_UNLOCK_ITEM);
			else if (g_UiBase.GetStatus() == UIS_S_UNLOCK_ITEM)
				g_UiBase.SetStatus(UIS_S_IDLE);
				
			if (KUiItem::GetIfVisible() == NULL)
				KUiItem::OpenWindow();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_EquipExpandBtn)
			SwitchExpand(m_EquipExpandBtn.IsButtonChecked());
		else if (uParam == (unsigned int)(KWndWindow*)&m_MaskFeature)
			g_pCoreShell->OperationRequest(GOI_MASKFEATURE, 0, 0);
		else if (uParam == (unsigned int)(KWndWindow*)&m_UnlockBtn)
		{
			if (g_pCoreShell->GetLockState())
				KUiGetString::OpenWindow(GSA_PW, "NhËp mËt khÈu", "", this, 0, 0, 1,16);
			else
				g_pCoreShell->OperationRequest(GOI_LOCKSTATE, 0, 0);
		}
		else if (m_nRemainPoint > 0)
		{
			if (uParam == (unsigned int)(KWndWindow*)&m_AddStrength)
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
					KUiGetString::OpenWindow(GSA_NORMAL, "NhËp Sè ®iÓm", "", this, UIITEM_WAIT_GETSTR, m_nRemainPoint);
				else
					UseRemainPoint(0, 1);
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_AddDexterity)
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
					KUiGetString::OpenWindow(GSA_NORMAL, "NhËp Sè ®iÓm", "", this, UIITEM_WAIT_GETDEX, m_nRemainPoint);
				else
					UseRemainPoint(1, 1);
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_AddVitality)
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
					KUiGetString::OpenWindow(GSA_NORMAL, "NhËp Sè ®iÓm", "", this, UIITEM_WAIT_GETVIT, m_nRemainPoint);
				else
					UseRemainPoint(2, 1);
			}
			else if (uParam == (unsigned int)(KWndWindow*)&m_AddEnergy)
			{
				if (GetKeyState(VK_CONTROL) & 0x8000)
					KUiGetString::OpenWindow(GSA_NORMAL, "NhËp Sè ®iÓm", "", this, UIITEM_WAIT_GETENE, m_nRemainPoint);
				else
					UseRemainPoint(3, 1);
			}
		}
		break;
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM) || g_UiBase.GetStatus() == UIS_S_TRADE_REPAIR || 
			g_UiBase.GetStatus() == UIS_S_LOCK_ITEM || g_UiBase.GetStatus() == UIS_S_UNLOCK_ITEM)
			OnEquiptChanged((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;	
	case WND_M_OTHER_WORK_RESULT:
		if(nParam)
		{
			if (uParam == UIITEM_WAIT_GETSTR)
				UseRemainPoint(0, nParam);
			else if (uParam == UIITEM_WAIT_GETDEX)
				UseRemainPoint(1, nParam);
			else if (uParam == UIITEM_WAIT_GETVIT)
				UseRemainPoint(2, nParam);
			else if (uParam == UIITEM_WAIT_GETENE)
				UseRemainPoint(3, nParam);
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÉý¼¶Ä³ÏîÊôÐÔ
//--------------------------------------------------------------------------
void KUiStatus::UseRemainPoint(int ntype, int nPoint)
{
	if (g_pCoreShell)
	g_pCoreShell->OperationRequest(GOI_TONE_UP_ATTRIBUTE, ntype, nPoint);
	m_nRemainPoint -= nPoint;
	m_AddStrength.Enable(m_nRemainPoint>0);
	m_AddDexterity.Enable(m_nRemainPoint>0);
	m_AddVitality.Enable(m_nRemainPoint>0);
	m_AddEnergy.Enable(m_nRemainPoint>0);
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂ»ù±¾Êý¾Ý£¨ÈËÃûµÈ²»Ò×±äÊý¾Ý£©
//--------------------------------------------------------------------------
void KUiStatus::UpdateBaseData()
{
	KUiPlayerBaseInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
	m_Name  .SetText(Info.Name);
	m_WorldRank .Set5IntText(Info.nRankInWorld);
}

void KUiStatus::Breathe()
{
	nNumIcon = g_pCoreShell->GetGameData(GDI_IS_CHECK_IMAGE, 0, 0);
	
	if (nNumIcon == 0)
	{
		//
	}
	if (nNumIcon == 1)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu1.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 2)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu2.spr");
		m_chooseavatar.SetText("");
	}	
	if (nNumIcon == 3)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu3.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 4)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu4.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 5)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu5.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 6)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu6.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 7)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu7.spr");
		m_chooseavatar.SetText("");
		
	}
	if (nNumIcon == 8)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu8.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 9)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu9.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 10)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu10.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 11)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu11.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 12)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu12.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 13)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam1.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 14)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam2.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 15)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam3.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 16)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam4.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 17)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam5.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 18)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam6.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 19)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam7.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 20)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam8.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 21)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam9.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 22)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam10.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 23)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam11.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 24)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam12.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 25)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam13.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 26)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam14.spr");
		m_chooseavatar.SetText("");
	}
	if (nNumIcon == 27)
	{
		m_Avatar.SetPosition(7,135);
		m_Avatar.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam15.spr");
		m_chooseavatar.SetText("");
	}
}
void KUiStatus::UpdateRuntimeInfo(KUiPlayerRuntimeInfo* pInfo)
{
	if (pInfo)
	{
		m_Life		.Set2IntText(pInfo->nLife, pInfo->nLifeFull, '/');
		m_Mana		.Set2IntText(pInfo->nMana, pInfo->nManaFull, '/');
		m_Stamina	.Set2IntText(pInfo->nStamina, pInfo->nStaminaFull, '/');
		m_Experience.Set2IntText(pInfo->nExperience, pInfo->nExperienceFull, '/');
	}
	m_UnlockBtn.CheckButton(g_pCoreShell->GetLockState());
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂÊý¾Ý
//--------------------------------------------------------------------------
void KUiStatus::UpdateData()
{
	UpdateAllEquips();
	UpdateBaseData();
	Breathe();
}

void KUiStatus::UpdateAllEquips()
{
	KUiObjAtRegion	Equips[_ITEM_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_EQUIPMENT, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < _ITEM_COUNT; i++)
		m_EquipBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateEquip(&Equips[i], true);
	}
}

void KUiStatus::UpdateRuntimeAttribute(KUiPlayerAttribute* pInfo)
{
	if (pInfo && g_pCoreShell)	
	{
		m_nRemainPoint = pInfo->nBARemainPoint;
		m_AddStrength.Enable(m_nRemainPoint);
		m_AddDexterity.Enable(m_nRemainPoint);
		m_AddVitality.Enable(m_nRemainPoint);
		m_AddEnergy.Enable(m_nRemainPoint);
		m_RemainPoint.SetIntText(pInfo->nBARemainPoint);
		m_Strength   .SetIntText(pInfo->nStrength);
		m_Dexterity  .SetIntText(pInfo->nDexterity);
		m_Vitality   .SetIntText(pInfo->nVitality);
		m_Energy     .SetIntText(pInfo->nEnergy);

		m_LeftDamage.Set2IntText2(pInfo->nKillMIN, pInfo->nKillMAX, '-', '/');
		m_RightDamage.Set2IntText2(pInfo->nRightKillMin, pInfo->nRightKillMax, '-', '/');
		m_Attack.Set2IntText2(pInfo->nLeftAttack, pInfo->nRightAttack, '-', '/');
		m_Defence.SetIntText(pInfo->nDefence);
		m_MoveSpeed.SetIntText(pInfo->nMoveSpeed);
		m_AttackSpeed.Set6IntText(pInfo->nAttackSpeed,pInfo->nCastSpeed);


		m_PhyDef.Set6IntText(pInfo->nPhyDef, '%');
		m_CoolDef.Set6IntText(pInfo->nCoolDef, '%');
		m_LightDef.Set6IntText(pInfo->nLightDef, '%');
		m_FireDef.Set6IntText(pInfo->nFireDef, '%');
		m_PoisonDef.Set6IntText(pInfo->nPoisonDef, '%');

		if (pInfo->nPhyDefPlus)
			m_PhyDefPlus.SetResistPlus(pInfo->nPhyDefPlus, '+');
		else
			m_PhyDefPlus.Clear();
		if (pInfo->nCoolDefPlus)
			m_CoolDefPlus.SetResistPlus(pInfo->nCoolDefPlus, '+');
		else
			m_CoolDefPlus.Clear();
		if (pInfo->nLightDefPlus)
			m_LightDefPlus.SetResistPlus(pInfo->nLightDefPlus, '+');
		else
			m_LightDefPlus.Clear();
		if (pInfo->nFireDefPlus)
			m_FireDefPlus.SetResistPlus(pInfo->nFireDefPlus, '+');
		else
			m_FireDefPlus.Clear();
		if (pInfo->nPoisonDefPlus)
			m_PoisonDefPlus.SetResistPlus(pInfo->nPoisonDefPlus, '+');
		else
			m_PoisonDefPlus.Clear();
		m_Level.SetIntText(pInfo->nLevel);			//µÈ¼¶
		m_StatusDesc.SetText(pInfo->StatusDesc);

		m_PKValue.SetIntText(pInfo->nPKValue);
		m_Repute  .SetIntText(pInfo->nRepute);
		m_FuYuan .SetIntText(pInfo->nFuYuan);
		m_TransLife .Set4IntText(pInfo->nTranslife);
		m_Title.SetText(pInfo->Title);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦½çÃæ²Ù×÷ÒýÆð×°±¸µÄ¸Ä±ä
//--------------------------------------------------------------------------
void KUiStatus::OnEquiptChanged(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;

	UISYS_STATUS	eStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		//_ASSERT(pPickPos->pWnd);
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = 0;
		Pick.eContainer = UOC_EQUIPTMENT;
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
		Drop.eContainer = UOC_EQUIPTMENT;
	}

	for (int i = 0; i < _ITEM_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_EquipBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}
	if (eStatus == UIS_S_TRADE_REPAIR)
	{
		KUiItemBuySelInfo	Price = { 0 };
		if(g_pCoreShell->IsDamage(Obj.uId))
		{
			if (g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,
				(unsigned int)(&Pick), (int)(&Price)))
			{
				KUiTradeConfirm::OpenWindow(&Pick, &Price, TCA_REPAIR);
			}
		}		
	}
	else if (eStatus == UIS_S_LOCK_ITEM)
		g_pCoreShell->OperationRequest(GOI_LOCKITEM, (unsigned int)(&Pick), 1);
	else if (eStatus == UIS_S_UNLOCK_ITEM)
		g_pCoreShell->OperationRequest(GOI_LOCKITEM, (unsigned int)(&Pick), 0);
	else
	{
		//_ASSERT(i < _ITEM_COUNT);
		g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
	}

}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º×°±¸±ä»¯¸üÐÂ
//--------------------------------------------------------------------------
void KUiStatus::UpdateEquip(KUiObjAtRegion* pEquip, int bAdd)
{
	if (pEquip)
	{
		for (int i = 0; i < _ITEM_COUNT; i++)
		{
			if (CtrlItemMap[i].nPosition == pEquip->Region.v)
			{
				//UiSoundPlay(UI_SI_PICKPUT_ITEM);
				if (bAdd)
					m_EquipBox[i].HoldObject(pEquip->Obj.uGenre, pEquip->Obj.uId,
						pEquip->Region.Width, pEquip->Region.Height);
				else
					m_EquipBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}
	}
}
