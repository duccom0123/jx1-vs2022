/*******************************************************************************
File        : UiTongAssignBox.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 任命帮会职位的界面
********************************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"

#include "../elem/wnds.h"
#include "../elem/wndmessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "../../../Engine/src/KFilePath.h"
#include "../../Core/Src/GameDataDef.h"

#include "UiTongAssignBox.h"
#include "UiInformation.h"
#include "UiTongGetString.h"


#define TONG_ASSIGN_BOX_INI "UiTongAssignBox.ini"


extern iCoreShell *g_pCoreShell;

KUiTongAssignBox* KUiTongAssignBox::ms_pSelf = NULL;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongAssignBox::KUiTongAssignBox()
{
	m_pMain = NULL;
	m_nSelectFigure = -1;
	m_szTargetPlayerName[0] = 0;
	m_szTargetPlayerAgName[0] = 0;
}

KUiTongAssignBox::~KUiTongAssignBox()
{
}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiTongAssignBox* KUiTongAssignBox::OpenWindow()
{
	if(g_pCoreShell)
	{
    	if (ms_pSelf == NULL)
    	{
		    ms_pSelf = new KUiTongAssignBox;
		    if (ms_pSelf)
    			ms_pSelf->Initialize();
    	}
    	if (ms_pSelf)
    	{
		    UiSoundPlay(UI_SI_WND_OPENCLOSE);
			ms_pSelf->BringToTop();
			ms_pSelf->Show();
			Wnd_SetExclusive((KWndWindow*)ms_pSelf);
	    }
	}
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiTongAssignBox* KUiTongAssignBox::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiTongAssignBox::CloseWindow(bool bDestory)
{
	if (ms_pSelf)
	{
		Wnd_ReleaseExclusive((KWndWindow*)ms_pSelf);
		ms_pSelf->Hide();
		if (bDestory)
		{
			ms_pSelf->Destroy();
			ms_pSelf = NULL;
		}
	}
}


/*********************************************************************
* 功能：初始化
**********************************************************************/
void KUiTongAssignBox::Initialize()
{
	AddChild(&m_BtnPilgarlic);
	AddChild(&m_TargetName);
	AddChild(&m_BtnCaptain);
	AddChild(&m_BtnConfirm);
	AddChild(&m_BtnCancel);
	AddChild(&m_BtnElder);
	AddChild(&m_ErrorBox);
	AddChild(&m_ErrorBox2);
	AddChild(&m_TitlePlayerName);
	AddChild(&m_TitlePositionName);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiTongAssignBox::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, TONG_ASSIGN_BOX_INI);

		if(Ini.Load(Buff))
		{
			ms_pSelf->Init(&Ini, "Main");

			ms_pSelf->m_BtnPilgarlic.Init(&Ini, "BtnMember");
			ms_pSelf->m_BtnCaptain.Init(&Ini, "BtnManager");
			ms_pSelf->m_BtnConfirm.Init(&Ini, "BtnConfirm");
			ms_pSelf->m_BtnElder.Init(&Ini, "BtnDirector");
			ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");
			ms_pSelf->m_TargetName.Init(&Ini, "PlayerName");
			ms_pSelf->m_ErrorBox.Init(&Ini, "ErrorBox");
			ms_pSelf->m_ErrorBox2.Init(&Ini, "ErrorBox2");
			ms_pSelf->m_TitlePlayerName.Init(&Ini, "TitlePlayerName");
			ms_pSelf->m_TitlePositionName.Init(&Ini, "TitlePositionName");

			Ini.GetString("Main", "AssignString", "", ms_pSelf->m_szAssign, sizeof(ms_pSelf->m_szAssign));
			Ini.GetString("Main", "CancelString", "", ms_pSelf->m_szCancel, sizeof(ms_pSelf->m_szCancel));
			Ini.GetString("Main", "ConfirmString", "", ms_pSelf->m_szConfirm, sizeof(ms_pSelf->m_szConfirm));
			
			Ini.GetString("Main", "MemberString", "", ms_pSelf->m_szMemberString, sizeof(ms_pSelf->m_szMemberString));
			Ini.GetString("Main", "DirectorString", "", ms_pSelf->m_szDirectorString, sizeof(ms_pSelf->m_szDirectorString));
			Ini.GetString("Main", "ManagerString", "", ms_pSelf->m_szManagerString, sizeof(ms_pSelf->m_szManagerString));
			Ini.GetString("Main", "AssignOld", "", ms_pSelf->m_szAssignOld, sizeof(ms_pSelf->m_szAssignOld));
			Ini.GetString("Main", "AssignString", "", ms_pSelf->m_szAssignString, sizeof(ms_pSelf->m_szAssignString));
			Ini.GetString("ErrorBox", "WithoutFaction", "", ms_pSelf->m_szWithoutFaction, sizeof(ms_pSelf->m_szWithoutFaction));

			ms_pSelf->m_BtnPilgarlic.SetText(ms_pSelf->m_szMemberString);
			ms_pSelf->m_BtnElder.SetText(ms_pSelf->m_szDirectorString);
			ms_pSelf->m_BtnCaptain.SetText(ms_pSelf->m_szManagerString);
		}
	}
}


/*********************************************************************
* 功能：窗口函数
**********************************************************************/
int KUiTongAssignBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnConfirm)
		{
			OnConfirm();
		}
		else if(uParam == (unsigned int)&m_BtnCancel)
		{
			if(m_pMain)
			{
    			m_pMain->Show();
	    		m_pMain = NULL;
			}
			CloseWindow();
		}
		else if(uParam == (unsigned int)&m_BtnElder)
		{
			UpdateCheckButton(enumTONG_FIGURE_DIRECTOR);
		}
		else if(uParam == (unsigned int)&m_BtnCaptain)
		{
			UpdateCheckButton(enumTONG_FIGURE_MANAGER);
		}
		else if(uParam == (unsigned int)&m_BtnPilgarlic)
		{
			UpdateCheckButton(enumTONG_FIGURE_MEMBER);
		}
		break;

	case WND_M_OTHER_WORK_RESULT:
		if (uParam == RESULT_T_ASSIGN)
		{
			if(nParam && g_pCoreShell)
			{
				KTongOperationParam Param;
    			KTongMemberItem Member;

			    Param.eOper = TONG_ACTION_ASSIGN;
			    Param.nData[0] = m_nCurrentFigure;
			    Param.nData[2] = m_nSelectFigure;
			    Param.nData[1] = 0;//m_nCurrentPostion;
			    Param.nData[3] = 0;

			    memset(&Member, 0, sizeof(KTongMemberItem));
			    strcpy(Member.Name, (char *)nParam);

 			    g_pCoreShell->TongOperation(GTOI_TONG_ACTION, (unsigned int)&Param, (int)&Member);
			}
			if(m_pMain)
			{
    			m_pMain->Show();
	    		m_pMain = NULL;
			}
			CloseWindow();
		}
		break;

	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}

	return 1;
}


/*********************************************************************
* 功能：建立这个界面和主界面的关联
**********************************************************************/
void KUiTongAssignBox::LinkToMainWindow(KWndWindow *pMain)
{
	if(pMain && ms_pSelf)
	{
		ms_pSelf->m_pMain = pMain;
		//ms_pSelf->m_pMain->Hide();
	}
}


/*********************************************************************
* 功能：为任命案的书写安排数据
**********************************************************************/
void KUiTongAssignBox::ArrangeData(int nType, char* szTarName, char* szTarAgName, int nCurrentFigure, int nCurrentPosition)
{
	if((nType == enumTONG_FIGURE_DIRECTOR || nType == enumTONG_FIGURE_MASTER) &&
		szTarName && szTarName[0] && nCurrentFigure >= enumTONG_FIGURE_MEMBER &&
		nCurrentFigure <= enumTONG_FIGURE_DIRECTOR && ms_pSelf)
	{
		ms_pSelf->m_nType = nType;
		strcpy(ms_pSelf->m_szTargetPlayerName, szTarName);
		ms_pSelf->m_TargetName.SetText(szTarName);

		if(szTarAgName && szTarAgName[0])
		{
			strcpy(ms_pSelf->m_szTargetPlayerAgName, szTarAgName);
		}
		ms_pSelf->m_nCurrentFigure = nCurrentFigure;
		ms_pSelf->m_nCurrentPostion = nCurrentPosition;

		if(nType == enumTONG_FIGURE_DIRECTOR)
		{
			ms_pSelf->m_BtnElder.Enable(false);
			ms_pSelf->m_BtnCaptain.Enable(true);
			ms_pSelf->m_BtnPilgarlic.Enable(true);
		}
		else if(nType == enumTONG_FIGURE_MASTER)
		{
			ms_pSelf->m_BtnElder.Enable(true);
			ms_pSelf->m_BtnCaptain.Enable(true);
			ms_pSelf->m_BtnPilgarlic.Enable(true);
		}
		char szTemp[256];
		if(nCurrentFigure == enumTONG_FIGURE_DIRECTOR)
		{
			sprintf(szTemp, "%s %s",ms_pSelf->m_szAssignOld, ms_pSelf->m_szDirectorString);
			ms_pSelf->m_ErrorBox.SetText(szTemp);
			ms_pSelf->m_BtnElder.Enable(false);
		}
		else if(nCurrentFigure == enumTONG_FIGURE_MANAGER)
		{
			sprintf(szTemp, "%s %s",ms_pSelf->m_szAssignOld, ms_pSelf->m_szManagerString);
			ms_pSelf->m_ErrorBox.SetText(szTemp);
			ms_pSelf->m_BtnCaptain.Enable(false);
		}
		else if(nCurrentFigure == enumTONG_FIGURE_MEMBER)
		{
			sprintf(szTemp, "%s %s",ms_pSelf->m_szAssignOld, ms_pSelf->m_szMemberString);
			ms_pSelf->m_ErrorBox.SetText(szTemp);
			ms_pSelf->m_BtnPilgarlic.Enable(false);
		}
	}
}


/*********************************************************************
* 功能：Check管理函数
**********************************************************************/
void KUiTongAssignBox::UpdateCheckButton(int nFigure)
{
	if(nFigure >= enumTONG_FIGURE_MEMBER && nFigure <= enumTONG_FIGURE_DIRECTOR)
	{
		m_BtnElder.CheckButton(FALSE);
		m_BtnCaptain.CheckButton(FALSE);
		m_BtnPilgarlic.CheckButton(FALSE);

		m_nSelectFigure = nFigure;
		m_ErrorBox2.Show();
		switch(nFigure)
		{
		case enumTONG_FIGURE_MEMBER:
		{
			char szTemp[256];
			sprintf(szTemp, "%s %s",m_szAssignString, m_szMemberString);
			m_ErrorBox2.SetText(szTemp);
			m_BtnPilgarlic.CheckButton(TRUE);
		}
			break;

		case enumTONG_FIGURE_MANAGER:
		{
			char szTemp[256];
			sprintf(szTemp, "%s %s",m_szAssignString, m_szManagerString);
			m_ErrorBox2.SetText(szTemp);
			m_BtnCaptain.CheckButton(TRUE);
		}
			break;
		case enumTONG_FIGURE_DIRECTOR:
		{
			char szTemp[256];
			sprintf(szTemp, "%s %s",m_szAssignString, m_szDirectorString);
			m_ErrorBox2.SetText(szTemp);
			m_BtnElder.CheckButton(TRUE);
		}
			break;
		}
		m_BtnConfirm.Enable(TRUE);
	}
	else
	{
		m_BtnConfirm.Enable(FALSE);
	}
}


/*********************************************************************
* 功能：弹出确认窗口
**********************************************************************/
void KUiTongAssignBox::PopupConfirmWindow(const char* pszInfo, unsigned int uHandleID)
{
	this->WndProc(WND_M_OTHER_WORK_RESULT, uHandleID, (int)&m_szTargetPlayerName);
	if (m_pMain)
	{
		m_pMain->Show();
		m_pMain = NULL;
	}
	Hide();
}


/*********************************************************************
* 功能：响应确认操作
**********************************************************************/
void KUiTongAssignBox::OnConfirm()
{
	if(m_nSelectFigure != -1)
	{
		PopupConfirmWindow(m_szAssign, RESULT_T_ASSIGN);
	}
	else
	{
		m_ErrorBox.SetText(m_szWithoutFaction);
		m_ErrorBox2.Hide();
	}
}
