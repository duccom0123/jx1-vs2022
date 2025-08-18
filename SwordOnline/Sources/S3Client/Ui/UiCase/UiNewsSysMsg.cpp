/*********************************************************************************
*文件    ：UiNewsSysMsg
*创建人  ：Fyt(Fan Zhanpeng)
*创建时间：08-01-2003
*功能描述：游戏中的新消息通知窗口
**********************************************************************************/
#include "KWin32.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "UiNewsSysMsg.h"
#include "../Elem/Wnds.h"
#include "../Elem/UiImage.h"
#include "../UiBase.h"
#include "../../../Engine/Src/Text.h"
#include "UiFaceSelector.h"
#include "UiNewsSysMsg.h"
#include <time.h>

#define NEWS_SYSMSG_INI "UiNewsSysMsg.ini"
#define SHOW_INTERVAL			3000
#define	MAX_NORMAL_SHOW_TIMES	3

extern iRepresentShell *g_pRepresentShell;
KUiNewsSysMsg* KUiNewsSysMsg::m_pSelf = NULL;
int			KUiNewsSysMsg::ms_nSystemMessageCount = 0;

/*********************************************************************************
*功能：构造
**********************************************************************************/
KUiNewsSysMsg::KUiNewsSysMsg()
{
	m_pHead    = NULL;
	m_nFontSize = 8;
}

/*********************************************************************************
*功能：增加一条消息在队列中
**********************************************************************************/
bool KUiNewsSysMsg::AddMessage(KNewsSysMsg* pMsg)
{
	KNewsSysMsgNode *pNew = NULL;
	if (pMsg)
	{
		pNew = (KNewsSysMsgNode*)malloc(sizeof(KNewsSysMsgNode));
		if (pNew)
		{
			pNew->Msg = *pMsg;
			pNew->pNext = m_pHead;
			m_pHead = pNew;
			ms_nSystemMessageCount++;
			if(ms_nSystemMessageCount > MAX_NORMAL_SHOW_TIMES)
				RemoveSystemMessage(0);
		}
	}
	return (pNew != NULL);
}

int KUiNewsSysMsg::GetSystemMessageCount()
{
	return ms_nSystemMessageCount;
}

bool KUiNewsSysMsg::RemoveSystemMessage(int nIndex)
{
	bool	bOk = false;
	if (nIndex >= 0 && nIndex < ms_nSystemMessageCount)
	{
		nIndex = ms_nSystemMessageCount - nIndex - 1;
		KNewsSysMsgNode* pToRemove = NULL;
		if (nIndex == 0)
		{
			pToRemove = m_pHead;
			m_pHead = pToRemove->pNext;
		}
		else
		{
			KNewsSysMsgNode* pPre = m_pHead;
			while(nIndex > 1)
			{
				pPre = pPre->pNext;
				nIndex --;
			};
			pToRemove = pPre->pNext;
			pPre->pNext = pToRemove->pNext;
		}
		free (pToRemove);
		ms_nSystemMessageCount --;
		bOk = true;
	}
	return bOk;
}

const KNewsSysMsg* KUiNewsSysMsg::GetSystemMessage(int nIndex)
{
	KNewsSysMsg* pRet = NULL;
	if (nIndex >= 0 && nIndex < ms_nSystemMessageCount)
	{
		nIndex = ms_nSystemMessageCount - nIndex - 1;
		KNewsSysMsgNode* pNode = m_pHead;
		while(nIndex > 0)
		{
			pNode = pNode->pNext;
			nIndex --;
		}
		pRet = &pNode->Msg;
	}
	return pRet;
}

/*********************************************************************************
*功能：打开窗口
**********************************************************************************/
KUiNewsSysMsg* KUiNewsSysMsg::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiNewsSysMsg;
		if (m_pSelf)
		{
			m_pSelf->Initialize();
		}
	}
	if (m_pSelf)
	{
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}


/*********************************************************************************
*功能：如果窗口正被显示，则返回实例指针
**********************************************************************************/
KUiNewsSysMsg* KUiNewsSysMsg::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}


/*********************************************************************************
*功能：关闭窗口
**********************************************************************************/
void KUiNewsSysMsg::CloseWindow(BOOL bDestory)
{
	if (m_pSelf)
	{		
		if(bDestory == TRUE)
		{
			m_pSelf->ClearAll();
			m_pSelf->Destroy();
		    m_pSelf = NULL;
		}
		else
		{
			m_pSelf->Hide();
		}
	}
}

void KUiNewsSysMsg::ClearAll()
{
	if (m_pHead)
	{
		free (m_pHead);
		m_pHead = NULL;
	}
	ms_nSystemMessageCount = 0;
}
/*********************************************************************************
*功能：初始化
**********************************************************************************/
void KUiNewsSysMsg::Initialize()
{
	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}

/*********************************************************************************
*功能：载入界面方案
**********************************************************************************/
void KUiNewsSysMsg::LoadScheme(const char* pszScheme)
{
	if (m_pSelf && pszScheme && pszScheme[0])
	{
		char    szBuf[128];
		sprintf(szBuf, "%s\\%s", pszScheme, NEWS_SYSMSG_INI);
		KIniFile	Ini;
		if (Ini.Load(szBuf))
		{
			m_pSelf->Init(&Ini, "Main");
			Ini.GetInteger("Main", "IndentH",	0, &m_pSelf->m_nIndentH);
			Ini.GetInteger("Main", "IndentV",   0, &m_pSelf->m_nIndentV);
			Ini.GetInteger("Main", "Font",      0, &m_pSelf->m_nFontSize);
			if(m_pSelf->m_nFontSize < 8)
		    	m_pSelf->m_nFontSize = 8;
			Ini.GetString ("Main", "TextColor", "0,0,0", szBuf, sizeof(szBuf));
		    m_pSelf->m_uTextColor = GetColor(szBuf);
			Ini.GetString ("Main", "TextBorderColor", "0,0,0", szBuf, sizeof(szBuf));
			m_pSelf->m_uTextBorderColor = GetColor(szBuf);
			Ini.GetString("Main", "ShadowColor", "", szBuf, sizeof(szBuf));
			m_pSelf->m_uShadowColor = (GetColor(szBuf) & 0xffffff);
			int nAlpha;
			Ini.GetInteger("Main", "ShadowColorAlpha", 128, &nAlpha);
			if (nAlpha > 255 || nAlpha < 0)
				m_pSelf->m_uShadowColor |= 0x16000000;
			else
				m_pSelf->m_uShadowColor |= (((255 - nAlpha) << 21) & 0xff000000);
			Ini.GetInteger("Main", "ShowInterval" ,  SHOW_INTERVAL,   (int*)&m_pSelf->m_uShowInterval);
		}
	}
}

/*********************************************************************************
*功能：两个字 - 穿透！！！
**********************************************************************************/
int KUiNewsSysMsg::PtInWindow(int x, int y)
{
	return 0;
}

//新闻消息真的来了
void KUiNewsSysMsg::MessageArrival(const char* pMsg, unsigned short nMsgLength)
{
	if (m_pSelf && pMsg &&
		nMsgLength > 0)
	{
		KNewsSysMsg pSys;
		memcpy(pSys.sMsg, pMsg, nMsgLength);
		pSys.nMsgLen = TEncodeText(pSys.sMsg, nMsgLength, &pSys.nCurLen);
		pSys.uTime = IR_GetCurrentTime() + m_pSelf->m_uShowInterval;
		m_pSelf->AddMessage((KNewsSysMsg*)&pSys);
	}
}

/*********************************************************************************
*功能：显示消息，并在显示完成后对消息进行进一步处理
**********************************************************************************/
void KUiNewsSysMsg::PaintWindow()
{
    if(ms_nSystemMessageCount && g_pRepresentShell)
	{
		KWndWindow::PaintWindow();
		KOutputTextParam Param;
		Param.Color       = m_uTextColor;
		Param.BorderColor = m_uTextBorderColor;
		Param.nSkipLine   = 0;
		Param.nNumLine    = 1;
		Param.nX		  = 0;
		Param.nY          = 0;
		Param.nZ          = TEXT_IN_SINGLE_PLANE_COORD;
		Param.nVertAlign  = 0;
		Param.bPicPackInSingleLine = true;
		
		unsigned int	uCurrentTime = IR_GetCurrentTime();	
		int i,x,y,nCount=0;
		x = m_nAbsoluteLeft;
		y = m_nAbsoluteTop;

		for(i = 0; i < ms_nSystemMessageCount;i++)
		{
			const KNewsSysMsg* pMsg = GetSystemMessage(i);
			if (pMsg == NULL)
				break;
			if(uCurrentTime > pMsg->uTime)
			{
				RemoveSystemMessage(i);
				continue;
			}
			int w = pMsg->nCurLen * (m_nFontSize / 2);
			Param.nX = (RESOLUTION_WIDTH - w) / 2 + m_nIndentH;
			Param.nY = y + nCount * (m_nFontSize * 2) + m_nIndentV;
			g_pRepresentShell->OutputRichText(m_nFontSize, &Param, pMsg->sMsg, pMsg->nMsgLen, 0);

			/*KRULine	Line;
			for (j =0 ; j < pMsg->nCurLen; j++)
			{
				Line.Color.Color_dw = m_uTextColor;
				Line.oPosition.nX = Param.nX;
				Line.oPosition.nY = Param.nY + m_nFontSize + 1;
				Line.oEndPos.nZ = Line.oPosition.nZ = 0;
				Line.oEndPos.nX = Line.oPosition.nX + w;
				Line.oEndPos.nY = Line.oPosition.nY;
				g_pRepresentShell->DrawPrimitives(1, &Line, RU_T_LINE, true);
			}*/
			nCount++;
		}		
	}
}