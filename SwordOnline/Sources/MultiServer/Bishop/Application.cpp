#include "stdafx.h"
#include "application.h"
#include "resource.h"

#include <process.h>
#include <time.h>
#include <shellapi.h>

#include "GameServer.h"
#include "GamePlayer.h"

#include "KProtocolDef.h"
#include "AccountLoginDef.h"
#include "Buffer.h"

#include "msg_define.h"
#include "Macro.h"
#include "../Engine/Src/KWin32.h"
#include "../Engine/Src/KSG_MD5_String.h"
#include "inoutmac.h"

#include "Inifile.h"
#include "Utils.h"
#include "tstring.h"

using OnlineGameLib::Win32::CIniFile;
using OnlineGameLib::Win32::GetAppFullPath;
using OnlineGameLib::Win32::_tstring;
using OnlineGameLib::Win32::CCriticalSection;

#include "SmartClient.h"
#include "../../Engine/Src/KGLog.h"

using OnlineGameLib::Win32::CPackager;

#define NOTIFY_MAINTENANCE_ANNOUNCE	"M¸y chñ sÏ b¶o tr× ®Þnh kú sau %d phót n÷a, thêi gian b¶o tr× dù kiÕn %d phót"
#define MAINTENANCE_ANNOUNCE		"M¸y chñ b¶o tr×, hÑn gÆp l¹i"

static const char szBishopClassName[] = "BishopClass";
static char szGameServerFilePath[] = ".\\GameServer.exe";
static size_t g_BackupSleepTime = 0;  //±¸·ÝÏß³Ì×Ô¶¯¹ÒÆðÊ±¼ä
static DWORD g_BackupSpaceTime = 0;
//static const DWORD g_dwServerIdentify = 0xAEFC07B5;

HINSTANCE		CBishopApp::m_hInst = NULL;
CNetwork		CBishopApp::m_theNetwork;

CIntercessor* CBishopApp::m_pIntercessor = NULL;

///////////////////////////////////////////////////////////////////////////////////////////


CBishopApp::CBishopApp()
	: m_theOnlyOneInstance(NULL, FALSE, TRUE, szBishopClassName)
{
}

CBishopApp::CBishopApp(HINSTANCE hInstance)
	: m_theOnlyOneInstance(NULL, FALSE, TRUE, szBishopClassName)
{
	m_hInst = hInstance;
}

CBishopApp::~CBishopApp()
{

}

int CBishopApp::Run()
{
	/* Seed the random-number generator with current time so that
	 * the numbers will be different every time we run.
	 */
	srand((unsigned)time(NULL));

	m_theNetwork.Create();

	g_theSmartClient.Open(m_theNetwork.GetAccSvrIP(), m_theNetwork.GetAccSvrPort(), 0, "fsjx", "1234");

	if (g_theSmartClient.Valid())
	{
		m_pIntercessor = new CIntercessor(500, m_theNetwork);

		ASSERT(m_pIntercessor);

		//MainDialog();


		if (!m_pIntercessor->Create(0))
		{
			m_pIntercessor->Destroy();
		}
		else
		{
			KGLogPrintf(LOG_INFO, "[Bishop] start success\n");
			MSG msg;
			// Main message loop:
			while (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);

			}
		}


	}

	m_theNetwork.Destroy();

	return 0;
}

/*
 * Log on to system
 */
bool CBishopApp::LoginSystem()
{
	if (WM_SERVER_LOGIN_SUCCESSFUL == ::DialogBox(m_hInst,
		MAKEINTRESOURCE(IDD_DLG_LOGIN),
		NULL,
		(DLGPROC)LoginDlgProc))
	{
		return true;
	}

	return false;
}

BOOL CALLBACK CBishopApp::LoginDlgProc(HWND hwndDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	char szBuffer[MAX_PATH];

	switch (message)
	{
	case WM_SERVER_LOGIN_SUCCESSFUL:

		::EndDialog(hwndDlg, WM_SERVER_LOGIN_SUCCESSFUL);

		break;

	case WM_SERVER_LOGIN_FAILED:

		switch (wParam)
		{
		case enumUsrNamePswdErr:

			::LoadString(m_hInst, IDS_NAMEPSWD_ERROR, szBuffer, MAX_PATH);

			::MessageBox(hwndDlg, szBuffer, NULL, MB_ICONEXCLAMATION);

			::PostMessage(hwndDlg, WM_LEAVE_AFFIRM, 0L, 0L);

			break;

		case enumConnectFailed:
		case enumIPPortErr:

			::LoadString(m_hInst, IDS_NETWORK_ERROR, szBuffer, MAX_PATH);

			::MessageBox(hwndDlg, szBuffer, NULL, MB_ICONEXCLAMATION);

			::PostMessage(hwndDlg, WM_LEAVE_AFFIRM, 0L, 0L);

			break;

		case enumException:
		default:

			::MessageBox(hwndDlg, "Exception!", NULL, MB_ICONEXCLAMATION);

			::EndDialog(hwndDlg, WM_SERVER_LOGIN_FAILED);

			break;
		}

		break;

	case WM_ENTER_AFFIRM:

		EnterToAffirm(hwndDlg);

		break;

	case WM_LEAVE_AFFIRM:

		LeaveToAffirm(hwndDlg);

		break;

	case WM_INITDIALOG:

		::SetFocus(::GetDlgItem(hwndDlg, IDC_EDIT_USERNAME));

		::SetDlgItemText(hwndDlg, IDC_EDIT_LOGONTO_IP, m_theNetwork.GetAccSvrIP());
		::SetDlgItemInt(hwndDlg, IDC_EDIT_LOGONTO_PORT, m_theNetwork.GetAccSvrPort(), FALSE);

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDOK:

			if (CheckUserInfo(hwndDlg))
			{
				::PostMessage(hwndDlg, WM_ENTER_AFFIRM, 0L, 0L);
			}

			break;

		case IDCANCEL:

			::EndDialog(hwndDlg, WM_SERVER_LOGIN_FAILED);

			return TRUE;
		}
	}

	return FALSE;
}

void CBishopApp::EnterToAffirm(HWND hwndDlg)
{
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_USERNAME), FALSE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_PASSWORD), FALSE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_LOGONTO_IP), FALSE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_LOGONTO_PORT), FALSE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDOK), FALSE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDCANCEL), FALSE);

	/*
	 * Start up a listening thread to wait a required confirm from the account server
	 */
	 //DWORD dwThreadID = 0;

	 //HANDLE hThread = ::CreateThread( NULL, 0, ServerLoginRoutine, ( void * )hwndDlg, 0, &dwThreadID );

	 //SAFE_CLOSEHANDLE( hThread );

	ServerLoginRoutine(hwndDlg);
}

void CBishopApp::LeaveToAffirm(HWND hwndDlg)
{
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_USERNAME), TRUE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_PASSWORD), TRUE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_LOGONTO_IP), TRUE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDC_EDIT_LOGONTO_PORT), TRUE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDOK), TRUE);
	::EnableWindow(::GetDlgItem(hwndDlg, IDCANCEL), TRUE);
}

bool CBishopApp::CheckUserInfo(HWND hwndDlg)
{
	UINT	nLen = 0;
	char	szBuffer[MAX_PATH];

	if (0 == (nLen = ::GetDlgItemText(hwndDlg, IDC_EDIT_USERNAME, szBuffer, MAX_PATH)))
	{
		::SetFocus(::GetDlgItem(hwndDlg, IDC_EDIT_USERNAME));

		::MessageBox(hwndDlg, "Please input a valid username!", "Warning", MB_ICONASTERISK);

		return false;
	}

	if (0 == (nLen = ::GetDlgItemText(hwndDlg, IDC_EDIT_LOGONTO_IP, szBuffer, MAX_PATH)))
	{
		::SetFocus(::GetDlgItem(hwndDlg, IDC_EDIT_LOGONTO_IP));

		::MessageBox(hwndDlg, "Please input a valid IP!", "Warning", MB_ICONASTERISK);

		return false;
	}

	BOOL bTranslated = FALSE;

	if (0 == (nLen = ::GetDlgItemInt(hwndDlg, IDC_EDIT_LOGONTO_PORT, &bTranslated, TRUE)))
	{
		::SetFocus(::GetDlgItem(hwndDlg, IDC_EDIT_LOGONTO_PORT));

		::MessageBox(hwndDlg, "Please input a valid port!", "Warning", MB_ICONASTERISK);

		return false;
	}

	return true;
}

DWORD WINAPI CBishopApp::ServerLoginRoutine(HWND hwndDlg)
{
	ASSERT(hwndDlg);

	/*
	 * Ask for log on to the account server
	 */

	UINT nPort = 0;

	char szBuffer[MAX_PATH];
	char szUsername[NAME_PWD_EX_LEN];
	char szPassword[NAME_PWD_EX_LEN];

	::GetDlgItemText(hwndDlg, IDC_EDIT_LOGONTO_IP, szBuffer, MAX_PATH);

	BOOL bTranslated = TRUE;

	nPort = ::GetDlgItemInt(hwndDlg, IDC_EDIT_LOGONTO_PORT, &bTranslated, TRUE);

	::GetDlgItemText(hwndDlg, IDC_EDIT_USERNAME, szUsername, NAME_PWD_EX_LEN);

	::GetDlgItemText(hwndDlg, IDC_EDIT_PASSWORD, szPassword, NAME_PWD_EX_LEN);

	g_theSmartClient.Open(szBuffer, nPort, hwndDlg, szUsername, szPassword);

	/*	IClient *pAccSvrClient = m_theNetwork.CreateAccSvrClient( szBuffer, nPort );

		if ( !pAccSvrClient )
		{
			::PostMessage( hwndDlg, WM_SERVER_LOGIN_FAILED, enumConnectFailed, 0L );

			SAFE_RELEASE( pAccSvrClient );
			return 1L;
		}
	*/
	if (!g_theSmartClient.Valid())
	{
		//		::PostMessage( hwndDlg, WM_SERVER_LOGIN_FAILED, enumConnectFailed, 0L );

		return 1L;
	}

	//	SAFE_RELEASE( pAccSvrClient );

	return 0L;
}

BOOL CBishopApp::RegisterMainWndClass()
{
	WNDCLASSEX wcx;

	/*
	 * Fill in the window class structure with parameters
	 * that describe the main window.
	 */

	wcx.cbSize = sizeof(wcx);					// size of structure 
	wcx.style = CS_HREDRAW | CS_VREDRAW;		// redraw if size changes 
	wcx.lpfnWndProc = (WNDPROC)MainWndProc;	// points to window procedure 
	wcx.cbClsExtra = 0;							// no extra class memory 
	wcx.cbWndExtra = 0;							// no extra window memory 
	wcx.hInstance = m_hInst;					// handle to instance 
	wcx.hIcon = LoadIcon(NULL,
		IDI_APPLICATION);						// predefined app. icon 
	wcx.hCursor = LoadCursor(NULL,
		IDC_ARROW);								// predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(
		WHITE_BRUSH);							// white background brush 
	wcx.lpszMenuName = NULL;					// name of menu resource 
	wcx.lpszClassName = _T("IntercessorClass");	// name of window class 
	wcx.hIconSm = (HICON)LoadImage(m_hInst,		// small class icon 
		MAKEINTRESOURCE(IDI_LOCK),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	/*
	 * Register the window class.
	 */
	return ::RegisterClassEx(&wcx);
}

UINT CBishopApp::MainDialog()
{
	/*
	 * Create the main window as dialog.
	 */
	 /*	if ( !RegisterMainWndClass() )
		 {
			 return 0L;
		 }
	 */
	HWND hwndMain = ::CreateDialog(m_hInst,
		MAKEINTRESOURCE(IDD_DLG_INTERCESSOR),
		NULL,
		(DLGPROC)MainWndProc);

	ASSERT(hwndMain);

	::ShowWindow(hwndMain, SW_SHOWNORMAL);
	::UpdateWindow(hwndMain);

	BOOL bRet;
	MSG msg;

	while ((bRet = ::GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if (!IsWindow(hwndMain) || !IsDialogMessage(hwndMain, &msg))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

/*
 *
 * MainWndProc() - Main window callback procedure.
 *
 */

static UINT g_nEvent = 0x100;
static UINT g_nTimer = 0;

static UINT g_nBackupEvent = 0x200;
static UINT g_nBackupTimer = 0;

BOOL CALLBACK CBishopApp::MainWndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{
	static size_t nGameServerNumber = 0;
	static char szInfo[64];

	UINT uLength = 0;
	TCHAR szAnnounceText[260];
	TCHAR szAccountName[32];

	switch (msg)
	{
	case WM_INITDIALOG:

		/*
		 * Initialize
		 */
		InitMainDlg(hwnd);

		return TRUE;

		break;

	case WM_CLOSE:

		if (IDYES == ::MessageBox(hwnd, "Are you sure quit?", "Info", MB_YESNO | MB_ICONQUESTION))
		{
			if (g_nTimer)
			{
				::KillTimer(hwnd, g_nEvent);

				g_nTimer = 0;
			}

			if (g_nBackupTimer)
			{
				::KillTimer(hwnd, g_nBackupEvent);

				g_nBackupTimer = 0;
			}

			CloseMainDlg(hwnd);

			if (m_pIntercessor)
			{
				m_pIntercessor->Destroy();
			}

			::DestroyWindow(hwnd);
			::PostQuitMessage(0);
		}

		return TRUE;

		break;

	case WM_TIMER:
		if (g_nEvent == wParam)
		{
			LONG nc = CGamePlayer::GetCapability();
			LONG nw = CGamePlayer::GetWorking();

			sprintf(szInfo, "%d / %d", nw, nc);

			::SetDlgItemText(hwnd, IDC_STATIC_TASK, (LPCTSTR)szInfo);
		}
		else if (g_nBackupEvent == wParam)
		{
			/*time_t rawtime;
			struct tm * timeinfo;

			time ( &rawtime );
			timeinfo = localtime ( &rawtime );

			if(timeinfo->tm_hour == (g_BackupSleepTime == 0 ? 23 : (g_BackupSleepTime - 1)))
			{
				if(timeinfo->tm_min >= 50)
				{
					uLength = sprintf(szAnnounceText, NOTIFY_MAINTENANCE_ANNOUNCE, 60 - timeinfo->tm_min, g_BackupSpaceTime);
					szAnnounceText[uLength] = '\0';
					SendAnnounceText( hwnd, ( const char * )szAnnounceText, uLength, AP_NOTIFY_ALL_PLAYER, TRUE );
				}
			}
			else if(timeinfo->tm_hour == g_BackupSleepTime)
			{
				if(timeinfo->tm_min == 0)
				{
					uLength = sprintf(szAnnounceText, MAINTENANCE_ANNOUNCE);
					szAnnounceText[uLength] = '\0';
					SendAnnounceText( hwnd, ( const char * )szAnnounceText, uLength, AP_NOTIFY_GAMESERVER_SAFECLOSE, TRUE );
				}
				else if(timeinfo->tm_min == g_BackupSpaceTime)
				{
					::ShellExecute(NULL,"open",(char*)szGameServerFilePath,NULL,NULL,SW_SHOWNORMAL);
				}
			}*/
		}
		break;

	case WM_COMMAND:

		switch (wParam)
		{
		case IDOK:
			time_t rawtime;
			struct tm* timeinfo;

			time(&rawtime);
			timeinfo = localtime(&rawtime);

			char szText[64];
			sprintf(szText, "%04d%02d%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday);
			if ((int)(atoi(szText)) > KPROTOCOL_EXPIRATION_DATE)
			{
				MessageBox(hwnd, "Protocol Version is illegal",
					"Information", MB_OK | MB_ICONERROR);
				break;
			}
			::EnableWindow(GetDlgItem(hwnd, IDOK), FALSE);
			::EnableWindow(GetDlgItem(hwnd, IDC_EDIT_CLIENT_PORT), FALSE);
			::EnableWindow(GetDlgItem(hwnd, IDC_EDIT_GAMESVR_PORT), FALSE);

			::EnableWindow(GetDlgItem(hwnd, IDC_EDIT_ROLESVRIP), FALSE);
			::EnableWindow(GetDlgItem(hwnd, IDC_EDIT_ROLESVRPORT), FALSE);

			::CheckDlgButton(hwnd, IDC_CHECK_PLAYER, BST_CHECKED);
			::CheckDlgButton(hwnd, IDC_CHECK_GAMESVR, BST_CHECKED);

			::CheckDlgButton(hwnd, IDC_CHECK_ACCSVR, BST_CHECKED);

			UpdateVariable(hwnd);

			if (!m_pIntercessor->Create(hwnd))
			{
				::EnableWindow(GetDlgItem(hwnd, IDOK), TRUE);

				m_pIntercessor->Destroy();
			}
			else
			{
				g_nTimer = ::SetTimer(hwnd, g_nEvent, 5000, NULL);
				g_nBackupTimer = ::SetTimer(hwnd, g_nBackupEvent, 60000, NULL);

				::SetWindowText(hwnd, "Bishop - [Enable]");
			}

			break;

		case IDCANCEL:

			::PostMessage(hwnd, WM_CLOSE, 0L, 0L);

			break;

		case IDC_BTN_SEND_MSG:
		{
			BOOL bToAllGS = FALSE;
			if (BST_CHECKED == ::IsDlgButtonChecked(hwnd, IDC_INDEX_SEND2GAMESERVER))
			{
				bToAllGS = TRUE;
			}

			UINT uOption = AP_NOTIFY_ALL_PLAYER;
			if (BST_CHECKED == ::IsDlgButtonChecked(hwnd, IDC_INDEX_GS2CLOSE))
			{
				uOption = AP_NOTIFY_GAMESERVER_SAFECLOSE;
			}

			if (BST_CHECKED == ::IsDlgButtonChecked(hwnd, IDC_INDEX_WANINGGS2CLOSE))
			{
				uOption = AP_WARNING_ALL_PLAYER_QUIT;
			}

			uLength = ::GetDlgItemText(hwnd, IDC_EDIT_ANNOUNCE_MSG, szAnnounceText, 256);
			szAnnounceText[uLength] = '\0';

			SendAnnounceText(hwnd, (const char*)szAnnounceText, uLength, uOption, bToAllGS);
		}
		break;
		case IDC_BTN_PUNISH:
		{
			UINT uOption = AP_PUNISH_LOCK;
			if (BST_CHECKED == ::IsDlgButtonChecked(hwnd, IDC_PUNISH_LOCK))
			{
				uOption = AP_PUNISH_LOCK;
			}

			if (BST_CHECKED == ::IsDlgButtonChecked(hwnd, IDC_PUNISH_CHAT))
			{
				uOption = AP_PUNISH_CHAT;
			}
			uLength = ::GetDlgItemText(hwnd, IDC_EDIT_ACCOUNTNAME, szAccountName, 32);
			szAccountName[uLength] = '\0';

			PunishAccount(hwnd, (const char*)szAccountName, uLength, uOption);
		}
		break;
		case IDC_BTN_UNPUNISH:
		{
			UINT uOption = AP_PUNISH_LOCK;
			if (BST_CHECKED == ::IsDlgButtonChecked(hwnd, IDC_PUNISH_LOCK))
			{
				uOption = AP_PUNISH_LOCK;
			}

			if (BST_CHECKED == ::IsDlgButtonChecked(hwnd, IDC_PUNISH_CHAT))
			{
				uOption = AP_PUNISH_CHAT;
			}
			uLength = ::GetDlgItemText(hwnd, IDC_EDIT_ACCOUNTNAME, szAccountName, 32);
			szAccountName[uLength] = '\0';

			UnpunishAccount(hwnd, (const char*)szAccountName, uLength, uOption);
		}
		break;
		case IDC_BTN_GAMESVRINFO:
		{
			HWND hCtrl = ::GetDlgItem(hwnd, IDC_LIST_GAMESERVER);

			BOOL bRet = FALSE;

			if (hCtrl && ::IsWindow(hCtrl))
			{
				int nSelIndex = LB_ERR;

				if (LB_ERR != (nSelIndex = ::SendMessage(hCtrl, LB_GETCURSEL, 0, 0)))
				{
					UINT nSearchID = 0;

					if (LB_ERR != (nSearchID = ::SendMessage(hCtrl, LB_GETITEMDATA, nSelIndex, 0)))
					{
						IGServer* pGServer = CGameServer::GetServer(nSearchID);

						if (pGServer)
						{
							size_t nNum = pGServer->GetContent();

							bRet = ::SetDlgItemInt(hwnd, IDC_STATIC_GAMESERVER_CONTEXT, nNum, TRUE);
						}
					}
				}
			}

			if (!bRet)
			{
				::SetDlgItemText(hwnd, IDC_STATIC_GAMESERVER_CONTEXT, "Invalid");
			}
		}
		break;
		}

		break;

	case WM_SERVER_STATUS:

		switch (wParam)
		{
		case ACCOUNTSERVER_NOTIFY:

			if (CONNECTED == lParam)
			{
				::CheckDlgButton(hwnd, IDC_CHECK_ACCSVR, BST_CHECKED);
			}
			else // DICONNECTED
			{
				::CheckDlgButton(hwnd, IDC_CHECK_ACCSVR, BST_UNCHECKED);
			}

			break;

		case DBROLESERVER_NOTIFY:

			if (CONNECTED == lParam)
			{
				::CheckDlgButton(hwnd, IDC_CHECK_ROLESVR, BST_CHECKED);
			}
			else // DICONNECTED
			{
				::CheckDlgButton(hwnd, IDC_CHECK_ROLESVR, BST_UNCHECKED);
			}

			break;
		}

		break;

	case WM_GAMESERVER_EXCHANGE:
	{
		switch (wParam)
		{
		case ADD_GAMESERVER_ACTION:
		{
			if (m_pIntercessor)
			{
				const char* pInfo = m_pIntercessor->GetGameServerInfo(lParam);

				AddGameServerInfo(hwnd, lParam, pInfo);

				if (0 == nGameServerNumber++)
				{
					EnableGameSvrCtrl(hwnd, TRUE);

					HWND hCtrl = ::GetDlgItem(hwnd, IDC_LIST_GAMESERVER);

					if (hCtrl && ::IsWindow(hCtrl))
					{
						::SendMessage(hCtrl, LB_SETCURSEL, 0, 0);
					}
				}
			}
		}
		break;

		case DEL_GAMESERVER_ACTION:
		{
			if (m_pIntercessor)
			{
				if (--nGameServerNumber <= 0)
				{
					nGameServerNumber = 0;

					EnableGameSvrCtrl(hwnd, FALSE);

					::SetDlgItemText(hwnd, IDC_STATIC_GAMESERVER_CONTEXT, "Invalid");
				}

				DelGameServerInfo(hwnd, lParam);
			}
		}
		break;
		}
	}

	break;

	default:

		break;

	}

	/*
	 * Clean up any unused messages by calling DefWindowProc
	 */
	return FALSE;
	//return ::DefWindowProc( hwnd, msg, wParam, lParam );
}

void CBishopApp::AddGameServerInfo(HWND hDlg, UINT nID, const char* pInfo)
{
	if (pInfo && pInfo[0])
	{
		HWND hCtrl = ::GetDlgItem(hDlg, IDC_LIST_GAMESERVER);

		if (hCtrl && ::IsWindow(hCtrl))
		{
			int nIndex = ::SendMessage(hCtrl, LB_ADDSTRING, 0, (LPARAM)pInfo);

			if (LB_ERR != nIndex)
			{
				::SendMessage(hCtrl, LB_SETITEMDATA, nIndex, nID);
			}
		}
	}
}

void CBishopApp::DelGameServerInfo(HWND hDlg, UINT nID)
{
	HWND hCtrl = ::GetDlgItem(hDlg, IDC_LIST_GAMESERVER);

	if (hCtrl && ::IsWindow(hCtrl))
	{
		int nCount = ::SendMessage(hCtrl, LB_GETCOUNT, 0, 0);

		if (LB_ERR != nCount)
		{
			for (int i = 0; i < nCount; i++)
			{
				UINT nSearchID = 0;

				if (nID == (nSearchID = ::SendMessage(hCtrl, LB_GETITEMDATA, i, 0)))
				{
					::SendMessage(hCtrl, LB_DELETESTRING, i, 0);

					return;
				}
			}
		}
	}
}

void CBishopApp::InitMainDlg(HWND hDlg)
{
	CIniFile theConfigFile;

	_tstring sAppPath = GetAppFullPath(NULL);

	_tstring sConfigFile;

	sConfigFile = sAppPath + "Goddess.cfg";

	theConfigFile.SetFile(sConfigFile.c_str());

	g_BackupSleepTime = theConfigFile.ReadInteger("Setting", "BackupSleepTime", 0);
	g_BackupSpaceTime = theConfigFile.ReadInteger("Setting", "BackupSpaceTime", 0);

	::SetDlgItemText(hDlg, IDC_EDIT_ACCSVRIP, m_theNetwork.GetAccSvrIP());
	::SetDlgItemInt(hDlg, IDC_EDIT_ACCSVRPORT, m_theNetwork.GetAccSvrPort(), FALSE);

	::SetDlgItemText(hDlg, IDC_EDIT_ROLESVRIP, m_theNetwork.GetRoleSvrIP());
	::SetDlgItemInt(hDlg, IDC_EDIT_ROLESVRPORT, m_theNetwork.GetRoleSvrPort(), FALSE);

	::SetDlgItemInt(hDlg, IDC_EDIT_CLIENT_PORT, m_theNetwork.GetClientOpenPort(), FALSE);

	::SetDlgItemInt(hDlg, IDC_EDIT_GAMESVR_PORT, m_theNetwork.GetGameSvrOpenPort(), FALSE);

	::CheckDlgButton(hDlg, IDC_INDEX_SEND2GAMESERVER, BST_CHECKED);
	::CheckDlgButton(hDlg, IDC_RADIO_ANNOUNCE_OPTION, BST_CHECKED);
	::CheckDlgButton(hDlg, IDC_PUNISH_LOCK, BST_CHECKED);
	g_theSmartClient.m_hwndContain = hDlg;
}

void CBishopApp::CloseMainDlg(HWND hDlg)
{
	UpdateVariable(hDlg);

	g_theSmartClient.Close();
}

void CBishopApp::UpdateVariable(HWND hDlg)
{
	char szBuffer[MAX_PATH];

	::GetDlgItemText(hDlg, IDC_EDIT_ROLESVRIP, szBuffer, MAX_PATH);
	m_theNetwork.SetRoleSvrIP(szBuffer);

	UINT nValue = 0;
	BOOL bTranslated = TRUE;

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_ROLESVRPORT, &bTranslated, FALSE);
	m_theNetwork.SetRoleSvrPort(nValue);

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_CLIENT_PORT, &bTranslated, FALSE);
	m_theNetwork.SetClientOpenPort(nValue);

	nValue = ::GetDlgItemInt(hDlg, IDC_EDIT_GAMESVR_PORT, &bTranslated, FALSE);
	m_theNetwork.SetGameSvrOpenPort(nValue);

}

void CBishopApp::EnableGameSvrCtrl(HWND hwndDlg, BOOL nEnable)
{
	EnableCtrl(hwndDlg, IDC_BTN_GAMESVRINFO, nEnable);
	EnableCtrl(hwndDlg, IDC_LIST_GAMESERVER, nEnable);

	EnableCtrl(hwndDlg, IDC_RADIO_ANNOUNCE_OPTION, nEnable);
	EnableCtrl(hwndDlg, IDC_INDEX_GS2CLOSE, nEnable);
	EnableCtrl(hwndDlg, IDC_INDEX_WANINGGS2CLOSE, nEnable);
	EnableCtrl(hwndDlg, IDC_INDEX_SEND2GAMESERVER, nEnable);
	EnableCtrl(hwndDlg, IDC_INDEX_SEND2GS_ISSEL, nEnable);
	EnableCtrl(hwndDlg, IDC_EDIT_ANNOUNCE_MSG, nEnable);
	EnableCtrl(hwndDlg, IDC_BTN_SEND_MSG, nEnable);
	EnableCtrl(hwndDlg, IDC_PUNISH_LOCK, nEnable);
	EnableCtrl(hwndDlg, IDC_PUNISH_CHAT, nEnable);
	EnableCtrl(hwndDlg, IDC_EDIT_ACCOUNTNAME, nEnable);
	EnableCtrl(hwndDlg, IDC_BTN_PUNISH, nEnable);
	EnableCtrl(hwndDlg, IDC_BTN_UNPUNISH, nEnable);
}

void CBishopApp::EnableCtrl(HWND hwndDlg, UINT nCtrlID, BOOL nEnable)
{
	HWND hCtrl = ::GetDlgItem(hwndDlg, nCtrlID);

	if (hCtrl && ::IsWindow(hCtrl))
	{
		::EnableWindow(hCtrl, nEnable);
	}
}

void CBishopApp::SendAnnounceText(HWND hwndDlg, const char* pText, UINT uLength, UINT uOption, BOOL bAllGS)
{
	if (!pText || 0 == uLength)
	{
		::MessageBox(hwndDlg, "Please input a valid message!", "Warning", MB_OK | MB_ICONINFORMATION);

		::SetFocus(::GetDlgItem(hwndDlg, IDC_EDIT_ANNOUNCE_MSG));

		return;
	}

	if (bAllGS)
	{
		CGameServer::SendToAll(pText, uLength, uOption);
	}
	else
	{
		HWND hCtrl = ::GetDlgItem(hwndDlg, IDC_LIST_GAMESERVER);

		if (hCtrl && ::IsWindow(hCtrl))
		{
			int nSelIndex = LB_ERR;

			if (LB_ERR != (nSelIndex = ::SendMessage(hCtrl, LB_GETCURSEL, 0, 0)))
			{
				UINT nSearchID = 0;

				if (LB_ERR != (nSearchID = ::SendMessage(hCtrl, LB_GETITEMDATA, nSelIndex, 0)))
				{
					IGServer* pGServer = CGameServer::GetServer(nSearchID);

					if (pGServer)
					{
						pGServer->SendText(pText, uLength, uOption);
					}
				}
			}
		}
	}
}

void CBishopApp::PunishAccount(HWND hwndDlg, const char* pName, UINT uLength, UINT uOption)
{
	if (!pName || 0 == uLength)
	{
		::MessageBox(hwndDlg, "Please input a valid accountname!", "Warning", MB_OK | MB_ICONINFORMATION);

		::SetFocus(::GetDlgItem(hwndDlg, IDC_EDIT_ACCOUNTNAME));

		return;
	}
	UINT nOption = AP_LOCK_ACCOUNTNAME;
	if (uOption == AP_PUNISH_LOCK)
	{
		nOption = AP_LOCK_ACCOUNTNAME;
	}

	if (uOption == AP_PUNISH_CHAT)
	{
		nOption = AP_LOCK_CHAT;
	}
	CGameServer::PunishAllGSV(pName, uLength, nOption);
}

void CBishopApp::UnpunishAccount(HWND hwndDlg, const char* pName, UINT uLength, UINT uOption)
{
	if (!pName || 0 == uLength)
	{
		::MessageBox(hwndDlg, "Please input a valid accountname!", "Warning", MB_OK | MB_ICONINFORMATION);

		::SetFocus(::GetDlgItem(hwndDlg, IDC_EDIT_ACCOUNTNAME));

		return;
	}
	UINT nOption = AP_LOCK_ACCOUNTNAME;
	if (uOption == AP_PUNISH_LOCK)
	{
		nOption = AP_UNLOCK_ACCOUNTNAME;
	}

	if (uOption == AP_PUNISH_CHAT)
	{
		nOption = AP_UNLOCK_CHAT;
	}
	CGameServer::PunishAllGSV(pName, uLength, nOption);
}