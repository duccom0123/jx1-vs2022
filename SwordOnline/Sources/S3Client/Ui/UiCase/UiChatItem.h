#include "../Elem/WndImage.h"
#include "../Elem/WndObjContainer.h"
#include "../../../core/src/GameDataDef.h"



class KUiChatItem : protected KWndImage
{
public:

public:
	static KUiChatItem* OpenWindow(int nIdx);
	static void			CloseWindow();
	static KUiChatItem*	GetIfVisible();
	static void			LoadScheme(const char* pScheme);
public:

private:
	static  KUiChatItem *m_pSelf;
private:
	KUiChatItem();
	~KUiChatItem();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void    Initialize();       
	void	PaintWindow();
	void	UpdateData(int nIdx);
private:
	unsigned int			m_uBoderColor;
	unsigned short			m_nRectColor;
	short					m_nMaxLineLen;
	short					m_nNumLine;
	char					m_pItemDesc[GOD_MAX_OBJ_TITLE_LEN];
	short					m_pItemLen;
	KWndObjectMatrix		m_ItemBox;
	KRUImagePart			m_Image;
	BYTE					m_nWidth;	        
	BYTE					m_nHeight;
};