#pragma once

#include "../elem/wndpage.h"
#include "../elem/wndbutton.h"
#include "../elem/wndtext.h"
#include "../../../core/src/gamedatadef.h"
#include "../Elem/WndMessageListBox.h"
#include "../Elem/WndList2.h"
#include "../Elem/WndScrollBar.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndPureTextBtn.h"
#include "../Elem/WndLabeledButton.h"
#include "../Elem/WndShadow.h"
#include "../../../core/src/gamedatadef.h"

class KUiRecovery : public KWndPage
{
public:
	void	Initialize();
	void	LoadScheme(KIniFile* pFile);
	void	SaveSetting(KIniFile* pFile);
	void	LoadSetting(KIniFile* pFile);		
private:
	static KUiRecovery* m_pSelf;
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();
	void	OnCheckInput();
	
	void	PopupSelRoomNE();
	void	SetPortalRoomNE(int nSel);
private:
	KWndText32			m_LifeGuide;
	KWndText32			m_LifeText;
	KWndButton			m_LifeChecker;
	KWndEdit32			m_LifeEdit1;
	KWndEdit32			m_LifeEdit2;
	KWndEdit32			m_LifeEdit3;
	
	KWndText32			m_ManaText;
	KWndButton			m_ManaChecker;
	KWndEdit32			m_ManaEdit1;
	KWndEdit32			m_ManaEdit2;
	KWndEdit32			m_ManaEdit3;
	
	KWndText32			m_TownPortalGuide;

	KWndButton			m_TPLifeLessCK;	
	KWndText32			m_TPLifeLessTxt;
	KWndEdit32			m_TPLifeEdit;
	
	KWndButton			m_TPManaLessCK;
	KWndText32			m_TPManaLessTxt;
	KWndEdit32			m_TPManaEdit;
	
	KWndButton			m_TPNotMedicineBloodCK;
	KWndText32			m_TPNotMedicineBloodTxt;
	
	KWndButton			m_TPNotMedicineManaCK;
	KWndText32			m_TPNotMedicineManaTxt;	
	
	KWndButton			m_TPHightMoneyCK;
	KWndText32			m_TPHightMoneyTxt;	
	KWndEdit32			m_TPHightMoneyEdit;
	
	KWndButton			m_TPDamageEquipCK;
	KWndText32			m_TPDamageEquipTxt;	
	KWndEdit32			m_TPDamageEquipEdit;
	
	KWndButton			m_TPNotEquipmentChecker;
	KWndText80			m_TPNotEquipmentText;	
	KWndShadow			m_TPNotEquipmentShadow;
	KWndButton			m_TPNotEquipmentPopup;	
	
	KWndText32			m_OtherGuide;	
	
	KWndButton			m_OtherAntiPoisonCK;
	KWndText32			m_OtherAntiPoisonTxt;
	
	KWndButton			m_OtherEnchaseExpCK;
	KWndText32			m_OtherEnchaseExpTxt;
	
	KWndButton			m_OtherRepairEqCK;
	KWndText32			m_OtherRepairEqTxt;
	
	KWndButton			m_OtherOpenMedicineCK;
	KWndText32			m_OtherOpenMedicineTxt;
	
	KWndButton			m_OtherInventoryMoneyCK;
	KWndText32			m_OtherInventoryMoneyTxt;

	KWndButton			m_OtherInventoryItemCK;
	KWndText32			m_OtherInventoryItemTxt;

	KWndButton			m_OtherReturnFromPortalCK;
	KWndText32			m_OtherReturnFromPortalTxt;
	KWndEdit32			m_OtherReturnFromPortalEdit;

	int					m_nSelRoom;
};

class KUiPick : public KWndPage
{
public:
	void	Initialize();
	void	LoadScheme(KIniFile* pFile);
	void	SaveSetting(KIniFile* pFile);
	void	LoadSetting(KIniFile* pFile);		
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();	
	void	PickPopup(int nIndex);
	void	SetPick(int nIndex, int nSel);
	void	SetFilterMagicList();
	int 	FilterSameMagic(const char* szMagic);
	int 	FindSameMagic(const char* szMagic);
private:
	KWndButton			m_PickFightNoneCK;
	KWndText80			m_PickFightNoneTxt;

    KWndText80			m_PickNoteTxt;	
	
	KWndButton			m_SortEquipmentCK;
	KWndText32			m_SortEquipmentTxt;

	KWndButton			m_FilterEquipmentCK;
	KWndText32			m_FilterEquipmentTxt;

	KWndButton			m_PickMoneyChecker;
	KWndText80			m_PickMoneyText;
	KWndButton			m_PickNotEquipChecker;
	KWndText80			m_PickNotEquipText;
	
	KWndButton			m_PickChecker;
	KWndShadow			m_PickShadow;
	KWndText80			m_PickEdit;
	KWndButton			m_PickPopupBtn;

	KWndButton			m_FollowPickCK;
	KWndText32			m_FollowPickTxt;
	
	KWndShadow			m_FilterShadow;
	KWndText80			m_FilterEdit;
	KWndButton			m_FilterPopupBtn;
	
	KWndShadow			m_FilterNumSdw;
	KWndEdit32			m_FilterNum;
	
	KWndLabeledButton	m_FilterDelBtn;
	KWndLabeledButton	m_FilterDelAll;
	KWndLabeledButton	m_FilterAddBtn;
	KWndShadow			m_FilterListSdw;
	KWndList2			m_FilterL;
	KWndScrollBar		m_FilterL_Scroll;
	
	KWndButton			m_SaveTrashCK;
	KWndText80			m_SaveTrashText;
	KWndEdit32			m_SaveTrashPrice;	
	
	KWndButton			m_SaveJewelryCK;
	KWndText80			m_SaveJewelryText;
	
	int					m_nPickKind;
	int					m_nSelMagic;
	int					m_nFltMagicIndex[MAX_AUTO_FILTERL];
	char				m_szFltMagic[MAX_AUTO_FILTERL][32];
	int					m_nFltMagicV[MAX_AUTO_FILTERL];
};

enum
{
	enumMagic_ID1,
	enumMagic_ID2,
	enumMagic_ID3,
};

enum
{
	SelMagic1,
	SelMagic2,
	SelMagic3,
	SelMagicFight1,
	SelMagicFight2,
	SelMagicSPF1,
	SelMagicSPF2,
	SelMagicAura1,
	SelMagicAura2,
	SelMagicNum,
};

class KUiFight : public KWndPage
{
public:
	void	Initialize();
	void	LoadScheme(KIniFile* pFile);
	void	SaveSetting(KIniFile* pFile);
	void	LoadSetting(KIniFile* pFile);	
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();
	void	OnCheckInput();

	void	PopupSupportSkills(int nIndex);
	void	PopupFightSkills(int nIndex);
	void	PopupSupportFightSkills(int nIndex);
	void	PopupAuraSkills(int nIndex);

	void	SetSupportSkills(int nIndex, int nSkillId);
	void	SetFightSkills(int nIndex, int nSkillId);
	void	SetSupportFightSkills(int nIndex, int nSkillId);	
	void	SetAuraSkills(int nIndex, int nSkillId);
private:
	KWndText32			m_FightGuide;

	KWndButton			m_FightUseSBCK;
	KWndText32			m_FightUseSBTxt;
	
	KWndText32			m_RadiusText;	
	KWndShadow			m_RadiusShadow;
	KWndEdit32			m_RadiusEdit;

	KWndButton			m_OverTargetCK;
	KWndText32			m_OverTargetTxt;		

	KWndButton			m_FollowTargetCK;
	KWndText32			m_FollowTargetTxt;	

	KWndButton			m_FightNearCK;
	KWndText32			m_FightNearTxt;	
	KWndEdit32			m_FightNearEdit;	
	
	KWndButton			m_KillNpcCK;
	KWndText32			m_KillNpcTxt;
	
	KWndText32			m_SupportGuide;
	
	KWndText32			m_Support1Txt;	
	KWndShadow			m_Support1Shadow;	
	KWndText80			m_Support1Edit;		
	KWndButton			m_Support1Popup;		
	
	KWndText32			m_Support2Txt;	
	KWndShadow			m_Support2Shadow;	
	KWndText80			m_Support2Edit;		
	KWndButton			m_Support2Popup;	
	
	KWndText32			m_Support3Txt;	
	KWndShadow			m_Support3Shadow;	
	KWndText80			m_Support3Edit;		
	KWndButton			m_Support3Popup;	

	KWndText32			m_SupportFightTxt;	
	KWndShadow			m_SupportFightShadow;	
	KWndText80			m_SupportFEdit;		
	KWndButton			m_SupportFightPopup;			
	
	KWndText32			m_SupportPTxt;	
	KWndShadow			m_SupportPShadow;	
	KWndText80			m_SupportPEdit;		
	KWndButton			m_SupportPPopupBtn;
	
	KWndText32			m_SupportBTxt;	
	KWndShadow			m_SupportBShadow;	
	KWndText80			m_SupportBEdit;		
	KWndButton			m_SupportBPopupBtn;	
	
	KWndText32			m_AuraTxt;
	KWndShadow			m_Aura1Shadow;	
	KWndText80			m_Aura1Edit;		
	KWndButton			m_Aura1PopupBtn;	
	KWndText32			m_Aura1Txt;
	KWndShadow			m_Aura2Shadow;	
	KWndText80			m_Aura2Edit;		
	KWndButton			m_Aura2PopupBtn;	

	KWndButton			m_LifeReplenishCK;
	KWndText32			m_LifeReplenishTxt;
	KWndEdit32			m_LifeReplenishEdit;

	int					m_nSelMagic[SelMagicNum];
};

enum
{
	NoneSelect,
	AutoAccept,
	AutoAcceptWithName,
	AutoInvite,
	AutoInviteWithList,
	AutoDenyInvite,
};

class KUiOther : public KWndPage
{
public:
	void	Initialize();
	void	LoadScheme(KIniFile* pFile);
	void	SaveSetting(KIniFile* pFile);
	void	LoadSetting(KIniFile* pFile);	
private:
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void 	OpenPopup(int nIndex);
	void	SetMoveMode(int nSel);
	void 	SetPTSel(int nSel);
	void	SetPartyList();
	BOOL	FilterSamePartyList(const char* szName);
	void 	PlayerList(int);
	BOOL	InsertMoveMpsList(int nSubW, int nMpsX, int nMpsY);
	void	SetMoveMpsList();
	void	PaintWindow();
	void	OnCheckInput();
private: 
	KWndButton			m_FollowChecker;
	KWndText32			m_FollowText;	
	KWndShadow			m_FollowShadow;	
	KWndText80			m_FollowEdit;
	KWndButton			m_FollowPopupBtn;
	KWndEdit32			m_FollowRadiusEdit;
	
	KWndText32			m_OtherTextTS;	
	KWndText32			m_OtherTextTD;
    KWndText32			m_FollowPVText;	
	

	KWndButton			m_MoveMpsCK;	
	KWndText80			m_MoveMpsText;
	KWndShadow			m_MoveMpsShadow;
	KWndList2			m_MoveMpsL;
	KWndScrollBar		m_MoveMpsL_Scroll;	
	KWndLabeledButton	m_MoveMpsGetBtn;
	KWndLabeledButton	m_MoveMpsDelAllBtn;	
	
	KWndButton			m_PTChecker;
	KWndText32			m_PTText;	
	KWndShadow			m_PTShadow;	
	KWndText80			m_PTEdit;
	KWndButton			m_PTPopupBtn;
	KWndShadow			m_PTListShadow;
	KWndList2			m_PTListL;
	KWndScrollBar		m_PTListL_Scroll;	
	KWndLabeledButton	m_PTListGetBtn;
	KWndLabeledButton	m_PTListDelAllBtn;	
	
	int					m_nMpsList[MAX_AUTO_LIST][3];
	int					m_nParTySel;
	char				m_szFollowName[32];
	char				m_szPartyList[MAX_AUTO_LIST][32];
public:
	KUiSceneTimeInfo	m_SceneInfo;
};
struct KUiSceneTimeInfo;
class KUiAutoPlay : public KWndPageSet
{
public:
	static KUiAutoPlay* OpenWindow(bool bShow = true);		
	static KUiAutoPlay* GetIfVisible();
	static void		 CloseWindow(bool bDestroy);
	static void		 LoadScheme(const char* pScheme);
	static int		 SavePrivateSetting(KIniFile* pFile);
	static BOOL		 LoadPrivateSetting(KIniFile* pFile);
	static void		 UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo);
private:
	KUiAutoPlay() {}
	~KUiAutoPlay() {}
	void	Initialize();
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	PaintWindow();
private:
	static KUiAutoPlay* m_pSelf;
private:
	KWndPureTextBtn		m_ActiveBtn;
	KWndPureTextBtn		m_CloseBtn;
	
	KUiRecovery			m_RecoveryPad;
	KUiPick				m_PickPad;
	KUiFight			m_FightPad;
	KUiOther			m_OtherPad;
	
	KWndLabeledButton	m_RecoveryBtn;
	KWndLabeledButton	m_PickBtn;
	KWndLabeledButton	m_FightBtn;
	KWndLabeledButton	m_OtherBtn;	
};

