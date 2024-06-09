#include	"KCore.h"

#ifndef _SERVER
#include	"CoreShell.h"
#include	"KItemSet.h"
#include	"KItem.h"
#include	"KSellItem.h"

KSellItem	g_cSellItem;

KSellItem::KSellItem()
{
	Init();
}

void	KSellItem::Init()
{
	m_dwSellID	= 0;
	m_nSellCount = 0;
	memset(m_nSellItem, 0, sizeof(m_nSellItem));
}

void	KSellItem::Release()
{	
	m_dwSellID	= 0;
	m_nSellCount = 0;

	for (int i = 0; i < MAX_EQUIPMENT_ITEM; i ++)
	{
		if (m_sItem[i].nIdx)
			ItemSet.Remove(m_sItem[i].nIdx);
		m_sItem[i].nIdx = 0;
		m_sItem[i].nX = 0;
		m_sItem[i].nY = 0;
		m_nSellItem[i] = 0;
	}
}

int		KSellItem::FindIdx(int uId)
{
	for (int i = 0; i < MAX_EQUIPMENT_ITEM; i++)
	{
		if (m_sItem[i].nIdx == uId)
			return m_nSellItem[i];
	}
	return 0;
}

void	KSellItem::GetData(BYTE* pMsg)
{
	if (!pMsg)
		return;
	
	int					i, j,nNewIdx;
	PLAYER_TRADE_ITEM_SYNC		*pSellItem = (PLAYER_TRADE_ITEM_SYNC*)pMsg;

	if (pSellItem->m_sInfo.m_nID == 0)
		return;

	nNewIdx = ItemSet.Add(
		pSellItem->m_sInfo.m_btNature,
		pSellItem->m_sInfo.m_btGenre,
		pSellItem->m_sInfo.m_btSeries,
		pSellItem->m_sInfo.m_btLevel,
		pSellItem->m_sInfo.m_btLuck,
		pSellItem->m_sInfo.m_btDetail,
		pSellItem->m_sInfo.m_btParticur,
		pSellItem->m_sInfo.m_btMagicLevel,
		pSellItem->m_sInfo.m_wVersion,
		pSellItem->m_sInfo.m_dwRandomSeed);

	if (nNewIdx <= 0)
		return;
		
	Item[nNewIdx].SetID(pSellItem->m_sInfo.m_nID);
	Item[nNewIdx].SetStackNum(pSellItem->m_sInfo.m_nStackNum);
	Item[nNewIdx].SetExpireTime(pSellItem->m_sInfo.m_ExpireTime);
	Item[nNewIdx].SetParam(pSellItem->m_sInfo.m_nParam);
	Item[nNewIdx].SetFortune(pSellItem->m_sInfo.m_nFortune);
	Item[nNewIdx].SetTradePrice(pSellItem->m_sInfo.m_uPrice);
	m_sItem[m_nSellCount].nIdx = nNewIdx;
	m_sItem[m_nSellCount].nX = pSellItem->m_sInfo.m_bX;
	m_sItem[m_nSellCount].nY = pSellItem->m_sInfo.m_bY;
	m_nSellItem[m_nSellCount] = pSellItem->m_sInfo.m_nIdx;
	m_nSellCount++;
}

void	KSellItem::ViewTrade(DWORD dwID)
{
	KUiPlayerItem	sPlayer;

	m_dwSellID = dwID;
	sPlayer.uId = m_dwSellID;
	int nNpcIdx = NpcSet.SearchID(m_dwSellID);
	if (nNpcIdx > 0)
	{
		sPlayer.nIndex = nNpcIdx;
		strcpy(sPlayer.Name, Npc[nNpcIdx].Name);
		sPlayer.nData = Npc[nNpcIdx].GetMenuState();
	}
	else
	{
		sPlayer.nIndex = 0;
		sPlayer.Name[0] = 0;
		sPlayer.nData = 0;
	}
	CoreDataChanged(GDCNI_VIEWTRADE, (unsigned int)&sPlayer, 0);
}


void	KSellItem::CancelTrade()
{
	KSystemMessage	sMsg;
	sMsg.eType = SMT_NORMAL;
	sMsg.byConfirmType = SMCT_NONE;
	sMsg.byPriority = 0;
	sMsg.byParamSize = 0;
	sprintf(sMsg.szMessage, MSG_TRADE_CLOSED);
	CoreDataChanged(GDCNI_SYSTEM_MESSAGE, (unsigned int)&sMsg, 0);

	CoreDataChanged(GDCNI_CANCELTRADE, 0 ,0);
}

void KSellItem::Update( BOOL bFirstView, DWORD dwID, int nCount )
{
	if (bFirstView)
	{
		Release();
		
		PLAYER_TRADE_VIEW_COMMAND	sView;
		sView.ProtocolType = c2s_playertradeview;
		sView.m_dwNpcID = dwID;
		if (g_pClient)
			g_pClient->SendPackToServer(&sView, sizeof(sView));
	}
	else
	{
		if (nCount)
		{
			Release();
			
			PLAYER_TRADE_VIEW_COMMAND	sView;
			sView.ProtocolType = c2s_playertradeview;
			sView.m_dwNpcID = dwID;
			if (g_pClient)
				g_pClient->SendPackToServer(&sView, sizeof(sView));
		}
	}
}

int KSellItem::GetCount()
{
	return m_nSellCount;
}
#endif








