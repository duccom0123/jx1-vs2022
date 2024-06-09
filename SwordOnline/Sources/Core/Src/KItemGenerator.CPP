//---------------------------------------------------------------------------
// Sword3 Core (c) 2002 by Kingsoft
//
// File:	KItemGenerator.CPP
// Date:	2002.08.26
// Code:	DongBo
// Desc:    CPP file. ���ļ�ʵ�ֵ����������ɵ���
//---------------------------------------------------------------------------
#include "KEngine.h"
#include "KCore.h"
#include "MyAssert.H"
#include "KItem.h"
#include "KSubWorldSet.h"
#include "KItemGenerator.h"

KItemGenerator	ItemGen;			//	װ��������

KItemGenerator::KItemGenerator()
{
}

KItemGenerator::~KItemGenerator()
{
}

/******************************************************************************
	���ܣ�	���ݳ�ʼ��. ��tab file�ж�ȡ����
******************************************************************************/
BOOL KItemGenerator::Init()
{
	if (!m_BPTLib.Init())		// �˵��ô����ɵ�tab file���������еĳ�ʼ����
		return FALSE;

	for (int i = equip_meleeweapon; i < equip_detailnum - equip_meleeweapon; i++)
	{
		switch(i)
		{
		case equip_meleeweapon:
			m_EquipNumOfEntries[i] = m_BPTLib.GetMeleeWeaponRecordNumber();
			break;
		case equip_rangeweapon:
			m_EquipNumOfEntries[i] = m_BPTLib.GetRangeWeaponRecordNumber();
			break;
		case equip_armor:
			m_EquipNumOfEntries[i] = m_BPTLib.GetArmorRecordNumber();
			break;
		case equip_helm:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHelmRecordNumber();
			break;
		case equip_boots:
			m_EquipNumOfEntries[i] = m_BPTLib.GetBootRecordNumber();
			break;
		case equip_belt:
			m_EquipNumOfEntries[i] = m_BPTLib.GetBeltRecordNumber();
			break;
		case equip_cuff:
			m_EquipNumOfEntries[i] = m_BPTLib.GetCuffRecordNumber();
			break;
		case equip_amulet:
			m_EquipNumOfEntries[i] = m_BPTLib.GetAmuletRecordNumber();
			break;
		case equip_ring:
			m_EquipNumOfEntries[i] = m_BPTLib.GetRingRecordNumber();
			break;
		case equip_pendant:
			m_EquipNumOfEntries[i] = m_BPTLib.GetPendantRecordNumber();
			break;
		case equip_horse:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHorseRecordNumber();
			break;
		case equip_mask:
			m_EquipNumOfEntries[i] = m_BPTLib.GetMaskRecordNumber();
			break;
		case equip_mantle:
			m_EquipNumOfEntries[i] = m_BPTLib.GetMantleRecordNumber();
			break;
		case equip_signet:
			m_EquipNumOfEntries[i] = m_BPTLib.GetSignetRecordNumber();
			break;
		case equip_shipin:
			m_EquipNumOfEntries[i] = m_BPTLib.GetShipinRecordNumber();
			break;
		case equip_hoods:
			m_EquipNumOfEntries[i] = m_BPTLib.GetHoodsRecordNumber();
			break;
		case equip_cloak:
			m_EquipNumOfEntries[i] = m_BPTLib.GetCloakRecordNumber();
			break;
		}
	}
	m_MedNumOfEntries = m_BPTLib.GetMedicineRecordNumber();
	return TRUE;
}

/******************************************************************************
	���ܣ�	����ҩƷ
	���:	nDetailType: ҩƷ����
			nLevel: �ȼ�
			nVersion: �汾��
	����:	�ɹ�ʱ���ط���, ��������� pItem ��ָ�����и���
			ʧ��ʱ������
******************************************************************************/
BOOL KItemGenerator::Gen_Medicine(IN int nDetailType,
								  IN int nLevel,
								  IN int nVersion,
								  IN OUT KItem* pItem)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	BOOL bEC = FALSE;
	
#ifdef _SERVER	// �������汾
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	pItem->m_GeneratorParam.nLuck = 0;
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
	int nGameVersion = pItem->m_GeneratorParam.nVersion;
#endif

	const int i = nDetailType * 5 + nLevel - 1;	// ����ʽ�ɲ߻�����
												// �ȼ����ݴ�1��ʼ������Ҫ������
	const KBASICPROP_MEDICINE* pMed = NULL;
	pMed = m_BPTLib.GetMedicineRecord(i);
	if (NULL == pMed)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pMed;
	return TRUE;
}
/******************************************************************************
	���ܣ�	�������װ��
	���:	nDetailType: �������, �����������������......
			nParticularType: ��ϸ���
			nSeriesReq: ��������
			nLevel: �ȼ�
			pnaryMALevel: ħ�����Եȼ�����[6]
			nLucky: ����ֵ
	����:	�ɹ�ʱ���ط���, ��������� pItem ��ָ�����и���
			ʧ��ʱ������
******************************************************************************/
BOOL KItemGenerator::Gen_Equipment(IN int nItemNature,
								   IN int nDetailType,
								   IN int nParticularType,
								   IN int nSeriesReq,
								   IN int nLevel,
								   IN const int* pnaryMALevel,
								   IN int nLucky,
								   IN int nVersion,
								   IN OUT KItem* pItem)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	switch (nItemNature)
	{
	case NATURE_GOLD:
		{
			this->Gen_GoldEquipment(nDetailType, pnaryMALevel, nLucky, pItem);
			pItem->SetRow(nDetailType);
			return TRUE;
		}
		break;
	case NATURE_PLATINA:
		{
			this->Gen_PlatinaEquipment(nDetailType, pnaryMALevel, nLucky, pItem);
			pItem->SetRow(nDetailType);
			pItem->SetLevel(nLevel);
			return TRUE;
		}
		break;
	default:
		{
		BOOL bEC = FALSE;
	#ifdef _SERVER	// �������汾
		pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
		if (pnaryMALevel)
			memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		else
			ZeroMemory(pItem->m_GeneratorParam.nGeneratorLevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
	#else
		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
		if (pnaryMALevel)
			memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
	#endif
		pItem->m_GeneratorParam.nLuck = nLucky;
		int nGameVersion = pItem->m_GeneratorParam.nVersion;

		// ������ڲ���, ȷ��װ���Ļ�������
		int i = 0;
		if (nDetailType > equip_horse)
			i = nParticularType;
		else
			i = nParticularType * 10 + nLevel - 1;// ����ʽ�ɲ߻�����
													// ���SPE 2002��8��31��7:40
													// email: ����������ֵ��
													// �ȼ����ݴ�1��ʼ������Ҫ������
		const KBASICPROP_EQUIPMENT* pEqu = NULL;
		switch(nDetailType)
		{
		case equip_meleeweapon:
			pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
			break;
		case equip_rangeweapon:
			pEqu = m_BPTLib.GetRangeWeaponRecord(i);
			break;
		case equip_armor:
			pEqu = m_BPTLib.GetArmorRecord(i);
			break;
		case equip_helm:
			pEqu = m_BPTLib.GetHelmRecord(i);
			break;
		case equip_boots:
			pEqu = m_BPTLib.GetBootRecord(i);
			break;
		case equip_belt:
			pEqu = m_BPTLib.GetBeltRecord(i);
			break;
		case equip_amulet:
			pEqu = m_BPTLib.GetAmuletRecord(i);
			break;
		case equip_ring:
			pEqu = m_BPTLib.GetRingRecord(i);
			break;
		case equip_cuff:
			pEqu = m_BPTLib.GetCuffRecord(i);
			break;
		case equip_pendant:
			pEqu = m_BPTLib.GetPendantRecord(i);
			break;
		case equip_horse:
			pEqu = m_BPTLib.GetHorseRecord(i);
			break;
		case equip_mask:
			pEqu = m_BPTLib.GetMaskRecord(i);
			break;
		case equip_mantle:
			pEqu = m_BPTLib.GetMantleRecord(i);
			break;
		case equip_signet:
			pEqu = m_BPTLib.GetSignetRecord(i);
			break;
		case equip_shipin:
			pEqu = m_BPTLib.GetShipinRecord(i);
			break;
		case equip_hoods:
			pEqu = m_BPTLib.GetHoodsRecord(i);
			break;
		case equip_cloak:
			pEqu = m_BPTLib.GetCloakRecord(i);
			break;
		default:
			break;
		}
		if (NULL == pEqu)
			{ _ASSERT(FALSE); return bEC; }
			// �������˵�ֱ��ԭ��: ֻ��n��װ��, ������ i ��ֵ��[0,n-1]֮��
			// ���3��: nParticularType ����?
			//			nLevel ����?
			//			ԭʼ��tab file������ m_BPTLib.m_BPTEquipment ��
			//			����������������?
		pItem->SetAttrib_CBR(pEqu);
		if (nSeriesReq < series_metal)
			pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
		else
			pItem->SetSeries(nSeriesReq);

		if (NULL == pnaryMALevel)
			return TRUE;
		
		KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];	
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
			pItem->SetAttrib_MA(sMA);

		pItem->SetNature(nItemNature);
		return bEC;
		}
		break;
	}
}


BOOL KItemGenerator::Gen_GoldEquipment(IN int nIndex, IN const int* pnaryMALevel, IN int nLucky, OUT KItem * pItem)
{
	int i = 0;
	int j = 0;
	int nCount = 0;

	int nLuckMin = (int)LOWORD(nLucky);
	int nLuckMax = (int)HIWORD(nLucky);

	if (nLuckMin > MAX_ITEM_LUCK)
		nLuckMin = MAX_ITEM_LUCK;

	if(nLuckMin > nLuckMax)
		nLuckMin = nLuckMax;

	if (nLuckMax > MAX_ITEM_LUCK)
		nLuckMax = MAX_ITEM_LUCK;

#ifdef _SERVER	// �������汾
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
#endif
	if(pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
	else
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB;i++)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = nLuckMin == nLuckMax ? nLuckMax : GetRandomNumber(nLuckMin, nLuckMax);
			pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
		}
	}
	pItem->m_GeneratorParam.nLuck = nLucky;

	nCount = m_BPTLib.GetGoldEquipNumber();
	const KBASICPROP_EQUIPMENT_GOLD* pTemp = m_BPTLib.GetGoldEquipRecord(nIndex);
	if (nIndex >= nCount)
		return FALSE;
	if (pTemp == NULL)
		return FALSE;
	pItem->SetAttrib_CBR((KBASICPROP_EQUIPMENT_GOLD*)pTemp);

	KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];

	if (pItem->m_GeneratorParam.nGeneratorLevel[MAX_ITEM_MAGICATTRIB])
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++) 
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[i] <= 0)
				break;
			sMA[i].nAttribType = pItem->m_GeneratorParam.nGeneratorLevel[i];
			sMA[i].nValue[0] = LOWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
			sMA[i].nValue[1] = -1;
			sMA[i].nValue[2] = HIWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
		}
	}
	else
	{
		KTabFile MagicTab;
		MagicTab.Load(GOLD_EQUIP_MAGIC_FILE);
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			const int* pSrc;
			KItemNormalAttrib* pDst;
			pSrc = &(pTemp->m_aryMagicAttribs[i]);
			pDst = &(sMA[i]);
			
			int nType,nLow,nHigh;
			MagicTab.GetInteger(*pSrc + 1, 5,0,&nType);
			pDst->nAttribType = nType;
			MagicTab.GetInteger(*pSrc + 1, 6,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1, 7,0,&nHigh);
			pDst->nValue[0] = nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / MAX_ITEM_LUCK);
			MagicTab.GetInteger(*pSrc + 1, 8,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1, 9,0,&nHigh);
			pDst->nValue[1] = nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i]  / MAX_ITEM_LUCK);
			MagicTab.GetInteger(*pSrc + 1, 10,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1, 11,0,&nHigh);
			pDst->nValue[2] = nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / MAX_ITEM_LUCK);
			if (sMA[i].nAttribType <= 0)
			{
				sMA[i].nValue[0] = 0;
				sMA[i].nValue[1] = 0;
				sMA[i].nValue[2] = 0;
			}
		}
		MagicTab.Clear();
	}
    for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
    {
        // ���ʣ�µ���
        sMA[i].nAttribType = 0;
        sMA[i].nValue[0] = 0;
        sMA[i].nValue[1] = 0;
        sMA[i].nValue[2] = 0;
    }
	pItem->SetAttrib_MA(sMA);

	return TRUE;
}

BOOL KItemGenerator::Gen_PlatinaEquipment(IN int nIndex, IN const int* pnaryMALevel, IN int nLucky, OUT KItem * pItem)
{
	int i = 0;
	int j = 0;
	int nCount = 0;

	int nLuckMin = (int)LOWORD(nLucky);
	int nLuckMax = (int)HIWORD(nLucky);

	if (nLuckMin > MAX_ITEM_LUCK)
		nLuckMin = MAX_ITEM_LUCK;

	if(nLuckMin > nLuckMax)
		nLuckMin = nLuckMax;

	if (nLuckMax > MAX_ITEM_LUCK)
		nLuckMax = MAX_ITEM_LUCK;

#ifdef _SERVER	// �������汾
	pItem->m_GeneratorParam.uRandomSeed = g_GetRandomSeed();
	pItem->m_GeneratorParam.nVersion = g_SubWorldSet.GetGameVersion();
#else
	g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
#endif
	if(pnaryMALevel)
		memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
	else
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB;i++)
		{
			pItem->m_GeneratorParam.nGeneratorLevel[i] = nLuckMin == nLuckMax ? nLuckMax : GetRandomNumber(nLuckMin, nLuckMax);
			pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB] = 0;
		}
	}
	pItem->m_GeneratorParam.nLuck = nLucky;

	nCount = m_BPTLib.GetPlatinaEquipNumber();
	const KBASICPROP_EQUIPMENT_PLATINA* pTemp = m_BPTLib.GetPlatinaEquipRecord(nIndex);
	if (nIndex >= nCount)
		return FALSE;
	if (pTemp == NULL)
		return FALSE;
	pItem->SetAttrib_CBR((KBASICPROP_EQUIPMENT_PLATINA*)pTemp);

	KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];

	if (pItem->m_GeneratorParam.nGeneratorLevel[MAX_ITEM_MAGICATTRIB])
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++) 
		{
			if (pItem->m_GeneratorParam.nGeneratorLevel[i] <= 0)
				break;
			sMA[i].nAttribType = pItem->m_GeneratorParam.nGeneratorLevel[i];
			sMA[i].nValue[0] = LOWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
			sMA[i].nValue[1] = -1;
			sMA[i].nValue[2] = HIWORD(pItem->m_GeneratorParam.nGeneratorLevel[i + MAX_ITEM_MAGICATTRIB]);
		}
	}
	else
	{
		KTabFile MagicTab;
		MagicTab.Load(GOLD_EQUIP_MAGIC_FILE);
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
		{
			const int* pSrc;
			KItemNormalAttrib* pDst;
			pSrc = &(pTemp->m_aryMagicAttribs_0[i]);
			pDst = &(sMA[i]);
			
			int nType,nLow,nHigh;
			MagicTab.GetInteger(*pSrc + 1, 5,0,&nType);
			pDst->nAttribType = nType;
			MagicTab.GetInteger(*pSrc + 1, 6,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1, 7,0,&nHigh);
			pDst->nValue[0] = nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / MAX_ITEM_LUCK);
			MagicTab.GetInteger(*pSrc + 1, 8,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1, 9,0,&nHigh);
			pDst->nValue[1] = nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i]  / MAX_ITEM_LUCK);
			MagicTab.GetInteger(*pSrc + 1, 10,0,&nLow);
			MagicTab.GetInteger(*pSrc + 1, 11,0,&nHigh);
			pDst->nValue[2] = nLow + ((nHigh - nLow) *  pItem->m_GeneratorParam.nGeneratorLevel[i] / MAX_ITEM_LUCK);
			if (sMA[i].nAttribType <= 0)
			{
				sMA[i].nValue[0] = 0;
				sMA[i].nValue[1] = 0;
				sMA[i].nValue[2] = 0;
			}
		}
		MagicTab.Clear();
	}
    for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
    {
        // ���ʣ�µ���
        sMA[i].nAttribType = 0;
        sMA[i].nValue[0] = 0;
        sMA[i].nValue[1] = 0;
        sMA[i].nValue[2] = 0;
    }
	pItem->SetAttrib_MA(sMA);

	return TRUE;
}
/******************************************************************************
	���ܣ�	����ħ������
	���:	nType: װ������
			pnaryMALevel: ����ָ��, ����ħ���ĵȼ�Ҫ��, ��6��
			nSeriesReq: ��������
			nLucky: ����ֵ
	����:	�� pnaryMA ��ָ�����з���ħ�������Ժ�������, ��6��
******************************************************************************/
// Add by Freeway Chen in 2003.5.30
BOOL KItemGenerator::Gen_MagicAttrib(
    int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,
	KItemNormalAttrib* pnaryMA, int nGameVersion
)
{
    int nResult = false;

    KBPT_ClassMAIT  SelectedMagicTable;	
	// ��¼ÿһ����¼ѡ�е�ħ�����ԣ���Ҫ��Ϊ�����ʹ�ñ�־���
    KMAGICATTRIB_TABFILE *pMagicAttrTable[MAX_ITEM_MAGICLEVEL] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

    int i = 0;

    _ASSERT(pnaryMALevel);
    _ASSERT(pnaryMA);

	if (pnaryMALevel[MAX_ITEM_MAGICATTRIB])
	{
		for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++) 
		{
			if (pnaryMALevel[i] <= 0)
				break;
			pnaryMA[i].nAttribType = pnaryMALevel[i];
			pnaryMA[i].nValue[0] = LOWORD(pnaryMALevel[i + MAX_ITEM_MAGICATTRIB]);
			pnaryMA[i].nValue[1] = -1;
			pnaryMA[i].nValue[2] = HIWORD(pnaryMALevel[i + MAX_ITEM_MAGICATTRIB]);
		}
	}
	else
	{
		for (i = 0; i < MAX_ITEM_MAGICLEVEL; i++) 
		{
			// ������ּ���Ϊ�㣬��ʾ����Ҫ���������ˣ��˳�
			if (pnaryMALevel[i] == 0)	
        		break;
			SelectedMagicTable.Clear();

			// ����ǰ׺��׺����Ʒ���͡������������󼶱����з���������ħ��������ȡ����
			const KBPT_ClassMAIT *pCMITItem = m_BPTLib.GetCMIT(
				1 - (i & 1),        // ǰ׺��׺
				nType,              // ��Ʒ����
				nSeriesReq,         // �������� 
				pnaryMALevel[i]     // ���󼶱�
			);

			_ASSERT(pCMITItem);
			if (!pCMITItem)
				break;

			// ��������������ħ�����ԣ�����
			// 1.�Ƿ��Ѿ�ʹ�ã�
			// 2.����ֵ
			// 3.�Ƿ�����ͬ�ġ����Ե������
			// ȡ����Ӧ�������SelectedMagicTable��
			int nCMITItemCount = pCMITItem->GetCount();
			int j = 0;
			int nDecide = GetRandomNumber(0, 100 - 1) / (1 + nLucky * 20 / 100);

			for (j = 0; j < nCMITItemCount; j++)
			{
				// ��������ֵ��ȡ����Ӧ����
				int nMAIndex = pCMITItem->Get(j);
				KMAGICATTRIB_TABFILE *pMAItem = (KMAGICATTRIB_TABFILE *)m_BPTLib.GetMARecord(nMAIndex);
				if (!pMAItem)
				{
					_ASSERT(pMAItem);
					continue;
				}

				// ����Ѿ�ʹ�ã�����ȡ��һ��
				if (pMAItem->m_nUseFlag)
					continue;

				// ����������㣬ȡ��һ��
				if ((pMAItem->m_DropRate[nType]) <= nDecide)
					continue;
            
				// ����Ƿ��Ѿ�����ͬ�ġ����Ե������
				int k = 0;
				for (k = 0; k < i; k++)
				{
					_ASSERT(pMagicAttrTable[k]);

					if ((pMagicAttrTable[k]->m_MagicAttrib.nPropKind) == (pMAItem->m_MagicAttrib.nPropKind))
						break;
				}

				// ������� �����Ե��������ͬ��˵���Ѿ���һ����ͬ��ֵ��ѡ�У���ôȡ��һ��
				if (k < i)
					continue;

				SelectedMagicTable.Insert(nMAIndex);
			}

			int nSelectedCount = SelectedMagicTable.GetCount();

			if (nSelectedCount == 0)
			{   
				// ���û������������ħ�����ԣ����˳�
				break;
			}
        
			// �����������������ħ�����ԣ�������
			int nLuckyItemIndex = GetRandomNumber(0, nSelectedCount - 1);
			KMAGICATTRIB_TABFILE *pMAItem = (KMAGICATTRIB_TABFILE *)m_BPTLib.GetMARecord(
				 SelectedMagicTable.Get(nLuckyItemIndex)
			);

			// ��������������ħ�����ԣ��Ա�������ʱ��ʹ�ñ�־���
			pMagicAttrTable[i] = pMAItem;

			_ASSERT(pMAItem);
			_ASSERT(!(pMAItem->m_nUseFlag));     // ȷ����û��ʹ��

			// ����ʹ�ñ�־
			pMAItem->m_nUseFlag = true;
			
			pnaryMA[i].nAttribType = pMAItem->m_MagicAttrib.nPropKind;
			pnaryMA[i].nValue[0] = GetRandomNumber(
				pMAItem->m_MagicAttrib.aryRange[0].nMin, 
				pMAItem->m_MagicAttrib.aryRange[0].nMax
			);
			pnaryMA[i].nValue[1] = GetRandomNumber(
				pMAItem->m_MagicAttrib.aryRange[1].nMin, 
				pMAItem->m_MagicAttrib.aryRange[1].nMax
			);
			pnaryMA[i].nValue[2] = GetRandomNumber(
				pMAItem->m_MagicAttrib.aryRange[2].nMin, 
				pMAItem->m_MagicAttrib.aryRange[2].nMax
			);

		} // for ������Ҫȡ�ص�ħ�����Ա�
	}
    for (NULL; i < MAX_ITEM_MAGICATTRIB; i++)
    {
        // ���ʣ�µ���
        pnaryMA[i].nAttribType = 0;
        pnaryMA[i].nValue[0] = 0;
        pnaryMA[i].nValue[1] = 0;
        pnaryMA[i].nValue[2] = 0;
    }

    for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
    {
        if (!pMagicAttrTable[i])
            break;

        _ASSERT(pMagicAttrTable[i]->m_nUseFlag);   // ȷ���Ѿ�ʹ��

        pMagicAttrTable[i]->m_nUseFlag = false;    // ����Ϊû��ʹ��
    }

    nResult = true;
//Exit0:
	return nResult;
}


// Following Code is implement by DongBo
//BOOL KItemGenerator::Gen_MagicAttrib(int nType, const int* pnaryMALevel, int nSeriesReq, int nLucky,
//									 KItemNormalAttrib* pnaryMA)
//{
//	
//    // ȷ�������ڸ�װ����ȫ��ħ�����Լ�������
//	if (FALSE == GMA_GetAvaliableMA(nType))
//		{ _ASSERT(FALSE); return FALSE; }
//
//	// ȷ��������ħ�����Եĵȼ�,���ָ���
//	if (FALSE == GMA_GetLevelAndDropRate(nType))
//		{ _ASSERT(FALSE); return FALSE; }
//
//	// ������ħ��������ѡ������������. �߻�Ҫ��ѡ��6��
//	for (int i = 0; i < 6; i++)				// ����: ǰ׺, ż��: ��׺
//	{
//		GMA_ChooseMA((i&1), pnaryMALevel[i], nLucky, &(pnaryMA[i]));
//		if (pnaryMA[i].nAttribType == 0)
//			break;
//	}
//	return TRUE;
//}

/******************************************************************************
	����:	ȷ��������ָ��װ����ȫ��ħ�����Լ�������
	���:	nType: ָ��װ��
	����:	�ɹ�ʱ���ط���. m_GMA_naryMA[0]���������ڸ�װ����ȫ��ħ������ǰ׺
							m_GMA_nCount[0]��������ħ�����Ե�����
							m_GMA_naryMA[1]���������ڸ�װ����ȫ��ħ�����Ժ�׺
							m_GMA_nCount[1]��������ħ�����Ե�����
			ʧ��ʱ������
******************************************************************************/
BOOL KItemGenerator::GMA_GetAvaliableMA(int nType)
{
	//TODO: ������Ҫ��nType����ת��, ��װ������ֵ��� m_BPTLib.m_CMAT ���������
	for (int i = 0; i < 2; i++)		// 0: ǰ׺, 1: ��׺
	{
		const KBPT_ClassifiedMAT* pCMAT;
		pCMAT = m_BPTLib.GetCMAT(i, nType);
		if (pCMAT == NULL)
			{ _ASSERT(FALSE); return FALSE; }
		m_GMA_nCount[i] = NUMOFCMA;
		pCMAT->GetAll((int*)m_GMA_naryMA[i], &(m_GMA_nCount[i]));
	}
	return TRUE;
}

/******************************************************************************
	����:	ȷ��������ħ�����Եĵȼ�Ҫ��ͳ��ָ���
	���:	nType: ָ��װ��
			m_GMA_naryMA����: ����ȫ��ħ�����Ե�����ֵ
	����:	�ɹ�ʱ���ط���. m_GMA_naryLevel�����������ħ�����Եĵȼ�Ҫ��
							m_GMA_naryDropRate��������ħ�����Եĳ��ָ���
			ʧ��ʱ������
******************************************************************************/
BOOL KItemGenerator::GMA_GetLevelAndDropRate(int nType)
{
	//TODO: ������Ҫ��nType����ת��, ��װ������ֵ��� m_BPTLib.m_CMAT ���������

	for (int i = 0; i < 2; i++)		// 0: ǰ׺, 1: ��׺
	{
		for (int n = 0; n < m_GMA_nCount[i]; n++)
		{
			const KMAGICATTRIB_TABFILE* pMATF;
			pMATF = GetMARecord(m_GMA_naryMA[i][n]);
			if (pMATF == NULL)
				{ _ASSERT(FALSE); return FALSE; }
			m_GMA_naryLevel[i][n] = pMATF->m_nLevel;
			m_GMA_naryDropRate[i][n] = pMATF->m_DropRate[nType];		// ���Ż�
		}
	}
	return TRUE;
}

/******************************************************************************
	����:	��ȫ�����õ�ħ��������ѡ������������ħ��
	���:	nPos: 0: ѡǰ׺, 1: ѡ��׺
			nLevel: ��ħ�����Եĵȼ�Ҫ��
			nLucky: ����ֵ
			m_GMA_naryMA����: ����ȫ�����õ�ħ�����Ե�����ֵ
	����:	*pINA ������ѡħ���ĺ��Ĳ���
******************************************************************************/
void KItemGenerator::GMA_ChooseMA(int nPos, int nLevel, int nLucky, KItemNormalAttrib* pINA)
{
	_ASSERT(pINA != NULL);

	// ����ֵ
	pINA->nAttribType = 0;
	pINA->nValue[0] = 0;
	pINA->nValue[1] = 0;
	pINA->nValue[2] = 0;

	// ɸѡ��ȫ�����õ�ħ������
	int nDropRate = ::GetRandomNumber(0, 100) - nLucky;
	int nCount = GMA_GetCandidateMA(nPos, nLevel, nDropRate);
	if (nCount > 0)
	{
		int nFinal = ::GetRandomNumber(0, nCount);
		int nMAi = m_GMA_naryCandidateMA[nFinal];
		int nMA = m_GMA_naryMA[nPos][nMAi];
		if (nMA == -1)	// ������ظ���������
		{
			pINA->nAttribType = 0;
			return;
		}
		m_GMA_naryMA[nPos][nMAi] = -1;		// ��Ϊ-1,��ʾ��MA����
		const KMAGICATTRIB_TABFILE* pMATF = GetMARecord(nMA);
		if (NULL != pMATF)
		{
			const KMACP* pMACP = &(pMATF->m_MagicAttrib);
			pINA->nAttribType = pMACP->nPropKind;
			pINA->nValue[0] = ::GetRandomNumber(pMACP->aryRange[0].nMin, pMACP->aryRange[0].nMax);
			pINA->nValue[1] = ::GetRandomNumber(pMACP->aryRange[1].nMin, pMACP->aryRange[1].nMax);
			pINA->nValue[2] = ::GetRandomNumber(pMACP->aryRange[2].nMin, pMACP->aryRange[2].nMax);
		}
	}
}

/******************************************************************************
	����:	��ȡָ����ħ�����Լ�¼
	���:	i: ָ����¼
	����:	�ɹ�ʱ����ָ��ü�¼��ָ��
			ʧ��ʱ����NULL
******************************************************************************/
const KMAGICATTRIB_TABFILE* KItemGenerator::GetMARecord(int i) const
{
	return m_BPTLib.GetMARecord(i);
}

/******************************************************************************
	����:	��ȫ�����õ�ħ��������ѡ������������ħ��
	���:	nPos: 0: ѡǰ׺, 1: ѡ��׺
			nLevel: ��ħ�����Եĵȼ�Ҫ��
			nDropRate: ѡ�����ָ��ʴ��ڴ�ֵ��ħ������
			m_GMA_naryMA����: ����ȫ�����õ�ħ�����Ե�����ֵ
			m_GMA_naryLevel����: ��������ħ�����Եĵȼ�Ҫ��
			m_GMA_naryDropRate����: ��������ħ�����Եĳ��ָ���
	����:	m_GMA_naryCandidateMA�����������������ħ��
	˵��:	�� n = m_GMA_naryMA[nPos][m_GMA_naryCandidateMA[i]]
			�� GetMARecord(n) ����ָ��ħ�����Խṹ��ָ��
******************************************************************************/
int KItemGenerator::GMA_GetCandidateMA(int nPos, int nLevel, int nDropRate)
{
	int nCount = 0;
	for (int i = 0; i < m_GMA_nCount[nPos]; i++)
	{
		if (m_GMA_naryMA[nPos][i] != -1 &&		// -1��ʾ��MA���ù�
			m_GMA_naryLevel[nPos][i] == nLevel &&
			m_GMA_naryDropRate[nPos][i] >= nDropRate)
		{
			m_GMA_naryCandidateMA[nCount++] = i;
		}
	}
	return nCount;
}

BOOL KItemGenerator::GetMedicineCommonAttrib(IN int nDetailType, IN int nLevel, IN OUT KItem* pItem)
{
	if (NULL == pItem)
	{
		KASSERT(FALSE);
		return FALSE;
	}

	const int i = nDetailType * 5 + nLevel - 1;

	const KBASICPROP_MEDICINE* pMed = NULL;
	pMed = m_BPTLib.GetMedicineRecord(i);
	if (NULL == pMed)
	{
		_ASSERT(FALSE);
		return FALSE;
	}
	*pItem = *pMed;
	return TRUE;
}
/******************************************************************************
	���ܣ�	����ָ���ƽ�װ��װ��
	���:	nCondition: ǰ��������(Emf+Pmf)*Level��������
			pItem: װ����Ŀָ�롣
	����:	�ɹ�ʱ������, ��������� pItem ��ָ�����и���
			ʧ��ʱ���ؼ�
******************************************************************************/
BOOL KItemGenerator::GetEquipmentCommonAttrib(IN int nDetailType, IN int nParticularType, IN int nLevel, IN int nSeries, IN OUT KItem* pItem)
{
	if (NULL == pItem)
	{
		KASSERT(FALSE);
		return FALSE;
	}

	int i = 0;
	if (nDetailType > equip_horse)
		i = nParticularType;
	else
		i = nParticularType * 10 + nLevel - 1;// ����ʽ�ɲ߻�����
												// ���SPE 2002��8��31��7:40
												// email: ����������ֵ��
	const KBASICPROP_EQUIPMENT* pEqu = NULL;
	switch(nDetailType)
	{
	case equip_meleeweapon:
		pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
		break;
	case equip_rangeweapon:
		pEqu = m_BPTLib.GetRangeWeaponRecord(i);
		break;
	case equip_armor:
		pEqu = m_BPTLib.GetArmorRecord(i);
		break;
	case equip_helm:
		pEqu = m_BPTLib.GetHelmRecord(i);
		break;
	case equip_boots:
		pEqu = m_BPTLib.GetBootRecord(i);
		break;
	case equip_belt:
		pEqu = m_BPTLib.GetBeltRecord(i);
		break;
	case equip_amulet:
		pEqu = m_BPTLib.GetAmuletRecord(i);
		break;
	case equip_ring:
		pEqu = m_BPTLib.GetRingRecord(i);
		break;
	case equip_cuff:
		pEqu = m_BPTLib.GetCuffRecord(i);
		break;
	case equip_pendant:
		pEqu = m_BPTLib.GetPendantRecord(i);
		break;
	case equip_horse:
		pEqu = m_BPTLib.GetHorseRecord(i);
		break;
	case equip_mask:
		pEqu = m_BPTLib.GetMaskRecord(i);
		break;
	case equip_mantle:
		pEqu = m_BPTLib.GetMantleRecord(i);
		break;
	case equip_signet:
		pEqu = m_BPTLib.GetSignetRecord(i);
		break;
	case equip_shipin:
		pEqu = m_BPTLib.GetShipinRecord(i);
		break;
	case equip_hoods:
		pEqu = m_BPTLib.GetHoodsRecord(i);
		break;
	case equip_cloak:
		pEqu = m_BPTLib.GetCloakRecord(i);
		break;
	default:
		break;
	}
	if (NULL == pEqu)
		{ _ASSERT(FALSE); return FALSE; }
		// �������˵�ֱ��ԭ��: ֻ��n��װ��, ������ i ��ֵ��[0,n-1]֮��
		// ���3��: nParticularType ����?
		//			nLevel ����?
		//			ԭʼ��tab file������ m_BPTLib.m_BPTEquipment ��
		//			����������������?
	pItem->SetAttrib_CBR(pEqu);
	if (nSeries < series_metal)
		pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
	else
		pItem->SetSeries(nSeries);
	return TRUE;
}

/******************************************************************************
	���ܣ�	�������װ��
	���:	nDetailType: �������, �����������������......
			nParticularType: ��ϸ���
			nSeriesReq: ��������
			nLevel: �ȼ�
			pnaryMALevel: ħ�����Եȼ�����[6]
			nLucky: ����ֵ
	����:	�ɹ�ʱ���ط���, ��������� pItem ��ָ�����и���
			ʧ��ʱ������
******************************************************************************/
BOOL KItemGenerator::Gen_ExistEquipment(IN int nItemNature,
										IN int nDetailType,
									    IN int nParticularType,
									    IN int nSeriesReq,
									    IN int nLevel,
									    IN const int* pnaryMALevel,
									    IN int nLucky,
									    IN int nVersion,
									    IN OUT KItem* pItem
									   )
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	pItem->m_CommonAttrib.nItemNature = nItemNature;
	switch(nItemNature)
	{
	case NATURE_GOLD:
		{
		this->Gen_GoldEquipment(nDetailType, pnaryMALevel, nLucky, pItem);
		pItem->SetRow(nDetailType);
		return TRUE;
		}
		break;
	case NATURE_PLATINA:
		{
		this->Gen_PlatinaEquipment(nDetailType, pnaryMALevel, nLucky, pItem);
		pItem->SetRow(nDetailType);
		pItem->SetLevel(nLevel);
		return TRUE;
		}
		break;
	default:
		{
		BOOL bEC = FALSE;

		g_RandomSeed(pItem->m_GeneratorParam.uRandomSeed);
		if (pnaryMALevel)
			memcpy(pItem->m_GeneratorParam.nGeneratorLevel, pnaryMALevel, sizeof(int) * MAX_ITEM_MAGICLEVEL);
		int nGameVersion = pItem->m_GeneratorParam.nVersion;

		// ������ڲ���, ȷ��װ���Ļ�������
		int i = 0;
		if (nDetailType > equip_horse)
			i = nParticularType;
		else
			i = nParticularType * 10 + nLevel - 1;// ����ʽ�ɲ߻�����
													// ���SPE 2002��8��31��7:40
													// email: ����������ֵ��
													// �ȼ����ݴ�1��ʼ������Ҫ������
		const KBASICPROP_EQUIPMENT* pEqu = NULL;
		switch(nDetailType)
		{
		case equip_meleeweapon:
			pEqu = m_BPTLib.GetMeleeWeaponRecord(i);
			break;
		case equip_rangeweapon:
			pEqu = m_BPTLib.GetRangeWeaponRecord(i);
			break;
		case equip_armor:
			pEqu = m_BPTLib.GetArmorRecord(i);
			break;
		case equip_helm:
			pEqu = m_BPTLib.GetHelmRecord(i);
			break;
		case equip_boots:
			pEqu = m_BPTLib.GetBootRecord(i);
			break;
		case equip_belt:
			pEqu = m_BPTLib.GetBeltRecord(i);
			break;
		case equip_amulet:
			pEqu = m_BPTLib.GetAmuletRecord(i);
			break;
		case equip_ring:
			pEqu = m_BPTLib.GetRingRecord(i);
			break;
		case equip_cuff:
			pEqu = m_BPTLib.GetCuffRecord(i);
			break;
		case equip_pendant:
			pEqu = m_BPTLib.GetPendantRecord(i);
			break;
		case equip_horse:
			pEqu = m_BPTLib.GetHorseRecord(i);
			break;
		case equip_mask:
			pEqu = m_BPTLib.GetMaskRecord(i);
			break;
		case equip_mantle:
			pEqu = m_BPTLib.GetMantleRecord(i);
			break;
		case equip_signet:
			pEqu = m_BPTLib.GetSignetRecord(i);
			break;
		case equip_shipin:
			pEqu = m_BPTLib.GetShipinRecord(i);
			break;
		case equip_hoods:
			pEqu = m_BPTLib.GetHoodsRecord(i);
			break;
		case equip_cloak:
			pEqu = m_BPTLib.GetCloakRecord(i);
			break;
		default:
			break;
		}
		if (NULL == pEqu)
			{ _ASSERT(FALSE); return bEC; }
			// �������˵�ֱ��ԭ��: ֻ��n��װ��, ������ i ��ֵ��[0,n-1]֮��
			// ���3��: nParticularType ����?
			//			nLevel ����?
			//			ԭʼ��tab file������ m_BPTLib.m_BPTEquipment ��
			//			����������������?
		pItem->SetAttrib_CBR(pEqu);
		if (nSeriesReq < series_metal)
			pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
		else
			pItem->SetSeries(nSeriesReq);

		if (NULL == pnaryMALevel)
			return TRUE;
		KItemNormalAttrib	sMA[MAX_ITEM_MAGICATTRIB];	// ���ߵ�ħ������
		bEC = Gen_MagicAttrib(nDetailType, pnaryMALevel, nSeriesReq, nLucky, sMA, nGameVersion);
		if (bEC)
			pItem->SetAttrib_MA(sMA);

		pItem->SetNature(nItemNature);
		return bEC;
		}
		break;
	}
}


BOOL KItemGenerator::Gen_Quest(IN int nDetailType, IN OUT KItem* pItem)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_QUEST* pQuest = NULL;
	pQuest = m_BPTLib.GetQuestRecord(nDetailType);
	if (NULL == pQuest)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pQuest;
	return TRUE;
}


BOOL KItemGenerator::Gen_TownPortal(IN int nDetailType, IN OUT KItem* pItem)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);

	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_TOWNPORTAL* pPortal = NULL;
	pPortal = m_BPTLib.GetTownPortalRecord(nDetailType);
	if (NULL == pPortal)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pPortal;
	return TRUE;
}

BOOL KItemGenerator::Gen_MagicScript(IN int nDetailType, IN OUT KItem* pItem, IN int nLevel, IN int nSeries, IN int nLuck)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);
	
	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_MAGICSCRIPT* pMagicScript = NULL;
	pMagicScript = m_BPTLib.GetMagicScript(nDetailType);
	if (NULL == pMagicScript)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pMagicScript;
	if (nSeries < series_metal)
		pItem->SetSeries(::GetRandomNumber(series_metal, series_earth));
	else
		pItem->SetSeries(nSeries);
	pItem->SetLevel(nLevel);
	pItem->m_GeneratorParam.nLuck = nLuck;
	return TRUE;
}

BOOL KItemGenerator::Gen_Event(IN int nDetailType, IN OUT KItem* pItem)
{
	_ASSERT(this != NULL);
	_ASSERT(pItem != NULL);
	
	BOOL bEC = FALSE;
	ZeroMemory(&pItem->m_GeneratorParam, sizeof(pItem->m_GeneratorParam));
	
	const KBASICPROP_EVENTITEM* pEvent = NULL;
	pEvent = m_BPTLib.GetEvent(nDetailType);
	if (NULL == pEvent)
	{
		_ASSERT(FALSE);
		return bEC;
	}
	*pItem = *pEvent;
	return TRUE;
}
