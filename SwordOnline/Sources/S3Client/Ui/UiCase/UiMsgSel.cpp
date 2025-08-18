// -------------------------------------------------------------------------
//	ÎÄ¼þÃû		£º	UiMsgSel.h
//	´´½¨Õß		£º	Wooy(Wu yue)
//	´´½¨Ê±¼ä	£º	2003-1-6
//	¹¦ÄÜÃèÊö	£º	´ø¹ö¶¯ÌõµÄÏûÏ¢Ñ¡Ôñ
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/Wnds.h"
#include "UiMsgSel.h"
#include "../../../engine/src/KDebug.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../../../engine/src/kstrbase.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI	"UiMsgSel.ini"
#define	SCHEME_INIP	"UiMsgSelPic.ini"

KUiMsgSel* KUiMsgSel::m_pSelf = NULL;


KUiMsgSel::KUiMsgSel()
{
	m_uLeftTime = 0;
	m_bAutoUp = false;
	m_bAutoDown = false;
	m_uLastScrollTime = 0;
	memset(m_szPic, 0, sizeof(m_szPic));
}

KUiMsgSel::~KUiMsgSel()
{
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´ò¿ª´°¿Ú£¬·µ»ØÎ¨Ò»µÄÒ»¸öÀà¶ÔÏóÊµÀý
//--------------------------------------------------------------------------
KUiMsgSel* KUiMsgSel::OpenWindow(KUiQuestionAndAnswer* pQAA, KUiImage *pPic) 
{ 
    if (m_pSelf == NULL) 
    { 
        m_pSelf = new KUiMsgSel; 
        if (m_pSelf) 
        { 
            m_pSelf->Initialize(pPic); 
        } 
    } 
    if (m_pSelf) 
    {
        UiSoundPlay(UI_SI_WND_OPENCLOSE); 
        m_pSelf->BringToTop(); 
        m_pSelf->Show(pQAA); 
    } 
    return m_pSelf; 
}  

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º¹Ø±ÕÏú»Ù´°¿Ú
//--------------------------------------------------------------------------
void KUiMsgSel::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->m_bAutoUp = false;
		m_pSelf->m_bAutoDown = false;

		Wnd_ReleaseExclusive((KWndWindow*)m_pSelf);
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
		else
			m_pSelf->Hide();		
	}
}


KUiMsgSel* KUiMsgSel::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}
//--------------------------------------------------------------------------
//	¹¦ÄÜ£º³õÊ¼»¯
//--------------------------------------------------------------------------
int KUiMsgSel::Initialize(KUiImage *pPic)
{
	if(pPic)
	{
		memcpy(&m_pPic ,pPic, sizeof(KUiImage));
		if (m_pPic.Name[0])
			AddChild(&m_Image);
	}
	else
		memset(&m_pPic, 0, sizeof(KUiImage));
	AddChild(&m_MsgScrollList);
	AddChild(&m_MessageScroll);
	AddChild(&m_MessageList);
	m_MessageList.SetScrollbar(&m_MessageScroll);
	m_Style &= ~WND_S_VISIBLE;
	Wnd_AddWindow(this, WL_TOPMOST);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	return true;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÔØÈë´°¿ÚµÄ½çÃæ·½°¸
//--------------------------------------------------------------------------
void KUiMsgSel::LoadScheme(const char* pScheme)
{
	if (m_pSelf == NULL)
		return;
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, m_pSelf->m_pPic.Name[0]?SCHEME_INIP:SCHEME_INI);
	if (Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_MsgScrollList.Init(&Ini, "Select");
		m_pSelf->m_MessageScroll.Init(&Ini, "MessageScroll");
		m_pSelf->m_MessageList.Init(&Ini, "MessageList");
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£º´°¿ÚÏûÏ¢º¯Êý
//--------------------------------------------------------------------------
int KUiMsgSel::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WM_MOUSEMOVE:
		{
			int x = LOWORD(nParam) - m_nAbsoluteLeft;
			int y = HIWORD(nParam) - m_nAbsoluteTop;
			if (x >= 0 && x <= m_Width &&
				y >= 0 && y <= m_Height)
			{
				int nMLeft = 0;
				int nMTop = 0;
				m_MsgScrollList.GetMessageListBox()->GetPosition(&nMLeft, &nMTop);
				int nMWidth = 0;
				int nMHeight = 0;
				m_MsgScrollList.GetMessageListBox()->GetSize(&nMWidth, &nMHeight);
				if (x >= nMLeft && x <= nMLeft + nMWidth)
				{
					if (y <= nMTop && y >= nMTop - 50)
						m_bAutoUp = true;
					else
						m_bAutoUp = false;

					if (y >= nMTop + nMHeight && y <= nMTop + nMHeight + 50)
						m_bAutoDown = true;
					else
						m_bAutoDown = false;
				}
			}
		}
		break;
	case WND_N_LIST_ITEM_HIGHLIGHT:
		m_bAutoUp = false;
		m_bAutoDown = false;
		if (nParam >= 0)
		{
			m_MsgScrollList.GetMessageListBox()->SetCurSel(nParam);
			if(m_pPic.Name[0])
			{
				int nMWidth, nMHeight; 
				if(m_szPic[nParam][0])
				{
					m_Image.SetImage(ISI_T_SPR, m_szPic[nParam], true); 
				}
				else
				{
					m_Image.SetImage(ISI_T_SPR, m_pPic.Name, true); 
					m_Image.SetMaxFrame(m_pPic.Frame);
				}
				m_Image.GetSize(&nMWidth, &nMHeight); 
				m_Image.SetPosition(70-nMWidth/2, 70-nMHeight/2); 
			}
		}
		else
		{
			ChangeCurSel(true);	//¾­Êµ¼Ê²âÊÔÔÚnParam < 0 Ê±ÊÇÏòÏÂ
			nRet = 1;
		}
		break;
	case WND_N_LIST_ITEM_ACTIVE:
		m_bAutoUp = false;
		m_bAutoDown = false;
		if (nParam >= 0)
			OnClickMsg(nParam);
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_MessageScroll)
			m_MessageList.SetFirstShowLine(nParam);
		break;
	case WM_KEYDOWN:
		m_bAutoUp = false;
		m_bAutoDown = false;
		if (uParam == VK_RETURN)
		{
			nRet = m_MsgScrollList.GetMessageListBox()->GetCurSel();
			if (nRet >= 0)
				OnClickMsg(nRet);
			nRet = 1;
		}
		else if (uParam == VK_UP)
		{
			ChangeCurSel(false);
			nRet = 1;
		}
		else if (uParam == VK_DOWN)
		{
			ChangeCurSel(true);
			nRet = 1;
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
		break;
	}
	return 0;
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏìÓ¦µã»÷°´Å¥
//--------------------------------------------------------------------------
void KUiMsgSel::OnClickMsg(int nMsg)
{
	if(m_pSelf)
	{
		m_pSelf->CloseWindow(true);
		g_pCoreShell->OperationRequest(GOI_QUESTION_CHOOSE, 0, nMsg);
	}
}

void KUiMsgSel::ChangeCurSel(bool bNext)
{
	int nRet = m_MsgScrollList.GetMessageListBox()->GetCurSel();
	if (bNext)
	{
		if (nRet < m_MsgScrollList.GetMessageListBox()->GetMsgCount() - 1)
			nRet = m_MsgScrollList.GetMessageListBox()->SetCurSel(nRet + 1);
	}
	else
	{
		if (nRet  > 0)
			nRet = m_MsgScrollList.GetMessageListBox()->SetCurSel(nRet - 1);
	}
}

//--------------------------------------------------------------------------
//	¹¦ÄÜ£ºÏÔÊ¾´°¿Ú
//--------------------------------------------------------------------------
void KUiMsgSel::Show(KUiQuestionAndAnswer* pQAA)
{
	if (pQAA && pQAA->AnswerCount >= 0)
	{
		m_MsgScrollList.GetMessageListBox()->Clear();
		m_MessageList.Clear();
		m_MessageList.AddOneMessage(pQAA->Question, pQAA->QuestionLen);
		int i = 0;
		if (pQAA->AnswerCount <= 0)
		{
			m_MsgScrollList.GetMessageListBox()->AddOneMessage("KÕt thóc ®èi tho¹i", -1);
		}
		else
		{
			for (i = 0; i < pQAA->AnswerCount ; i++)
			{
				char* pAnswer = strstr(pQAA->Answer[i].AnswerText, "LINK:");
				if(m_pPic.Name[0] && pAnswer)
				{
					g_StrCpyLen(m_szPic[i], pAnswer+5, sizeof(m_szPic[i]));
					*pAnswer = 0;
					if (!m_MsgScrollList.GetMessageListBox()->AddOneMessage(pQAA->Answer[i].AnswerText, pQAA->Answer[i].AnswerLen))
						m_MsgScrollList.GetMessageListBox()->AddOneMessage("[--KÞch b¶n xö lý lçi--]", -1);
				}
				else
				{
					m_szPic[i][0] = 0;
					if (!m_MsgScrollList.GetMessageListBox()->AddOneMessage(pQAA->Answer[i].AnswerText, pQAA->Answer[i].AnswerLen))
						m_MsgScrollList.GetMessageListBox()->AddOneMessage("[--KÞch b¶n xö lý lçi--]", -1);
				}
			}				
		}
	}
	KWndShowAnimate::Show();
	Wnd_SetExclusive((KWndWindow*)this);

	m_uLeftTime = pQAA->LeftTime;
	m_uLastScrollTime = IR_GetCurrentTime();
	m_bAutoUp = false;
	m_bAutoDown = false;
	
	if (m_pPic.Name[0])
	{
		m_Image.SetImage(ISI_T_SPR, m_pPic.Name, true); 
		int nMWidth, nMHeight; 
		m_Image.GetSize(&nMWidth, &nMHeight); 
		m_Image.SetPosition(70-nMWidth/2, 70-nMHeight/2); 
		m_Image.SetMaxFrame(m_pPic.Frame); 
	}
}

void KUiMsgSel::Breathe()
{    
	if(m_uLeftTime > 0)
	{
		m_uLeftTime--;
		if(m_uLeftTime == 0)
			CloseWindow(true);
	}

	if (m_Image.IsVisible())
        m_Image.NextFrame();  
		
	if (m_bAutoUp)
	{
		if (IR_IsTimePassed(200, m_uLastScrollTime))
		{
			ChangeCurSel(false);
			m_uLastScrollTime = IR_GetCurrentTime();
		}
	}

	if (m_bAutoDown)
	{
		if (IR_IsTimePassed(200, m_uLastScrollTime))
		{
			ChangeCurSel(true);
			m_uLastScrollTime = IR_GetCurrentTime();
		}
	}
}