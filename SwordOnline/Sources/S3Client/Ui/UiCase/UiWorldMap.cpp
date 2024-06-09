/* 
 * File:     UiWorldMap.cpp
 * Desc:     世界地图
 * Author:   flying
 * Creation: 2003/7/22
 */
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include <crtdbg.h>
//#include "../ShortcutKey.h"
#include "UiWorldMap.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "Text.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../Engine/src/KDebug.h"

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

KUiWorldmap* KUiWorldmap::m_pSelf = NULL;

#define		SCHEME_INI		"UiWorldMap.ini"

void MapToggleStatus();

KUiWorldmap* KUiWorldmap::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiWorldmap;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->Show();
		m_pSelf->BringToTop();
		Wnd_SetExclusive(m_pSelf);
	}
	return m_pSelf;
}

void KUiWorldmap::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ReleaseExclusive(m_pSelf);
		m_pSelf->Destroy();
		m_pSelf = NULL;
		MapToggleStatus();
	}
}

KUiWorldmap* KUiWorldmap::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

//初始化
void KUiWorldmap::Initialize()
{
	AddChild(&m_Sign);
	AddChild(&m_City);
	AddChild(&m_Capital);
	AddChild(&m_Cave);
	AddChild(&m_Battlefield);
	AddChild(&m_Field);
	AddChild(&m_Others);
	AddChild(&m_Country);
	AddChild(&m_Tong);

	char Buff[128];
	g_UiBase.GetCurSchemePath(Buff, sizeof(Buff));
	strcat(Buff, "\\" SCHEME_INI);
	KIniFile	Ini;
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_Sign.Init(&Ini, "Sign");
		m_City.Init(&Ini, "City");
		m_Capital.Init(&Ini, "Capital");
		m_Cave.Init(&Ini, "Cave");
		m_Battlefield.Init(&Ini, "Battlefield");
		m_Field.Init(&Ini, "Field");
		m_Others.Init(&Ini, "Others");
		m_Country.Init(&Ini, "Country");
		m_Tong.Init(&Ini, "Tong");
	}

	Wnd_AddWindow(this, WL_TOPMOST);
	return;
}

int KUiWorldmap::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_KEYDOWN:
		CloseWindow();
		nResult = true;
		break;
	default:
		nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiWorldmap::UpdateData()
{
	m_Sign.Hide();
	m_City.Hide();
	m_Capital.Hide();
	m_Cave.Hide();
	m_Battlefield.Hide();
	m_Field.Hide();
	m_Others.Hide();
	m_Country.Hide();
	m_Tong.Hide();

	KIniFile Ini;
	if (Ini.Load(MAPLIST_SETTING_FILE))
	{
		//取得世界地图的图形文件明
		char	szBuffer[128];
		char	CityText[64];
		char	CapitalText[64];
		char	CaveText[64];
		char	BattlefieldText[64];
		char	FieldText[64];
		char	OthersText[64];
		char	CountryText[64];
		char	TongText[64];
		char	MapType[64];
		if (Ini.GetString("List", "WorldMapImage", "", szBuffer, sizeof(szBuffer)))
		{
			SetImage(ISI_T_SPR, szBuffer, true);

			int nAreaX = -1, nAreaY = 0;
			KUiSceneTimeInfo Info;
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&Info, 0);
			sprintf(szBuffer, "%d_MapPos", Info.nSceneId);
			Ini.GetInteger2("List", szBuffer, &nAreaX, &nAreaY);
			sprintf(CityText, "City");
			sprintf(CapitalText, "Capital");
			sprintf(CaveText, "Cave");
			sprintf(BattlefieldText, "Battlefield");
			sprintf(FieldText, "Field");
			sprintf(OthersText, "Others");
			sprintf(CountryText, "Country");
			sprintf(TongText, "Tong");
			sprintf(MapType, "%d_MapType", Info.nSceneId);
			Ini.GetString("List", MapType, "", MapType, sizeof(MapType));
			if (nAreaX != -1)
			{
				int nWidth, nHeight;
				/*m_Sign.GetSize(&nWidth,  &nHeight);
				m_Sign.SetPosition(nAreaX - nWidth / 2,nAreaY - nHeight / 2);
				m_Sign.Show();*/
				if (strcmpi(CityText,MapType)==0)
				{
					m_City.GetSize(&nWidth,  &nHeight);
					m_City.SetPosition((nAreaX - nWidth / 2) + 30, (nAreaY - nHeight / 2) + 18);
					m_City.Show();
				}
				else if (strcmpi(CapitalText,MapType)==0)
				{
					m_Capital.GetSize(&nWidth,  &nHeight);
					m_Capital.SetPosition((nAreaX - nWidth / 2) + 30, (nAreaY - nHeight / 2) + 18);
					m_Capital.Show();
				}
				else if (strcmpi(CaveText,MapType)==0)
				{
					m_Cave.GetSize(&nWidth,  &nHeight);
					m_Cave.SetPosition((nAreaX - nWidth / 2) + 30, (nAreaY - nHeight / 2) + 18);
					m_Cave.Show();
				}
				else if (strcmpi(BattlefieldText,MapType)==0)
				{
					m_Battlefield.GetSize(&nWidth,  &nHeight);
					m_Battlefield.SetPosition((nAreaX - nWidth / 2) + 30, (nAreaY - nHeight / 2) + 18);
					m_Battlefield.Show();
				}
				else if (strcmpi(FieldText,MapType)==0)
				{
					m_Field.GetSize(&nWidth,  &nHeight);
					m_Field.SetPosition((nAreaX - nWidth / 2) + 30, (nAreaY - nHeight / 2) + 18);
					m_Field.Show();
				}
				else if (strcmpi(OthersText,MapType)==0)
				{
					m_Others.GetSize(&nWidth,  &nHeight);
					m_Others.SetPosition((nAreaX - nWidth / 2) + 30, (nAreaY - nHeight / 2) + 18);
					m_Others.Show();
				}
				else if (strcmpi(CountryText,MapType)==0)
				{
					m_Country.GetSize(&nWidth,  &nHeight);
					m_Country.SetPosition(nAreaX - nWidth / 2 + 30, nAreaY - nHeight / 2 + 18);
					m_Country.Show();
				}
				else if (strcmpi(TongText,MapType)==0)
				{
					m_Tong.GetSize(&nWidth,  &nHeight);
					m_Tong.SetPosition((nAreaX - nWidth / 2) + 30, (nAreaY - nHeight / 2) + 18);
					m_Tong.Show();
				}
			}
		}
	}
}

void KUiWorldmap::PaintWindow()
{
	KWndImage::PaintWindow();
	
	if (m_City.IsVisible())
		m_City.NextFrame();
	else if (m_Capital.IsVisible())
		m_Capital.NextFrame();
	else if (m_Cave.IsVisible())
		m_Cave.NextFrame();
	else if (m_Battlefield.IsVisible())
		m_Battlefield.NextFrame();
	else if (m_Field.IsVisible())
		m_Field.NextFrame();
	else if (m_Others.IsVisible())
		m_Others.NextFrame();
	else if (m_Country.IsVisible())
		m_Country.NextFrame();
	else if (m_Tong.IsVisible())
		m_Tong.NextFrame();
}