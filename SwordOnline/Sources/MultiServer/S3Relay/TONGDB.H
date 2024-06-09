// TongDB.h: interface for the CTongDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_)
#define AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "DBTable.h"
#include "KTongControl.h"

/*
enum enumMemberClass		//��������
{
	mcMaster = 0,			//����
	mcDirector,			//����
	mcManager,			//����
	mcMember			//����
};
*/

typedef struct
{
	int			nCamp;								// �����Ӫ
	DWORD		dwMoney;								// ����ʽ�
	int			nCredit;								// �������
	BYTE		btLevel;
	DWORD		dwTotalEff;
	int			nSaveEff;
	int		    nTongJiyuParam;
	char 		szTongJiyuNotify[defTONG_NOTIFY_LENGTH];
	BOOL 		bRecruit;
	int 		nTongParam;

	char		szName[defTONG_NAME_LENGTH_32];			// �����
	char		szMasterAgname[defTONG_NAME_LENGTH_32];	// �����ƺ�
	char		szDirectorAgname[defTONG_MAX_DIRECTOR][defTONG_NAME_LENGTH_32];// �����ϳƺ�
	char		szManagerAgname[defTONG_MAX_MANAGER][defTONG_NAME_LENGTH_32];	// ���ӳ��ƺ�
	char		szNormalAgname[defTONG_NAME_LENGTH_32];	// �����ڳƺ�
}TTongStruct;	//�����������ݿ�Ľṹ

typedef struct
{
	char		szName[defTONG_NAME_LENGTH_32];			// ������
	char		szTong[defTONG_NAME_LENGTH_32];		// �����
	TONG_MEMBER_FIGURE		MemberClass;			//��������
	int			nTitleIndex;					//���ڳƺ�����
	int			nSex;
	int 		nJoinTm;
	int 		nSaveEff;
}TMemberStruct;	//�����������ݿ�Ľṹ

class CTongDB  
{
private:
	ZDBTable* m_TongTable;						//������ݱ�
	ZDBTable* m_MemberTable;					//�������ݱ�
	// ZDBTable* m_MoneyTable;
	
	TTongList m_TongList[MAX_TONG_COUNT];	//����б�
	int m_TongListCount;						//����б���
public:
	CTongDB();
	virtual ~CTongDB(){};

	bool Open();
	bool OpenNew();
	void Close();
	
	bool ChangeTong(CTongControl& aTongControl);	//�����͸ı����
	bool DelTong(char* aTongName);	//ɾ������
	bool SearchTong(char* aTongName, CTongControl& aTongControl);	//�����ݿ���ȡ�ð���
	int GetTongCount();							//ȡ��������
	int GetTongList(TTongList* aTongList, int aMaxNum);	//ȡ�ð���б�

	bool ChangeMember(TMemberStruct aMember);//���Ӱ���
	bool DelMember(char* aMemberName);		//ɾ������
};

#endif // !defined(AFX_TONGDB_H__8C487F07_C490_4562_AD01_77953CCCBCE2__INCLUDED_)