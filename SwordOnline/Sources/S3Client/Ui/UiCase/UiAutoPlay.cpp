#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/popupmenu.h"
#include "../elem/wnds.h"
#include "UiAutoPlay.h"
#include "../../../core/src/coreshell.h"
#include "../../../Engine/src/KDebug.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"


extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;
#define 	SCHEME_INI		"UiAutoPlay.ini"

#define		MENU_ROOM		0x00
#define		MENU_PICK		0x02
#define		MENU_MAGIC		0x04
#define		MENU_TEAM		0x06
#define		MENU_FOLLOW		0x10
#define		MENU_PLAYER		0x12
#define		MENU_PTADD		0x16
#define		MENU_HORSE		0x18
#define		MENU_SKILL1		0x20
#define		MENU_SKILL2		0x22
#define		MENU_SKILL3		0x24
#define		MENU_SKILL4		0x26
#define		MENU_SKILL5		0x28
#define		MENU_SKILL6		0x30
#define		MENU_SKILL7		0x32
#define		MENU_SKILL8		0x34
#define		MENU_SKILL9		0x36

#define		NO_SETTINGS					"Kh«ng thiÕt lËp"
#define		MAGIC_KEYNAME				"%s >= %d"
#define		MOVEMPS_KEYNAME				"%d:%d/%d"
#define		SAVE_SECTION				"AutoPlay"

char szArray_SelRoom[][64] = 
{
	"Hµnh trang hÕt chç 1 «",
	"Hµnh trang hÕt chç 2 «",
	"Hµnh trang hÕt chç 4 «",
	"Hµnh trang hÕt chç 6 «",
};

KUiRecovery* KUiRecovery::m_pSelf = NULL;

void KUiRecovery::Initialize()
{
	AddChild(&m_LifeGuide);
	AddChild(&m_LifeText);
	AddChild(&m_LifeChecker);
	AddChild(&m_LifeEdit1);
	AddChild(&m_LifeEdit2);
	AddChild(&m_LifeEdit3);
	
	AddChild(&m_ManaText);
	AddChild(&m_ManaChecker);
	AddChild(&m_ManaEdit1);
	AddChild(&m_ManaEdit2);
	AddChild(&m_ManaEdit3);
	
	AddChild(&m_TownPortalGuide);
	
	AddChild(&m_TPLifeLessCK);
	AddChild(&m_TPLifeLessTxt);
	AddChild(&m_TPLifeEdit);
	
	AddChild(&m_TPManaLessCK);
	AddChild(&m_TPManaLessTxt);
	AddChild(&m_TPManaEdit);
	
	AddChild(&m_TPNotMedicineBloodCK);
	AddChild(&m_TPNotMedicineBloodTxt);
	
	AddChild(&m_TPNotMedicineManaCK);
	AddChild(&m_TPNotMedicineManaTxt);
	
	AddChild(&m_TPHightMoneyCK);
	AddChild(&m_TPHightMoneyTxt);
	AddChild(&m_TPHightMoneyEdit);
	
	AddChild(&m_TPDamageEquipCK);
	AddChild(&m_TPDamageEquipTxt);
	AddChild(&m_TPDamageEquipEdit);
	
	AddChild(&m_TPNotEquipmentChecker);
	AddChild(&m_TPNotEquipmentShadow);
	AddChild(&m_TPNotEquipmentText);
	AddChild(&m_TPNotEquipmentPopup);
	
	AddChild(&m_OtherGuide);
	
	AddChild(&m_OtherAntiPoisonCK);
	AddChild(&m_OtherAntiPoisonTxt);
	
	AddChild(&m_OtherEnchaseExpCK);
	AddChild(&m_OtherEnchaseExpTxt);
	
	AddChild(&m_OtherRepairEqCK);
	AddChild(&m_OtherRepairEqTxt);

	AddChild(&m_OtherOpenMedicineCK);
	AddChild(&m_OtherOpenMedicineTxt);

	AddChild(&m_OtherInventoryMoneyCK);
	AddChild(&m_OtherInventoryMoneyTxt);

	/* AddChild(&m_OtherInventoryItemCK);
	AddChild(&m_OtherInventoryItemTxt); */

	AddChild(&m_OtherReturnFromPortalCK);
	AddChild(&m_OtherReturnFromPortalTxt);	
	AddChild(&m_OtherReturnFromPortalEdit);

	m_nSelRoom = 0;
}

void KUiRecovery::LoadScheme(KIniFile* pIni)
{
	KWndPage::Init(pIni, "Page");
	m_LifeGuide.Init(pIni, "LifeGuide");
	m_LifeText.Init(pIni, "LifeText");
	m_LifeChecker.Init(pIni, "LifeChecker");
	m_LifeEdit1.Init(pIni, "LifeEdit1");
	m_LifeEdit2.Init(pIni, "LifeEdit2");
	m_LifeEdit3.Init(pIni, "LifeEdit3");	
	
	m_ManaText.Init(pIni, "ManaText");
	m_ManaChecker.Init(pIni, "ManaChecker");
	m_ManaEdit1.Init(pIni, "ManaEdit1");
	m_ManaEdit2.Init(pIni, "ManaEdit2");
	m_ManaEdit3.Init(pIni, "ManaEdit3");

	m_TownPortalGuide.Init(pIni, "TownPortalGuide");
	m_TPLifeLessCK.Init(pIni, "TPLifeLessCK");
	m_TPLifeLessTxt.Init(pIni, "TPLifeLessTxt");
	m_TPLifeEdit.Init(pIni, "TPLifeEdit");
	
	m_TPManaLessCK.Init(pIni, "TPManaLessCK");
	m_TPManaLessTxt.Init(pIni, "TPManaLessTxt");
	m_TPManaEdit.Init(pIni, "TPManaEdit");
	
	m_TPNotMedicineBloodCK.Init(pIni, "TPNotMedicineBloodCK");
	m_TPNotMedicineBloodTxt.Init(pIni, "TPNotMedicineBloodTxt");

	m_TPNotMedicineManaCK.Init(pIni, "TPNotMedicineManaCK");
	m_TPNotMedicineManaTxt.Init(pIni, "TPNotMedicineManaTxt");
	
	m_TPHightMoneyCK.Init(pIni, "TPHightMoneyCK");
	m_TPHightMoneyTxt.Init(pIni, "TPHightMoneyTxt");
	m_TPHightMoneyEdit.Init(pIni, "TPHightMoneyEdit");
	
	m_TPDamageEquipCK.Init(pIni, "TPDamageEquipCK");
	m_TPDamageEquipTxt.Init(pIni, "TPDamageEquipTxt");
	m_TPDamageEquipEdit.Init(pIni, "TPDamageEquipEdit");
	
	m_TPNotEquipmentChecker.Init(pIni, "TPNotEquipmentChecker");
	m_TPNotEquipmentText.Init(pIni, "TPNotEquipmentText");
	m_TPNotEquipmentShadow.Init(pIni, "TPNotEquipmentShadow");
	m_TPNotEquipmentPopup.Init(pIni, "TPNotEquipmentPopup");
	
	m_OtherGuide.Init(pIni, "OtherGuide");
	m_OtherAntiPoisonCK.Init(pIni, "OtherAntiPoisonCK");
	m_OtherAntiPoisonTxt.Init(pIni, "OtherAntiPoisonTxt");
	
	m_OtherEnchaseExpCK.Init(pIni, "OtherEnchaseExpCK");
	m_OtherEnchaseExpTxt.Init(pIni, "OtherEnchaseExpTxt");
	
	m_OtherRepairEqCK.Init(pIni, "OtherRepairEqCK");
	m_OtherRepairEqTxt.Init(pIni, "OtherRepairEqTxt");
	
	m_OtherOpenMedicineCK.Init(pIni, "OtherOpenMedicineCK");
	m_OtherOpenMedicineTxt.Init(pIni, "OtherOpenMedicineTxt");

	m_OtherInventoryMoneyCK.Init(pIni, "OtherInventoryMoneyCK");		
	m_OtherInventoryMoneyTxt.Init(pIni, "OtherInventoryMoneyTxt");

	m_OtherInventoryItemCK.Init(pIni, "OtherInventoryItemCK");		
	m_OtherInventoryItemTxt.Init(pIni, "OtherInventoryItemTxt");

	m_OtherReturnFromPortalCK.Init(pIni, "OtherReturnFromPortalCK");		
	m_OtherReturnFromPortalTxt.Init(pIni, "OtherReturnFromPortalTxt");
	m_OtherReturnFromPortalEdit.Init(pIni, "OtherReturnFromPortalEdit");
}

void KUiRecovery::PopupSelRoomNE()
{
	int nActionDataCount = sizeof(szArray_SelRoom) / sizeof(szArray_SelRoom[0]);
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if ((i == enumRoomNotEnough1) || 
			(i == enumRoomNotEnough2) || 
			(i == enumRoomNotEnough4) || 
			(i == enumRoomNotEnough6)
			)
		{
			strncpy(pSelUnitMenu->Items[i].szData, szArray_SelRoom[i], sizeof(szArray_SelRoom[i]));
			pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
			pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->nNumItem++;
		}
	}
	int x, y;
	m_TPNotEquipmentShadow.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, MENU_ROOM);
}

void KUiRecovery::SetPortalRoomNE(int nSel)
{
	m_nSelRoom = nSel;
	m_TPNotEquipmentText.SetText(szArray_SelRoom[m_nSelRoom]);
	g_pCoreShell->PAIOperation(GPI_TP_NOT_EQUIPMENTV, m_nSelRoom, NULL, NULL);
}

void KUiRecovery::PaintWindow()
{
/*	KWndPage::PaintWindow();
	if	(g_pRepresentShell)
	{
		KRURect	Rect;
		GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		Rect.oPosition.nX += 5;
		Rect.oPosition.nY += 18;
		Rect.Color.Color_dw = 0xff808080;
		GetSize(&Rect.oEndPos.nX, &Rect.oEndPos.nY);
		Rect.oEndPos.nX += Rect.oPosition.nX - 10;
		Rect.oEndPos.nY = Rect.oPosition.nY + 32;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
		
		Rect.oPosition.nY = Rect.oEndPos.nY + 18;
		Rect.oEndPos.nY += 80;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
		
		Rect.oPosition.nY = Rect.oEndPos.nY + 18;
		Rect.oEndPos.nY += 80;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);		
	}*/
}

int KUiRecovery::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_LifeChecker)
		{
			if (m_LifeChecker.IsButtonChecked())
			{
				m_LifeChecker.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_EAT_LIFE, TRUE, NULL, NULL);
			}
			else
			{
				m_LifeChecker.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_EAT_LIFE, FALSE, NULL, NULL);
			}			
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_ManaChecker)
		{
			if (m_ManaChecker.IsButtonChecked())
			{
				m_ManaChecker.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_EAT_MANA, TRUE, NULL, NULL);
			}
			else
			{
				m_ManaChecker.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_EAT_MANA, FALSE, NULL, NULL);
			}			
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPNotEquipmentChecker)
		{
			if (m_TPNotEquipmentChecker.IsButtonChecked())
			{
				m_TPNotEquipmentChecker.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_TP_NOT_EQUIPMENT, TRUE, NULL, NULL);
			}
			else
			{
				m_TPNotEquipmentChecker.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_TP_NOT_EQUIPMENT, FALSE, NULL, NULL);				
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPNotEquipmentPopup)
			PopupSelRoomNE();
		else if (uParam == (unsigned int)(KWndWindow*)&m_OtherAntiPoisonCK)
		{
			if (m_OtherAntiPoisonCK.IsButtonChecked())
			{
				m_OtherAntiPoisonCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_ANTI_POISON, TRUE, NULL, NULL);				
			}
			else
			{
				m_OtherAntiPoisonCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_ANTI_POISON, FALSE, NULL, NULL);				
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OtherEnchaseExpCK)
		{
			if (m_OtherEnchaseExpCK.IsButtonChecked())
			{
				m_OtherEnchaseExpCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_ENCHASEEXP, TRUE, NULL, NULL);
			}
			else
			{
				m_OtherEnchaseExpCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_ENCHASEEXP, FALSE, NULL, NULL);				
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OtherRepairEqCK)
		{
			if (m_OtherRepairEqCK.IsButtonChecked())
			{
				m_OtherRepairEqCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_AUTO_REPAIR, TRUE, NULL, NULL);
			}
			else
			{
				m_OtherRepairEqCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_AUTO_REPAIR, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPDamageEquipCK)
		{
			if (m_TPDamageEquipCK.IsButtonChecked())
			{
				m_TPDamageEquipCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_TP_DAMAGEEQUIP, TRUE, NULL, NULL);
			}
			else
			{
				m_TPDamageEquipCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_TP_DAMAGEEQUIP, FALSE, NULL, NULL);
			}		
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPHightMoneyCK)
		{
			if (m_TPHightMoneyCK.IsButtonChecked())
			{
				m_TPHightMoneyCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_TP_HIGHTMONEY, TRUE, NULL, NULL);
			}
			else
			{
				m_TPHightMoneyCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_TP_HIGHTMONEY, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPNotMedicineBloodCK)
		{
			if (m_TPNotMedicineBloodCK.IsButtonChecked())
			{
				m_TPNotMedicineBloodCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_TP_NOT_MEDICINEBLOOD, TRUE, NULL, NULL);
			}
			else
			{
				m_TPNotMedicineBloodCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_TP_NOT_MEDICINEBLOOD, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPNotMedicineManaCK)
		{
			if (m_TPNotMedicineManaCK.IsButtonChecked())
			{
				m_TPNotMedicineManaCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_TP_NOT_MEDICINEMANA, TRUE, NULL, NULL);				
			}
			else
			{
				m_TPNotMedicineManaCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_TP_NOT_MEDICINEMANA, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPLifeLessCK)
		{
			if (m_TPLifeLessCK.IsButtonChecked())
			{
				m_TPLifeLessCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_TP_LIFE, TRUE, NULL, NULL);				

			}
			else
			{
				m_TPLifeLessCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_TP_LIFE, FALSE, NULL, NULL);			
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_TPManaLessCK)
		{
			if (m_TPManaLessCK.IsButtonChecked())
			{
				m_TPManaLessCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_TP_MANA, TRUE, NULL, NULL);
			}
			else
			{
				m_TPManaLessCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_TP_MANA, FALSE, NULL, NULL);	
			}		
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OtherOpenMedicineCK)
		{
			if (m_OtherOpenMedicineCK.IsButtonChecked())
			{
				m_OtherOpenMedicineCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_OPEN_MEDICINE, TRUE, NULL, NULL);
			}
			else
			{
				m_OtherOpenMedicineCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_OPEN_MEDICINE, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OtherInventoryMoneyCK)
		{
			if (m_OtherInventoryMoneyCK.IsButtonChecked())
			{
				m_OtherInventoryMoneyCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_INVENTORYMONEY, TRUE, NULL, NULL);
			}
			else
			{
				m_OtherInventoryMoneyCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_INVENTORYMONEY, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OtherInventoryItemCK)
		{
			if (m_OtherInventoryItemCK.IsButtonChecked())
			{
				m_OtherInventoryItemCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_INVENTORYITEM, TRUE, NULL, NULL);
			}
			else
			{
				m_OtherInventoryItemCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_INVENTORYITEM, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OtherReturnFromPortalCK)
		{
			if (m_OtherReturnFromPortalCK.IsButtonChecked())
			{
				m_OtherReturnFromPortalCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_RETURNPORTAL, TRUE, NULL, NULL);
			}
			else
			{
				m_OtherReturnFromPortalCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_RETURNPORTAL, FALSE, NULL, NULL);
			}
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCheckInput();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == MENU_ROOM && (short)(LOWORD(nParam)) >= 0)
			{
				SetPortalRoomNE(LOWORD(nParam));
			}
		}		
	default:
		return KWndPage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiRecovery::LoadSetting(KIniFile* pFile)
{
	if (pFile)
	{
		int nValue;
		
		KUiPlayerRuntimeInfo	Info;
		memset(&Info, 0, sizeof(KUiPlayerRuntimeInfo));
		g_pCoreShell->GetGameData(GDI_PLAYER_RT_INFO, (int)&Info, 0);
	
		pFile->GetInteger(SAVE_SECTION, "LifeCK", 1, &nValue);
		m_LifeChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_EAT_LIFE, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "ManaCK", 1, &nValue);
		m_ManaChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_EAT_MANA, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "Life1", 0, &nValue);
		m_LifeEdit1.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "Life2", 80, &nValue);
		m_LifeEdit2.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "Life3", 3000, &nValue);
		if (nValue < 1000)
			nValue = 1000;
		m_LifeEdit3.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "Mana1", 0, &nValue);
		m_ManaEdit1.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "Mana2", 30, &nValue);
		m_ManaEdit2.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "Mana3", 3000, &nValue);
		if (nValue < 1000)
			nValue = 1000;
		m_ManaEdit3.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "TPLife", 0, &nValue);
		m_TPLifeEdit.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "TPMana", 0, &nValue);
		m_TPManaEdit.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "TPMoney", 0, &nValue);
		m_TPHightMoneyEdit.SetIntText(nValue);

		pFile->GetInteger(SAVE_SECTION, "TPBloodCK", 0, &nValue);
		m_TPNotMedicineBloodCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_TP_NOT_MEDICINEBLOOD, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "TPManaCK", 0, &nValue);
		m_TPNotMedicineManaCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_TP_NOT_MEDICINEMANA, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "TPLifeLCK", 0, &nValue);
		m_TPLifeLessCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_TP_LIFE, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "TPManaLCK", 0, &nValue);
		m_TPManaLessCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_TP_MANA, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "TPMoneyCK", 0, &nValue);
		m_TPHightMoneyCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_TP_HIGHTMONEY, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "TPDmgEqCK", 1, &nValue);
		m_TPDamageEquipCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_TP_DAMAGEEQUIP, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "TPDmgEqV", 3, &nValue);
		m_TPDamageEquipEdit.SetIntText(nValue);		
		
		pFile->GetInteger(SAVE_SECTION, "TPRoomNECK", 0, &nValue);
		m_TPNotEquipmentChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_TP_NOT_EQUIPMENT, nValue > 0, NULL, NULL);
		
		pFile->GetInteger(SAVE_SECTION, "TPRoomNEV", 0, &nValue);
		SetPortalRoomNE(nValue);		
		
		pFile->GetInteger(SAVE_SECTION, "UsePoisonCK", 0, &nValue);
		m_OtherAntiPoisonCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_ANTI_POISON, nValue > 0, NULL, NULL);
			
		pFile->GetInteger(SAVE_SECTION, "UseEnchaseExpCK", 0, &nValue);
		m_OtherEnchaseExpCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_ENCHASEEXP, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "RepairEqCK", 1, &nValue);
		g_pCoreShell->PAIOperation(GPI_AUTO_REPAIR, nValue > 0, NULL, NULL);
		m_OtherRepairEqCK.CheckButton(nValue > 0);
		
		pFile->GetInteger(SAVE_SECTION, "OpenMdc", 1, &nValue);
		m_OtherOpenMedicineCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_OPEN_MEDICINE, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "InventoryMoney", 0, &nValue);
		m_OtherInventoryMoneyCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_INVENTORYMONEY, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "InventoryItem", 0, &nValue);
		m_OtherInventoryItemCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_INVENTORYITEM, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "ReturnPortal", 0, &nValue);
		m_OtherReturnFromPortalCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_RETURNPORTAL, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "ReturnPortalV", 15, &nValue);
		m_OtherReturnFromPortalEdit.SetIntText(nValue);
	}	
}
		
void KUiRecovery::SaveSetting(KIniFile* pFile)
{
	if (pFile)
	{
		pFile->WriteInteger(SAVE_SECTION, "LifeCK", 					m_LifeChecker.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "Life1", 						m_LifeEdit1.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "Life2", 						m_LifeEdit2.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "Life3", 						m_LifeEdit3.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "ManaCK", 					m_ManaChecker.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "Mana1", 						m_ManaEdit1.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "Mana2", 						m_ManaEdit2.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "Mana3", 						m_ManaEdit3.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "TPLife", 					m_TPLifeEdit.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "TPMana", 					m_TPManaEdit.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "TPMoney", 					m_TPHightMoneyEdit.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "TPLifeLCK", 					m_TPLifeLessCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "TPManaLCK", 					m_TPManaLessCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "TPBloodCK",					m_TPNotMedicineBloodCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "TPManaCK", 					m_TPNotMedicineManaCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "TPMoneyCK", 					m_TPHightMoneyCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "TPDmgEqCK", 					m_TPDamageEquipCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "TPDmgEqV", 					m_TPDamageEquipEdit.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "TPRoomNECK", 				m_TPNotEquipmentChecker.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "TPRoomNEV", 					m_nSelRoom);		
		pFile->WriteInteger(SAVE_SECTION, "UsePoisonCK", 				m_OtherAntiPoisonCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "UseEnchaseExpCK",			m_OtherEnchaseExpCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "RepairEqCK", 				m_OtherRepairEqCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "OpenMdc",					m_OtherOpenMedicineCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "InventoryMoney",				m_OtherInventoryMoneyCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "InventoryItem",				m_OtherInventoryItemCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "ReturnPortal",				m_OtherReturnFromPortalCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "ReturnPortalV",				m_OtherReturnFromPortalEdit.GetIntNumber());
	}
}

void KUiRecovery::OnCheckInput()
{
	if (g_pCoreShell)
	{
		g_pCoreShell->PAIOperation(GPI_LIFE_VALUE, m_LifeEdit1.GetIntNumber(), NULL, NULL);
		if (m_LifeEdit2.GetIntNumber() > MAX_PERCENT)
			m_LifeEdit2.SetIntText(MAX_PERCENT);
		g_pCoreShell->PAIOperation(GPI_LIFE_PERCENT, m_LifeEdit2.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_LIFE_TIME, m_LifeEdit3.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_MANA_VALUE, m_ManaEdit1.GetIntNumber(), NULL, NULL);
		if (m_ManaEdit2.GetIntNumber() > MAX_PERCENT)
			m_ManaEdit2.SetIntText(MAX_PERCENT);
		g_pCoreShell->PAIOperation(GPI_MANA_PERCENT, m_ManaEdit2.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_MANA_TIME, m_ManaEdit3.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_TP_LIFEV, m_TPLifeEdit.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_TP_MANAV, m_TPManaEdit.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_TP_HIGHTMONEYV, m_TPHightMoneyEdit.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_TP_DAMAGEEQUIPV, m_TPDamageEquipEdit.GetIntNumber(), NULL, NULL);
		g_pCoreShell->PAIOperation(GPI_RETURNPORTALSEC, m_OtherReturnFromPortalEdit.GetIntNumber(), NULL, NULL);
	}
}
///////////////////////////////////////////////////
//////////////////////////////////////////////////

char szArray_Pickkind[][64] = 
{
	"TÊt c¶ trang bÞ",
	"Trang bÞ trang søc",
	"Trang bÞ thuéc tÝnh",
};

char szArray_Equipmagic[MAX_AUTO_FILTERL][2][64] = 
{
	"Sinh lùc(®)", 		"lifemax_v",
	"Néi lùc(®)", 		"manamax_v",
	"ThÓ lùc(®)", 		"staminamax_v",
	"Phôc håi SL(®)", 	"lifereplenish_v",
	"Phôc håi NL(®)", 	"manareplenish_v",
	"Phôc håi TL(®)", 	"staminareplenish_v",
	"Hót sinh lùc(%)", 	"steallifeenhance_p",
	"Hót néi lùc(%)", 	"stealmanaenhance_p",
	"Hót thÓ lùc(%)", 	"stealstaminaenhance_p",
	"Søc m¹nh(®)", 		"strength_v",
	"Th©n ph¸p(®)", 	"dexterity_v",
	"Sinh khÝ(®)", 		"vitality_v",
	"Néi c«ng(®)", 		"energy_v",
	"Phßng thñ(%)", 	"physicsres_p",
	"Kh¸ng ®éc(%)", 	"poisonres_p",
	"Kh¸ng b¨ng(%)", 	"coldres_p",
	"Kh¸ng háa(%)", 	"fireres_p",
	"Kh¸ng l«i(%)", 	"lightingres_p",
	"Kh¸ng tÊt c¶(%)", 	"allres_p",
	"Tèc ®é ch¹y(%)", 	"fastwalkrun_p",
	"Tèc ®é ®¸nh(%)", 	"attackspeed_v",
	"Ph¶n ®ßn cËn(®)", 	"meleedamagereturn_v",
	"STVL ngo¹i(®)", 	"addphysicsdamage_v",
	"B¨ng s¸t ngo¹i(®)", "addcolddamage_v",
	"§éc s¸t ngo¹i(®)", "addpoisondamage_v",
	"STVL ngo¹i(%)", 	"addphysicsdamage_p",
	"CHSTTNL(%)", 		"damage2addmana_p",
	"May m¾n(%)", 		"lucky_v",
	"Bá qua nÐ tr¸nh(%)", "ignoredefense_p",
	"§é chÝnh x¸c(®)", 	"attackrating_v",
	"STLV néi(®)", 		"addphysicsmagic_v",
	"B¨ng s¸t néi(®)", 	"addcoldmagic_v",
	"Háa s¸t néi(®)", 	"addfiremagic_v",
	"L«i s¸t néi(®)", 	"addlightingmagic_v",
	"§éc s¸t néi(®)", 	"addpoisonmagic_v",
	"Gi¶m chËm(%)", 	"freezetimereduce_p",
	"Gi¶m tróng ®éc(%)", "poisontimereduce_p",
	"Gi¶m cho¸ng(%)", 	"stuntimereduce_p",
	"Gi¶m th­¬ng(%)", 	"fasthitrecover_v",
	"Kü n¨ng vèn cã(®)", "allskill_v",
};
void KUiPick::Initialize()
{
	AddChild(&m_PickFightNoneCK);
	AddChild(&m_PickFightNoneTxt);
	
	AddChild(&m_PickNoteTxt);

	/* AddChild(&m_SortEquipmentCK);
	AddChild(&m_SortEquipmentTxt); */

	AddChild(&m_FilterEquipmentCK);
	AddChild(&m_FilterEquipmentTxt);

	AddChild(&m_PickMoneyChecker);
	AddChild(&m_PickMoneyText);
	AddChild(&m_PickNotEquipChecker);
	AddChild(&m_PickNotEquipText);
	
	AddChild(&m_PickChecker);
	AddChild(&m_PickShadow);
	AddChild(&m_PickEdit);
	AddChild(&m_PickPopupBtn);
	
	AddChild(&m_FollowPickCK);
	AddChild(&m_FollowPickTxt);

	AddChild(&m_FilterShadow);
	AddChild(&m_FilterEdit);	
	AddChild(&m_FilterPopupBtn);
	
	AddChild(&m_FilterNumSdw);
	AddChild(&m_FilterNum);
	
	AddChild(&m_FilterDelBtn);
	AddChild(&m_FilterDelAll);
	AddChild(&m_FilterAddBtn);	
	AddChild(&m_FilterListSdw);
	AddChild(&m_FilterL);
	AddChild(&m_FilterL_Scroll);
	m_FilterL.SetScrollbar(&m_FilterL_Scroll);
	
	AddChild(&m_SaveTrashCK);
	AddChild(&m_SaveTrashText);	
	AddChild(&m_SaveTrashPrice);	
	
	AddChild(&m_SaveJewelryCK);
	AddChild(&m_SaveJewelryText);	
	
	m_nPickKind = enumPickEquipHaveMagic;
	m_nSelMagic = 0;
	memset(m_nFltMagicIndex, 0, sizeof(m_nFltMagicIndex));
	memset(m_szFltMagic, 0, sizeof(m_szFltMagic));
	memset(m_nFltMagicV, 0, sizeof(m_nFltMagicV));
}

void KUiPick::LoadScheme(KIniFile* pIni)
{
	KWndPage::Init(pIni, "Page");

	m_PickFightNoneCK.Init(pIni, "PickFightNoneCK");
	m_PickFightNoneTxt.Init(pIni, "PickFightNoneTxt");
	
	m_PickNoteTxt.Init(pIni, "PickNoteTxt");

	m_SortEquipmentCK.Init(pIni, "SortEquipmentCK");
	m_SortEquipmentTxt.Init(pIni, "SortEquipmentTxt");

	m_FilterEquipmentCK.Init(pIni, "FilterEquipmentCK");
	m_FilterEquipmentTxt.Init(pIni, "FilterEquipmentTxt");

	m_PickMoneyChecker.Init(pIni, "PickMoneyChecker");
	m_PickMoneyText.Init(pIni, "PickMoneyText");
	m_PickNotEquipChecker.Init(pIni, "PickNotEquipChecker");
	m_PickNotEquipText.Init(pIni, "PickNotEquipText");
	
	m_PickChecker.Init(pIni, "PickChecker");
	m_PickShadow.Init(pIni, "PickShadow");
	m_PickEdit.Init(pIni, "PickEdit");
	m_PickPopupBtn.Init(pIni, "PickPopupBtn");
	
	m_FollowPickCK.Init(pIni, "FollowPickCK");
	m_FollowPickTxt.Init(pIni, "FollowPickTxt");

	m_FilterShadow.Init(pIni, "FilterShadow");
	m_FilterEdit.Init(pIni, "FilterEdit");
	m_FilterPopupBtn.Init(pIni, "FilterPopupBtn");
	
	m_FilterNumSdw.Init(pIni, "FilterNumSdw");
	m_FilterNum.Init(pIni, "FilterNum");
	
	m_FilterDelBtn.Init(pIni, "FilterDelBtn");
	m_FilterDelAll.Init(pIni, "FilterDelAll");
	m_FilterAddBtn.Init(pIni, "FilterAddBtn");
	m_FilterListSdw.Init(pIni, "FilterListSdw");
	m_FilterL.Init(pIni, "FilterL");
	m_FilterL_Scroll.Init(pIni, "FilterL_Scroll");
	
	m_SaveTrashCK.Init(pIni, "SaveTrashCK");
	m_SaveTrashText.Init(pIni, "SaveTrashText");
	m_SaveTrashPrice.Init(pIni, "SaveTrashPrice");
	
	m_SaveJewelryCK.Init(pIni, "SaveJewelryCK");
	m_SaveJewelryText.Init(pIni, "SaveJewelryText");		
}

int KUiPick::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_PickFightNoneCK)
		{
			if (m_PickFightNoneCK.IsButtonChecked())
			{
				m_PickFightNoneCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_PICK_FIGHTNONE, TRUE, NULL, NULL);
			}
			else
			{
				m_PickFightNoneCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_PICK_FIGHTNONE, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SortEquipmentCK)
		{
			g_pCoreShell->PAIOperation(GPI_SORT_EQUIPMENT, NULL, NULL, NULL);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FilterEquipmentCK)
		{
			if (m_FilterEquipmentCK.IsButtonChecked())
			{
				m_FilterEquipmentCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_FILTER_EQUIPMENT, TRUE, NULL, NULL);
			}
			else
			{
				m_FilterEquipmentCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_FILTER_EQUIPMENT, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PickMoneyChecker)
		{
			if (m_PickMoneyChecker.IsButtonChecked())
			{
				m_PickMoneyChecker.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_PICK_MONEY, TRUE, NULL, NULL);
			}
			else
			{
				m_PickMoneyChecker.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_PICK_MONEY, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PickNotEquipChecker)
		{
			if (m_PickNotEquipChecker.IsButtonChecked())
			{
				m_PickNotEquipChecker.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_PICK_NOTEQUIP, TRUE, NULL, NULL);
			}
			else
			{
				m_PickNotEquipChecker.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_PICK_NOTEQUIP, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PickChecker)
		{
			if (m_PickChecker.IsButtonChecked())
			{
				m_PickChecker.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_PICK_EQUIP, TRUE, NULL, NULL);
			}
			else
			{
				m_PickChecker.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_PICK_EQUIP, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PickPopupBtn)
			PickPopup(MENU_PICK);
		else if (uParam == (unsigned int)(KWndWindow*)&m_FollowPickCK)
		{
			if (m_FollowPickCK.IsButtonChecked())
			{
				m_FollowPickCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_FOLLOW_PICK, TRUE, NULL, NULL);
			}
			else
			{
				m_FollowPickCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_FOLLOW_PICK, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FilterPopupBtn)
			PickPopup(MENU_MAGIC);
		else if (uParam == (unsigned int)(KWndWindow*)&m_FilterAddBtn)
		{
			int i = FilterSameMagic(szArray_Equipmagic[m_nSelMagic][1]);

			if (i >= 0 && i < MAX_AUTO_FILTERL)
			{
				strcpy(m_szFltMagic[i], szArray_Equipmagic[m_nSelMagic][1]);
				m_nFltMagicV[i] = m_FilterNum.GetIntNumber();
			}
			else
			{
				for (i = 0; i < MAX_AUTO_FILTERL; i++)
				{
					if (!m_szFltMagic[i][0])
					{
						strcpy(m_szFltMagic[i], szArray_Equipmagic[m_nSelMagic][1]);
						m_nFltMagicV[i] = m_FilterNum.GetIntNumber();
						break;
					}
				}
			}
			SetFilterMagicList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FilterDelBtn)
		{
			int nSel = m_FilterL.GetCurSel();
			memset(m_szFltMagic[m_nFltMagicIndex[nSel]], 0, sizeof(m_szFltMagic[nSel]));
			m_nFltMagicV[m_nFltMagicIndex[nSel]] = 0;
			SetFilterMagicList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FilterDelAll)
		{
			memset(m_szFltMagic, 0, sizeof(m_szFltMagic));
			memset(m_nFltMagicV, 0, sizeof(m_nFltMagicV));
			SetFilterMagicList();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SaveTrashCK)
		{
			if (m_SaveTrashCK.IsButtonChecked())
			{
				m_SaveTrashCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_SAVE_TRASH, TRUE, NULL, NULL);
			}
			else
			{
				m_SaveTrashCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_SAVE_TRASH, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SaveJewelryCK)
		{
			if (m_SaveJewelryCK.IsButtonChecked())
			{
				m_SaveJewelryCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_SAVE_JEWELRY, TRUE, NULL, NULL);
			}
			else
			{
				m_SaveJewelryCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_SAVE_JEWELRY, FALSE, NULL, NULL);
			}
		}
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if ((short)(LOWORD(nParam)) >= 0)
			{
				SetPick(HIWORD(nParam), (short)(LOWORD(nParam)));
			}
		}
		break;
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_FilterL_Scroll)
			m_FilterL.SetTopItemIndex(nParam);
		break;
	case WND_N_EDIT_CHANGE:
		g_pCoreShell->PAIOperation(GPI_SAVE_TRASH_PRICE, m_SaveTrashPrice.GetIntNumber(), NULL, NULL);
		break;
	default:
		return KWndPage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiPick::SetFilterMagicList()
{
	memset(m_nFltMagicIndex, 0, sizeof(m_nFltMagicIndex));
	m_FilterL.ResetContent();

	char Buff[64];
	int nCount = 0;
	for (int i = 0; i < MAX_AUTO_FILTERL; i++)
	{
		g_pCoreShell->PAIOperation(GPI_AUTO_FILTERMAGIC, NULL, 0, i);

		if (m_szFltMagic[i][0])
		{
			g_pCoreShell->PAIOperation(GPI_AUTO_FILTERMAGIC, (unsigned int)m_szFltMagic[i], m_nFltMagicV[i], nCount);

			sprintf(Buff, MAGIC_KEYNAME, szArray_Equipmagic[FindSameMagic(m_szFltMagic[i])][0], 
			m_nFltMagicV[i]);
			m_FilterL.AddString(nCount, Buff);
			m_nFltMagicIndex[nCount] = i;
			nCount++;
		}
	}
}

int KUiPick::FilterSameMagic(const char* szMagic)
{
	for (int i = 0; i < MAX_AUTO_FILTERL; i ++)
	{
		if (strcmp(szMagic, m_szFltMagic[i]) == 0)
			return i;
	}
	return -1;
}

int KUiPick::FindSameMagic(const char* szMagic)
{
	for (int i = 0; i < MAX_AUTO_FILTERL; i ++)
	{
		if (strcmp(szMagic, szArray_Equipmagic[i][1]) == 0)
			return i;
	}
	return -1;
}

void KUiPick::PickPopup(int nIndex)
{
	int nActionDataCount = 0;
	if (nIndex == MENU_PICK)
		nActionDataCount = sizeof(szArray_Pickkind) / sizeof(szArray_Pickkind[0]);
	else if (nIndex = MENU_MAGIC)
		nActionDataCount = sizeof(szArray_Equipmagic) / sizeof(szArray_Equipmagic[0]);
	else return;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (nIndex == MENU_PICK)
			strncpy(pSelUnitMenu->Items[i].szData, szArray_Pickkind[i], sizeof(szArray_Pickkind[i]));
		else if (nIndex = MENU_MAGIC)
			strncpy(pSelUnitMenu->Items[i].szData, szArray_Equipmagic[i][0], sizeof(szArray_Equipmagic[i][0]));
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	if (nIndex == MENU_PICK)
		m_PickShadow.GetAbsolutePos(&x, &y);
	else if (nIndex = MENU_MAGIC)
		m_FilterShadow.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, nIndex);
}

void KUiPick::SetPick(int nIndex, int nSel)
{
	switch (nIndex)
	{
		case MENU_PICK:
			if (nSel >= enumPickEquipAll && nSel < enumPickNum)
			{
				m_nPickKind = nSel;
				m_PickEdit.SetText(szArray_Pickkind[m_nPickKind]);
				g_pCoreShell->PAIOperation(GPI_PICK_KIND, m_nPickKind, NULL, NULL);
			}
			break;
		case MENU_MAGIC:
			m_nSelMagic = nSel;
			m_FilterEdit.SetText(szArray_Equipmagic[m_nSelMagic][0]);
			break;
	}
}

void KUiPick::PaintWindow()
{
	KWndPage::PaintWindow();
	if	(g_pRepresentShell)
	{
		KRURect	Rect;
		Rect.Color.Color_dw = 0xff808080;
		m_PickFightNoneCK.GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		Rect.oPosition.nX -= 5;
		Rect.oPosition.nY -= 3;
		m_FilterEquipmentCK.GetAbsolutePos(NULL, &Rect.oEndPos.nY);
//		Rect.oEndPos.nY -= 4;
//		GetSize(&Rect.oEndPos.nX, NULL);
//		Rect.oEndPos.nX += (Rect.oPosition.nX - 10);
//		Rect.oEndPos.nY -= 4;
//		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
//		Rect.oPosition.nY = Rect.oEndPos.nY + 4;
//		Rect.oEndPos.nY += 172;
//		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
		m_FilterL_Scroll.GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		m_FilterL_Scroll.GetSize(&Rect.oEndPos.nX, &Rect.oEndPos.nY);
		Rect.oEndPos.nX += Rect.oPosition.nX;
		Rect.oEndPos.nY += Rect.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
	}
}

void KUiPick::LoadSetting(KIniFile* pFile)
{
	if (pFile)
	{
		int nValue;
		pFile->GetInteger(SAVE_SECTION, "PickFN", 0, &nValue);
		m_PickFightNoneCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_PICK_FIGHTNONE, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "FilterEq", 0, &nValue);
		m_FilterEquipmentCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_FILTER_EQUIPMENT, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "PickMoney", 0, &nValue);
		m_PickMoneyChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_PICK_MONEY, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "PickNotEquip", 0, &nValue);
		m_PickNotEquipChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_PICK_NOTEQUIP, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "PickEquip", 0, &nValue);
		m_PickChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_PICK_EQUIP, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "FollowPick", 0, &nValue);
		m_FollowPickCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_FOLLOW_PICK, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "PickKind", enumPickEquipHaveMagic, &m_nPickKind);
		m_PickEdit.SetText(szArray_Pickkind[m_nPickKind]);
		g_pCoreShell->PAIOperation(GPI_PICK_KIND, m_nPickKind, NULL, NULL);
		pFile->GetInteger(SAVE_SECTION, "SelMagic", 0, &m_nSelMagic);
		m_FilterEdit.SetText(szArray_Equipmagic[m_nSelMagic][0]);
		pFile->GetInteger(SAVE_SECTION, "MagicValue", 0, &nValue);
		m_FilterNum.SetIntText(nValue);

		memset(m_szFltMagic, 0, sizeof(m_szFltMagic));
		memset(m_nFltMagicV, 0, sizeof(m_nFltMagicV));
		char szKeyName[10];
		for (int i = 0; i < MAX_AUTO_FILTERL; i++)
		{
			sprintf(szKeyName, "Name%d", i);
			pFile->GetString("FilterMagic", szKeyName, "", m_szFltMagic[i], sizeof(m_szFltMagic[i]));
			sprintf(szKeyName, "Value%d", i);
			pFile->GetInteger("FilterMagic", szKeyName, 0, &m_nFltMagicV[i]);
		}	
		SetFilterMagicList();

		pFile->GetInteger(SAVE_SECTION, "SaveTrash", 0, &nValue);
		m_SaveTrashCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_SAVE_TRASH, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "SaveTrashPrice", 0, &nValue);
		m_SaveTrashPrice.SetIntText(nValue);
		g_pCoreShell->PAIOperation(GPI_SAVE_TRASH_PRICE, nValue, NULL, NULL);
		pFile->GetInteger(SAVE_SECTION, "SaveJewelry", 0, &nValue);
		m_SaveJewelryCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_SAVE_JEWELRY, nValue > 0, NULL, NULL);		
	}	
}
		
void KUiPick::SaveSetting(KIniFile* pFile)
{
	if (pFile)
	{	
		pFile->WriteInteger(SAVE_SECTION, "PickFN", 			m_PickFightNoneCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "FilterEq", 			m_FilterEquipmentCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "PickMoney", 			m_PickMoneyChecker.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "PickNotEquip", 		m_PickNotEquipChecker.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "PickEquip", 			m_PickChecker.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "FollowPick", 		m_FollowPickCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "PickKind", 			m_nPickKind);
		pFile->WriteInteger(SAVE_SECTION, "SelMagic", 			m_nSelMagic);
		pFile->WriteInteger(SAVE_SECTION, "MagicValue",			m_FilterNum.GetIntNumber());
		char szKeyName[10];
		int nCount = 0;
		for (int i = 0; i < MAX_AUTO_FILTERL; i++)
		{
			if (m_szFltMagic[i][0])
			{
				sprintf(szKeyName, "Name%d", nCount);
				pFile->WriteString("FilterMagic", szKeyName, m_szFltMagic[i]);
				sprintf(szKeyName, "Value%d", nCount);
				pFile->WriteInteger("FilterMagic", szKeyName, m_nFltMagicV[i]);
				nCount++;
			}
		}
		pFile->WriteInteger(SAVE_SECTION, "SaveTrash", 		m_SaveTrashCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "SaveTrashPrice", m_SaveTrashPrice.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "SaveJewelry", 	m_SaveJewelryCK.IsButtonChecked());
	}
}
////////////////////////////////////////////
////////////////////////////////////////////
char szArray_FightBack[][64] = 
{
	NO_SETTINGS,
	"Thæ ®Þa phï",
};

char szArray_FightBoss[][64] = 
{
	NO_SETTINGS,
	"Tr¸nh tÊt c¶ Boss",
	"ChØ tr¸nh Boss Xanh",
	"ChØ tr¸nh Boss Vµng",
};

void KUiFight::Initialize()
{
	AddChild(&m_FightGuide);
	
	AddChild(&m_FightUseSBCK);
	AddChild(&m_FightUseSBTxt);
	
	AddChild(&m_RadiusText);
	AddChild(&m_RadiusShadow);
	AddChild(&m_RadiusEdit);

	AddChild(&m_OverTargetCK);
	AddChild(&m_OverTargetTxt);

	AddChild(&m_FollowTargetCK);
	AddChild(&m_FollowTargetTxt);
	
	AddChild(&m_FightNearCK);
	AddChild(&m_FightNearTxt);
	AddChild(&m_FightNearEdit);
	
	AddChild(&m_KillNpcCK);
	AddChild(&m_KillNpcTxt);
	
	AddChild(&m_SupportGuide);
	
	AddChild(&m_Support1Txt);
	AddChild(&m_Support1Shadow);
	AddChild(&m_Support1Edit);
	AddChild(&m_Support1Popup);
	
	AddChild(&m_Support2Txt);
	AddChild(&m_Support2Shadow);
	AddChild(&m_Support2Edit);
	AddChild(&m_Support2Popup);
	
	AddChild(&m_Support3Txt);
	AddChild(&m_Support3Shadow);
	AddChild(&m_Support3Edit);
	AddChild(&m_Support3Popup);
	
	AddChild(&m_SupportFightTxt);
	AddChild(&m_SupportFightShadow);
	AddChild(&m_SupportFEdit);
	AddChild(&m_SupportFightPopup);
	
	AddChild(&m_SupportPTxt);
	AddChild(&m_SupportPShadow);
	AddChild(&m_SupportPEdit);
	AddChild(&m_SupportPPopupBtn);
	
	AddChild(&m_SupportBTxt);
	AddChild(&m_SupportBShadow);
	AddChild(&m_SupportBEdit);
	AddChild(&m_SupportBPopupBtn);
	
	AddChild(&m_AuraTxt);
	AddChild(&m_Aura1Shadow);
	AddChild(&m_Aura1Edit);
	AddChild(&m_Aura1PopupBtn);
	AddChild(&m_Aura1Txt);
	AddChild(&m_Aura2Shadow);
	AddChild(&m_Aura2Edit);
	AddChild(&m_Aura2PopupBtn);

	AddChild(&m_LifeReplenishCK);
	AddChild(&m_LifeReplenishTxt);
	AddChild(&m_LifeReplenishEdit);
	
	memset(m_nSelMagic, 0, sizeof(m_nSelMagic));
}

void KUiFight::LoadScheme(KIniFile* pIni)
{
	KWndPage::Init(pIni, "Page");
	m_FightGuide.Init(pIni, "FightGuide");
	m_FightUseSBCK.Init(pIni, "FightUseSBCK");
	m_FightUseSBTxt.Init(pIni, "FightUseSBTxt");
	
	m_RadiusText.Init(pIni, "RadiusText");
	m_RadiusShadow.Init(pIni, "RadiusShadow");
	m_RadiusEdit.Init(pIni, "RadiusEdit");
	
	m_OverTargetCK.Init(pIni, "OverTargetCK");
	m_OverTargetTxt.Init(pIni, "OverTargetTxt");

	m_FollowTargetCK.Init(pIni, "FollowTargetCK");
	m_FollowTargetTxt.Init(pIni, "FollowTargetTxt");
	
	m_FightNearCK.Init(pIni, "FightNearCK");
	m_FightNearTxt.Init(pIni, "FightNearTxt");
	m_FightNearEdit.Init(pIni, "FightNearEdit");
	
	m_KillNpcCK.Init(pIni, "KillNpcCK");
	m_KillNpcTxt.Init(pIni, "KillNpcTxt");
	
	m_SupportGuide.Init(pIni, "SupportGuide");
	
	m_Support1Txt.Init(pIni, "Support1Txt");
	m_Support1Shadow.Init(pIni, "Support1Shadow");
	m_Support1Edit.Init(pIni, "Support1Edit");
	m_Support1Popup.Init(pIni, "Support1Popup");
	
	m_Support2Txt.Init(pIni, "Support2Txt");
	m_Support2Shadow.Init(pIni, "Support2Shadow");
	m_Support2Edit.Init(pIni, "Support2Edit");
	m_Support2Popup.Init(pIni, "Support2Popup");
	
	m_Support3Txt.Init(pIni, "Support3Txt");
	m_Support3Shadow.Init(pIni, "Support3Shadow");
	m_Support3Edit.Init(pIni, "Support3Edit");
	m_Support3Popup.Init(pIni, "Support3Popup");
	
	m_SupportFightTxt.Init(pIni, "SupportFightTxt");
	m_SupportFightShadow.Init(pIni, "SupportFightShadow");
	m_SupportFEdit.Init(pIni, "SupportFEdit");
	m_SupportFightPopup.Init(pIni, "SupportFightPopup");
	
	m_SupportPTxt.Init(pIni, "SupportPTxt");
	m_SupportPShadow.Init(pIni, "SupportPShadow");
	m_SupportPEdit.Init(pIni, "SupportPEdit");
	m_SupportPPopupBtn.Init(pIni, "SupportPPopupBtn");
	
	m_SupportBTxt.Init(pIni, "SupportBTxt");
	m_SupportBShadow.Init(pIni, "SupportBShadow");
	m_SupportBEdit.Init(pIni, "SupportBEdit");
	m_SupportBPopupBtn.Init(pIni, "SupportBPopupBtn");
	
	m_AuraTxt.Init(pIni, "AuraTxt");
	m_Aura1Shadow.Init(pIni, "Aura1Shadow");		
	m_Aura1Edit.Init(pIni, "Aura1Edit");
	m_Aura1PopupBtn.Init(pIni, "Aura1PopupBtn");
	m_Aura1Txt.Init(pIni, "Aura1Txt");
	m_Aura2Shadow.Init(pIni, "Aura2Shadow");		
	m_Aura2Edit.Init(pIni, "Aura2Edit");
	m_Aura2PopupBtn.Init(pIni, "Aura2PopupBtn");

	m_LifeReplenishCK.Init(pIni, "LifeReplenishCK");
	m_LifeReplenishTxt.Init(pIni, "LifeReplenishTxt");		
	m_LifeReplenishEdit.Init(pIni, "LifeReplenishEdit");
}

int KUiFight::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_FightUseSBCK)
		{
			if (m_FightUseSBCK.IsButtonChecked())
			{
				m_FightUseSBCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_FIGHT_USE_SB, TRUE, NULL, NULL);
			}
			else
			{
				m_FightUseSBCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_FIGHT_USE_SB, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FightNearCK)
		{
			if (m_FightNearCK.IsButtonChecked())
			{
				m_FightNearCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_FIGHTNEAR_CHECK, TRUE, NULL, NULL);
			}
			else
			{
				m_FightNearCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_FIGHTNEAR_CHECK, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_KillNpcCK)
		{
			if (m_KillNpcCK.IsButtonChecked())
			{
				m_KillNpcCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_ATTACK_NPC_CHECK, TRUE, NULL, NULL);
			}
			else
			{
				m_KillNpcCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_ATTACK_NPC_CHECK, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OverTargetCK)
		{
			if (m_OverTargetCK.IsButtonChecked())
			{
				m_OverTargetCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_OVER_TARGET, TRUE, NULL, NULL);
			}
			else
			{
				m_OverTargetCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_OVER_TARGET, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FollowTargetCK)
		{
			if (m_FollowTargetCK.IsButtonChecked())
			{
				m_FollowTargetCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_FOLLOW_TARGET, TRUE, NULL, NULL);
			}
			else
			{
				m_FollowTargetCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_FOLLOW_TARGET, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Support1Popup)
		{
			PopupSupportSkills(SelMagic1);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Support2Popup)
		{
			PopupSupportSkills(SelMagic2);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Support3Popup)
		{
			PopupSupportSkills(SelMagic3);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SupportFightPopup)
		{
			PopupFightSkills(SelMagic1);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SupportPPopupBtn)
		{
			PopupSupportFightSkills(SelMagic1);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_SupportBPopupBtn)
		{
			PopupSupportFightSkills(SelMagic2);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Aura1PopupBtn)
		{
			PopupAuraSkills(SelMagic1);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_Aura2PopupBtn)
		{
			PopupAuraSkills(SelMagic2);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_LifeReplenishCK)
		{
			if (m_LifeReplenishCK.IsButtonChecked())
			{
				g_pCoreShell->PAIOperation(GPI_LIFEREPLENISH, TRUE, NULL, NULL);
				m_LifeReplenishCK.CheckButton(true);
			}
			else
			{
				g_pCoreShell->PAIOperation(GPI_LIFEREPLENISH, FALSE, NULL, NULL);
				m_LifeReplenishCK.CheckButton(false);
			}
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCheckInput();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == MENU_SKILL1 && (short)(LOWORD(nParam)) >= 0)
				SetSupportSkills(SelMagic1, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL2 && (short)(LOWORD(nParam)) >= 0)
				SetSupportSkills(SelMagic2, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL3 && (short)(LOWORD(nParam)) >= 0)
				SetSupportSkills(SelMagic3, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL4 && (short)(LOWORD(nParam)) >= 0)
				SetFightSkills(SelMagic1, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL5 && (short)(LOWORD(nParam)) >= 0)
				SetFightSkills(SelMagic2, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL6 && (short)(LOWORD(nParam)) >= 0)
				SetSupportFightSkills(SelMagic1, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL7 && (short)(LOWORD(nParam)) >= 0)
				SetSupportFightSkills(SelMagic2, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL8 && (short)(LOWORD(nParam)) >= 0)
				SetAuraSkills(SelMagic1, LOWORD(nParam));
			else if (HIWORD(nParam) == MENU_SKILL9 && (short)(LOWORD(nParam)) >= 0)
				SetAuraSkills(SelMagic2, LOWORD(nParam));
		}
		break;
	default:
		return KWndPage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiFight::PaintWindow()
{
/*	KWndPage::PaintWindow();
	if	(g_pRepresentShell)
	{
		KRURect	Rect;
		Rect.Color.Color_dw = 0xff808080;
		m_FightUseSBCK.GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		Rect.oPosition.nX -= 5;
		Rect.oPosition.nY -= 4;
		GetSize(&Rect.oEndPos.nX, &Rect.oEndPos.nY);
		Rect.oEndPos.nX += (Rect.oPosition.nX - 10);
		Rect.oEndPos.nY = Rect.oPosition.nY + 48;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
		m_Support1Txt.GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		Rect.oPosition.nX -= 5;
		Rect.oPosition.nY -= 4;
		m_LifeReplenishEdit.GetAbsolutePos(NULL, &Rect.oEndPos.nY);
		Rect.oEndPos.nY += 16;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);	
	}*/
}

void KUiFight::LoadSetting(KIniFile* pFile)
{
	if (pFile)
	{
		int nValue;
		pFile->GetInteger(SAVE_SECTION, "RadiusEdit", 800, &nValue);
		if (nValue <= 0)
			nValue = 800;
		m_RadiusEdit.SetIntText(nValue);
		pFile->GetInteger(SAVE_SECTION, "FightNearEdit", 75, &nValue);
		if (nValue <= 0)
			nValue = 75;
		m_FightNearEdit.SetIntText(nValue);
		pFile->GetInteger(SAVE_SECTION, "FightUseSBCK", 0, &nValue);
		m_FightUseSBCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_FIGHT_USE_SB, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "OverTargetCK", 0, &nValue);
		g_pCoreShell->PAIOperation(GPI_OVER_TARGET, nValue > 0, NULL, NULL);
		m_OverTargetCK.CheckButton(nValue > 0);		

		pFile->GetInteger(SAVE_SECTION, "FollowTargetCK", 1, &nValue);
		g_pCoreShell->PAIOperation(GPI_FOLLOW_TARGET, nValue > 0, NULL, NULL);
		m_FollowTargetCK.CheckButton(nValue > 0);

		pFile->GetInteger(SAVE_SECTION, "FightNearCK", 0, &nValue);
		m_FightNearCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_FIGHTNEAR_CHECK, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "KillNpcCK", 1, &nValue);
		m_KillNpcCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_ATTACK_NPC_CHECK, nValue > 0, NULL, NULL);

		char szKeyName[10];
		for (int i = 0; i < SelMagicNum; i ++)
		{
			sprintf(szKeyName, "SelMagic%d", i);
			pFile->GetInteger(SAVE_SECTION, szKeyName, 0, &m_nSelMagic[i]);
			if (i >= SelMagic1 && i <= SelMagic3)
			{
				if (i == SelMagic1) nValue = 0;
				SetSupportSkills(nValue, m_nSelMagic[i]);
				nValue++;
			}
			else if (i >= SelMagicFight1 && i <= SelMagicFight2)
			{
				if (i == SelMagicFight1) nValue = 0;
				SetFightSkills(nValue, m_nSelMagic[i]);
				nValue++;
			}
			else if (i >= SelMagicSPF1 && i <= SelMagicSPF2)
			{
				if (i == SelMagicSPF1) nValue = 0;
				SetSupportFightSkills(nValue, m_nSelMagic[i]);
				nValue++;
			}
			else if (i >= SelMagicAura1 && i <= SelMagicAura2)			
			{
				if (i == SelMagicAura1) nValue = 0;
				SetAuraSkills(nValue, m_nSelMagic[i]);
				nValue++;
			}
		}
		pFile->GetInteger(SAVE_SECTION, "LifeRepCK", 0, &nValue);
		m_LifeReplenishCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_LIFEREPLENISH, nValue > 0, NULL, NULL);

		pFile->GetInteger(SAVE_SECTION, "LifeRep", 50, &nValue);	
		m_LifeReplenishEdit.SetIntText(nValue);
	}	
}
		
void KUiFight::SaveSetting(KIniFile* pFile)
{
	if (pFile)
	{
		pFile->WriteInteger(SAVE_SECTION, "RadiusEdit", 		m_RadiusEdit.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "FightNearEdit", 	m_FightNearEdit.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "FightUseSBCK", 		m_FightUseSBCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "OverTargetCK", 		m_OverTargetCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "FollowTargetCK", 	m_FollowTargetCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "FightNearCK", 		m_FightNearCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "KillNpcCK", 		m_KillNpcCK.IsButtonChecked());
		
		char szKeyName[10];
		for (int i = 0; i < SelMagicNum; i ++)
		{
			sprintf(szKeyName, "SelMagic%d", i);
			pFile->WriteInteger(SAVE_SECTION, szKeyName, 		m_nSelMagic[i]);		
		}
		pFile->WriteInteger(SAVE_SECTION, "LifeRepCK",					m_LifeReplenishCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "LifeRep",					m_LifeReplenishEdit.GetIntNumber());
	}
}

void KUiFight::OnCheckInput()
{
	g_pCoreShell->PAIOperation(GPI_RANGER_AUTO, m_RadiusEdit.GetIntNumber(), NULL, NULL);
	g_pCoreShell->PAIOperation(GPI_FIGHT_AUTO, m_FightNearEdit.GetIntNumber(), NULL, NULL);
	if (m_LifeReplenishEdit.GetIntNumber() > MAX_PERCENT)
		m_LifeReplenishEdit.SetIntText(MAX_PERCENT);
	g_pCoreShell->PAIOperation(GPI_LIFEREPLENISHP, m_LifeReplenishEdit.GetIntNumber(), NULL, NULL);
}

void KUiFight::PopupSupportSkills(int nIndex)
{
	int nActionDataCount = g_pCoreShell->GetNextSkill(0,0) + 1;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, NO_SETTINGS, sizeof(NO_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nSkillId = g_pCoreShell->GetNextSkill(0,i);
			g_pCoreShell->GetSkillName(_nSkillId, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nSkillId;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	switch (nIndex)
	{
		case SelMagic1:
			m_Support1Shadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL1);
			break;
		case SelMagic2:
			m_Support2Shadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL2);
			break;
		case SelMagic3:
			m_Support3Shadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL3);
			break;
	}
}

void KUiFight::SetSupportSkills(int nIndex, int nSkillId)
{
	g_pCoreShell->PAIOperation(GPI_SUPPORT_SKILLS, nIndex, nSkillId, NULL);
	switch (nIndex)
	{
		case SelMagic1:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagic1] = nSkillId;
				char Name[128];
				g_pCoreShell->GetSkillName(nSkillId, Name);
				m_Support1Edit.SetText(Name);
			}
			else
			{
				m_nSelMagic[SelMagic1] = 0;
				m_Support1Edit.SetText(NO_SETTINGS);
			}
			break;
		case SelMagic2:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagic2] = nSkillId;
				char Name[128];
				g_pCoreShell->GetSkillName(nSkillId, Name);
				m_Support2Edit.SetText(Name);
			}
			else
			{
				m_nSelMagic[SelMagic2] = 0;
				m_Support2Edit.SetText(NO_SETTINGS);
			}
			break;
		case SelMagic3:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagic3] = nSkillId;
				char Name[128];
				g_pCoreShell->GetSkillName(nSkillId, Name);
				m_Support3Edit.SetText(Name);
			}
			else
			{
				m_nSelMagic[SelMagic3] = 0;
				m_Support3Edit.SetText(NO_SETTINGS);
			}
			break;
	}
}

void KUiFight::SetFightSkills(int nIndex, int nSkillId)
{
	switch (nIndex)
	{
		case SelMagic1:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagicFight1] = nSkillId;
				char Name[128];
				g_pCoreShell->GetSkillName(nSkillId, Name);
				m_SupportFEdit.SetText(Name);
			}
			else
			{
				m_nSelMagic[SelMagicFight1] = 0;
				m_SupportFEdit.SetText(NO_SETTINGS);
			}	
			g_pCoreShell->PAIOperation(GPI_FIGHT_SKILLS, nSkillId, NULL, NULL);
			break;
	}
}

void KUiFight::SetSupportFightSkills(int nIndex, int nSkillId)
{
	switch (nIndex)
	{
		case SelMagic1:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagicSPF1] = nSkillId;
				m_SupportPEdit.SetText(szArray_FightBack[nSkillId]);
			}
			else
			{
				m_nSelMagic[SelMagicSPF1] = 0;
				m_SupportPEdit.SetText(NO_SETTINGS);
			}	
			g_pCoreShell->PAIOperation(GPI_FIGHTBACK, nSkillId, NULL, NULL);
			break;
		case SelMagic2:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagicSPF2] = nSkillId;
				m_SupportBEdit.SetText(szArray_FightBoss[nSkillId]);
			}
			else
			{
				m_nSelMagic[SelMagicSPF2] = 0;
				m_SupportBEdit.SetText(NO_SETTINGS);
			}
			g_pCoreShell->PAIOperation(GPI_FIGHTBOSS, nSkillId, NULL, NULL);
			break;
	}
}

void KUiFight::SetAuraSkills(int nIndex, int nSkillId)
{
	switch (nIndex)
	{
		case SelMagic1:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagicAura1] = nSkillId;
				char Name[128];
				g_pCoreShell->GetSkillName(nSkillId, Name);
				m_Aura1Edit.SetText(Name);
			}
			else
			{
				m_nSelMagic[SelMagicAura1] = 0;
				m_Aura1Edit.SetText(NO_SETTINGS);
			}	
			g_pCoreShell->PAIOperation(GPI_AURA_SKILLS1, nSkillId, NULL, NULL);
			break;
		case SelMagic2:
			if (nSkillId > 0)
			{
				m_nSelMagic[SelMagicAura2] = nSkillId;
				char Name[128];
				g_pCoreShell->GetSkillName(nSkillId, Name);
				m_Aura2Edit.SetText(Name);
			}
			else
			{
				m_nSelMagic[SelMagicAura2] = 0;
				m_Aura2Edit.SetText(NO_SETTINGS);
			}	
			g_pCoreShell->PAIOperation(GPI_AURA_SKILLS2, nSkillId, NULL, NULL);
			break;
	}
}

void KUiFight::PopupFightSkills(int nIndex)
{
	int nActionDataCount = g_pCoreShell->GetNextSkill(1,0) + 1;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, NO_SETTINGS, sizeof(NO_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nSkillId = g_pCoreShell->GetNextSkill(1,i);
			g_pCoreShell->GetSkillName(_nSkillId, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nSkillId;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	switch (nIndex)
	{
		case SelMagic1:
			m_SupportFightShadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL4);
			break;
		case SelMagic2:
			break;
	}
}

void KUiFight::PopupSupportFightSkills(int nIndex)
{
	int nActionDataCount = 0;
	switch (nIndex)
	{
		case SelMagic1:
			nActionDataCount = sizeof(szArray_FightBack) / sizeof(szArray_FightBack[0]);
			break;
		case SelMagic2:
			nActionDataCount = sizeof(szArray_FightBoss) / sizeof(szArray_FightBoss[0]);
			break;
		default:
			return;
	}
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		switch (nIndex)
		{
			case SelMagic1:
				strncpy(pSelUnitMenu->Items[i].szData, szArray_FightBack[i], sizeof(szArray_FightBack[i]));
				break;
			case SelMagic2:
				strncpy(pSelUnitMenu->Items[i].szData, szArray_FightBoss[i], sizeof(szArray_FightBoss[i]));
				break;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	switch (nIndex)
	{
		case SelMagic1:
			m_SupportPShadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL6);
			break;
		case SelMagic2:
			m_SupportBShadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL7);
			break;
	}
}

void KUiFight::PopupAuraSkills(int nIndex)
{
	int nActionDataCount = g_pCoreShell->GetNextSkill(2,0) + 1;
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		if (i == 0)
		{
			strncpy(pSelUnitMenu->Items[i].szData, NO_SETTINGS, sizeof(NO_SETTINGS));
			pSelUnitMenu->Items[i].uID = 0;
		}
		else
		{
			int _nSkillId = g_pCoreShell->GetNextSkill(2,i);
			g_pCoreShell->GetSkillName(_nSkillId, pSelUnitMenu->Items[i].szData);
			pSelUnitMenu->Items[i].uID = _nSkillId;
		}
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	switch (nIndex)
	{
		case SelMagic1:
			m_Aura1Shadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL8);
			break;
		case SelMagic2:
			m_Aura2Shadow.GetAbsolutePos(&x, &y);
			pSelUnitMenu->nX = x;
			pSelUnitMenu->nY = y;
			KPopupMenu::Popup(pSelUnitMenu, this, MENU_SKILL9);
			break;
	}
}

KUiPlayerItem*	m_pNearbyPlayersList;
char szArray_Team[][64] = 
{
	NO_SETTINGS,
	"Tù nhËn tÊt c¶ lêi mêi nhãm",
	"ChØ nhËn nhãm cã trong danh s¸ch",
	"Tù mêi nhãm tÊt c¶",
	"Tù mêi nhãm theo danh s¸ch",
	"Tõ chèi vµo nhãm",
};

void KUiOther::Initialize()
{
	AddChild(&m_FollowChecker);
	AddChild(&m_FollowText);
	AddChild(&m_FollowShadow);
	AddChild(&m_FollowEdit);
	AddChild(&m_FollowPopupBtn);
	AddChild(&m_FollowRadiusEdit);
	
	AddChild(&m_MoveMpsCK);
	AddChild(&m_MoveMpsText);
	AddChild(&m_MoveMpsShadow);
	AddChild(&m_MoveMpsL);
	AddChild(&m_MoveMpsL_Scroll);
	m_MoveMpsL.SetScrollbar(&m_MoveMpsL_Scroll);
	AddChild(&m_MoveMpsGetBtn);
	AddChild(&m_MoveMpsDelAllBtn);		
	
	AddChild(&m_PTChecker);
	AddChild(&m_PTText);
	AddChild(&m_PTShadow);
	AddChild(&m_PTEdit);
	AddChild(&m_PTPopupBtn);
	AddChild(&m_PTListShadow);
	AddChild(&m_PTListL);
	AddChild(&m_PTListL_Scroll);
	m_PTListL.SetScrollbar(&m_PTListL_Scroll);
	AddChild(&m_PTListGetBtn);
	AddChild(&m_PTListDelAllBtn);		
	
	AddChild(&m_OtherTextTS);
	AddChild(&m_OtherTextTD);
	AddChild(&m_FollowPVText);
	
	m_nParTySel = 0;
	memset(m_szFollowName, 0, sizeof(m_szFollowName));
	memset(m_szPartyList, 0, sizeof(m_szPartyList));
	memset(m_nMpsList, 0, sizeof(m_nMpsList));
}

void KUiOther::LoadScheme(KIniFile* pIni)
{
	KWndPage::Init(pIni, "Page");
	
	m_OtherTextTS.Init(pIni, "OtherTextTS"); // them
	m_OtherTextTD.Init(pIni, "OtherTextTD"); // them
	m_FollowPVText.Init(pIni, "FollowPVText"); // them
	
	m_FollowChecker.Init(pIni, "FollowChecker");
	m_FollowText.Init(pIni, "FollowText");
	m_FollowShadow.Init(pIni, "FollowShadow");
	m_FollowEdit.Init(pIni, "FollowEdit");
	m_FollowPopupBtn.Init(pIni, "FollowPopupBtn");
	m_FollowRadiusEdit.Init(pIni, "FollowRadiusEdit");
	
	m_MoveMpsText.Init(pIni, "MoveMpsText");
	m_MoveMpsCK.Init(pIni, "MoveMpsCK");
	m_MoveMpsShadow.Init(pIni, "MoveMpsShadow");
	m_MoveMpsL.Init(pIni, "MoveMpsL");
	m_MoveMpsL_Scroll.Init(pIni, "MoveMpsL_Scroll");
	m_MoveMpsGetBtn.Init(pIni, "MoveMpsGetBtn");
	m_MoveMpsDelAllBtn.Init(pIni, "MoveMpsDelAllBtn");		
	
	m_PTChecker.Init(pIni, "PTChecker");
	m_PTText.Init(pIni, "PTText");
	m_PTShadow.Init(pIni, "PTShadow");
	m_PTEdit.Init(pIni, "PTEdit");
	m_PTPopupBtn.Init(pIni, "PTPopupBtn");	
	m_PTListShadow.Init(pIni, "PTListShadow");
	m_PTListL.Init(pIni, "PTListL");
	m_PTListL_Scroll.Init(pIni, "PTListL_Scroll");
	m_PTListGetBtn.Init(pIni, "PTListGetBtn");
	m_PTListDelAllBtn.Init(pIni, "PTListDelAllBtn");		
}

int KUiOther::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_FollowChecker)
		{
			if (m_FollowChecker.IsButtonChecked())
			{
				g_pCoreShell->PAIOperation(GPI_FOLLOW_PEOPLE, TRUE, NULL, NULL);
				m_FollowChecker.CheckButton(true);

				if (m_MoveMpsCK.IsButtonChecked())
				{
					m_MoveMpsCK.CheckButton(false);
					g_pCoreShell->PAIOperation(GPI_AUTO_MOVEMPS, FALSE, NULL, NULL);
				}
			}
			else
			{
				g_pCoreShell->PAIOperation(GPI_FOLLOW_PEOPLE, FALSE, NULL, NULL);
				m_FollowChecker.CheckButton(false);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_FollowPopupBtn)
			PlayerList(MENU_FOLLOW);
		else if (uParam == (unsigned int)(KWndWindow*)&m_MoveMpsCK)
		{
			if (m_MoveMpsCK.IsButtonChecked())
			{
				m_MoveMpsCK.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_AUTO_MOVEMPS, TRUE, NULL, NULL);

				if (m_FollowChecker.IsButtonChecked())
				{
					g_pCoreShell->PAIOperation(GPI_FOLLOW_PEOPLE, FALSE, NULL, NULL);
					m_FollowChecker.CheckButton(false);
				}
			}
			else
			{
				m_MoveMpsCK.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_AUTO_MOVEMPS, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PTChecker)
		{
			if (m_PTChecker.IsButtonChecked())
			{
				m_PTChecker.CheckButton(true);
				g_pCoreShell->PAIOperation(GPI_AUTO_PARTY, TRUE, NULL, NULL);
			}
			else
			{
				m_PTChecker.CheckButton(false);
				g_pCoreShell->PAIOperation(GPI_AUTO_PARTY, FALSE, NULL, NULL);
			}
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_PTPopupBtn)
			OpenPopup(MENU_TEAM);
		else if (uParam == (unsigned int)(KWndWindow*)&m_PTListGetBtn)
			PlayerList(MENU_PTADD);
		else if (uParam == (unsigned int)(KWndWindow*)&m_MoveMpsGetBtn)
			InsertMoveMpsList(m_SceneInfo.nSceneId, m_SceneInfo.nScenePos0, m_SceneInfo.nScenePos1);
		else if (uParam == (unsigned int)(KWndWindow*)&m_MoveMpsDelAllBtn)
		{
			memset(m_nMpsList, 0, sizeof(m_nMpsList));
			SetMoveMpsList();
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_PTListDelAllBtn)
		{
			memset(m_szPartyList, 0, sizeof(m_szPartyList));
			SetPartyList();
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCheckInput();
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == MENU_TEAM && (short)(LOWORD(nParam)) >= 0)
			{
				SetPTSel(LOWORD(nParam));
			}
			else if (HIWORD(nParam) == MENU_FOLLOW && (short)(LOWORD(nParam)) >= 0)
			{
				strcpy(m_szFollowName, m_pNearbyPlayersList[(short)(LOWORD(nParam))].Name);
				m_FollowEdit.SetText(m_szFollowName);
				g_pCoreShell->PAIOperation(GPI_FOLLOW_PEOPLE_NAME, (unsigned int)&m_szFollowName, NULL, NULL);
			}
			else if (HIWORD(nParam) == MENU_PTADD && (short)(LOWORD(nParam)) >= 0)
			{
				if (FilterSamePartyList(m_pNearbyPlayersList[(short)(LOWORD(nParam))].Name))
					return 0;

				for (int i = 0; i < MAX_AUTO_LIST; i ++)
				{
					if (!m_szPartyList[i][0])
					{
						strcpy(m_szPartyList[i], m_pNearbyPlayersList[(short)(LOWORD(nParam))].Name);
						SetPartyList();
						return 0;
					}
				}
			}
		}
		break;
	case WND_M_MENUITEMRIGHT_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == MENU_PLAYER && (short)(LOWORD(nParam)) >= 0)
			{
				m_szPartyList[(short)(LOWORD(nParam))][0] = 0;
				SetPartyList();
			}
		}
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)&m_MoveMpsL_Scroll)
			m_MoveMpsL.SetTopItemIndex(nParam);
		break;
	default:
		return KWndPage::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

BOOL KUiOther::InsertMoveMpsList(int nSubWorldId, int nMpsX, int nMpsY)
{
	int i;
	for (i = 0; i < MAX_AUTO_LIST; i ++)
	{
		if (m_nMpsList[i][0] == nSubWorldId && 
			m_nMpsList[i][1] == nMpsX && 
			m_nMpsList[i][2] == nMpsY)
		{
			i = MAX_AUTO_LIST;
			break;
		}
		else if (m_nMpsList[i][0] == 0 && 
				m_nMpsList[i][1] == 0 && 
				m_nMpsList[i][2] == 0)
		{
			break;
		}
	}
	if (i < MAX_AUTO_LIST)
	{
		m_nMpsList[i][0] = nSubWorldId;
		m_nMpsList[i][1] = nMpsX;
		m_nMpsList[i][2] = nMpsY;
		SetMoveMpsList();
	}
	return TRUE;
}

void KUiOther::SetMoveMpsList()
{
	char szList[32];
	m_MoveMpsL.ResetContent();
	for (int i = 0; i < MAX_AUTO_LIST; i ++)
	{
		g_pCoreShell->PAIOperation(GPI_AUTO_MOVEMPSID, i, m_nMpsList[i][0], NULL);
		g_pCoreShell->PAIOperation(GPI_AUTO_MOVEMPSX, i, m_nMpsList[i][1], NULL);
		g_pCoreShell->PAIOperation(GPI_AUTO_MOVEMPSY, i, m_nMpsList[i][2], NULL);

		if(m_nMpsList[i][0])
		{
			sprintf(szList, MOVEMPS_KEYNAME, m_nMpsList[i][0], m_nMpsList[i][1], m_nMpsList[i][2]);
			m_MoveMpsL.AddString(i, szList);
		}
	}
}

void KUiOther::LoadSetting(KIniFile* pFile)
{
	if (pFile)
	{
		int nValue, i;
		memset(m_nMpsList, 0, sizeof(m_nMpsList));
		pFile->GetInteger(SAVE_SECTION, "MoveCK", 0, &nValue);
		m_MoveMpsCK.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_AUTO_MOVEMPS, nValue > 0, NULL, NULL);
		char szKeyName[10];
		for (i = 0; i < MAX_AUTO_LIST; i++)
		{
			sprintf(szKeyName, "%d", i);
			pFile->GetInteger3("MoveMpsList", szKeyName, &m_nMpsList[i][0], &m_nMpsList[i][1], &m_nMpsList[i][2]);
		}
		SetMoveMpsList();
		pFile->GetInteger(SAVE_SECTION, "Follow", 0, &nValue);
		m_FollowChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_FOLLOW_PEOPLE, nValue > 0, NULL, NULL);

		pFile->GetString(SAVE_SECTION, "FollowName", "", m_szFollowName, sizeof(m_szFollowName));
		if (m_szFollowName[0])
			m_FollowEdit.SetText(m_szFollowName);
		else
			m_FollowEdit.SetText(NORMAL_UNCLEAR_WORD);
		g_pCoreShell->PAIOperation(GPI_FOLLOW_PEOPLE_NAME, (unsigned int)&m_szFollowName, NULL, NULL);
		pFile->GetInteger(SAVE_SECTION, "FollowRadius", 0, &nValue);
		if (nValue <= 0)
			nValue = 300;
		m_FollowRadiusEdit.SetIntText(nValue);
			
		pFile->GetInteger(SAVE_SECTION, "Party", 0, &nValue);
		m_PTChecker.CheckButton(nValue > 0);
		g_pCoreShell->PAIOperation(GPI_AUTO_PARTY, nValue > 0, NULL, NULL);
	
		for (i = 0; i < MAX_AUTO_LIST; i ++)
		{
			sprintf(szKeyName, "%d", i);
			pFile->GetString("PartyList", szKeyName, "", m_szPartyList[i], sizeof(m_szPartyList[i]));
		}
		SetPartyList();
		pFile->GetInteger(SAVE_SECTION, "PartyKind", 0, &nValue);
		SetPTSel(nValue);
	}	
}
		
void KUiOther::SaveSetting(KIniFile* pFile)
{
	if (pFile)
	{	
		char szKeyName[10];
		int nCount = 0;
		for (int i = 0; i < MAX_AUTO_LIST; i++)
		{
			if (m_nMpsList[i][0])
			{
				sprintf(szKeyName, "%d", nCount);
				pFile->WriteInteger3("MoveMpsList", szKeyName, m_nMpsList[i][0], m_nMpsList[i][1], m_nMpsList[i][2]);
				nCount++;
			}
		}
		pFile->WriteInteger(SAVE_SECTION, "Follow", 		m_FollowChecker.IsButtonChecked());
		pFile->WriteString(SAVE_SECTION,  "FollowName", 	m_szFollowName);
		pFile->WriteInteger(SAVE_SECTION, "FollowRadius", 	m_FollowRadiusEdit.GetIntNumber());
		pFile->WriteInteger(SAVE_SECTION, "MoveCK", 		m_MoveMpsCK.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "Party", 			m_PTChecker.IsButtonChecked());
		pFile->WriteInteger(SAVE_SECTION, "PartyKind", 		m_nParTySel);
		nCount = 0;
		for (int i = 0; i < MAX_AUTO_LIST; i ++)
		{
			if (m_szPartyList[i][0])
			{
				sprintf(szKeyName, "%d", nCount);
				pFile->WriteString("PartyList", szKeyName, m_szPartyList[i]);
				nCount++;
			}
		}
	}
}

void KUiOther::OpenPopup(int nIndex)
{
	int nActionDataCount = sizeof(szArray_Team) / sizeof(szArray_Team[0]);
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	for (int i = 0; i < nActionDataCount; i++)
	{
		strncpy(pSelUnitMenu->Items[i].szData, szArray_Team[i], sizeof(szArray_Team[i]));
		pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
		pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
		pSelUnitMenu->nNumItem++;
	}
	int x, y;
	m_PTShadow.GetAbsolutePos(&x, &y);
	pSelUnitMenu->nX = x;
	pSelUnitMenu->nY = y;
	KPopupMenu::Popup(pSelUnitMenu, this, nIndex);
}


void KUiOther::SetPTSel(int nSel)
{
	m_nParTySel = nSel;
	m_PTEdit.SetText(szArray_Team[m_nParTySel]);

	g_pCoreShell->TeamOperation(TEAM_OI_REFUSE_INVITE, 0, FALSE);
	g_pCoreShell->PAIOperation(GPI_AUTO_INVITE, FALSE, NULL, NULL);
	g_pCoreShell->PAIOperation(GPI_AUTO_INVITE_LIST, FALSE, NULL, NULL);
	g_pCoreShell->PAIOperation(GPI_AUTO_ACCEPT, FALSE, NULL, NULL);
	g_pCoreShell->PAIOperation(GPI_AUTO_ACCEPT_WITH_NAME, FALSE, NULL, NULL);

	switch (m_nParTySel)
	{
		case NoneSelect:
			break;
		case AutoAccept:
			g_pCoreShell->PAIOperation(GPI_AUTO_ACCEPT, TRUE, NULL, NULL);
			break;
		case AutoAcceptWithName:
			g_pCoreShell->PAIOperation(GPI_AUTO_ACCEPT_WITH_NAME, TRUE, NULL, NULL);
			break;
		case AutoInvite:
			g_pCoreShell->PAIOperation(GPI_AUTO_INVITE, TRUE, NULL, NULL);
			break;
		case AutoInviteWithList:
			g_pCoreShell->PAIOperation(GPI_AUTO_INVITE, TRUE, NULL, NULL);
			g_pCoreShell->PAIOperation(GPI_AUTO_INVITE_LIST, TRUE, NULL, NULL);
			break;
		case AutoDenyInvite:
			g_pCoreShell->TeamOperation(TEAM_OI_REFUSE_INVITE, 0, TRUE);
			break;
		default:
			break;
	}
}

void KUiOther::SetPartyList()
{
	m_PTListL.ResetContent();
	int nCount = 0;
	for (int i = 0; i < MAX_AUTO_LIST; i ++)
	{
		g_pCoreShell->PAIOperation(GPI_SET_INVITE_LIST, (unsigned int)&m_szPartyList[i], i, NULL);
		if (m_szPartyList[i][0])
		{
			m_PTListL.AddString(nCount, m_szPartyList[i]);
			nCount++;
		}
	}
}

BOOL KUiOther::FilterSamePartyList(const char* Name)
{
	if (!Name || !Name[0])
		return FALSE;

	for (int i = 0; i < MAX_AUTO_LIST; i ++)
	{
		if (m_szPartyList[i][0] && strcmp(Name, m_szPartyList[i]) == 0)
			return TRUE;
	}
	return FALSE;
}

void KUiOther::PlayerList(int uParam)
{
	if (m_pNearbyPlayersList)
	{
		free (m_pNearbyPlayersList);
		m_pNearbyPlayersList = NULL;
	}
	int nActionDataCount = 0;
	if (uParam == MENU_PLAYER)
	{
		for (int i = 0; i < MAX_AUTO_LIST; i ++)
		{
			if (m_szPartyList[i][0])
				nActionDataCount++;
		}
		if (!nActionDataCount)
		{
			return;
		}
	}
	else
	{
		nActionDataCount = g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, 0, 0);
		if (!nActionDataCount)
		{
			return;
		}
	}
	struct KPopupMenuData* pSelUnitMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nActionDataCount));
	if (pSelUnitMenu == NULL)
		return;
	KPopupMenu::InitMenuData(pSelUnitMenu, nActionDataCount);
	pSelUnitMenu->nNumItem = 0;
	pSelUnitMenu->usMenuFlag |= PM_F_AUTO_DEL_WHEN_HIDE;
	m_pNearbyPlayersList = (KUiPlayerItem*)malloc(sizeof(KUiPlayerItem) * nActionDataCount);
	if (m_pNearbyPlayersList)
	{
		g_pCoreShell->GetGameData(GDI_NEARBY_PLAYER_LIST, (unsigned int)m_pNearbyPlayersList, nActionDataCount);

		if (uParam == MENU_PLAYER)
		{
			for (int i = 0; i < MAX_AUTO_LIST; i++)
			{
				if (m_szPartyList[i][0])
				{
					strncpy(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData, m_szPartyList[i], sizeof(m_szPartyList[i]));
					pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData[sizeof(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData) - 1] = 0;
					pSelUnitMenu->Items[pSelUnitMenu->nNumItem].uDataLen = strlen(pSelUnitMenu->Items[pSelUnitMenu->nNumItem].szData);
					pSelUnitMenu->Items[pSelUnitMenu->nNumItem].uID = i;
					pSelUnitMenu->nNumItem++;
				}
			}
		}
		else
		{
			for (int i = 0; i < nActionDataCount; i++)
			{
				strncpy(pSelUnitMenu->Items[i].szData, m_pNearbyPlayersList[i].Name, sizeof(m_pNearbyPlayersList[i].Name));
				pSelUnitMenu->Items[i].szData[sizeof(pSelUnitMenu->Items[i].szData) - 1] = 0;
				pSelUnitMenu->Items[i].uDataLen = strlen(pSelUnitMenu->Items[i].szData);
				pSelUnitMenu->nNumItem++;
			}
		}
		int x, y;
		GetAbsolutePos(&x, &y);
		if (uParam == MENU_FOLLOW)
			m_FollowShadow.GetAbsolutePos(&x, &y);
		else
			m_PTListShadow.GetAbsolutePos(&x, &y);
		pSelUnitMenu->nX = x;
		pSelUnitMenu->nY = y;
		KPopupMenu::Popup(pSelUnitMenu, this, uParam);
	}
}

void KUiOther::PaintWindow()
{
	KWndPage::PaintWindow();
	if	(g_pRepresentShell)
	{
		KRURect	Rect;
		Rect.Color.Color_dw = 0xff808080;
		m_FollowChecker.GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
//		Rect.oPosition.nX -= 5;
//		Rect.oPosition.nY -= 4;
//		m_MoveMpsCK.GetAbsolutePos(NULL, &Rect.oEndPos.nY);
//		GetSize(&Rect.oEndPos.nX, NULL);
//		Rect.oEndPos.nX += (Rect.oPosition.nX - 10);
//		Rect.oEndPos.nY -= 8;
//		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
//		Rect.oPosition.nY = Rect.oEndPos.nY + 4;
//		m_PTChecker.GetAbsolutePos(NULL, &Rect.oEndPos.nY);
//		Rect.oEndPos.nY -= 14;
//		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
//		Rect.oPosition.nY = Rect.oEndPos.nY + 4;
//		m_PTListL.GetSize(NULL, &Rect.oEndPos.nY);
//		Rect.oEndPos.nY = Rect.oEndPos.nY + Rect.oPosition.nY + 28;
//		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);

		Rect.oPosition.nY = Rect.oEndPos.nY + 4;
		m_MoveMpsL_Scroll.GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		m_MoveMpsL_Scroll.GetSize(&Rect.oEndPos.nX, &Rect.oEndPos.nY);
		Rect.oEndPos.nX += Rect.oPosition.nX;
		Rect.oEndPos.nY += Rect.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
		m_PTListL_Scroll.GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		m_PTListL_Scroll.GetSize(&Rect.oEndPos.nX, &Rect.oEndPos.nY);
		Rect.oEndPos.nX += Rect.oPosition.nX;
		Rect.oEndPos.nY += Rect.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
	}
}

void KUiOther::OnCheckInput()
{
	g_pCoreShell->PAIOperation(GPI_FOLLOW_PEOPLE_RADIUS, m_FollowRadiusEdit.GetIntNumber(), NULL, NULL);
}


KUiAutoPlay* KUiAutoPlay::m_pSelf = NULL;

KUiAutoPlay* KUiAutoPlay::OpenWindow(bool bShow)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiAutoPlay;
		if (m_pSelf)
			m_pSelf->Initialize();
		if (bShow)m_pSelf->m_RecoveryPad.Show();
	
	}
	if (m_pSelf && bShow)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}


KUiAutoPlay* KUiAutoPlay::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

void KUiAutoPlay::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{	
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KUiAutoPlay::Initialize()
{
	m_RecoveryPad.Initialize();
	AddPage(&m_RecoveryPad, &m_RecoveryBtn);
	m_PickPad.Initialize();
	AddPage(&m_PickPad, &m_PickBtn);
	m_FightPad.Initialize();
	AddPage(&m_FightPad, &m_FightBtn);
	m_OtherPad.Initialize();
	AddPage(&m_OtherPad, &m_OtherBtn);
	
	AddChild(&m_ActiveBtn);
	AddChild(&m_CloseBtn);

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
	
	Wnd_AddWindow(this);
}

void KUiAutoPlay::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_ActiveBtn.Init(&Ini, "ActiveText");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseText");
		
		m_pSelf->m_RecoveryBtn.Init(&Ini, "RecoveryBtn");
		m_pSelf->m_PickBtn.Init(&Ini, "PickBtn");
		m_pSelf->m_FightBtn.Init(&Ini, "FightBtn");
		m_pSelf->m_OtherBtn.Init(&Ini, "OtherBtn");
		
		m_pSelf->m_RecoveryPad.LoadScheme(&Ini);
		m_pSelf->m_PickPad.LoadScheme(&Ini);
		m_pSelf->m_FightPad.LoadScheme(&Ini);
		m_pSelf->m_OtherPad.LoadScheme(&Ini);
	}
}

int KUiAutoPlay::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if(uParam == (unsigned int)(KWndWindow*)&m_ActiveBtn)
			g_pCoreShell->PAIOperation(GPI_SWITCH_ACTIVE, NULL, NULL, NULL);
		else if(uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow(false);
		break;
	}
	return KWndPageSet::WndProc(uMsg, uParam, nParam);
}

void KUiAutoPlay::PaintWindow()
{
	KWndPageSet::PaintWindow();
	if	(g_pRepresentShell)
	{
		KRUShadow	Shadow;
		GetAbsolutePos(&Shadow.oPosition.nX, &Shadow.oPosition.nY);
		Shadow.Color.Color_dw = (GetColor("6,1,97") & 0xffffff) | ((192 << 21) & 0xff000000);
		GetSize(&Shadow.oEndPos.nX, &Shadow.oEndPos.nY);
		Shadow.oEndPos.nX += Shadow.oPosition.nX;
		Shadow.oEndPos.nY += Shadow.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);

/*		KRURect	Rect;
		GetAbsolutePos(&Rect.oPosition.nX, &Rect.oPosition.nY);
		Rect.Color.Color_dw = 0xff808080;
		GetSize(&Rect.oEndPos.nX, &Rect.oEndPos.nY);
		Rect.oEndPos.nX += Rect.oPosition.nX;
		Rect.oEndPos.nY += Rect.oPosition.nY;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);		
		Rect.oPosition.nX -= 1;
		Rect.oPosition.nY -= 1;
		Rect.oEndPos.nX -= 1;
		Rect.oEndPos.nY -= 1;
		g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);	*/
	}
	m_ActiveBtn.CheckButton(g_pCoreShell->PAIOperation(GPI_ISACTIVE, NULL, NULL, NULL));
}

int KUiAutoPlay::SavePrivateSetting(KIniFile* pFile)
{
	if (m_pSelf)
	{
		if (pFile && g_pCoreShell)
		{
			m_pSelf->m_RecoveryPad.SaveSetting(pFile);
			m_pSelf->m_PickPad.SaveSetting(pFile);
			m_pSelf->m_FightPad.SaveSetting(pFile);
			m_pSelf->m_OtherPad.SaveSetting(pFile);		
		}
	}
	return 1;
}

BOOL KUiAutoPlay::LoadPrivateSetting(KIniFile* pFile)
{
	if (m_pSelf)
	{
		if (pFile && g_pCoreShell)
		{
			m_pSelf->m_RecoveryPad.LoadSetting(pFile);
			m_pSelf->m_PickPad.LoadSetting(pFile);
			m_pSelf->m_FightPad.LoadSetting(pFile);
			m_pSelf->m_OtherPad.LoadSetting(pFile);
		}
	}
	return TRUE;
}
void KUiAutoPlay::UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo)
{
	if (m_pSelf && pInfo)
		m_pSelf->m_OtherPad.m_SceneInfo = *pInfo;
}
