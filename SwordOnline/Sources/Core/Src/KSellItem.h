#ifndef _SERVER

class KSellItem
{
private:
	DWORD		m_dwSellID;				// 对方玩家的 npc 的 id
	int			m_nSellCount;
public:
	PlayerItem	m_sItem[MAX_EQUIPMENT_ITEM];	// 对方玩家穿在身上的装备在客户端 Item 数组中的位置信息
	int			m_nSellItem[MAX_EQUIPMENT_ITEM];
public:
	KSellItem();						// 构造
	void		Init();					// 初始化
	int			GetCount();
	void		Release();
	void		GetData(BYTE* pMsg);
	void		ViewTrade(DWORD dwID);
	void		CancelTrade();
	void		Update( BOOL bFirstView, DWORD dwID, int nCount = 0 );
	int			FindIdx(int uId);
	DWORD		GetSellId() {return m_dwSellID;};
};

extern	KSellItem	g_cSellItem;

#endif


