#pragma once

#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndPage.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndObjContainer.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndImage.h"
#include "../Elem/WndPureTextBtn.h"
#include "../../../core/src/gamedatadef.h"

enum PAGE_KIND
{
	UPGRADE_PAGE,
	COMPOUND_PAGE,
	DISTILL_PAGE,
	FORGE_PAGE,
	ENCHASE_PAGE,
	PAGE_NUM,
};

enum THIS_INTERFACE_STATUS
{
	STATUS_WAIT = 0,
	STATUS_BEGIN,
	STATUS_DOING,
	STATUS_CHANGE,
	STATUS_FINISH,
};


class KGetFrameImage_Compound : public KWndImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KUiEnchase_Compound : public KWndPage
{
public:	
	KUiEnchase_Compound();
	virtual ~KUiEnchase_Compound();
	static	KUiEnchase_Compound* OpenWindow();
	static	KUiEnchase_Compound* GetIfVisible();
	static void   CloseWindow(bool bDestory = TRUE);
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	void	Show();
	void	Hide();
	virtual void  Breathe();
	void	UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd);
	void	PopUp();
	void	ApplyShow(int nAction);
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	UpdateData();
	int		PlayEffect();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	static	KUiEnchase_Compound *ms_pSelf;
private:	
	KWndMessageListBox		m_GuideList;
	KWndScrollBar			m_GuideList_Scroll;
	KWndButton				m_CompoundBtn;
	KWndButton				m_CancleBtn;
	KGetFrameImage_Compound	m_Effect_0, m_Effect_1, m_Effect_2;
	KWndText32				m_Box_0, m_Box_1, m_Box_2;
	KWndObjectBox			m_CompoundBox[MAX_COMPOUND_ITEM];
public:
	int						m_nStatus;
	char	m_szTip[CPA_NUM][MAX_MESSAGE_LENGTH];
};


class KGetFarmImage_Distill : public KWndImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KUiEnchase_Distill : public KWndPage
{
public:	
	KUiEnchase_Distill();
	virtual ~KUiEnchase_Distill();
	static	KUiEnchase_Distill* OpenWindow();
	static	KUiEnchase_Distill* GetIfVisible();
	static void   CloseWindow(bool bDestory = TRUE);
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	void	Show();
	void	Hide();
	virtual void  Breathe();
	void	UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateDataObjBox();
	void	UpdateItemMatrixBox(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateDataMatrixBox();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	int		PlayEffect();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	static	KUiEnchase_Distill *ms_pSelf;
private:	
	KWndMessageListBox		m_GuideList;
	KWndScrollBar			m_GuideList_Scroll;
	KWndButton				m_DistillBtn;
	KWndButton				m_CancleBtn;
	int						m_nStatus;
	KGetFarmImage_Distill	m_EquipEffect, m_ConsumeEffect;
	KWndObjectBox			m_DistillBox[MAX_COMPOUND_ITEM];
	KWndObjectMatrix		m_ItemBox;
	KWndText32				m_EquipPos, m_CryolitePos, m_PropMinePos, m_ConsumePos;
public:
	char	m_szTip[MAX_MESSAGE_LENGTH];
};


class KGetFarmImage_Forge : public KWndImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KUiEnchase_Forge : public KWndPage
{
public:	
	KUiEnchase_Forge();
	virtual ~KUiEnchase_Forge();
	static	KUiEnchase_Forge* OpenWindow();
	static	KUiEnchase_Forge* GetIfVisible();
	static void   CloseWindow(bool bDestory = TRUE);
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	void	Show();
	void	Hide();
	virtual void  Breathe();
	void	UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateData();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	int		PlayEffect();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	static	KUiEnchase_Forge *ms_pSelf;
private:	
	KWndMessageListBox		m_GuideList;
	KWndScrollBar			m_GuideList_Scroll;
	KWndButton				m_ForgeBtn;
	KWndButton				m_CancleBtn;
	int						m_nStatus;
	KGetFarmImage_Forge		m_EquipEffect;
	KWndObjectBox			m_ForgeBox[MAX_COMPOUND_ITEM];
	KWndText32				m_EquipPos, m_CryolitePos;
public:
	char	m_szTip[MAX_MESSAGE_LENGTH];
};


class KGetFarmImage_Enchase : public KWndImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KUiEnchase_Enchase : public KWndPage
{
public:	
	KUiEnchase_Enchase();
	virtual ~KUiEnchase_Enchase();
	static	KUiEnchase_Enchase* OpenWindow();
	static	KUiEnchase_Enchase* GetIfVisible();
	static void   CloseWindow(bool bDestory = TRUE);
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	void	Show();
	void	Hide();
	virtual void  Breathe();
	void	UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateDataObjBox();
	void	UpdateItemMatrixBox(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateDataMatrixBox();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	int		PlayEffect();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	static	KUiEnchase_Enchase *ms_pSelf;
private:	
	KWndMessageListBox		m_GuideList;
	KWndScrollBar			m_GuideList_Scroll;
	KWndButton				m_EnchaseBtn;
	KWndButton				m_CancleBtn;
	int						m_nStatus;
	KGetFarmImage_Enchase	m_EquipEffect, m_ConsumeEffect;
	KWndObjectBox			m_EnchaseBox[MAX_COMPOUND_ITEM];
	KWndObjectMatrix		m_ItemBox;
	KWndText32				m_PurpleEquipPos, m_CryolitePos, m_PropMinePos, m_ConsumePos;
public:
	char	m_szTip[MAX_MESSAGE_LENGTH];
};

class KGetFarmImage_Upgrade : public KWndImage
{
public:
	int GetMaxFrame();
	int GetCurrentFrame();
};

class KUiEnchase_Upgrade : public KWndPage
{
public:	
	KUiEnchase_Upgrade();
	virtual ~KUiEnchase_Upgrade();
	static	KUiEnchase_Upgrade* OpenWindow();
	static	KUiEnchase_Upgrade* GetIfVisible();
	static void   CloseWindow(bool bDestory = TRUE);
	void	Initialize();
	void	LoadScheme(const char* pScheme);
	void	Show();
	void	Hide();
	virtual void  Breathe();
	void	UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd);
	void	UpdateDataObjBox();
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	int		PlayEffect();
	void	OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos);
	static	KUiEnchase_Upgrade *ms_pSelf;
private:	
	KWndMessageListBox		m_GuideList;
	KWndScrollBar			m_GuideList_Scroll;
	KWndButton				m_UpgradeBtn;
	KWndButton				m_CancleBtn;
	int						m_nStatus;
	KGetFarmImage_Upgrade	m_ConsumeEffect;
	KWndObjectBox			m_UpgradeBox[MAX_COMPOUND_ITEM];
	KWndText32				m_EquipPos, m_CryolitePos;
public:
	char	m_szTip[MAX_MESSAGE_LENGTH];
};


class KUiEnchase : public KWndPageSet
{
public:
	static KUiEnchase*		OpenWindow();
	static void				CloseWindow(bool bDestroy);
	static KUiEnchase*		GetIfVisible();
	static void				LoadScheme(const char* pScheme);
	static void UpdateItemObjBox(KUiObjAtRegion* pItem, int bAdd);
	static void UpdateItemMatrixBox(KUiObjAtRegion* pItem, int bAdd);
private:
	KUiEnchase();
	virtual ~KUiEnchase();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	Initialize();
	virtual void  Breathe();
	static KUiEnchase*		m_pSelf;
	
	KWndButton				m_UpgradeBtn;
	KWndButton				m_CompoundBtn;
	KWndButton				m_DistillBtn;
	KWndButton				m_ForgeBtn;
	KWndButton				m_EnchaseBtn;
	KWndButton				m_CloseBtn;
	int						m_nCurPage;
public:
	KUiEnchase_Compound		m_CompoundPad;
	KUiEnchase_Distill		m_DistillPad;
	KUiEnchase_Forge		m_ForgePad;
	KUiEnchase_Enchase		m_EnchasePad;
	KUiEnchase_Upgrade		m_UpgradePad;
};