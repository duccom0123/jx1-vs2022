// db_Tran.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include "DBTable.h"

#define MAX_TASK 2000

using namespace std;

#pragma pack( push, 1 )

struct TRoleBaseInfoOld
{
	DWORD	dwId;
	char	szName[32];
	bool	bSex;
	char	szAlias[32];		
	char	caccname[32];
	BYTE	nFirstSect;
	BYTE	nSect;
	BYTE	cFightMode;
	BYTE	cUseRevive;
	BYTE	cIsExchange;
	BYTE	cPkStatus;
	int		ijoincount;
	int		isectrole;
	int		iexpandrank;
	int		igroupcode;
	int		igrouprole;
	int		irevivalid;
	int		irevivalx;
	int		irevivaly;
	int		ientergameid;
	int		ientergamex;
	int		ientergamey;
	char	cpartnercode[32];
	int		isavemoney;
	int		imoney;
	int		ifiveprop;
	int		iteam;
	int		ifightlevel;
	int		fightexp;
	int		ileadlevel;
	int		ileadexp;
	int		iliveexp;
	int		ipower;
	int		iagility;
	int		iouter;
	int		iinside;
	int		iluck;
	int		imaxlife;
	int		imaxstamina;
	int		imaxinner;
	int		icurlife;
	int		icurstamina;
	int		icurinner;
	int		ipkvalue;
	int		ireputevalue;
	int		ifuyuanvalue;
	int		irebornvalue;
	int		ipassrole;
	BYTE	ilockpkstate;
	int		ileavetongtime;
	char	imatename[32];
	char	iplayertitle[64];
	char	itongtitle[64];
	int		ileftprop;
	int		ileftfight;
	int		ileftlife;
	int		ifinishgame;
	short	iarmorres;
	short	iweaponres;
	short	ihelmres;
	int		nSectStat;
	int		nWorldStat;
	int		nKillPeopleNumber;
	int		nForbiddenFlag;
	DWORD	dwTongID;
	int		iavatarrole;
	int		iexitemrole;
	int		iexboxrole;
};

struct TRoleDataOld
{
	TRoleBaseInfoOld BaseInfo;
	DWORD	dwVersion;
	bool	bBaseNeedUpdate;
	short	nFightSkillCount;
	short	nLiveSkillCount;
	BYTE	nTaskCount;
	short	nItemCount;
	short	nFriendCount;
	int		dwTaskOffset;
	int		dwLSkillOffset;
	int		dwFSkillOffset;
	int		dwItemOffset;
	int		dwFriendOffset;
	int		dwDataLen;
	BYTE	pBuffer[1];
};

struct TDBItemDataOld
{
	char iproperty[32];
	char icontent[128];
	int isecretyear;
	BYTE isecretmonth;
	BYTE isecretday;
	BYTE isecrethour;
	BYTE isecretmin;
	BYTE isecretsec;
	int iid;
	int iequipclasscode;
	int ilocal;
	int ix;
	int iy;
	int ipreviouslocal;
	int ipreviousx;
	int ipreviousy;
	int iequipcode;
	int idetailtype;
	int iparticulartype;
	int ilevel;
	int iseries;
	int iequipversion;
	int irandseed;
	int iparam[16];
	int ilucky;
	int iidentify;
	int	idurability;
	int	iitemstack;
	int iyear;
	BYTE imonth;
	BYTE iday;
	BYTE ihour;
	BYTE imin;
	int	ibind;
	BYTE icansell;
	BYTE icantrade;
	BYTE icandrop;
	int  igoldid;
	int	iitemvalue;
	BYTE ipurpleitem;
	BYTE imantlearmor;
	int iweapontypeitem;
	BYTE iitembroken;
	BYTE ipreviouswidth;
	BYTE ipreviousheight;
	int iitemfortune;
	int ifusionmagic[4];
};

struct TRoleBaseInfoNew
{
	DWORD	dwId;
	char	szName[32];
	bool	bSex;
	char	szAlias[32];		
	char	caccname[32];
	BYTE	nFirstSect;
	BYTE	nSect;
	BYTE	cFightMode;
	BYTE	cUseRevive;
	BYTE	cIsExchange;
	BYTE	cPkStatus;
	int		ijoincount;
	int		isectrole;
	int		iexpandrank;
	int		igroupcode;
	int		igrouprole;
	int		irevivalid;
	int		irevivalx;
	int		irevivaly;
	int		ientergameid;
	int		ientergamex;
	int		ientergamey;
	char	cpartnercode[32];
	int		isavemoney;
	int		imoney;
	int		ifiveprop;
	int		iteam;
	int		ifightlevel;
	int		fightexp;
	int		ileadlevel;
	int		ileadexp;
	int		iliveexp;
	int		ipower;
	int		iagility;
	int		iouter;
	int		iinside;
	int		iluck;
	int		imaxlife;
	int		imaxstamina;
	int		imaxinner;
	int		icurlife;
	int		icurstamina;
	int		icurinner;
	int		ipkvalue;
	int		ireputevalue;
	int		ifuyuanvalue;
	int		irebornvalue;
	int		ipassrole;
	BYTE	ilockpkstate;
	int		ileavetongtime;
	char	imatename[32];
	char	iplayertitle[64];
	char	itongtitle[64];
	int		ileftprop;
	int		ileftfight;
	int		ileftlife;
	int		ifinishgame;
	short	iarmorres;
	short	iweaponres;
	short	ihelmres;
	int		nSectStat;
	int		nWorldStat;
	int		nKillPeopleNumber;
	int		nForbiddenFlag;
	DWORD	dwTongID;
	int		iavatarrole;
	int		iexitemrole;
};

struct TRoleDataNew
{
	TRoleBaseInfoNew BaseInfo;
	DWORD	dwVersion;
	bool	bBaseNeedUpdate;
	short	nFightSkillCount;
	short	nLiveSkillCount;
	BYTE	nTaskCount;
	short	nItemCount;
	short	nFriendCount;
	int		dwTaskOffset;
	int		dwLSkillOffset;
	int		dwFSkillOffset;
	int		dwItemOffset;
	int		dwFriendOffset;
	int		dwDataLen;
	BYTE	pBuffer[1];
};

struct TDBItemDataNew
{
	char iproperty[32];
	char icontent[128];
	int isecretyear;
	BYTE isecretmonth;
	BYTE isecretday;
	BYTE isecrethour;
	BYTE isecretmin;
	BYTE isecretsec;
	int iid;
	int iequipclasscode;
	int ilocal;
	int ix;
	int iy;
	int ipreviouslocal;
	int ipreviousx;
	int ipreviousy;
	int iequipcode;
	int idetailtype;
	int iparticulartype;
	int ilevel;
	int iseries;
	int iequipversion;
	int irandseed;
	int iparam[16];
	int ilucky;
	int iidentify;
	int	idurability;
	int	iitemstack;
	int iyear;
	BYTE imonth;
	BYTE iday;
	BYTE ihour;
	BYTE imin;
	int	ibind;
	BYTE icansell;
	BYTE icantrade;
	BYTE icandrop;
	int  igoldid;
	int	iitemvalue;
	BYTE ipurpleitem;
	BYTE imantlearmor;
	int iweapontypeitem;
	BYTE iitembroken;
	BYTE ipreviouswidth;
	BYTE ipreviousheight;
	int iitemfortune;
	int ifusionmagic[4];
};

struct TDBTaskData
{
	int		m_nTaskId;
	int		m_nTaskValue;
};

struct TDBSkillData
{
	short	m_nSkillId;
	short	m_nSkillLevel;
	short	m_nSkillExp;
};

#pragma pack( pop )

int get_account(DB *db, const DBT *pkey, const DBT *pdata, DBT *ikey) 
{
	memset( ikey, 0, sizeof( DBT ) );
	TRoleDataNew *pRoleData = (TRoleDataNew *)pdata->data;

	ikey->data = pRoleData->BaseInfo.caccname;
	ikey->size = strlen( pRoleData->BaseInfo.caccname ) + 1;

	return 0;
}

void DBTranslate()
{
	cout<<"Please wait..."<<endl;

	ZDBTable DBOld("database", "roledb");
	if(!DBOld.open())
	{
		cout<<"Can't open old database!";
		return;
	};

	ZDBTable DBNew("database_new", "roledb");
	DBNew.addIndex( get_account );

	if(!DBNew.open())
	{
		cout<<"Can't open new database!";
		return;
	};

	int i = 0;
	char* aBuffer;
	int size;
	char* aKey;
	int Keysize;
	int nTotalKey = 0;
	int nTotalKeyOk = 0;
	int nTotalKeyError = 0;

	aBuffer = DBOld.GetRecord(size, ZDBTable::cpFirst);
	aKey = DBOld.GetRecord_key(Keysize, ZDBTable::cpCurrent);
	while(aBuffer)
	{
		TRoleDataOld* aRoleBuffer = (TRoleDataOld*)aBuffer;

		char* aNewBuffer = new char[64 * 1024];
		int aNewSize = 0;
		TRoleDataNew* aRoleBufferNew = (TRoleDataNew*)aNewBuffer;
		memset(aNewBuffer,0, 64 * 1024);
		
		//Base Info
		aRoleBufferNew->BaseInfo.dwId = aRoleBuffer->BaseInfo.dwId;
		memcpy(aRoleBufferNew->BaseInfo.szName, aRoleBuffer->BaseInfo.szName, 32);
		memcpy(aRoleBufferNew->BaseInfo.caccname, aRoleBuffer->BaseInfo.caccname, 32);
		aRoleBufferNew->BaseInfo.bSex = aRoleBuffer->BaseInfo.bSex;
		aRoleBufferNew->BaseInfo.nFirstSect = aRoleBuffer->BaseInfo.nFirstSect;
		aRoleBufferNew->BaseInfo.nSect = aRoleBuffer->BaseInfo.nSect;
		aRoleBufferNew->BaseInfo.cFightMode = aRoleBuffer->BaseInfo.cFightMode;
		aRoleBufferNew->BaseInfo.cUseRevive = aRoleBuffer->BaseInfo.cUseRevive;
		aRoleBufferNew->BaseInfo.cIsExchange = aRoleBuffer->BaseInfo.cIsExchange;
		aRoleBufferNew->BaseInfo.cPkStatus = aRoleBuffer->BaseInfo.cPkStatus;
		aRoleBufferNew->BaseInfo.ijoincount = aRoleBuffer->BaseInfo.ijoincount;
		aRoleBufferNew->BaseInfo.isectrole = aRoleBuffer->BaseInfo.isectrole;
		aRoleBufferNew->BaseInfo.iexpandrank = aRoleBuffer->BaseInfo.iexpandrank;
		aRoleBufferNew->BaseInfo.igroupcode = aRoleBuffer->BaseInfo.igroupcode;
		aRoleBufferNew->BaseInfo.igrouprole = aRoleBuffer->BaseInfo.igrouprole;
		aRoleBufferNew->BaseInfo.irevivalid = aRoleBuffer->BaseInfo.irevivalid;
		aRoleBufferNew->BaseInfo.irevivalx = aRoleBuffer->BaseInfo.irevivalx;
		aRoleBufferNew->BaseInfo.irevivaly = aRoleBuffer->BaseInfo.irevivaly;
		aRoleBufferNew->BaseInfo.ientergameid = aRoleBuffer->BaseInfo.ientergameid;
		aRoleBufferNew->BaseInfo.ientergamex = aRoleBuffer->BaseInfo.ientergamex;
		aRoleBufferNew->BaseInfo.ientergamey = aRoleBuffer->BaseInfo.ientergamey;
		aRoleBufferNew->BaseInfo.isavemoney = aRoleBuffer->BaseInfo.isavemoney;
		aRoleBufferNew->BaseInfo.imoney = aRoleBuffer->BaseInfo.imoney;
		aRoleBufferNew->BaseInfo.ifiveprop = aRoleBuffer->BaseInfo.ifiveprop;
		aRoleBufferNew->BaseInfo.iteam = aRoleBuffer->BaseInfo.iteam;
		aRoleBufferNew->BaseInfo.ifightlevel = aRoleBuffer->BaseInfo.ifightlevel;
		aRoleBufferNew->BaseInfo.fightexp = aRoleBuffer->BaseInfo.fightexp;
		aRoleBufferNew->BaseInfo.ileadlevel = aRoleBuffer->BaseInfo.ileadlevel;
		aRoleBufferNew->BaseInfo.ileadexp = aRoleBuffer->BaseInfo.ileadexp;
		aRoleBufferNew->BaseInfo.iliveexp = aRoleBuffer->BaseInfo.iliveexp;
		aRoleBufferNew->BaseInfo.ipower = aRoleBuffer->BaseInfo.ipower;
		aRoleBufferNew->BaseInfo.iagility = aRoleBuffer->BaseInfo.iagility;
		aRoleBufferNew->BaseInfo.iouter = aRoleBuffer->BaseInfo.iouter;
		aRoleBufferNew->BaseInfo.iinside = aRoleBuffer->BaseInfo.iinside;
		aRoleBufferNew->BaseInfo.iluck = aRoleBuffer->BaseInfo.iluck;
		aRoleBufferNew->BaseInfo.imaxlife = aRoleBuffer->BaseInfo.imaxlife;
		aRoleBufferNew->BaseInfo.imaxstamina = aRoleBuffer->BaseInfo.imaxstamina;
		aRoleBufferNew->BaseInfo.imaxinner = aRoleBuffer->BaseInfo.imaxinner;
		aRoleBufferNew->BaseInfo.icurlife = aRoleBuffer->BaseInfo.icurlife;
		aRoleBufferNew->BaseInfo.icurstamina = aRoleBuffer->BaseInfo.icurstamina;
		aRoleBufferNew->BaseInfo.icurinner = aRoleBuffer->BaseInfo.icurinner;
		aRoleBufferNew->BaseInfo.ipkvalue = aRoleBuffer->BaseInfo.ipkvalue;
		aRoleBufferNew->BaseInfo.ireputevalue = aRoleBuffer->BaseInfo.ireputevalue;
		aRoleBufferNew->BaseInfo.ifuyuanvalue = aRoleBuffer->BaseInfo.ifuyuanvalue;
		aRoleBufferNew->BaseInfo.irebornvalue = aRoleBuffer->BaseInfo.irebornvalue;
		aRoleBufferNew->BaseInfo.ipassrole = aRoleBuffer->BaseInfo.ipassrole;
		aRoleBufferNew->BaseInfo.ilockpkstate = aRoleBuffer->BaseInfo.ilockpkstate;
		aRoleBufferNew->BaseInfo.ileavetongtime = aRoleBuffer->BaseInfo.ileavetongtime;
		memcpy(aRoleBufferNew->BaseInfo.imatename, aRoleBuffer->BaseInfo.imatename, 32);
		memcpy(aRoleBufferNew->BaseInfo.iplayertitle, aRoleBuffer->BaseInfo.iplayertitle, 64);
		memcpy(aRoleBufferNew->BaseInfo.itongtitle, aRoleBuffer->BaseInfo.itongtitle, 64);
		aRoleBufferNew->BaseInfo.ileftprop = aRoleBuffer->BaseInfo.ileftprop;
		aRoleBufferNew->BaseInfo.ileftfight = aRoleBuffer->BaseInfo.ileftfight;
		aRoleBufferNew->BaseInfo.ileftlife = aRoleBuffer->BaseInfo.ileftlife;
		aRoleBufferNew->BaseInfo.ifinishgame = aRoleBuffer->BaseInfo.ifinishgame;
		aRoleBufferNew->BaseInfo.iarmorres = aRoleBuffer->BaseInfo.iarmorres;
		aRoleBufferNew->BaseInfo.iweaponres = aRoleBuffer->BaseInfo.iweaponres;
		aRoleBufferNew->BaseInfo.ihelmres = aRoleBuffer->BaseInfo.ihelmres;
		aRoleBufferNew->BaseInfo.nSectStat = aRoleBuffer->BaseInfo.nSectStat;
		aRoleBufferNew->BaseInfo.nWorldStat = aRoleBuffer->BaseInfo.nWorldStat;
		aRoleBufferNew->BaseInfo.nKillPeopleNumber = aRoleBuffer->BaseInfo.nKillPeopleNumber;
		aRoleBufferNew->BaseInfo.nForbiddenFlag = aRoleBuffer->BaseInfo.nForbiddenFlag;
		aRoleBufferNew->BaseInfo.dwTongID = aRoleBuffer->BaseInfo.dwTongID;
		aRoleBufferNew->BaseInfo.iavatarrole = aRoleBuffer->BaseInfo.iavatarrole;
		aRoleBufferNew->BaseInfo.iexitemrole = aRoleBuffer->BaseInfo.iexitemrole;
		
		aRoleBufferNew->dwVersion = 0;
		aRoleBufferNew->bBaseNeedUpdate = 1;

		aRoleBufferNew->nFightSkillCount = aRoleBuffer->nFightSkillCount;
		aRoleBufferNew->nLiveSkillCount = aRoleBuffer->nLiveSkillCount;
		aRoleBufferNew->nTaskCount = aRoleBuffer->nTaskCount;
		aRoleBufferNew->nItemCount = aRoleBuffer->nItemCount;
		aRoleBufferNew->nFriendCount = aRoleBuffer->nFriendCount;
		
		aRoleBufferNew->dwFSkillOffset = aRoleBufferNew->pBuffer - (BYTE *)aNewBuffer;

		//Fight Skill
		TDBSkillData * pSkillDataOld = (TDBSkillData *) ((BYTE *)aBuffer + aRoleBuffer->dwFSkillOffset);
		TDBSkillData* pSkillData = (TDBSkillData*) ((BYTE *)aNewBuffer + aRoleBufferNew->dwFSkillOffset);
		for (i = 0 ; i < aRoleBuffer->nFightSkillCount; i++, pSkillDataOld++, pSkillData++)
		{
			int nSkillId = pSkillDataOld->m_nSkillId;
			int nSkillLevel = pSkillDataOld->m_nSkillLevel;
			int nSkillExp = pSkillDataOld->m_nSkillExp;
			pSkillData->m_nSkillId = nSkillId;
			pSkillData->m_nSkillLevel = nSkillLevel;
			pSkillData->m_nSkillExp = nSkillExp;
		}
		aRoleBufferNew->dwLSkillOffset = (BYTE*)pSkillData - (BYTE*)aRoleBufferNew;
		
		//Life Skill
		pSkillDataOld = (TDBSkillData *) ((BYTE *)aBuffer + aRoleBuffer->dwLSkillOffset);
		pSkillData = (TDBSkillData*) ((BYTE *)aNewBuffer + aRoleBufferNew->dwLSkillOffset);
		for (i = 0 ; i < aRoleBuffer->nLiveSkillCount; i++, pSkillDataOld++, pSkillData++)
		{
			int nSkillId = pSkillDataOld->m_nSkillId;
			int nSkillLevel = pSkillDataOld->m_nSkillLevel;
			int nSkillExp = pSkillDataOld->m_nSkillExp;
			pSkillData->m_nSkillId = nSkillId;
			pSkillData->m_nSkillLevel = nSkillLevel;
			pSkillData->m_nSkillExp = nSkillExp;
		}
		aRoleBufferNew->dwTaskOffset = (BYTE*)pSkillData - (BYTE*)aNewBuffer;
		
		//Task
		TDBTaskData * pTaskDataOld = (TDBTaskData *)((BYTE *)aBuffer + aRoleBuffer->dwTaskOffset);
		TDBTaskData* pTaskData = (TDBTaskData*) ((BYTE *)aNewBuffer + aRoleBufferNew->dwTaskOffset);
		for (i = 0; i < aRoleBuffer->nTaskCount; i++, pTaskDataOld++, pTaskData++)
		{
			int nTaskId = pTaskDataOld->m_nTaskId;
			int nTaskValue = pTaskDataOld->m_nTaskValue;
			pTaskData->m_nTaskId = nTaskId;
			pTaskData->m_nTaskValue = nTaskValue;
		}
		aRoleBufferNew->dwItemOffset = (BYTE*)pTaskData - (BYTE*)aNewBuffer;

		TDBItemDataOld * pItemDataOld = (TDBItemDataOld *)((BYTE *)aBuffer + aRoleBuffer->dwItemOffset);
		TDBItemDataNew* pItemData = (TDBItemDataNew*) ((BYTE *)aNewBuffer + aRoleBufferNew->dwItemOffset);
		for (i=0; i<aRoleBufferNew->nItemCount; i++, pItemDataOld++, pItemData++)
		{
			memcpy(pItemData->icontent, pItemDataOld->icontent, 128);
			memcpy(pItemData->iproperty, pItemDataOld->iproperty, 128);
			pItemData->isecretyear = pItemDataOld->isecretyear;
			pItemData->isecretmonth = pItemDataOld->isecretmonth;
			pItemData->isecretday = pItemDataOld->isecretday;
			pItemData->isecrethour = pItemDataOld->isecrethour;
			pItemData->isecretmin = pItemDataOld->isecretmin;
			pItemData->isecretsec = pItemDataOld->isecretsec;
			pItemData->iid = pItemDataOld->iid;
			pItemData->iequipclasscode = pItemDataOld->iequipclasscode;
			pItemData->ilocal = pItemDataOld->ilocal;
			pItemData->ix = pItemDataOld->ix;
			pItemData->iy = pItemDataOld->iy;
			pItemData->ipreviouslocal = pItemDataOld->ipreviouslocal;
			pItemData->ipreviousx = pItemDataOld->ipreviousx;
			pItemData->ipreviousy = pItemDataOld->ipreviousy;
			pItemData->iequipcode = pItemDataOld->iequipcode;
			pItemData->idetailtype = pItemDataOld->idetailtype;
			pItemData->iparticulartype = pItemDataOld->iparticulartype;
			pItemData->ilevel = pItemDataOld->ilevel;
			pItemData->iseries = pItemDataOld->iseries;
			pItemData->iequipversion = pItemDataOld->iequipversion;
			pItemData->irandseed = pItemDataOld->irandseed;
			for (int j = 0; j < 16; j++)
			{
				pItemData->iparam[j] = pItemDataOld->iparam[j];
			}
			pItemData->ilucky = pItemDataOld->ilucky;
			pItemData->iidentify = pItemDataOld->iidentify;
			pItemData->idurability = pItemDataOld->idurability;
			pItemData->iitemstack = pItemDataOld->iitemstack;
			pItemData->iyear = pItemDataOld->iyear;
			pItemData->imonth = pItemDataOld->imonth;
			pItemData->iday = pItemDataOld->iday;
			pItemData->ihour = pItemDataOld->ihour;
			pItemData->imin = pItemDataOld->imin;
			pItemData->ibind = pItemDataOld->ibind;
			pItemData->icansell = pItemDataOld->icansell;
			pItemData->icantrade = pItemDataOld->icantrade;
			pItemData->icandrop = pItemDataOld->icandrop;
			pItemData->igoldid = pItemDataOld->igoldid;
			pItemData->iitemvalue = pItemDataOld->iitemvalue;
			pItemData->ipurpleitem = pItemDataOld->ipurpleitem;
			pItemData->imantlearmor = pItemDataOld->imantlearmor;
			pItemData->iweapontypeitem = pItemDataOld->iweapontypeitem;
			pItemData->iitembroken = pItemDataOld->iitembroken;
			pItemData->ipreviouswidth = pItemDataOld->ipreviouswidth;
			pItemData->ipreviousheight = pItemDataOld->ipreviousheight;
			pItemData->iitemfortune = pItemDataOld->iitemfortune;
			for (i = 0; i < 4; i++)
			{
				pItemData->ifusionmagic[i] = pItemDataOld->ifusionmagic[i];
			}
		}
		aRoleBufferNew->dwFriendOffset = (BYTE *)pItemData - (BYTE*)aNewBuffer;
		
		aNewSize = sizeof(TRoleDataNew) + sizeof(TDBSkillData) * aRoleBufferNew->nFightSkillCount + sizeof(TDBTaskData) * aRoleBufferNew->nTaskCount + sizeof(TDBSkillData) * aRoleBufferNew->nLiveSkillCount + sizeof(TDBItemDataNew) * aRoleBufferNew->nItemCount - 1;
		//aNewSize = (BYTE *)pItemData - (BYTE*)aNewBuffer;
		aRoleBufferNew->dwDataLen = aNewSize;

		if(!DBNew.add(aKey,Keysize,aNewBuffer,aNewSize))
		{
			cout<<endl<<"Can't add key: "<<aKey<<" to the database!"<<endl;
			nTotalKeyError++;
		}
		else
		{
			nTotalKeyOk++;
		}
		nTotalKey++;
		
		delete [] aNewBuffer;
		delete [] aBuffer;
		delete [] aKey;

		aBuffer = DBOld.GetRecord(size,ZDBTable::cpNext);
		aKey = DBOld.GetRecord_key(Keysize, ZDBTable::cpCurrent);
	}

	DBOld.close();
	DBNew.close();

	cout<<"Convert Database Successful!!!"<<endl;
	cout<<"Total key: "<<nTotalKey<<endl;
	cout<<"Success: "<<nTotalKeyOk<<endl;
	cout<<"Error: "<<nTotalKeyError<<endl;
}

int main(int argc, char **argv)
{
	system("color 1E");
	system("title Database Translate By Minh Kiem (c) 2014");
	cout<<"Author: Minh Kiem"<<endl;
	cout<<"Coder: Minh Kiem"<<endl;
	char charin;
	do 
	{
		cout<<"Please press 'y' to continue: ";
		cin>>charin;
	} while (charin != 'y');
	
	DBTranslate();

	system("pause");
	return 0;
}