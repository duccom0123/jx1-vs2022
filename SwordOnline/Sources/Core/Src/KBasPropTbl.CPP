//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KBasPropTbl.CPP
// Date:	2002.08.14
// Code:	DongBo
// Desc:    cpp file. ���ļ�ʵ�ֵ������ڴ�tab file�ж������ߵĳ�ʼ����,
//			�����ɶ�Ӧ�����Ա�
//---------------------------------------------------------------------------

#include "KCore.h"
#include "KTabFile.h"
#include "MyAssert.H"
#include "KBasPropTbl.h"

#define		TABFILE_PATH				"\\settings\\item"
#define		TABFILE_EVENTITEM			"eventitem.txt"
#define		TABFILE_TASK				"questkey.txt"
#define		TABFILE_MEDICINE			"potion.txt"
#define		TABFILE_MELEEWEAPON			"MeleeWeapon.txt"
#define		TABFILE_RANGEWEAPON			"RangeWeapon.txt"
#define		TABFILE_ARMOR				"Armor.txt"
#define		TABFILE_HELM				"Helm.txt"
#define		TABFILE_BOOT				"Boot.txt"
#define		TABFILE_BELT				"Belt.txt"
#define		TABFILE_AMULET				"Amulet.txt"
#define		TABFILE_RING				"Ring.txt"
#define		TABFILE_CUFF				"Cuff.txt"
#define		TABFILE_PENDANT				"Pendant.txt"
#define		TABFILE_HORSE				"Horse.txt"
#define		TABFILE_TOWNPORTAL			"TownPortal.txt"
#define		TABFILE_MAGICSCRIPT			"magicscript.txt"
#define		TABFILE_MAGICATTRIB			"magicattrib.txt"
#define		TABFILE_GOLDEQUIP			"GoldEquip.txt"
#define		TABFILE_PLATINAEQUIP		"platinaequip.txt"
#define		TABFILE_MASK				"Mask.txt"
#define		TABFILE_MANTLE				"Mantle.txt"
#define		TABFILE_SIGNET				"Signet.txt"
#define		TABFILE_SHIPIN				"Shipin.txt"
#define		TABFILE_HOODS				"Hoods.txt"
#define		TABFILE_CLOAK				"Cloak.txt"

// ���¶���Ľṹ���ڸ�����tabfile�ж������Եĳ�ʼֵ
typedef struct tagPROPINFO
{
	int		m_nType;		// ���Ե�����. ��� PI_VARTYPE_...ϵ�ж���
	union
	{
	char*	m_pszBuf;		// ָ���ַ�����������ָ��
	int*	m_pnData;		// ָ��int������ָ��
	}m_pData;
	int		m_nBufSize;		// �������ĳ���
} PROPINFO;
#define		PI_VARTYPE_CHAR		0
#define		PI_VARTYPE_INT		1

char*	TABFILE_EQUIPMENT[] = 
{
	TABFILE_MELEEWEAPON,	//"MeleeWeapon.txt",
	TABFILE_RANGEWEAPON,	//"RangeWeapon.txt",
	TABFILE_ARMOR,			//"Armor.txt",
	TABFILE_HELM,			//"Helm.txt",
	TABFILE_BOOT,			//"Boot.txt",
	TABFILE_BELT,			//"Belt.txt",
	TABFILE_AMULET,			//"Amulet.txt",
	TABFILE_RING,			//"Ring.txt",
	TABFILE_CUFF,			//"Cuff.txt",
	TABFILE_PENDANT,		//"Pendant.txt",
	TABFILE_HORSE,			//"Horse.txt",
	TABFILE_MASK,			//"Mask.txt"
	TABFILE_MANTLE,			//"Mantle.txt"
	TABFILE_SIGNET,			//"Signet.txt"
	TABFILE_SHIPIN,			//"Shipin.txt"
	TABFILE_HOODS,			//"Hoods.txt"
	TABFILE_CLOAK,			//"Cloak.txt"
};

//int GetRandomNumber(int nMin, int nMax);

//=============================================================================

/******************************************************************************
	����:	��tab file�ж����ض������ݼ�¼
	���:	pTF: ������ָ��, �ô˹������ȡtab file
			nRow: ����nRow���¼
			pPI[i].m_nType: ���������ļ�¼�е�i���������, ���������ͻ��ַ���
			pPI[i].m_pData: �����ݶ����˻�������
			cbFields: ÿ���¼������ô�����
	����:	�ɹ�ʱ���ط���, m_pBuf ָ�������ڴ�
			ʧ��ʱ������
******************************************************************************/
BOOL LoadRecord(IN KTabFile* pTF, IN int nRow,
				IN OUT const PROPINFO* pPI, IN int cbFields)
{
	BOOL bEC = TRUE;

	nRow += 2;	// ��1: ����tabfile�ĵ�һ��. ���и������Ǹ��е�����
				// �ټ�1: KTabFile::GetInteger()����Ҫ��nRow��1��ʼ����

	// ��������������
	for (int n = 0; n < cbFields; n++)
	{
		if (PI_VARTYPE_INT == (pPI+n)->m_nType)
		{	// ���� int ������
			if (FALSE == pTF->GetInteger(nRow, n+1, -1, (pPI+n)->m_pData.m_pnData))
				{ _ASSERT(FALSE); bEC = FALSE; break; }
		}
		else
		{	// �����ַ���������
			_ASSERT(PI_VARTYPE_CHAR == (pPI+n)->m_nType);

			if (FALSE == pTF->GetString(nRow, n+1, "", (pPI+n)->m_pData.m_pszBuf,
										(pPI+n)->m_nBufSize))
				{ _ASSERT(FALSE); bEC = FALSE; break; }
		}
	}
	return bEC;
}

//=============================================================================

KLibOfBPT::KLibOfBPT()
{
}

KLibOfBPT::~KLibOfBPT()
{
}

/******************************************************************************
	���ܣ�	�ܿأ���tab file�ж������,ħ����ԭʼ����
	����:	������ݴ���m_BPTWeapon,m_BPTWeaponDirty�ȳ�Ա������
******************************************************************************/
BOOL KLibOfBPT::Init()
{
	// ��ʼ��
	KBasicPropertyTable*	paryBPT[] = {	
											&m_BPTMeleeWeapon,
											&m_BPTRangeWeapon,
											&m_BPTArmor,
											&m_BPTHelm,
											&m_BPTBoot,
											&m_BPTBelt,
											&m_BPTAmulet,
											&m_BPTRing,
											&m_BPTCuff,
											&m_BPTPendant,
											&m_BPTHorse,
											&m_BPTMask,
											&m_BPTMantle,
											&m_BPTSignet,
											&m_BPTShipin,
											&m_BPTHoods,
											&m_BPTCloak,
											&m_BPTGoldEquip,
											&m_BPTPlatinaEquip,
											&m_BPTMedicine,
											&m_BPTEvent,
											&m_BPTQuest,
											&m_BPTTownPortal,
											&m_BPTMagicScript,
											&m_BPTMagicAttrib,
										};
	// ��tab file�������
	const int cbNumOfTables = sizeof(paryBPT)/sizeof(paryBPT[0]);
	for (int i = 0; i < cbNumOfTables; i++)
	{
		if (i < 17)	// װ�����⴦��
		{
			KBPT_Equipment* pTemp = (KBPT_Equipment *)paryBPT[i];
			pTemp->Init(i);
		}
		else if (i == 17)
		{
			KBPT_Equipment_Gold* pTemp = (KBPT_Equipment_Gold *)paryBPT[i];
			pTemp->Init();
		}
		else if (i == 18)
		{
			KBPT_Equipment_Platina* pTemp = (KBPT_Equipment_Platina *)paryBPT[i];
			pTemp->Init();
		}
		if (FALSE == paryBPT[i]->Load())
			{ _ASSERT(FALSE); return FALSE; }
	}

	// ����ħ�����Ա�
	InitMALib();
    
    // ����ħ������������
    // add by Freeway Chen in 2003.5.30
    InitMAIT();

	return TRUE;
}

/******************************************************************************
	���ܣ�	����һ����ά�����飬����(ǰ��׺����Ʒ���͡����С�����)
            ȷ��һ��ħ�����Ե�����ֵ�б�
	���:	m_BPTMagicAttrib: �ں���tab file�����ȫ��ħ������
	����:	��ά���� m_CMAIT �ֱ������Ӧ������ֵ�б�
******************************************************************************/
// Add by Freeway Chen in 2003.5.30
BOOL KLibOfBPT::InitMAIT()
{
    int nResult = false;
    int i = 0;
    int nPrefixPostfix = 0;
    int nType = 0;
    int nSeries = 0;
    int nSeriesMin = 0;
    int nSeriesMax = 0;

    int nLevel  = 0;
    int nLevelMin = 0;
    int nLevelMax = 0;

    int nTotalCount = 0;

    for (i = 0; i < m_BPTMagicAttrib.NumOfEntries(); i++)
    {
        KMAGICATTRIB_TABFILE *pItem = (KMAGICATTRIB_TABFILE *)m_BPTMagicAttrib.GetRecord(i);
        if (!pItem)
        {
            _ASSERT(pItem);
            continue;
        }

        pItem->m_nUseFlag = false;

        nPrefixPostfix = pItem->m_nPos;
        _ASSERT((nPrefixPostfix >= 0) && (nPrefixPostfix < MATF_PREFIXPOSFIX));

        for (nType = 0; nType < MATF_CBDR; nType++)
        {
            if ((pItem->m_DropRate[nType]) == 0)
                continue; // ���û�и��ʳ���������;�������һ��
            
            nSeriesMin = nSeriesMax = pItem->m_nClass;
            if ((pItem->m_nClass) == -1)
            {
                nSeriesMin = 0;
                nSeriesMax = MATF_SERIES - 1;
            }
            else
            {
                _ASSERT(((pItem->m_nClass) >= 0) && ((pItem->m_nClass) < MATF_SERIES));
            }

            for (nSeries = nSeriesMin; nSeries <= nSeriesMax; nSeries++)
            {
                nLevelMin = pItem->m_nLevel;
                nLevelMax = MATF_LEVEL;

                for (nLevel = nLevelMin; nLevel <= nLevelMax; nLevel++)
                {  
                    m_CMAIT[nPrefixPostfix][nType][nSeries][nLevel - 1].Insert(i);
                    nTotalCount++;
                }
            }
        }
    }

    //#ifdef _DEBUG
    #if 0
    for (nPrefixPostfix = 0; nPrefixPostfix < MATF_PREFIXPOSFIX; nPrefixPostfix++)
    {
        for (nType = 0; nType < MATF_CBDR; nType++)
        {
            for (nSeries = 0; nSeries < MATF_SERIES; nSeries++)
            {
                for (nLevel = 1; nLevel < (MATF_LEVEL + 1); nLevel++)
                {
                    KBPT_ClassMAIT *pMAITItem = &m_CMAIT[nPrefixPostfix][nType][nSeries][nLevel - 1];

                    char szOutputString[8192];
                    char szStringContent[4096];
                    szStringContent[0] = '\0';

                    for (i = 0; i < pMAITItem->GetCount(); i++)
                    {
                        KMAGICATTRIB_TABFILE *pItem = (KMAGICATTRIB_TABFILE *)m_BPTMagicAttrib.GetRecord(pMAITItem->Get(i));
                        char szTemp[1024];
                        sprintf(szTemp, " %3d(%-8s) ", pMAITItem->Get(i) + 2, pItem->m_szName);

                        strcat(szStringContent, szTemp);
                    }

                    sprintf(szOutputString, 
                        "[ħ������]m_CMAIT[%d][%d][%d][%d]: Count = %3d, %s  \n",
                        nPrefixPostfix,
                        nType,
                        nSeries,
                        nLevel - 1,
                        pMAITItem->GetCount(),
                        szStringContent
                    );
                    printf("%s", szOutputString);
                }
            }
        }
    }
    ExitProcess(0); // for redirect to File save

    #endif // _DEBUG


    nResult = true;
//Exit0:
    return nResult;
}


/******************************************************************************
	���ܣ�	����ԭʼ��ħ�����Ա���ͳ�Ƴ�ÿ��װ��������Щ���ܵ�ħ������
	���:	m_BPTMagicAttrib: �ں���tab file�����ȫ��ħ������
	����:	ԭʼ��ħ�����Ա�������, ���������ݴ���m_CMAT������
			m_CMAT[i]�и����������ڵ�i��װ����ȫ��ħ������
******************************************************************************/
BOOL KLibOfBPT::InitMALib()
{
	BOOL bEC = FALSE;

	// ȷ��ÿ��װ�����ж����ֿ��ܵ�ħ������
    int naryMACount[2][MATF_CBDR];	// ��i��װ������naryMACount[0][i]�ֿ��ܵ�ǰ׺
									//			  ��naryMACount[1][i]�ֿ��ܵĺ�׺
	m_BPTMagicAttrib.GetMACount((int*)naryMACount);

	// ����ȡ�ص���ֵ��Ϊ��װ����ħ��������������������
	for (int i = 0; i < MATF_CBDR - 1; i++)	// -1 because of horse
	{
		_ASSERT(naryMACount[0][i] > 0);	// ������û��һ��ħ��ǰ׺�����ڸ�װ��
		_ASSERT(naryMACount[1][i] > 0);	// ������û��һ��ħ����׺�����ڸ�װ��
		if (FALSE == m_CMAT[0][i].GetMemory(naryMACount[0][i]))
			return bEC;
		if (FALSE == m_CMAT[1][i].GetMemory(naryMACount[1][i]))
			return bEC;
	}

	// ����ħ�����Ա����������װ����ħ������������
	const int nNumOfMA = m_BPTMagicAttrib.NumOfEntries();	// ħ�����Ե�����
	_ASSERT(nNumOfMA > 0);
	int m, n;		// ��m��ħ�����������ڵ�n��װ��
	for (m = 0; m < nNumOfMA; m++)				// ��������ħ������
	{
		const KMAGICATTRIB_TABFILE* pRec;
		pRec = m_BPTMagicAttrib.GetRecord(m);		
		for (n = 0; n < MATF_CBDR; n++)			// ����MATF_CBDR��װ����ħ��
		{	// ȷ��ħ��������������Щװ��
			if (0 != pRec->m_DropRate[n])
			{	// ��������, ˵����m��ħ�����������ڵ�n��װ��
				int nPos;
				nPos = (0 == pRec->m_nPos) ? 1 : 0;
				m_CMAT[nPos][n].Set(m);
			}
		}
	}
	bEC = TRUE;
	return bEC;
}

const KBASICPROP_EQUIPMENT_GOLD* KLibOfBPT::GetGoldEquipRecord(IN int nIndex) const
{
	return (KBASICPROP_EQUIPMENT_GOLD*)m_BPTGoldEquip.GetRecord(nIndex);
}

const int KLibOfBPT::GetGoldEquipNumber() const
{
	return m_BPTGoldEquip.GetRecordCount();
}

const KBASICPROP_EQUIPMENT_PLATINA* KLibOfBPT::GetPlatinaEquipRecord(IN int nIndex) const
{
	return (KBASICPROP_EQUIPMENT_PLATINA*)m_BPTPlatinaEquip.GetRecord(nIndex);
}

const int KLibOfBPT::GetPlatinaEquipNumber() const
{
	return m_BPTPlatinaEquip.GetRecordCount();
}
/******************************************************************************
	���ܣ�	��ȡָ����CMAT
******************************************************************************/
const KBPT_ClassifiedMAT* KLibOfBPT::GetCMAT(int nPos, int i) const
{
	_ASSERT(this != NULL);

	if (nPos != 0 && nPos != 1)
		{ _ASSERT(FALSE); return NULL; }
	if (i < 0 || i >= MATF_CBDR)
		{ _ASSERT(FALSE); return NULL; }
	return &(m_CMAT[nPos][i]);
}

/******************************************************************************
	���ܣ�	��ȡָ����CMAIT
******************************************************************************/
// Add by Freeway Chen in 2003.5.30
const KBPT_ClassMAIT*       KLibOfBPT::GetCMIT(IN int nPrefixPostfix, IN int nType, IN int nSeries, int nLevel) const
{
    _ASSERT((nPrefixPostfix >= 0) && (nPrefixPostfix < MATF_PREFIXPOSFIX));
    if (!((nPrefixPostfix >= 0) && (nPrefixPostfix < MATF_PREFIXPOSFIX)))
        return NULL;

    _ASSERT((nType >= 0) && (nType < MATF_CBDR));
    if (!((nType >= 0) && (nType < MATF_CBDR)))
        return NULL;

    _ASSERT((nSeries >= 0) && (nSeries < MATF_SERIES));
    if (!((nSeries >= 0) && (nSeries < MATF_SERIES)))
        return NULL;

    _ASSERT(((nLevel - 1) >= 0) && ((nLevel - 1) < MATF_LEVEL));    // nLevel is from 1..MATF_LEVEL
    if (!(((nLevel - 1) >= 0) && ((nLevel - 1) < MATF_LEVEL)))
        return NULL;

    return &m_CMAIT[nPrefixPostfix][nType][nSeries][nLevel - 1];
}

const KMAGICATTRIB_TABFILE* KLibOfBPT::GetMARecord(IN int i) const
{
	return m_BPTMagicAttrib.GetRecord(i);
}

const int KLibOfBPT::GetMARecordNumber() const
{
	return m_BPTMagicAttrib.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetMeleeWeaponRecord(IN int i) const
{
	return m_BPTMeleeWeapon.GetRecord(i);
}

const int KLibOfBPT::GetMeleeWeaponRecordNumber() const
{
	return m_BPTMeleeWeapon.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetRangeWeaponRecord(IN int i) const
{
	return m_BPTRangeWeapon.GetRecord(i);
}

const int KLibOfBPT::GetRangeWeaponRecordNumber() const
{
	return m_BPTRangeWeapon.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetArmorRecord(IN int i) const
{
	return m_BPTArmor.GetRecord(i);
}

const int KLibOfBPT::GetArmorRecordNumber() const
{
	return m_BPTArmor.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetHelmRecord(IN int i) const
{
	return m_BPTHelm.GetRecord(i);
}

const int KLibOfBPT::GetHelmRecordNumber() const
{
	return m_BPTHelm.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetBootRecord(IN int i) const
{
	return m_BPTBoot.GetRecord(i);
}

const int KLibOfBPT::GetBootRecordNumber() const
{
	return m_BPTBoot.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetBeltRecord(IN int i) const
{
	return m_BPTBelt.GetRecord(i);
}

const int KLibOfBPT::GetBeltRecordNumber() const
{
	return m_BPTBelt.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetAmuletRecord(IN int i) const
{
	return m_BPTAmulet.GetRecord(i);
}

const int KLibOfBPT::GetAmuletRecordNumber() const
{
	return m_BPTAmulet.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetRingRecord(IN int i) const
{
	return m_BPTRing.GetRecord(i);
}

const int KLibOfBPT::GetRingRecordNumber() const
{
	return m_BPTRing.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetCuffRecord(IN int i) const
{
	return m_BPTCuff.GetRecord(i);
}

const int KLibOfBPT::GetCuffRecordNumber() const
{
	return m_BPTCuff.NumOfEntries();
}

const KBASICPROP_EQUIPMENT*	KLibOfBPT::GetPendantRecord(IN int i) const
{
	return m_BPTPendant.GetRecord(i);
}

const int KLibOfBPT::GetPendantRecordNumber() const
{
	return m_BPTPendant.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetHorseRecord(IN int i) const
{
	return m_BPTHorse.GetRecord(i);
}

const int KLibOfBPT::GetHorseRecordNumber() const
{
	return m_BPTHorse.NumOfEntries();
}

const KBASICPROP_MEDICINE* KLibOfBPT::GetMedicineRecord(IN int i) const
{
	return m_BPTMedicine.GetRecord(i);
}

const int KLibOfBPT::GetMedicineRecordNumber() const
{
	return m_BPTMedicine.NumOfEntries();
}

const KBASICPROP_QUEST* KLibOfBPT::GetQuestRecord(IN int i) const
{
	return m_BPTQuest.GetRecord(i);
}

const int KLibOfBPT::GetQuestRecordNumber() const
{
	return m_BPTQuest.NumOfEntries();
}

const KBASICPROP_TOWNPORTAL* KLibOfBPT::GetTownPortalRecord(IN int i) const
{
	return m_BPTTownPortal.GetRecord(i);
}

const int KLibOfBPT::GetTownPortalRecordNumber() const
{
	return m_BPTTownPortal.NumOfEntries();
}

const KBASICPROP_MAGICSCRIPT* KLibOfBPT::GetMagicScript(IN int i) const
{
	return m_BPTMagicScript.GetRecord(i);
}

const int KLibOfBPT::GetMagicScriptRecordNumber() const
{
	return m_BPTMagicScript.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetMaskRecord(IN int i) const
{
	return m_BPTMask.GetRecord(i);
}

const int KLibOfBPT::GetMaskRecordNumber() const
{
	return m_BPTMask.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetMantleRecord(IN int i) const
{
	return m_BPTMantle.GetRecord(i);
}

const int KLibOfBPT::GetMantleRecordNumber() const
{
	return m_BPTMantle.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetSignetRecord(IN int i) const
{
	return m_BPTSignet.GetRecord(i);
}

const int KLibOfBPT::GetSignetRecordNumber() const
{
	return m_BPTSignet.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetShipinRecord(IN int i) const
{
	return m_BPTShipin.GetRecord(i);
}

const int KLibOfBPT::GetShipinRecordNumber() const
{
	return m_BPTShipin.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetHoodsRecord(IN int i) const
{
	return m_BPTHoods.GetRecord(i);
}

const int KLibOfBPT::GetHoodsRecordNumber() const
{
	return m_BPTHoods.NumOfEntries();
}

const KBASICPROP_EQUIPMENT* KLibOfBPT::GetCloakRecord(IN int i) const
{
	return m_BPTCloak.GetRecord(i);
}

const int KLibOfBPT::GetCloakRecordNumber() const
{
	return m_BPTCloak.NumOfEntries();
}
//
/*
const KBASICPROP_EQUIPMENT* KLibOfBPT::FindEquipment(IN int a, IN int b, IN int c) const
{
	return m_BPTEquipment.FindRecord(a, b, c);
}
*/

const KBASICPROP_EQUIPMENT_PLATINA* KLibOfBPT::FindEquipmentUnique(IN int a, IN int b, IN int c) const
{
	return 0;// TODO:m_BPTEquipment_Unique.FindRecord(a, b, c);
}
/******************************************************************************
	����:	��ȡָ����ҩƷ�����Լ�¼
	���:	nType: ҩƷ������
			nLevel: ҩƷ�ļ���
	����:	�ɹ�ʱ����ָ��ü�¼��ָ��
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_MEDICINE* KLibOfBPT::FindMedicine(IN int nType, IN int nLevel) const
{
	return 0;//TODO: m_BPTMedicine.FindRecord(nType, nLevel);
}

/******************************************************************************
	����:	��ȡָ���Ŀ�ʯ�����Լ�¼
	���:	i: ָ����ʯ
	����:	�ɹ�ʱ����ָ��ü�¼��ָ��
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EVENTITEM* KLibOfBPT::GetEvent(IN int i) const
{
	return m_BPTEvent.GetRecord(i);
}

const int KLibOfBPT::GetEventRecordNumber() const
{
	return m_BPTEvent.NumOfEntries();
}
//=============================================================================

KBasicPropertyTable::KBasicPropertyTable()
{
	m_pBuf = NULL;
	m_nNumOfEntries = 0;
	m_nSizeOfEntry = 0;
	m_szTabFile[0] = 0;
}

KBasicPropertyTable::~KBasicPropertyTable()
{
	ReleaseMemory();
}

/******************************************************************************
	����:	��¼tab file�й��ж��������ݼ�¼
******************************************************************************/
void KBasicPropertyTable::SetCount(int cbCount)
{
	_ASSERT(cbCount>0);
	_ASSERT(0==m_nNumOfEntries);	// this function is supposed to be called only once
	m_nNumOfEntries = cbCount;
}

/******************************************************************************
	����:	�����ڴ�,���ڱ����tab file�ж��������
	���:	m_nNumOfEntries: ������ô�������ݼ�¼
			m_nSizeOfEntry: ÿ�����ݼ�¼�Ĵ�С(�ֽ�)
	����:	�ɹ�ʱ���ط���, m_pBuf ָ�������ڴ�
			ʧ��ʱ������
******************************************************************************/
BOOL KBasicPropertyTable::GetMemory()
{
	_ASSERT(NULL == m_pBuf);
	_ASSERT(m_nNumOfEntries > 0 && m_nSizeOfEntry > 0);

	BOOL bEC = FALSE;
	const int nMemSize = m_nSizeOfEntry * m_nNumOfEntries;
	void* pBuf = new BYTE[nMemSize];
	_ASSERT(pBuf != NULL);
	if (pBuf != NULL)
	{
		m_pBuf = pBuf;
		bEC = TRUE;
	}
	return bEC;
}

/******************************************************************************
	����:	�ͷ��ڴ�
******************************************************************************/
void KBasicPropertyTable::ReleaseMemory()
{
	if (m_pBuf)
	{
		delete []m_pBuf;
		m_pBuf = NULL;
		m_nNumOfEntries = 0;
	}
}

/******************************************************************************
	����:	����tab file�е�ȫ������
	���:	m_szTabFile: �ļ���
	����:	�ɹ�ʱ���ط���, ȫ�����ݶ���m_pBuf��ָ��������.
				m_nNumOfEntries �������������������
			ʧ��ʱ������
******************************************************************************/
BOOL KBasicPropertyTable::Load()
{
	BOOL bEC = FALSE;
	KTabFile	theLoader;

	// ����tab file
	g_SetRootPath(NULL);

//	::g_SetFilePath(TABFILE_PATH);
	char	szFileName[FILE_NAME_LENGTH];
	g_UnitePathAndName(TABFILE_PATH, m_szTabFile, szFileName);
	if (FALSE == theLoader.Load(szFileName))
		{ _ASSERT(FALSE); return bEC; }

	// ȷ��file�ڸ����˶������¼
	const int cbItems = theLoader.GetHeight() - 1;	// ��һ�и�����������,
	if (cbItems <= 0)							// ʵ�����ݴӵ�2�п�ʼ����.
		{ _ASSERT(FALSE); return bEC; }
	SetCount(cbItems);

	// �����ڴ棬�������Ա�
	if (FALSE == GetMemory())
		{ _ASSERT(FALSE); return bEC; }

	// �����Լ�¼��������
	int i = 0;
	for (i = 0; i < cbItems; i++)
	{
		if (FALSE == LoadRecord(i, &theLoader))
			{ _ASSERT(FALSE); return bEC; }
	}

	bEC = TRUE;
	return bEC;
}

//============================================================================
KBPT_Event::KBPT_Event()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_EVENTITEM);
	::strcpy(m_szTabFile, TABFILE_EVENTITEM);
}

KBPT_Event::~KBPT_Event()
{
}

BOOL KBPT_Event::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_EVENTITEM* pBuf = (KBASICPROP_EVENTITEM*)m_pBuf;
	pBuf = pBuf + i;
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_CHAR,			pBuf->m_szScript, sizeof(pBuf->m_szScript)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bShortKey), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nMaxStack), 0},
	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

/******************************************************************************
	����:	��ȡָ���Ŀ�ʯ������
	���:	i: Ҫ���ȡ��i���ʯ������. ��Ϊ-1��ʾ���ȡһ��
	����:	�ɹ�ʱ����ָ��ÿ�ʯ���Ե�ָ��(һ��KBASICPROP_EVENT�ṹ)
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EVENTITEM* KBPT_Event::GetRecord(int i) const
{
	_ASSERT(this != NULL);

	if (-1 == i)
		i = ::GetRandomNumber(0, m_nNumOfEntries-1);

	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_EVENTITEM*)m_pBuf) + i) : NULL;
}


const KBASICPROP_EVENTITEM* KBPT_Event::FindRecord(IN int nDetailType) const
{
	const KBASICPROP_EVENTITEM* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_EVENTITEM* pMed;
		pMed = GetRecord(i);
		_ASSERT(NULL != pMed);
		if (nDetailType == pMed->m_nDetailType)
		{
			pData = pMed;
			break;
		}
	}
	return pData;
}
//============================================================================
KBPT_MagicScript::KBPT_MagicScript()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_MAGICSCRIPT);
	::strcpy(m_szTabFile, TABFILE_MAGICSCRIPT);
}

KBPT_MagicScript::~KBPT_MagicScript()
{
}

BOOL KBPT_MagicScript::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_MAGICSCRIPT* pBuf = (KBASICPROP_MAGICSCRIPT*)m_pBuf;
	pBuf = pBuf + i;			// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_CHAR,			pBuf->m_szScript, sizeof(pBuf->m_szScript)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bShortKey), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nMaxStack), 0},
	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_MAGICSCRIPT* KBPT_MagicScript::GetRecord(IN int i) const
{
	_ASSERT(this != NULL);

	if (-1 == i)
		i = ::GetRandomNumber(0, m_nNumOfEntries-1);

	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_MAGICSCRIPT*)m_pBuf) + i) : NULL;
}


const KBASICPROP_MAGICSCRIPT* KBPT_MagicScript::FindRecord(IN int nType) const
{
	const KBASICPROP_MAGICSCRIPT* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_MAGICSCRIPT* pMed;
		pMed = GetRecord(i);
		_ASSERT(NULL != pMed);
		if (nType == pMed->m_nDetailType)
		{
			pData = pMed;
			break;
		}
	}
	return pData;
}
//============================================================================
KBPT_TownPortal::KBPT_TownPortal()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_TOWNPORTAL);
	::strcpy(m_szTabFile, TABFILE_TOWNPORTAL);
}

KBPT_TownPortal::~KBPT_TownPortal()
{
}

BOOL KBPT_TownPortal::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_TOWNPORTAL* pBuf = (KBASICPROP_TOWNPORTAL*)m_pBuf;
	pBuf = pBuf + i;			// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_CHAR,			pBuf->m_szScript, sizeof(pBuf->m_szScript)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bShortKey), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nMaxStack), 0},
	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

const KBASICPROP_TOWNPORTAL* KBPT_TownPortal::GetRecord(IN int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_TOWNPORTAL*)m_pBuf) + i) : NULL;
}

//============================================================================
KBPT_Quest::KBPT_Quest()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_QUEST);
	::strcpy(m_szTabFile, TABFILE_TASK);
}

KBPT_Quest::~KBPT_Quest()
{
}

BOOL KBPT_Quest::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	KBASICPROP_QUEST* pBuf = (KBASICPROP_QUEST*)m_pBuf;
	pBuf = pBuf + i;			// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_bShortKey), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nMaxStack), 0},
	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));

}

const KBASICPROP_QUEST* KBPT_Quest::GetRecord(IN int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_QUEST*)m_pBuf) + i) : NULL;

}

const KBASICPROP_QUEST* KBPT_Quest::FindRecord(IN int nDetailType) const
{
	const KBASICPROP_QUEST* pData = NULL;
	// ����ʹ��˳����ҵ��㷨. ��ԭʼ�����ǰ�m_nDetailType�����,
	// ��ɽ����㷨�Ż�
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_QUEST* pQst;
		pQst = GetRecord(i);
		_ASSERT(NULL != pQst);
		if (nDetailType == pQst->m_nDetailType)
		{
			// ���ݲ߻������, ����1���� == �������. ������֤
//			_ASSERT(nType == pMed->m_nAttrib1_Type);
			pData = pQst;
			break;
		}
	}
	return pData;
}

//============================================================================

KBPT_Medicine::KBPT_Medicine()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_MEDICINE);
	::strcpy(m_szTabFile, TABFILE_MEDICINE);
}

KBPT_Medicine::~KBPT_Medicine()
{
}

BOOL KBPT_Medicine::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	// ��ʼ��
	KBASICPROP_MEDICINE* pBuf = (KBASICPROP_MEDICINE*)m_pBuf;
	pBuf = pBuf + i;	// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nMaxStack), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[0].nAttrib), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[0].nValue), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[0].nTime), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[1].nAttrib), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[1].nValue), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryAttrib[1].nTime), 0},
	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

/******************************************************************************
	����:	��ȡָ����ҩƷ������
	���:	i: Ҫ���ȡ��i��ҩƷ������
	����:	�ɹ�ʱ����ָ���ҩ�����Ե�ָ��(һ��KBASICPROP_MEDICINE�ṹ)
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_MEDICINE* KBPT_Medicine::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_MEDICINE*)m_pBuf) + i) : NULL;
}

const KBASICPROP_MEDICINE* KBPT_Medicine::FindRecord(IN int nType,
													 IN int nLevel) const
{
	const KBASICPROP_MEDICINE* pData = NULL;
	// ����ʹ��˳����ҵ��㷨. ��ԭʼ�����ǰ�m_nDetailType��m_nLevel�����,
	// ��ɽ����㷨�Ż�
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_MEDICINE* pMed;
		pMed = GetRecord(i);
		_ASSERT(NULL != pMed);
		if (nType == pMed->m_nDetailType && nLevel == pMed->m_nLevel)
		{
			// ���ݲ߻������, ����1���� == �������. ������֤
//			_ASSERT(nType == pMed->m_nAttrib1_Type);
			pData = pMed;
			break;
		}
	}
	return pData;
}
//======================================================

KBPT_Equipment::KBPT_Equipment()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_EQUIPMENT);
}

KBPT_Equipment::~KBPT_Equipment()
{
}

void KBPT_Equipment::Init(IN int i)
{
	::strcpy(m_szTabFile, TABFILE_EQUIPMENT[i]);
}

BOOL KBPT_Equipment::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	// ��ʼ��
	KBASICPROP_EQUIPMENT* pBuf = (KBASICPROP_EQUIPMENT*)m_pBuf;
	pBuf = pBuf + i;	// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nPara), 0},

	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

/******************************************************************************
	����:	��ȡָ����װ��������
	���:	i: Ҫ���ȡ��i��װ��������
	����:	�ɹ�ʱ����ָ���װ�����Ե�ָ��(һ��KBASICPROP_EQUIPMENT�ṹ)
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EQUIPMENT* KBPT_Equipment::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_EQUIPMENT*)m_pBuf) + i) : NULL;
}

/******************************************************************************
	����:	��ȡָ����װ�����Լ�¼
	���:	nDetailType: �������
			nParticularType: ��ϸ���
			nLevel: �ȼ�
	����:	�ɹ�ʱ����ָ��ü�¼��ָ��
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EQUIPMENT* KBPT_Equipment::FindRecord( IN int nDetailType,
														IN int nParticularType,
														IN int nLevel) const
{
	_ASSERT(this != NULL);
	
	const KBASICPROP_EQUIPMENT* pData = NULL;
	// ����ʹ��˳����ҵ��㷨. ��ԭʼ�����ǰ�m_nDetailType��m_nLevel�����,
	// ��ɽ����㷨�Ż�
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_EQUIPMENT* pEqu;
		pEqu = GetRecord(i);
		_ASSERT(NULL != pEqu);
		if (nDetailType == pEqu->m_nDetailType &&
			nParticularType == pEqu->m_nParticularType &&
			nLevel == pEqu->m_nLevel)
		{
			pData = pEqu;
			break;
		}
	}
	return pData;
}

//============================================================================
KBPT_Equipment_Gold::KBPT_Equipment_Gold()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_EQUIPMENT_GOLD);
	// Copy the gold item information file's name into the buffer
}

KBPT_Equipment_Gold::~KBPT_Equipment_Gold()
{
	return;
}

BOOL KBPT_Equipment_Gold::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	// ��ʼ��
	KBASICPROP_EQUIPMENT_GOLD* pBuf = (KBASICPROP_EQUIPMENT_GOLD*)m_pBuf;
	pBuf = pBuf + i;	// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},	
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMax), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nPara), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[3]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[4]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[5]), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nGroup), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nNeedToActive1), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nNeedToActive2), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSetID), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[6]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs[7]), 0},
	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

/******************************************************************************
	����:	��ȡָ����װ��������
	���:	i: Ҫ���ȡ��i��װ��������
	����:	�ɹ�ʱ����ָ���װ�����Ե�ָ��(һ��KBASICPROP_EQUIPMENT�ṹ)
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EQUIPMENT_GOLD* KBPT_Equipment_Gold::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_EQUIPMENT_GOLD*)m_pBuf) + i) : NULL;
}

/******************************************************************************
	����:	��ȡָ����Ψһװ�����Լ�¼
	���:	nDetailType: �������
			nParticularType: ��ϸ���
			nLevel: �ȼ�
	����:	�ɹ�ʱ����ָ��ü�¼��ָ��
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EQUIPMENT_GOLD*
		KBPT_Equipment_Gold::FindRecord(IN int nDetailType,
										  IN int nParticularType,
										  IN int nLevel) const
{
	_ASSERT(this != NULL);
	
	const KBASICPROP_EQUIPMENT_GOLD* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_EQUIPMENT_GOLD* pEqu;
		pEqu = GetRecord(i);
		_ASSERT(NULL != pEqu);
		if (NULL == pEqu)
			continue;
		if (nDetailType == pEqu->m_nDetailType &&
			nParticularType == pEqu->m_nParticularType &&
			nLevel == pEqu->m_nLevel)
		{
			pData = pEqu;
			break;
		}
	}
	return pData;
}

void KBPT_Equipment_Gold::Init()
{
	::strcpy(m_szTabFile, TABFILE_GOLDEQUIP);
}
//============================================================================
KBPT_Equipment_Platina::KBPT_Equipment_Platina()
{
	m_nSizeOfEntry = sizeof(KBASICPROP_EQUIPMENT_PLATINA);
}

KBPT_Equipment_Platina::~KBPT_Equipment_Platina()
{
	return;
}

BOOL KBPT_Equipment_Platina::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	// ��ʼ��
	KBASICPROP_EQUIPMENT_PLATINA* pBuf = (KBASICPROP_EQUIPMENT_PLATINA*)m_pBuf;
	pBuf = pBuf + i;	// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nItemGenre), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nDetailType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nParticularType), 0},	
		{ PI_VARTYPE_CHAR,			pBuf->m_szImageName, sizeof(pBuf->m_szImageName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nObjIdx), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nWidth), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nHeight), 0},
		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSeries), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPrice), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[0].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[1].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[2].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[3].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[4].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[5].sRange.nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropBasic[6].sRange.nMax), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[0].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[1].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[2].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[3].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[4].nPara), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nType), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryPropReq[5].nPara), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_10[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_10[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_10[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[3]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_10[3]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[4]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_10[4]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[5]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_10[5]), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nGroup), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nNeedToActive1), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nNeedToActive2), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nSetID), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[6]), 0},
		//pBuf->m_aryMagicAttribs_10[6] = pBuf->m_aryMagicAttribs_10[6];
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_aryMagicAttribs_0[7]), 0},
		//pBuf->m_aryMagicAttribs_10[7] = pBuf->m_aryMagicAttribs_10[7];
	};

	// ��������������
	return ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0]));
}

/******************************************************************************
	����:	��ȡָ����װ��������
	���:	i: Ҫ���ȡ��i��װ��������
	����:	�ɹ�ʱ����ָ���װ�����Ե�ָ��(һ��KBASICPROP_EQUIPMENT�ṹ)
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EQUIPMENT_PLATINA* KBPT_Equipment_Platina::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KBASICPROP_EQUIPMENT_PLATINA*)m_pBuf) + i) : NULL;
}

/******************************************************************************
	����:	��ȡָ����Ψһװ�����Լ�¼
	���:	nDetailType: �������
			nParticularType: ��ϸ���
			nLevel: �ȼ�
	����:	�ɹ�ʱ����ָ��ü�¼��ָ��
			ʧ��ʱ����NULL
******************************************************************************/
const KBASICPROP_EQUIPMENT_PLATINA*
		KBPT_Equipment_Platina::FindRecord(IN int nDetailType,
										  IN int nParticularType,
										  IN int nLevel) const
{
	_ASSERT(this != NULL);
	
	const KBASICPROP_EQUIPMENT_PLATINA* pData = NULL;
	for (int i = 0; i < m_nNumOfEntries; i++)
	{
		const KBASICPROP_EQUIPMENT_PLATINA* pEqu;
		pEqu = GetRecord(i);
		_ASSERT(NULL != pEqu);
		if (NULL == pEqu)
			continue;
		if (nDetailType == pEqu->m_nDetailType &&
			nParticularType == pEqu->m_nParticularType &&
			nLevel == pEqu->m_nLevel)
		{
			pData = pEqu;
			break;
		}
	}
	return pData;
}

void KBPT_Equipment_Platina::Init()
{
	::strcpy(m_szTabFile, TABFILE_PLATINAEQUIP);
}

//============================================================================

KBPT_MagicAttrib_TF::KBPT_MagicAttrib_TF()
{
	m_nSizeOfEntry = sizeof(KMAGICATTRIB_TABFILE);
	::strcpy(m_szTabFile, TABFILE_MAGICATTRIB);
	::memset(&m_naryMACount, 0, sizeof(m_naryMACount));
}

KBPT_MagicAttrib_TF::~KBPT_MagicAttrib_TF()
{
}

BOOL KBPT_MagicAttrib_TF::LoadRecord(int i, KTabFile* pTF)
{
	_ASSERT(pTF != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);

	// ��ʼ��
	KMAGICATTRIB_TABFILE* pBuf = (KMAGICATTRIB_TABFILE*)m_pBuf;
	pBuf = pBuf + i;	// ��������Լ��� pBuf ��ָ�ṹ��
	const PROPINFO	aryPI[] =
	{
		{ PI_VARTYPE_CHAR,			pBuf->m_szName, sizeof(pBuf->m_szName)},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nPos), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nClass), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_nLevel), 0},

		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.nPropKind), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[0].nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[0].nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[1].nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[1].nMax), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[2].nMin), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_MagicAttrib.aryRange[2].nMax), 0},

		{ PI_VARTYPE_CHAR,			pBuf->m_szIntro, sizeof(pBuf->m_szIntro)},
		
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[0]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[1]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[2]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[3]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[4]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[5]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[6]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[7]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[8]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[9]), 0},
		{ PI_VARTYPE_INT,  (char*)&(pBuf->m_DropRate[10]), 0}
	};
	
	// ��������������
	if (FALSE == ::LoadRecord(pTF, i, aryPI, sizeof(aryPI)/ sizeof(aryPI[0])))
		return FALSE;

    pBuf->m_nUseFlag = false;       // ��ʼ��Ϊû��ʹ��

	// ����ͳ�ƣ�ÿ��װ�������õ�ħ����Ŀ
	for (int n = 0; n < MATF_CBDR; n++)
	{
		if (0 != pBuf->m_DropRate[n])
		{
			int nPos;
			nPos = (0 == pBuf->m_nPos) ? 1 : 0;
			m_naryMACount[nPos][n]++;
		}
	}
	return TRUE;
}

/******************************************************************************
	����:	��ȡָ����ħ������
	���:	i: Ҫ���ȡ��i��ħ������
	����:	�ɹ�ʱ����ָ���ħ�����Ե�ָ��(һ��KMAGICATTRIB_TABFILE�ṹ)
			ʧ��ʱ����NULL
******************************************************************************/
const KMAGICATTRIB_TABFILE* KBPT_MagicAttrib_TF::GetRecord(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfEntries);
	return (i >= 0 && i < m_nNumOfEntries) ?
		(((KMAGICATTRIB_TABFILE*)m_pBuf) + i) : NULL;
}

void KBPT_MagicAttrib_TF::GetMACount(int* pnCount) const
{
	_ASSERT(this != NULL);
	_ASSERT(pnCount != NULL);
	::memcpy(pnCount, m_naryMACount, sizeof(m_naryMACount));
}

//============================================================================
// Magic Item Index Table Class
KBPT_ClassMAIT::KBPT_ClassMAIT()
{
	m_pnTable = NULL;
	m_nSize = 0;
	m_nNumOfValidData = 0;
}

KBPT_ClassMAIT::~KBPT_ClassMAIT()
{
    m_nSize = 0;
	m_nNumOfValidData = 0;

	if (m_pnTable)
    {
        delete []m_pnTable;
        m_pnTable = NULL;
    }
}

/******************************************************************************
	����:	����������գ������ͷ��ڴ�
	���:	
	����:	�ɹ����ط��㣬ʧ�ܷ�����
	˵��:	
******************************************************************************/
BOOL KBPT_ClassMAIT::Clear()
{
    m_nNumOfValidData = 0; 
    
    return true;   
}


/******************************************************************************
	����:	����������ȡֵ
	���:	i: ���������и�λ��ȡֵ
	����:	������ȡ��ֵ
	˵��:	������ֵΪnItemIndex, �����KLibOfBPT::m_BPTMagicAttrib.GetRecord(nItemIndex)
            �ɻ�ö�Ӧ��ħ������
******************************************************************************/
int KBPT_ClassMAIT::Get(int i) const
{
	_ASSERT(this != NULL);
	_ASSERT(i >= 0 && i < m_nNumOfValidData);
    _ASSERT(m_pnTable);
	return m_pnTable[i];
}


/******************************************************************************
	����:	������������һ���µ�ֵ
	���:	nItemIndex: ����ֵ֮
	����:	�ɹ�ʱ���ط���, m_pnTable[m_nNumOfValidData]Ϊ����ֵ֮
			m_nNumOfValidDataָ����һ��λ
			ʧ��ʱ������
******************************************************************************/
BOOL KBPT_ClassMAIT::Insert(int nItemIndex)
{
    int nResult = false;

	_ASSERT(this != NULL);
	_ASSERT(nItemIndex >= 0);	// nData Ϊ�����±�

    if (!m_pnTable)
    {
        m_pnTable = new int [4];
        if (!m_pnTable)
            goto Exit0;

        m_nNumOfValidData = 0;
        m_nSize = 4;
    }

    if (m_nNumOfValidData >= m_nSize)
    {
        int *pnaryTemp = new int [m_nSize + 8];
        if (!pnaryTemp)
            goto Exit0;

        memcpy(pnaryTemp, m_pnTable, m_nNumOfValidData * sizeof(int));

        m_nSize += 8;
        delete []m_pnTable;
        m_pnTable = pnaryTemp;
        pnaryTemp = NULL;
    }

	m_pnTable[m_nNumOfValidData++] = nItemIndex;

    nResult = true;
Exit0:
    return nResult;
}



//============================================================================

KBPT_ClassifiedMAT::KBPT_ClassifiedMAT()
{
	m_pnTable = NULL;
	m_nSize = 0;
	m_nNumOfValidData = 0;
}

KBPT_ClassifiedMAT::~KBPT_ClassifiedMAT()
{
	ReleaseMemory();
}

/******************************************************************************
	����:	Ϊ�����������ڴ�
	���:	nCount: ��������С(���������Ŀ)
	����:	m_pnTable ָ�������ڴ�
			m_nSize = nCount
******************************************************************************/
BOOL KBPT_ClassifiedMAT::GetMemory(int nCount)
{
	_ASSERT(this != NULL);
	_ASSERT(nCount > 0);
	_ASSERT(NULL == m_pnTable);
	_ASSERT(0 == m_nSize);

	BOOL bEC = FALSE;
	int* pnBuf = new int[nCount];
	if (pnBuf)
	{
		m_pnTable = pnBuf;
		m_nSize = nCount;
		bEC = TRUE;
	}
	return bEC;
}

/******************************************************************************
	����:	�ͷ����������õ��ڴ�
******************************************************************************/
void KBPT_ClassifiedMAT::ReleaseMemory()
{
	if (m_pnTable)
	{
		delete []m_pnTable;
		m_pnTable = NULL;
		m_nSize = 0;
	}
}

/******************************************************************************
	����:	����������ֵ
	���:	nData: ����ֵ֮
	����:	�ɹ�ʱ���ط���, m_pnTable[m_nNumOfValidData]Ϊ����ֵ֮
			m_nNumOfValidDataָ����һ��λ
			ʧ��ʱ������
******************************************************************************/
BOOL KBPT_ClassifiedMAT::Set(int nData)
{
	_ASSERT(this != NULL);
	_ASSERT(nData >= 0);	// nData Ϊ�����±�
	_ASSERT(m_nNumOfValidData >= 0 && m_nNumOfValidData < m_nSize);
	m_pnTable[m_nNumOfValidData++] = nData;
	return TRUE;
}

/******************************************************************************
	����:	����������ȡֵ
	���:	nIndex: ���������и�λ��ȡֵ
	����:	������ȡ��ֵ
	˵��:	������ֵΪi, �����KLibOfBPT::m_BPTMagicAttrib.GetRecord(i)�ɻ��
			��Ӧ��ħ������
******************************************************************************/
int KBPT_ClassifiedMAT::Get(int nIndex) const
{
	_ASSERT(this != NULL);
	_ASSERT(nIndex >= 0 && nIndex < m_nSize);
	return (nIndex >= 0 && nIndex < m_nSize) ? m_pnTable[nIndex] : 0;
}

/******************************************************************************
	����:	�����ݵ����������Ļ�������
	���:	pnaryBuf: ������ָ��
			*pnCount: �����������ɶ��ٸ�int������
	����:	�ɹ�ʱ���ط���, ���ݱ���������������, *pnCount �������ݸ���
			ʧ��ʱ������
******************************************************************************/
BOOL KBPT_ClassifiedMAT::GetAll(int* pnaryBuf, int* pnCount) const
{
	_ASSERT(this != NULL);
	_ASSERT(pnaryBuf != NULL);
	_ASSERT(pnCount != NULL);

	BOOL bEC = FALSE;
	if (pnaryBuf && pnCount && *pnCount > 0)
	{
		int nLength = *pnCount;
		if(m_nSize< nLength) nLength = m_nSize;
		::memcpy(pnaryBuf, m_pnTable, nLength*sizeof(int));
		*pnCount = nLength;
		bEC = TRUE;
	}
	return bEC;
}