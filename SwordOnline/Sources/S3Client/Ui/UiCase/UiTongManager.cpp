/********************************************************************
File        : UiTongManager.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : ¹ÜÀí°ï»áµÄ½çÃæ
*********************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "CoreUseNameDef.h"
#include "KPlayerDef.h"

#include "../elem/wnds.h"
#include "../elem/wndmessage.h"
#include "../elem/popupmenu.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "../../../Engine/src/KFilePath.h"
#include "../../../Engine/src/KSG_StringProcess.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"

#include "UiTongCreateSheet.h"
#include "UiTongAssignBox.h"
#include "UiTongManager.h"
#include "UiInformation.h"
#include "UiTongGetString.h"
#include "UiGetMoney.h"
#include "UiInformation.h"
#include "UiSysMsgCentre.h"
#include "UiPlayerBar.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"

#include <time.h>

KUiTongManager* KUiTongManager::ms_pSelf = NULL;
KTongInfo       KUiTongManager::m_TongData;
KTongParam       KUiTongManager::m_TongParam;
int             KUiTongManager::m_nElderDataIndex = 0;
int             KUiTongManager::m_nMemberDataIndex = 0;
int             KUiTongManager::m_nCaptainDataIndex = 0;

#define TONG_MANAGER_INI "UiTongManager.ini"
#define TONG_DATA_TEMP_FILE "°ï»áÁÙÊ±ÎÄ¼þ.ini"

#define TONG_REQUEST_INTERVAL 			100000

enum INPUT_STRING_PARAM
{
	ISP_NEW_AGNAME		= 0x100,
	ISP_NEW_MALEAGNAME	= 0x200,
	ISP_NEW_FEMALEAGNAME	= 0x300,
	ISP_NEW_NOTIFY		= 0x400,
};

char szArray_QingXiang[][64] = 
{
	"Bá trèng môc nµy",
	"Hoµi niÖm",
	"ThÝch PK",
	"Tranh b¸",
	"TÝnh n¨ng PVE",
	"KÕt giao",
	"T×m cè nh©n",
	"Dù thi v« tranh"
};

char szArray_Jiyu[][64] = 
{
	"Bá trèng môc nµy",
	"Tèng kim",
	"V­ît ¶i",
	"§¸nh boss",
	"C«ng thµnh",
	"Phong l¨ng ®é",
	"D· tÈu",
	"Viªm ®Õ",
	"TÝn sø"
};

char szArray_Jiyu_Ne[][64] = 
{
	"Kh«ng hiÓn thÞ",
	"Kim Long",
	"Thanh Long",
	"B¹ch Long",
	"Háa Long",
	"H¾c Long"
};

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell* g_pCoreShell;
extern KUiInformation g_UiInformation;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongManager::KUiTongManager()
{
	m_szData[0] = 0;
	m_nSelectIndex = -1;
	m_nCurrentCheckBox = -1;
	m_TongData.szName[0] = 0;
	m_szTargetPlayerName[0] = 0;
	m_uLastRequestTongDataTime = 0;
	m_uLastRequestElderListTime = 0;
	m_uLastRequestMemberListTime = 0;
	m_uLastRequestCaptainListTime = 0;
	m_bOtherTong = false;
}

KUiTongManager::~KUiTongManager()
{

}


/*********************************************************************
* ¹¦ÄÜ£º´ò¿ª´°¿Ú
**********************************************************************/
KUiTongManager* KUiTongManager::OpenWindow()
{
	if(g_pCoreShell)
	{
    	if (ms_pSelf == NULL)
    	{
		    ms_pSelf = new KUiTongManager;
		    if (ms_pSelf)
    			ms_pSelf->Initialize();
    	}
    	if (ms_pSelf)
    	{
		    UiSoundPlay(UI_SI_WND_OPENCLOSE);
			ms_pSelf->BringToTop();
			ms_pSelf->Show();
	    }
	}
	return ms_pSelf;
}


/*********************************************************************
* ¹¦ÄÜ£º´ò¿ª´°¿Ú£¬Í¬Ê±°²ÅÅ½çÃæ²¼¾Ö
**********************************************************************/
KUiTongManager* KUiTongManager::OpenWindow(char* pszPlayerName)
{
	OpenWindow();
	if(ms_pSelf)
	{
		ms_pSelf->ArrangeComposition(pszPlayerName);
	}
	return ms_pSelf;
}


/*********************************************************************
* ¹¦ÄÜ£º¸ù¾Ý´«ÈëµÄÍæ¼ÒÃû×Ö£¬°²ÅÅ½çÃæ²¼¾Ö
**********************************************************************/
void KUiTongManager::ArrangeComposition(char* pszPlayerName)
{
	if(g_pCoreShell)
	{
    	KUiPlayerItem Player;
		int nKind;

    	ClearTongData();
    	memset(&Player, 0, sizeof(KUiPlayerItem));
    	if(pszPlayerName && pszPlayerName[0])
    	{
			m_bOtherTong = true;
			if(g_pCoreShell->FindSpecialNPC(pszPlayerName, &Player, nKind))
				m_nIndex = Player.nIndex;
	    }
	    else
	    {
    		KUiPlayerBaseInfo Me;
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&Me, 0);
			if(g_pCoreShell->FindSpecialNPC(Me.Name, &Player, nKind))
				m_nIndex = Player.nIndex;
	    }
		if(m_nIndex >= 0)
		{
			strcpy(m_szPlayerName, Player.Name);
	        ms_pSelf->UpdateBtnTheme(-1, TRUE);

	        g_pCoreShell->TongOperation(GTOI_REQUEST_PLAYER_TONG, (unsigned int)&Player, (int)TRUE);
		}
		else
		{
			CloseWindow();
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÈç¹û´°¿ÚÕý±»ÏÔÊ¾£¬Ôò·µ»ØÊµÀýÖ¸Õë
**********************************************************************/
KUiTongManager* KUiTongManager::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* ¹¦ÄÜ£º¹Ø±Õ´°¿Ú£¬Í¬Ê±¿ÉÒÔÑ¡ÔòÊÇ·ñÉ¾³ý¶ÔÏóÊµÀý
**********************************************************************/
void KUiTongManager::CloseWindow(bool bDestory)
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


/*********************************************************************
* ¹¦ÄÜ£º³õÊ¼»¯
**********************************************************************/
void KUiTongManager::Initialize()
{
	AddChild(&m_MemberCount);
	AddChild(&m_MasterName);
	AddChild(&m_Alignment);
	AddChild(&m_TongName);
	AddChild(&m_Money);
	AddChild(&m_TotalEff);
	AddChild(&m_TongLevel);

	//AddChild(&m_TitleTongInfo);
	AddChild(&m_TitleTongNotify);

	AddChild(&m_BtnDismiss);
	AddChild(&m_BtnRecruit);
	AddChild(&m_BtnAssign);
	AddChild(&m_BtnDemise);
	AddChild(&m_BtnLeave);

	AddChild(&m_BtnCaptainList);
	AddChild(&m_BtnMemberList);
	AddChild(&m_BtnElderList);
	AddChild(&m_BtnLeagueList);
	AddChild(&m_BtnCancel);
	AddChild(&m_BtnApply);

	AddChild(&m_List);

	AddChild(&m_BtnPrev);
	AddChild(&m_BtnNext);
	
	AddChild(&m_BtnChangeCamp);
	AddChild(&m_BtnRefresh);
	AddChild(&m_BtnClose);

	AddChild(&m_BtnDepositMoney);
	AddChild(&m_BtnTakeMoney);

	AddChild(&m_AutoAcceptTitle);
	AddChild(&m_AutoAcceptLevel);
	AddChild(&m_Save);
	
	AddChild(&m_QingXiangTitle);
	AddChild(&m_JiyuTitle);
	for(int i=0;i<MAX_TONGJIYU;i++)
	{
		if(i==4)
		{
		}
		else
			AddChild(&m_JiyuText[i]);
		AddChild(&m_JiyuPopup[i]);
	}

	AddChild(&m_NotifyEdit);
	
	AddChild(&m_BtnChangeTitle);
	AddChild(&m_BtnChangeMaleTitle);
	AddChild(&m_BtnChangeFemaleTitle);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

/*********************************************************************
* ¹¦ÄÜ£ºÔØÈë½çÃæ·½°¸
**********************************************************************/
void KUiTongManager::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, TONG_MANAGER_INI);

		if(Ini.Load(Buff))
		{
			ms_pSelf->Init(&Ini, "Main");
			ms_pSelf->m_TongName.Init(&Ini, "TextTongName");
	        ms_pSelf->m_MasterName.Init(&Ini, "TextMasterName");
	        ms_pSelf->m_Alignment.Init(&Ini, "Faction");
	        ms_pSelf->m_MemberCount.Init(&Ini, "MemberCount");
			ms_pSelf->m_MemberCount.Init(&Ini, "MemberCount");
			ms_pSelf->m_TongLevel.Init(&Ini, "TongLevel");
			ms_pSelf->m_TotalEff.Init(&Ini, "TotalEff");
	        ms_pSelf->m_Money.Init(&Ini, "Money");

			ms_pSelf->m_TitleTongInfo.Init(&Ini, "TitleTongInfo");
			ms_pSelf->m_TitleTongNotify.Init(&Ini, "TitleTongNotify");

	        ms_pSelf->m_BtnDismiss.Init(&Ini, "BtnDismiss");
	        ms_pSelf->m_BtnAssign.Init(&Ini, "BtnAssign");
	        ms_pSelf->m_BtnDemise.Init(&Ini, "BtnDemise");
	        ms_pSelf->m_BtnLeave.Init(&Ini, "BtnLeave");
	        ms_pSelf->m_BtnRecruit.Init(&Ini, "BtnRecruit");

	        ms_pSelf->m_BtnApply.Init(&Ini, "BtnApply");
	        ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");
			ms_pSelf->m_BtnElderList.Init(&Ini, "BtnDirectorList");
	        ms_pSelf->m_BtnCaptainList.Init(&Ini, "BtnManagerList");
	        ms_pSelf->m_BtnMemberList.Init(&Ini, "BtnMemberList");
			ms_pSelf->m_BtnLeagueList.Init(&Ini, "BtnLeagueList");

			ms_pSelf->m_List.Init(&Ini, "List");

			ms_pSelf->m_BtnPrev.Init(&Ini, "Prev");
			ms_pSelf->m_BtnNext.Init(&Ini, "Next");
			
			ms_pSelf->m_BtnChangeCamp.Init(&Ini, "BtnChangeCamp");
			ms_pSelf->m_BtnRefresh.Init(&Ini, "BtnRefresh");
			ms_pSelf->m_BtnClose.Init(&Ini, "BtnClose");

	        ms_pSelf->m_BtnDepositMoney.Init(&Ini, "BtnDepositMoney");
	        ms_pSelf->m_BtnTakeMoney.Init(&Ini, "BtnTakeMoney");				
			
			ms_pSelf->m_AutoAcceptTitle.Init(&Ini, "AutoAcceptTitle");					
			ms_pSelf->m_AutoAcceptLevel.Init(&Ini, "AutoAcceptLevel");					
			ms_pSelf->m_Save.Init(&Ini, "Save");					

	        ms_pSelf->m_QingXiangTitle.Init(&Ini, "QingXiangTitle");
			ms_pSelf->m_JiyuTitle.Init(&Ini, "JiyuTitle");
			char szKey[16];
			for(int i=0;i<MAX_TONGJIYU;i++)
			{
				sprintf(szKey, "JiyuText%d",i);
				ms_pSelf->m_JiyuText[i].Init(&Ini, szKey);
				sprintf(szKey, "JiyuPopup%d",i);
				ms_pSelf->m_JiyuPopup[i].Init(&Ini, szKey);
			}
			ms_pSelf->m_NotifyEdit.Init(&Ini, "NotifyEdit");
			
			ms_pSelf->m_BtnChangeTitle.Init(&Ini, "Title");
			ms_pSelf->m_BtnChangeMaleTitle.Init(&Ini, "BtnMaleTitle");
			ms_pSelf->m_BtnChangeFemaleTitle.Init(&Ini, "BtnFemaleTitle");
			
			Ini.GetString("Main", "StringDismiss", "", ms_pSelf->m_szDismiss, sizeof(ms_pSelf->m_szDismiss));
			Ini.GetString("Main", "StringAssign", "", ms_pSelf->m_szAssign, sizeof(ms_pSelf->m_szAssign));
			Ini.GetString("Main", "StringDemise", "", ms_pSelf->m_szDemise, sizeof(ms_pSelf->m_szDemise));
			Ini.GetString("Main", "StringLeave", "", ms_pSelf->m_szLeave, sizeof(ms_pSelf->m_szLeave));
			Ini.GetString("Main", "StringSave", "", ms_pSelf->m_szSaveJiyu, sizeof(ms_pSelf->m_szSaveJiyu));

			Ini.GetString("Main", "StringJustice", "", ms_pSelf->m_szJustice, sizeof(ms_pSelf->m_szJustice));
			Ini.GetString("Main", "StringBalance", "", ms_pSelf->m_szBalance, sizeof(ms_pSelf->m_szBalance));
			Ini.GetString("Main", "StringEvil", "", ms_pSelf->m_szEvil, sizeof(ms_pSelf->m_szEvil));

			Ini.GetString("Main", "StringConfirm", "", ms_pSelf->m_szConfirm, sizeof(ms_pSelf->m_szConfirm));
			Ini.GetString("Main", "StringCancel", "", ms_pSelf->m_szCancel, sizeof(ms_pSelf->m_szCancel));

			Ini.GetInteger("List", "Font", 12, &ms_pSelf->m_nFontSize);
			Ini.GetInteger("List", "ItemLen", 4, &ms_pSelf->m_nItemLen);
			Ini.GetInteger("List", "NameLen", 10, &ms_pSelf->m_nNameLen);
			Ini.GetInteger("List", "AgnameLen", 10, &ms_pSelf->m_nAgnameLen);
			int nWidth, nHeight;
			ms_pSelf->m_List.GetSize(&nWidth, &nHeight);
			ms_pSelf->m_nLineLen = nWidth * 2 / ms_pSelf->m_nFontSize - 2;

			ms_pSelf->UpdatePageofList();
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£º´°¿Úº¯Êý
**********************************************************************/
int KUiTongManager::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_LIST_ITEM_SEL:
		if(uParam == (unsigned int)&m_List)
		{
			m_nSelectIndex = nParam;
			if (m_nSelectIndex == -1)
			{
				m_BtnDismiss.Enable(FALSE);
				m_BtnAssign.Enable(FALSE);
				m_BtnDemise.Enable(FALSE);
				m_BtnChangeTitle.Enable(FALSE);
			}
			else
			{
				if (!m_bOtherTong)
					UpdateBtnTheme(m_Relation);
			}
		}
		break;
	case WND_N_LIST_ITEM_D_CLICK:
		if(uParam == (unsigned int)&m_List)
		{
			m_nSelectIndex = nParam;
			if (m_nSelectIndex != -1)
			{
				KPilgarlicItem *pNode = GetSelectedPlayer();
				if (!KUiPlayerBar::IsSelfName((char*)pNode->Name))
					KUiPlayerBar::InputNameMsg(false, (char*)pNode->Name, true);
			}
		}		
		break;
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnPrev)
		{
			m_nCurrentPage--;
			UpdatePageofList();
		}
		else if(uParam == (unsigned int)&m_BtnNext)
		{
			m_nCurrentPage++;
			UpdatePageofList();
		}
		else if(uParam == (unsigned int)&m_BtnChangeCamp)
		{
			KUiTongCreateSheet::OpenWindow();
			KUiTongCreateSheet::LinkToMainWindow(this);
			KUiTongCreateSheet::ArrangeData(m_TongData.szName, m_TongData.nFaction, RESULT_T_CCAMP);
		}
		else if(uParam == (unsigned int)&m_BtnDismiss)
		{
			PopupConfirmWindow(m_szDismiss, RESULT_T_DISMISS);
		}
		else if(uParam == (unsigned int)&m_BtnAssign)
		{
			KPilgarlicItem *pNode = GetSelectedPlayer();

			KUiTongAssignBox::OpenWindow();
			KUiTongAssignBox::LinkToMainWindow(this);
			KUiTongAssignBox::ArrangeData(m_nRelation, pNode->Name, pNode->szAgname, m_nCurrentCheckBox, m_nIndex);
		}
		else if(uParam == (unsigned int)&m_BtnDemise)
		{
			PopupConfirmWindow(m_szDemise, RESULT_T_DEMISE);
		}
		else if(uParam == (unsigned int)&m_BtnLeave)
		{
			PopupConfirmWindow(m_szLeave, RESULT_T_LEAVE);
		}
		else if(uParam == (unsigned int)&m_BtnRecruit)
		{
			if (m_bOtherTong)
				m_BtnRecruit.CheckButton(m_TongData.bRecruit);
			else
			{
				if (m_nRelation != enumTONG_FIGURE_MASTER)
					m_BtnRecruit.CheckButton(m_TongData.bRecruit);
				else
					g_pCoreShell->TongOperation(GTOI_TONG_RECRUIT,
										m_BtnRecruit.IsButtonChecked(), 0);
			}
		}
		else if(uParam == (unsigned int)&m_BtnApply)
		{
			OnAction(TONG_ACTION_APPLY);
		}
		else if(uParam == (unsigned int)&m_BtnCancel || uParam == (unsigned int)&m_BtnClose)
		{
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_BtnRefresh)
		{
			OnRefresh();
		}
		else if(uParam == (unsigned int)&m_BtnElderList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_DIRECTOR);
			if(m_ElderData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_DIRECTOR);
			else
				OnRefresh();
		}
		else if(uParam == (unsigned int)&m_BtnCaptainList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_MANAGER);
			if(m_CaptainData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_MANAGER);
			else
				OnRefresh();
		}
		else if(uParam == (unsigned int)&m_BtnMemberList)
		{
			UpdateListCheckButton(enumTONG_FIGURE_MEMBER);
			if(m_MemberData.GetCount() == 0)
			    LoadData(enumTONG_FIGURE_MEMBER);
			else
				OnRefresh();
		}	
		else if (uParam == (unsigned int)&m_BtnDepositMoney)
		{
			int nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
			if(nMoney)
				KUiGetMoney::OpenWindow(0, nMoney, this, RESULT_T_SAVE, NULL);
		}
		else if (uParam == (unsigned int)&m_BtnTakeMoney)
		{
			if(m_TongData.nMoney)
				KUiGetMoney::OpenWindow(0, m_TongData.nMoney, this, RESULT_T_GET, NULL);
		}
		else if (uParam == (unsigned int)&m_BtnChangeTitle)
			KUiTongGetString::OpenWindow(true, "§æi danh hiÖu", NULL, this, ISP_NEW_AGNAME, 4, defTONG_NAME_LENGTH_12);
		else if (uParam == (unsigned int)&m_BtnChangeMaleTitle)
			KUiTongGetString::OpenWindow(true, "§æi hiÖu nam", NULL, this, ISP_NEW_MALEAGNAME, 4, defTONG_NAME_LENGTH_12);
		else if (uParam == (unsigned int)&m_BtnChangeFemaleTitle)
			KUiTongGetString::OpenWindow(true, "§æi hiÖu n÷", NULL, this, ISP_NEW_FEMALEAGNAME, 4, defTONG_NAME_LENGTH_12);
		else if (uParam == (unsigned int)&m_Save)
		{
			PopupConfirmWindow(m_szSaveJiyu, RESULT_T_SAVEJIYU);
		}
		else
		{
			for(int i=0; i<MAX_TONGJIYU;i++)
			{
				if(uParam == (unsigned int)&m_JiyuPopup[i])
					OpenJiyuPopup(i);
			}
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		Show();
		if (nParam && uParam < RESULT_T_LEAVE)
		{
			strcpy(m_szTargetPlayerName, (char *)nParam);
		}
		if (uParam == RESULT_T_DISMISS && nParam)
		{
			OnAction(TONG_ACTION_DISMISS);
		}
		else if (uParam == RESULT_T_ASSIGN && nParam)
		{
			OnAction(TONG_ACTION_ASSIGN);
		}
		else if (uParam == RESULT_T_DEMISE)
		{
			if(nParam)
			{
			    OnAction(TONG_ACTION_DEMISE);
			}
			else
			{
				memset(m_szData, 0, sizeof(m_szData));
			}
		}
		else if (uParam == RESULT_T_LEAVE)
		{
			if (!nParam)
				OnAction(TONG_ACTION_LEAVE);
		}
		else if (uParam == RESULT_T_CCAMP)
		{
			switch(nParam)
			{
			case camp_justice:
				OnAction(TONG_ACTION_CHANGE_CAMP_JUSTIE);
				break;

			case camp_evil:
				OnAction(TONG_ACTION_CHANGE_CAMP_EVIL);
				break;

			case camp_balance:
				OnAction(TONG_ACTION_CHANGE_CAMP_BALANCE);
				break;
			}
		}	
		else if (uParam == RESULT_T_SAVE)
		{
			if (nParam > 0)
				OnMoney(0, nParam);
		}
		else if (uParam == RESULT_T_GET)
		{
			if (nParam > 0)
				OnMoney(1,nParam);
		}
		else if (uParam == RESULT_T_SAVEJIYU)
		{
			if(nParam==0)
			{
			KTongInfo Info;
			strcpy(Info.szName, m_szPlayerName);
			Info.nTongJiyuParam = m_nTongJiyuParam;
			m_NotifyEdit.GetText(Info.szTongJiyuNotify, sizeof(Info.szTongJiyuNotify), false);
			g_pCoreShell->TongOperation(GTOI_TONG_CHANGE_CREDIT, (unsigned int)&Info, 0);
			}
		}
		else if (uParam == ISP_NEW_AGNAME)
		{
			if(nParam)
			{
				strcpy(m_szData, (const char *)nParam);
				OnAction(TONG_ACTION_CHANGE_AGNAME);
			}
			else
			{
				memset(m_szData, 0, sizeof(m_szData));
			}
		}			
		else if (uParam == ISP_NEW_MALEAGNAME)
		{
			if(nParam)
			{
				strcpy(m_szData, (const char *)nParam);
				OnAction(TONG_ACTION_CHANGE_MALE_AGNAME);
			}
			else
			{
				memset(m_szData, 0, sizeof(m_szData));
			}
		}			
		else if (uParam == ISP_NEW_FEMALEAGNAME)
		{
			if(nParam)
			{
				strcpy(m_szData, (const char *)nParam);
				OnAction(TONG_ACTION_CHANGE_FEMALE_AGNAME);
			}
			else
			{
				memset(m_szData, 0, sizeof(m_szData));
			}
		}
		else if (uParam == ISP_NEW_NOTIFY)
		{
			if(nParam)
			{
				strcpy(m_szNotify, (const char *)nParam);
				m_NotifyEdit.SetText(m_szNotify);
			}
			else
			{
				memset(m_szNotify, 0, sizeof(m_szNotify));
				m_NotifyEdit.Clear();
			}
		}
		m_szTargetPlayerName[0] = 0;
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			int i=(int)(HIWORD(nParam));
			short j=(short)(LOWORD(nParam));
			if (i>= 0 && i< MAX_TONGJIYU && j>=0)
			{
			m_nTongJiyuParam=KSG_StringSetValue(1,m_nTongJiyuParam,i+1,j);
			if(i==0)
				m_JiyuText[i].SetText(szArray_QingXiang[j]);
			else
				m_JiyuText[i].SetText(szArray_Jiyu[j]);	
			}
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCheckInput();
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 1;
}

/*********************************************************************
* ¹¦ÄÜ£º¸üÐÂ°´Å¥ÅäÖÃ·½°¸
**********************************************************************/
void KUiTongManager::OnCheckInput()
{
	int nLevel = m_AutoAcceptLevel.GetIntNumber();
	m_nTongJiyuParam=KSG_StringSetValue(3,m_nTongJiyuParam,3,nLevel);

	char	szBuff1[16], szBuff2[16];
	itoa(nLevel, szBuff1, 10);
	m_AutoAcceptLevel.GetText(szBuff2, sizeof(szBuff2), true);
	if (strcmp(szBuff1, szBuff2))
	{
		m_AutoAcceptLevel.SetIntText(nLevel);
	}
}

/*********************************************************************
* ¹¦ÄÜ£º¸üÐÂ°´Å¥ÅäÖÃ·½°¸
**********************************************************************/
void KUiTongManager::UpdateBtnTheme(int nType, BOOL bDissable)
{
	m_BtnDismiss.Enable(false);
	m_BtnAssign.Enable(false);
	m_BtnDemise.Enable(false);
	
	m_BtnLeave.Hide();

	m_BtnCaptainList.Enable(false);
	m_BtnMemberList.Enable(false);
	m_BtnElderList.Enable(false);
	m_BtnLeagueList.Enable(false);
	m_BtnCancel.Hide();
	m_BtnApply.Hide();
	m_BtnRefresh.Enable(false);

	m_BtnDepositMoney.Enable(false);
	m_BtnTakeMoney.Enable(false);
	m_BtnChangeCamp.Enable(false);

	for(int i=0; i<MAX_TONGJIYU;i++)
		m_JiyuPopup[i].Enable(false);
	m_NotifyEdit.Enable(false);
	m_AutoAcceptLevel.Enable(false);
	m_Save.Enable(false);
	m_BtnChangeMaleTitle.Enable(false);
	m_BtnChangeTitle.Enable(false);
	m_BtnChangeFemaleTitle.Enable(false);

	m_BtnRecruit.CheckButton(m_TongData.bRecruit);
	if (m_bOtherTong)
	{
		if (g_pCoreShell && !g_pCoreShell->TongOperation(GTOI_TONG_FLAG, 0, 0))
			m_BtnApply.Show();
		UpdateListCheckButton(enumTONG_FIGURE_DIRECTOR);
		return;
	}

	if (bDissable || g_pCoreShell->GetLockState())
		return;

	switch(nType)
	{
	case enumTONG_FIGURE_MASTER:
	{
		KPilgarlicItem *pNode = GetSelectedPlayer();
		if (m_nSelectIndex > -1 && !KUiPlayerBar::IsSelfName((char*)pNode->Name))
		{
			m_BtnDismiss.Enable(true);
			m_BtnAssign.Enable(true);
			m_BtnDemise.Enable(true);
			m_BtnChangeTitle.Enable(true);
		}

		m_BtnCaptainList.Enable(true);
	    m_BtnMemberList.Enable(true);
	    m_BtnElderList.Enable(true);
		
		m_NotifyEdit.Enable(true);
		for(int i=0; i<MAX_TONGJIYU;i++)
			m_JiyuPopup[i].Enable(true);
		
		m_BtnDepositMoney.Enable(true);
		m_BtnTakeMoney.Enable(true);
		
		m_BtnChangeMaleTitle.Enable(true);
		m_BtnChangeFemaleTitle.Enable(true);
		
		m_BtnChangeCamp.Enable(true);
		m_AutoAcceptLevel.Enable(true);
		m_Save.Enable(true);
		m_BtnRefresh.Enable(true);
		break;
	}
	case enumTONG_FIGURE_DIRECTOR:
	{
		KPilgarlicItem *pNode = GetSelectedPlayer();
		if (m_nSelectIndex > -1 && !KUiPlayerBar::IsSelfName((char*)pNode->Name))
		{
			m_BtnAssign.Enable(true);
			m_BtnDismiss.Enable(true);
		}

		m_BtnCaptainList.Enable(true);
	    m_BtnMemberList.Enable(true);
	    m_BtnElderList.Enable(true);
		
		m_BtnDepositMoney.Enable(true);
		m_BtnTakeMoney.Enable(true);
		
		m_BtnRefresh.Enable(true);
		break;
	}
	case enumTONG_FIGURE_MANAGER:
	case enumTONG_FIGURE_MEMBER:
		m_BtnLeave.Show();

		m_BtnCaptainList.Enable(true);
	    m_BtnMemberList.Enable(true);
	    m_BtnElderList.Enable(true);
		
		m_BtnDepositMoney.Enable(true);
		
		m_BtnRefresh.Enable(true);
		break;
	default:
		m_BtnApply.Show();
		m_BtnCancel.Show();
		break;
	}
}


/*********************************************************************
* ¹¦ÄÜ£º¸üÐÂ°ï»áÑ¶Ï¢µÄÏÔÊ¾
**********************************************************************/
void KUiTongManager::UpdateTongInfoView()
{
	char Buff[MAX_MESSAGE_LENGTH];

	m_TongName.SetText(m_TongData.szName);
	m_MasterName.SetText(m_TongData.szMasterName);
	time_t rawtime=(m_TongData.nMasterJoinTm+1451581200);
	struct tm * timeinfo = localtime ( &rawtime );
	int nMsgLen=sprintf(Buff, "<color=189,190,189>LËp bang: <color=0,255,0>%02d:%02d:%02d %02d-%02d-%04d<color>\n\nCèng hiÕn tuÇn tr­íc: <color=0,255,0>%d<color>\nCèng hiÕn tuÇn nµy: <color=0,255,0>%d<color>\nCèng hiÕn kh¶ dông: <color=0,255,0>%d<color><color>", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900,
			KSG_StringGetValue(3,m_TongData.nMasterSaveEff,1), KSG_StringGetValue(3,m_TongData.nMasterSaveEff,2), KSG_StringGetValue(3,m_TongData.nMasterSaveEff,3));
	m_MasterName.SetToolTipInfo(Buff,nMsgLen);
	switch(m_TongData.nFaction)
	{
	case camp_justice:
		m_Alignment.SetText(m_szJustice);
		break;
	case camp_balance:
		m_Alignment.SetText(m_szBalance);
		break;
	case camp_evil:
		m_Alignment.SetText(m_szEvil);
		break;
	}
	m_MemberCount.SetText(itoa(m_TongData.nMemberCount+m_TongData.nManagerCount+m_TongData.nDirectorCount, Buff, 10));
	m_Money.Set3IntText(m_TongData.nMoney,true);
	m_TongLevel.SetIntText(m_TongData.btLevel);
	m_TotalEff.Set3IntText(m_TongData.dwTotalEff,true,"®iÓm");
	m_nTongJiyuParam=m_TongData.nTongJiyuParam;
	for(int i=0;i<MAX_TONGJIYU;i++)
	{
		if(i==0)
			m_JiyuText[i].SetText(szArray_QingXiang[KSG_StringGetValue(1,m_TongData.nTongJiyuParam,i+1)]);
		else
			m_JiyuText[i].SetText(szArray_Jiyu[KSG_StringGetValue(1,m_TongData.nTongJiyuParam,i+1)]);
	}
	m_AutoAcceptLevel.SetIntText(KSG_StringGetValue(3,m_TongData.nTongJiyuParam,3));
	if (m_TongData.szTongJiyuNotify[0])
		m_NotifyEdit.SetText(m_TongData.szTongJiyuNotify);
		
	if (!m_bOtherTong)
	{
		UpdateBtnTheme(m_Relation);
	}
}
/*********************************************************************
* ¹¦ÄÜ£º¸üÐÂÁÐ±íÀ¸µÄÏÔÊ¾
**********************************************************************/
void KUiTongManager::UpdateTongListView()
{
	KLinkStruct<KPilgarlicItem> *pData = GetCurrentSelectedData();
	if(pData)
	{
    	nViewCount = m_List.GetCount();
    	nDataCount = pData->GetCount();
		if(nDataCount > nViewCount)
		{
			KPilgarlicItem *pNode = pData->GetItem(nViewCount);
			if(pNode)
			{
				char szMsgBuff[MAX_MESSAGE_LENGTH],szBuf[6];
				for(int i = nViewCount;i < nDataCount;i++)
				{
					int nMsgLen = 0,nCurLen=0;
					/*nCurLen = sprintf(szBuf, "%d", i+1);
					memcpy(szMsgBuff+nMsgLen,szBuf,nCurLen);
					if(nCurLen < m_nItemLen)
						memset(szMsgBuff+nMsgLen+nCurLen, 0x20, m_nItemLen-nCurLen);
					nMsgLen+=m_nItemLen;*/
					nCurLen=strlen(pNode->Name);
					memcpy(szMsgBuff+nMsgLen,pNode->Name,nCurLen);
					if(nCurLen < m_nNameLen)
						memset(szMsgBuff+nMsgLen+nCurLen, 0x20, m_nNameLen-nCurLen);
					nMsgLen+=m_nNameLen;
					nCurLen=strlen(pNode->szAgname);
					memcpy(szMsgBuff+nMsgLen,pNode->szAgname,nCurLen);
					if(nCurLen < m_nAgnameLen)
						memset(szMsgBuff+nMsgLen+nCurLen, 0x20, m_nAgnameLen-nCurLen);
					nMsgLen+=m_nAgnameLen;
					nCurLen=sprintf(szBuf, "%d", KSG_StringGetValue(3,pNode->nSaveEff,2));
					memcpy(szMsgBuff+nMsgLen,szBuf,nCurLen);
					nMsgLen+=nCurLen;
					szMsgBuff[nMsgLen] = '\0';
					if (m_List.AddString(i, szMsgBuff)> WNDLIST_ERROR)
					{
						time_t rawtime=(pNode->nJoinTm+1451581200);
						struct tm * timeinfo = localtime ( &rawtime );
						nMsgLen=sprintf(szMsgBuff, "<color=189,190,189>NhËp bang: <color=0,255,0>%02d:%02d:%02d %02d-%02d-%04d<color>\n\nCèng hiÕn tuÇn tr­íc: <color=0,255,0>%d<color>\nCèng hiÕn tuÇn nµy: <color=0,255,0>%d<color>\nCèng hiÕn kh¶ dông: <color=0,255,0>%d<color><color>", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon+1, timeinfo->tm_year+1900,
							KSG_StringGetValue(3,pNode->nSaveEff,1), KSG_StringGetValue(3,pNode->nSaveEff,2), KSG_StringGetValue(3,pNode->nSaveEff,3));
						m_List.SetItemColor(i, pNode->bOnline?0x00fffb6b:0x0000929c);
						m_List.SetToolTipInfo(i, szMsgBuff, nMsgLen);
					}
					pNode = pData->NextItem();
				}
			}
		}
		m_nCurrentPage = 0;
		UpdatePageofList();
	}
}

/*********************************************************************
* ¹¦ÄÜ£º·µ»Øµ±Ç°ËùÑ¡ÔñµÄÁÐ±íÀàÐÍËù¶ÔÓ¦µÄÊý¾Ý¼¯
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::GetCurrentSelectedData()
{
	switch(m_nCurrentCheckBox)
	{
	case enumTONG_FIGURE_DIRECTOR:
		return &m_ElderData;
		break;

	case enumTONG_FIGURE_MANAGER:
		return &m_CaptainData;
		break;

	case enumTONG_FIGURE_MEMBER:
		return &m_MemberData;
		break;

	default:
		return NULL;
		break;
	}
}


/*********************************************************************
* ¹¦ÄÜ£º·µ»ØËùÑ¡ÔñµÄ»áÔ±
**********************************************************************/
KPilgarlicItem* KUiTongManager::GetSelectedPlayer()
{
	KLinkStruct<KPilgarlicItem>* pData = NULL;
	KPilgarlicItem* pNode = NULL;

	pData = GetCurrentSelectedData();
	if(pData)
	{
    	pNode = pData->GetItem(m_nSelectIndex);
	}
	return pNode;
}


/*********************************************************************
* ¹¦ÄÜ£ºÈý¸öÁÐ±íCheckBoxµÄ¹ÜÀí
**********************************************************************/
void KUiTongManager::UpdateListCheckButton(int nType)
{
   	m_BtnElderList.CheckButton(FALSE);
   	m_BtnMemberList.CheckButton(FALSE);
   	m_BtnCaptainList.CheckButton(FALSE);

   	switch(nType)
   	{
   	case enumTONG_FIGURE_DIRECTOR:
	    m_BtnElderList.CheckButton(TRUE);
	    break;

    case enumTONG_FIGURE_MANAGER:
   		m_BtnCaptainList.CheckButton(TRUE);
	    break;

    case enumTONG_FIGURE_MEMBER:
   		m_BtnMemberList.CheckButton(TRUE);
	    break;

	default:
		return;
    }
	if(nType != m_nCurrentCheckBox)
	{
		m_nCurrentCheckBox = nType;
		m_nSelectIndex = -1;
		m_List.ResetContent();
		UpdateTongListView();
	}
}

void KUiTongManager::UpdatePageofList()
{
	m_BtnPrev.Enable(false);
	m_BtnNext.Enable(false);
	if (m_nCurrentPage < 0)
		m_nCurrentPage = 0;
	int nMax = 0;

	switch (m_nCurrentCheckBox)
   	{
   	case enumTONG_FIGURE_DIRECTOR:
		if (m_TongData.nDirectorCount/m_List.GetVisibleLineCount())
			nMax =  m_TongData.nDirectorCount/m_List.GetVisibleLineCount();
	    break;

    case enumTONG_FIGURE_MANAGER:
		if (m_TongData.nManagerCount/m_List.GetVisibleLineCount())
			nMax =  m_TongData.nManagerCount/m_List.GetVisibleLineCount();
	    break;

    case enumTONG_FIGURE_MEMBER:
		if (m_TongData.nMemberCount/m_List.GetVisibleLineCount())
			nMax =  m_TongData.nMemberCount/m_List.GetVisibleLineCount();
	    break;

	default:
		return;
    }
	if (m_nCurrentPage > nMax)
		m_nCurrentPage = nMax;

	m_List.SetTopItemIndex(m_nCurrentPage*m_List.GetVisibleLineCount());
	if (nMax > 0 && m_nCurrentPage < nMax)
		m_BtnNext.Enable(true);
	if (nMax > 0 && m_nCurrentPage > 0)
		m_BtnPrev.Enable(true);
}
/*********************************************************************
* ¹¦ÄÜ£º°Ñ°ï»áÐÅÏ¢À¸Çå¿Õ
**********************************************************************/
void KUiTongManager::ClearTongData()
{
	m_MemberCount.SetText("");
	m_MasterName.SetText("");
	m_Alignment.SetText("");
	m_TongName.SetText("");
	m_Money.SetText("");
	m_TongLevel.SetText("");

	m_List.ResetContent();

	m_CaptainData.Clear();
	m_MemberData.Clear();
	m_ElderData.Clear();

	m_nElderDataIndex = 0;
	m_nMemberDataIndex = 0;
	m_nCaptainDataIndex = 0;
	m_bOtherTong = false;
}


/*********************************************************************
* ¹¦ÄÜ£ºµ¯³öÈ·ÈÏ´°¿Ú
**********************************************************************/
void KUiTongManager::PopupConfirmWindow(const char* pszInfo, unsigned int uHandleID)
{
	KPilgarlicItem *pPlayer = GetSelectedPlayer();
	if(uHandleID == RESULT_T_LEAVE)
	{
		char szBuffer[256];
		sprintf(szBuffer, pszInfo, m_TongParam.m_nLeaveMoney, m_TongParam.m_nLeaveRepute, m_TongParam.m_nLeaveTime);
		UIMessageBox(szBuffer, this, m_szConfirm, m_szCancel, uHandleID);
	}
	else if(uHandleID == RESULT_T_SAVEJIYU)
	{
		char szBuffer[256];
		sprintf(szBuffer, pszInfo, m_TongParam.m_nMoneySaveJiyu);
		UIMessageBox(szBuffer, this, m_szConfirm, m_szCancel, uHandleID);
	}
	else if (pPlayer)
	    KUiTongGetString::OpenWindow(false, pszInfo, pPlayer->Name, this, uHandleID, 1, 31);
	else
		KUiTongGetString::OpenWindow(false, pszInfo, m_szPlayerName, this, uHandleID, 1, 31);
	//Hide();
}


/*********************************************************************
* ¹¦ÄÜ£º½ÓÊÜÐÂµÄÊý¾ÝµÄÊý¾Ý½Ó¿Ú
**********************************************************************/
void KUiTongManager::OpenJiyuPopup(int nIndex)
{
	int nActionDataCount = 0;
	if(nIndex ==0)
		nActionDataCount=sizeof(szArray_QingXiang) / sizeof(szArray_QingXiang[0]);
	else if(nIndex==4)
		nActionDataCount=sizeof(szArray_Jiyu_Ne) / sizeof(szArray_Jiyu_Ne[0]);
	else
		nActionDataCount=sizeof(szArray_Jiyu) / sizeof(szArray_Jiyu[0]);
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;

	for (int i = 0; i < nActionDataCount; i++)
	{
		if(nIndex==0)
			strncpy(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData, szArray_QingXiang[i], sizeof(szArray_QingXiang[i]));
		else if(nIndex==4)
			strncpy(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData, szArray_Jiyu_Ne[i], sizeof(szArray_Jiyu_Ne[i]));
		else
			strncpy(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData, szArray_Jiyu[i], sizeof(szArray_Jiyu[i]));
		pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData[sizeof(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData) - 1] = 0;
		pSelUnitMenu->Items[pSelUnitMenu->nNumItem].uDataLen = strlen(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	if(nIndex==4)
		m_JiyuPopup[nIndex].GetAbsolutePos(&x, &y);
	else
		m_JiyuText[nIndex].GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, ms_pSelf, nIndex);
}

void KUiTongManager::NewDataArrive(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome)
{
	KLinkStruct<KPilgarlicItem> *pData;
	int *pNum, nCount;

	if(ParamInfo->nData == enumTONG_FIGURE_DIRECTOR)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_ElderData;
		}
		pNum = &m_nElderDataIndex;
		nCount = m_TongData.nDirectorCount;
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MANAGER)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_CaptainData;
		}
		pNum = &m_nCaptainDataIndex;
		nCount = m_TongData.nManagerCount;
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MEMBER)
	{
		if(ms_pSelf)
		{
		    pData = &ms_pSelf->m_MemberData;
		}
		pNum = &m_nMemberDataIndex;
		nCount = m_TongData.nMemberCount;
	}
	else
	{
		return;
	}
	if(*pNum != ParamInfo->nParam)
	{
		return;
	}

	if(ParamInfo->uParam)
	{
		*pNum += ParamInfo->uParam;
		if(!strcmp(m_TongData.szName, ParamInfo->szName) && ms_pSelf)
		{
			KPilgarlicItem Add;

			for(int i = 0;i < ParamInfo->uParam;i++)
			{
				if(!pIncome[i].Name[0])
				{
					continue;
				}
				memcpy(&Add, pIncome + i, sizeof(KTongMemberItem));
				Add.nHashID = StringToHash(Add.Name);
				pData->AddItem(Add);
			}
			ms_pSelf->UpdateTongListView();
		}
		SaveNewData(ParamInfo, pIncome);
		if(*pNum < nCount)
		{
			RequestData(ParamInfo->nData, *pNum);
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£º²éÑ¯ÌØ¶¨Î»ÖÃµÄÇ°Ãæ£¬Ä³¸öÃû×ÖÊÇ·ñÒÑ¾­´æÔÚ
**********************************************************************/
int KUiTongManager::IsPlayerExist(KPilgarlicItem *MeToCmp, KLinkStruct<KPilgarlicItem> *pData)
{
	unsigned int nID;
	KPilgarlicItem *pNode;

	nID = MeToCmp->nHashID;
	pNode = pData->Begin();
	while(pNode || pNode != MeToCmp)
	{
		if(pNode->nHashID == nID)
		{
			if(!strcmp(MeToCmp->Name, pNode->Name))
			{
				return 1;
			}
		}
		pNode = pData->NextItem();
	}
	return 0;
}


/*********************************************************************
* ¹¦ÄÜ£º½ÓÊÜ°ï»áÐÅÏ¢
**********************************************************************/
void KUiTongManager::TongInfoArrive(KUiPlayerRelationWithOther* Relation, KTongInfo *pTongInfo)
{
	if(ms_pSelf)
	{
		if(Relation)
		{
			if(!strcmp(Relation->Name, ms_pSelf->m_szPlayerName))
			{
				g_pCoreShell->TongOperation(GTOI_TONG_PARAM, (unsigned int)&m_TongParam, 0);
				memcpy(&ms_pSelf->m_TongData, pTongInfo, sizeof(KTongInfo));
				ms_pSelf->m_nRelation = Relation->nRelation;

				ms_pSelf->UpdateTongInfoView();
				ms_pSelf->UpdateBtnTheme(Relation->nRelation);
				if(Relation->nRelation <= enumTONG_FIGURE_MASTER && Relation->nRelation >= enumTONG_FIGURE_MEMBER)
				{
					ms_pSelf->m_Relation = (TONG_MEMBER_FIGURE)Relation->nRelation;
    				ms_pSelf->UpdateListCheckButton(enumTONG_FIGURE_DIRECTOR);
				}
			}
		}
		else
		{
			int nMasterJoinTm=m_TongData.nMasterJoinTm;
			int nMasterSaveEff=m_TongData.nMasterSaveEff;
			memcpy(&ms_pSelf->m_TongData, pTongInfo, sizeof(KTongInfo));
			m_TongData.nMasterJoinTm=nMasterJoinTm;
			m_TongData.nMasterSaveEff =nMasterSaveEff;
			ms_pSelf->UpdateTongInfoView();
			ms_pSelf->UpdateBtnTheme(ms_pSelf->m_nRelation);
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£º°ÑÐÂµ½µÄÁÐ±íÐÅÏ¢´æµ½ÁÙÊ±ÎÄ¼þ
**********************************************************************/
void KUiTongManager::SaveNewData(KUiGameObjectWithName *ParamInfo, KTongMemberItem *pIncome)
{
	char szSection[32];
	KIniFile Ini;

	if(ParamInfo->nData == enumTONG_FIGURE_DIRECTOR)
	{
		sprintf(szSection, "%s_Director", ParamInfo->szName);
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MANAGER)
	{
		sprintf(szSection, "%s_Manager", ParamInfo->szName);
	}
	else if(ParamInfo->nData == enumTONG_FIGURE_MEMBER)
	{
		sprintf(szSection, "%s_Member", ParamInfo->szName);
	}
	char	szFileName[128];
	sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	if(Ini.Load(szFileName))
	{
		int nCount, nDataCount;
		char szKey[32];

		Ini.GetInteger(szSection, "Count", 0, &nCount);
		nDataCount = nCount + ParamInfo->nParam;
		for(int i = nCount;i < nDataCount;i++)
		{
			sprintf(szKey, "%d_Name", i);
			Ini.WriteString(szSection, szKey, pIncome[i - nCount].Name);

			sprintf(szKey, "%d_AgName", i);
			Ini.WriteString(szSection, szKey, pIncome[i - nCount].szAgname);
			
			sprintf(szKey, "%d_JoinTm", i);
			Ini.WriteInteger(szSection, szKey, pIncome[i - nCount].nJoinTm);
			
			sprintf(szKey, "%d_Hash", i);
			Ini.WriteInteger(szSection, szKey, StringToHash(pIncome[i - nCount].Name));
		}
		Ini.Save(szFileName);
	}
}


/*********************************************************************
* ¹¦ÄÜ£º°ÑÖ¸¶¨ÀàÐÍµÄÁÐ±íÊý¾ÝÕû¸ö´æ´¢ÆðÀ´
**********************************************************************/
void KUiTongManager::SaveWholeData(int nType)
{
	if(ms_pSelf && ms_pSelf->m_TongData.szName[0])
	{
    	KLinkStruct<KPilgarlicItem> *pData = NULL;
		char szSection[32];
		switch(nType)
		{
		case enumTONG_FIGURE_DIRECTOR:
			pData = &ms_pSelf->m_ElderData;
			sprintf(szSection, "%s_Director", ms_pSelf->m_TongData.szName);
			break;

		case enumTONG_FIGURE_MANAGER:
			pData = &ms_pSelf->m_CaptainData;
			sprintf(szSection, "%s_Manager", ms_pSelf->m_TongData.szName);
			break;

		case enumTONG_FIGURE_MEMBER:
			pData = &ms_pSelf->m_MemberData;
			sprintf(szSection, "%s_Member", ms_pSelf->m_TongData.szName);
			break;
		}
		if(pData)
		{
			KPilgarlicItem *pNode = pData->Reset();
			int nCount = pData->GetCount();
			char szKey[32];
			KIniFile Ini;
		/*----------------------------------*/
			char	szFileName[128];
			sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
			Ini.Load(szFileName);
			Ini.EraseSection(szSection);
			for(int i = 0;i < nCount;i++)
			{
				sprintf(szKey, "%d_Name", i);
			    Ini.WriteString(szSection, szKey, pNode->Name);

			    sprintf(szKey, "%d_AgName", i);
			    Ini.WriteString(szSection, szKey, pNode->szAgname);

			    sprintf(szKey, "%d_Hash", i);
			    Ini.WriteInteger(szSection, szKey, pNode->nHashID);

				pNode = pData->NextItem();
			}
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÔØÈë×ÊÁÏ
**********************************************************************/
void KUiTongManager::LoadData(int nType)
{
	if(m_TongData.szName[0])
	{
    	char szSection[32];
	    switch(nType)
	    {
	    case enumTONG_FIGURE_DIRECTOR:
    		sprintf(szSection, "%s_Director", m_TongData.szName);
		    if(!(m_nElderDataIndex = LoadDataHandler(&m_ElderData, szSection)) &&
				m_nElderDataIndex < m_TongData.nDirectorCount)
			{
				m_nElderDataIndex = 0;
    			RequestData(enumTONG_FIGURE_DIRECTOR, m_nElderDataIndex);
			}
		    break;

	    case enumTONG_FIGURE_MANAGER:
    		sprintf(szSection, "%s_Manager", m_TongData.szName);
		    if(!(m_nCaptainDataIndex = LoadDataHandler(&m_CaptainData, szSection)))
			{
				m_nCaptainDataIndex = 0;
    			RequestData(enumTONG_FIGURE_MANAGER, m_nCaptainDataIndex);
			}
		    break;

	    case enumTONG_FIGURE_MEMBER:
    		sprintf(szSection, "%s_Member", m_TongData.szName);
		    if(!(m_nMemberDataIndex = LoadDataHandler(&m_MemberData, szSection)))
			{
				m_nMemberDataIndex = 0;
    			RequestData(enumTONG_FIGURE_MEMBER, m_nMemberDataIndex);
			}
		    break;

		default:
			break;
	    }
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÔØÈë×ÊÁÏµÄ´¦Àíº¯Êý
**********************************************************************/
int KUiTongManager::LoadDataHandler(KLinkStruct<KPilgarlicItem> *pData,
									const char* szSection)
{
	char szFileName[256];
	KIniFile Ini;
	sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	if(Ini.Load(szFileName) && m_TongData.szName[0] && Ini.IsSectionExist(szSection))
	{
		int nCount, i;
		char szKey[32];
		KPilgarlicItem AddItem;

		pData->Clear();
		Ini.GetInteger(szSection, "Count", 0, &nCount);

		for(i = 0;i < nCount;i++)
		{
			sprintf(szKey, "%d_Name", i);
			Ini.GetString(szSection, szKey, "", AddItem.Name, sizeof(AddItem.Name));

			sprintf(szKey, "%d_AgName", i);
			Ini.GetString(szSection, szKey, "", AddItem.szAgname, sizeof(AddItem.szAgname));

			sprintf(szKey, "%d_JoinTm", i);
			Ini.GetInteger(szSection, szKey, 0, &AddItem.nJoinTm);
			
			sprintf(szKey, "%d_Hash", i);
			Ini.GetInteger(szSection, szKey, 0, &AddItem.nHashID);

			pData->AddItem(AddItem);
		}
		UpdateTongListView();
		return nCount;
	}
	return 0;
}


/*********************************************************************
* ¹¦ÄÜ£ºÏòCore·¢³öÊý¾ÝÇëÇó
**********************************************************************/
void KUiTongManager::RequestData(int nType, int nIndex)
{
	if(g_pCoreShell)
	{
    	KUiGameObjectWithName ParamInfo;
    	strcpy(ParamInfo.szName, m_TongData.szName);
    	ParamInfo.nData = nType;
    	ParamInfo.nParam = nIndex;
    	g_pCoreShell->TongOperation(GTOI_REQUEST_TONG_DATA, (unsigned int)&ParamInfo, 0);
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÉ¾³ýÁÙÊ±ÎÄ¼þ
**********************************************************************/
int KUiTongManager::RemoveTempFile()
{
	char szPath[256], szFile[128];

	sprintf(szFile, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	g_GetFullPath(szPath, szFile);
	return remove(szPath);
}


/*********************************************************************
* ¹¦ÄÜ£º¸÷ÖÖ°ï»á²Ù×÷µÄ´¦Àíº¯Êý
**********************************************************************/
void KUiTongManager::OnAction(int nType)
{
	if (g_pCoreShell)
	{
		KPilgarlicItem *pNode = GetSelectedPlayer();
		KTongOperationParam Param;
		KTongMemberItem Player;

		memset(&Player, 0, sizeof(KTongMemberItem));
		Param.eOper = (TONG_ACTION_TYPE)nType;
		if (nType == TONG_ACTION_LEAVE)
		{
			Param.nData[0] = m_nRelation;
			Param.nData[1] = -1;
		}
		else
		{
			Param.nData[0] = m_nCurrentCheckBox;
			Param.nData[1] = m_nIndex;
		}
		strcpy(Param.szData, m_szData);

		if(m_szTargetPlayerName[0])
		{
			strcpy(Player.Name, m_szTargetPlayerName);
		}
	    else if(pNode)
	    {
		    strcpy(Player.Name, pNode->Name);
			Player.nIndex = pNode->nIndex;
	    }
		else
		{
			strcpy(Player.Name, m_szPlayerName);
			Player.nIndex = m_nIndex;
		}
		g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&Param, (int)&Player);
		memset(m_szData, 0, sizeof(m_szData));
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦Ë¢ÐÂ°´Å¥µÄ²Ù×÷º¯Êý
**********************************************************************/
void KUiTongManager::OnRefresh()
{
	if(m_TongData.szName[0])
	{
		int nType;
		KIniFile Ini;
		char szSection[32];

		if(m_nCurrentCheckBox == enumTONG_FIGURE_DIRECTOR)
		{
			sprintf(szSection, "%s_Director", m_TongData.szName);
			m_ElderData.Clear();
			m_nElderDataIndex = 0;
		}
		else if(m_nCurrentCheckBox == enumTONG_FIGURE_MANAGER)
		{
			sprintf(szSection, "%s_Manager", m_TongData.szName);
			m_CaptainData.Clear();
			m_nCaptainDataIndex = 0;
		}
		else if(m_nCurrentCheckBox == enumTONG_FIGURE_MEMBER)
		{
			sprintf(szSection, "%s_Member", m_TongData.szName);
			m_MemberData.Clear();
			m_nMemberDataIndex = 0;
		}
		else
		{
			return;
		}
		nType = m_nCurrentCheckBox;
		
		char	szFileName[128];
		sprintf(szFileName, "%s\\%s", g_UiBase.GetUserTempDataFolder(), TONG_DATA_TEMP_FILE);
	    if(Ini.Load(szFileName))
	    {
    		Ini.EraseSection(szSection);
			Ini.Save(szFileName);
	    }
		m_List.ResetContent();
		LoadData(nType);
	}
}


/*********************************************************************
* ¹¦ÄÜ£º¶Ô½á¹ûµÄÏìÓ¦Èë¿Ú
**********************************************************************/
void KUiTongManager::ResponseResult(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(ms_pSelf)
	{
    	switch(pResult->nData)
    	{
    	case TONG_ACTION_DISMISS:
			ms_pSelf->ResponseDismiss(pResult, nbIsSucceed);
		    break;

	    case TONG_ACTION_DEMISE:
			ms_pSelf->ResponseDemise(pResult, nbIsSucceed);
    		break;

    	case TONG_ACTION_LEAVE:
			ms_pSelf->ResponseLeave(pResult, nbIsSucceed);
		    break;

		case TONG_ACTION_ASSIGN:
			ms_pSelf->ResponseAssign(pResult, nbIsSucceed);
		    break;

		case TONG_ACTION_APPLY:
			ms_pSelf->ResponseApply(pResult, nbIsSucceed);
			break;
	    }
	}
	else
	{
		RemoveTempFile();
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦Àë°ïºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseLeave(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
    	RemoveTempFile();
    	CloseWindow();
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦ÌßÈËºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseDismiss(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(strcmp(pResult->szName, m_szPlayerName) == 0)
		{
			RemoveTempFile();
			CloseWindow();
		}
		else
		{
			KLinkStruct<KPilgarlicItem> *pForDelete;
			pForDelete = FindPlayer(pResult->szName);
			if(pForDelete)
			{
				pForDelete->RemoveItem();
				SaveWholeData(pResult->nParam);
				m_List.ResetContent();
				UpdateTongListView();
			}
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦´«Î»ºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseDemise(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		RemoveTempFile();
		ArrangeComposition(NULL);
	}
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦´«Î»ºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseAssign(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(!strcmp(pResult->szName, m_szPlayerName))
		{
			RemoveTempFile();
		    ArrangeComposition(NULL);
		}
		else
		{
			KLinkStruct<KPilgarlicItem> *pPlayer = FindPlayer(pResult->szName);
			if(pPlayer)
			{
				KPilgarlicItem Add, *pNode = pPlayer->Current();
				memcpy(&Add, pNode, sizeof(KPilgarlicItem));
				pPlayer->RemoveItem();
				switch(pResult->nParam)
				{
				case enumTONG_FIGURE_DIRECTOR:
					m_ElderData.AddItem(Add);
					break;

				case enumTONG_FIGURE_MANAGER:
					m_CaptainData.AddItem(Add);
					break;

				case enumTONG_FIGURE_MEMBER:
					m_MemberData.AddItem(Add);
					break;

				default:
					return;
				}
				SaveWholeData(pResult->nParam);
				SaveWholeData(pResult->uParam);
				m_List.ResetContent();
				UpdateTongListView();
			}
			//////////
		}
		/////////
	}
	////////
}


/*********************************************************************
* ¹¦ÄÜ£ºÏìÓ¦ÉêÇëºóµÄ²Ù×÷
**********************************************************************/
void KUiTongManager::ResponseApply(KUiGameObjectWithName *pResult, int nbIsSucceed)
{
	if(nbIsSucceed)
	{
		if(!strcmp(m_szPlayerName, pResult->szName))
		{
			RemoveTempFile();
			ArrangeComposition(NULL);
		}
		else
		{
			KPilgarlicItem Add;
			strcpy(Add.Name, pResult->szName);
			strcpy(Add.szAgname, pResult->szString);
			Add.nHashID = StringToHash(Add.Name);
			m_MemberData.AddItem(Add);
			SaveWholeData(enumTONG_FIGURE_MEMBER);
			m_List.ResetContent();
			UpdateTongListView();
		}
	}
}


/*********************************************************************
* ¹¦ÄÜ£º²éÑ¯ÌØ¶¨Ãû×ÖµÄÍæ¼ÒµÄÊý¾ÝÎ»ÖÃ
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::FindPlayer(char *pszName)
{
	KLinkStruct<KPilgarlicItem> *pData = NULL;
	if(pszName && pszName[0])
	{
    	pData = HandleFind(&m_MemberData, pszName);
    	if(!pData)
    	{
		    pData = HandleFind(&m_CaptainData, pszName);
		    if(!pData)
		    {
    			pData = HandleFind(&m_ElderData, pszName);
		    }
	    }
	}
	return pData;
}


/*********************************************************************
* ¹¦ÄÜ£º²éÕÒÍæ¼Ò´¦Àíº¯Êý
**********************************************************************/
KLinkStruct<KPilgarlicItem>* KUiTongManager::HandleFind(KLinkStruct<KPilgarlicItem> *pData, char *pszName)
{
	KLinkStruct<KPilgarlicItem> *pReturn = NULL;
	KPilgarlicItem *pNode = pData->Reset();
	int nID = StringToHash(pszName);
	int nCount = pData->GetCount();

	for(int i = 0;i < nCount;i++)
	{
		if(nID == pNode->nHashID)
		{
			if(strcmp(pszName, pNode->Name) == 0)
			{
				pReturn = pData;
				break;
			}
		}
		pNode = pData->NextItem();
	}
	return pReturn;
}

void KUiTongManager::OnMoney( int ntype ,int nMoney, int nSelect)
{	
	if (g_pCoreShell)
	{
		KUiPlayerItem Param;
		strcpy(Param.Name,m_szPlayerName);
		Param.nIndex = nSelect;
		Param.nData = nMoney;
		Param.uId = 0;
		g_pCoreShell->TongOperation(GTOI_TONG_MONEY_ACTION, (unsigned int)&Param, ntype);
	}
}

unsigned long StringToHash(const char *pString, BOOL bIsCaseSensitive)
{
	if(pString && pString[0])
	{
		unsigned long id = 0;
		const char *ptr;
		int index = 0;

		if(bIsCaseSensitive)
		{
			ptr = pString;

			while(*ptr)
			{
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		else
		{
			char Buff[256];
			strcpy(Buff, pString);
			strlwr(Buff);
			ptr = Buff;

        	while(*ptr)
		    {
    			id = (id + (++index) * (*ptr)) % 0x8000000b * 0xffffffef;
		        ptr++;
	        }
		}
		return (id ^ 0x12345678);
	}

	return 0;
}
