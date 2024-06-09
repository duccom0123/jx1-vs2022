#include "KWin32.h"
#include "KIniFile.h"
#include "GameDataDef.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiGive.h"
#include "UiItem.h"
#include "UiStatus.h"
#include "../UiSoundSetting.h"
#include "UiSysMsgCentre.h"
#include "../../../core/src/coreshell.h"
#include "../../../engine/src/Text.h"
#include "../../../engine/src/KDebug.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"UiGive.ini"


KUiGive* KUiGive::m_pSelf = NULL;

KUiGive::KUiGive()
{
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
KUiGive* KUiGive::OpenWindow(const char* szName, char* szInitString)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiGive;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
//	g_DebugLog("mo ne %s",szName);
		if(KUiItem::GetIfVisible() ==FALSE)
			KUiItem::OpenWindow();

		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_LOCKMOVE, TRUE, pos_give);
//        g_DebugLog("mo ne %s");
		Wnd_GameSpaceHandleInput(false);
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData(szName, szInitString);
		m_pSelf->BringToTop();
		m_pSelf->Show();		
	}
	return m_pSelf;
}

void KUiGive::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		Wnd_GameSpaceHandleInput(true);
		if(bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		g_pCoreShell->OperationRequest(GOI_LOCKMOVE, 0, 0);
	}
}

KUiGive*	KUiGive::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiGive::Initialize()
{
	AddChild(&m_Title);
	AddChild(&m_ContentList);
	AddChild(&m_ContentScroll);
	AddChild(&m_Confirm);
	AddChild(&m_Cancel);
	AddChild(&m_ItemBox);
	m_ContentList.SetScrollbar(&m_ContentScroll);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_ItemBox.SetContainerId((int)UOC_ITEM_GIVE);
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void KUiGive::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_Title.Init(&Ini, "Title");
			m_pSelf->m_ContentList.Init(&Ini, "ContentList");
			m_pSelf->m_ContentScroll.Init(&Ini, "ContentScroll");
			m_pSelf->m_Confirm.Init(&Ini, "Assemble");
			m_pSelf->m_Cancel.Init(&Ini, "Close");
			m_pSelf->m_ItemBox.Init(&Ini, "Items");
			m_pSelf->m_ItemBox.EnableTracePutPos(true);
			m_pSelf->m_ItemBox.EnablePickPut(true);	
		}
	}
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
int KUiGive::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Confirm)
		{
			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_GIVE, TRUE, NULL);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Cancel)
		{
			if (g_pCoreShell)
				g_pCoreShell->OperationRequest(GOI_GIVE, FALSE, NULL);
		}
		break;
	case WND_N_RIGHT_CLICK_ITEM:
			OnClickItem((KUiDraggedObject*)uParam);
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_ContentScroll)
			m_ContentList.SetFirstShowLine(nParam);
		break;		
	case WND_N_ITEM_PICKDROP:
			OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	default:
		return KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiGive::UpdateData(const char* szName, char* szInitString)
{
	m_Title.SetText(szName);
	m_ContentList.Clear();
	int nLen = TEncodeText(szInitString, strlen(szInitString));
	m_ContentList.AddOneMessage(szInitString, nLen);

	m_ItemBox.Clear();

	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_GIVE, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_GIVE, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], true);
		free(pObjs);
		pObjs = NULL;
	}
}

void KUiGive::UpdateItem( KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		KUiDraggedObject Obj;
		Obj.uGenre = pItem->Obj.uGenre;
		Obj.uId = pItem->Obj.uId;
		Obj.DataX = pItem->Region.h;
		Obj.DataY = pItem->Region.v;
		Obj.DataW = pItem->Region.Width;
		Obj.DataH = pItem->Region.Height;
		if (bAdd)
		{
			m_ItemBox.AddObject(&Obj, 1);
		}
		else
		{
			m_ItemBox.RemoveObject(&Obj);
		}
//		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		BringToTop();
	}
}

void KUiGive::OnClickItem(KUiDraggedObject* pItem)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_GIVE;
}

void KUiGive::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (!g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM) && 
		!g_UiBase.IsOperationEnable(UIS_O_TRADE_ITEM))
		return;
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;
	
	UISYS_STATUS eStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		if(!pPickPos->pWnd) return;		
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(
			Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_ITEM_GIVE;				
	}
	
	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_ITEM_GIVE;		
	}
	
	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}