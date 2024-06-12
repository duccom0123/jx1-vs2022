/*******************Editer	: duccom0123 EditTime:	2024/06/12 11:48:42*********************
//	������Core server �ӿڷ���
//	Copyright : Kingsoft 2002
//	Author	:   Wooy (Wu yue)
//	CreateTime:	2002-12-20
------------------------------------------------------------------------------------------
*****************************************************************************************/
#include "KCore.h"
#include "CoreServerShell.h"
#include "KThread.h"
#include "KPlayer.h"
#include "KItemList.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KNewProtocolProcess.h"
#include "KPlayerSet.h"
#include "KGameData.h"
#include "KTongData.h"
#include "KLadder.h"
#include	<time.h>
#ifdef _STANDALONE
#include "KLadder.cpp"
#endif

//#include "KNetServer.h"
//#include "../MultiServer/Heaven/Interface/iServer.h"
#ifdef _STANDALONE
#include "IClient.h"
#else
#include "../../lib/S3DBInterface.h"
#include "../../Headers/IClient.h"
#include "../../../Headers/IClient.h"
#include "../../../Headers/KGmProtocol.h"
#endif

#include "LuaFuns.h"
#include "KSortScript.h"
#include "KSubWorld.h"


#include "malloc.h"

class CoreServerShell : public iCoreServerShell
{
public:
	int  GetLoopRate();
	void GetGuid(int nIndex, void* pGuid);
	DWORD GetExchangeMap(int nIndex);
	bool IsPlayerLoginTimeOut(int nIndex);
	void RemovePlayerLoginTimeOut(int nIndex);
	bool IsPlayerExchangingServer(int nIndex);
	void ProcessClientMessage(int nIndex, const char* pChar, int nSize);
	void ProcessNewClientMessage(IClient*, DWORD, DWORD, int nIndex, const char* pChar, int nSize);
	void SendNetMsgToTransfer(IClient* pClient);
	void SendNetMsgToChat(IClient* pClient);
	void SendNetMsgToTong(IClient* pClient);
	void ProcessBroadcastMessage(const char* pChar, int nSize);
	void ProcessExecuteMessage(const char* pChar, int nSize);
	void ClientDisconnect(int nIndex);
	void RemoveQuitingPlayer(int nIndex);
	void* SavePlayerDataAtOnce(int nIndex);
	bool IsCharacterQuiting(int nIndex);
	void SetCharacterLixian(int nIndex, BYTE byLixian);
	int GetCharacterLixian(int nIndex);
	bool CheckProtocolSize(const char* pChar, int nSize);
	bool PlayerDbLoading(int nPlayerIndex, int bSyncEnd, int& nStep, unsigned int& nParam);
	int  AttachPlayer(const unsigned long lnID, GUID* pGuid);
	void GetPlayerIndexByGuid(GUID* pGuid, int* pnIndex, int* plnID);
	void AddPlayerToWorld(int nIndex);
	void* PreparePlayerForExchange(int nIndex);
	void PreparePlayerForLoginFailed(int nIndex);
	void RemovePlayerForExchange(int nIndex);
	void RecoverPlayerExchange(int nIndex);
	int  AddCharacter(int nExtPoint, int nChangeExtPoint, void* pBuffer, GUID* pGuid);
	//����Ϸ���Ͳ���
	int	 OperationRequest(unsigned int uOper, unsigned int uParam, int nParam);
	//��ȡ����״��
	int	 GetConnectInfo(KCoreConnectInfo* pInfo);
	//BOOL ValidPingTime(int nIndex);
	//����Ϸ�����ȡ����
	int	 GetGameData(unsigned int uDataId, unsigned int uParam, int nParam);
	//�ճ����core���Ҫ���������򷵻�0�����򷵻ط�0ֵ
	int  Breathe();
	//�ͷŽӿڶ���
	void Release();
	void SetSaveStatus(int nIndex, UINT uStatus);
	UINT GetSaveStatus(int nIndex);

	BOOL GroupChat(IClient* pClient, DWORD FromIP, unsigned long FromRelayID, DWORD channid, BYTE tgtcls, DWORD tgtid, const void* pData, size_t size);
	void SetLadder(void* pData, size_t uSize);
	BOOL PayForSpeech(int nIndex, int nType);
	void SetExtPoint(int nIndex, int nExtPoint);
	void GetLastName(int nIndex, char* szName);
	void RenameRoleRequest(int nIndex, bool bResult);
	void SaveData();
private:
	int	 OnLunch(LPVOID pServer);
	int	 OnLaunch(LPVOID pServer);
	int	 OnShutdown();
};

static CoreServerShell	g_CoreServerShell;

CORE_API void g_InitCore();

#ifndef _STANDALONE
extern "C" __declspec(dllexport)
#endif
iCoreServerShell* CoreGetServerShell()
{
	g_InitCore();
	return &g_CoreServerShell;
}

void CoreServerShell::Release()
{
	g_ReleaseCore();
}

int CoreServerShell::GetLoopRate()
{
	return g_SubWorldSet.m_nLoopRate;
}

	//��ȡ����״��
int	 CoreServerShell::GetConnectInfo(KCoreConnectInfo* pInfo)
{
	if (pInfo)
		pInfo->nNumPlayer = PlayerSet.GetPlayerNumber();
	return 1;
}


int CoreServerShell::AddCharacter(int nExtPoint, int nChangeExtPoint, void* pBuffer, GUID* pGuid)
{
	int nIdx = 0;
	const TRoleData* pData = (const TRoleData*)pBuffer;

	if (pData && pData->BaseInfo.szName[0])
	{
		nIdx = PlayerSet.Add((char*)pData->BaseInfo.szName, pGuid);
		if (nIdx <= 0 || nIdx >= MAX_PLAYER)
			return 0;
		strcpy(Player[nIdx].AccountName, pData->BaseInfo.caccname);
		strcpy(Player[nIdx].Name, pData->BaseInfo.szName);

		DWORD	dwLen = pData->dwDataLen;
		ZeroMemory(Player[nIdx].m_SaveBuffer, sizeof(Player[nIdx].m_SaveBuffer));
		memcpy(Player[nIdx].m_SaveBuffer, pBuffer, dwLen);
		Player[nIdx].m_nExtPoint = nExtPoint;
		Player[nIdx].m_pStatusLoadPlayerInfo = Player[nIdx].m_SaveBuffer;
		return nIdx;
	}
	return 0;
}

bool CoreServerShell::PlayerDbLoading(int nPlayerIndex, int bSyncEnd, int& nStep, unsigned int& nParam)
{
	TRoleData* pData = (TRoleData *)Player[nPlayerIndex].m_pStatusLoadPlayerInfo;

	if (bSyncEnd)
	{
		Player[nPlayerIndex].m_pStatusLoadPlayerInfo = NULL;
		nStep = 0;
		nParam = 0;
		return true;
	}
	else if (pData)	
	{
		return Player[nPlayerIndex].LoadDBPlayerInfo((BYTE *)pData, nStep, nParam);
	}
	return false;
}

void CoreServerShell::AddPlayerToWorld(int nIndex)
{
	Player[nIndex].LaunchPlayer();
}

void CoreServerShell::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	PlayerSet.ProcessClientMessage(nIndex, pChar, nSize);
}

void CoreServerShell::ProcessNewClientMessage(IClient* pTransfer,
									   DWORD dwFromIP, DWORD dwFromRelayID,
									   int nPlayerIndex,
									   const char* pChar, int nSize)
{
	g_NewProtocolProcess.ProcessNetMsg(pTransfer, dwFromIP, dwFromRelayID,
										nPlayerIndex, (BYTE*)pChar, nSize);

}

void CoreServerShell::SendNetMsgToTransfer(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToTransfer(pClient);
}

void CoreServerShell::SendNetMsgToChat(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToChat(pClient);
}

void CoreServerShell::SendNetMsgToTong(IClient* pClient)
{
	g_NewProtocolProcess.SendNetMsgToTong(pClient);
}

void CoreServerShell::ProcessBroadcastMessage(const char* pChar, int nSize)
{
	g_NewProtocolProcess.BroadcastLocalServer(pChar, nSize);
}

void CoreServerShell::ProcessExecuteMessage(const char* pChar, int nSize)
{
	g_NewProtocolProcess.ExecuteLocalServer(pChar, nSize);
}

void CoreServerShell::ClientDisconnect(int nIndex)
{
	PlayerSet.PrepareRemove(nIndex);
}

void CoreServerShell::RemoveQuitingPlayer(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].IsWaitingRemove())
		PlayerSet.RemoveQuiting(nIndex);
}
//--------------------------------------------------------------------------
//	���ܣ�����Ϸ�����ȡ����
//	������unsigned int uDataId --> ��ʾ��ȡ��Ϸ���ݵ�������������������ֵΪ÷������
//							GAMEDATA_INDEX��ȡֵ֮һ��
//		  unsigned int uParam  --> ����uDataId��ȡֵ�������
//		  int nParam --> ����uDataId��ȡֵ�������
//	���أ�����uDataId��ȡֵ���������
//--------------------------------------------------------------------------
int	CoreServerShell::GetGameData(unsigned int uDataId, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uDataId)
	{
	case SGDI_CHARACTER_ACCOUNT:
		if (uParam)
		{
			nRet = PlayerSet.GetPlayerAccount(nParam, (char *)uParam);
			if (nRet == FALSE)
				((char *)uParam)[0] = 0;
		}
		break;
	case SGDI_CHARACTER_NAME:
		if (uParam)
		{
			nRet = PlayerSet.GetPlayerName(nParam, (char*)uParam);
			if (nRet == FALSE)
				((char *)uParam)[0] = 0;
		}
		break;
	case SGDI_CHARACTER_EXTPOINTCHANGED:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetExtPointChanged();
		}
		break;
	case SGDI_CHARACTER_EXTPOINT:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			nRet = Player[uParam].GetExtPoint();
		}
		break;
	case SGDI_LOADEDMAP_ID:
		if (uParam)
		{
			int i = 0;
			int nMax = nParam;
			if(nMax < MAX_SUBWORLD) nMax = MAX_SUBWORLD;
			for (i = 0; i < nMax; i++)
			{
				if (SubWorld[i].m_SubWorldID != -1)
				{
					((int *)uParam)[i] = SubWorld[i].m_SubWorldID; 
					// ((char *)uParam)[i] = (char)SubWorld[i].m_SubWorldID;
				}
				else
				{
					nRet = i;
					break;
				}
			}
		}
		break;
	case SGDI_CHARACTER_ID:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}

			nRet = Player[uParam].m_dwID;
			break;
		}
		break;
	case SGDI_CHARACTER_SEX:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Npc[Player[nParam].m_nIndex].m_nSex;
		}
		break;
	case SGDI_CHARACTER_NETID:
		if (uParam)
		{
			if (uParam >= MAX_PLAYER)
			{
				nRet = -1;
				break;
			}
			nRet = Player[uParam].m_nNetConnectIdx;
		}
		break;

	// �����Ὠ�����������������Ƿ����
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_CREATE point
	// return : �����Ƿ����
	case SGDI_TONG_APPLY_CREATE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_APPLY_CREATE	*pApply = (STONG_SERVER_TO_CORE_APPLY_CREATE*)uParam;

			int		nPlayerIdx;
			int		nCamp;
			char	szTongName[defTONG_NAME_LENGTH_32 + 1];

			szTongName[sizeof(szTongName) - 1] = 0;
			memcpy(szTongName, pApply->m_szTongName, sizeof(szTongName));
			nPlayerIdx = pApply->m_nPlayerIdx;
			nCamp = pApply->m_nCamp;

			if (nPlayerIdx <= 0 || nPlayerIdx >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nPlayerIdx].m_cTong.CheckCreateCondition(nCamp, szTongName);
		}
		break;

	// ���������
	// uParam : struct STONG_SERVER_TO_CORE_APPLY_ADD point
	case SGDI_TONG_APPLY_ADD:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_APPLY_ADD	*pAdd = (STONG_SERVER_TO_CORE_APPLY_ADD*)uParam;
			if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				break;
			Player[pAdd->m_nPlayerIdx].m_cTong.TransferAddApply(pAdd->m_dwNpcID);
		}
		break;

	// �жϼ����������Ƿ����
	// uParam : ����� char point �����ڽ��հ������
	// nParam : struct STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION point
	case SGDI_TONG_CHECK_JOIN:
		{
			nRet = 0;
			if (Player[uParam].m_cTong.CheckJoinTong())
			{
				nRet = 1;
			}
		}
		break;
	case SGDI_TONG_CHECK_ADD_CONDITION:
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION	*pAdd;
			pAdd = (STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION*)nParam;
			if (pAdd->m_nSelfIdx <= 0 || pAdd->m_nSelfIdx >= MAX_PLAYER)
				break;
			if (pAdd->m_nTargetIdx <= 0 || pAdd->m_nTargetIdx >= MAX_PLAYER || g_FileName2Id(Npc[Player[pAdd->m_nTargetIdx].m_nIndex].Name) != pAdd->m_dwNameID)
				break;
			if (Player[pAdd->m_nSelfIdx].m_cTong.CheckAddCondition(pAdd->m_nTargetIdx))
			{
				Player[pAdd->m_nSelfIdx].m_cTong.GetTongName((char*)uParam);
				nRet = 1;
			}
		}
		break;
	case SGDI_TONG_CHECK_ADD_CONDITION_REPLY:
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION	*pAdd;
			pAdd = (STONG_SERVER_TO_CORE_CHECK_ADD_CONDITION*)nParam;
			if (pAdd->m_nSelfIdx <= 0 || pAdd->m_nSelfIdx >= MAX_PLAYER)
				break;
			if (pAdd->m_nTargetIdx <= 0 || pAdd->m_nTargetIdx >= MAX_PLAYER || g_FileName2Id(Npc[Player[pAdd->m_nTargetIdx].m_nIndex].Name) != pAdd->m_dwNameID)
				break;
			if (Player[pAdd->m_nTargetIdx].m_cTong.CheckAddConditionReply(pAdd->m_nSelfIdx))
			{
				Player[pAdd->m_nTargetIdx].m_cTong.GetTongName((char*)uParam);
				nRet = 1;
			}
			else
			{
				int nLength = strlen(Npc[Player[pAdd->m_nSelfIdx].m_nIndex].Name);

				SHOW_MSG_SYNC	sMsg;
				sMsg.ProtocolType = s2c_msgshow;
				sMsg.m_wMsgID = enumMSG_ID_TONG_APPLY_ADD_ERROR;
				sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1 - sizeof(LPVOID) + nLength;
				sMsg.m_lpBuf = new BYTE[sMsg.m_wLength + 1];

				memcpy(sMsg.m_lpBuf, &sMsg, sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID));
				memcpy((char*)sMsg.m_lpBuf + sizeof(SHOW_MSG_SYNC) - sizeof(LPVOID), Npc[Player[pAdd->m_nSelfIdx].m_nIndex].Name, nLength);

				if (g_pServer)
					g_pServer->PackDataToClient(Player[pAdd->m_nTargetIdx].m_nNetConnectIdx, sMsg.m_lpBuf, sMsg.m_wLength + 1);
			}
		}
		break;
	// ��ð����Ϣ
	// uParam : ����� STONG_SERVER_TO_CORE_GET_INFO point
	case SGDI_TONG_GET_INFO:
		{
			STONG_SERVER_TO_CORE_GET_INFO	*pInfo = (STONG_SERVER_TO_CORE_GET_INFO*)uParam;
			switch (pInfo->m_nInfoID)
			{
			case enumTONG_APPLY_INFO_ID_SELF:
				{
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					Player[pInfo->m_nSelfIdx].m_cTong.SendSelfInfo();
				}
				break;
			case enumTONG_APPLY_INFO_ID_MASTER:
				break;
			case enumTONG_APPLY_INFO_ID_DIRECTOR:
				break;
			case enumTONG_APPLY_INFO_ID_MANAGER:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetManagerInfo((DWORD)pInfo->m_nParam1))
						nRet = 1;
				}
				break;
			case enumTONG_APPLY_INFO_ID_MEMBER:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					if (Player[pInfo->m_nSelfIdx].m_cTong.CanGetMemberInfo((DWORD)pInfo->m_nParam1))
						nRet = 1;
				}
				break;
			case enumTONG_APPLY_INFO_ID_ONE:
				break;

			case enumTONG_APPLY_INFO_ID_TONG_HEAD:
				{
					nRet = 0;
					if (pInfo->m_nSelfIdx <= 0 || pInfo->m_nSelfIdx >= MAX_PLAYER)
						break;
					int		nPlayer;
					if (Npc[Player[pInfo->m_nSelfIdx].m_nIndex].m_dwID == (DWORD)pInfo->m_nParam1)
						nPlayer = pInfo->m_nSelfIdx;
					else
						nPlayer = Player[pInfo->m_nSelfIdx].FindAroundPlayer((DWORD)pInfo->m_nParam1);
					if (nPlayer == -1)
						break;
					nRet = Player[nPlayer].m_cTong.GetTongNameID();
				}
				break;
			}
		}
		break;

	// �ж��Ƿ�������Ȩ��
	// uParam : ����� TONG_APPLY_INSTATE_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_INSTATE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_INSTATE_COMMAND	*pApply = (TONG_APPLY_INSTATE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckInstatePower(pApply);
		}
		break;

	// ��������������ݱ仯
	// uParam : ����� STONG_SERVER_TO_CORE_BE_INSTATED point
	case SGDI_TONG_BE_INSTATED:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_INSTATED	*pInstated = (STONG_SERVER_TO_CORE_BE_INSTATED*)uParam;
			if (pInstated->m_nPlayerIdx <= 0 || pInstated->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pInstated->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pInstated->m_nPlayerIdx].m_cTong.BeInstated(pInstated);
		}
		break;

	// �ж��Ƿ�������Ȩ��
	// uParam : ����� TONG_APPLY_KICK_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_KICK_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_KICK_COMMAND	*pKick = (TONG_APPLY_KICK_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckKickPower(pKick);
		}
		break;

	// ���߳����
	// uParam : ����� STONG_SERVER_TO_CORE_BE_KICKED point
	case SGDI_TONG_BE_KICKED:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_KICKED	*pKicked = (STONG_SERVER_TO_CORE_BE_KICKED*)uParam;
			if (pKicked->m_nPlayerIdx <= 0 || pKicked->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pKicked->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pKicked->m_nPlayerIdx].m_cTong.BeKicked(pKicked);
		}
		break;

	// �뿪���
	// uParam : ����� TONG_APPLY_LEAVE_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_LEAVE_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_LEAVE_COMMAND	*pLeave = (TONG_APPLY_LEAVE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckLeavePower(pLeave);
		}
		break;

	case SGDI_TONG_MONEY_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_SAVE_COMMAND	*pSave = (TONG_APPLY_SAVE_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckMoneyPower(pSave);
		}
		break;
	case SGDI_TONG_SAVEEFF_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_SAVE_EFF_COMMAND	*pSave = (TONG_APPLY_SAVE_EFF_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckSaveEffPower(pSave);
		}
		break;

	// �뿪���
	// uParam : ����� STONG_SERVER_TO_CORE_LEAVE point
	case SGDI_TONG_LEAVE:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_LEAVE	*pLeave = (STONG_SERVER_TO_CORE_LEAVE*)uParam;
			if (pLeave->m_nPlayerIdx <= 0 || pLeave->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pLeave->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pLeave->m_nPlayerIdx].m_cTong.Leave(pLeave);
		}
		break;

	// �뿪����ж�
	// uParam : ����� TONG_APPLY_CHANGE_MASTER_COMMAND point
	// nParam : PlayerIndex
	case SGDI_TONG_CHANGE_MASTER_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_MASTER_COMMAND	*pChange = (TONG_APPLY_CHANGE_MASTER_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeMasterPower(pChange);
		}
		break;

	// �ܷ���ܴ�λ�ж�
	// uParam : ����� STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER point
	case SGDI_TONG_GET_MASTER_POWER:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER	*pCheck = (STONG_SERVER_TO_CORE_CHECK_GET_MASTER_POWER*)uParam;
			if (pCheck->m_nPlayerIdx <= 0 || pCheck->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pCheck->m_nPlayerIdx].m_nIndex <= 0)
				break;
			nRet = Player[pCheck->m_nPlayerIdx].m_cTong.CheckGetMasterPower(pCheck);
		}
		break;

	// ��λ�������ݸı�
	// uParam : ����� STONG_SERVER_TO_CORE_CHANGE_AS point
	case SGDI_TONG_CHANGE_AS:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_CHANGE_AS	*pAs = (STONG_SERVER_TO_CORE_CHANGE_AS*)uParam;
			if (pAs->m_nPlayerIdx <= 0 || pAs->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pAs->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pAs->m_nPlayerIdx].m_cTong.ChangeAs(pAs);
		}
		break;
	case SGDI_TONG_CHANGE_MONEY:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_MONEY	*pChange = (STONG_SERVER_TO_CORE_MONEY*)uParam;
			switch(pChange->nType)
			{
			case 0://Save
				Player[pChange->m_nPlayerIdx].Pay(pChange->m_nMoney);
				break;
			case 1:
				Player[pChange->m_nPlayerIdx].Earn(pChange->m_nMoney);
				break;
			}
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeMoney(pChange->m_dwMoney);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	// ��������
	// uParam : ����� STONG_SERVER_TO_CORE_CHANGE_MASTER point
	case SGDI_TONG_CHANGE_MASTER:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_CHANGE_MASTER	*pChange = (STONG_SERVER_TO_CORE_CHANGE_MASTER*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeMaster(pChange->m_szName);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;

	// ��ð�����ַ���ת���ɵ� dword
	// nParam : PlayerIndex
	case SGDI_TONG_GET_TONG_NAMEID:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.GetTongNameID();
		}
		break;

	// ��½ʱ���ð����Ϣ
	// uParam : ����� STONG_SERVER_TO_CORE_LOGIN point
	case SGDI_TONG_LOGIN:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_LOGIN	*pLogin = (STONG_SERVER_TO_CORE_LOGIN*)uParam;
			if (pLogin->m_dwParam <= 0 || pLogin->m_dwParam >= MAX_PLAYER)
				break;
			if (Player[pLogin->m_dwParam].m_nIndex <= 0)
				break;
			Player[pLogin->m_dwParam].m_cTong.Login(pLogin);
		}
		break;
		
	// ֪ͨcore����ĳ��ҵİ����Ϣ
	// nParam : player index
	case SGDI_TONG_SEND_SELF_INFO:
		{
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			Player[nParam].m_cTong.SendSelfInfo();
		}
		break;
	case SGDI_TONG_CHANGE_AGNAME_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_AGNAME_COMMAND	*pChange = (TONG_APPLY_CHANGE_AGNAME_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeAgnamePower(pChange);
		}
		break;
	case SGDI_TONG_GET_AGNAME_POWER:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_CHECK_GET_AGNAME_POWER	*pCheck = (STONG_SERVER_TO_CORE_CHECK_GET_AGNAME_POWER*)uParam;
			if (pCheck->m_nPlayerIdx <= 0 || pCheck->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pCheck->m_nPlayerIdx].m_nIndex <= 0)
				break;
			nRet = Player[pCheck->m_nPlayerIdx].m_cTong.CheckGetAgnamePower(pCheck);
		}
		break;
	case SGDI_TONG_CHANGE_SEX_AGNAME_POWER:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND	*pChange = (TONG_APPLY_CHANGE_SEX_AGNAME_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeSexAgnamePower(pChange);
		}
		break;
	case SGDI_TONG_BE_CHANGED_AGNAME:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_AGNAME	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_AGNAME*)uParam;
			if (pSync->m_nPlayerIdx <= 0 || pSync->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pSync->m_nPlayerIdx].m_nIndex <= 0)
				break;
			Player[pSync->m_nPlayerIdx].m_cTong.BeChangedAgname(pSync);
		}
		break;
	case SGDI_TONG_APPLY_CHANGE_CAMP:
		if (uParam)
		{
			TONG_APPLY_CHANGE_CAMP_COMMAND	*pChange = (TONG_APPLY_CHANGE_CAMP_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
			{
				nRet = FALSE;
				break;
			}
			nRet = Player[nParam].m_cTong.CheckChangeCampCondition(pChange);
		}
		break;
	case SGDI_TONG_BE_CHANGED_CAMP:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_CAMP	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_CAMP*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pSync->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.BeChangedCamp(pSync);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_BE_CHANGED_LEVEL:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_LEVEL*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pSync->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.BeChangedLevel(pSync);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_BE_CHANGED_MONEY:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_MONEY	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_MONEY*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pSync->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeMoney(pSync->m_nMoney);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_BE_CHANGED_TONG_EFF:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_TONG_EFF	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_TONG_EFF*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pSync->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeTongEff(pSync->m_nEff);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_BE_CHANGED_RECRUIT:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_RECRUIT*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pSync->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.BeChangedRecruit(pSync);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_CHANGE_RECRUIT:
		if (uParam)
		{
			nRet = 0;
			TONG_CHANGE_RECRUIT_COMMAND	*pChange = (TONG_CHANGE_RECRUIT_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeRecruit(pChange);
		}
		break;
	case SGDI_TONG_BE_CHANGED_TONGPARAM:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_TONGPARAM	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_TONGPARAM*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pSync->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.BeChangedTongParam(pSync);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_CHANGE_INFO_TONG:
		if (uParam)
		{
			nRet = 0;
			TONG_APPLY_CHANGE_INFO_COMMAND	*pChange = (TONG_APPLY_CHANGE_INFO_COMMAND*)uParam;
			if (nParam <= 0 || nParam >= MAX_PLAYER)
				break;
			if (Player[nParam].m_nIndex <= 0)
				break;
			nRet = Player[nParam].m_cTong.CheckChangeInfo(pChange);
		}
		break;
	case SGDI_TONG_BE_CHANGED_JIYU:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_JIYU	*pSync = (STONG_SERVER_TO_CORE_BE_CHANGED_JIYU*)uParam;
			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pSync->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.BeChangedJiyu(pSync);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	case SGDI_TONG_BE_CHANGED_EFF:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_BE_CHANGED_EFF	*pChange = (STONG_SERVER_TO_CORE_BE_CHANGED_EFF*)uParam;
			Player[pChange->m_nPlayerIdx].m_cTong.BeChangedEff(pChange);

			int nIdx;
			nIdx = PlayerSet.GetFirstPlayer();
			while (nIdx)
			{
				if (Player[nIdx].m_cTong.GetTongNameID() == pChange->m_dwTongNameID)
				{
					Player[nIdx].m_cTong.ChangeTongEff(pChange->m_dwTotalEff);
				}
				nIdx = PlayerSet.GetNextPlayer();
			}
		}
		break;
	default:
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	���ܣ�����Ϸ���Ͳ���
//	������unsigned int uDataId --> Core�ⲿ�ͻ���core�Ĳ����������������
//							��ֵΪ÷������GAMEOPERATION_INDEX��ȡֵ֮һ��
//		  unsigned int uParam  --> ����uOperId��ȡֵ�������
//		  int nParam --> ����uOperId��ȡֵ�������
//	���أ�����ɹ����Ͳ������󣬺������ط�0ֵ�����򷵻�0ֵ��
//--------------------------------------------------------------------------
int	CoreServerShell::OperationRequest(unsigned int uOper, unsigned int uParam, int nParam)
{
	int nRet = 1;
	switch(uOper)
	{		
	case SSOI_BROADCASTING:
		nRet = PlayerSet.Broadcasting((char*)uParam, nParam);
		break;
	case SSOI_LAUNCH:	//��������
		nRet = OnLunch((LPVOID)uParam);
		break;
	case SSOI_TONG://Tong client
		nRet = OnLaunch((LPVOID)uParam);
	case SSOI_SHUTDOWN:	//�رշ���
		nRet = OnShutdown();
		break;

	// relay ��ᴴ���ɹ���֪ͨ core ������Ӧ�Ĵ���
	case SSOI_TONG_CREATE:
		{
			STONG_SERVER_TO_CORE_CREATE_SUCCESS	*pCreate = (STONG_SERVER_TO_CORE_CREATE_SUCCESS*)uParam;
			if (pCreate->m_nPlayerIdx <= 0 || pCreate->m_nPlayerIdx >= MAX_PLAYER)
			{
				nRet = 0;
				break;
			}
			if (Player[pCreate->m_nPlayerIdx].m_nIndex)
			{
				DWORD	dwID = g_FileName2Id(Npc[Player[pCreate->m_nPlayerIdx].m_nIndex].Name);
				if (dwID != pCreate->m_dwPlayerNameID)
				{
					nRet = 0;
					break;
				}
			}
			else
			{
				nRet = 0;
				break;
			}
			nRet = Player[pCreate->m_nPlayerIdx].CreateTong(pCreate->m_nCamp, pCreate->m_szTongName);
		}
		break;

	case SSOI_TONG_REFUSE_ADD:
		if (uParam)
		{
			STONG_SERVER_TO_CORE_REFUSE_ADD	*pRefuse = (STONG_SERVER_TO_CORE_REFUSE_ADD*)uParam;
			if (pRefuse->m_nSelfIdx > 0 && pRefuse->m_nSelfIdx <= MAX_PLAYER)
			{
				Player[pRefuse->m_nSelfIdx].m_cTong.SendRefuseMessage(pRefuse->m_nTargetIdx, pRefuse->m_dwNameID);
			}
		}
		break;

	case SSOI_TONG_ADD:
		if (uParam)
		{
			nRet = 0;
			STONG_SERVER_TO_CORE_ADD_SUCCESS	*pAdd = (STONG_SERVER_TO_CORE_ADD_SUCCESS*)uParam;
			if (pAdd->m_nPlayerIdx <= 0 || pAdd->m_nPlayerIdx >= MAX_PLAYER)
				break;
			if (Player[pAdd->m_nPlayerIdx].m_nIndex <= 0)
				break;
			if (g_FileName2Id(Npc[Player[pAdd->m_nPlayerIdx].m_nIndex].Name) != pAdd->m_dwPlayerNameID)
				break;
			Player[pAdd->m_nPlayerIdx].m_cTong.AddTong(
				pAdd->m_btCamp,
				pAdd->m_nMemberNum,
				pAdd->m_dwMoney,
				pAdd->m_btLevel,
				pAdd->m_dwTotalEff,
				pAdd->m_bRecruit,
				pAdd->m_nTongParam,
				pAdd->m_nTongJiyuParam,
				pAdd->m_szTongName,
				pAdd->m_szMasterName,
				pAdd->m_szAgname);
		}
		break;
	case SSOI_LOCK_CHAT:
		{
			int nIndex = PlayerSet.GetFirstPlayer();
			while(nIndex > 0)
			{
				if (strcmpi(Player[nIndex].AccountName, (char*)uParam) == 0 || 
					strcmpi(Player[nIndex].Name, (char*)uParam) == 0)
					break;
				
				nIndex = PlayerSet.GetNextPlayer();
			}
			if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
			{
				Player[nIndex].SetChatForbiddenTm(KSG_GetCurSec()+86400);
				nRet = 1;
			}
		}
		break;
	case SSOI_UNLOCK_CHAT:
		{
			int nIndex = PlayerSet.GetFirstPlayer();
			while(nIndex > 0)
			{
				if (strcmpi(Player[nIndex].AccountName, (char*)uParam) == 0 || 
					strcmpi(Player[nIndex].Name, (char*)uParam) == 0)
					break;
				
				nIndex = PlayerSet.GetNextPlayer();
			}
			if (nIndex && Player[nIndex].m_nNetConnectIdx >= 0)
			{
				Player[nIndex].SetChatForbiddenTm(0);
				nRet = 1;
			}
		}
		break;
	default:
		nRet = 0;
		break;
	}	
	return nRet;
}

int CoreServerShell::OnLunch(LPVOID pServer)
{
	g_SetServer(pServer);

	KLuaScript* g_pStartScript = (KLuaScript*) g_GetScript(SERVERSTARTUP_SCRIPT);
	if (g_pStartScript)
	{
		g_pStartScript->CallFunction(NORMAL_FUNCTION_NAME,0,"");

		g_pStartScript = (KLuaScript*) g_GetScript(SERVERTIMER_SCRIPT);
		if (g_pStartScript)
			g_pStartScript->CallFunction(NORMAL_FUNCTION_NAME,0,"");
		else
		{
			printf("Load ServerTimerScript failed!");
			return false;
		}
	}
	else
	{
		printf("Load ServerScript failed!");
		return false;
	}

	return true;
}

int CoreServerShell::OnLaunch(LPVOID pTong)
{
	g_SetTongClient(pTong);
	return true;
}

int CoreServerShell::OnShutdown()
{
	return true;
}

//�ճ����core���Ҫ���������򷵻�0�����򷵻ط�0ֵ
int CoreServerShell::Breathe()
{
	if (g_pStartScript)
	{
		if(g_SubWorldSet.GetGameTime()%GAME_FPS==0)
		{
			SYSTEMTIME aSysTime;
			GetSystemTime(&aSysTime);
			if(aSysTime.wSecond ==0)
			{
				g_pStartScript->CallFunction(NORMAL_FUNCTION_NAME,0,"");
			}
		}
	}

	g_SubWorldSet.MessageLoop();
	g_SubWorldSet.MainLoop();
	return true;
}

bool CoreServerShell::CheckProtocolSize(const char* pChar, int nSize)
{
	WORD wCheckSize;
	BYTE nProtocol = (BYTE)pChar[0];

	if (nProtocol >= c2s_end || nProtocol <= c2s_gameserverbegin)
	{
		g_DebugLog("[error]NetServer:Invalid Protocol!");
		return false;
	}

	if (g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1] == -1)
	{
		wCheckSize = *(WORD*)&pChar[1] + PROTOCOL_MSG_SIZE;
	}
	else
	{
		wCheckSize = g_nProtocolSize[nProtocol - c2s_gameserverbegin - 1];
	}
	if (wCheckSize != nSize)
	{
		g_DebugLog("[error] Protocol <%d>, CheckSize %d, Size %d\n", nProtocol, wCheckSize, nSize);
#ifndef _WIN32
		printf("[error]<%d>, should %d, but %d\n", nProtocol, wCheckSize, nSize);
#endif
		return false;
	}
	return true;
}


int CoreServerShell::AttachPlayer(const unsigned long lnID, GUID* pGuid)
{
	return PlayerSet.AttachPlayer(lnID, pGuid);
}

void* CoreServerShell::SavePlayerDataAtOnce(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return NULL;
	}

	if (Player[nIndex].Save())
	{
		Player[nIndex].m_uMustSave = SAVE_REQUEST;
		return &Player[nIndex].m_SaveBuffer;
	}
	else
	{
		return NULL;
	}
}

bool CoreServerShell::IsCharacterQuiting(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return FALSE;
	}	
	return Player[nIndex].IsWaitingRemove();
}

void CoreServerShell::SetCharacterLixian(int nIndex, BYTE byLixian)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return;
	}	
	Player[nIndex].m_byLixian = byLixian;
}

int CoreServerShell::GetCharacterLixian(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
	{
		return 0;
	}	
	return Player[nIndex].m_byLixian;
}


bool CoreServerShell::IsPlayerLoginTimeOut(int nIndex)
{
	return Player[nIndex].IsLoginTimeOut();
}

void CoreServerShell::RemovePlayerLoginTimeOut(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	if (Player[nIndex].IsLoginTimeOut())
	{
		PlayerSet.RemoveLoginTimeOut(nIndex);
	}
}

void CoreServerShell::GetPlayerIndexByGuid(GUID* pGuid, int* pnIndex, int* plnID)
{
	*pnIndex = PlayerSet.GetPlayerIndexByGuid(pGuid);
	if (*pnIndex)
	{
		*plnID = Player[*pnIndex].m_nNetConnectIdx;
	}
	else
	{
		*plnID = -1;
	}

	if (*plnID == -1)
	{
		*pnIndex = 0;
	}
}

void* CoreServerShell::PreparePlayerForExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return NULL;
	PlayerSet.PrepareExchange(nIndex);
	return &Player[nIndex].m_SaveBuffer;
}

bool CoreServerShell::IsPlayerExchangingServer(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return false;

	return Player[nIndex].IsExchangingServer();
}

void CoreServerShell::RemovePlayerForExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PlayerSet.RemoveExchanging(nIndex);
}

void CoreServerShell::GetGuid(int nIndex, void* pGuid)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;
	memcpy(pGuid, &Player[nIndex].m_Guid, sizeof(GUID));
}

DWORD CoreServerShell::GetExchangeMap(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return -1;

	return Player[nIndex].m_sExchangePos.m_dwMapID;
}

void CoreServerShell::RecoverPlayerExchange(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	Player[nIndex].m_bExchangeServer = FALSE;
	if (Player[nIndex].m_nIndex > 0)
	{
		KNpc* pNpc = &Npc[Player[nIndex].m_nIndex];
		pNpc->m_bExchangeServer = FALSE;
		pNpc->m_FightMode = pNpc->m_OldFightMode;
	}
	Player[nIndex].Earn(Player[nIndex].m_nPrePayMoney);
	Player[nIndex].m_nPrePayMoney = 0;
}

void CoreServerShell::SetSaveStatus(int nIndex, UINT uStatus)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	Player[nIndex].m_uMustSave = uStatus;
}

UINT CoreServerShell::GetSaveStatus(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return SAVE_IDLE;

	return Player[nIndex].m_uMustSave;
}


void CoreServerShell::PreparePlayerForLoginFailed(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	PlayerSet.PrepareLoginFailed(nIndex);
}

BOOL CoreServerShell::GroupChat(IClient* pClient, DWORD FromIP, unsigned long FromRelayID, DWORD channid, BYTE tgtcls, DWORD tgtid, const void* pData, size_t size)
{
	switch(tgtcls)
	{

	case tgtcls_team:
		{{
		if (tgtid < 0 || tgtid >= MAX_TEAM)
			return FALSE;

		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		// ���ӳ���
		nTargetIdx = g_Team[tgtid].m_nCaptain;
//		if (FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
			g_pServer->SendData(Player[nTargetIdx].m_nNetConnectIdx, pData, size);
		// ����Ա��
		for (int i = 0; i <	MAX_TEAM_MEMBER; i++)
		{
			nTargetIdx = g_Team[tgtid].m_nMember[i];
			if (nTargetIdx < 0)
				continue;

//			if (FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;

	case tgtcls_fac:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);
		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Player[nTargetIdx].m_cFaction.m_nCurFaction == tgtid
)//				&& FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;

	case tgtcls_tong:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Player[nTargetIdx].m_cTong.GetTongNameID() == tgtid
)//				&& FromRelayID != Player[nTargetIdx].m_nNetConnectIdx)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;

	case tgtcls_msgr:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Npc[Player[nTargetIdx].m_nIndex].m_nMissionGroup == tgtid)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);
			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;

	case tgtcls_cr:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			if (Player[nTargetIdx].m_cRoom.m_nID == tgtid)
				g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);
			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;

	case tgtcls_scrn:
		{{

//		int nMaxRelayPlayer = (1024 - 32 - sizeof(CHAT_GROUPMAN) - size) / sizeof(WORD);
//		if (nMaxRelayPlayer <= 0)
//			return FALSE;


		int idxNPC = Player[tgtid].m_nIndex;
		int idxSubWorld = Npc[idxNPC].m_SubWorldIndex;
		int idxRegion = Npc[idxNPC].m_RegionIndex;
//		_ASSERT(idxSubWorld >= 0 && idxRegion >= 0);
		int nOX = Npc[idxNPC].m_MapX;
		int nOY = Npc[idxNPC].m_MapY;
		int nTX = 0;
		int nTY = 0;
		if (idxSubWorld < 0 || idxRegion < 0)
			return FALSE;


//		size_t basesize = sizeof(CHAT_GROUPMAN) + size;
//		BYTE buffer[1024];
//
//		CHAT_GROUPMAN* pCgc = (CHAT_GROUPMAN*)buffer;
//		pCgc->ProtocolType = chat_groupman;
//		pCgc->wChatLength = size;
//		pCgc->byHasIdentify = false;
//
//		void* pExPckg = pCgc + 1;
//		memcpy(pExPckg, pData, size);
//
//		WORD* pPlayers = (WORD*)((BYTE*)pExPckg + size);
//
//
//		pCgc->wPlayerCount = 0;


		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

#define	MAX_SYNC_RANGE	23
		static POINT POff[9] = 
		{
			{0, 0},
			{0, 32},
			{-16, 32},
			{-16, 0},
			{-16, -32},
			{0, -32},
			{16, -32},
			{16, 0},
			{16, 32},
		};

		KRegion* pRegionBase = &SubWorld[idxSubWorld].m_Region[idxRegion];

		for (int i = -1; i < 8; i++)
		{
			KRegion* pRegion = NULL;
			if (i < 0)
				pRegion = pRegionBase;
			else
			{
				if (pRegionBase->m_nConnectRegion[i] < 0)
					continue;
				pRegion = &SubWorld[idxSubWorld].m_Region[pRegionBase->m_nConnectRegion[i]];
			}
			if (pRegion == NULL)
				continue;


			KIndexNode *pNode = (KIndexNode *)pRegion->m_PlayerList.GetHead();
			while(pNode)
			{
//				_ASSERT(pNode->m_nIndex > 0 && pNode->m_nIndex < MAX_PLAYER);

				//if (FromRelayID != Player[pNode->m_nIndex].m_nNetConnectIdx)
				{
					int nTargetNpc = Player[pNode->m_nIndex].m_nIndex;
					if (nTargetNpc > 0)
					{
						nTX = Npc[nTargetNpc].m_MapX + POff[i + 1].x;
						nTY = Npc[nTargetNpc].m_MapY + POff[i + 1].y;
						
						if ((nTX - nOX) * (nTX - nOX) + (nTY - nOY) * (nTY - nOY) < MAX_SYNC_RANGE * MAX_SYNC_RANGE)
							g_pServer->PackDataToClient(Player[pNode->m_nIndex].m_nNetConnectIdx, pExHeader, pckgsize);
					}


//					pPlayers[pCgc->wPlayerCount] = (WORD)Player[pNode->m_nIndex].m_nNetConnectIdx;
//					++ pCgc->wPlayerCount;
//
//					if (pCgc->wPlayerCount >= nMaxRelayPlayer)
//					{
//						size_t pckgsize = basesize + sizeof(WORD) * pCgc->wPlayerCount;
//						pCgc->wSize = pckgsize - 1;
//						
//						pClient->SendPackToServer(pCgc, pckgsize);
//
//						pCgc->wPlayerCount = 0;
//					}
				}

				pNode = (KIndexNode *)pNode->GetNext();
			}
		}

//		if (pCgc->wPlayerCount > 0)
//		{
//			size_t pckgsize = basesize + sizeof(WORD) * pCgc->wPlayerCount;
//			pCgc->wSize = pckgsize - 1;
//
//			pClient->SendPackToServer(pCgc, pckgsize);
//		}

#ifndef WIN32
		delete (char*)pExHeader;
#endif
		}}
		break;


	case tgtcls_bc:
		{{
		size_t pckgsize = sizeof(tagExtendProtoHeader) + size;
#ifdef WIN32
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)_alloca(pckgsize);
#else
		tagExtendProtoHeader* pExHeader = (tagExtendProtoHeader*)(new char[pckgsize]);
#endif
		pExHeader->ProtocolType = s2c_extendchat;
		pExHeader->wLength = pckgsize - 1;
		memcpy(pExHeader + 1, pData, size);

		int nTargetIdx;
		nTargetIdx = PlayerSet.GetFirstPlayer();
		while (nTargetIdx)
		{
			g_pServer->PackDataToClient(Player[nTargetIdx].m_nNetConnectIdx, pExHeader, pckgsize);

			nTargetIdx = PlayerSet.GetNextPlayer();
		}
#ifndef WIN32
		delete ((char*)pExHeader);
#endif
		}}
		break;

	default:
		break;
	}
	return TRUE;
}

void CoreServerShell::SetLadder(void* pData, size_t uSize)
{
	Ladder.Init(pData, uSize);
}

BOOL CoreServerShell::PayForSpeech(int nIndex, int nType)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return FALSE;

	int	nMoney = 0;
	int nNpcIdx = Player[nIndex].m_nIndex;
	if (nNpcIdx <= 0)
		return FALSE;
	if ((Player[nIndex].m_nForbiddenTm > KSG_GetCurSec()) & KPlayer::FF_CHAT)	//������
	{
		SHOW_MSG_SYNC	sMsg;
		sMsg.ProtocolType = s2c_msgshow;
		sMsg.m_wMsgID = enumMSG_ID_FUNCTION_CHAT_FORBIDDENED;
		sMsg.m_wLength = sizeof(SHOW_MSG_SYNC) - 1;
		sMsg.m_lpBuf = (LPVOID)Player[nIndex].m_nForbiddenTm;
		g_pServer->PackDataToClient(Player[nIndex].m_nNetConnectIdx, &sMsg, sMsg.m_wLength + 1);
		sMsg.m_lpBuf = 0;
		return FALSE;
	}
	if (Player[nIndex].m_bForbidName && nType == 0)
		return FALSE;
	int nLevel = Npc[nNpcIdx].m_Level;
	int nMaxMana = Npc[nNpcIdx].m_CurrentManaMax;
	switch (nType)
	{
	case 0:
		return TRUE;
		break;
	case 1:
		{
			nMoney = 100;
			if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nMoney)
				return FALSE;
			return Player[nIndex].Pay(nMoney);
		}
		break;
	case 2:
		{
			if (nLevel < 20)
				return FALSE;
			nMoney = 1000;
			if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nMoney)
				return FALSE;
			Player[nIndex].Pay(nMoney);
			return Npc[nNpcIdx].Cost(attrib_mana_v, nMaxMana / 2);
		}
		break;
	case 3:
		{
			if (nLevel < 10)
				return FALSE;
			nMoney = 500;
			if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nMoney)
				return FALSE;
			Player[nIndex].Pay(nMoney);
			return Npc[nNpcIdx].Cost(attrib_mana_v, nMaxMana / 10);
		}
		break;
	case 4:
		{
			if (nLevel < 30)
				return FALSE;
			nMoney = 5000;
			if (Player[nIndex].m_ItemList.GetEquipmentMoney() < nMoney)
				return FALSE;
			Player[nIndex].Pay(nMoney);
			return Npc[nNpcIdx].Cost(attrib_mana_v, nMaxMana * 4 / 5);
		}
		break;
	}
	return FALSE;
}

void CoreServerShell::SetExtPoint(int nIndex, int nExtPoint)
{
 	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
 	{
 		return;
 	}
	Player[nIndex].SetExtPoint(nExtPoint, 0);
}

void CoreServerShell::GetLastName(int nIndex, char* szName)
{
 	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
 	{
 		return;
 	}
	int nLen = strlen(Player[nIndex].m_szLastName);
	if(nLen > 0)
	{
		memcpy(szName, Player[nIndex].m_szLastName, nLen);
		szName[nLen] = '\0';
	}
}

void CoreServerShell::RenameRoleRequest(int nIndex, bool bResult)
{
 	if (nIndex > 0 && nIndex < MAX_PLAYER)
 	{
		if (bResult)
		{
			//to do
		}
		else
		{
			int nLen = strlen(Player[nIndex].m_szLastName);
			memcpy(Player[nIndex].Name, Player[nIndex].m_szLastName,nLen);
			Player[nIndex].Name[nLen] = '\0';
		}
		memset(Player[nIndex].m_szLastName, 0, sizeof(Player[nIndex].m_szLastName));

		Player[nIndex].ExecuteScript(Player[nIndex].m_dwTaskExcuteScriptId, Player[nIndex].m_szTaskExcuteFun, bResult);
	}
}

void CoreServerShell::SaveData()
{
	GameData.Save();
}