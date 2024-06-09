/*****************************************************************************************
//	½çÃæ--×´Ì¬½çÃæ
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiParadeItem.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../UiBase.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../Engine/Src/KDebug.h"
extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"UiParadeItem.ini"

KUiParadeItem* KUiParadeItem::m_pSelf = NULL;

// -------------------------------------------------------------------------
// ---> ½¨Á¢¿Ø¼þÓëUIEP_*ÒÔ¼°¿É½ÓÄÉÎïÆ·µÄÀàÐÍµÄ¶ÔÓ¦¹ØÏµ
static struct UE_CTRL_MAP
{
	int				nPosition;
	const char*		pIniSection;
}CtrlItemMap[_ITEM_COUNT] =
{
	{ UIEP_HEAD,		"Cap"		},	//×°±¸-Ã±×Ó
	{ UIEP_HAND,		"Weapon"	},	//×°±¸-ÎäÆ÷
	{ UIEP_NECK,		"Necklace"	},	//×°±¸-ÏîÁ´
	{ UIEP_FINESSE,		"Bangle"	},	//×°±¸-ÊÖïí
	{ UIEP_BODY,		"Cloth"		},	//×°±¸-ÒÂ·þ
	{ UIEP_WAIST,		"Sash"		},	//×°±¸-Ñü´ø
	{ UIEP_FINGER1,		"Ring1"		},	//×°±¸-½äÖ¸
	{ UIEP_FINGER2,		"Ring2"		},	//×°±¸-½äÖ¸
	{ UIEP_WAIST_DECOR,	"Pendant"	},	//×°±¸-Ñü×º
	{ UIEP_FOOT,		"Shoes"		},	//×°±¸-Ð¬×Ó
	{ UIEP_HORSE,		"Horse"		},	//×°±¸-Âí
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
KUiParadeItem* KUiParadeItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiParadeItem* KUiParadeItem::OpenWindow(KUiPlayerItem* pDest)//pdest la splayer do à , gio lay cai teamid do ra la id cua avatar thoi
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiParadeItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		
		char Scheme[256];
		g_UiBase.GetCurSchemePath(Scheme, 256);
		LoadScheme(Scheme);
		m_pSelf->UpdateData(pDest);
		m_pSelf->BringToTop();
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
void KUiParadeItem::CloseWindow(bool bDestroy)
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

		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM_END, 0, 0);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
void KUiParadeItem::Initialize()
{
	AddChild(&m_Name);
	AddChild(&m_Title);
	AddChild(&m_Tong);
	AddChild(&m_WorldRank);
	AddChild(&m_Avt);
	AddChild(&m_PKValue);
	AddChild(&m_FuYuan);
	AddChild(&m_Repute);
	AddChild(&m_TransLife);
	AddChild(&m_MateName);
	AddChild(&m_Close);
	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		if(i==UIEP_MASK)
			AddChild(&m_EquipExpandImg);
		m_EquipBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_EquipBox[i]);
		m_EquipBox[i].SetContainerId((int)UOC_EQUIPTMENT);
	}
	AddChild(&m_EquipExpandBtn);
	SwitchExpand(FALSE);

	Wnd_AddWindow(this);
}

void KUiParadeItem::SwitchExpand(BOOL bShow)
{
	m_EquipExpandBtn.CheckButton(bShow);
		bShow?m_EquipExpandImg.Show():m_EquipExpandImg.Hide();
			for (int i = UIEP_MANTLE; i < _ITEM_COUNT; i ++)
			bShow?m_EquipBox[i].Show():m_EquipBox[i].Hide();
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiParadeItem::LoadScheme(const char* pScheme)
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
void KUiParadeItem::LoadScheme(class KIniFile* pIni)
{
	if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, m_Dest.uId))
		Init(pIni, "Male");
	else
		Init(pIni, "Female");

	m_Avt	  .Init(pIni, "Face");
	m_Name    .Init(pIni, "Name");
	m_Title   .Init(pIni, "Title");
	m_Tong    .Init(pIni, "Tong");
	m_WorldRank   .Init(pIni, "WorldRank");
	m_PKValue   .Init(pIni, "PKValue");
	m_FuYuan    .Init(pIni, "FuYuan");
	m_Repute    .Init(pIni, "Prestige");
	m_TransLife   .Init(pIni, "TransLife");
	m_MateName   .Init(pIni, "MateName");

	m_Close	 .Init(pIni, "Close");
	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].Init(pIni, CtrlItemMap[i].pIniSection);
	}
	m_EquipExpandBtn.Init(pIni, "EquipExpandBtn");
	m_EquipExpandImg.Init(pIni, "EquipExpandImg");
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿Úº¯Êý
//--------------------------------------------------------------------------
int KUiParadeItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
			Hide();
		else if (uParam == (unsigned int)(KWndWindow*)&m_EquipExpandBtn)
			SwitchExpand(m_EquipExpandBtn.IsButtonChecked());
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂ»ù±¾Êý¾Ý£¨ÈËÃûµÈ²»Ò×±äÊý¾Ý£©
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateBaseData(KUiPlayerItem* pDest)
{
	if (!g_pCoreShell)
		return;

	KUiPlayerBaseInfo	Info;
	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, pDest->uId);
	m_Name  .SetText(Info.Name);
	m_Title .SetText(Info.Title);
	if (Info.szTongName[0])
		m_Tong  .SetText(Info.szTongName);
	else
		m_Tong  .SetText("Ch­a vµo bang");
	m_WorldRank   .SetIntText(Info.nRankInWorld, true);
	m_PKValue   .SetIntText(Info.nPKValue);
	m_FuYuan    .SetIntText(Info.nFuYuan);
	m_Repute    .SetIntText(Info.nRepute);
	m_TransLife   .Set4IntText(Info.nTranslife);
	if(Info.szMateName[0])
		m_MateName  .SetText(Info.szMateName);
	else
		m_MateName  .SetText(NORMAL_UNCLEAR_WORD);
	if (pDest->nTeamID == 1)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu1.spr",true);
		
	}
	else if (pDest->nTeamID == 2)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu2.spr",true);
		
	}	
	else if (pDest->nTeamID == 3)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu3.spr",true);
		
	}
	else if (pDest->nTeamID == 4)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu4.spr",true);
		
	}
	else if (pDest->nTeamID == 5)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu5.spr",true);
		
	}
	else if (pDest->nTeamID == 6)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu6.spr",true);
		
	}
	else if (pDest->nTeamID == 7)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu7.spr",true);
		
		
	}
	else if (pDest->nTeamID == 8)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu8.spr",true);
		
	}
	else if (pDest->nTeamID == 9)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu9.spr",true);
		
	}
	else if (pDest->nTeamID == 10)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu10.spr",true);
		
	}
	else if (pDest->nTeamID == 11)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu11.spr",true);
		
	}
	else if (pDest->nTeamID == 12)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nu12.spr",true);
		
	}
	else if (pDest->nTeamID == 13)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam1.spr",true);
		
	}
	else if (pDest->nTeamID == 14)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam2.spr",true);
		
	}
	else if (pDest->nTeamID == 15)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam3.spr",true);
		
	}
	else if (pDest->nTeamID == 16)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam4.spr",true);
		
	}
	else if (pDest->nTeamID == 17)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam5.spr",true);
		
	}
	else if (pDest->nTeamID == 18)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam6.spr",true);
		
	}
	else if (pDest->nTeamID == 19)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam7.spr",true);
		
	}
	else if (pDest->nTeamID == 20)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam8.spr",true);
		
	}
	else if (pDest->nTeamID == 21)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam9.spr",true);
		
	}
	else if (pDest->nTeamID == 22)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam10.spr",true);
		
	}
	else if (pDest->nTeamID == 23)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam11.spr",true);
		
	}
	else if (pDest->nTeamID == 24)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam12.spr",true);
		
	}
	else if (pDest->nTeamID == 25)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam13.spr",true);
		
	}
	else if (pDest->nTeamID == 26)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam14.spr",true);
		
	}
	else if (pDest->nTeamID == 27)
	{
		
		m_Avt.SetImage(ISI_T_SPR,"\\spr\\Ui3\\UiChooseFace\\Nam15.spr",true);
		
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¸üÐÂÊý¾Ý
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateData(KUiPlayerItem* pDest)
{
	if (pDest == NULL)
		return;

	UpdateBaseData(pDest);

	UpdateAllEquips(pDest);

	m_Dest = *pDest;
}

void KUiParadeItem::UpdateAllEquips(KUiPlayerItem* pDest)
{
	if (!g_pCoreShell)
		return;

	KUiObjAtRegion	Equips[_ITEM_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_PARADE_EQUIPMENT, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < _ITEM_COUNT; i++)
		m_EquipBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateEquip(&Equips[i], true);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º×°±¸±ä»¯¸üÐÂ
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateEquip(KUiObjAtRegion* pEquip, int bAdd)
{
	if (pEquip)
	{
		for (int i = 0; i < _ITEM_COUNT; i++)
		{
			if (CtrlItemMap[i].nPosition == pEquip->Region.v)
			{
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
