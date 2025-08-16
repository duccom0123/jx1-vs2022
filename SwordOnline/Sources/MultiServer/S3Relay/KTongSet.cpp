// KTongSet.cpp: implementation of the CTongSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "TongDB.h"
#include "KTongSet.h"
#include "S3Relay.h"
#include <time.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTongSet::CTongSet()
{
	m_pcTong = NULL;
	m_nTongPointSize = 0;
	Init();
}

CTongSet::~CTongSet()
{
	DeleteAll();
}

void	CTongSet::Init()
{
	DeleteAll();

	m_pcTong = (CTongControl**)new LPVOID[defTONG_SET_INIT_POINT_NUM];
	m_nTongPointSize = defTONG_SET_INIT_POINT_NUM;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		m_pcTong[i] = NULL;
	}
}

void	CTongSet::DeleteAll()
{
	if (m_pcTong)
	{
		for (int i = 0; i < m_nTongPointSize; i++)
		{
			if (m_pcTong[i])
			{
				delete m_pcTong[i];
				m_pcTong[i] = NULL;
			}
		}
		delete []m_pcTong;
		m_pcTong = NULL;
	}
	m_nTongPointSize = 0;
}

int		CTongSet::Create(int nCamp, BYTE btLevel, char *lpszPlayerName, char *lpszTongName, int nSex, int nJoinTm, int nSaveEff, int nTongJiyuParam, char *szTongJiyuNotify)
{
	// 帮会模块出错
	if (!m_pcTong || m_nTongPointSize <= 0)
		return enumTONG_CREATE_ERROR_ID13;
	// 名字字符串出错
	if (!lpszPlayerName || !lpszTongName)
		return enumTONG_CREATE_ERROR_ID14;
	// 名字字符串过长
	if (strlen(lpszTongName) >= defTONG_NAME_LENGTH_32 /*||
		strlen(lpszPlayerName) >= defTONG_NAME_LENGTH_32*/)
		return enumTONG_CREATE_ERROR_ID14;

	int		i, nPos;
	DWORD	dwTongNameID, dwPlayerNameID;

	dwTongNameID = g_String2Id(lpszTongName);
	dwPlayerNameID = g_String2Id(lpszPlayerName);

	// 名称检测，是否有同名的帮主或者同名的帮会
	for (i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID || m_pcTong[i]->m_dwMasterID == dwPlayerNameID))
			break;
	}
	// 帮会同名错误
	if (i < m_nTongPointSize)
		return enumTONG_CREATE_ERROR_ID15;

	// 查找空位
	nPos = -1;
	for (i = 0; i < m_nTongPointSize; i++)
	{
		if (!m_pcTong[i])
		{
			nPos = i;
			break;
		}
	}
	// 现有指针空间已满，分配新的更大的指针空间
	if (nPos < 0)
	{
		// 临时存储旧的指针
		CTongControl**	pTemp;
		pTemp = (CTongControl**)new LPVOID[m_nTongPointSize];
		for (i = 0; i < m_nTongPointSize; i++)
			pTemp[i] = m_pcTong[i];

		// 分配新的更大的指针空间，大小是原来的两倍
		delete []m_pcTong;
		m_pcTong = NULL;
		m_pcTong = (CTongControl**)new LPVOID[m_nTongPointSize * 2];
		for (i = 0; i < m_nTongPointSize; i++)
			m_pcTong[i] = pTemp[i];
		delete []pTemp;
		m_nTongPointSize *= 2;
		for (i = m_nTongPointSize / 2; i < m_nTongPointSize; i++)
			m_pcTong[i] = NULL;
		nPos = m_nTongPointSize / 2;
	}

	// 产生一个新的帮会
	m_pcTong[nPos] = new CTongControl(nCamp, btLevel, lpszPlayerName, lpszTongName, nSex, nJoinTm, nSaveEff, nTongJiyuParam, szTongJiyuNotify);
	// 产生失败
	if (m_pcTong[nPos]->m_dwNameID == 0)
	{
		delete m_pcTong[nPos];
		m_pcTong[nPos] = NULL;
		return enumTONG_CREATE_ERROR_ID13;
	}
	// 存盘，数据保存至数据库
	TMemberStruct	sMember;
	sMember.MemberClass = enumTONG_FIGURE_MASTER;
	sMember.nTitleIndex = 0;
	strcpy(sMember.szTong, m_pcTong[nPos]->m_szName);
	strcpy(sMember.szName, m_pcTong[nPos]->m_szMasterName);
	sMember.nSex = nSex;
	sMember.nJoinTm = nJoinTm;
	sMember.nSaveEff = nSaveEff;
	

	try
	{
		g_cTongDB.ChangeTong(*m_pcTong[nPos]);
		g_cTongDB.ChangeMember(sMember);
	}
	catch (...)
	{
		char	szMsg[96];
		sprintf(szMsg, "\\O%u", m_pcTong[i]->m_dwNameID);
					
		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
		if (channid != -1)
		{
			sprintf(szMsg, "%s �� t�o bang h�i th�nh c�ng", m_pcTong[nPos]->m_szName);
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
		}
	}

	return 0;
}

//----------------------------------------------------------------------
//	功能：添加一个帮会成员，if return == 0 成功 else return error id
//----------------------------------------------------------------------
int		CTongSet::AddMember(char *lpszPlayerName, char *lpszTongName, int nSex, int nJoinTm, int nSaveEff)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return -1;
	if (!lpszPlayerName || !lpszPlayerName[0] || !lpszTongName || !lpszTongName[0])
		return -1;
	if (strlen(lpszTongName) >= defTONG_NAME_LENGTH_32 ||
		strlen(lpszPlayerName) >= defTONG_NAME_LENGTH_32)
		return -1;

	int		i;
	DWORD	dwTongNameID;

	dwTongNameID = g_String2Id(lpszTongName);

	// 寻找帮会
	for (i = 0; i < m_nTongPointSize; i++)
	{
		// 找到了
		if (m_pcTong[i] && (m_pcTong[i]->m_dwNameID == dwTongNameID))
		{
			if (!m_pcTong[i]->AddMember(lpszPlayerName, nSex, nJoinTm, nSaveEff))
				return -1;
			else
			{
				// 存盘，数据保存至数据库
				TMemberStruct	sMember;
				sMember.MemberClass = enumTONG_FIGURE_MEMBER;
				sMember.nTitleIndex = 0;
				strcpy(sMember.szTong, m_pcTong[i]->m_szName);
				strcpy(sMember.szName, lpszPlayerName);
				sMember.nSex = nSex;
				sMember.nJoinTm = nJoinTm;
				sMember.nSaveEff = nSaveEff;
				g_cTongDB.ChangeMember(sMember);

				// 给帮会频道发消息
				char	szMsg[96];
				sprintf(szMsg, "\\O%u", m_pcTong[i]->m_dwNameID);

				DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
				if (channid != -1)
				{
					sprintf(szMsg, "%s �� gia nh�p b�n bang ", lpszPlayerName);
					g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
				}

				return i;
			}
		}
	}

	return -1;
}

//----------------------------------------------------------------------
//	功能：获得帮会阵营
//----------------------------------------------------------------------
int		CTongSet::GetTongCamp(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_nCamp;

	return 0;
}

int		CTongSet::GetTongMemberNum(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_nMemberNum;

	return 0;
}

DWORD		CTongSet::GetTongMoney(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_dwMoney;

	return 0;
}

int		CTongSet::GetTongLevel(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_btLevel;

	return 0;
}

DWORD		CTongSet::GetTongEff(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_dwTotalEff;

	return 0;
}

BOOL		CTongSet::GetTongRecruit(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return FALSE;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_bRecruit;

	return FALSE;
}

int		CTongSet::GetTongParam(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_nTongParam;

	return 0;
}

int		CTongSet::GetTongJiyuParam(int nTongIdx)
{
	if (!m_pcTong || m_nTongPointSize <= 0)
		return 0;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return 0;

	if (m_pcTong[nTongIdx])
		return m_pcTong[nTongIdx]->m_nTongJiyuParam;

	return 0;
}

BOOL	CTongSet::GetMasterName(int nTongIdx, char *lpszName)
{
	if (!lpszName)
		return FALSE;
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return FALSE;
	if (!m_pcTong[nTongIdx])
		return FALSE;
	strcpy(lpszName, m_pcTong[nTongIdx]->m_szMasterName);
	return TRUE;
}

BOOL	CTongSet::GetMemberAgname(int nTongIdx, char *lpszAgname, int nSex)
{
	if (!lpszAgname)
		return FALSE;
	if (!m_pcTong || m_nTongPointSize <= 0)
		return FALSE;
	if (nTongIdx < 0 || nTongIdx >= m_nTongPointSize)
		return FALSE;
	if (!m_pcTong[nTongIdx])
		return FALSE;
	if (nSex > 0)
		strcpy(lpszAgname, m_pcTong[nTongIdx]->m_szMaleAgname);
	else
		strcpy(lpszAgname, m_pcTong[nTongIdx]->m_szFemaleAgname);
	if (!lpszAgname[0])
	{
		if (m_pcTong[nTongIdx]->m_szNormalAgname[0])
			strcpy(lpszAgname, m_pcTong[nTongIdx]->m_szNormalAgname);
		else
			strcpy(lpszAgname, defTONG_MEMBER_AGNAME);
	}
	return TRUE;
}

BOOL	CTongSet::GetTongHeadInfo(DWORD dwTongNameID, STONG_HEAD_INFO_SYNC *pInfo)
{
	if (!m_pcTong || m_nTongPointSize <= 0 || dwTongNameID == 0)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			return m_pcTong[i]->GetTongHeadInfo(pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::GetTongManagerInfo(STONG_GET_MANAGER_INFO_COMMAND *pApply, STONG_MANAGER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == (DWORD)pApply->m_nParam1)
		{
			return m_pcTong[i]->GetTongManagerInfo(pApply, pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::GetTongMemberInfo(STONG_GET_MEMBER_INFO_COMMAND *pApply, STONG_MEMBER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == (DWORD)pApply->m_nParam1)
		{
			return m_pcTong[i]->GetTongMemberInfo(pApply, pInfo);
		}
	}

	return FALSE;
}

BOOL	CTongSet::Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync)
{
	if (!pInstate || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pInstate->m_dwTongNameID)
		{
			return m_pcTong[i]->Instate(pInstate, pSync);
		}
	}

	return FALSE;
}

BOOL	CTongSet::Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync)
{
	if (!pKick || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pKick->m_dwTongNameID)
		{
			return m_pcTong[i]->Kick(pKick, pSync);
		}
	}

	return FALSE;
}

BOOL	CTongSet::Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync)
{
	if (!pLeave || !pSync)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pLeave->m_dwTongNameID)
		{
			return m_pcTong[i]->Leave(pLeave, pSync);
		}
	}

	return FALSE;
}

BOOL	CTongSet::AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pAccept->m_dwTongNameID)
		{
			return m_pcTong[i]->AcceptMaster(pAccept);
		}
	}

	return FALSE;
}

BOOL	CTongSet::InitFromDB()
{
	int		nTongNum;

	nTongNum = g_cTongDB.GetTongCount();
	if (nTongNum < 0)
		return FALSE;
	if (nTongNum == 0)
		return TRUE;
	CTongControl**	m_tmpTong;

	m_tmpTong = (CTongControl**)new LPVOID[nTongNum];
	for (int i = 0; i < nTongNum; i++)
	{
		m_tmpTong[i] = NULL;
	}


/*	int i;*/
	TTongList	*pList = new TTongList[nTongNum];
	memset(pList, 0, sizeof(TTongList) * nTongNum);

	int nGetNum = g_cTongDB.GetTongList(pList, nTongNum);
	if (nGetNum <= 0)
		return TRUE;

	for (int i = 0; i < nGetNum; i++)
	{
		m_tmpTong[i] = new CTongControl(pList[i]);
		if (!m_tmpTong[i]->m_szName[0])
		{
			delete m_tmpTong[i];
			m_tmpTong[i] = NULL;
			continue;
		}
	}
	Init();
	g_cTongDB.Close();
	g_cTongDB.OpenNew();
	for (int i = 0; i < nGetNum; i++)
	{
		if (m_tmpTong[i] && m_tmpTong[i]->m_szName[0])
		{
			Create(m_tmpTong[i]->m_nCamp, m_tmpTong[i]->m_btLevel, m_tmpTong[i]->m_szMasterName,m_tmpTong[i]->m_szName, m_tmpTong[i]->m_psMember[0].m_nSex, m_tmpTong[i]->m_psMember[0].m_nMemberJoinTm, m_tmpTong[i]->m_psMember[0].m_nMemberSaveEff, m_tmpTong[i]->m_nTongJiyuParam, m_tmpTong[i]->m_szTongJiyuNotify);
			for (int j = 0; j < m_tmpTong[i]->m_nMemberPointSize;j++)
			{
				AddMember(m_tmpTong[i]->m_psMember[j].m_szName,m_tmpTong[i]->m_szName, m_tmpTong[i]->m_psMember[j].m_nSex, m_tmpTong[i]->m_psMember[j].m_nMemberJoinTm, m_tmpTong[i]->m_psMember[j].m_nMemberSaveEff);
			}
		}
	}
	for (int i = 0; i < nGetNum; i++)
	{
		m_pcTong[i] = new CTongControl(pList[i]);
		if (!m_pcTong[i]->m_szName[0])
		{
			delete m_pcTong[i];
			m_pcTong[i] = NULL;
		}
	}
	for (int i = 0; i < nGetNum; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_szName[0])
		{
			g_cTongDB.SearchTong(m_pcTong[i]->m_szName, *(m_pcTong[i]));
			for (int j = 0;j < m_tmpTong[i]->m_nManagerNum;j++)
			{
				m_pcTong[i]->DBInstate(m_tmpTong[i]->m_szManagerName[j],1);
			}

			for (int j = 0;j < m_tmpTong[i]->m_nDirectorNum;j++)
			{
				m_pcTong[i]->DBInstate(m_tmpTong[i]->m_szDirectorName[j],2);
			}
		}
	}
	if (m_tmpTong)
	{
		delete [] m_tmpTong;
	}
	if (pList)
	{
		delete [] pList;
	}

	return TRUE;
}

BOOL	CTongSet::GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync)
{
	if (!pLogin || !pSync)
		return FALSE;

	memset(pSync, 0, sizeof(STONG_LOGIN_DATA_SYNC));
	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_LOGIN_DATA;
	pSync->m_btFlag			= 0;
	pSync->m_dwParam		= pLogin->m_dwParam;

	if (!m_pcTong)
		return FALSE;

	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pLogin->m_dwTongNameID)
		{
			return m_pcTong[i]->GetLoginData(pLogin, pSync);
		}
	}

	return TRUE;
}

BOOL	CTongSet::SearchOne(DWORD dwTongNameID, char *lpszName, STONG_ONE_LEADER_INFO *pInfo)
{
	if (!m_pcTong)
		return FALSE;
	if (dwTongNameID == 0 || !lpszName || !lpszName[0] || !pInfo)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == dwTongNameID)
		{
			return m_pcTong[i]->SearchOne(lpszName, pInfo);
		}
	}

	return FALSE;
}

BOOL CTongSet::ChangeMoney( STONG_MONEY_COMMAND *pMoney,STONG_MONEY_SYNC *Sync)
{
	if (!pMoney)
		return FALSE;
	if (!m_pcTong)
		return FALSE;
	
	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pMoney->m_dwTongNameID)
		{
			if (m_pcTong[i]->ChangeMoney(pMoney,Sync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::AcceptAgname(STONG_ACCEPT_AGNAME_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	if (!m_pcTong)
		return FALSE;
	
	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pAccept->m_dwTongNameID)
		{
			BOOL bRet = m_pcTong[i]->DBChangeAgname(pAccept);
			if (bRet)
			{
				g_cTongDB.ChangeTong(*m_pcTong[i]);
				return TRUE;
			}
			return FALSE;
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::AcceptSexAgname(STONG_ACCEPT_SEX_AGNAME_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	if (!m_pcTong)
		return FALSE;
	
	// 寻找帮会
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pAccept->m_dwTongNameID)
		{
			BOOL bRet = m_pcTong[i]->DBChangeSexAgname(pAccept);
			if (bRet)
			{
				g_cTongDB.ChangeTong(*m_pcTong[i]);
				return TRUE;
			}
			return FALSE;
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeCamp(STONG_CHANGE_CAMP_COMMAND *pData, STONG_BE_CHANGED_CAMP_SYNC *pSync)
{
	if (!pData)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeCamp(pData, pSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongRecruit(STONG_CHANGE_TONG_INFO_COMMAND *pData, STONG_CHANGE_TONG_INFO_SYNC *pSync)
{
	if (!pData)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeTongRecruit(pData, pSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongParam(STONG_CHANGE_TONG_INFO_COMMAND *pData, STONG_CHANGE_TONG_INFO_SYNC *pSync)
{
	if (!pData)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeTongParam(pData, pSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongLevel(STONG_CHANGE_TONG_INFO_COMMAND *pData, STONG_CHANGE_TONG_INFO_SYNC *pSync)
{
	if (!pData)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeTongLevel(pData, pSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongMoney(STONG_CHANGE_TONG_INFO_COMMAND *pData, STONG_CHANGE_TONG_INFO_SYNC *pSync)
{
	if (!pData)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeTongMoney(pData, pSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeTongEff(STONG_CHANGE_TONG_INFO_COMMAND *pData, STONG_CHANGE_TONG_INFO_SYNC *pSync)
{
	if (!pData)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeTongEff(pData, pSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL	CTongSet::ChangeJiyu(STONG_CHANGE_TONG_INFO_COMMAND *pData, STONG_CHANGE_TONG_INFO_SYNC *pSync)
{
	if (!pData)
		return FALSE;
	
	if (!m_pcTong)
		return FALSE;
	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeJiyu(pData, pSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL CTongSet::ChangeMemberEff(STONG_CHANGE_TONG_MEMBEREFF_COMMAND *pInfo, STONG_CHANGE_TONG_MEMBEREFF_SYNC *sSync)
{
	if (!pInfo)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pInfo->m_dwTongNameID)
		{
			if (m_pcTong[i]->DBChangeMemberEff(pInfo, sSync))
			{
				return g_cTongDB.ChangeTong(*m_pcTong[i]);
			}
		}
	}
	
	return FALSE;
}

BOOL CTongSet::MessageToTong(STONG_MESSAGE_INFO_COMMAND *pData)
{
	if (!pData)
		return FALSE;
	if (!m_pcTong)
		return FALSE;

	for (int i = 0; i < m_nTongPointSize; i++)
	{
		if (m_pcTong[i] && m_pcTong[i]->m_dwNameID == pData->dwParam)
		{
			return m_pcTong[i]->MessageToTong(pData);
		}
	}
	
	return FALSE;
}

BOOL CTongSet::MessageToFaction(STONG_MESSAGE_INFO_COMMAND *pData)
{
	if (!pData)
		return FALSE;
	if (pData->dwParam >= 0)
	{
		char	szMsg[96];
		sprintf(szMsg, "\\F%u", pData->dwParam);

		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
		if (channid != -1)
		{
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(pData->szName), std::string(pData->szMsg));
		}
		return TRUE;
	}
	return FALSE;
}

BOOL CTongSet::MessageToChatRoom(STONG_MESSAGE_INFO_COMMAND *pData)
{
	if (!pData)
		return FALSE;
	if (pData->dwParam >= 0)
	{
		char	szMsg[96];
		sprintf(szMsg, "\\C%u", pData->dwParam);

		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
		if (channid != -1)
		{
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(pData->szName), std::string(pData->szMsg));
		}
		return TRUE;
	}
	return FALSE;
}