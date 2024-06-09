#include "KCore.h"
#include "MyAssert.H"
#include "KTabFile.h"
#include "KNpc.h"
#include "KItem.h"
#include "KItemSet.h"
#include "KItemGenerator.h"
#include "KItemChangeRes.h"
#include "KTaskFuns.h"
#include "KSortScript.h"
#include "KBuySell.h"
#ifndef _STANDALONE
#include "../../../lib/S3DBInterface.h"
#else
#include "S3DBInterface.h"
#endif
#include	<time.h>
#ifndef _SERVER
#include "ImgRef.h"
#include "KPlayer.h"
#include "../../Represent/iRepresent/iRepresentshell.h"
#include "KMagicDesc.h"
#endif
KItem	Item[MAX_ITEM];
int GetRandomNumber(int nMin, int nMax);

KItem::KItem()
{
	::memset(&m_CommonAttrib,    0, sizeof(m_CommonAttrib));
	::memset(m_aryBaseAttrib,    0, sizeof(m_aryBaseAttrib));
	::memset(m_aryRequireAttrib, 0, sizeof(m_aryRequireAttrib));
	::memset(m_aryMagicAttrib,   0, sizeof(m_aryMagicAttrib));
	::memset(&m_GeneratorParam,	 0, sizeof(m_GeneratorParam));
	m_nCurrentDur = -1;
#ifndef _SERVER
	::memset(&m_Image,   0, sizeof(KRUImage));
#endif
	m_nIndex = 0;
}

KItem::~KItem()
{
}

void* KItem::GetRequirement(IN int nReq)
{
	int i = sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]);
	if (nReq >= i)
		return NULL;

	return &m_aryRequireAttrib[nReq];
}
/******************************************************************************
����:	��item�ϵ�ħ��Ӧ�õ�NPC����
��ڣ�	pNPC: ָ��NPC��ָ�룬nMagicAcive���򿪵�����������Ŀ
����:	ħ����Ӧ�á�
		���幤����KNpc�ĳ�Ա������ɡ�
		KItem ������û�г�Ա�������޸�
******************************************************************************/
void KItem::ApplyMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive, IN int nMagicActiveE) const
{
	_ASSERT(this != NULL);
	_ASSERT(nMagicActive >= 0);

	int nCount = nMagicActive;
	int nCountE = nMagicActiveE;
	int i = 0;

	// �������Ե���NPC
	for (i = 0; i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
		}
	}
	// ħ�����Ե���NPC
	for (i = 0; i < sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);

		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			if(i >= MAX_ITEM_NORMAL_MAGICATTRIB)
			{
				if (nCountE > 0)
				{
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
					nCountE--;
				}
			}
			else
			{
				if (i & 1)						// Ϊ�������Ǻ�׺��i���㿪ʼ��
				{
					if (nCount > 0)
					{
						pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
						nCount--;
					}
				}
				else
				{
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
				}
			}
		}
	}
}

/******************************************************************************
����:	��item�ϵ�ħ����NPC�����Ƴ�
��ڣ�	pNPC: ָ��NPC��ָ�룬nMagicAcive���򿪵�����������Ŀ
����:	ħ����Ӧ�á�
		���幤����KNpc�ĳ�Ա������ɡ�
		KItem ������û�г�Ա�������޸�
******************************************************************************/
void KItem::RemoveMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive, IN int nMagicActiveE) const
{
	_ASSERT(this != NULL);
	_ASSERT(nMagicActive >= 0);

	int nCount = nMagicActive;
	int nCountE = nMagicActiveE;
	int	i;
	
	// �������Ե���NPC
	for (i = 0; i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryBaseAttrib[i]);
		if (INVALID_ATTRIB != pAttrib->nAttribType)
		{
			KItemNormalAttrib RemoveAttrib;
			RemoveAttrib.nAttribType = pAttrib->nAttribType;
			RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
			RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
			RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
			pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
		}
	}

	for (i = 0; i < sizeof(m_aryMagicAttrib)/sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KItemNormalAttrib* pAttrib;
		pAttrib = &(m_aryMagicAttrib[i]);

		if (INVALID_ATTRIB != pAttrib->nAttribType)		// TODO: Ϊ -1 ����һ������?
		{
			KItemNormalAttrib RemoveAttrib;
			if(i >= MAX_ITEM_NORMAL_MAGICATTRIB)
			{
				if (nCountE > 0)
				{
					RemoveAttrib.nAttribType = pAttrib->nAttribType;
					RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
					RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
					RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
					nCountE--;
				}
			}
			else
			{
				if (i & 1)						// Ϊ�������Ǻ�׺��i���㿪ʼ��
				{
					if (nCount > 0)
					{
						RemoveAttrib.nAttribType = pAttrib->nAttribType;
						RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
						RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
						RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
						pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
						nCount--;
					}
				}
				else
				{
					RemoveAttrib.nAttribType = pAttrib->nAttribType;
					RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
					RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
					RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
					pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
				}
			}
		}
	}
}

/******************************************************************************
����:	��item�ϵĵ�N������ħ������Ӧ�õ�NPC����
��ڣ�	pNPC: ָ��NPC��ָ��
����:	ħ����Ӧ�á�
		���幤����KNpc�ĳ�Ա������ɡ�
		KItem ������û�г�Ա�������޸�
******************************************************************************/
void KItem::ApplyHiddenMagicAttribToNPC(IN KNpc* pNPC, IN int nMagicActive) const
{
	_ASSERT(this != NULL);
	if (nMagicActive <= 0)
		return;

	const KItemNormalAttrib* pAttrib;
	pAttrib = &(m_aryMagicAttrib[(nMagicActive << 1) - 1]);	// ��׺Ϊ�����������Գ�2��һ
	if (-1 != pAttrib->nAttribType)
	{
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)pAttrib);
	}
}

/******************************************************************************
����:	��item�ϵĵ�N������ħ�����Դ�NPC�����Ƴ�
��ڣ�	pNPC: ָ��NPC��ָ�룬nMagicActive����n��ħ������
����:	ħ�����Ƴ���
		���幤����KNpc�ĳ�Ա������ɡ�
		KItem ������û�г�Ա�������޸�
******************************************************************************/
void KItem::RemoveHiddenMagicAttribFromNPC(IN KNpc* pNPC, IN int nMagicActive) const
{
	_ASSERT(this != NULL);
	if (nMagicActive <= 0)
		return;

	const KItemNormalAttrib* pAttrib;
	pAttrib = &(m_aryMagicAttrib[(nMagicActive << 1) - 1]);	// ��׺Ϊ�����������Գ�2��һ
	if (-1 != pAttrib->nAttribType)
	{
		KItemNormalAttrib RemoveAttrib;
		RemoveAttrib.nAttribType = pAttrib->nAttribType;
		RemoveAttrib.nValue[0] = -pAttrib->nValue[0];
		RemoveAttrib.nValue[1] = -pAttrib->nValue[1];
		RemoveAttrib.nValue[2] = -pAttrib->nValue[2];
		pNPC->ModifyAttrib(pNPC->m_Index, (void *)&RemoveAttrib);
	}
}

/******************************************************************************
����:	���������ļ��е�����,Ϊitem�ĸ����ֵ
��ڣ�	pData: �������������ļ�������
����:	�ɹ�ʱ���ط���, ���³�Ա������ֵ:
			m_CommonAttrib,m_aryBaseAttrib,m_aryRequireAttrib
		ʧ��ʱ������
˵��:	CBR: Common,Base,Require
******************************************************************************/
BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT* pData)
{
	_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (pData)
	{
		//SetAttrib_Common(pData);
		*this = *pData;		// ���������
		SetAttrib_Base(pData->m_aryPropBasic);
		SetAttrib_Req(pData->m_aryPropReq);
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_GOLD* pData)
{
	_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (pData)
	{
		*this = *pData;		// ���������
		SetAttrib_Base(pData->m_aryPropBasic);
		SetAttrib_Req(pData->m_aryPropReq);
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_CBR(IN const KBASICPROP_EQUIPMENT_PLATINA* pData)
{
	_ASSERT(pData != NULL);
	
	BOOL bEC = FALSE;
	if (pData)
	{
		*this = *pData;		// ���������
		SetAttrib_Base(pData->m_aryPropBasic);
		SetAttrib_Req(pData->m_aryPropReq);
		bEC = TRUE;
	}
	return bEC;
}

BOOL KItem::SetAttrib_Base(const KEQCP_BASIC* pBasic)
{
	for (int i = 0;
		 i < sizeof(m_aryBaseAttrib)/sizeof(m_aryBaseAttrib[0]); i++)
	{
		KItemNormalAttrib* pDst;
		const KEQCP_BASIC* pSrc;
		pDst = &(m_aryBaseAttrib[i]);
		pSrc = &(pBasic[i]);
		pDst->nAttribType = pSrc->nType;
		pDst->nValue[0] = ::GetRandomNumber(pSrc->sRange.nMin, pSrc->sRange.nMax);
		pDst->nValue[1] = 0;	// RESERVED
		pDst->nValue[2] = 0;	// RESERVED
		if (pDst->nAttribType == magic_durability_v)
			SetDurability(pDst->nValue[0]);
	}
	//if (m_nCurrentDur == 0)	// ˵��û���;ö�����
	//	m_nCurrentDur = -1;
	return TRUE;
}

BOOL KItem::SetAttrib_Req(const KEQCP_REQ* pReq)
{
	for (int i = 0;
		 i < sizeof(m_aryRequireAttrib)/sizeof(m_aryRequireAttrib[0]); i++)
	{
		KItemNormalAttrib* pDst;
		pDst = &(m_aryRequireAttrib[i]);
		pDst->nAttribType = pReq[i].nType;
		pDst->nValue[0] = pReq[i].nPara;
		pDst->nValue[1] = 0;	// RESERVED
		pDst->nValue[2] = 0;	// RESERVED
	}
	return TRUE;
}

/******************************************************************************
����:	���ݴ��������, Ϊitem��ħ�����Ը���ֵ
��ڣ�	pMA: ��������
����:	�ɹ�ʱ���ط���, ���³�Ա������ֵ:
			m_aryMagicAttrib
		ʧ��ʱ������
******************************************************************************/
BOOL KItem::SetAttrib_MA(IN const KItemNormalAttrib* pMA)
{
	if (NULL == pMA)
		{ _ASSERT(FALSE); return FALSE; }
	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		m_aryMagicAttrib[i] = pMA[i];
		if (m_aryMagicAttrib[i].nAttribType == magic_indestructible_b)
		{
			SetDurability(-1);
		}
	}
	return TRUE;
}

/******************************************************************************
����:	���ݴ��������, Ϊitem��ħ�����Ը���ֵ
��ڣ�	pMA: ��������
����:	�ɹ�ʱ���ط���, ���³�Ա������ֵ:
			m_aryMagicAttrib
		ʧ��ʱ������
******************************************************************************/
BOOL KItem::SetAttrib_MA(IN const KMACP* pMA)
{
	if (NULL == pMA)
		{ _ASSERT(FALSE); return FALSE; }

	for (int i = 0; i < sizeof(m_aryMagicAttrib) / sizeof(m_aryMagicAttrib[0]); i++)
	{
		const KMACP* pSrc;
		KItemNormalAttrib* pDst;
		pSrc = &(pMA[i]);
		pDst = &(m_aryMagicAttrib[i]);

		pDst->nAttribType = pSrc->nPropKind;
		pDst->nValue[0] =  ::GetRandomNumber(pSrc->aryRange[0].nMin, pSrc->aryRange[0].nMax);
		pDst->nValue[1] =  ::GetRandomNumber(pSrc->aryRange[1].nMin, pSrc->aryRange[1].nMax);
		pDst->nValue[2] =  ::GetRandomNumber(pSrc->aryRange[2].nMin, pSrc->aryRange[2].nMax);
	}
	return TRUE;
}

void KItem::operator = (const KBASICPROP_EQUIPMENT& sData)
{
	KItemCommonAttrib* pCA	= &m_CommonAttrib;
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_NORMAL;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= sData.m_nParticularType;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= sData.m_nLevel;
	pCA->nSeries			= sData.m_nSeries;
	pCA->bShortKey			= FALSE;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= 1;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_EVENTITEM& sData)
{
	KItemCommonAttrib* pCA  = &m_CommonAttrib;
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_NORMAL;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->bShortKey			= FALSE;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,  sData.m_szScript);
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName); 
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));	
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_QUEST& sData)
{
	// ��ֵ: ��ͬ���Բ���
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_NORMAL;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	// ��ֵ: �������Բ���: ��
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryBaseAttrib));
	// ��ֵ: ħ�����Բ���: ��
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryBaseAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_TOWNPORTAL& sData)
{
	// ��ֵ: ��ͬ���Բ���
	KItemCommonAttrib* pCA = &m_CommonAttrib;
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_NORMAL;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,  sData.m_szScript);
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	// ��ֵ: �������Բ���: ��
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryBaseAttrib));
	// ��ֵ: ħ�����Բ���: ��
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryBaseAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_MAGICSCRIPT& sData)
{
	// ��ֵ: ��ͬ���Բ���
	KItemCommonAttrib* pCA	= &(m_CommonAttrib);
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_NORMAL;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= 0;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= 0;
	pCA->nSeries			= series_num;
	pCA->bShortKey			= sData.m_bShortKey;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::strcpy(pCA->szScript,  sData.m_szScript);
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));	
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::operator = (const KBASICPROP_MEDICINE& sData)
{
	// ��ֵ: ��ͬ���Բ���
	KItemCommonAttrib* pCA	= &m_CommonAttrib;
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_NORMAL;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= sData.m_nParticularType;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= sData.m_nLevel;
	pCA->nSeries			= sData.m_nSeries;
	pCA->bShortKey			= TRUE;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= sData.m_nMaxStack;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= 0;
	pCA->nSetID				= 0;
	pCA->nNeedToActive1		= 0;
	pCA->nNeedToActive2		= 0;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
#endif
	// ��ֵ: �������Բ���
	ZeroMemory(m_aryBaseAttrib, sizeof(m_aryBaseAttrib));
	KItemNormalAttrib* pBA = m_aryBaseAttrib;
	pBA[0].nAttribType = sData.m_aryAttrib[0].nAttrib;
	pBA[0].nValue[0]   = sData.m_aryAttrib[0].nValue;
	pBA[0].nValue[1]   = sData.m_aryAttrib[0].nTime;
	pBA[1].nAttribType = sData.m_aryAttrib[1].nAttrib;
	pBA[1].nValue[0]   = sData.m_aryAttrib[1].nValue;
	pBA[1].nValue[1]   = sData.m_aryAttrib[1].nTime;
	
	ZeroMemory(m_aryRequireAttrib, sizeof(m_aryRequireAttrib));
	ZeroMemory(m_aryMagicAttrib, sizeof(m_aryMagicAttrib));
#ifndef _SERVER
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif

}

void KItem::operator = (const KBASICPROP_EQUIPMENT_PLATINA& sData)
{
	KItemCommonAttrib* pCA  = &m_CommonAttrib;	
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_PLATINA;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= sData.m_nParticularType;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= sData.m_nLevel;
	pCA->nSeries			= sData.m_nSeries;
	pCA->bShortKey			= FALSE;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= 1;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= sData.m_nGroup;
	pCA->nSetID				= sData.m_nSetID;
	pCA->nNeedToActive1		= sData.m_nNeedToActive1;
	pCA->nNeedToActive2		= sData.m_nNeedToActive2;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}


void KItem::operator = (const KBASICPROP_EQUIPMENT_GOLD& sData)
{
	KItemCommonAttrib* pCA  = &m_CommonAttrib;	
	pCA->bTemp				= FALSE;
	pCA->BackLocal.Release();
	pCA->nItemNature		= NATURE_GOLD;
	pCA->nItemGenre			= sData.m_nItemGenre;
	pCA->nDetailType		= sData.m_nDetailType;
	pCA->nParticularType	= sData.m_nParticularType;
	pCA->nObjIdx			= sData.m_nObjIdx;
	pCA->nPrice				= sData.m_nPrice;
	pCA->nNewPrice			= sData.m_nPrice;
	pCA->bNewArrival		= FALSE;
	pCA->nLevel				= sData.m_nLevel;
	pCA->nSeries			= sData.m_nSeries;
	pCA->bShortKey			= FALSE;
	pCA->nWidth				= sData.m_nWidth;
	pCA->nHeight			= sData.m_nHeight;
	pCA->nStackNum			= 1;
	pCA->nMaxStack			= 1;
	pCA->nExpirePoint		= 0;
	pCA->nRow				= -1;
	pCA->nGroup				= sData.m_nGroup;
	pCA->nSetID				= sData.m_nSetID;
	pCA->nNeedToActive1		= sData.m_nNeedToActive1;
	pCA->nNeedToActive2		= sData.m_nNeedToActive2;
	pCA->nMantle			= 0;
	pCA->uFlash				= 0;
	pCA->nUpgradeLvl		= 0;
	pCA->nPhysicVal			= 0;
	pCA->nMagicVal			= 0;
	pCA->bLockSell			= FALSE;
	pCA->bLockTrade			= FALSE;
	pCA->bLockDrop			= FALSE;
	pCA->nParam				= -1;
	pCA->nFortune			= 0;
	pCA->nExpireTime		= 0;
	pCA->LockItem.Clear();
	::strcpy(pCA->szItemName,  sData.m_szName);
	::memset(pCA->szScript, 0, sizeof(pCA->szScript));
#ifndef _SERVER
	::strcpy(pCA->szImageName, sData.m_szImageName);
	::strcpy(pCA->szIntro,	   sData.m_szIntro);
	m_Image.Color.Color_b.a = 255;
	m_Image.nFrame = 0;
	m_Image.nISPosition = IMAGE_IS_POSITION_INIT;
	m_Image.nType = ISI_T_SPR;
	::strcpy(m_Image.szImage, pCA->szImageName);
	m_Image.uImage = 0;
#endif
}

void KItem::Remove()
{
	m_nIndex = 0;
}

BOOL KItem::SetBaseAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryBaseAttrib) / sizeof(m_aryBaseAttrib[0]); i++)
	{
		m_aryBaseAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetRequireAttrib(IN const KItemNormalAttrib* pAttrib)
{
	if (!pAttrib)
		return FALSE;

	for (int i = 0; i < sizeof(m_aryRequireAttrib) / sizeof(m_aryRequireAttrib[0]); i++)
	{
		m_aryRequireAttrib[i] = pAttrib[i];
	}
	return TRUE;
}

BOOL KItem::SetMagicAttrib(IN const KItemNormalAttrib* pAttrib)
{
	return SetAttrib_MA(pAttrib);
}
//------------------------------------------------------------------
//	ĥ�𣬷���ֵ��ʾʣ���;ö�
//------------------------------------------------------------------
int KItem::Abrade(IN const int nAbradeP, IN const int nRange)
{
	if (m_nCurrentDur == -1 || nRange == 0)	// ����ĥ��
		return -1;

	if(nAbradeP > 0)
		m_nCurrentDur -= GetMaxDurability() * nAbradeP / MAX_PERCENT;

	if(m_nCurrentDur > 0)
	{
		if (g_Random(nRange) == 0)	// nRandRange��֮һ�ĸ���
			m_nCurrentDur--;
	}
	if (m_nCurrentDur <= 0)
		m_nCurrentDur = 0;

	return m_nCurrentDur;
}

#ifndef _SERVER
void KItem::Paint(int nX, int nY, bool bResize/* = false*/, bool bPaintStack/* = false*/)
{
	if (bResize && (m_CommonAttrib.nWidth*m_CommonAttrib.nHeight > 1))
		strcpy(m_Image.szImage, RESIZEITEM_SPR);

	m_Image.oPosition.nX = nX;
	m_Image.oPosition.nY = nY;
	m_Image.bRenderStyle = IMAGE_RENDER_STYLE_ALPHA;
	g_pRepresent->DrawPrimitives(1, &m_Image, RU_T_IMAGE, TRUE);	

	if (IsStack() && bPaintStack)
	{
		char szNum[4];
		int nLen = sprintf(szNum, "%d", GetStackNum());
		szNum[3] = 0;
		g_pRepresent->OutputText(NORMAL_FONTSIZE, szNum, KRF_ZERO_END,
			nX + (GetWidth() * 27) - nLen * (NORMAL_FONTSIZE ) / 2,
			nY + (GetHeight() * 26) - (NORMAL_FONTSIZE + 1), 0xFFFFCC);

	}
}

void KItem::GetDesc(char* pszMsg, bool bShowPrice, bool bPriceScale, int nActiveAttrib, int nGoldActiveAttrib)
{
	char pszKeyName[256];
	char pszTemp[256];
	char pszTemp2[256];

	memset(pszKeyName, 0, sizeof(pszKeyName));
	memset(pszTemp, 0, sizeof(pszTemp));
	memset(pszTemp2, 0, sizeof(pszTemp2));

	switch(this->GetQuality())
	{
	case equip_normal:
		strcat(pszMsg, "<color=255,255,255>");
		break;
	case equip_magic:
		strcat(pszMsg, "<color=100,100,255>");
		break;
	case equip_damage:
		strcat(pszMsg, "<color=255,0,66>");
		break;
	case equip_violet:
		strcat(pszMsg, "<color=170,30,255>");
		break;
	case equip_gold:
		strcat(pszMsg, "<color=255,255,0>");
		break;
	case equip_platina:
		strcat(pszMsg, "<bclr=100,100,255><color=255,255,0>");
		break;
	}
	strcat(pszMsg, m_CommonAttrib.szItemName);

	if (m_CommonAttrib.nItemGenre == item_equip)
	{
		if (m_CommonAttrib.nItemNature == NATURE_PLATINA)
		{
			sprintf(pszTemp, " +%d", m_CommonAttrib.nLevel);
		}
		else if (m_CommonAttrib.nDetailType <= equip_horse)
		{
			if (m_CommonAttrib.nLevel > MAX_ITEM_LEVEL)
			{
				if (m_CommonAttrib.nLevel < MAX_ITEM_LEVEL * 10)
				{
					if (m_CommonAttrib.nLevel % MAX_ITEM_LEVEL == 0)
						sprintf(pszTemp, " [c�p %d]", m_CommonAttrib.nLevel/(m_CommonAttrib.nLevel / MAX_ITEM_LEVEL));
					else
						sprintf(pszTemp, " [c�p %d]", m_CommonAttrib.nLevel % MAX_ITEM_LEVEL);
				}
				else if (m_CommonAttrib.nLevel < MAX_ITEM_LEVEL * 100)
				{
					if (m_CommonAttrib.nLevel%100 == 0)
						sprintf(pszTemp, " [c�p %d]", m_CommonAttrib.nLevel/(m_CommonAttrib.nLevel / MAX_ITEM_LEVEL * 10));
					else
						sprintf(pszTemp, " [c�p %d]", m_CommonAttrib.nLevel % MAX_ITEM_LEVEL * 10);
				}
			}
			else if (m_CommonAttrib.nLevel > MIN_ITEM_LEVEL)
				sprintf(pszTemp, " [c�p %d]", m_CommonAttrib.nLevel);
		}
		if(pszTemp[0])
			strcat(pszMsg, pszTemp);
	}
	strcat(pszMsg, "<bclr=0,0,0><color>");

	char szPriceColor[moneyunit_num][32] = 
	{
		"<color=255,255,255>",
		"<color=255,90,0>",
		"<color=255,219,74>",
		"<color=255,219,74>",
		"<color=0,255,0>",
		"<color=0,255,0>",
		"<color=255,90,0>",
	};
	if (bShowPrice)
	{
		int nPrice = 0;
		if (bPriceScale)
		{
			nPrice = GetSalePrice();
			strcat(pszMsg, szPriceColor[moneyunit_money]);
			sprintf(pszTemp2, "%d", moneyunit_money);
		}
		else
		{
			nPrice = GetCurPrice();
			strcat(pszMsg, szPriceColor[Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit]);
			sprintf(pszTemp2, "%d", Player[CLIENT_PLAYER_INDEX].m_BuyInfo.m_nMoneyUnit);	
		}
		strcat(pszMsg, "\n");
		strcpy(pszTemp, "Gi� c�: ");
		strcat(pszMsg, pszTemp);

		g_GameSetting.GetString("MoneyUnit", pszTemp2, "", pszTemp, sizeof(pszTemp));		
		sprintf(pszTemp2, "%d %s<color=255,255,255>", nPrice, pszTemp);
		strcat(pszMsg, pszTemp2);	
	}
		
	pszTemp2[0] = 0;
	switch (m_CommonAttrib.LockItem.nState)
	{
		case LOCK_STATE_CHARACTER:
			sprintf(pszTemp2, "<color=0,255,0>V�t ph�m ��nh k�m theo nh�n v�t<color>");
			break;
		case LOCK_STATE_FOREVER:
			sprintf(pszTemp2, "<color=0,255,0>V�t ph�m n�y �� kh�a b�o hi�m v�nh vi�n<color>");
			break;
		case LOCK_STATE_LOCK:
			sprintf(pszTemp2, "<color=0,255,0>V�t ph�m n�y �� kh�a b�o hi�m<color>");
			break;
		case LOCK_STATE_UNLOCK:
			if (m_CommonAttrib.LockItem.dwLockTime > KSG_GetCurSec())
			{
				time_t nowtime=m_CommonAttrib.LockItem.dwLockTime+1451581200;
				struct tm * timeinfo = localtime(&nowtime);
				strcpy(pszTemp, "<color=0,255,0>Th�i gian m� kh�a: %H:%M:%S %d-%m-%Y<color>");
				strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
			}
			break;
	}
	if (pszTemp2[0])
	{
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
	}	

	sprintf(pszKeyName, "%d", m_CommonAttrib.nSeries);
	g_GameSetting.GetString("Elements", pszKeyName, "", pszTemp, sizeof(pszTemp));
	if (pszTemp[0])
	{
		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp);
	}

	if (m_CommonAttrib.szIntro[0])
	{
		char szIntro[SZBUFLEN_1];

		int offset = 0, nL = 0, nS = 0;
		int nStrL = sprintf(szIntro, "%s", m_CommonAttrib.szIntro);
		while(szIntro[offset] != 0)
		{
			if(szIntro[offset] == '<')
			{
				if(szIntro[offset+1] == 'e' &&
					szIntro[offset+2] == 'n' &&
					szIntro[offset+3] == 't' &&
					szIntro[offset+4] == 'e' &&
					szIntro[offset+5] == 'r' &&
					szIntro[offset+6] == '>')
					nL = 0;
				if(szIntro[offset+1] == 'c' &&
					szIntro[offset+2] == 'o' &&
					szIntro[offset+3] == 'l' &&
					szIntro[offset+4] == 'o' &&
					szIntro[offset+5] == 'r')
				{
					if (szIntro[offset+6] == '>')
						nL -= 6;
					else
					{
						int k = 0;
						for(k = 0; k<12; k++)
						{
							if(szIntro[offset+6+k] == '>') 
								break;
						}
						nL -= 6+k;
					}
				}
				if(szIntro[offset+1] == 'b' &&
					szIntro[offset+2] == 'c' &&
					szIntro[offset+3] == 'l' &&
					szIntro[offset+4] == 'r')
				{
					if (szIntro[offset+5] == '>')
						nL -= 5;
					else
					{
						if (szIntro[offset+5] == '=')
						{
							int k = 0;
							for(k = 0; k<12; k++)
							{
								if(szIntro[offset+5+k] == '>') 
									break;
							}
							nL -= 5+k;
						}
					}
				}
			}

			if(nL == 32 && (offset+7) <nStrL)
			{
				while(offset+nS < nStrL && szIntro[offset+nS] < 0 || (szIntro[offset+nS] > 32 && szIntro[offset+nS] < 126))
					nS++;
				if(nS >= 6)
				{
					memmove(&szIntro[offset+nS+6], &szIntro[offset+nS], nStrL-offset+nS);
					memcpy(&szIntro[offset+nS],"<enter>",7);
					offset += 7+nS;
					nStrL += 6;
				}
				else
				{
					memmove(&szIntro[offset+7], &szIntro[offset], nStrL-offset+1);
					memcpy(&szIntro[offset+nS],"<enter>",7);
					offset += 7+nS;
					nStrL += 7;
				}
				nL = 0;
				nS = 0;
			}
			offset++;
			nL++;
		}
		if(strlen(szIntro) > (SZBUFLEN_1-1))
			szIntro[SZBUFLEN_1-1] = 0;

		strcat(pszMsg, "\n");
		strcat(pszMsg, szIntro);
	}
	if (m_CommonAttrib.nItemGenre == item_equip)
	{
		/*if (m_aryBaseAttrib[0].nAttribType > 0 || 
			m_aryRequireAttrib[0].nAttribType > 0 || 
			m_aryMagicAttrib[0].nAttribType > 0)*/
			strcat(pszMsg, "\n\n");
	}
	else
		strcat(pszMsg, "\n");

	if (m_CommonAttrib.nItemGenre == item_magicscript)
	{
		if (m_GeneratorParam.nLuck)
		{
			KTabFile MagicTab;
			
			MagicTab.Load(MAGICATTRIB_LVINDEX_FILE);

			sprintf(pszKeyName,"%d", m_GeneratorParam.nLuck);

			MagicTab.GetString(pszKeyName,"DESC","",pszTemp,sizeof(pszTemp));
			strcat(pszMsg, "<color=100,100,255>Thu�c t�nh: ");
			strcat(pszMsg, pszTemp);
			strcat(pszMsg, "\n");
			MagicTab.GetString(pszKeyName,"FIT_EQUIP","",pszTemp,sizeof(pszTemp));
			strcat(pszMsg, "<color=255,219,74>Lo�i trang b� c� th� kh�m n�m: ");
			strcat(pszMsg, pszTemp);
			strcat(pszMsg, "<color=255,255,255>");
			strcat(pszMsg, "\n");
		}

		if(m_CommonAttrib.nLevel)
		{
			sprintf(pszTemp, "<color=100,100,255>Ph�m ch�t thu�c t�nh: <color=255,255,0>%d<color=255,255,255>", m_CommonAttrib.nLevel);
			strcat(pszMsg, pszTemp);
		}
	}

	bool bActiveAllAttrib = true;
	int i = 0;
	for (i = 0; i < 7; i++)
	{
		if (!m_aryBaseAttrib[i].nAttribType)
		{
			continue;
		}
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			if (GetDurability()==-1)
				sprintf(pszTemp, "<color=255,255,0>Kh�ng th� ph� h�y<color=255,255,255>");
			else
				sprintf(pszTemp, "�� b�n: %3d / %3d", GetDurability(), GetMaxDurability());
			strcat(pszMsg, pszTemp);
		}
		else
		{
			char* pszTemp2 = (char *)g_MagicDesc.GetDesc(&m_aryBaseAttrib[i]);
			if (!pszTemp2 || !pszTemp2[0])
				continue;
			strcat(pszMsg, pszTemp2);
		}
		strcat(pszMsg, "\n");
	}
	for (i = 0; i < 6; i++)
	{
		if (!m_aryRequireAttrib[i].nAttribType)
			continue;

		char* pszTemp2 = (char *)g_MagicDesc.GetDesc(&m_aryRequireAttrib[i]);
		if (!pszTemp2 || !pszTemp2[0])
			continue;

		if (Player[CLIENT_PLAYER_INDEX].m_ItemList.EnoughAttrib(&m_aryRequireAttrib[i]))
			strcat(pszMsg, "<color=255,255,255>");
		else
			strcat(pszMsg, "<color=255,0,0>");

		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n");
	}

	for (i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		if (!m_aryMagicAttrib[i].nAttribType)
		{
			if(m_CommonAttrib.nItemNature == NATURE_VIOLET)
			{
				if(i < m_GeneratorParam.nLuck)
				{
					if (i == MAX_ITEM_NORMAL_MAGICATTRIB)
						strcat(pszMsg, "\n");
					strcat(pszMsg, "<color=255,255,0>Ch�a kh�m n�m<color=255,255,255>");
					strcat(pszMsg, "\n");
				}
			}
			continue;
		}

		char* pszTemp2 = (char *)g_MagicDesc.GetDesc(&m_aryMagicAttrib[i]);
		if (!pszTemp2 || !pszTemp2[0])
			continue;
		if (i == MAX_ITEM_NORMAL_MAGICATTRIB && m_aryMagicAttrib[i].nAttribType)
			strcat(pszMsg, "\n");

		if (i < MAX_ITEM_NORMAL_MAGICATTRIB)
		{
			if ((i & 1) == 0)
			{
				switch(m_CommonAttrib.nItemNature)
				{
				case NATURE_GOLD:
				case NATURE_PLATINA:
					strcat(pszMsg, "<color=255,255,0>");
					break;
				case NATURE_VIOLET:
					strcat(pszMsg, "<color=170,30,255>");
					break;
				default:
					strcat(pszMsg, "<color=99,101,255>");
					break;
				}
			}
			else
			{
				if ((i>>1) < nActiveAttrib)
				{
					switch(m_CommonAttrib.nItemNature)
					{
					case NATURE_GOLD:
					case NATURE_PLATINA:
						strcat(pszMsg, "<color=255,255,0>");
						break;
					case NATURE_VIOLET:
						strcat(pszMsg, "<color=170,30,255>");
						break;
					default:
						strcat(pszMsg, "<color=100,100,255>");
						break;
					}
				}
				else
				{
					switch(m_CommonAttrib.nItemNature)
					{
						case NATURE_GOLD:
						case NATURE_PLATINA:
							strcat(pszMsg, "<color=123,125,90>");
							break;
						case NATURE_VIOLET:
							strcat(pszMsg, "<color=170,135,184>");
							break;
						default:
							strcat(pszMsg, "<color=120,120,120>");
							break;
					}
					bActiveAllAttrib = false;
				}
			}
		}
		else
		{
			if(nGoldActiveAttrib)
			{
				switch(m_CommonAttrib.nItemNature)
				{
				case NATURE_GOLD:
				case NATURE_PLATINA:
					strcat(pszMsg, "<color=255,255,0>");
					break;
				case NATURE_VIOLET:
					strcat(pszMsg, "<color=170,30,255>");
					break;
				default:
					strcat(pszMsg, "<color=100,100,255>");
					break;
				}
				nGoldActiveAttrib--;
			}
			else
			{
				switch(m_CommonAttrib.nItemNature)
				{
					case NATURE_GOLD:
					case NATURE_PLATINA:
						strcat(pszMsg, "<color=123,125,90>");
						break;
					case NATURE_VIOLET:
						strcat(pszMsg, "<color=170,135,184>");
						break;
					default:
						strcat(pszMsg, "<color=120,120,120>");
						break;
				}
			}
		}
		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n");
		strcat(pszMsg, "<color=255,255,255>");
	}
	
	if (m_CommonAttrib.nExpireTime > KSG_GetCurSec())
	{
		time_t nowtime= m_CommonAttrib.nExpireTime+1451581200;
		struct tm * timeinfo = localtime(&nowtime);
		strcpy(pszTemp, "<color=255,90,0>Th�i h�n s� d�ng: %H:%M:%S %d-%m-%Y<color>");
		strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
		strcat(pszMsg, "\n");
		strcat(pszMsg,pszTemp2);
		strcat(pszMsg, "\n");
	}

	if (m_CommonAttrib.nExpirePoint) 
	{
		time_t nowtime = m_CommonAttrib.nExpirePoint+(KSG_GetCurSec()+1451581200);
		struct tm * timeinfo = localtime(&nowtime);
		strcpy(pszTemp, "<color=255,90,0>Th�i h�n s� d�ng: %H:%M:%S %d-%m-%Y<color>");
		strftime(pszTemp2, sizeof(pszTemp2), pszTemp, timeinfo);
		strcat(pszMsg, "\n");
		strcat(pszMsg,pszTemp2);
		strcat(pszMsg, "\n");
	}
	PlayerItem m_pItems = Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[Player[CLIENT_PLAYER_INDEX].m_ItemList.FindSame(m_dwID)];

	
	if (!bActiveAllAttrib && 
		m_aryMagicAttrib[0].nAttribType && 
		m_CommonAttrib.nItemGenre == item_equip && 
		m_CommonAttrib.nDetailType < equip_horse
		)
	{
		if (m_pItems.nPlace == pos_equip)
		{
			char szBuff[32], szBuffer[32];

			sprintf(pszKeyName, "NeedSr%d", m_CommonAttrib.nSeries);
			g_GameSetting.GetString("ActiveEquip", pszKeyName, "", szBuff, sizeof(szBuff));
			if (m_pItems.nX == itempart_ring1)
				sprintf(pszKeyName, "NeedEq%d1", m_CommonAttrib.nDetailType);
			else if (m_pItems.nX == itempart_ring2)
				sprintf(pszKeyName, "NeedEq%d2", m_CommonAttrib.nDetailType);
			else
				sprintf(pszKeyName, "NeedEq%d", m_CommonAttrib.nDetailType);
			g_GameSetting.GetString("ActiveEquip", pszKeyName, "", szBuffer, sizeof(szBuffer));

			sprintf(pszTemp, "<color=Yellow>C�n h� %s c�a %s �� k�ch ho�t thu�c t�nh �m<color>", szBuff, szBuffer);
			strcat(pszMsg, "\n");	
			strcat(pszMsg, pszTemp);	
			strcat(pszMsg, "\n<color=255,255,255>");
		}
	}
	if (m_CommonAttrib.nItemNature >= NATURE_GOLD)
	{
		BOOL bFlag = FALSE;
		KTabFile TabFile;

		if(m_CommonAttrib.nItemNature == NATURE_GOLD)
			bFlag = TabFile.Load(GOLD_EQUIP_FILE);
		else
			bFlag = TabFile.Load(PLATINA_EQUIP_FILE);
		if(bFlag)
		{
			if(m_CommonAttrib.nDetailType < itempart_horse)
				strcat(pszMsg, "\n");

			int i,j;
			for (i = 0; i < m_CommonAttrib.nNeedToActive2; i++)
			{
				TabFile.GetInteger(m_CommonAttrib.nRow + 3 - m_CommonAttrib.nSetID + i, "Group", 0, &j);
				if(j == m_CommonAttrib.nGroup)
				{
					TabFile.GetString(m_CommonAttrib.nRow + 3 - m_CommonAttrib.nSetID + i, "Name","", pszTemp, sizeof(pszTemp));
					bFlag = FALSE;
					if(m_pItems.nPlace > pos_hand)
					{
						for (j=0;j<MAX_PLAYER_ITEM;j++)
						{
							if ((Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[j].nIdx].GetGroup() == m_CommonAttrib.nGroup) && 
								(Item[Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[j].nIdx].GetSetID() == i+1))
							{
								bFlag = TRUE;
								break;
							}
						}	
						if(bFlag)
						{
							if(Player[CLIENT_PLAYER_INDEX].m_ItemList.m_Items[j].nPlace == pos_equip)
								strcat(pszMsg, "<color=247,195,90>");
							else
								strcat(pszMsg, "<color=0,255,0>");
						}
						else
							strcat(pszMsg, "<color=0,75,0>");
					}
					else
						strcat(pszMsg, "<color=0,75,0>");

					strcat(pszMsg, pszTemp);
					strcat(pszMsg, "\n<color=255,255,255>");
				}
			}
		}
	}

	KLuaScript * pScript = NULL;
	g_SetFilePath("\\");
	//����Lua�ű�
	KLuaScript Script;
	Script.Init();
	if (Script.Load(m_CommonAttrib.szScript)) 
	{
		pScript = &Script;
		int nSafeIndex = 1;
		pScript->SafeCallBegin(&nSafeIndex);

		pScript->CallFunction("GetDesc", 1, "d", Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchID(GetID()));
		if (Lua_IsString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
		{
			strcat(pszMsg, "<color=255,255,255>\n");
			strcat(pszMsg, (char *)Lua_ValueToString(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)));
			strcat(pszMsg, "\n<color=255,255,255>");
		}
		pScript->SafeCallEnd(nSafeIndex);
	}

	if (m_CommonAttrib.nFortune)
	{
		sprintf(pszTemp2, "<color=255,255,0>Tr� s� t�i ph� binh gi�p:<color> <color=0,255,0>%d<color>", m_CommonAttrib.nFortune);

		strcat(pszMsg, "\n");
		strcat(pszMsg, pszTemp2);
		strcat(pszMsg, "\n<color=255,255,255>");
	}

	if ((Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_PTrade.nTrade || bShowPrice) && 
		m_CommonAttrib.nTradePrice) 
	{
		strcat(pszMsg,"\n");
		strcat(pszMsg, "<color=255,255,255>Gi� ni�m y�t: <color=255,255,0>");
		if (m_CommonAttrib.nTradePrice < MONEY_FLOOR)
			sprintf(pszTemp,"%d l��ng",m_CommonAttrib.nTradePrice);
		else if ((m_CommonAttrib.nTradePrice % MONEY_FLOOR) == 0)
			sprintf(pszTemp,"%d v�n l��ng",m_CommonAttrib.nTradePrice / MONEY_FLOOR);
		else
			sprintf(pszTemp,"%d v�n %d l��ng",m_CommonAttrib.nTradePrice / MONEY_FLOOR, m_CommonAttrib.nTradePrice % MONEY_FLOOR);
		strcat(pszMsg, pszTemp);
	}
}
#endif

void KItem::SetDurability(IN const int nDur)
{
	m_nCurrentDur = nDur;
};

int KItem::GetMaxDurability()
{
	for (int i = 0; i < 7; i++)
	{
		if (m_aryBaseAttrib[i].nAttribType == magic_durability_v)
		{
			return m_aryBaseAttrib[i].nValue[0];
		}
	}
	return -1;
}

int KItem::GetTotalMagicLevel()
{
	int nRet = 0;
	for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++)
	{
		nRet += m_GeneratorParam.nGeneratorLevel[i];
	}
	return nRet;
}

BOOL KItem::IsReduce()
{
	for (int i = 0; i < MAX_ITEM_MAGICATTRIB; i++) 
	{
		if (m_GeneratorParam.nGeneratorLevel[i] > MAX_ITEM_GENERATORLEVEL)
			return TRUE;
	}
	return FALSE;
}

int KItem::GetRepairPrice()
{
	if (!ItemSet.m_sRepairParam.nMagicScale)
		return 0;

	if (m_CommonAttrib.nItemGenre != item_equip)
		return 0;

	if (m_nCurrentDur == -1)
		return 0;

	int nMaxDur = GetMaxDurability();
	if (nMaxDur <= 0)
		return 0;

	switch(m_CommonAttrib.nItemNature)
	{
	case NATURE_GOLD:
		return m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nGoldScale / 100 * (nMaxDur - m_nCurrentDur);
	case NATURE_PLATINA:
		return m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nPlatinaScale / 100 * (nMaxDur - m_nCurrentDur);
	default:
		int nSumMagic = GetTotalMagicLevel();

		if (IsReduce())
			nSumMagic = MAX_ITEM_MAGICATTRIB*MAX_ITEM_GENERATORLEVEL;
		return m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nPriceScale / 100 * (nMaxDur - m_nCurrentDur) / nMaxDur * (ItemSet.m_sRepairParam.nMagicScale + nSumMagic) / ItemSet.m_sRepairParam.nMagicScale;
	}
	return m_CommonAttrib.nPrice * ItemSet.m_sRepairParam.nPriceScale / 100 * (nMaxDur - m_nCurrentDur) / nMaxDur;
}

BOOL KItem::CanBeRepaired()
{
	if (GetGenre() != item_equip)
		return FALSE;

	if (m_nCurrentDur == -1)
		return FALSE;

	int nMaxDur = GetMaxDurability();
	if (m_nCurrentDur == nMaxDur)
		return FALSE;

	return TRUE;
}

BOOL KItem::CanShortKey()
{
	if (m_CommonAttrib.nWidth != 1 || m_CommonAttrib.nHeight != 1)
		return FALSE;

	return m_CommonAttrib.bShortKey;
}

int KItem::GetParticularMelee()
{	int nParticularTypeMellee = m_CommonAttrib.nParticularType;
	if (m_CommonAttrib.nObjIdx == 9)
		nParticularTypeMellee = 0; 
	else if (m_CommonAttrib.nObjIdx == 10)
		nParticularTypeMellee = 1; 
	else if (m_CommonAttrib.nObjIdx == 11)
		nParticularTypeMellee = 2; 
	else if (m_CommonAttrib.nObjIdx == 12)
		nParticularTypeMellee = 3; 
	else if (m_CommonAttrib.nObjIdx == 13)
		nParticularTypeMellee = 4; 
	else if (m_CommonAttrib.nObjIdx == 14)
		nParticularTypeMellee = 5; 
	else if (m_CommonAttrib.nDetailType == 1)
	{
		if (nParticularTypeMellee == 3)
			nParticularTypeMellee = 0;
		else if (nParticularTypeMellee == 4)
			nParticularTypeMellee = 1;
		else if (nParticularTypeMellee == 5)
			nParticularTypeMellee = 2;
	}
	return nParticularTypeMellee;
}

int KItem::GetQuality()
{
	if (GetGenre() == item_equip)
	{
		if(m_nCurrentDur == 0)
			return equip_damage;
		else
		{
			switch(m_CommonAttrib.nItemNature)
			{	
			case NATURE_GOLD:
				return equip_gold;
			case NATURE_PLATINA:
				return equip_platina;
			case NATURE_VIOLET:
				return equip_violet;
			}
		}
		if (m_aryMagicAttrib[0].nAttribType)
			return equip_magic;
	}
	return equip_normal;
}

int KItem::GetSalePrice()
{
	if(m_CommonAttrib.bLockTrade)
		return 0;

	return (m_CommonAttrib.nPrice/BUY_SELL_SCALE)*m_CommonAttrib.nStackNum;
}