// KTongControl.cpp: implementation of the CTongControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Global.h"
#include "KTongControl.h"
#include "S3Relay.h"
#include "time.h"

#define		defTONG_INIT_MEMBER_SIZE		100		// ³ÉÔ±ÄÚ´æ³õÊ¼»¯Ê±µÄ´óÐ¡
#define		defTONG_MEMBER_SIZE_ADD			100		// ³ÉÔ±ÄÚ´æÃ¿´ÎÔö¼ÓµÄ´óÐ¡

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTongControl::CTongControl(int nCamp, BYTE btLevel, char *lpszPlayerName, char *lpszTongName, int nMasterSex, int nMasterJoinTm, int nMasterSaveEff, int nTongJiyuParam, char *szTongJiyuNotify)
{
	m_nCamp			= 0;
	m_dwMoney		= 0;
	m_nCredit		= 0;
	m_btLevel		= 0;
	m_nDirectorNum	= 0;
	m_nManagerNum	= 0;
	m_nMemberNum	= 0;

	m_dwNameID		= 0;
	m_szName[0]		= 0;

	m_szMasterAgname[0] = 0;
	memset(m_szDirectorAgname, 0, sizeof(m_szDirectorAgname));
	memset(m_szManagerAgname, 0, sizeof(m_szManagerAgname));
	m_szNormalAgname[0] = 0;

	m_dwMasterID = 0;
	m_szMasterName[0] = 0;

	memset(m_dwDirectorID, 0, sizeof(m_dwDirectorID));
	memset(m_szDirectorName, 0, sizeof(m_szDirectorName));

	memset(m_dwManagerID, 0, sizeof(m_dwManagerID));
	memset(m_szManagerName, 0, sizeof(m_szManagerName));

	m_psMember = (STONG_MEMBER*)new STONG_MEMBER[defTONG_INIT_MEMBER_SIZE];
	m_nMemberPointSize = defTONG_INIT_MEMBER_SIZE;
	memset(m_psMember, 0, sizeof(STONG_MEMBER) * m_nMemberPointSize);

	if (nCamp != camp_justice && nCamp != camp_evil && nCamp != camp_balance)
		return;
	if (!lpszPlayerName || !lpszPlayerName[0] || strlen(lpszPlayerName) >= defTONG_NAME_LENGTH_32)
		return;
	if (!lpszTongName || !lpszTongName[0] || strlen(lpszTongName) >= defTONG_NAME_LENGTH_32)
		return;
	m_nCamp = nCamp;
	m_btLevel = btLevel;
	strcpy(m_szMasterName, lpszPlayerName);
	m_dwMasterID = g_String2Id(m_szMasterName);
	strcpy(m_szName, lpszTongName);
	m_dwNameID = g_String2Id(m_szName);
	m_szMaleAgname[0] = 0;
	m_szFemaleAgname[0] = 0;
	m_nMasterSex = nMasterSex;
	m_nMasterJoinTm = nMasterJoinTm;
	memset(m_nDirectorSex, 0, sizeof(m_nDirectorSex));
	memset(m_nManagerSex, 0, sizeof(m_nManagerSex));
	memset(m_nDirectorJoinTm, 0 , sizeof(m_nDirectorJoinTm));
	memset(m_nManagerJoinTm, 0 , sizeof(m_nManagerJoinTm));
	m_nTongJiyuParam = nTongJiyuParam;
	strcpy(m_szTongJiyuNotify, szTongJiyuNotify);
	m_nMasterSaveEff = nMasterSaveEff;
	m_nDirectorSaveEff[0] = 0;
	m_nManagerSaveEff[0] = 0;
	m_dwTotalEff = 0;
	m_nSaveEff = 0;
	m_bRecruit = TRUE;
	m_nTongParam = 0;
}

CTongControl::CTongControl(TTongList sList)
{
	m_nCamp			= 0;
	m_dwMoney		= 0;
	m_nCredit		= 0;
	m_btLevel		= 0;
	m_nDirectorNum	= 0;
	m_nManagerNum	= 0;

	strcpy(m_szName, sList.szName);
	m_dwNameID		= g_String2Id(m_szName);

	m_szMasterAgname[0] = 0;
	memset(m_szDirectorAgname, 0, sizeof(m_szDirectorAgname));
	memset(m_szManagerAgname, 0, sizeof(m_szManagerAgname));
	m_szNormalAgname[0] = 0;

	m_dwMasterID = 0;
	m_szMasterName[0] = 0;

	memset(m_dwDirectorID, 0, sizeof(m_dwDirectorID));
	memset(m_szDirectorName, 0, sizeof(m_szDirectorName));

	memset(m_dwManagerID, 0, sizeof(m_dwManagerID));
	memset(m_szManagerName, 0, sizeof(m_szManagerName));

	m_nMemberNum = sList.MemberCount;
	m_nMemberPointSize = m_nMemberNum + defTONG_INIT_MEMBER_SIZE;
	m_psMember = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize];
	memset(m_psMember, 0, sizeof(STONG_MEMBER) * m_nMemberPointSize);
	m_szMaleAgname[0] = 0;
	m_szFemaleAgname[0] = 0;
	m_nMasterSex = 0;
	memset(m_nDirectorSex, 0, sizeof(m_nDirectorSex));
	memset(m_nManagerSex, 0, sizeof(m_nManagerSex));
	m_nMasterJoinTm = 0;
	memset(m_nDirectorJoinTm, 0 , sizeof(m_nDirectorJoinTm));
	memset(m_nManagerJoinTm, 0 , sizeof(m_nManagerJoinTm));
	m_nTongJiyuParam = 0;
	m_szTongJiyuNotify[0] = 0;
	m_nMasterSaveEff = 0;
	m_nDirectorSaveEff[0] = 0;
	m_nManagerSaveEff[0] = 0;
	m_dwTotalEff = 0;
	m_nSaveEff = 0;
	m_bRecruit = 1;
	m_nTongParam = 0;
}

CTongControl::~CTongControl()
{
	if (m_psMember)
		delete []m_psMember;
}

BOOL	CTongControl::AddMember(char *lpszPlayerName, int nSex, int nJoinTm, int nSaveEff)
{
	// ÕâÖÖÇé¿ö²»Ó¦¸Ã·¢Éú
	if (m_nMemberPointSize <= 0 || !m_psMember)
		return FALSE;
	if (!lpszPlayerName || strlen(lpszPlayerName) >= defTONG_NAME_LENGTH_32)
		return FALSE;
	int i;
	// Ñ°ÕÒ¿ÕÎ»
	for ( i = 0; i < m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID == 0)
			break;
	}
	// Ã»ÕÒ×Å£¬ÂúÁË£¬¿ª±ÙÒ»¿é¸ü´óµÄÄÚ´æ
	if (i >= m_nMemberPointSize)
	{
		i = m_nMemberPointSize;
		// ÔÝÊ±±¸·ÝÒ»ÏÂ
		STONG_MEMBER	*pTemp;
		pTemp = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize];
		memcpy(pTemp, m_psMember, sizeof(STONG_MEMBER) * m_nMemberPointSize);

		// ·ÖÅä¸ü´óµÄÄÚ´æ
		delete []m_psMember;
		m_psMember = (STONG_MEMBER*)new STONG_MEMBER[m_nMemberPointSize + defTONG_MEMBER_SIZE_ADD];
		memcpy(m_psMember, pTemp, sizeof(STONG_MEMBER) * m_nMemberPointSize);
		memset(&m_psMember[m_nMemberPointSize], 0, sizeof(STONG_MEMBER) * defTONG_MEMBER_SIZE_ADD);
		m_nMemberPointSize += defTONG_MEMBER_SIZE_ADD;

		delete []pTemp;
	}

	// Ôö¼ÓÒ»¸ö³ÉÔ±
	strcpy(m_psMember[i].m_szName, lpszPlayerName);
	m_psMember[i].m_dwNameID = g_String2Id(lpszPlayerName);
	m_psMember[i].m_nSex = nSex;
	m_psMember[i].m_nMemberJoinTm = nJoinTm;
	m_psMember[i].m_nMemberSaveEff = nSaveEff;
	m_nMemberNum++;

	return TRUE;
}

BOOL	CTongControl::GetTongHeadInfo(STONG_HEAD_INFO_SYNC *pInfo)
{
	if (!pInfo)
		return FALSE;

	int		i, j;

	pInfo->ProtocolFamily = pf_tong;
	pInfo->ProtocolID = enumS2C_TONG_HEAD_INFO;
	pInfo->m_dwMoney = m_dwMoney;
	pInfo->m_nCredit = m_nCredit;
	pInfo->m_btCamp = m_nCamp;
	pInfo->m_btLevel = m_btLevel;
	pInfo->m_btManagerNum = m_nManagerNum;
	pInfo->m_dwMemberNum = m_nMemberNum;
	strcpy(pInfo->m_szTongName, m_szName);
	pInfo->m_nTongJiyuParam = m_nTongJiyuParam;
	strcpy(pInfo->m_szTongJiyuNotify, m_szTongJiyuNotify);
	pInfo->m_dwTotalEff = m_dwTotalEff;
	pInfo->m_nSaveEff = m_nSaveEff;
	pInfo->m_bRecruit = m_bRecruit;
	pInfo->m_nTongParam = m_nTongParam;

	pInfo->m_sMember[0].m_btFigure = enumTONG_FIGURE_MASTER;
	pInfo->m_sMember[0].m_btPos = 0;
	pInfo->m_sMember[0].m_nMasterEff = m_nMasterSaveEff;
	GetMasterAgname(pInfo->m_sMember[0].m_szAgname);
	strcpy(pInfo->m_sMember[0].m_szName, this->m_szMasterName);
	pInfo->m_sMember[0].m_nMasterJoinTm = m_nMasterJoinTm;
	CNetConnectDup conndup;
	CNetConnectDup tongconndup;
	DWORD nameid = 0;
	unsigned long param = 0;
	if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pInfo->m_sMember[0].m_szName), &conndup, NULL, &nameid, &param))
	{
		tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
		if (tongconndup.IsValid())
			pInfo->m_sMember[0].m_bOnline = TRUE;
		else
			pInfo->m_sMember[0].m_bOnline = FALSE;
		tongconndup.Clearup();
	}
	else
	{
		pInfo->m_sMember[0].m_bOnline = FALSE;
	}
	conndup.Clearup();

	pInfo->m_btDirectorNum = 0;
	for (i = 0, j = 1; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (!m_szDirectorName[i][0])
			continue;
		pInfo->m_sMember[j].m_btFigure = enumTONG_FIGURE_DIRECTOR;
		pInfo->m_sMember[j].m_btPos = i;
		GetDirectorAgname(pInfo->m_sMember[j].m_szAgname, i);
		strcpy(pInfo->m_sMember[j].m_szName, this->m_szDirectorName[i]);
		pInfo->m_sMember[j].m_nDirectorEff[i] = this->m_nDirectorSaveEff[i];
		pInfo->m_sMember[j].m_nDirectorJoinTm[i] =  this->m_nDirectorJoinTm[i];
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pInfo->m_sMember[j].m_szName), &conndup, NULL, &nameid, &param))
		{
			tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
				pInfo->m_sMember[j].m_bOnline = TRUE;
			else
				pInfo->m_sMember[j].m_bOnline = FALSE;
			tongconndup.Clearup();
		}
		else
		{
			pInfo->m_sMember[j].m_bOnline = FALSE;
		}
		conndup.Clearup();
		j++;
		pInfo->m_btDirectorNum++;
	}

	pInfo->m_wLength = sizeof(STONG_HEAD_INFO_SYNC) - sizeof(pInfo->m_sMember) + sizeof(STONG_ONE_LEADER_INFO) * (1 + pInfo->m_btDirectorNum);

	return TRUE;
}

BOOL	CTongControl::GetTongManagerInfo(
			STONG_GET_MANAGER_INFO_COMMAND *pApply,
			STONG_MANAGER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (pApply->m_nParam3 <= 0)
		return FALSE;

	pInfo->ProtocolFamily	= pf_tong;
	pInfo->ProtocolID		= enumS2C_TONG_MANAGER_INFO;
	pInfo->m_dwParam		= pApply->m_dwParam;
	pInfo->m_dwMoney		= m_dwMoney;
	pInfo->m_nCredit		= m_nCredit;
	pInfo->m_btCamp			= m_nCamp;
	pInfo->m_btLevel		= m_btLevel;
	pInfo->m_btDirectorNum	= m_nDirectorNum;
	pInfo->m_btManagerNum	= m_nManagerNum;
	pInfo->m_dwMemberNum	= m_nMemberNum;
	pInfo->m_btStartNo		= pApply->m_nParam2;
	strcpy(pInfo->m_szTongName, m_szName);

	int		nNeedNum, nStartNum, i, j;

	nStartNum = pApply->m_nParam2;
	nNeedNum = pApply->m_nParam3;
	if (nNeedNum > defTONG_ONE_PAGE_MAX_NUM)
		nNeedNum = defTONG_ONE_PAGE_MAX_NUM;

	pInfo->m_btCurNum = 0;
	// ÕÒ¿ªÊ¼Î»ÖÃ
	for (i = 0, j = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (j >= nStartNum)
			break;
		if (m_szManagerName[i][0])
			j++;
	}
	if (i >= defTONG_MAX_MANAGER || j < nStartNum)
		return FALSE;

	// ¿ªÊ¼¼ÇÂ¼Êý¾Ý
	for (; i < defTONG_MAX_MANAGER; i++)
	{
		if (pInfo->m_btCurNum >= nNeedNum)
			break;
		if (!m_szManagerName[i][0])
			continue;

		pInfo->m_sMember[pInfo->m_btCurNum].m_btFigure = enumTONG_FIGURE_MANAGER;
		pInfo->m_sMember[pInfo->m_btCurNum].m_btPos = i;
		GetManagerAgname(pInfo->m_sMember[pInfo->m_btCurNum].m_szAgname, i);
		strcpy(pInfo->m_sMember[pInfo->m_btCurNum].m_szName, m_szManagerName[i]);
		pInfo->m_sMember[pInfo->m_btCurNum].m_nManagerEff[i] = m_nManagerSaveEff[i];
		pInfo->m_sMember[pInfo->m_btCurNum].m_nManagerJoinTm[i] = m_nManagerJoinTm[i];
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(pInfo->m_sMember[pInfo->m_btCurNum].m_szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
				pInfo->m_sMember[pInfo->m_btCurNum].m_bOnline = TRUE;
			else
				pInfo->m_sMember[pInfo->m_btCurNum].m_bOnline = FALSE;
			tongconndup.Clearup();
		}
		else
		{
			pInfo->m_sMember[pInfo->m_btCurNum].m_bOnline = FALSE;
		}
		conndup.Clearup();
		pInfo->m_btCurNum++;
	}
	if (pInfo->m_btCurNum == 0)
		return FALSE;

	pInfo->m_wLength = sizeof(STONG_MANAGER_INFO_SYNC) - sizeof(STONG_ONE_LEADER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pInfo->m_btCurNum);

	return TRUE;
}

BOOL	CTongControl::GetTongMemberInfo(
			STONG_GET_MEMBER_INFO_COMMAND *pApply,
			STONG_MEMBER_INFO_SYNC *pInfo)
{
	if (!pApply || !pInfo)
		return FALSE;
	if (pApply->m_nParam3 <= 0)
		return FALSE;
	if (!m_psMember || m_nMemberPointSize <= 0)
		return FALSE;
	
	pInfo->ProtocolFamily	= pf_tong;
	pInfo->ProtocolID		= enumS2C_TONG_MEMBER_INFO;
	pInfo->m_dwParam		= pApply->m_dwParam;
	pInfo->m_dwMoney		= m_dwMoney;
	pInfo->m_nCredit		= m_nCredit;
	pInfo->m_btCamp			= m_nCamp;
	pInfo->m_btLevel		= m_btLevel;
	pInfo->m_btDirectorNum	= m_nDirectorNum;
	pInfo->m_btManagerNum	= m_nManagerNum;
	pInfo->m_dwMemberNum	= m_nMemberNum;
	pInfo->m_btStartNo		= pApply->m_nParam2;
	strcpy(pInfo->m_szTongName, m_szName);
	if (m_szNormalAgname[0])
		strcpy(pInfo->m_szAgname, m_szNormalAgname);
	else
		strcpy(pInfo->m_szAgname, defTONG_MEMBER_AGNAME);
	strcpy(pInfo->m_szMaleAgname, m_szMaleAgname);
	strcpy(pInfo->m_szFemaleAgname, m_szFemaleAgname);

	int		nNeedNum, nStartNum, i, j;

	nStartNum = pApply->m_nParam2;
	nNeedNum = pApply->m_nParam3;
	if (nNeedNum > defTONG_ONE_PAGE_MAX_NUM)
		nNeedNum = defTONG_ONE_PAGE_MAX_NUM;

	pInfo->m_btCurNum = 0;
	// ÕÒ¿ªÊ¼Î»ÖÃ
	for (i = 0, j = 0; i < m_nMemberPointSize; i++)
	{
		if (j >= nStartNum)
			break;
		if (m_psMember[i].m_szName[0])
			j++;
	}
	if (i >= m_nMemberPointSize || j < nStartNum)
		return FALSE;

	// ¿ªÊ¼¼ÇÂ¼Êý¾Ý
	for (; i < m_nMemberPointSize; i++)
	{
		if (pInfo->m_btCurNum >= nNeedNum)
			break;
		if (!m_psMember[i].m_szName[0])
			continue;
		pInfo->m_sMember[pInfo->m_btCurNum].m_btSex = m_psMember[i].m_nSex;
		strcpy(pInfo->m_sMember[pInfo->m_btCurNum].m_szName, m_psMember[i].m_szName);
		pInfo->m_sMember[pInfo->m_btCurNum].m_nMemberEff = m_psMember[i].m_nMemberSaveEff;
		pInfo->m_sMember[pInfo->m_btCurNum].m_nMemberJoinTm = m_psMember[i].m_nMemberJoinTm;
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
				pInfo->m_sMember[pInfo->m_btCurNum].m_bOnline = TRUE;
			else
				pInfo->m_sMember[pInfo->m_btCurNum].m_bOnline = FALSE;
			tongconndup.Clearup();
		}
		else
		{
			pInfo->m_sMember[pInfo->m_btCurNum].m_bOnline = FALSE;
		}
		conndup.Clearup();
		pInfo->m_btCurNum++;
	}
	if (pInfo->m_btCurNum == 0)
		return FALSE;

	pInfo->m_wLength = sizeof(STONG_MEMBER_INFO_SYNC) - sizeof(STONG_ONE_MEMBER_INFO) * (defTONG_ONE_PAGE_MAX_NUM - pInfo->m_btCurNum);

	return TRUE;
}

BOOL	CTongControl::Instate(STONG_INSTATE_COMMAND *pInstate, STONG_INSTATE_SYNC *pSync)
{
	if (!pInstate || !pSync)
		return FALSE;
	if (pInstate->m_btCurFigure == pInstate->m_btNewFigure && pInstate->m_btCurPos == pInstate->m_btNewPos)
		return FALSE;

	int		i, nOldPos, nNewPos;
	DWORD	dwNameID;
	char	szName[32];

	memcpy(szName, pInstate->m_szName, sizeof(pInstate->m_szName));
	szName[31] = 0;
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_INSTATE;
	pSync->m_btOldFigure	= pInstate->m_btCurFigure;
	pSync->m_btOldPos		= 0;
	pSync->m_btNewFigure	= pInstate->m_btNewFigure;
	pSync->m_btNewPos		= 0;
	pSync->m_dwParam		= pInstate->m_dwParam;
	pSync->m_btSuccessFlag	= 0;
	pSync->m_dwTongNameID	= pInstate->m_dwTongNameID;
	pSync->m_szAgname[0]		= 0;
	strcpy(pSync->m_szName, szName);
	int nPlayerSex = 0, nSaveEff = 0, nJoinTm = 0;
	if (pInstate->m_btCurFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		nOldPos = i;
		nPlayerSex = m_nDirectorSex[nOldPos];
		nJoinTm = m_nDirectorJoinTm[nOldPos];
		nSaveEff = m_nDirectorSaveEff[nOldPos];
		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_MANAGER:
			for (i = 0; i < defTONG_MAX_MANAGER; i++)
			{
				if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
					break;
			}
			if (i >= defTONG_MAX_MANAGER)
				return FALSE;
			nNewPos = i;

			strcpy(m_szManagerName[nNewPos], szName);
			m_dwManagerID[nNewPos] = dwNameID;
			m_nManagerSex[nNewPos] = nPlayerSex;
			m_nManagerJoinTm[nNewPos] = nJoinTm;
			m_nManagerSaveEff[nNewPos] = nSaveEff;
			m_szDirectorName[nOldPos][0] = 0;
			m_dwDirectorID[nOldPos] = 0;
			m_nDirectorJoinTm[nOldPos] = 0;
			m_nDirectorSaveEff[nOldPos] = 0;
			m_nDirectorNum--;
			m_nManagerNum++;
			if (m_szManagerAgname[nNewPos][0])
				memset(m_szManagerAgname[nNewPos], 0, sizeof(m_szManagerAgname[nNewPos]));
			strcpy(pSync->m_szAgname, m_szManagerAgname[nNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName, nPlayerSex, nJoinTm, nSaveEff))
				return FALSE;
			m_szDirectorName[nOldPos][0] = 0;
			m_dwDirectorID[nOldPos] = 0;
			m_nDirectorJoinTm[nOldPos] = 0;
			m_nDirectorSaveEff[nOldPos] = 0;
			m_nDirectorNum--;
			if (m_szNormalAgname[0])
				memset(m_szNormalAgname, 0, sizeof(m_szNormalAgname));
			GetMemberAgname(pSync->m_szAgname, nPlayerSex);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nOldPos = i;
		nJoinTm = m_nManagerJoinTm[nOldPos];
		nPlayerSex = m_nManagerSex[nOldPos];
		nSaveEff = m_nManagerSaveEff[nOldPos];
		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
			{
				if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
					break;
			}
			if (i >= defTONG_MAX_DIRECTOR)
				return FALSE;
			nNewPos = i;

			strcpy(m_szDirectorName[nNewPos], szName);
			m_dwDirectorID[nNewPos] = dwNameID;
			m_nDirectorSex[nNewPos] = nPlayerSex;
			m_nDirectorSaveEff[nNewPos] = nSaveEff;
			m_nDirectorJoinTm[nNewPos] = nJoinTm;
			m_szManagerName[nOldPos][0] = 0;
			m_dwManagerID[nOldPos] = 0;
			m_nManagerJoinTm[nOldPos] = 0;
			m_nManagerSaveEff[nOldPos] = 0;
			m_nDirectorNum++;
			m_nManagerNum--;
			if (m_szDirectorAgname[nNewPos][0])
				memset(m_szDirectorAgname[nNewPos], 0, sizeof(m_szDirectorAgname[nNewPos]));
			strcpy(pSync->m_szAgname, m_szDirectorAgname[nNewPos]);
			break;
		case enumTONG_FIGURE_MEMBER:
			if (!AddMember(szName, nPlayerSex, nJoinTm, nSaveEff))
				return FALSE;
			m_szManagerName[nOldPos][0] = 0;
			m_dwManagerID[nOldPos] = 0;
			m_nManagerJoinTm[nOldPos] = 0;
			m_nManagerSaveEff[nOldPos] = 0;
			m_nManagerNum--;
			if (m_szNormalAgname[0])
				memset(m_szNormalAgname, 0, sizeof(m_szNormalAgname));
			GetMemberAgname(pSync->m_szAgname, nPlayerSex);
			break;
		default:
			return FALSE;
		}
	}
	else if (pInstate->m_btCurFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;
		int		nPos = -1;

		for (int i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID != dwNameID)
				continue;
			nPos = i;
			break;
		}
		if (nPos < 0)
			return FALSE;
		nOldPos = 0;
		nPlayerSex = this->m_psMember[nPos].m_nSex;
		nJoinTm = this->m_psMember[nPos].m_nMemberJoinTm;
		nSaveEff = this->m_psMember[nPos].m_nMemberSaveEff;

		switch (pInstate->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
			{
				if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
					break;
			}
			if (i >= defTONG_MAX_DIRECTOR)
				return FALSE;
			nNewPos = i;
			
			strcpy(m_szDirectorName[nNewPos], szName);
			m_dwDirectorID[nNewPos] = dwNameID;
			m_nDirectorSex[nNewPos] = nPlayerSex;
			m_nDirectorJoinTm[nNewPos] = nJoinTm;
			m_nDirectorSaveEff[nNewPos] = nSaveEff;
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			this->m_psMember[nPos].m_nMemberJoinTm = 0;
			this->m_psMember[nPos].m_nMemberSaveEff = 0;
			m_nDirectorNum++;
			m_nMemberNum--;
			if (m_szDirectorAgname[nNewPos][0])
				memset(m_szDirectorAgname[nNewPos], 0, sizeof(m_szDirectorAgname[nNewPos]));
			strcpy(pSync->m_szAgname, m_szDirectorAgname[nNewPos]);
			break;
		case enumTONG_FIGURE_MANAGER:
			for (i = 0; i < defTONG_MAX_MANAGER; i++)
			{
				if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
					break;
			}
			if (i >= defTONG_MAX_MANAGER)
				return FALSE;
			nNewPos = i;

			strcpy(m_szManagerName[nNewPos], szName);
			m_dwManagerID[nNewPos] = dwNameID;
			m_nManagerSex[nNewPos] = nPlayerSex;
			m_nManagerJoinTm[nNewPos] = nJoinTm;
			m_nManagerSaveEff[nNewPos] = nSaveEff;
			this->m_psMember[nPos].m_szName[0] = 0;
			this->m_psMember[nPos].m_dwNameID = 0;
			this->m_psMember[nPos].m_nMemberJoinTm = 0;
			this->m_psMember[nPos].m_nMemberSaveEff = 0;
			m_nManagerNum++;
			m_nMemberNum--;
			if (m_szManagerAgname[nNewPos][0])
				memset(m_szManagerAgname[nNewPos], 0, sizeof(m_szManagerAgname[nNewPos]));
			strcpy(pSync->m_szAgname, m_szManagerAgname[nNewPos]);
			break;
		default:
			return FALSE;
		}
	}
	pSync->m_btSuccessFlag = 1;
	pSync->m_btOldPos = nOldPos;
	pSync->m_btNewPos = nNewPos;

	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_INSTATED_SYNC	sSync;

				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_INSTATED;
				sSync.m_btFigure = pInstate->m_btNewFigure;
				sSync.m_btPos = nNewPos;
				sSync.m_dwParam = param;
				sSync.m_nSaveEff = nSaveEff;
				strcpy(sSync.m_szName, szName);
				sSync.m_nJoinTm = nJoinTm;
				switch (pInstate->m_btNewFigure)
				{
				case enumTONG_FIGURE_DIRECTOR:
					strcpy(sSync.m_szAgname, m_szDirectorAgname[nNewPos]);
					break;
				case enumTONG_FIGURE_MANAGER:
					strcpy(sSync.m_szAgname, m_szManagerAgname[nNewPos]);
					break;
				case enumTONG_FIGURE_MEMBER:
					GetMemberAgname(sSync.m_szAgname, nPlayerSex);
					break;
				}

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
			else
			{
				//tong is not connect
			}

		}
		else
		{
			//not find
		}
	}}

	// Ïò°ï»áÆµµÀ·¢ÏûÏ¢£¬Ä³ÈË±»ÈÎÃü
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		switch (pSync->m_btNewFigure)
		{
		case enumTONG_FIGURE_DIRECTOR:
			sprintf(szMsg, "%s ®­îc bæ nhiÖm lµm %s ", szName, defTONG_DIRECTOR_AGNAME);
			break;
		case enumTONG_FIGURE_MANAGER:
			if (pSync->m_btNewFigure > pSync->m_btOldFigure)
				sprintf(szMsg, "%s ®­îc bæ nhiÖm lµm %s ", szName, defTONG_MANAGER_AGNAME);
			else
				sprintf(szMsg, "%s bÞ b·i nhiÖm trë thµnh %s ", szName, defTONG_MANAGER_AGNAME);
			break;
		case enumTONG_FIGURE_MEMBER:
			sprintf(szMsg, "%s bÞ b·i nhiÖm trë thµnh %s ", szName, defTONG_MEMBER_AGNAME);
			break;
		}
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	}

	// ´æÅÌ£¬Êý¾Ý±£´æÖÁÊý¾Ý¿â
	TMemberStruct	sMember;
	sMember.MemberClass = (TONG_MEMBER_FIGURE)pInstate->m_btNewFigure;
	sMember.nTitleIndex = nNewPos;
	sMember.nSex = nPlayerSex;
	sMember.nJoinTm = nJoinTm;
	sMember.nSaveEff = nSaveEff;
	strcpy(sMember.szTong, this->m_szName);
	strcpy(sMember.szName, szName);
	g_cTongDB.ChangeMember(sMember);
	return TRUE;
}

BOOL	CTongControl::Kick(STONG_KICK_COMMAND *pKick, STONG_KICK_SYNC *pSync)
{
	if (!pKick || !pSync)
		return FALSE;

	char	szName[32];
	DWORD	dwNameID;
	int		nKickPos;
	memcpy(szName, pKick->m_szName, sizeof(pKick->m_szName));
	szName[31] = 0;
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_KICK;
	pSync->m_btFigure		= pKick->m_btFigure;
	pSync->m_btPos			= pKick->m_btPos;
	pSync->m_dwParam		= pKick->m_dwParam;
	pSync->m_dwTongNameID	= pKick->m_dwTongNameID;
	pSync->m_btSuccessFlag	= 0;
	strcpy(pSync->m_szName, szName);

	if (pKick->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		if (pKick->m_btPos >= defTONG_MAX_MANAGER)
			return FALSE;
		int i;
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(m_szManagerName[i], szName) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nKickPos = i;
		this->m_szManagerName[i][0] = 0;
		this->m_dwManagerID[i] = 0;
		this->m_nManagerJoinTm[i] = 0;
		this->m_nManagerSaveEff[i] = 0;
		this->m_nManagerNum--;
	}
	else if (pKick->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember)
			return FALSE;

		int		nPos = -1;
		DWORD	dwNameID = g_String2Id(szName);
		if (dwNameID == 0)
			return FALSE;

		for (int i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID != dwNameID)
				continue;
			nPos = i;
			break;
		}
		if (nPos < 0)
			return FALSE;

		if (!m_psMember[nPos].m_szName[0] ||
			strcmp(m_psMember[nPos].m_szName, szName) != 0)
			return FALSE;
		m_psMember[nPos].m_szName[0] = 0;
		m_psMember[nPos].m_dwNameID = 0;
		m_psMember[nPos].m_nMemberJoinTm = 0;
		m_psMember[nPos].m_nMemberSaveEff;
		m_nMemberNum--;
		nKickPos = 0;
	}
	else
	{
		return FALSE;
	}
	pSync->m_btSuccessFlag = 1;
	pSync->m_btPos = nKickPos;


	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_KICKED_SYNC	sSync;

				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_KICKED;
				sSync.m_btFigure = pKick->m_btFigure;
				sSync.m_btPos = nKickPos;
				sSync.m_dwParam = param;
				strcpy(sSync.m_szName, szName);

				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
			else
			{
				//tong is not connect
			}

		}
		else
		{
			//not find
		}
	}}

	// ´æÅÌ
	g_cTongDB.DelMember(szName);

	// Ïò°ï»áÆµµÀ·¢ÏûÏ¢£¬Ä³ÈË±»Ìß³ö°ï»á
	char	szMsg[128];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		sprintf(szMsg, "%s ®· bÞ trôc xuÊt khái bæn bang", szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	}

	return TRUE;
}

BOOL	CTongControl::Leave(STONG_LEAVE_COMMAND *pLeave, STONG_LEAVE_SYNC *pSync)
{
	if (!pLeave || !pSync)
		return FALSE;

	char	szName[32];
	memcpy(szName, pLeave->m_szName, sizeof(pLeave->m_szName));
	szName[31] = 0;

	pSync->ProtocolFamily	= pf_tong;
	pSync->ProtocolID		= enumS2C_TONG_LEAVE;
	pSync->m_dwParam		= pLeave->m_dwParam;
	pSync->m_btSuccessFlag	= 0;
	strcpy(pSync->m_szName, szName);

	switch (pLeave->m_btFigure)
	{
	case enumTONG_FIGURE_MEMBER:
		{
			if (!m_psMember)
				break;
			int		nPos = -1;
			DWORD	dwNameID = g_String2Id(szName);
			if (dwNameID == 0)
				return FALSE;
			for (int i = 0; i < m_nMemberPointSize; i++)
			{
				if (m_psMember[i].m_dwNameID != dwNameID)
					continue;
				nPos = i;
				break;
			}
			if (nPos < 0)
				return FALSE;
			if (!m_psMember[nPos].m_szName[0] ||
				strcmp(m_psMember[nPos].m_szName, szName) != 0)
				return FALSE;

			m_psMember[nPos].m_szName[0] = 0;
			m_psMember[nPos].m_dwNameID = 0;
			m_psMember[nPos].m_nMemberJoinTm = 0;
			m_psMember[nPos].m_nMemberSaveEff = 0;
			m_nMemberNum--;
		}
		break;
	case enumTONG_FIGURE_MANAGER:
		{
			if (pLeave->m_btPos >= 0 && pLeave->m_btPos < defTONG_MAX_MANAGER)
			{
				if (strcmp(m_szManagerName[pLeave->m_btPos], szName) == 0)
				{
					m_szManagerName[pLeave->m_btPos][0] = 0;
					m_dwManagerID[pLeave->m_btPos] = 0;
					m_nManagerJoinTm[pLeave->m_btPos] = 0;
					m_nManagerSaveEff[pLeave->m_btPos] = 0;
					m_nManagerNum--;
					break;
				}
			}

			int		nPos = -1;
			DWORD	dwNameID = g_String2Id(szName);
			if (dwNameID == 0)
				return FALSE;
			for (int i = 0; i < defTONG_MAX_MANAGER; i++)
			{
				if (m_dwManagerID[i] != dwNameID)
					continue;
				nPos = i;
				break;
			}
			if (nPos < 0)
				return FALSE;
			if (!m_szManagerName[nPos][0] ||
				strcmp(szName, m_szManagerName[nPos]) != 0)
				return FALSE;
			m_szManagerName[nPos][0] = 0;
			m_dwManagerID[nPos] = 0;
			m_nManagerJoinTm[nPos] = 0;
			m_nManagerSaveEff[pLeave->m_btPos] = 0;
			m_nManagerNum--;
		}
		break;
	case enumTONG_FIGURE_DIRECTOR:
		return FALSE;
	case enumTONG_FIGURE_MASTER:
		return FALSE;
	}
	pSync->m_btSuccessFlag = 1;

	// ´æÅÌ
	g_cTongDB.DelMember(szName);

	// Í¨¹ý°ï»áÆµµÀ·¢²¼ÏûÏ¢£ºÄ³Ä³ÈËÅÑ°ï
	char	szMsg[128];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		sprintf(szMsg, "%s chñ ®éng rêi khái bæn bang", szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	}

	return TRUE;
}

BOOL	CTongControl::AcceptMaster(STONG_ACCEPT_MASTER_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;

	char	szName[32], szOldMaster[32];
	memcpy(szName, pAccept->m_szName, sizeof(pAccept->m_szName));
	szName[31] = 0;
	if (!szName[0])
		return FALSE;
	strcpy(szOldMaster, this->m_szMasterName);
	int nOldMasterSex = this->m_nMasterSex;
	int nOldMasterJoinTm = this->m_nMasterJoinTm;
	int nOldMasterSaveEff = this->m_nMasterSaveEff;

	// ÄÜÁ¦²»¹»
	if (pAccept->m_btAcceptFalg == 0)
	{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_CHANGE_MASTER_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_MASTER_FAIL;
				sFail.m_dwParam			= param;
				sFail.m_btFailID		= 1;
				sFail.m_dwTongNameID	= pAccept->m_dwTongNameID;
				memcpy(sFail.m_szName, szName, sizeof(szName));
				tongconndup.SendPackage((const void *)&sFail, sizeof(sFail));
			}
			else
			{
				//tong is not connect
			}
		}
		else
		{
			//not find
		}

		return FALSE;
	}

	DWORD	dwNameID;
	int		i;

	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;
	int nNewMasterSex = 0;
	int nNewMasterSaveEff = 0;
	int nNewMasterJoinTm;
		
	if (pAccept->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		nNewMasterSex = m_nDirectorSex[i];
		nNewMasterJoinTm = m_nDirectorJoinTm[i];

		if (!AddMember(m_szMasterName, nNewMasterSex, nNewMasterJoinTm, nNewMasterSaveEff))
			return FALSE;
		strcpy(this->m_szMasterName, szName);
		this->m_dwMasterID = dwNameID;
		this->m_nMasterSex = nNewMasterSex;
		this->m_nMasterJoinTm, nNewMasterJoinTm;
		this->m_nMasterSaveEff = nNewMasterSaveEff;
		m_szDirectorName[i][0] = 0;
		m_dwDirectorID[i] = 0;
		m_nDirectorJoinTm[i] = 0;
		m_nDirectorSaveEff[i] = 0;
		this->m_nDirectorNum--;
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nNewMasterSex = m_nManagerSex[i];
		nNewMasterJoinTm = m_nManagerJoinTm[i];

		if (!AddMember(m_szMasterName, nNewMasterSex, nNewMasterJoinTm, nNewMasterSaveEff))
			return FALSE;
		strcpy(this->m_szMasterName, szName);
		this->m_dwMasterID = dwNameID;
		this->m_nMasterSex = nNewMasterSex;
		this->m_nMasterJoinTm, nNewMasterJoinTm;
		this->m_nMasterSaveEff = nNewMasterSaveEff;
		m_szManagerName[i][0] = 0;
		m_dwManagerID[i] = 0;
		m_nManagerJoinTm[i] = 0;
		m_nManagerSaveEff[i] = 0;
		this->m_nManagerNum--;
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MEMBER)
	{
		if (!m_psMember || m_nMemberPointSize <= 0)
			return FALSE;
		for (i = 0; i < this->m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID &&
				strcmp(szName, m_psMember[i].m_szName) == 0)
				break;
		}
		if (i >= m_nMemberPointSize)
			return FALSE;
		nNewMasterSex = m_psMember[i].m_nSex;
		nNewMasterJoinTm = m_psMember[i].m_nMemberJoinTm;
		nNewMasterSaveEff = m_psMember[i].m_nMemberSaveEff;

		if (!AddMember(m_szMasterName, nNewMasterSex, nNewMasterJoinTm, nNewMasterSaveEff))
			return FALSE;
		strcpy(m_szMasterName, szName);
		m_dwMasterID = dwNameID;
		this->m_nMasterSex = nNewMasterSex;
		this->m_nMasterJoinTm = nNewMasterJoinTm;
		this->m_nMasterSaveEff = nNewMasterSaveEff;
		m_psMember[i].m_szName[0] = 0;
		m_psMember[i].m_dwNameID = 0;
		m_psMember[i].m_nMemberJoinTm = 0;
		m_psMember[i].m_nMemberSaveEff = 0;
		m_nMemberNum--;
	}
	else
	{
		return FALSE;
	}

	STONG_CHANGE_AS_SYNC	sChange;

	sChange.ProtocolFamily	= pf_tong;
	sChange.ProtocolID		= enumS2C_TONG_CHANGE_AS;
	sChange.m_dwTongNameID	= this->m_dwNameID;

	// ¸øÀÏ°ïÖ÷·¢ÏûÏ¢
	DWORD nameid = 0;
	unsigned long param = 0;
	CNetConnectDup conndup1;
	if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(szOldMaster), &conndup1, NULL, &nameid, &param))
	{
		CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup1.GetIP());
		if (tongconndup.IsValid())
		{
			sChange.m_dwParam		= param;
			sChange.m_btFigure		= enumTONG_FIGURE_MEMBER;
			sChange.m_btPos			= 0;
			GetMemberAgname(sChange.m_szAgname, nOldMasterSex);
			strcpy(sChange.m_szName, m_szMasterName);

			tongconndup.SendPackage((const void *)&sChange, sizeof(sChange));
		}
	}

	// ¸øÐÂ°ïÖ÷·¢ÏûÏ¢
	CNetConnectDup conndup2;
	if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup2, NULL, &nameid, &param))
	{
		CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup2.GetIP());
		if (tongconndup.IsValid())
		{
			sChange.m_dwParam		= param;
			sChange.m_btFigure		= enumTONG_FIGURE_MASTER;
			sChange.m_btPos			= 0;
			strcpy(sChange.m_szAgname, this->m_szMasterAgname);
			strcpy(sChange.m_szName, this->m_szMasterName);

			tongconndup.SendPackage((const void *)&sChange, sizeof(sChange));
		}
	}

	STONG_CHANGE_MASTER_SYNC	sMaster;
	sMaster.ProtocolFamily	= pf_tong;
	sMaster.ProtocolID		= enumS2C_TONG_CHANGE_AS;
	sMaster.m_dwTongNameID	= m_dwNameID;
	strcpy(sMaster.m_szName, m_szMasterName);

	g_TongServer.BroadPackage((const void*)&sMaster, sizeof(sMaster));

	// ´æÅÌ
	// ÐÂ°ïÖ÷
	TMemberStruct	sMember1;
	sMember1.MemberClass = enumTONG_FIGURE_MASTER;
	sMember1.nTitleIndex = 0;
	strcpy(sMember1.szTong, this->m_szName);
	strcpy(sMember1.szName, szName);
	sMember1.nSex = nNewMasterSex;
	sMember1.nJoinTm = nNewMasterJoinTm;
	sMember1.nSaveEff = nNewMasterSaveEff;
	g_cTongDB.ChangeMember(sMember1);

	// ÀÏ°ïÖ÷
	TMemberStruct	sMember2;
	sMember2.MemberClass = enumTONG_FIGURE_MEMBER;
	sMember2.nTitleIndex = 0;
	strcpy(sMember2.szTong, this->m_szName);
	strcpy(sMember2.szName, szOldMaster);
	sMember2.nSex = nOldMasterSex;
	sMember2.nJoinTm = nOldMasterJoinTm;
	sMember2.nSaveEff = nOldMasterSaveEff;
	g_cTongDB.ChangeMember(sMember2);

	// ÁÄÌìÆµµÀ·¢ÏûÏ¢
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);

	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		sprintf(szMsg, "Bang chñ %s ®· nh­êng chøc vÞ bang chñ l¹i cho %s ", szOldMaster, szName);
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	}

	return TRUE;
}

BOOL	CTongControl::GetLoginData(STONG_GET_LOGIN_DATA_COMMAND *pLogin, STONG_LOGIN_DATA_SYNC *pSync)
{
	if (!pLogin || !pSync)
		return FALSE;

	int		i;
	DWORD	dwNameID = g_String2Id(pLogin->m_szName);
	if (dwNameID == 0)
		return FALSE;

	pSync->m_btCamp		= this->m_nCamp;
	pSync->m_btLevel	= this->m_btLevel;
	pSync->m_dwMemberNum	= this->m_nMemberNum;
	pSync->m_btManagerNum	= this->m_nManagerNum;
	pSync->m_btDirectorNum	= this->m_nDirectorNum;
	pSync->m_dwTotalEff = this->m_dwTotalEff;
	pSync->m_bRecruit = this->m_bRecruit;
	pSync->m_nTongParam = this->m_nTongParam;
	strcpy(pSync->m_szTongName, this->m_szName);
	strcpy(pSync->m_szMaster, this->m_szMasterName);
	strcpy(pSync->m_szName, pLogin->m_szName);
	pSync->m_nTongJiyuParam = this->m_nTongJiyuParam;
	strcpy(pSync->m_szTongJiyuNotify, this->m_szTongJiyuNotify);
	pSync->m_nMoney		= this->m_dwMoney;
	if (this->m_dwMasterID == dwNameID)
	{
		pSync->m_btFigure	= enumTONG_FIGURE_MASTER;
		pSync->m_btPos		= 0;
		pSync->m_btFlag		= 1;
		GetMasterAgname(pSync->m_szAgname);
		pSync->m_nJoinTm = this->m_nMasterJoinTm;
		pSync->m_nSaveEff = this->m_nMasterSaveEff;
		return TRUE;
	}

	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (this->m_dwDirectorID[i] == dwNameID)
		{
			pSync->m_btFigure	= enumTONG_FIGURE_DIRECTOR;
			pSync->m_btPos		= i;
			pSync->m_btFlag		= 1;
			GetDirectorAgname(pSync->m_szAgname, i);
			pSync->m_nJoinTm = this->m_nDirectorJoinTm[i];
			pSync->m_nSaveEff = this->m_nDirectorSaveEff[i];
			return TRUE;
		}
	}

	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (this->m_dwManagerID[i] == dwNameID)
		{
			pSync->m_btFigure	= enumTONG_FIGURE_MANAGER;
			pSync->m_btPos		= i;
			pSync->m_btFlag		= 1;
			GetManagerAgname(pSync->m_szAgname, i);
			pSync->m_nJoinTm = this->m_nManagerJoinTm[i];
			pSync->m_nSaveEff = this->m_nManagerSaveEff[i];
			return TRUE;
		}
	}

	if (this->m_psMember)
	{
		for (i = 0; i < this->m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				pSync->m_btFigure	= enumTONG_FIGURE_MEMBER;
				pSync->m_btPos		= 0;
				pSync->m_btFlag		= 1;
				GetMemberAgname(pSync->m_szAgname, pLogin->m_nSex);
				pSync->m_nJoinTm = this->m_psMember[i].m_nMemberJoinTm;
				pSync->m_nSaveEff = this->m_psMember[i].m_nMemberSaveEff;
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL	CTongControl::SearchOne(char *lpszName, STONG_ONE_LEADER_INFO *pInfo)
{
	if (!lpszName || !lpszName[0] || !pInfo)
		return FALSE;

	int		i;
	DWORD	dwNameID = g_String2Id(lpszName);
	if (dwNameID == 0)
		return FALSE;

	if (m_dwMasterID == dwNameID)
	{
		pInfo->m_btFigure	= enumTONG_FIGURE_MASTER;
		pInfo->m_btPos		= 0;
		strcpy(pInfo->m_szAgname, m_szMasterAgname);
		strcpy(pInfo->m_szName, m_szMasterName);
		return TRUE;
	}

	for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
	{
		if (m_dwDirectorID[i] == dwNameID)
		{
			pInfo->m_btFigure	= enumTONG_FIGURE_DIRECTOR;
			pInfo->m_btPos		= i;
			strcpy(pInfo->m_szAgname, m_szDirectorAgname[i]);
			strcpy(pInfo->m_szName, m_szDirectorName[i]);
			return TRUE;
		}
	}

	for (i = 0; i < defTONG_MAX_MANAGER; i++)
	{
		if (m_dwManagerID[i] == dwNameID)
		{
			pInfo->m_btFigure	= enumTONG_FIGURE_MANAGER;
			pInfo->m_btPos		= i;
			strcpy(pInfo->m_szAgname, m_szManagerAgname[i]);
			strcpy(pInfo->m_szName, m_szManagerName[i]);
			return TRUE;
		}
	}

	if (m_psMember)
	{
		for (i = 0; i < m_nMemberPointSize; i++)
		{
			if (m_psMember[i].m_dwNameID == dwNameID)
			{
				pInfo->m_btFigure	= enumTONG_FIGURE_MEMBER;
				pInfo->m_btPos		= 0;
				strcpy(pInfo->m_szAgname, m_szNormalAgname);
				strcpy(pInfo->m_szName, m_psMember[i].m_szName);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CTongControl::DBInstate( char *lpszPlayerName, BYTE nSite )
{
	if (!lpszPlayerName[0] || nSite > 4)
		return FALSE;

	int		i, nOldPos, nNewPos;
	DWORD	dwNameID;
	char	szName[32];
	memcpy(szName, lpszPlayerName, sizeof(lpszPlayerName));
	szName[31] = 0;
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;

	if (!m_psMember)
		return FALSE;
	int		nPos = -1;

	for (i = 0; i < m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID != dwNameID)
			continue;
		nPos = i;
		break;
	}
	if (nPos < 0)
		return FALSE;
	nOldPos = i;
	int nPlayerSex = this->m_psMember[nPos].m_nSex;
	int nJoinTm = this->m_psMember[nPos].m_nMemberJoinTm;

	switch (nSite)
	{
	case enumTONG_FIGURE_DIRECTOR:
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == 0 && !m_szDirectorName[i][0])
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;
		nNewPos = i;

		strcpy(m_szDirectorName[nNewPos], szName);
		m_dwDirectorID[nNewPos] = dwNameID;
		m_nDirectorSex[nNewPos] = nPlayerSex;
		m_nDirectorJoinTm[nNewPos] = nJoinTm;
		this->m_psMember[nPos].m_szName[0] = 0;
		this->m_psMember[nPos].m_dwNameID = 0;
		m_nDirectorNum++;
		m_nMemberNum--;
		break;
	case enumTONG_FIGURE_MANAGER:
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == 0 && !m_szManagerName[i][0])
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;
		nNewPos = i;

		strcpy(m_szManagerName[nNewPos], szName);
		m_dwManagerID[nNewPos] = dwNameID;
		m_nManagerSex[nNewPos] = nPlayerSex;
		m_nManagerJoinTm[nNewPos] = nJoinTm;
		this->m_psMember[nPos].m_szName[0] = 0;
		this->m_psMember[nPos].m_dwNameID = 0;
		m_nManagerNum++;
		m_nMemberNum--;
		break;
	default:
		return FALSE;
	}


	// ´æÅÌ£¬Êý¾Ý±£´æÖÁÊý¾Ý¿â
	TMemberStruct	sMember;
	if (nSite == enumTONG_FIGURE_DIRECTOR)
	{
		sMember.MemberClass = enumTONG_FIGURE_DIRECTOR;
	} 
	else if (nSite == enumTONG_FIGURE_MANAGER)
	{
		sMember.MemberClass = enumTONG_FIGURE_MANAGER;
	}
	sMember.nTitleIndex = nNewPos;
	strcpy(sMember.szTong, this->m_szName);
	sMember.nSex = nPlayerSex;
	sMember.nJoinTm = nJoinTm;
	strcpy(sMember.szName, szName);
	g_cTongDB.ChangeMember(sMember);
	return TRUE;
}

BOOL CTongControl::ChangeMoney(STONG_MONEY_COMMAND* pMoney, STONG_MONEY_SYNC *Sync)
{
	if (!pMoney)
		return FALSE;

	// ÁÄÌìÆµµÀ·¢ÏûÏ¢
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	Sync->ProtocolFamily = pf_tong;
	Sync->m_dwTongNameID = m_dwNameID;
	Sync->m_dwParam = pMoney->m_dwParam;
	strcpy(Sync->m_szName,pMoney->m_szName);
	switch(pMoney->ProtocolID)
	{
	case enumC2S_TONG_MONEY_SAVE:
		m_dwMoney += pMoney->m_dwMoney;

		Sync->ProtocolID = enumS2C_TONG_MONEY_SAVE;
		Sync->m_nMoney = pMoney->m_dwMoney;
		Sync->m_dwMoney = m_dwMoney;
		if (channid != -1)
		{
			sprintf(szMsg, "%s ®· ®ãng gãp %d ng©n l­îng vµo vµo tr­¬ng môc ng©n khè bæn bang", pMoney->m_szName,pMoney->m_dwMoney);	// gui tien vao bang
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
		}
		break;
	case enumC2S_TONG_MONEY_GET:
		if (m_dwMoney < pMoney->m_dwMoney)
			return FALSE;
		m_dwMoney -= pMoney->m_dwMoney;

		Sync->ProtocolID = enumS2C_TONG_MONEY_GET;
		Sync->m_nMoney = pMoney->m_dwMoney;
		Sync->m_dwMoney = m_dwMoney;
		if (channid != -1)
		{
			sprintf(szMsg, "%s ®· rót khái tr­¬ng môc ng©n khè %d ng©n l­îng", pMoney->m_szName,pMoney->m_dwMoney);	// lay tien khoi bang
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
		}
		break;
	}

	return TRUE;
}

BOOL CTongControl::DBChangeAgname(STONG_ACCEPT_AGNAME_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;

	char	szName[32];
	memcpy(szName, pAccept->m_szName, sizeof(pAccept->m_szName));
	szName[31] = 0;
	if (!szName[0])
		return FALSE;
	
	char	szAgname[32];
	memcpy(szAgname, pAccept->m_szAgname, sizeof(pAccept->m_szAgname));
	szAgname[31] = 0;
	if (!szAgname[0])
		return FALSE;
	
	if (pAccept->m_btAcceptFalg == 0)
	{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(m_szMasterName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_CHANGE_AGNAME_FAIL_SYNC	sFail;
				sFail.ProtocolFamily	= pf_tong;
				sFail.ProtocolID		= enumS2C_TONG_CHANGE_AGNAME_FAIL;
				sFail.m_dwParam			= param;
				sFail.m_btFailID		= 1;
				sFail.m_dwTongNameID	= pAccept->m_dwTongNameID;
				memcpy(sFail.m_szName, szName, sizeof(szName));
				tongconndup.SendPackage((const void *)&sFail, sizeof(sFail));
			}
			else
			{
				//tong is not connect
			}
		}
		else
		{
			//not find
		}
		
		return FALSE;
	}
	
	DWORD	dwNameID;
	int		i;
	dwNameID = g_String2Id(szName);
	if (dwNameID == 0)
		return FALSE;
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	if (channid == -1)
		return FALSE;
	if (pAccept->m_btFigure == enumTONG_FIGURE_DIRECTOR)
	{
		for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
		{
			if (m_dwDirectorID[i] == dwNameID && strcmp(szName, m_szDirectorName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_DIRECTOR)
			return FALSE;

		if (strcmp(szAgname, m_szDirectorAgname[i]) == 0)
			return FALSE;

		strcpy(this->m_szDirectorAgname[i], szAgname);
		sprintf(szMsg, "%s %s ®­îc thay ®æi danh hiÖu thµnh: %s ", defTONG_DIRECTOR_AGNAME, m_szDirectorName[i], szAgname);
	}
	else if (pAccept->m_btFigure == enumTONG_FIGURE_MANAGER)
	{
		for (i = 0; i < defTONG_MAX_MANAGER; i++)
		{
			if (m_dwManagerID[i] == dwNameID && strcmp(szName, m_szManagerName[i]) == 0)
				break;
		}
		if (i >= defTONG_MAX_MANAGER)
			return FALSE;

		if (strcmp(szAgname, m_szManagerAgname[i]) == 0)
			return FALSE;
		
		strcpy(this->m_szManagerAgname[i], szAgname);
		sprintf(szMsg, "%s %s ®­îc thay ®æi danh hiÖu thµnh: %s ", defTONG_MANAGER_AGNAME, m_szManagerName[i], szAgname);
	}
	else
	{
		return FALSE;
	}
	
	{{
		CNetConnectDup conndup;
		DWORD nameid = 0;
		unsigned long param = 0;
		if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(szName), &conndup, NULL, &nameid, &param))
		{
			CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
			if (tongconndup.IsValid())
			{
				STONG_BE_CHANGED_AGNAME_SYNC	sSync;
				sSync.ProtocolFamily = pf_tong;
				sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_AGNAME;
				sSync.m_dwParam = param;
				switch (pAccept->m_btFigure)
				{
				case enumTONG_FIGURE_DIRECTOR:
					strcpy(sSync.m_szAgname, m_szDirectorAgname[i]);
					break;
				case enumTONG_FIGURE_MANAGER:
					strcpy(sSync.m_szAgname, m_szManagerAgname[i]);
					break;
				}
				tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
			}
		}
	}}
	g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	return TRUE;
}

BOOL	CTongControl::GetMasterAgname(char *lpszAgname)
{
	if (!lpszAgname)
		return FALSE;
	if (m_szMasterAgname[0])
		strcpy(lpszAgname, m_szMasterAgname);
	else
		strcpy(lpszAgname, defTONG_MASTER_AGNAME);
	return TRUE;
}

BOOL	CTongControl::GetDirectorAgname(char *lpszAgname, int nPos)
{
	if (!lpszAgname)
		return FALSE;
	if (m_szDirectorAgname[nPos][0])
		strcpy(lpszAgname, m_szDirectorAgname[nPos]);
	else
		strcpy(lpszAgname, defTONG_DIRECTOR_AGNAME);
	return TRUE;
}

BOOL	CTongControl::GetManagerAgname(char *lpszAgname, int nPos)
{
	if (!lpszAgname)
		return FALSE;
	if (m_szManagerAgname[nPos][0])
		strcpy(lpszAgname, m_szManagerAgname[nPos]);
	else
		strcpy(lpszAgname, defTONG_MANAGER_AGNAME);
	return TRUE;
}

BOOL	CTongControl::GetMemberAgname(char *lpszAgname, int nSex)
{
	if (!lpszAgname)
		return FALSE;
	if (nSex)
		strcpy(lpszAgname, m_szFemaleAgname);
	else
		strcpy(lpszAgname, m_szMaleAgname);
	if (!lpszAgname[0])
	{
		if (m_szNormalAgname[0])
			strcpy(lpszAgname, m_szNormalAgname);
		else
			strcpy(lpszAgname, defTONG_MEMBER_AGNAME);
	}
	return TRUE;
}

BOOL CTongControl::DBChangeSexAgname(STONG_ACCEPT_SEX_AGNAME_COMMAND *pAccept)
{
	if (!pAccept)
		return FALSE;
	
	char	szAgname[32];
	memcpy(szAgname, pAccept->m_szAgname, sizeof(pAccept->m_szAgname));
	szAgname[31] = 0;
	if (!szAgname[0])
		return FALSE;

	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
	
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	
	if (channid == -1)
		return FALSE;
	
	if (pAccept->m_btSex)
	{
		if (strcmp(m_szFemaleAgname, szAgname) == 0)
			return FALSE;
		strcpy(this->m_szFemaleAgname, szAgname);
		sprintf(szMsg, "Thay ®æi danh hiÖu n÷ ®Ö tö trong bang thµnh: %s\n", szAgname);
	}
	else
	{
		if (strcmp(m_szMaleAgname, szAgname) == 0)
			return FALSE;
		strcpy(this->m_szMaleAgname, szAgname);
		sprintf(szMsg, "Thay ®æi danh hiÖu nam ®Ö tö trong bang thµnh: %s\n", szAgname);
	}

	if (!m_psMember || m_nMemberPointSize <= 0)
		return FALSE;
	int i;
	for (i = 0; i < this->m_nMemberPointSize; i++)
	{
		if (m_psMember[i].m_dwNameID == 0)
			break;
		if (m_psMember[i].m_nSex == pAccept->m_btSex)
		{
			{{
				CNetConnectDup conndup;
				DWORD nameid = 0;
				unsigned long param = 0;
				if (g_HostServer.FindPlayerByRole(NULL, std::_tstring(m_psMember[i].m_szName), &conndup, NULL, &nameid, &param))
				{
					CNetConnectDup tongconndup = g_TongServer.FindTongConnectByIP(conndup.GetIP());
					if (tongconndup.IsValid())
					{
						STONG_BE_CHANGED_AGNAME_SYNC	sSync;
						sSync.ProtocolFamily = pf_tong;
						sSync.ProtocolID = enumS2C_TONG_BE_CHANGED_AGNAME;
						sSync.m_dwParam = param;
						strcpy(sSync.m_szAgname, szAgname);
						tongconndup.SendPackage((const void *)&sSync, sizeof(sSync));
					}
					tongconndup.Clearup();
				}
				conndup.Clearup();
			}}
		}
	}
	if (i >= m_nMemberPointSize)
		return FALSE;
	
	g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	return TRUE;
}

BOOL CTongControl::DBChangeCamp(STONG_CHANGE_CAMP_COMMAND *pInfo, STONG_BE_CHANGED_CAMP_SYNC *sInfo)
{
	if (!pInfo)
		return FALSE;

	// ÁÄÌìÆµµÀ·¢ÏûÏ¢
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);

	switch(pInfo->m_btCamp)
	{
	case camp_justice:
		if (m_dwMoney < pInfo->m_nMoney)
			return FALSE;
		m_nCamp = pInfo->m_btCamp;
		m_dwMoney -= pInfo->m_nMoney;

		if (channid != -1)
		{
			sprintf(szMsg, "Phe ph¸i hiÖn t¹i cña bæn bang lµ: ChÝnh ph¸i");
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
		}
		break;
	case camp_evil:
		if (m_dwMoney < pInfo->m_nMoney)
			return FALSE;
		m_nCamp = pInfo->m_btCamp;
		m_dwMoney -= pInfo->m_nMoney;

		if (channid != -1)
		{
			sprintf(szMsg, "Phe ph¸i hiÖn t¹i cña bæn bang lµ: Tµ ph¸i");
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
		}
		break;
	case camp_balance:
		if (m_dwMoney < pInfo->m_nMoney)
			return FALSE;
		m_nCamp = pInfo->m_btCamp;
		m_dwMoney -= pInfo->m_nMoney;

		if (channid != -1)
		{
			sprintf(szMsg, "Phe ph¸i hiÖn t¹i cña bæn bang lµ: Trung lËp");
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
		}
		break;
	}

	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = m_dwNameID;
	sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_CAMP;
	sInfo->m_dwParam = pInfo->m_dwParam;
	sInfo->m_nMoney = m_dwMoney;
	sInfo->m_btCamp = pInfo->m_btCamp;
	return TRUE;
}

BOOL CTongControl::DBChangeTongRecruit(STONG_CHANGE_TONG_INFO_COMMAND *pInfo, STONG_CHANGE_TONG_INFO_SYNC *sInfo)
{
	if (!pInfo)
		return FALSE;
		
	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = pInfo->m_dwTongNameID;
	sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_RECRUIT;
	sInfo->m_dwParam = pInfo->m_dwParam;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
		
	char	szMsg[256];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	m_bRecruit = pInfo->m_nValue;
	sInfo->m_nValue = m_bRecruit;
	if (m_bRecruit)
		sprintf(szMsg,"Bang chñ më chøc n¨ng chiªu mé ®Ö tö ");
	else
		sprintf(szMsg,"Bang chñ ®ãng chøc n¨ng chiªu mé ®Ö tö ");
	if (channid != -1)
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	return TRUE;
}

BOOL CTongControl::DBChangeTongParam(STONG_CHANGE_TONG_INFO_COMMAND *pInfo, STONG_CHANGE_TONG_INFO_SYNC *sInfo)
{
	if (!pInfo)
		return FALSE;
		
	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = pInfo->m_dwTongNameID;
	sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_TONGPARAM;
	sInfo->m_dwParam = pInfo->m_dwParam;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
		
	char	szMsg[256];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	m_nTongParam = pInfo->m_nValue;
	sInfo->m_nValue = m_nTongParam;
	return TRUE;
}

BOOL CTongControl::DBChangeTongLevel(STONG_CHANGE_TONG_INFO_COMMAND *pInfo, STONG_CHANGE_TONG_INFO_SYNC *sInfo)
{
	if (!pInfo)
		return FALSE;
		
	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = pInfo->m_dwTongNameID;
	sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_LEVEL;
	sInfo->m_dwParam = pInfo->m_dwParam;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
		
	if (pInfo->m_nValue > 0)
	{
		m_btLevel = pInfo->m_nValue;
		sInfo->m_nValue = m_btLevel;
		
		char	szMsg[256];
		sprintf(szMsg, "\\O%u", m_dwNameID);
		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);		
		sprintf(szMsg,"§¼ng cÊp bang héi hiÖn t¹i lµ: %d", m_btLevel);
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeTongMoney(STONG_CHANGE_TONG_INFO_COMMAND *pInfo, STONG_CHANGE_TONG_INFO_SYNC *sInfo)
{
	if (!pInfo)
		return FALSE;
		
	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = pInfo->m_dwTongNameID;
	sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_MONEY;
	sInfo->m_dwParam = pInfo->m_dwParam;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
		
	if (pInfo->m_nValue > 0)
	{
		m_dwMoney = pInfo->m_nValue;
		sInfo->m_nValue = m_dwMoney;	
		
		char	szMsg[256];
		sprintf(szMsg, "\\O%u", m_dwNameID);
		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);			
		sprintf(szMsg, "Ng©n quü bang héi hiÖn t¹i lµ %d l­îng", m_dwMoney);
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));	
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeTongEff(STONG_CHANGE_TONG_INFO_COMMAND *pInfo, STONG_CHANGE_TONG_INFO_SYNC *sInfo)
{
	if (!pInfo)
		return FALSE;
		
	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = pInfo->m_dwTongNameID;
	sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_EFF;
	sInfo->m_dwParam = pInfo->m_dwParam;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
		
	if (pInfo->m_nValue > 0)
	{
		m_dwTotalEff = pInfo->m_nValue;
		sInfo->m_nValue = m_dwTotalEff;
		
		char	szMsg[256];
		sprintf(szMsg, "\\O%u", m_dwNameID);
		DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
		sprintf(szMsg, "Cèng hiÕn tån bang héi hiÖn t¹i lµ %d ®iÓm", m_dwTotalEff);
		if (channid != -1)
			g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));	
		return TRUE;
	}
	return FALSE;
}

BOOL CTongControl::DBChangeJiyu(STONG_CHANGE_TONG_INFO_COMMAND *pInfo, STONG_CHANGE_TONG_INFO_SYNC *sInfo)
{
	if (!pInfo)
		return FALSE;
		
	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = pInfo->m_dwTongNameID;
	sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_JIYU;
	sInfo->m_dwParam = pInfo->m_dwParam;
	
	DWORD	dwNameID;
	dwNameID = g_String2Id(m_szName);
	if (dwNameID == 0)
		return FALSE;
		
	m_dwMoney -= pInfo->m_nValue;
	m_nTongJiyuParam = pInfo->m_nTongJiyuParam;
	strcpy(m_szTongJiyuNotify, pInfo->m_szTongJiyuNotify);

	sInfo->m_nValue = m_dwMoney;
	sInfo->m_nTongJiyuParam = pInfo->m_nTongJiyuParam;
	strcpy(sInfo->m_szTongJiyuNotify, pInfo->m_szTongJiyuNotify);

	char	szMsg[100];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		sprintf(szMsg,"Bang chñ ®· tiÕn hµnh B¶o l­u tin tøc bang héi");
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
	}
	return TRUE;
}

BOOL CTongControl::DBChangeMemberEff(STONG_CHANGE_TONG_MEMBEREFF_COMMAND* pInfo, STONG_CHANGE_TONG_MEMBEREFF_SYNC* sInfo)
{
	if (!pInfo)
		return FALSE;

	int nJoinTm = 0,nSaveEff = 0, nNewPos = 0;
	int		i = 0;
	
	sInfo->ProtocolFamily = pf_tong;
	sInfo->m_dwTongNameID = pInfo->m_dwTongNameID;
	sInfo->m_dwParam = pInfo->m_dwParam;
	DWORD	dwNameID = g_String2Id(pInfo->m_szName);
	char	szMsg[96];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	switch(pInfo->ProtocolID)
	{
		case enumC2S_CHANGE_TONG_MEMBEREFF:
		{
			sInfo->ProtocolID = enumS2C_TONG_BE_CHANGED_MEMBEREFF;
			m_dwTotalEff += pInfo->m_nValue2;
			sInfo->m_dwTotalEff = m_dwTotalEff;

			switch(pInfo->m_btFigure)
			{
				case enumTONG_FIGURE_MASTER:
					if (m_dwMasterID == dwNameID)
					{
						m_nMasterSaveEff = pInfo->m_nValue1;
						nSaveEff = m_nMasterSaveEff;
						sInfo->m_nTotalEff = m_nMasterSaveEff;
						nJoinTm = m_nMasterJoinTm;
						nNewPos = 0;
						break;
					}
					break;
				case enumTONG_FIGURE_DIRECTOR:
					for (i = 0; i < defTONG_MAX_DIRECTOR; i++)
					{
						if (m_dwDirectorID[i] == dwNameID)
						{
							m_nDirectorSaveEff[i] = pInfo->m_nValue1;
							nSaveEff = m_nDirectorSaveEff[i];
							sInfo->m_nTotalEff = m_nDirectorSaveEff[i];
							nJoinTm = m_nDirectorJoinTm[i];
							nNewPos = i;
							break;
						}
					}
					break;
				case enumTONG_FIGURE_MANAGER:
					for (i = 0; i < defTONG_MAX_MANAGER; i++)
					{
						if (m_dwManagerID[i] == dwNameID)
						{
							m_nManagerSaveEff[i] = pInfo->m_nValue1;
							nSaveEff = m_nManagerSaveEff[i];
							sInfo->m_nTotalEff = m_nManagerSaveEff[i];
							nJoinTm = m_nManagerJoinTm[i];
							nNewPos = i;
							break;
						}
					}
					break;
				case enumTONG_FIGURE_MEMBER:
					for (i = 0; i < m_nMemberPointSize; i++)
					{
						if (m_psMember[i].m_dwNameID == dwNameID)
						{
							nNewPos	= 0;
							m_psMember[i].m_nMemberSaveEff = pInfo->m_nValue1;
							nSaveEff = m_psMember[i].m_nMemberSaveEff;
							sInfo->m_nTotalEff = m_psMember[i].m_nMemberSaveEff;
							nJoinTm = m_psMember[i].m_nMemberJoinTm;
							break;
						}
					}
					break;
				}
				if (channid != -1)
				{
					if(pInfo->m_nValue2)
					{
					sprintf(szMsg, "%s ®· ®ãng gãp %d ®iÓm cèng hiÕn vµo tr­¬ng môc cèng hiÕn bæn bang", pInfo->m_szName, pInfo->m_nValue2);
					g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(MESSAGE_TONG_ANNOUCE_HEAD), std::string(szMsg));
					}
					TMemberStruct	sMember;
					sMember.MemberClass = (TONG_MEMBER_FIGURE)pInfo->m_btFigure;
					sMember.nTitleIndex = nNewPos;
					sMember.nSaveEff = nSaveEff;
					sMember.nJoinTm = nJoinTm;
					strcpy(sMember.szTong, this->m_szName);
					strcpy(sMember.szName, pInfo->m_szName);
					g_cTongDB.ChangeMember(sMember);
				}
			}
		}
	return TRUE;
}

BOOL CTongControl::MessageToTong(STONG_MESSAGE_INFO_COMMAND *pInfo)
{
	if (!pInfo)
		return FALSE;
	
	char	szMsg[256];
	sprintf(szMsg, "\\O%u", m_dwNameID);
	DWORD channid = g_ChannelMgr.GetChannelID(szMsg, 0);
	if (channid != -1)
	{
		g_ChannelMgr.SayOnChannel(channid, TRUE, std::string(), std::string(pInfo->szName), std::string(pInfo->szMsg));
	}
	return TRUE;
}
