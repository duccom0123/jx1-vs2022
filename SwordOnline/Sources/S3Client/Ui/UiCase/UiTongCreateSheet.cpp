/********************************************************************
File        : UiTongCreateSheet.cpp
Creator     : Fyt(Fan Zhanpeng)
create data : 08-29-2003(mm-dd-yyyy)
Description : 创建帮会的表单
*********************************************************************/

#include "KWin32.h"
#include "KIniFile.h"
#include "CoreShell.h"
#include "GameDataDef.h"

#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"

#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "UiTongCreateSheet.h"


#define TONG_CREATE_SHEET_SCHEME "UiTongCreateSheet.ini"

KUiTongCreateSheet* KUiTongCreateSheet::ms_pSelf = NULL;


extern iCoreShell *g_pCoreShell;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

KUiTongCreateSheet::KUiTongCreateSheet()
{
	m_nSelectSide = -1;
	m_pMain = NULL;
	m_uHandleID = RESULT_T_CREATE;
	m_nCurrentCamp=camp_justice;
}

KUiTongCreateSheet::~KUiTongCreateSheet()
{

}


/*********************************************************************
* 功能：打开窗口
**********************************************************************/
KUiTongCreateSheet* KUiTongCreateSheet::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiTongCreateSheet;
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
	return ms_pSelf;
}


/*********************************************************************
* 功能：如果窗口正被显示，则返回实例指针
**********************************************************************/
KUiTongCreateSheet* KUiTongCreateSheet::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}


/*********************************************************************
* 功能：关闭窗口，同时可以选则是否删除对象实例
**********************************************************************/
void KUiTongCreateSheet::CloseWindow(bool bDestory)
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
void KUiTongCreateSheet::Initialize()
{
	AddChild(&m_ImgTitle);
	AddChild(&m_TxtTongName);
	AddChild(&m_TxtTongCamp);
	AddChild(&m_EditTongName);
	AddChild(&m_BtnOrder);
	AddChild(&m_BtnNatural);
	AddChild(&m_BtnChaos);
	AddChild(&m_BtnDone);
	AddChild(&m_BtnCancel);
	AddChild(&m_ErrorBox);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}


/*********************************************************************
* 功能：载入界面方案
**********************************************************************/
void KUiTongCreateSheet::LoadScheme(const char* pScheme)
{
	if(ms_pSelf)
	{
    	char		Buff[128];
    	KIniFile	Ini;
    	sprintf(Buff, "%s\\%s", pScheme, TONG_CREATE_SHEET_SCHEME);
    	if (Ini.Load(Buff))
    	{
		    ms_pSelf->Init(&Ini, "Main");

		    ms_pSelf->m_ImgTitle.Init(&Ini, "ImgTitle");
			ms_pSelf->m_TxtTongName.Init(&Ini, "TxtTongName");
			ms_pSelf->m_TxtTongCamp.Init(&Ini, "TxtTongCamp");

		    ms_pSelf->m_BtnOrder.Init(&Ini, "BtnOrder");
			ms_pSelf->m_BtnChaos.Init(&Ini, "BtnChaos");
			ms_pSelf->m_BtnNatural.Init(&Ini, "BtnNatural");

			ms_pSelf->m_ErrorBox.Init(&Ini, "ErrorBox");
			ms_pSelf->m_EditTongName.Init(&Ini, "Input");

			ms_pSelf->m_BtnDone.Init(&Ini, "BtnDone");
			ms_pSelf->m_BtnCancel.Init(&Ini, "BtnCancel");

			
			Ini.GetString("ImgTitle", "ImageChangeCamp", "",
				ms_pSelf->m_szChangeCampImage, sizeof(ms_pSelf->m_szChangeCampImage));
			Ini.GetString("ErrorBox", "WithoutName", "",
				ms_pSelf->m_szNameNullString, sizeof(ms_pSelf->m_szNameNullString));
			Ini.GetString("ErrorBox", "WithoutFaction", "",
				ms_pSelf->m_szAlignmentNullString, sizeof(ms_pSelf->m_szAlignmentNullString));
			Ini.GetString("ErrorBox", "ChangeFaction", "",
				ms_pSelf->m_szChangeFactionString, sizeof(ms_pSelf->m_szChangeFactionString));
		}
		/*--------------------*/
	}
	/*--------------------*/
}

/*********************************************************************
* 功能：建立这个界面和主界面的关联
**********************************************************************/
void KUiTongCreateSheet::LinkToMainWindow(KWndWindow *pMain)
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
void KUiTongCreateSheet::ArrangeData(const char* szTongName, int nCurCamp, unsigned int uHandleID)
{
	if(ms_pSelf)
	{
		switch(nCurCamp)
		{
		case camp_justice:
			ms_pSelf->m_BtnOrder.Enable(false);
			break;

		case camp_balance:
			ms_pSelf->m_BtnNatural.Enable(false);
			break;

		case camp_evil:
			ms_pSelf->m_BtnChaos.Enable(false);
			break;
		};

		ms_pSelf->m_uHandleID=uHandleID;
		ms_pSelf->m_ImgTitle.SetImage(ISI_T_SPR, ms_pSelf->m_szChangeCampImage);
		ms_pSelf->m_EditTongName.SetText(szTongName);
		ms_pSelf->m_ErrorBox.SetText(ms_pSelf->m_szChangeFactionString);
		ms_pSelf->m_EditTongName.Enable(false);
	}
}
/*********************************************************************
* 功能：窗口函数
**********************************************************************/
int KUiTongCreateSheet::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)&m_BtnOrder )
		{
			AlignmentButtonCheck(camp_justice);
		}
		else if(uParam == (unsigned int)&m_BtnChaos)
		{
			AlignmentButtonCheck( camp_evil  );
		}
		else if(uParam == (unsigned int)&m_BtnNatural  )
		{
			AlignmentButtonCheck(camp_balance);
		}
		/*----------------------------------------------------------*/
		else if(uParam == (unsigned int)&m_BtnCancel)
		{
			if(m_pMain)
			{
    			m_pMain->Show();
	    		m_pMain = NULL;
			}
			CloseWindow(true);
		}
		else if(uParam == (unsigned int)&m_BtnDone)
		{
			OnDone();
		}
		break;

	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
		break;
	}
    return 1;
}


/*********************************************************************
* 功能：正中邪CheckBox的管理函数
**********************************************************************/
void KUiTongCreateSheet::AlignmentButtonCheck(int eSide)
{
	m_BtnOrder.CheckButton(FALSE);
	m_BtnChaos.CheckButton(FALSE);
	m_BtnNatural.CheckButton(FALSE);

	switch(eSide)
	{
	case camp_justice:
		m_BtnOrder.CheckButton(TRUE);
		break;

	case camp_evil:
		m_BtnChaos.CheckButton(TRUE);
		break;

	case camp_balance:
		m_BtnNatural.CheckButton(TRUE);
		break;
	}
	m_nSelectSide = eSide;
}


/*********************************************************************
* 功能：响应确认按钮被按下
**********************************************************************/
#include "../ChatFilter.h"
extern CChatFilter g_ChatFilter;

void KUiTongCreateSheet::OnDone()
{
	if(m_uHandleID == RESULT_T_CREATE)
	{

	char szName[16];
	m_EditTongName.GetText(szName, sizeof(szName), TRUE);

	if(szName[0] && g_pCoreShell)
	{
		if (m_nSelectSide != -1)
		{
			if(!g_ChatFilter.IsTextPass(szName))
			{
				m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR01);
				return;
			}
			char szResultMsg[128];

			KTongParam	Param;
			g_pCoreShell->TongOperation(GTOI_TONG_PARAM, (unsigned int)&Param, 0);
			int nRet = g_pCoreShell->TongOperation(GTOI_TONG_CREATE, (unsigned int)szName, (int)m_nSelectSide);
			switch (nRet)
			{
				case enumTONG_CREATE_ERROR_ID1:
					m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR01);
					break;
				case enumTONG_CREATE_ERROR_ID2:
					m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR02);
					break;
				case enumTONG_CREATE_ERROR_ID3:
					m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR03);
					break;
				case enumTONG_CREATE_ERROR_ID4:
					m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR04);
					break;
				case enumTONG_CREATE_ERROR_ID5:
					sprintf(szResultMsg, MSG_TONG_CREATE_ERROR05, Param.m_nLevel);
					m_ErrorBox.SetText(szResultMsg);
					break;
				case enumTONG_CREATE_ERROR_ID6:
					sprintf(szResultMsg, MSG_TONG_CREATE_ERROR06, Param.m_nLeadLevel);
					m_ErrorBox.SetText(szResultMsg);
					break;
				case enumTONG_CREATE_ERROR_ID7:
					sprintf(szResultMsg, MSG_TONG_CREATE_ERROR07, Param.m_nMoney);
					m_ErrorBox.SetText(szResultMsg);
					break;
				case enumTONG_CREATE_ERROR_ID8:
					m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR08);
					break;
				case enumTONG_CREATE_ERROR_ID9:
					m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR09);
					break;
				case enumTONG_CREATE_ERROR_ID10:
					m_ErrorBox.SetText(MSG_TONG_CREATE_ERROR10);
					break;
				case enumTONG_CREATE_ERROR_ID11:
					sprintf(szResultMsg, MSG_TONG_CREATE_ERROR11, Param.m_nRepute);
					m_ErrorBox.SetText(szResultMsg);
					break;
				case enumTONG_CREATE_ERROR_ID12:
					sprintf(szResultMsg, MSG_TONG_CREATE_ERROR12, Param.m_nFuYuan);
					m_ErrorBox.SetText(szResultMsg);
					break;
				default:
					if(m_pMain)
					{
						m_pMain->Show();
						m_pMain = NULL;
					}
					CloseWindow(true);
					break;
			}
		}
		else
		{
			m_ErrorBox.SetText(m_szAlignmentNullString, strlen(m_szAlignmentNullString));
		}
	}
	else
	{
		m_ErrorBox.SetText(m_szNameNullString, strlen(m_szNameNullString));
	}
	}
	else
	{
		if (m_nSelectSide != -1)
		{
		if(m_pMain)
		{
			m_pMain->Show();
			m_pMain->WndProc(WND_M_OTHER_WORK_RESULT, m_uHandleID, (unsigned int)m_nSelectSide);
			m_pMain = NULL;
		}
		CloseWindow(true);
		}
		else
		{
			m_ErrorBox.SetText(m_szAlignmentNullString, strlen(m_szAlignmentNullString));
		}
	}
}