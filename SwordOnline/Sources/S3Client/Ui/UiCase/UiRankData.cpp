#include "KWin32.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/MouseHover.h"
#include "UiRankData.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../Engine/src/Text.h"
#include "../../../Engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"
#include <crtdbg.h>

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;


KUiRankData*			KUiRankData::m_pSelf = NULL;

#define		SCHEME_INI			"UiRankData.ini"

KUiRankData::KUiRankData()
{
}

KUiRankData::~KUiRankData()
{
}

KUiRankData* KUiRankData::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiRankData;
		m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

void KUiRankData::CloseWindow(bool bDestory)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestory)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}


KUiRankData* KUiRankData::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

void KUiRankData::Initialize()
{	
	AddChild(&m_BattleWarTxt);
	AddChild(&m_WinCondition);
	AddChild(&m_WinConditionVal);
	AddChild(&m_Proportion);
	AddChild(&m_ProportionVal);
	AddChild(&m_LeftTime);
	AddChild(&m_LeftTimeVal);
	
	AddChild(&m_ReportItem);
	AddChild(&m_ObjNum);
	AddChild(&m_SelfGrade);
	
	for (int i = 0; i < MAX_SELFINFO_ITEMNAME; i ++)
	{
		AddChild(&m_SelfInfoItemName[i]);
		AddChild(&m_SelfInfoObj[i]);
		AddChild(&m_SelfInfoGrade[i]);
	}
	
	AddChild(&m_SelfCurGrade);
	AddChild(&m_SelfCurGradeVal);
	AddChild(&m_SelfCurDeath);
	AddChild(&m_SelfCurDeathVal);
	AddChild(&m_SelfCurMaxKill);
	AddChild(&m_SelfCurMaxKillVal);
	AddChild(&m_OperateRemind);
	
	AddChild(&m_TopTenPlayName);
	AddChild(&m_TopTenGrade);
	AddChild(&m_TopTenKillNpc);
	AddChild(&m_TopTenPkPlayer);
	AddChild(&m_TopTenMaxKill);
	AddChild(&m_TopTenGetItem);
	AddChild(&m_TopTenGetFlag);
	AddChild(&m_TopTenDeath);

	for (int i = 0; i < MISSION_STATNUM; i ++)
	{
		AddChild(&m_TopTenPlayFaction[i]);
		AddChild(&m_TopTenPlayNameTxt[i]);
		AddChild(&m_TopTenGradeVal[i]);
		AddChild(&m_TopTenKillNpcVal[i]);
		AddChild(&m_TopTenPkPlayerVal[i]);
		AddChild(&m_TopTenMaxKillVal[i]);
		AddChild(&m_TopTenGetItemVal[i]);
		AddChild(&m_TopTenGetFlagVal[i]);
		AddChild(&m_TopTenDeathVal[i]);
	}
	
	char szScheme[128];
	g_UiBase.GetCurSchemePath(szScheme, sizeof(szScheme));
	LoadScheme(szScheme);
	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiRankData::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);

	if (Ini.Load(Buff))
	{
		int	nValue;

		Init(&Ini, "Main");
		Ini.GetString("Main", "LineColor", "", Buff, sizeof(Buff));
		m_LineColor = GetColor(Buff);		
		Ini.GetInteger("Main", "Left_Right", 0, &m_nLeft_Right);
		Ini.GetInteger("Main", "TitleFont", 0, &m_nTitleFont);
		Ini.GetInteger("Main", "ValueFont", 0, &m_nValueFont);
		Ini.GetInteger("TopPos", "TopInfoPos", 0, &m_nTopInfoPos);
		Ini.GetInteger("TopPos", "LinePos1", 0, &m_nLinePos1);
		Ini.GetInteger("TopPos", "SelfInfoPos", 0, &m_nSelfInfoPos);		
		Ini.GetInteger("TopPos", "SelfGradePos", 0, &m_nSelfGradePos);
		Ini.GetInteger("TopPos", "LinePos2", 0, &m_nLinePos2);
		Ini.GetInteger("TopPos", "TopTenPos", 0, &m_nTopTenPos);	
		char szKey[6];
		for (int i = 0; i < 2; i ++)
		{
			sprintf(szKey, "%d", i);		
			Ini.GetString("FactionName", szKey, "", m_szFaction[i], sizeof(m_szFaction[i]));
		}

		m_BattleWarTxt.Init(&Ini, "YellowTxt");
		Ini.GetInteger("TopTypeSet", "BattleWarTxt", 0, &nValue);
		m_BattleWarTxt.SetPosition(nValue, m_nTopInfoPos);	
		
		m_WinCondition.Init(&Ini, "YellowTxt");
		m_WinConditionVal.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "WinCondition", "", Buff, sizeof(Buff));
		m_WinCondition.SetText(Buff);
		Ini.GetInteger("TopTypeSet", "WinCondition", 0, &nValue);
		m_WinCondition.SetPosition(nValue, m_nTopInfoPos);
		Ini.GetInteger("TopTypeSet", "WinConditionVal", 0, &nValue);
		m_WinConditionVal.SetPosition(nValue, m_nTopInfoPos);		

		m_Proportion.Init(&Ini, "YellowTxt");
		m_ProportionVal.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "Proportion", "", Buff, sizeof(Buff));
		m_Proportion.SetText(Buff);
		Ini.GetInteger("TopTypeSet", "Proportion", 0, &nValue);
		m_Proportion.SetPosition(nValue, m_nTopInfoPos);	
		Ini.GetInteger("TopTypeSet", "ProportionVal", 0, &nValue);
		m_ProportionVal.SetPosition(nValue, m_nTopInfoPos);	
		
		m_LeftTime.Init(&Ini, "YellowTxt");
		m_LeftTimeVal.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "LeftTime", "", Buff, sizeof(Buff));
		m_LeftTime.SetText(Buff);
		Ini.GetInteger("TopTypeSet", "LeftTime", 0, &nValue);
		m_LeftTime.SetPosition(nValue, m_nTopInfoPos);	
		Ini.GetInteger("TopTypeSet", "LeftTimeVal", 0, &nValue);
		m_LeftTimeVal.SetPosition(nValue, m_nTopInfoPos);			
		
		m_ReportItem.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "ReportItem", "", Buff, sizeof(Buff));
		m_ReportItem.SetText(Buff);
		Ini.GetInteger("SelfTypeSet", "ReportItem", 0, &nValue);
		m_ReportItem.SetPosition(nValue, m_nSelfInfoPos);
		
		for (int i = 0; i < MAX_SELFINFO_ITEMNAME; i ++)
		{
		sprintf(szKey, "%d", i+1);
		m_SelfInfoItemName[i].Init(&Ini, "WhiteTxt");
		Ini.GetString("ItemName", szKey, "", Buff, sizeof(Buff));
		m_SelfInfoItemName[i].SetText(Buff);
		m_SelfInfoItemName[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
		m_SelfInfoItemName[i].SetPosition(nValue, m_nSelfInfoPos - 1 + (m_nTitleFont*(2+i*2)));
		}		
		
		m_ObjNum.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "ObjNum", "", Buff, sizeof(Buff));
		m_ObjNum.SetText(Buff);
		Ini.GetInteger("SelfTypeSet", "ObjNum", 0, &nValue);
		m_ObjNum.SetPosition(nValue, m_nSelfInfoPos);
		for (int i = 0; i < MAX_SELFINFO_ITEMNAME; i ++)
		{
			m_SelfInfoObj[i].Init(&Ini, "WhiteTxt");
			m_SelfInfoObj[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_SelfInfoObj[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_SelfInfoObj[i].SetPosition(nValue, m_nSelfInfoPos - 1 + (m_nTitleFont*(2+i*2)));
		}
		
		m_SelfGrade.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "SelfGrade", "", Buff, sizeof(Buff));
		m_SelfGrade.SetText(Buff);
		Ini.GetInteger("SelfTypeSet", "Grade", 0, &nValue);
		m_SelfGrade.SetPosition(nValue, m_nSelfInfoPos);
		for (int i = 0; i < MAX_SELFINFO_ITEMNAME; i ++)
		{
			m_SelfInfoGrade[i].Init(&Ini, "WhiteTxt");
			m_SelfInfoGrade[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_SelfInfoGrade[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_SelfInfoGrade[i].SetPosition(nValue, m_nSelfInfoPos - 1 + (m_nTitleFont*(2+i*2)));
		}
		
		m_SelfCurGrade.Init(&Ini, "WhiteTxt");
		m_SelfCurGradeVal.Init(&Ini, "WhiteTxt");
		Ini.GetString("ItemName", "SelfCurGrade", "", Buff, sizeof(Buff));
		m_SelfCurGrade.SetText(Buff);
		Ini.GetInteger("SelfGradeSet", "Grade", 0, &nValue);
		m_SelfCurGrade.SetPosition(nValue, m_nSelfGradePos);	
		m_SelfCurGradeVal.SetPosition(nValue+(strlen(Buff)+1)*(m_nTitleFont/2), m_nSelfGradePos);	
		
		m_SelfCurDeath.Init(&Ini, "WhiteTxt");
		m_SelfCurDeathVal.Init(&Ini, "WhiteTxt");
		Ini.GetString("ItemName", "SelfCurDeath", "", Buff, sizeof(Buff));
		m_SelfCurDeath.SetText(Buff);
		Ini.GetInteger("SelfGradeSet", "Death", 0, &nValue);
		m_SelfCurDeath.SetPosition(nValue, m_nSelfGradePos);
		m_SelfCurDeathVal.SetPosition(nValue+(strlen(Buff)+1)*(m_nTitleFont/2), m_nSelfGradePos);
		
		m_SelfCurMaxKill.Init(&Ini, "WhiteTxt");
		m_SelfCurMaxKillVal.Init(&Ini, "WhiteTxt");
		Ini.GetString("ItemName", "SelfCurMaxKill", "", Buff, sizeof(Buff));
		m_SelfCurMaxKill.SetText(Buff);
		Ini.GetInteger("SelfGradeSet", "MaxKill", 0, &nValue);
		m_SelfCurMaxKill.SetPosition(nValue, m_nSelfGradePos);
		m_SelfCurMaxKillVal.SetPosition(nValue+(strlen(Buff)+1)*(m_nTitleFont/2), m_nSelfGradePos);
		
		m_OperateRemind.Init(&Ini, "WhiteTxt");
		Ini.GetString("ItemName", "OperateRemind", "", Buff, sizeof(Buff));
		m_OperateRemind.SetText(Buff);
		m_OperateRemind.SetSize(strlen(Buff)*m_nTitleFont,m_nTitleFont+1);
		Ini.GetInteger("SelfGradeSet", "OperateRemind", 0, &nValue);
		m_OperateRemind.SetPosition(nValue, m_nSelfGradePos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenPlayFaction[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenPlayFaction[i].Init(&Ini, "YellowTxt");
			Ini.GetInteger("TenTypeSet", "PlayerFaction", 0, &nValue);
			m_TopTenPlayFaction[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}					
		
		m_TopTenPlayName.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenPlayName", "", Buff, sizeof(Buff));
		m_TopTenPlayName.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "PlayerName", 0, &nValue);
		m_TopTenPlayName.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenPlayNameTxt[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenPlayNameTxt[i].Init(&Ini, "YellowTxt");
			m_TopTenPlayNameTxt[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenPlayNameTxt[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenPlayNameTxt[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}					
		
		m_TopTenGrade.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenGrade", "", Buff, sizeof(Buff));
		m_TopTenGrade.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "Grade", 0, &nValue);
		m_TopTenGrade.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenGradeVal[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenGradeVal[i].Init(&Ini, "YellowTxt");
			m_TopTenGradeVal[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenGradeVal[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenGradeVal[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}			
		
		m_TopTenKillNpc.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenKillNpc", "", Buff, sizeof(Buff));
		m_TopTenKillNpc.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "KillNpc", 0, &nValue);
		m_TopTenKillNpc.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenKillNpcVal[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenKillNpcVal[i].Init(&Ini, "YellowTxt");
			m_TopTenKillNpcVal[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenKillNpcVal[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenKillNpcVal[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}			
		
		m_TopTenPkPlayer.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenPkPlayer", "", Buff, sizeof(Buff));
		m_TopTenPkPlayer.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "PkPlayer", 0, &nValue);
		m_TopTenPkPlayer.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenPkPlayerVal[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenPkPlayerVal[i].Init(&Ini, "YellowTxt");
			m_TopTenPkPlayerVal[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenPkPlayerVal[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenPkPlayerVal[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}				
		
		m_TopTenMaxKill.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenMaxKill", "", Buff, sizeof(Buff));
		m_TopTenMaxKill.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "MaxKill", 0, &nValue);
		m_TopTenMaxKill.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenMaxKillVal[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenMaxKillVal[i].Init(&Ini, "YellowTxt");
			m_TopTenMaxKillVal[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenMaxKillVal[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenMaxKillVal[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}			
		
		m_TopTenGetItem.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenGetItem", "", Buff, sizeof(Buff));
		m_TopTenGetItem.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "GetItem", 0, &nValue);
		m_TopTenGetItem.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenGetItemVal[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenGetItemVal[i].Init(&Ini, "YellowTxt");
			m_TopTenGetItemVal[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenGetItemVal[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenGetItemVal[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}			
		
		m_TopTenGetFlag.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenGetFlag", "", Buff, sizeof(Buff));
		m_TopTenGetFlag.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "GetFlag", 0, &nValue);
		m_TopTenGetFlag.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenGetFlagVal[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenGetFlagVal[i].Init(&Ini, "YellowTxt");
			m_TopTenGetFlagVal[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenGetFlagVal[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenGetFlagVal[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}		
		
		m_TopTenDeath.Init(&Ini, "YellowTxt");
		Ini.GetString("ItemName", "TopTenDeath", "", Buff, sizeof(Buff));
		m_TopTenDeath.SetText(Buff);
		Ini.GetInteger("TenTypeSet", "Death", 0, &nValue);
		m_TopTenDeath.SetPosition(nValue, m_nTopTenPos);
		
		for (int i = 0; i < MISSION_STATNUM; i ++)
		{
			if(i % m_nLeft_Right)
				m_TopTenDeathVal[i].Init(&Ini, "WhiteTxt");
			else
				m_TopTenDeathVal[i].Init(&Ini, "YellowTxt");
			m_TopTenDeathVal[i].SetHAlign(WNDTEXT_ES_HALIGN_CENTRE);
			m_TopTenDeathVal[i].SetSize(strlen(Buff)*(m_nTitleFont/2),m_nTitleFont+1);
			m_TopTenDeathVal[i].SetPosition(nValue, m_nTopTenPos + (m_nTitleFont + m_nLeft_Right*2)+ (m_nTitleFont + m_nLeft_Right*2)*i);
		}
	}
}

void KUiRankData::Breathe()
{
	TMissionLadderSelfInfo		tmp;
	g_pCoreShell->GetGameData(GDI_MISSION_SELFDATA,
		(unsigned int)(&tmp), NULL);	

	m_BattleWarTxt.SetText(tmp.szMissionName);
	m_WinConditionVal.SetIntText(tmp.nGlbParam[0]);
	m_ProportionVal.SetIntText(tmp.nGlbParam[1]);
	m_LeftTimeVal.SetIntText(tmp.nGlbParam[2]/1080);
	
	m_SelfCurGradeVal.SetIntText(tmp.nParam[2]);
	m_SelfCurDeathVal.SetIntText(tmp.nParam[3]);
	m_SelfCurMaxKillVal.SetIntText(tmp.nParam[4]);

	m_SelfInfoObj[PkPlayer].SetIntText(tmp.nParam[5]);
	m_SelfInfoGrade[PkPlayer].SetIntText(tmp.nParam[6]);

	m_SelfInfoObj[NpcKill].SetIntText(tmp.nParam[7]);
	m_SelfInfoGrade[NpcKill].SetIntText(tmp.nParam[8]);

	m_SelfInfoObj[MaxKill].SetIntText(tmp.nParam[9]);
	m_SelfInfoGrade[MaxKill].SetIntText(tmp.nParam[10]);

	m_SelfInfoObj[GetItem].SetIntText(tmp.nParam[11]);
	m_SelfInfoGrade[GetItem].SetIntText(tmp.nParam[12]);

	m_SelfInfoObj[GetFlag].SetIntText(tmp.nParam[13]);
	m_SelfInfoGrade[GetFlag].SetIntText(tmp.nParam[14]);
	
	for (int i = 0; i < MISSION_STATNUM; i ++)
	{
		TMissionLadderInfo		tmp;
		g_pCoreShell->GetGameData(GDI_MISSION_RANKDATA,
			(unsigned int)(&tmp), i);	
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenPlayFaction[i].SetText(m_szFaction[tmp.ucGroup]):m_TopTenPlayFaction[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenPlayNameTxt[i].SetText(tmp.Name):m_TopTenPlayNameTxt[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenGradeVal[i].SetIntText(tmp.nParam[2]):m_TopTenGradeVal[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenDeathVal[i].SetIntText(tmp.nParam[3]):m_TopTenDeathVal[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenMaxKillVal[i].SetIntText(tmp.nParam[4]):m_TopTenMaxKillVal[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenPkPlayerVal[i].SetIntText(tmp.nParam[5]):m_TopTenPkPlayerVal[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenKillNpcVal[i].SetIntText(tmp.nParam[7]):m_TopTenKillNpcVal[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenGetItemVal[i].SetIntText(tmp.nParam[11]):m_TopTenGetItemVal[i].Clear();
		tmp.nParam[MISSION_PARAM_AVAILABLE]?m_TopTenGetFlagVal[i].SetIntText(tmp.nParam[13]):m_TopTenGetFlagVal[i].Clear();
	}
}

int KUiRankData::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	return 0;
}


void KUiRankData::PaintWindow()
{
	KWndShadow::PaintWindow();
	if (g_pCoreShell)
	{
		KRULine	Line;
		Line.Color.Color_dw = m_LineColor;
		GetAbsolutePos(&Line.oPosition.nX, &Line.oPosition.nY);
		Line.oPosition.nY += m_nLinePos1;
		Line.oEndPos.nZ = Line.oPosition.nZ = 0;
		GetSize(&Line.oEndPos.nX, NULL);
		Line.oEndPos.nX+=Line.oPosition.nX;
		Line.oEndPos.nY = Line.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
		GetAbsolutePos(&Line.oPosition.nX, &Line.oPosition.nY);
		Line.oPosition.nY += m_nLinePos2;
		Line.oEndPos.nY = Line.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
	}
}
