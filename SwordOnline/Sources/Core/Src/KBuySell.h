#ifndef	KBuySellH
#define	KBuySellH

#define	BUY_SELL_SCALE		4

class KItem;

typedef struct
{
	int		nItemNature;
	int		nGenre;
	int		nDetailType;
	int		nParticularType;
	int		nSeriesReq;
	int		nLevel;
	int		nNewPrice;
	int		nParam;
	int		nExpirePoint;
	int		nLock;
	int		nNewArrival;
} ItemGenerate;

class KInventory;

class KBuySell
{
public:
	KBuySell();
	~KBuySell();
	int				Init();
	int				GetWidth() { return m_Width; }
	int				GetHeight() { return m_Height; }
	KItem*			GetItem(int nIndex);
	int				GetItemIndex(int nShop, int nIndex);
	BOOL			BuySellCheck(int nBuy, int nBuyIdx);
private:
	int**			m_SellItem;
	KItem*			m_Item;
	int				m_Width;
	int				m_Height;
	int				m_ItemNum;
public:
#ifdef _SERVER
	BOOL			CanBuy(int nPlayerIdx, int nBuy, int nBuyIdx, int nBuyNumber);
	void			OpenSale(int nPlayerIdx, int nShop, int nShopMoneyUnit);
	void			OpenSale(int nPlayerIdx, int nSaleType, int nMoneyUnit, int nShopNum, int *nShopId);
	BOOL			Buy(int nPlayerIdx, int nBuy, int nBuyIdx, BYTE nBuyNumber);	// 买第几个买卖列表中的第几项道具
	BOOL			Sell(int nPlayerIdx, int nBuy, int nIdx, int nBuyNumber);
#endif
#ifndef _SERVER
	KInventory*		m_pShopRoom;
	KInventory*		m_pSShopRoom;
	void			OpenSale(BuySellInfo *pInfo);
	void			OpenSale(int nSaleType, BuySellInfo *pInfo);
	void			PaintItem(int nIdx, int nX, int nY, BOOL bStack = FALSE);
#endif
};

extern KBuySell	BuySell;
#endif
