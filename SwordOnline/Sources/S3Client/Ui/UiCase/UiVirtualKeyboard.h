/*****************************************************************************************
//	KeyBoard
//	Copyright : LTG 2013
//	Author	:   SystemError
//	CreateTime:	18-01-2013
*****************************************************************************************/

#pragma once
#include "../Elem/WndWindow.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndShowAnimate.h"
#define MAX_KEYNUM_FORMAT 10
#define MAX_KEYLETTER_FORMAT 26
#define MAX_KEYCHAR_FORMAT 11


enum SPECIAL_KEY
{
	KeyShift,
	KeyBack,
	KeyRange,
	KeySpecialNum,
};

enum KEY_FORMAT
{
	KeySpecial,
	KeyNormal,
	KeyFormatNum,
};

class KUiVirtualKeyboard : protected KWndShowAnimate
{
public:
	static KUiVirtualKeyboard* OpenWindow(bool rd=true);
	static void		 CloseWindow(bool bDestroy);
	static KUiVirtualKeyboard* GetIfVisible();
	static void SetLinkToInsert(KWndWindow *pWndWindow = NULL, KWndEdit *pWndEdit = NULL, bool bInsert = false);
	static BOOL VirtualKeyboardAgree(KWndWindow *pWnd);
private:
	KUiVirtualKeyboard();
	~KUiVirtualKeyboard();
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	bool	FindSame(int a[],int n,int x);
	void	Gen_Keyboard(bool rd, int t[],int n, int a = 0, int b = 0);
	void	ArrangeKey(bool,int);
	void	Breathe();
private:
	static KUiVirtualKeyboard* m_pSelf;
	KWndButton	m_KeyNormal[MAX_KEYNUM_FORMAT+MAX_KEYLETTER_FORMAT+MAX_KEYCHAR_FORMAT];
	KWndButton	m_KeySpecial[KeySpecialNum];
	KWndWindow	*m_pWndWindow;
	KWndEdit	*m_pWndEdit;
private:
	bool		m_bPressDownShift;
	int			m_nFrameFly;
	bool		m_bInsertCount;
	int			m_nInsertCount;
};