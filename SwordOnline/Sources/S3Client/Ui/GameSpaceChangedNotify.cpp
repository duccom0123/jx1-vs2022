#include "KWin32.h"

#include "Windows.h"
#include "../../core/src/coreshell.h"
#include "../../core/src/GameDataDef.h"
#include "../../core/src/CoreObjGenreDef.h"
#include "Windows.h"
#include "KEngine.h"
#include "Elem/Wnds.h"
#include "Elem/MouseHover.h"
#include "ShortcutKey.h"
#include "UiSoundSetting.h"
#include "UiCase/UiReconnect.h"
#include "UiCase/UiPlayerBar.h"
#include "UiCase/UiStatus.h"
#include "UiCase/UiTrade.h"
#include "UiCase/UiSkills.h"
#include "UiCase/UiItem.h"
#include "UiCase/UiShop.h"
#include "UiCase/UiMsgSel.h"
#include "UiCase/UiMsgCentrePad.h"
#include "UiCase/UiInformation1.h"
#include "UiCase/UiInformation2.h"
#include "UiCase/UiSysMsgCentre.h"
#include "UiCase/UiNewsSysMsg.h"
#include "UiCase/UiTeamManage.h"
#include "UiCase/UiSelPlayerNearby.h"
#include "UiCase/UiChatCentre.h"
#include "UiCase/UiStoreBox.h"
#include "UiCase/UiExpandItem.h"
#include "UiCase/UiTaskNote.h"
#include "UiCase/UiNewsMessage.h"
#include "UiCase/UiNewsMessage2.h"
#include "UiCase/UiNewsSysMsg.h"
#include "UiCase/UiStrengthRank.h"
#include "UiCase/UiTongManager.h"
#include "UiCase/UiTongCreateSheet.h"
#include "UiCase/UiESCDlg.h"
#include "UiCase/UiFaceSelector.h"
#include "UiCase/UiMinimap.h"
#include "UiCase/UiInit.h"
#include "UiCase/UiPlayerShop.h"
#include "UiCase/UiStatus.h"
#include "UiCase/UiGive.h"
#include "UiCase/UiRankData.h"
#include "UiCase/UiAutoPlay.h"
#include "UiCase/UiTeamManage.h"
#include "UiCase/UiGetString.h"
#include "../Login/Login.h"
#include "UiCase/UiParadeItem.h"
#include "UiCase/UiEnchase.h"
#include "KTongProtocol.h"
#include "UiCase/UiSuperShop.h"
#include "UiCase/UiPlayerControlBar.h"
#include "UiCase/UiChatRoom.h"

#include "../S3Client.h"
#include "UiShell.h"

#include "../../Engine/Src/Text.h"

bool UiCloseWndsInGame(bool bAll);

extern iCoreShell*		g_pCoreShell;

void GameWorldTips(unsigned int uParam, int nParam);

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º½ÓÊÜÓÎÏ·ÊÀ½çÊý¾Ý¸Ä±äÍ¨ÖªµÄº¯Êý
//--------------------------------------------------------------------------
void CoreDataChangedCallback(unsigned int uDataId, unsigned int uParam, int nParam)
{
	KUiTrade* pTradeBar = NULL;
	switch(uDataId)
	{
	case GDCNI_HOLD_OBJECT:
		Wnd_DragFinished();
		if (uParam && ((KUiObjAtRegion*)uParam)->Obj.uGenre != CGOG_NOTHING)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = ((KUiObjAtRegion*)uParam)->Obj.uGenre;
			Obj.uId = ((KUiObjAtRegion*)uParam)->Obj.uId;
			Obj.DataW = ((KUiObjAtRegion*)uParam)->Region.Width;
			Obj.DataH = ((KUiObjAtRegion*)uParam)->Region.Height;			
			Wnd_DragBegin(&Obj, DrawDraggingGameObjFunc);
		}
		break;
	case GDCNI_PLAYER_BASE_INFO:	//Ö÷½ÇµÄÒ»Ð©²»Ò×±äµÄÊý¾Ý
		{
			KUiStatus* pBar = KUiStatus::GetIfVisible();
			if (pBar)
				pBar->UpdateBaseData();
			
			KUiPlayerBaseInfo	Info;
			memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
			g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);

			if ((Info.nCurFaction >= 0) || 
				(Info.nCurTong != 0) || 
				(Info.nMissionGroup >= 0) || 
				(Info.nRoomId >= 0)
				)
				KUiMsgCentrePad::QueryAllChannel();
			
			if (Info.nCurFaction < 0)
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Faction);

			if (Info.nCurTong == 0)
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Tong);

			if (Info.nMissionGroup < 0)
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Msgr);

			if (Info.nRoomId < 0)
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Cr);	
		}
		break;
	case GDCNI_PLAYER_RT_ATTRIBUTE:
		{
			KUiStatus* pBar = KUiStatus::GetIfVisible();
			if (pBar)
				pBar->UpdateData();
		}
		break;
	case GDCNI_PLAYER_IMMED_ITEMSKILL://Ö÷½ÇµÄÁ¢¼´Ê¹ÓÃÎïÆ·ÓëÎä¹¦
		if (uParam)
		{
			KUiGameObject* pObj = (KUiGameObject*)uParam;
			KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
			if (pBar)
			{
				if (nParam >= 0)
					pBar->UpdateItem(nParam, pObj->uGenre, pObj->uId);
				else
					pBar->UpdateSkill(-nParam - 1, pObj->uGenre, pObj->uId);
			}
		}
		break;
	case GDCNI_OBJECT_CHANGED:
		if (uParam)
		{
			KUiObjAtContRegion* pObject = (KUiObjAtContRegion*)uParam;
			if (pObject->eContainer == UOC_ITEM_TAKE_WITH)
			{
				pTradeBar = KUiTrade::GetIfVisible();
				if (pTradeBar)
				{
					pTradeBar->OnChangedTakewithItem((KUiObjAtRegion*)uParam, nParam);
				}
				else
				{
					KUiItem* pItemsBar = KUiItem::GetIfVisible();
					if (pItemsBar)
						pItemsBar->UpdateItem((KUiObjAtRegion*)uParam, nParam);
				}
			}
			else if (pObject->eContainer == UOC_ITEM_TAKE_WITH_EX)
			{
				KUiExpandItem* pExpandItem = KUiExpandItem::GetIfVisible();
				if (pExpandItem)
					pExpandItem->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_EQUIPTMENT)
			{
				KUiStatus* pEquips = KUiStatus::GetIfVisible();
				if (pEquips)
					pEquips->UpdateEquip((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem(-1, (KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX1)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem(REPOSITORY_ONE, (KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX2)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem(REPOSITORY_TWO, (KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX3)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem(REPOSITORY_THREE, (KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX4)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem(REPOSITORY_FOUR, (KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_STORE_BOX5)
			{
				KUiStoreBox* pStoreBox = KUiStoreBox::GetIfVisible();
				if (pStoreBox)
					pStoreBox->UpdateItem(REPOSITORY_FIVE, (KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_TO_BE_TRADE)
			{
				KUiTrade* pTrade = KUiTrade::GetIfVisible();
				if (pTrade)
					pTrade->OnSelfChangedItem(pObject, nParam);
			}
			else if (pObject->eContainer == UOC_IN_HAND)
			{
				Wnd_DragFinished();
				if (pObject->Obj.uGenre != CGOG_NOTHING)
				{
					KUiDraggedObject Obj;
					Obj.uGenre = pObject->Obj.uGenre;
					Obj.uId = pObject->Obj.uId;
					Obj.DataW = pObject->Region.Width;
					Obj.DataH = pObject->Region.Height;			
					Wnd_DragBegin(&Obj, DrawDraggingGameObjFunc);
				}
			}
			else if (pObject->eContainer == UOC_IMMEDIA_ITEM)
			{
				KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
				if (pBar)
				{
					if (nParam)
						pBar->UpdateItem(pObject->Region.h, pObject->Obj.uGenre, pObject->Obj.uId);
					else
						pBar->UpdateItem(pObject->Region.h, CGOG_NOTHING, 0);
				}
			}
			else if (pObject->eContainer == UOC_IMMEDIA_SKILL)
			{
				KUiPlayerBar* pBar = KUiPlayerBar::GetIfVisible();
				if (pBar)
				{
					if (nParam)
						pBar->UpdateSkill(pObject->Region.h, pObject->Obj.uGenre, pObject->Obj.uId);
					else
						pBar->UpdateSkill(pObject->Region.h, CGOG_NOTHING, 0);
				}
			}
			else if (pObject->eContainer == UOC_ITEM_GIVE)
			{
				KUiGive* pGive = KUiGive::GetIfVisible();
				if (pGive)
					pGive->UpdateItem((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_COMPOUND)
			{
				KUiEnchase* pEnchase = KUiEnchase::GetIfVisible();
				if (pEnchase)
					pEnchase->UpdateItemObjBox((KUiObjAtRegion*)uParam, nParam);
			}
			else if (pObject->eContainer == UOC_COMPOUND_BOX)
			{
				KUiEnchase* pEnchase = KUiEnchase::GetIfVisible();
				if (pEnchase)
					pEnchase->UpdateItemMatrixBox((KUiObjAtRegion*)uParam, nParam);
			}
		}
		break;
	case GDCNI_FIGHT_SKILL_POINT:	//Ê£ÓàÕ½¶·¼¼ÄÜµãÊý±ä»¯
		{
			KUiSkills* pPad = KUiSkills::GetIfVisible();
			if (pPad)
			{
				pPad->UpdateFightRemainPoint(nParam);
			}
		}
		break;
	case GDCNI_SKILL_CHANGE:			//ÐÂÕÆÎÕÁËÒ»¸ö¼¼ÄÜ
		if (uParam)
		{
			KUiSkills::UpdateSkill((KUiSkillData*)uParam, nParam);
		}
		break;
	case GDCNI_TRADE_START:
		if (uParam)
		{
			UiCloseWndsInGame(false);
			KUiTrade::OpenWindow((KUiPlayerItem*)uParam, (BOOL)nParam);
		}
		break;
	case GDCNI_TRADE_DESIRE_ITEM:	//¶Ô·½Ôö¼õÏë½»Ò×µÄÎïÆ·
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->OnOppositeChangedItem((KUiObjAtRegion*)uParam, nParam);
		break;
	case GDCNI_TRADE_OPER_DATA:		//½»Ò××´Ì¬±ä»¯
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->UpdateOperData((BOOL)uParam, (BOOL)nParam);
		break;
	case GDCNI_TRADE_END:			//½»Ò×½áÊø
		pTradeBar = KUiTrade::GetIfVisible();
		if (pTradeBar)
			pTradeBar->UpdateTradeEnd(nParam);
		break;
	case GDCNI_NPC_TRADE:
		if (nParam)
			KUiShop::OpenWindow();
		else
			KUiShop::CloseWindow();
			
		break;
	case GDCNI_NPC_TRADE_ITEM:
		{
			KUiShop* pShop = KUiShop::GetIfVisible();
			if (pShop)
				pShop->UpdateData();
		}
		break;
	case GDCNI_QUESTION_CHOOSE:
        { 
        if (nParam) 
			KUiMsgSel::OpenWindow((KUiQuestionAndAnswer*)uParam, (KUiImage*)nParam); 
        else
			KUiMsgSel::OpenWindow((KUiQuestionAndAnswer*)uParam); 
        }
		break;  
	case GDCNI_GAME_START:
		{
			g_LoginLogic.NotifyToStartGame();
			Wnd_GameSpaceHandleInput(true);
			KUiMsgCentrePad::ReleaseActivateChannelAll();
			KUiMsgCentrePad::QueryAllChannel();
		}
		break;
	case GDCNI_SPEAK_WORDS:			//npcËµ»°ÄÚÈÝ
	//uParam = (KUiInformationParam*) pWordDataList Ö¸ÏòKUiInformationParamÊý×é
	//nParam = pWordDataList°üº¬KUiInformationParamÔªËØµÄÊýÄ¿
		if (uParam && nParam)
			g_UiInformation2.SpeakWords((KUiInformationParam*)uParam, nParam);
		break;
	case GDCNI_INFORMATION:
		if (uParam && nParam)
			g_UiInformation1.SpeakWords((KUiInformationParam*)uParam, LOWORD(nParam), HIWORD(nParam));
		break;
	case GDCNI_CHAT_GROUP:
		KUiChatCentre::UpdateData(UICC_U_ALL, 0, 0);
		break;
	case GDCNI_CHAT_FRIEND:		//ÁÄÌìºÃÓÑ·¢Éú±ä»¯
		KUiChatCentre::UpdateData(UICC_U_GROUP, 0, nParam);
		break;
	case GDCNI_CHAT_FRIEND_STATUS:	//ÁÄÌìºÃÓÑ×´Ì¬·¢Éú±ä»¯
		KUiChatCentre::UpdateData(UICC_U_FRIEND, uParam, nParam);
		break;
	case GDCNI_TEAM:
		{
			KUiTeamManage* pPad = KUiTeamManage::GetIfVisible();
			if (pPad)
				pPad->UpdateData((KUiPlayerTeam*)uParam);
			if (uParam)
				KUiMsgCentrePad::QueryAllChannel();
			else
				KUiMsgCentrePad::CloseSelfChannel(KUiMsgCentrePad::ch_Team);
		}
		break;
	case GDCNI_SWITCH_CURSOR:
		Wnd_SwitchCursor(nParam);
		break;
	case GDCNI_SYSTEM_MESSAGE:
		if (uParam)
		{
			KSystemMessage* pMsg = (KSystemMessage*)uParam;
			KUiSysMsgCentre::AMessageArrival((KSystemMessage*)uParam, (void*)nParam);
		}
		break;
	case GDCNI_OPEN_STORE_BOX:
		KUiStoreBox::OpenWindow((int)uParam);
		break;
	case GDCNI_OPEN_EQUIPEX_BOX:
		KUiExpandItem::OpenWindow();
		break;
	case GDCNI_SWITCHING_SCENEPLACE:
		break;
	case GDCNI_SWITCHING_MAPMODE:
		if (uParam)
			MapSetMode(MINIMAP_M_BRIEF_PIC);
		else
			MapSetMode(MINIMAP_M_BRIEF_NOT_PIC);
		break;
	// flying add this branch to deal with the task record from server.
	case GDCNI_MISSION_RECORD:
		if (uParam)
		{
			KMissionRecord* pRecord = (KMissionRecord*)uParam;
			KUiTaskNote::WakeUp((KMissionRecord*)uParam);
		}
		break;
	case GDCNI_MISSION_NOTE:
		if (uParam)
		{
			KUiMissionNote::OpenWindow((KMissionRecord*)uParam);
		}
		break;
	case GDCNI_EXIT_GAME:
		if (g_pCoreShell)
		{
			g_pCoreShell->OperationRequest(GOI_EXIT_GAME, 0, 0);
		}

		//g_LoginLogic.ReturnToIdle();
		UiEndGame();
		KUiInit::OpenWindow(true, false);
		break;
	case GDCNI_VIEW_PLAYERITEM:
		KUiParadeItem::OpenWindow((KUiPlayerItem*)uParam);
		break;
	case GDCNI_PLAYER_BRIEF_PROP:
		GameWorldTips(uParam, nParam);
		break;
	case GDCNI_NEWS_MESSAGE:		//ÐÂÎÅÏûÏ¢
		if (uParam)
			KUiNewsMessage::MessageArrival((KNewsMessage*)uParam, (SYSTEMTIME*)nParam);
		break;
	case GDCNI_NEWS_MESSAGE2:		//ÐÂÎÅÏûÏ¢
		if (uParam)
			KUiNewsMessage2::MessageArrival((KNewsMessage*)uParam, (SYSTEMTIME*)nParam);
		break;		
	case GDCNII_RANK_INDEX_LIST_ARRIVE:
		KUiStrengthRank::OpenWindow();
		KUiStrengthRank::NewIndexArrive(uParam, (KRankIndex *)nParam, true);
		break;
	case GDCNII_RANK_INFORMATION_ARRIVE:
		KUiStrengthRank::NewRankArrive(uParam, (KRankMessage *)nParam);
		break;
	case GDCNI_TONG_INFO:
		KUiTongManager::TongInfoArrive((KUiPlayerRelationWithOther *)uParam, (KTongInfo *)nParam);
		break;
	case GDCNI_TONG_MEMBER_LIST:
		KUiTongManager::NewDataArrive((KUiGameObjectWithName *)uParam, (KTongMemberItem *)nParam);
		break;
	case GDCNI_TONG_ACTION_RESULT:
		KUiTongManager::ResponseResult((KUiGameObjectWithName *)uParam, nParam);
		break;
	case GDCNI_OPEN_TONG_CREATE_SHEET:
			KUiTongCreateSheet::OpenWindow();
		break;
	case GDCNI_ITEMEX:
		KUiExpandItem::OpenWindow();
		break;
	case GDCNI_RANKDATA:		
		KUiRankData::OpenWindow();
		break; 
	case GDCNI_ENCHASE:
		KUiEnchase::OpenWindow();
		break;
	case GDCNI_INPUT:
		KUiGetString::OpenWindow(GSA_INPUT, "NhËp d÷ liÖu", "",
				NULL, 0, (int)uParam,0,32);
		break;
	case GDCNI_GIVE:
		if(uParam && nParam)
			KUiGive::OpenWindow((char*)uParam,(char*)nParam);
		else
		{
			if (KUiGive::GetIfVisible())
				KUiGive::CloseWindow(true);
		}
		break;
	case GDCNI_VIEWTRADE:
		{
			KUiPlayerShop* pShop = KUiPlayerShop::GetIfVisible();
			if (pShop)
				pShop->UpdateItem();
			else
				KUiPlayerShop::OpenWindow((KUiPlayerItem*)uParam);
		}
		break;
	case GDCNI_CANCELTRADE:
		{
			KUiPlayerShop* pShop = KUiPlayerShop::GetIfVisible();
			if (pShop)
				pShop->CloseWindow();
		}
		break;
	case GDCNI_SUPERSHOP:
		{
			if(nParam)
			{
				if (!KUiDynamicShop::GetIfVisible())
					KUiDynamicShop::OpenWindow((BuySellInfo*)uParam);

			}
			else
			{
				if (KUiSuperShop::GetIfVisible())
					KUiSuperShop::UpdateShop((BuySellInfo*)uParam);
				else
					KUiSuperShop::OpenWindow((BuySellInfo*)uParam);
			}
		}
		break;
	case GDCNI_REQUEST_CHATROOM_LIST:
		KUiChatRoom::ServerRequestRoomList((KOneRoom*)uParam);
		break;
	case GDCNI_REQUEST_MEMBER_LIST:
		KUiChatRoom::ServerRequestMemberList((KOneMem*)uParam);
		break;
	case GDCNI_REQUEST_BLACK_LIST:
		KUiChatRoom::ServerRequestBlackList((KOneMem*)uParam);
		break;
	case GDCNI_CHATROOM_UPDATE_INTERFACE:
		if(KUiChatRoom::GetIfVisible())
			KUiChatRoom::UpdateInterface();
		break;
	case GDCNI_CHATROOM_JOIN:
		KUiChatRoom::Join((const char*)uParam, (int)nParam);
		break;
	case GDCNI_CHATROOM_LEAVE:
		KUiChatRoom::Leave();
		break;
	}
}

//////////////////////////////////////////////////////

void KClientCallback::CoreDataChanged(unsigned int uDataId, unsigned int uParam, int nParam)
{
	CoreDataChangedCallback(uDataId, uParam, nParam);
}

typedef std::map<std::string, std::string> BLACKLIST;
BLACKLIST g_BlackListUserNames;

struct BlacklistNotify : public AddinNotify
{
	virtual int	RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends);
	virtual int	MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends);
	virtual int	DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends);
	virtual int	DeleteUnitMember(const char* Unit, const char* Name);
	virtual int	MoveUnitMember(const char* Unit, const char* Name, const char* Group);
	void	SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends);
	void	SendNotifyDeleteFriend(const char* Unit, const char* Name);
};

BlacklistNotify s_LNotify;

void ClearBlackList()
{
	g_BlackListUserNames.clear();
}

void CreateBlackListUnit()
{
	ClearBlackList();
	KUiChatCentre::AddAddinUnit(BLACKLIST_UNITNAME, &s_LNotify);
}

void AddBlackList(const char* strName, const char* strGroup)
{
	if (strName && strName[0] != 0 && strGroup)
	{
		g_BlackListUserNames[strName] = strGroup;

		if (g_BlackListUserNames.size() > 0)
		{
			int nUnit = KUiChatCentre::FindUnitIndex(BLACKLIST_UNITNAME);
			if (nUnit >= 0)
			{
				KUiChatCentre::AddFriendInfo(nUnit, (char*)strName, (char*)strGroup);
			}
		}
	}
}

void RemoveBlackList(char* strName)
{
	if (strName && strName[0] != 0)
	{
		BLACKLIST::iterator i = g_BlackListUserNames.find(strName);
		if (i != g_BlackListUserNames.end())
		{
			g_BlackListUserNames.erase(i);

			int nUnit = KUiChatCentre::FindUnitIndex(BLACKLIST_UNITNAME);
			if (nUnit >= 0)
			{
				KUiChatCentre::DeleteFriend(nUnit, strName, true);
			}
		}
	}
}

bool IsInBlackName(char* strName)
{
	if (strName && strName[0] != 0)
	{
		return g_BlackListUserNames.find(strName) != g_BlackListUserNames.end();
	}

	return false;
}

int	SaveBlackListPrivateSetting(KIniFile* pFile, LPCSTR lpSection, int nStart)
{
	int i = nStart;
	char szKey[10];
	char szLine[256];
	BLACKLIST::iterator iU = g_BlackListUserNames.begin();
	while (iU != g_BlackListUserNames.end())
	{
		sprintf(szKey, "%d", i);
		sprintf(szLine, "AddBlackList(\"%s\", \"%s\")", iU->first.c_str(), iU->second.c_str());
		pFile->WriteString(lpSection, szKey, szLine);
		i++;
		iU++;
	}
	return i;
}

int	BlacklistNotify::RenameUnitGroup(const char* Unit, const char* Name, const char* NewName, const STRINGLIST& friends)
{
	AddinNotify::RenameUnitGroup(Unit, Name, NewName, friends);

	if (NewName && NewName[0])
		SendNotifyGroupFriend(Unit, std::string(NewName), friends);

	return 0;
}

int	BlacklistNotify::MoveUnitGroup(const char* Unit, const char* Name, const char* Name2, const STRINGLIST& friends)
{
	AddinNotify::MoveUnitGroup(Unit, Name, Name2, friends);

	if (Name2 && Name2[0])
		SendNotifyGroupFriend(Unit, std::string(Name2), friends);

	return 0;
}

int	BlacklistNotify::DeleteUnitGroup(const char* Unit, const char* Name, const STRINGLIST& friends)
{
	AddinNotify::DeleteUnitGroup(Unit, Name, friends);

	if (Name && Name[0])
		SendNotifyGroupFriend(Unit, "", friends);

	return 0;
}

int	BlacklistNotify::DeleteUnitMember(const char* Unit, const char* Name)
{
	AddinNotify::DeleteUnitMember(Unit, Name);

	SendNotifyDeleteFriend(Unit, Name);

	return 0;
}

int	BlacklistNotify::MoveUnitMember(const char* Unit, const char* Name, const char* Group)
{
	AddinNotify::MoveUnitMember(Unit, Name, Group);

	if (Name && Name[0])
	{
		STRINGLIST friends;
		friends.push_back(Name);
		SendNotifyGroupFriend(Unit, Group, friends);
	}

	return 0;
}

void BlacklistNotify::SendNotifyGroupFriend(const char* Unit, const std::string& group, const STRINGLIST& friends)
{
	if (strcmp(Unit, BLACKLIST_UNITNAME) == 0)
	{
		STRINGLIST::const_iterator i = friends.begin();
		while (i != friends.end())
		{
			g_BlackListUserNames[(*i)] = group;
			i++;
		}
	}
}

void BlacklistNotify::SendNotifyDeleteFriend(const char* Unit, const char* Name)
{
	if (strcmp(Unit, BLACKLIST_UNITNAME) == 0)
	{
		RemoveBlackList((char*)Name);
	}
}

///////////////////////////////////////////////////////////////////////////////

void KClientCallback::ChannelMessageArrival(DWORD nChannelID, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, const char* pItem, BYTE btSomeFlag, bool bSucc)
{
	if (nMsgLength >= MAX_MESSAGE_LENGTH)
		return;

	if (!bSucc)
	{
		char szInfo[256];
		int n = sprintf(szInfo, "Xin thø lçi! TÇn sè vÉn ch­a më, kh«ng thÓ chuyÓn tin tøc!");
		KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
		return;
	}

	int nIndex = -1;
	
	switch(nChannelID)
	{
	case -1:	//gm alias ID
		{
			//ÕÒµ½ÏµÍ³ÆµµÀ
			int nChannelDataCount = KUiMsgCentrePad::GetChannelCount();
			for (int n = 0; n < nChannelDataCount; n++)
			{
				if (KUiMsgCentrePad::IsChannelType(n, KUiMsgCentrePad::ch_GM))
				{
					nIndex = n;
					break;
				}
			}
			if (nIndex < 0)
				return;

			nChannelID = KUiMsgCentrePad::GetChannelID(nIndex);
			if (nChannelID == -1)
				return;
		}
		break;
	case -2:
		{
			KUiNewsSysMsg *pNewsSys = KUiNewsSysMsg::GetIfVisible();
			if(pNewsSys)
			{
				pNewsSys->MessageArrival(pMsgBuff, nMsgLength);
				return;
			}
		}
		break;
	default:
		{
			nIndex = KUiMsgCentrePad::GetChannelIndex(nChannelID);
			if (nIndex < 0)
				return;

			if (IsInBlackName(szSendName))
				return;
		}
		break;
	}

	KUiMsgCentrePad::NewChannelMessageArrival(nChannelID, szSendName, pMsgBuff, nMsgLength, pItem, btSomeFlag);

	if (KUiMsgCentrePad::GetChannelSubscribe(nIndex) &&
		KUiMsgCentrePad::IsChannelType(nIndex, KUiMsgCentrePad::ch_Screen))
	{
		KUiPlayerItem SelectPlayer;
		int nKind = -1;
		if (g_pCoreShell->FindSpecialNPC(szSendName, &SelectPlayer, nKind) && nKind == kind_player)
		{
			if(pItem)
			{
				char m_pItem[MAX_SENTENCE_LENGTH];
				memset(m_pItem, 0, sizeof(m_pItem));
				memcpy(m_pItem, pItem, sizeof(m_pItem));
				int nIdx = g_pCoreShell->GetGameData(GDI_ITEM_CHAT, true, (int)&m_pItem);
				if(nIdx)
				{
					char szName[82], Buffer[MAX_SENTENCE_LENGTH];
					g_pCoreShell->GetGameData(GDI_ITEM_NAME, (unsigned int)&szName, nIdx);
					int nItemLen = strlen(szName);
					memmove(szName+1, szName, nItemLen);
					szName[0] = '<';
					nItemLen++;
					szName[nItemLen] = '>';
					nItemLen++;
					szName[nItemLen] = '\0';
					int nPos = 0, nOffset =0;
					while(nPos < nMsgLength)
					{
						if(pMsgBuff[nPos] == '<')
						{
							int  i;
							for(i=0; i < nItemLen; i++)
							{
								if(pMsgBuff[nPos+i] != szName[i])
									break;
							}
							if(i == nItemLen)
							{
								Buffer[nOffset] = ' ';
								nOffset++;
								nPos += nItemLen;
								continue;
							}
						}
						Buffer[nOffset] = pMsgBuff[nPos];
						nPos++;
						nOffset++;
					}
					if(nOffset)
						g_pCoreShell->ChatSpecialPlayer(&SelectPlayer, (const char*)Buffer, nOffset);
					g_pCoreShell->GetGameData(GDI_ITEM_CHAT, false, nIdx);
					return;
				}
			}
			g_pCoreShell->ChatSpecialPlayer(&SelectPlayer, pMsgBuff, nMsgLength);
		}
	}
}

void KClientCallback::MSNMessageArrival(char* szSourceName, char* szSendName, const char* pMsgBuff, unsigned short nMsgLength, const char* pItem, bool bSucc)
{
	if (nMsgLength >= MAX_MESSAGE_LENGTH)
		return;

	if (!bSucc)
	{
		char szInfo[256];
		int n = sprintf(szInfo, "Xin th«ng c¶m, %s kh«ng ë trong Vâ L©m TruyÒn Kú, tin tøc kh«ng thÓ göi ®Õn!", szSendName);
		KUiMsgCentrePad::SystemMessageArrival(szInfo, n);
		return;
	}

	if (IsInBlackName(szSourceName))
		return;

	KUiMsgCentrePad::NewMSNMessageArrival(szSourceName, szSendName, pMsgBuff, nMsgLength, pItem);
}

void KClientCallback::NotifyChannelID(char* ChannelName, DWORD channelid, BYTE cost)
{
	KUiMsgCentrePad::OpenChannel(ChannelName, channelid, cost);
}

void KClientCallback::FriendInvite(char* roleName)
{
	if (roleName && roleName[0] != 0)
	{
		// Í¨Öª½çÃæÓÐÈËÉêÇëÌí¼ÓÁÄÌìºÃÓÑ
		KSystemMessage	sMsg;
		KUiPlayerItem	sPlayer;

		strcpy(sPlayer.Name, roleName);
		sPlayer.nIndex = -1;
		sPlayer.uId = 0;
		sPlayer.nData = 0;

		sprintf(sMsg.szMessage, MSG_CHAT_GET_FRIEND_APPLY, roleName);
		sMsg.eType = SMT_FRIEND;
		sMsg.byConfirmType = SMCT_UI_FRIEND_INVITE;
		sMsg.byPriority = 3;
		sMsg.byParamSize = sizeof(KUiPlayerItem);
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);
	}
}

void KClientCallback::AddFriend(char* roleName, BYTE answer)
{
	if (roleName && roleName[0] != 0)
	{
		if (answer == answerAgree)
		{
			KSystemMessage	sMsg;
			KUiPlayerItem	sPlayer;

			memset(&sPlayer, 0, sizeof(KUiPlayerItem));
			strcpy(sPlayer.Name, roleName);
			sPlayer.uId = 0;
			sPlayer.nIndex = -1;

			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_SUCCESS, roleName);
			sMsg.eType = SMT_FRIEND;
			sMsg.byConfirmType = SMCT_UI_INTERVIEW;
			sMsg.byPriority = 2;
			sMsg.byParamSize = sizeof(KUiPlayerItem);
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, (int)&sPlayer);

			int nUnit = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnit >= 0)
			{
				KUiChatCentre::DeleteFriend(nUnit, roleName, false);
			}

			KUiChatCentre::AddFriendInfo(0, roleName, "");
			KUiChatCentre::FriendStatus(0, roleName, stateOnline);
		}
		else if (answer == answerDisagree)
		{
			// Í¨Öª½çÃæpRefuse->m_szName¾Ü¾øÁËËûµÄ½»ÓÑÉêÇë
			KSystemMessage	sMsg;

			sprintf(sMsg.szMessage, MSG_CHAT_REFUSE_FRIEND, roleName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
		else if (answer == answerUnable)
		{
			// Í¨Öª½çÃæÌí¼ÓÄ³ÈËÎªºÃÓÑ²Ù×÷Ê§°Ü
			KSystemMessage	sMsg;
			sprintf(sMsg.szMessage, MSG_CHAT_ADD_FRIEND_FAIL, roleName);
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void KClientCallback::DeleteFriend(char* roleName)
{
	if (roleName && roleName[0] != 0)
	{
		KSystemMessage	sMsg;
		sprintf(sMsg.szMessage, MSG_CHAT_DELETED_FRIEND, roleName);
		sMsg.eType = SMT_NORMAL;
		sMsg.byConfirmType = SMCT_NONE;
		sMsg.byPriority = 0;
		sMsg.byParamSize = 0;
		CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

		int nUnit = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
		if (nUnit >= 0)
		{
			KUiChatCentre::DeleteFriend(nUnit, roleName, true);
		}
	}
}

void KClientCallback::FriendStatus(char* roleName, BYTE state)
{
	if (roleName && roleName[0] != 0)
	{
		int nUnit = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
		if (nUnit >= 0 && KUiChatCentre::FriendStatus(nUnit, roleName, state))
		{
			// Í¨Öª½çÃæÓÐºÃÓÑÉÏÏß
			KSystemMessage	sMsg;
			switch (nUnit)
			{
			case FRIEND_INDEX:
				if (state == stateOnline)
					sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_ONLINE, roleName);
				else if (state == stateOffline)
					sprintf(sMsg.szMessage, MSG_CHAT_FRIEND_OFFLINE, roleName);
				else
					return;
					break;
			case ENEMY_INDEX:
				if (state == stateOnline)
					sprintf(sMsg.szMessage, MSG_CHAT_ENEMY_ONLINE, roleName);
				else if (state == stateOffline)
					sprintf(sMsg.szMessage, MSG_CHAT_ENEMY_OFFLINE, roleName);
				else
					return;
					break;
			}
			sMsg.eType = SMT_NORMAL;
			sMsg.byConfirmType = SMCT_NONE;
			sMsg.byPriority = 0;
			sMsg.byParamSize = 0;
			CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);
		}
	}
}

void KClientCallback::FriendInfo(char* roleName, char* unitName, char* groupName, BYTE state)
{
	if (roleName && roleName[0] != 0 && unitName && groupName)
	{
		int nUnit = 0;
		if (unitName[0])
			nUnit = KUiChatCentre::FindUnitIndex(unitName);
		if (nUnit >= 0)
		{
			int nUnitOld = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnitOld >= 0 && nUnitOld != nUnit)
			{
				KUiChatCentre::DeleteFriend(nUnitOld, roleName, false);
			}

			KUiChatCentre::AddFriendInfo(nUnit, roleName, groupName);
			KUiChatCentre::FriendStatus(nUnit, roleName, state);
		}
	}
}

void KClientCallback::AddPeople(char* unitName, char* roleName)
{
	if (roleName && roleName[0] != 0 && unitName)
	{
		int nUnit = KUiChatCentre::FindUnitIndex(unitName);
		if (nUnit >= 0)
		{
			int nUnitOld = KUiChatCentre::FindUnitIndexByRoleNameAtServerUnit(roleName);
			if (nUnitOld >= 0 && nUnitOld != nUnit)
			{
				KUiChatCentre::DeleteFriend(nUnitOld, roleName, false);
			}
			
			KUiChatCentre::AddFriendInfo(nUnit, roleName, "");
			KUiChatCentre::FriendStatus(nUnit, roleName, stateOnline);
		}
	}
}

#define LEVEL_TIPS_INI "\\Ui\\ÔÓ»â.ini"
//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºµÈ¼¶µÄÌùÊ¿
//--------------------------------------------------------------------------
void LevelTips(int nNewLevel)
{
	char szMsg[256], szBuf[16];
	KIniFile Ini;
	int nMsgLen;

	if(Ini.Load(LEVEL_TIPS_INI))
	{
		Ini.GetString("LevelUpTips", itoa(nNewLevel, szBuf, 10), "", szMsg, sizeof(szMsg));

		nMsgLen = TEncodeText(szMsg, strlen(szMsg));

		if(nMsgLen)
		{
		    KUiMsgCentrePad::SystemMessageArrival(szMsg, nMsgLen);
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÃÅÅÉµÄÌùÊ¿
//--------------------------------------------------------------------------
void FactionTips(int nbJoin)
{
	char szMsg[256] = "";
	KIniFile Ini;
	int nMsgLen;

	if(Ini.Load(LEVEL_TIPS_INI))
	{
		if (nbJoin)
			Ini.GetString("FactionTips", "Join", "", szMsg, sizeof(szMsg));

		nMsgLen = TEncodeText(szMsg, strlen(szMsg));

		if(nMsgLen)
		{
		    KUiMsgCentrePad::SystemMessageArrival(szMsg, nMsgLen);
		}
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÌùÊ¿ÀÏ´ó
//--------------------------------------------------------------------------
void GameWorldTips(unsigned int uParam, int nParam)
{
	if(uParam == PBP_LEVEL)
		LevelTips(nParam);
	else if(uParam == PBP_FACTION)
		FactionTips(nParam);
}
