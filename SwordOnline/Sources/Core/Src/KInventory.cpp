#include "KCore.h"
#include "KItem.h"
#include "MyAssert.H"
#include "KInventory.h"

KInventory::KInventory()
{
	m_pArray = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
}

KInventory::~KInventory()
{
    Release();
}

BOOL KInventory::Init(int nWidth, int nHeight)
{
	if (m_pArray)
	{
		delete [] m_pArray;
		m_pArray = NULL;
	}
	m_pArray = new int[nWidth * nHeight];

	if (!m_pArray)
		return FALSE;

	ZeroMemory(m_pArray, sizeof(int) * nWidth * nHeight);
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	return TRUE;
}

void KInventory::Release()
{
	if (m_pArray)
	{
		delete []m_pArray;
	}
	m_pArray = NULL;
	this->m_nWidth = 0;
	this->m_nHeight = 0;
	this->m_nMoney = 0;
}

void KInventory::Clear()
{
	if (m_pArray)
		memset(m_pArray, 0, sizeof(int) * m_nWidth * m_nHeight);
	m_nMoney = 0;
}

void KInventory::ClearItem()
{
	if (m_pArray)
		memset(m_pArray, 0, sizeof(int) * m_nWidth * m_nHeight);
}

#pragma optimize( "y", off)

BOOL KInventory::PlaceItem(int nX, int nY, int nIdx, int nWidth, int nHeight)
{
	if (!m_pArray)
		return FALSE;
	if (nX < 0 || nY < 0 || nWidth < 1 || nHeight < 1 || nX + nWidth > m_nWidth || nY + nHeight > m_nHeight)
		return FALSE;
	if (nIdx <= 0)
	{
		_ASSERT(0);
		return FALSE;
	}

	int i, j;
	int nOldIdx = 0;

	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			if (!nOldIdx)
				nOldIdx = m_pArray[j * m_nWidth + i];
			// 位置上有东西
			if (nOldIdx)
			{
				// 位置上有多个东西，无法放置
				if (nOldIdx != m_pArray[j * m_nWidth + i])
					return FALSE;
				return FALSE;
			}
		}
	}
	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = nIdx;
		}
	}
	return TRUE;
}

#pragma optimize( "", on)

BOOL KInventory::HoldItem(int nIdx, int nWidth, int nHeight)
{
	int i, j;
	for (i = 0; i < m_nWidth - nWidth + 1; i++)
	{
		for (j = 0; j < m_nHeight - nHeight + 1; j++)
		{
			if (PlaceItem(i, j, nIdx, nWidth, nHeight))
				return TRUE;
		}
	}
	return FALSE;
}

BOOL	KInventory::PickUpItem(int nIdx, int nX, int nY, int nWidth, int nHeight)
{
	if (nX < 0 || nY < 0 || nWidth < 1  || nHeight < 1 || nX + nWidth > this->m_nWidth || nY + nHeight > this->m_nHeight)
		return FALSE;

	int		i;
	for (i = nX; i < nX + nWidth; i++)
	{
		for (int j = nY; j < nY + nHeight; j++)
		{
			if (m_pArray[j * m_nWidth + i] != nIdx)
			{
				_ASSERT(0);
				return FALSE;
			}
		}
	}

	for (i = nX; i < nX + nWidth; i++)
	{
		for (int j = nY; j < nY + nHeight; j++)
		{
			m_pArray[j * m_nWidth + i] = 0;
		}
	}

	return TRUE;
}
int		KInventory::FindItem(int nX, int nY)
{
	if (!m_pArray)
		return -1;

	if (nX < 0 || nX >= this->m_nWidth || nY < 0 || nY >= this->m_nHeight)
		return -1;

	int		nPos = nY * m_nWidth + nX;
	int		*pArray = &m_pArray[nPos];
	if (*pArray <= 0)
		return 0;

	int		nIdx = *pArray;
	if ((nPos > 0 && *(pArray - 1) == nIdx))
	{
		g_DebugLog("[error] FindItem1");
		return -1;
	}
	if ((nPos >= m_nWidth && *(pArray - m_nWidth) == nIdx))
	{
		g_DebugLog("[error] FindItem2");
		return -1;
	}
	return nIdx;
}

int	KInventory::FindFreeCell(int nWidth, int nHeight)
{
	if (nWidth < 1 || nWidth > m_nWidth || nHeight < 1 || nHeight > m_nHeight)
		return 0;

	int i, j;
	int nCount = 0;
	for (i = 0; i < m_nWidth; i++)
	{
		int nReduce = 0;
		for (j = 0; j < m_nHeight; j++)
		{
			if (CheckRoom(i, j, nWidth, nHeight))
			{
				nReduce=(nWidth-1);
				j+=(nHeight-1);
				nCount++;
			}
		}
		i+=nReduce;
	}
	return nCount;
}


BOOL KInventory::FindRoom(int nWidth, int nHeight, POINT* pPos)
{
	if (!pPos)
		return FALSE;
	if (nWidth < 1 || nWidth > m_nWidth || nHeight < 1 || nHeight > m_nHeight)
		return FALSE;

	int i, j;
	for (i = 0; i < m_nWidth - nWidth + 1; i++)
	{
		for (j = 0; j < m_nHeight - nHeight + 1; j++)
		{
			if (CheckRoom(i, j, nWidth, nHeight))
			{
				pPos->x = i;
				pPos->y = j;
				return TRUE;
			}
		}
	}
	pPos->x = 0;
	pPos->y = 0;
	return FALSE;
}

BOOL KInventory::CheckRoom(int nX, int nY, int nWidth, int nHeight)
{
	if (!m_pArray)
	{
		_ASSERT(0);
		return FALSE;
	}

	if (nX < 0 || nY < 0 || nWidth < 1 || nHeight < 1 || nX + nWidth > m_nWidth || nY + nHeight > m_nHeight)
		return FALSE;

	int i, j;
	int nOldIdx = 0;

	for (i = nX; i < nX + nWidth; i++)
	{
		for (j = nY; j < nY + nHeight; j++)
		{
			if (m_pArray[j * m_nWidth + i])
			{
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL KInventory::AddMoney(int nMoney)
{
	if (m_nMoney + nMoney < 0)
		return FALSE;
	m_nMoney += nMoney;
	return TRUE;
}

int		KInventory::GetNextItem(int nStartIdx, int nXpos, int nYpos, int *pX, int *pY)
{
	if (!m_pArray)
		return 0;
	if (nXpos < 0 || nYpos < 0 || nXpos >= m_nWidth || nYpos >= m_nHeight || !pX || !pY)
		return 0;
	int		nSize = m_nWidth * m_nHeight;
	int		i = nYpos * m_nWidth + nXpos;
	int		*pArray = &m_pArray[i];
	for ( ; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (*pArray == nStartIdx)
			continue;
		if (i < m_nWidth || pArray[-m_nWidth] != *pArray)
		{
			*pX = i % m_nWidth;
			*pY = i / m_nWidth;
			return *pArray;
		}
	}
	return 0;
}

int		KInventory::CalcSameDetailType(int nItemNature, int nItemGenre, int nDetailType, int nLevel, int nSeries)
{
	if (!m_pArray)
		return 0;
	int		nNum = 0;
	int		nCurIdx = 0;
	int		nSize = m_nWidth * m_nHeight;
	int		*pArray = m_pArray;
	for (int i = 0; i < nSize; i++)
	{
		if (*pArray <= 0)
		{
			pArray++;
			continue;
		}
		if (nCurIdx == *pArray)
		{
			pArray++;
			continue;
		}
		if (i < m_nWidth || pArray[-m_nWidth] != *pArray)
		{
			nCurIdx = *pArray;
			if (Item[nCurIdx].GetNature() == nItemNature && 
				Item[nCurIdx].GetGenre() == nItemGenre && 
				Item[nCurIdx].GetDetailType() == nDetailType && 
				Item[nCurIdx].GetLevel() == nLevel && 
				Item[nCurIdx].GetSeries() == nSeries)
			{
				if (Item[nCurIdx].IsStack())
					nNum += Item[nCurIdx].GetStackNum();
				else
					nNum ++;
			}
		}
		pArray++;
	}

	return nNum;
}

BOOL	KInventory::FindEmptyPlace(int nWidth, int nHeight, POINT *pPos)
{
	if (!m_pArray)
		return FALSE;
	if (!pPos || nWidth <= 0 || nHeight <= 0 || nWidth > m_nWidth || nHeight > m_nHeight)
		return FALSE;

	int		i, j, nIdx, a, b, nFind;
	for (i = 0; i < m_nHeight - nHeight + 1; i++)
	{
		for (j = 0; j < m_nWidth - nWidth + 1; )
		{
			nIdx = m_pArray[i * m_nWidth + j];
			if (nIdx)
			{
				_ASSERT(Item[nIdx].GetWidth() > 0);
				j += Item[nIdx].GetWidth();
			}
			else
			{
				nFind = 1;
				for (a = i; a < i + nHeight; a++)
				{
					for (b = j; b < j + nWidth; b++)
					{
						if (m_pArray[a * m_nWidth + b])
						{
							nFind = 0;
							break;
						}
					}
					if (nFind == 0)
						break;
				}
				if (nFind)
				{
					pPos->x = a;
					pPos->y = b;
					return TRUE;
				}
				j++;
			}
		}
	}

	return FALSE;
}


BOOL	KInventory::FindSameToRemove(int nItemNature, int nItemGenre, int nDetailType, int nLevel, int nSeries, int *pnIdx)
{
	if (!m_pArray)
		return FALSE;
	if (!pnIdx)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetNature() == nItemNature && 
			Item[*pArray].GetGenre() == nItemGenre && 
			(nDetailType < 0 || Item[*pArray].GetDetailType() == nDetailType) && 
			(nLevel < 0 || Item[*pArray].GetLevel() == nLevel) && 
			(nSeries < 0 || Item[*pArray].GetSeries() == nSeries))
		{
			*pnIdx = *pArray;
			return TRUE;
		}
	}

	return FALSE;
}
//---------------------------------------------------------------------------------
// 功能：输入物品类型和具体类型，察看Inventory里面有没有相同的物品
//---------------------------------------------------------------------------------
BOOL	KInventory::CheckSameImmediacyItem(int nIdx)
{
	if (!m_pArray)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if (Item[*pArray].GetGenre() == Item[nIdx].GetGenre())
		{
			if (Item[nIdx].GetGenre() == item_event)
			{
				if (Item[*pArray].GetParticular() == Item[nIdx].GetParticular() && Item[*pArray].GetLevel() == Item[nIdx].GetLevel())
					return TRUE;
			}
			else
			{
				if (Item[*pArray].GetDetailType() == Item[nIdx].GetDetailType())
					return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL	KInventory::FindSameItem(int nItemGenre, int nDetailType, int nParticularType, int nLevel, int *pnIdx, int *pnX, int *pnY)
{
	if (!m_pArray)
		return FALSE;
	if (!pnIdx || !pnX || !pnY)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;

	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0 )
			continue;
		if (Item[*pArray].GetGenre() == nItemGenre && 
			Item[*pArray].GetDetailType() == nDetailType && 
			Item[*pArray].GetParticular() == nParticularType && 
			Item[*pArray].GetLevel() == nLevel)
		{
			*pnIdx = *pArray;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	}

	return FALSE;

}

//==============================================
BOOL  KInventory::FindSameItemToStack(int nIdx, int *pnIdx, int *pnX, int *pnY)
{
	if (!m_pArray)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;
	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
		if  (Item[*pArray].GetLock()->IsLock())
			continue;
	if (Item[*pArray].GetRow() == Item[nIdx].GetRow() && 
		Item[*pArray].IsStack() && Item[nIdx].IsStack() && 
		Item[*pArray].GetNature() == Item[nIdx].GetNature() && 
		Item[*pArray].GetGenre() == Item[nIdx].GetGenre() && 
		Item[*pArray].GetDetailType() == Item[nIdx].GetDetailType() && 
		Item[*pArray].GetParticular() == Item[nIdx].GetParticular() && 
		Item[*pArray].GetSeries() == Item[nIdx].GetSeries() && 
		Item[*pArray].GetLevel() == Item[nIdx].GetLevel() && 
		Item[*pArray].GetMantle() == Item[nIdx].GetMantle() && 
		Item[*pArray].GetStackNum() < Item[*pArray].GetMaxStackNum() && 
		Item[*pArray].GetExpireTime() == Item[nIdx].GetExpireTime() && 
		Item[*pArray].GetLockSell() == Item[nIdx].GetLockSell() &&
		Item[*pArray].GetLockTrade() == Item[nIdx].GetLockTrade() &&
		Item[*pArray].GetLockDrop() == Item[nIdx].GetLockDrop() &&
		Item[*pArray].GetParam() == Item[nIdx].GetParam()
	)
		{
			*pnIdx = *pArray;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	
	}
	return FALSE;
}


int  KInventory::FindSameItemToMove(int nIdx, int *pnIdx, int *pnX, int *pnY)
{
	if (!m_pArray)
		return FALSE;

	int		*pArray = m_pArray;
	int		i, nSize = m_nWidth * m_nHeight;
	for (i = 0; i < nSize; i++, pArray++)
	{
		if (*pArray <= 0)
			continue;
	if (Item[*pArray].GetRow() == Item[nIdx].GetRow() && 
		Item[*pArray].GetNature() == Item[nIdx].GetNature() && 
		Item[*pArray].GetGenre() == Item[nIdx].GetGenre() && 
		Item[*pArray].GetDetailType() == Item[nIdx].GetDetailType() && 
		Item[*pArray].GetParticular() == Item[nIdx].GetParticular() && 
		Item[*pArray].GetSeries() == Item[nIdx].GetSeries() && 
		Item[*pArray].GetLevel() == Item[nIdx].GetLevel() && 
		Item[*pArray].GetMantle() == Item[nIdx].GetMantle() && 
		Item[*pArray].GetExpireTime() == Item[nIdx].GetExpireTime() && 
		Item[*pArray].GetLockSell() == Item[nIdx].GetLockSell() && 
		Item[*pArray].GetLockTrade() == Item[nIdx].GetLockTrade() && 
		Item[*pArray].GetLockDrop() == Item[nIdx].GetLockDrop() && 
		Item[*pArray].GetParam() == Item[nIdx].GetParam()
	)
		{ 
			*pnIdx = *pArray;
			*pnX = i % m_nWidth;
			*pnY = i / m_nWidth;
			return TRUE;
		}
	
	}
	return FALSE;
}
