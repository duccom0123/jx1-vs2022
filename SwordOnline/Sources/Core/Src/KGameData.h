#ifndef KGameDataH
#define	KGameDataH

#define 	MAX_DATA_INT				100

enum GAMEDATA
{
	GameDataInt,
	GameDataGr,
	GameDataNum,
};

class KGameData
{
public:
#ifdef	_SERVER
	char 			m_szSave[MAX_DATA_INT][16];
	KDataGroup		m_sDataGroup[MAX_PLAYER];
	int 			m_nNumGroup;

public:
	KGameData();
	BOOL	Init();
	
	int 	GetDataInt(int nData);
	const char* 	GetDataStr(int nData);
	void 	SetData(int nNo, char* szValue);
	int		AddDataGr(KDataGroup* pInfo);
	void	SetDataGr(int nGroup, KDataGroup* pInfo);
	int		FindFreeDataGr();
	int		FindDataId(DWORD dwId);
	int 	GetDataGr_dwName(int nNo);
	int 	GetDataGr_nValue(int nNo, int nIndex);
	char* 	GetStrDataGr_Name1(int nNo);
	char* 	GetStrDataGr_Name2(int nNo);
	BOOL	InitGameData();
	BOOL	Save();
#endif
};

extern KGameData GameData;
#endif //KGameDataH
