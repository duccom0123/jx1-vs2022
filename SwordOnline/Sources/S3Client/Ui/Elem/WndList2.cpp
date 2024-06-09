/*****************************************************************************************
//	界面窗口体系结构--列表窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-11
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndList2.h"
#include "UiImage.h"
#include "MouseHover.h"
#include "../../../Engine/Src/Text.h"
#include "../../../Engine/Src/KDebug.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
extern iRepresentShell*	g_pRepresentShell;
#define	NO_BG_COLOR		0xffffffff
#define NODE_RECT		8
#define	NODE_RECT_DIV2	4
#define	NODE_RECT_DIV4	2
unsigned int KWndList2::ms_uBtnTipTextColor = 0xffffffff;	//按钮提示名称文字的颜色
int			 KWndList2::ms_nDisableBtnTip   = false;		//是否静止按钮的提示文字
//设置按钮提示名称文字的颜色
void KWndList2::SetAllButtonTipTextColor(unsigned int uColor)
{
	ms_uBtnTipTextColor = uColor;
}

//禁止/允许按钮的提示文字
void KWndList2::EnableAllButtonTip(int bEnable)
{
	ms_nDisableBtnTip = !bEnable;
}
struct KWndList2Item
{
	int		Data;
	bool	IsNode;
	bool	Open;
	unsigned int Color;
	char	Tip[512];
	int		TipLen;
	DWORD	StringSize;
	char	String[1];
};

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndList2::KWndList2()
{
	m_pContent		= NULL;
	m_nItemSpace	= 0;
	m_pScrollbar    = NULL;
	m_nNumItem		= 0;
	m_nNodeList		= 0;
	m_nFontSize		= 12;
	m_nRowDis = 0;
	m_nTopItemIndex = 0;
	m_nLastData = 0;
	m_nSelItemIndex		  = -1;
	m_NodeItemColor = 0xffffffff;
	m_ItemBorderColor = m_SelItemBorderColor = 0;
	m_NodeItemBorderColor = m_uIconLineColor = m_uNodeLineColor = 0;
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KWndList2::~KWndList2()
{
	ResetContent();
}

void KWndList2::Clone(KWndList2* pCopy)
{
	if (pCopy)
	{
		pCopy->m_nFontSize	= m_nFontSize;
		pCopy->m_nRowDis = m_nRowDis;
		pCopy->m_ItemColor	= m_ItemColor;
		pCopy->m_nNodeList = m_nNodeList;
		pCopy->m_nLastData = m_nLastData;
		pCopy->m_SelItemColor	= m_SelItemColor;
		pCopy->m_ItemBorderColor = m_ItemBorderColor;
		pCopy->m_SelItemBorderColor = m_SelItemBorderColor;
		pCopy->m_uSelItemBgColor = m_uSelItemBgColor;
		pCopy->m_NodeItemColor	= m_NodeItemColor;
		pCopy->m_NodeItemBorderColor = m_NodeItemBorderColor;
		pCopy->m_uIconLineColor = m_uIconLineColor;
		pCopy->m_uNodeLineColor = m_uNodeLineColor;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KWndList2::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "Node", 0, &m_nNodeList);
		pIniFile->GetInteger(pSection, "Font", 16, &m_nFontSize);
		if (m_nFontSize < 8)
			m_nFontSize = 8;
		pIniFile->GetInteger(pSection, "RowDis", 0, &m_nRowDis);
		char	Buff[16];
		pIniFile->GetString(pSection, "Color", "", Buff, 16);
		m_ItemColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "", Buff, 16);
		m_ItemBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "NodeColor", "", Buff, 16);
		m_NodeItemColor = GetColor(Buff);
		pIniFile->GetString(pSection, "NodeBorderColor", "", Buff, 16);
		m_NodeItemBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "IconLineColor", "", Buff, 16);
		m_uIconLineColor = GetColor(Buff);
		pIniFile->GetString(pSection, "NodeLineColor", "", Buff, 16);
		m_uNodeLineColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelColor", "", Buff, 16);
		m_SelItemColor = GetColor(Buff);
		pIniFile->GetString(pSection, "SelBorderColor", "", Buff, 16);
		m_SelItemBorderColor = GetColor(Buff);
		if (pIniFile->GetString(pSection, "SelItemBgColor", "", Buff, 16) && Buff[0])
		{
			m_uSelItemBgColor = (GetColor(Buff) & 0xffffff) | ((50 << 21) & 0xff000000);
			int nValue;
			pIniFile->GetInteger(pSection, "SelItemBgAlpha", 128, &nValue);
			if (nValue > 255 || nValue < 0)
				m_uSelItemBgColor |= 0x16000000;
			else
				m_uSelItemBgColor |= (((255 - nValue) << 21) & 0xff000000);
		}
		else
			m_uSelItemBgColor = NO_BG_COLOR;
		return true;
	}
	return false;
}


void KWndList2::SetScrollbar(KWndScrollBar* pScroll)
{
	if (m_pScrollbar = pScroll)
	{
		m_pScrollbar->SetStyle(m_pScrollbar->GetStyle() | WND_S_MOVEALBE);
    	if(GetCurCount() > GetVisibleLineCount())
		{
			m_pScrollbar->SetValueRange(0, GetCurCount() - GetVisibleLineCount());
			m_pScrollbar->Enable(true);
		}
		else
			m_pScrollbar->Enable(false);
	}
}


//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndList2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WM_MOUSEMOVE://to be check
		OnMouseMove(LOWORD(nParam), HIWORD(nParam));
		break;
	case WM_LBUTTONDOWN:
		OnLButtonDown(LOWORD(nParam), HIWORD(nParam));
		break;
	case WM_LBUTTONDBLCLK:
		OnLButtonDClick(LOWORD(nParam), HIWORD(nParam));
		break;
	case WM_MOUSEWHEEL:
		if (m_pScrollbar && !m_pScrollbar->IsDisable())
		{
			int zDelta = short(HIWORD(uParam));
			int nPos = m_pScrollbar->GetScrollPos();
			nPos += (-zDelta / WHEEL_DELTA);
			m_pScrollbar->SetScrollPos(nPos);
		}
		break;
	default:
		return KWndWindow::WndProc(uMsg, nParam, nParam);
	}
	return 0;
}

int KWndList2::SetCurSel(int nSel)
{
	if (nSel >= 0 && nSel < m_nNumItem && nSel != m_nSelItemIndex)
	{
		if(m_nNodeList)
		{
			if(nSel < m_nSelItemIndex)
			{
				KWndList2Item* pItem = (KWndList2Item*)m_pContent[nSel];
				if(!pItem->IsNode)
				{
					pItem = (KWndList2Item*)m_pContent[m_nSelItemIndex];
					if(pItem->IsNode)
					{
						int nReduce=0;
						for(int j=nSel-1;j>=0;j--)
						{
							KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
							nReduce++;
							if(pTemp->IsNode)
							{
								if(!pTemp->Open)
									nSel-=nReduce;
								break;
							}
						}
					}
				}
			}
			else
			{
				if(nSel == m_nNumItem -1)
				{
					int nReduce=0;
					for(int j=m_nNumItem-1;j>=0;j--)
					{
						KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
						if(pTemp->IsNode)
						{
							if(!pTemp->Open)
								nSel-=nReduce;
							break;
						}
						nReduce++;
					}
				}
				else if (m_nSelItemIndex >= 0 && m_nSelItemIndex < m_nNumItem)
				{
					KWndList2Item* pItem = (KWndList2Item*)m_pContent[m_nSelItemIndex];
					if(pItem->IsNode && !pItem->Open)
					{
						for(int j=m_nSelItemIndex+1;j<m_nNumItem;j++)
						{
							KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
							if(!pTemp->IsNode && pTemp->Data == pItem->Data)
							{
								nSel++;
							}
						}
					}
				}
			}
		}
		if(nSel >= m_nNumItem)
			nSel = 0;
		m_nSelItemIndex = nSel;
		if (m_nSelItemIndex < m_nTopItemIndex)
		{
			m_nTopItemIndex = m_nSelItemIndex;
		}
		else if(m_nSelItemIndex >= m_nTopItemIndex + GetVisibleLineCount())
		{
			m_nTopItemIndex = m_nSelItemIndex - GetVisibleLineCount() + 1;
			m_nTopItemIndex -= GetVisibleNodeCount(m_nSelItemIndex);
			if (m_nTopItemIndex < 0)
				m_nTopItemIndex = 0;
		}
		if (m_pParentWnd)
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
	}
	return m_nSelItemIndex;
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键在此按下
//--------------------------------------------------------------------------
void KWndList2::OnLButtonDown(int x, int y)
{
	int nCurNum = GetCurCount();
	int nSel = m_nTopItemIndex - GetVisibleNodeCount(m_nTopItemIndex) + (y - m_nAbsoluteTop) / (m_nFontSize + 1 + m_nRowDis);
	if (nSel >= nCurNum)
		nSel = -1;

	if(m_nNodeList)
	{
		if (nSel > 0 && nSel < m_nNumItem)
		{
			for(int i=0;i< nSel;i++)
			{
				KWndList2Item* pItem = (KWndList2Item*)m_pContent[i];
				if(pItem->IsNode && !pItem->Open)
				{
					for(int j=i;j<m_nNumItem;j++)
					{
						KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
						if(!pTemp->IsNode && pTemp->Data == pItem->Data)
						{
							nSel++;
						}
					}
				}
			}
		}
	}
	if(nSel >-1)
	{
		KWndList2Item* pItem = (KWndList2Item*)m_pContent[nSel];
		if(m_nNodeList)
		{
			if(pItem->IsNode)
			{
				if(x <= (m_nAbsoluteLeft + NODE_RECT))
				{
					pItem->Open = !pItem->Open;
					if (m_pScrollbar)
					{
    					if(GetCurCount() > GetVisibleLineCount())
						{
							m_pScrollbar->SetValueRange(0, GetCurCount() - GetVisibleLineCount());
    						m_pScrollbar->Enable(true);
						}
						else
						{
    						m_pScrollbar->Enable(false);
						}
					}
				}
			}
		}
		if (nSel != m_nSelItemIndex)
		{
			m_nSelItemIndex = nSel;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
		}
		if (ms_nDisableBtnTip == false && pItem->Tip[0] && !g_MouseOver.IsMoseHoverWndObj((void*)(KWndWindow*)this, 0))
		{
			char szTip[256];
			
			int nTipLen = GetToolTipInfo(nSel, szTip, 255);
			szTip[nTipLen] = 0;		
			
			g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this, 0,
				x, y, false, true);
			g_MouseOver.SetMouseHoverTitle(szTip, nTipLen, ms_uBtnTipTextColor);
		}
	}
}

void KWndList2::OnLButtonDClick(int x, int y)
{
	int nCurNum = GetCurCount();
	int nSel = m_nTopItemIndex - GetVisibleNodeCount(m_nTopItemIndex) + (y - m_nAbsoluteTop) / (m_nFontSize + 1 + m_nRowDis);
	if (nSel >= nCurNum)
		nSel = -1;
	if(m_nNodeList)
	{
		if (nSel > 0 && nSel < m_nNumItem)
		{
			for(int i=0;i< nSel;i++)
			{
				KWndList2Item* pItem = (KWndList2Item*)m_pContent[i];
				if(pItem->IsNode && !pItem->Open)
				{
					for(int j=i+1;j<m_nNumItem;j++)
					{
						KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
						if(pTemp->Data == pItem->Data)
						{
							nSel++;
						}
					}
				}
			}
		}
	}
	if (nSel != -1)
	{
		KWndList2Item* pItem = (KWndList2Item*)m_pContent[nSel];
		if(m_nNodeList)
		{
			if(pItem->IsNode)
			{
				pItem->Open = !pItem->Open;
				if (m_pScrollbar)
				{
    				if(GetCurCount() > GetVisibleLineCount())
					{
						m_pScrollbar->SetValueRange(0, GetCurCount() - GetVisibleLineCount());
    					m_pScrollbar->Enable(true);
					}
					else
					{
    					m_pScrollbar->Enable(false);
					}
				}
			}
		}
		m_nSelItemIndex = nSel;
		if (m_pParentWnd)
			m_pParentWnd->WndProc(WND_N_LIST_ITEM_D_CLICK, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
	}
}

//--------------------------------------------------------------------------
//	功能：响应鼠标移动
//--------------------------------------------------------------------------
void KWndList2::OnMouseMove(int x, int y)
{
	int nCurNum = GetCurCount();
	int nIndex = m_nTopItemIndex - GetVisibleNodeCount(m_nTopItemIndex) + (y - m_nAbsoluteTop) / (m_nFontSize + 1 + m_nRowDis);
	if (nIndex >= nCurNum)
	{
		g_MouseOver.CancelMouseHoverInfo();
		return;
	}
	if(m_nNodeList)
	{
		if (nIndex > 0 && nIndex < m_nNumItem)
		{
			for(int i=0;i< nIndex;i++)
			{
				KWndList2Item* pItem = (KWndList2Item*)m_pContent[i];
				if(pItem->IsNode && !pItem->Open)
				{
					for(int j=i+1;j<m_nNumItem;j++)
					{
						KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
						if(pTemp->Data == pItem->Data)
						{
							nIndex++;
						}
					}
				}
			}
		}
	}
	if(nIndex ==m_nSelItemIndex)
	{
		KWndList2Item* pItem = (KWndList2Item*)m_pContent[nIndex];
		if (ms_nDisableBtnTip == false && pItem->Tip[0] && !g_MouseOver.IsMoseHoverWndObj((void*)(KWndWindow*)this, 0))
		{
			char szTip[256];
			
			int nTipLen = GetToolTipInfo(nIndex, szTip, 255);
			szTip[nTipLen] = 0;		
			
			g_MouseOver.SetMouseHoverInfo((void*)(KWndWindow*)this, 0,
				x, y, false, true);
			g_MouseOver.SetMouseHoverTitle(szTip, nTipLen, ms_uBtnTipTextColor);
		}
	}
}

int	KWndList2::GetToolTipInfo(int nIndex, char* pBuffer, int nSize)
{
	if (nIndex >= 0 && nIndex < m_nNumItem && pBuffer)
	{
		KWndList2Item* pItem = (KWndList2Item*)m_pContent[nIndex];
		if (pItem->TipLen < nSize)
		{
			memcpy(pBuffer, pItem->Tip, pItem->TipLen);
			pBuffer[pItem->TipLen] = 0;
			return pItem->TipLen;
		}
	}
	return WNDLIST_ERROR;
}

int KWndList2::SetToolTipInfo(int nIndex, char* pBuffer, int nSize)
{
	if (nIndex >= 0 && nIndex < m_nNumItem && pBuffer)
	{
		KWndList2Item* pItem = (KWndList2Item*)m_pContent[nIndex];
		pItem->TipLen = min(nSize, sizeof(pItem->Tip) - 1);
		memcpy(pItem->Tip, pBuffer, pItem->TipLen);
		pItem->Tip[pItem->TipLen] = 0;
		pItem->TipLen = TEncodeText(pItem->Tip, strlen(pItem->Tip));
		return pItem->TipLen;
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：添加项
//	参数：int nIndex --> 新项添加的位置(基于0的索引数)
//		　const char* pString --> 新项的字符串
//	返回：成功添加的话，返回新项在列表中的索引未知；否则返回WNDLIST_ERROR，
//			失败的原因可能是pString指针为NULL，或者无法为新项分配存储空间。
//--------------------------------------------------------------------------
int KWndList2::AddString(int nIndex, const char* pszString, bool bIsNode/* = false*/)
{
	if (pszString == NULL)
		return WNDLIST_ERROR;
	if (m_nItemSpace >= m_nNumItem)
	{
		void** pNew = (void**)realloc(m_pContent, sizeof(void*) * (m_nItemSpace + 10));
		if (pNew)
		{
			m_pContent = pNew;
			m_nItemSpace += 10;
		}
		else
			return WNDLIST_ERROR;
	}

	int nStrLen = strlen(pszString);
	KWndList2Item* pItem = (KWndList2Item*)malloc(sizeof(KWndList2Item) + nStrLen);
	if (pItem)
	{
		pItem->Open = false;
		pItem->IsNode = bIsNode;
		pItem->Color = m_ItemColor;
		pItem->TipLen = 0;
		memset(pItem->Tip, 0, sizeof(pItem->Tip));
		pItem->StringSize = nStrLen;
		memcpy(pItem->String, pszString, nStrLen);
		pItem->String[nStrLen] = 0;

		if (nIndex < 0)
			nIndex = 0;
		else if (nIndex > m_nNumItem)
			nIndex = m_nNumItem;
		for (int i = m_nNumItem; i > nIndex; i--)
		{
			m_pContent[i] = m_pContent[i - 1];
		}
		m_pContent[nIndex] = pItem;
		if(nIndex >0)
		{
			if(bIsNode)
				m_nLastData++;
		}
		pItem->Data = m_nLastData;
		m_nNumItem ++;
		if (m_nSelItemIndex >= nIndex)
		{
			m_nSelItemIndex ++;
			if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
		}
		if (m_pScrollbar)
	    {
    		if(GetCurCount() > GetVisibleLineCount())
		    {
				m_pScrollbar->SetValueRange(0, GetCurCount() - GetVisibleLineCount());
    			m_pScrollbar->Enable(true);
		    }
		    else
		    {
    			m_pScrollbar->Enable(false);
		    }
	    }
	}
	else
		nIndex = WNDLIST_ERROR;
	return nIndex;
}

//--------------------------------------------------------------------------
//	功能：删除项
//	参数：int nIndex --> 要删除项的索引
//	返回：如果成功删除指定的项，则返回列表里剩余项的数目，否则返回WNDLIST_ERROR
//--------------------------------------------------------------------------
int	KWndList2::DeleteString(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		free(m_pContent[nIndex]);
		m_nNumItem --;
		for (int i = nIndex; i < m_nNumItem; i++)
			m_pContent[i] = m_pContent[i + 1];
		if (m_nTopItemIndex >= m_nNumItem && m_nTopItemIndex)
			m_nTopItemIndex --;
		if (m_nSelItemIndex > nIndex)
			m_nSelItemIndex --;
		else if (m_nSelItemIndex == nIndex)
			m_nSelItemIndex = -1;
		if (m_pParentWnd)
				m_pParentWnd->WndProc(WND_N_LIST_ITEM_SEL, (unsigned int)(KWndWindow*)this, m_nSelItemIndex);
		if (m_pScrollbar)
	    {
    		if(GetCurCount() > GetVisibleLineCount())
			{
				m_pScrollbar->SetValueRange(0, GetCurCount() - GetVisibleLineCount());
    			m_pScrollbar->Enable(true);
		    }
		    else
		    {
    			m_pScrollbar->Enable(false);
		    }
	    }
		return m_nNumItem;
	}
	else
		return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：删除列表中全部的内容
//--------------------------------------------------------------------------
void KWndList2::ResetContent()
{
	if (m_pContent)
	{
		for (int i = 0; i < m_nNumItem; i++)
			free(m_pContent[i]);
		m_nNumItem = 0;
		m_nItemSpace = 0;
		free(m_pContent);
		m_pContent = NULL;
	}
	m_nTopItemIndex = 0;
	m_nSelItemIndex = -1;
	if(m_pScrollbar)
	{
		m_pScrollbar->Enable(FALSE);
		m_pScrollbar->SetScrollPos(0);
		m_pScrollbar->SetValueRange(0, 0);
	}
}

//--------------------------------------------------------------------------
//	功能：绘制窗口
//--------------------------------------------------------------------------
void KWndList2::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (g_pRepresentShell)
	{
		int nCount =0;
		int x = m_nAbsoluteLeft;
		int y = m_nAbsoluteTop;
		//int EndIndex = m_nTopItemIndex + GetVisibleLineCount();
		int EndIndex = m_nNumItem;
		if (EndIndex > m_nNumItem)
			EndIndex = m_nNumItem;

		int	nMaxLen = (m_Width * 2) / m_nFontSize + 1;
		char	szBuffer[128];
		if (nMaxLen > sizeof(szBuffer))
			nMaxLen = sizeof(szBuffer);

		unsigned int Color;
		unsigned int BorderColor;
		for (int i = m_nTopItemIndex; i < EndIndex; i++)
		{
			KWndList2Item* pItem = (KWndList2Item*)m_pContent[i];
			if (i == m_nSelItemIndex)
			{
				Color = m_SelItemColor;
				BorderColor = m_SelItemBorderColor;
				if (m_uSelItemBgColor != NO_BG_COLOR)
				{
					KRUShadow	Shadow;
					Shadow.Color.Color_dw = m_uSelItemBgColor;
					Shadow.oPosition.nX = x;
					Shadow.oPosition.nY = y;
					Shadow.oPosition.nZ = 0;
					Shadow.oEndPos.nX = x + m_Width;
					Shadow.oEndPos.nY = y + m_nFontSize + 1;
					g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
				}
			}
			else
			{
				if(m_nNodeList && pItem->IsNode)
				{
					Color = m_NodeItemColor;
					BorderColor = m_NodeItemBorderColor;
				}
				else
				{
					Color = pItem->Color;
					BorderColor = m_ItemBorderColor;
				}
			}

			int nLeft = m_nAbsoluteLeft;
			if(m_nNodeList)
			{
				KRURect	Rect;
				Rect.Color.Color_dw = m_uIconLineColor;
				Rect.oEndPos.nZ = Rect.oPosition.nZ = 0;
				KRULine Line;
				Line.Color.Color_dw = m_uNodeLineColor;
				Line.oEndPos.nZ = Line.oPosition.nZ = 0;
				bool bIsOpen = false;
				bool bIsPrevNode = false;
				if(i)
				{
					for(int j=0; j<i;j++)
					{
						KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
						if(pTemp->IsNode)
						{
							if(pTemp->Data == pItem->Data)
							{
								bIsOpen = pTemp->Open;
								break;
							}
						}
					}
					KWndList2Item* pTemp = (KWndList2Item*)m_pContent[i-1];
					if(pTemp->IsNode)
						bIsPrevNode = true;
				}
				else
					bIsOpen = pItem->Open;
				if(pItem->IsNode)
				{
					nLeft += NODE_RECT + 4;
					/*if(bIsPrevNode)
					{
						Line.oPosition.nX = x + NODE_RECT_DIV2;
						Line.oPosition.nY = y + (m_nFontSize - 12);
						Line.oEndPos.nX = Line.oPosition.nX;
						Line.oEndPos.nY = Line.oPosition.nY + NODE_RECT;
						g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
					}*/

					Rect.oPosition.nX = x;
					Rect.oPosition.nY = y + NODE_RECT_DIV4 + (m_nFontSize - 12);
					Rect.oEndPos.nX = Rect.oPosition.nX + NODE_RECT;
					Rect.oEndPos.nY = Rect.oPosition.nY + NODE_RECT;
					g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);

					Line.oPosition.nX = Rect.oPosition.nX + NODE_RECT_DIV4;
					Line.oPosition.nY = Rect.oPosition.nY + NODE_RECT_DIV2;
					Line.oEndPos.nX = Rect.oEndPos.nX - NODE_RECT_DIV4;
					Line.oEndPos.nY = Line.oPosition.nY;
					g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
					if(!pItem->Open)
					{
						Line.oPosition.nX = Rect.oPosition.nX + NODE_RECT_DIV2;
						Line.oPosition.nY = Rect.oPosition.nY + NODE_RECT_DIV4;
						Line.oEndPos.nX = Line.oPosition.nX;
						Line.oEndPos.nY = Rect.oEndPos.nY - NODE_RECT_DIV4;
						g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
					}
					Line.oPosition.nX = Rect.oEndPos.nX;
					Line.oPosition.nY = Rect.oPosition.nY + NODE_RECT_DIV2;
					Line.oEndPos.nX = nLeft + NODE_RECT_DIV2;
					Line.oEndPos.nY = Rect.oPosition.nY +  + NODE_RECT_DIV2;
					g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
				}
				else
				{
					if(!bIsOpen)
					{
						//EndIndex++;
						continue;
					}
					else
					{
						Line.Color.Color_dw = m_uNodeLineColor;
						Line.oPosition.nX = nLeft + NODE_RECT * 2;
						Line.oPosition.nY = y + (m_nFontSize/2) + 1;
						nLeft += NODE_RECT * 3;
						Line.oEndPos.nX = nLeft;
						Line.oEndPos.nY = Line.oPosition.nY;
						g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);

						Line.oEndPos.nX = Line.oPosition.nX;
						if((i == m_nTopItemIndex && !pItem->IsNode) || bIsPrevNode)
							Line.oPosition.nY = y - (m_nFontSize - 12) - m_nRowDis;
						else
							Line.oPosition.nY = y - NODE_RECT - (m_nFontSize - 12) - m_nRowDis;
						g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);

						Line.oPosition.nX = x + NODE_RECT_DIV2;
						Line.oPosition.nY -= NODE_RECT;
						Line.oEndPos.nX = Line.oPosition.nX;
						Line.oEndPos.nY += NODE_RECT;
						g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
					}
				}
			}

			const char* pShowString = TGetLimitLenString((const char*)(pItem->String), -1, szBuffer, nMaxLen);
			int nLen = strlen(pShowString);
			g_pRepresentShell->OutputText(m_nFontSize, pShowString, nLen, nLeft, y, Color, 0, TEXT_IN_SINGLE_PLANE_COORD, BorderColor);
			y += m_nFontSize + 1 + m_nRowDis;
			nCount++;
			if(nCount >= GetVisibleLineCount())
				break;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：查找列表（显示）字串
//--------------------------------------------------------------------------
int	KWndList2::FindString(const char* pString)
{
	if (pString)
	{
		unsigned int nLen = strlen(pString);
		for (int i = 0; i < m_nNumItem; i++)
		{
			KWndList2Item*	pItem = (KWndList2Item*)m_pContent[i];
			if (nLen == pItem->StringSize &&
				memcmp(pItem->String, pString, nLen) == 0)
				return i;
		}
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：设置列表项关联数据的值
//--------------------------------------------------------------------------
int	KWndList2::SetItemData(int nIndex, int nData)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		((KWndList2Item*)m_pContent[nIndex])->Data = nData;
		return 0;
	}
	return WNDLIST_ERROR;
}

int KWndList2::GetCurCount()
{
	int nCount = m_nNumItem;
	if(m_nNodeList)
	{
		for(int nIndex=0;nIndex< m_nNumItem;nIndex++)
		{
			KWndList2Item* pItem = (KWndList2Item*)m_pContent[nIndex];
			if(pItem->IsNode && !pItem->Open)
			{
				for(int i=nIndex+1;i<m_nNumItem;i++)
				{
					KWndList2Item* pTemp = (KWndList2Item*)m_pContent[i];
					if(pTemp->Data == pItem->Data)
						nCount--;
				}
			}
		}
	}
	return nCount;
}	


//--------------------------------------------------------------------------
//	功能：获得列表项数据中的一个整数
//--------------------------------------------------------------------------
int KWndList2::GetItemData(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		return (((KWndList2Item*)m_pContent[nIndex])->Data);
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：获得列表项数据中的一个整数
//--------------------------------------------------------------------------
int	KWndList2::SetItemColor(int nIndex, unsigned int Color)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		((KWndList2Item*)m_pContent[nIndex])->Color = Color;
		return 0;
	}
	return m_ItemColor;
}

//--------------------------------------------------------------------------
//	功能：获得列表项数据中的一个整数
//--------------------------------------------------------------------------
unsigned int KWndList2::GetItemColor(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nNumItem)
	{
		return (((KWndList2Item*)m_pContent[nIndex])->Color);
	}
	return m_ItemColor;
}
//--------------------------------------------------------------------------
//	功能：获得列表项的(显示)字串
//--------------------------------------------------------------------------
int KWndList2::GetString(int nIndex, char* pBuffer, unsigned int nSize)
{
	if (nIndex >= 0 && nIndex < m_nNumItem && pBuffer)
	{
		KWndList2Item* pItem = (KWndList2Item*)m_pContent[nIndex];
		if (pItem->StringSize < nSize)
		{
			memcpy(pBuffer, pItem->String, pItem->StringSize);
			pBuffer[pItem->StringSize] = 0;
			return pItem->StringSize;
		}
	}
	return WNDLIST_ERROR;
}

//--------------------------------------------------------------------------
//	功能：设置被显示的最顶列表项的索引值
//--------------------------------------------------------------------------
void KWndList2::SetTopItemIndex(int nTopItemIndex)
{
	if (nTopItemIndex >= 0 && nTopItemIndex < m_nNumItem)
	{
		if(m_nNodeList)
		{
			if (nTopItemIndex > 0)
			{
				for(int i=0;i< nTopItemIndex;i++)
				{
					KWndList2Item* pItem = (KWndList2Item*)m_pContent[i];
					if(pItem->IsNode && !pItem->Open)
					{
						for(int j=i+1;j<m_nNumItem;j++)
						{
							KWndList2Item* pTemp = (KWndList2Item*)m_pContent[j];
							if(pTemp->Data == pItem->Data)
							{
								nTopItemIndex++;
							}
						}
					}
				}
			}
		}
		m_nTopItemIndex = nTopItemIndex;
	}
}


//--------------------------------------------------------------------------
//	功能：获得列表框可以同时显示的项的数目
//--------------------------------------------------------------------------
int	KWndList2::GetVisibleLineCount() const
{
	return m_Height / (m_nFontSize + 1 + m_nRowDis);
}


int	KWndList2::GetVisibleNodeCount(int nTopIndex) const
{
	int nCount=0;
	if(m_nNodeList)
	{
		for(int nIndex=0;nIndex< nTopIndex;nIndex++)
		{
			KWndList2Item* pItem = (KWndList2Item*)m_pContent[nIndex];
			if(pItem->IsNode && !pItem->Open)
			{
				for(int i=nIndex+1;i<nTopIndex;i++)
				{
					KWndList2Item* pTemp = (KWndList2Item*)m_pContent[i];
					if(pTemp->Data == pItem->Data)
						nCount++;
				}
			}
		}
	}
	return nCount;
}