#ifndef KTongDataH
#define	KTongDataH

struct KTongMap
{
	char szName[80];
	char szPath[MAX_PATH];
};

class KTongLevelAdd
{
private:
	int		m_nExpVal[MAX_TONG_LEVEL];				// 升级经验
	int		m_nEffVal[MAX_TONG_LEVEL];			// 每个系升级加生命点
	int		m_nMemberNum[MAX_TONG_LEVEL];			// 每个系升级加生命点
public:
	KTongLevelAdd();
	BOOL		Init();
	int			GetExp(int nLevel);	
	int			GetEff(int nLevel);			// 传入等级获得可带队员数
	int			GetMemNum(int nLevel);			// 传入经验获得可带队员数
};

class KOneTong
{
private:
	DWORD		m_dwNameID;
	KTongMap	m_sMap;

public:
	KOneTong();
	BOOL		Init();
	BOOL		AddTong();
};


class KTongData
{
private:
#ifdef	_SERVER
	int						m_nNumTong;
public:
	KOneTong				m_sTong[MAX_TONG_COUNT];
#endif

public:
	KTongParam				m_sTongParam;
	KTongLevelAdd			m_sTongLevel;

	KTongData();
	BOOL	Init();
};

extern KTongData TongData;
#endif //KTongDataH
