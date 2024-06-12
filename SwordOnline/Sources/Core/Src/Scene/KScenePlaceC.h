// *****************Editer	: duccom0123 EditTime:	2024/06/12 11:48:45*********************
// ������ͼ���ͻ��˰棩
// Copyright : Kingsoft 2002
// Author    : wooy(wu yue)
// CreateTime: 2002-11-11
// ---------------------------------------------------------------------------------------
// ***************************************************************************************

#ifndef KSCENEPLACEC_H_HEADER_INCLUDED_C22D00B5
#define KSCENEPLACEC_H_HEADER_INCLUDED_C22D00B5
#include "../../Represent/iRepresent/KRepresentUnit.h"
#include "KScenePlaceRegionC.h"
#include "SceneDataDef.h"
#include "KIpoTree.h"
#include "ScenePlaceMapC.h"

// ��������������
/*
�ӣ���23��00��1��00��RGB1
�󣺣�1��00��3��00��RGB1��RGB2
������3��00��5��00��RGB2
î����5��00��7��00��RGB2��RGB3
������7��00��9��00��RGB3��RGB4
�ȣ���9��00��11��00��RGB4��RGB5
�磺��11��00��13��00��RGB5
δ����13��00��15��00��RGB5��RGB4
�꣺��15��00��17��00��RGB4��RGB6
�ϣ���17��00��19��00��RGB6��RGB7
�磺��19��00��21��00��RGB7��RGB1
������21��00��23��00��RGB1
*/
#define ENVLIGHT_MIDNIGHT	0	//RGB1����ҹ�ĺڰ�
#define ENVLIGHT_DAWN		1	//RGB2������ǰ��Щ�����ĺڰ�
#define ENVLIGHT_MORNING	2	//RGB3��̫���ճ���ʱ������
#define ENVLIGHT_FORENOON	3	//RGB4����ͼ����������
#define ENVLIGHT_NOON		4	//RGB5������ĸ���
#define ENVLIGHT_DUSK		5	//RGB6��̫����ɽ����ϼ����
#define ENVLIGHT_EVENING	6	//RGB7�����������ɵ�����
#define MAX_BACKGROUND_IMAGE 3
#define MAX_CLOUD_IMAGE		6
enum
{
	BACKGROUND_IMAGE = 1,
	OVER_CLOUD_IMAGE,
	BACKGROUND_CLOUD_IMAGE,
};
class EnvironmentLight
{
private:
	KLColor m_cLight[7];
public:
	EnvironmentLight();
	// ���õ�nIdx����ɫ
	void SetLight(const KLColor &cLight, int nIdx);
	// ���õ�nIdx����ɫ
	void SetLight(BYTE r, BYTE g, BYTE b, int nIdx);
	// ��������7����ɫ
	void SetLight(KLColor *pLight);
	// ȡ�þ�һ�쿪ʼnMinutes����ʱ�Ļ�������ɫ
	DWORD GetEnvironmentLight(int nMinutes);
};

//================================================================
//	������һ�����򱻼�����Ϻ�Ļص������ĺ���ԭ��
//================================================================
//	����: nRegionH, nRegionV �ֱ�Ϊ������ϵ�����ĺ�������������
typedef void (*funScenePlaceRegionLoadedCallback)(int nRegionH, int nRegionV);

class KWeather;


#define KMAX_PREV_LOAD_POS_NUM  5
typedef struct _KPrevLoadPosItem
{
    int     m_nNum;
    POINT   m_Pos[KMAX_PREV_LOAD_POS_NUM];

} KPrevLoadPosItem;

#define MAX_PREV_LOAD_FILE_COUNT    1024*2
#define PREV_LOAD_FILE_MIN_SIZE     (32 * 1024)

typedef struct _KPrevLoadFileNameAndFrameAndFrame
{
    char szName[MAX_RESOURCE_FILE_NAME_LEN];
    int  nFrame;
} KPrevLoadFileNameAndFrame;



//typedef struct _KPrevLoadItem
//{
//    POINT
//
//} KPrevLoadItem;


//##ModelId=3DB8F8B40128
//##Documentation
//## ������һ����ͼ��
class KScenePlaceC
{
  public:
	//##ModelId=3DBE3B53008C
	//##Documentation
	//## ���캯����
	//## ������һ�����������m_szPlaceRootPath[0] = 0
	KScenePlaceC();

	//##ModelId=3DD17A770383
	//##Documentation
	//## call Terminate
	~KScenePlaceC();

	//##ModelId=3DCAA6A703DB
	//##Documentation
	//## ��ʼ��
	bool Initialize();

	//##ModelId=3DCD58AC00BC
	//##Documentation
	//## ���������ܡ��ͷŶ����ȫ�������붯̬�������Դ��
	void Terminate();

	//##ModelId=3DCAA64C01DA
	//##Documentation
	//## ����/����ָ���ĳ�����ͼ��
	bool OpenPlace(
		//##Documentation
		//## ������ͼ����
		int nPlaceIndex);

	//##ModelId=3DCAAE3703A6
	//##Documentation
	//## �رճ�����ͼ��
	void ClosePlace();

	//##ModelId=3DCD7F0A0071
	//##Documentation
	//## ���Ƴ���
	void Paint();

	//##ModelId=3DBCE7B70358
	//##Documentation
	//## ���ó�����ͼ�Ľ���
	void SetFocusPosition(
		//##Documentation
		//## �������������x����
		int nX, 
		//##Documentation
		//## �������������y����
		int nY, 
		//##Documentation
		//## �������������z����
		int nZ);

	void GetFocusPosition(
		//##Documentation
		//## �������������x����
		int& nX, 
		//##Documentation
		//## �������������y����
		int& nY, 
		//##Documentation
		//## �������������z����
		int& nZ);

//----���ڲ������ڳ�����ͼ�Ķ����볡����ϵ�Ĳ���----

	//##ModelId=3DCAA6B90196
	//##Documentation
	//## ��������ʱ����(�����ڳ����е��ǲ������ڳ�����ͼ�Ķ���)�������С�
	//## �������Ӷ������ӵ������л�õı�ǡ��������0ֵ��ʾ���Ӷ���ʧ�ܡ����߶���û�в��ڳ������ڳ�����ǰ�Ĵ��������У����Ӳ��������ԡ�
	unsigned int AddObject(
		//##Documentation
		//## Ҫ����Ķ���������
		unsigned int uGenre, 
		//##Documentation
		//## Ҫ����Ķ����id
		int nId, 
		//##Documentation
		//## �����λ�����ꡣ
		int x, int y, int z,
		int eLayerParam = IPOT_RL_OBJECT);

	//##ModelId=3DCAA7000085
	//##Documentation
	//## �ƶ������ڳ����е��ǲ������ڳ�����ͼ�Ķ����ڳ����е�λ�á�
	//## ��������ƶ��ɹ��򷵻ض����ڳ�����ͼ�еı����ֵ��Ϊ��0ֵ�����ʧ���򷵻�0����������Ƴ��˳�����ǰ����������Ҳ����0��
	unsigned int MoveObject(
		//##Documentation
		//## Ҫ�ƶ��Ķ���������
		unsigned int uGenre, 
		//##Documentation
		//## Ҫ�ƶ��Ķ����id
		int nId, 
		//##Documentation
		//## �����Ŀ�����ꡣ
		int x, int y, int z, 		 
		//##Documentation
		//## �ƶ�ǰ�����ڳ����еı����ֵ������������¼��룬����0ֵ��
		unsigned int& uRtoid,
		int eLayerParam = IPOT_RL_OBJECT);

	//##ModelId=3DCAA70603E3
	//##Documentation
	//## ȥ�������ڳ����е��ǲ������ڳ�����ͼ�Ķ���
	void RemoveObject(
		//##Documentation
		//## Ҫȥ���Ķ���������
		unsigned int uGenre, 
		//##Documentation
		//## Ҫȥ���Ķ����id
		int nId, 
		//##Documentation
		//## �������ǰ�ڳ����еı����ֵ��
		unsigned int& uRtoid);

	void Breathe();
	
	void SetRepresentAreaSize(int nWidth, int nHeight);

	void ProjectDistToSpaceDist(int& nXDistance, int& nYDistance);

	//��ͼ/��ͼ�豸���� ת��Ϊ�ռ�����
	void ViewPortCoordToSpaceCoord(
		int& nX,	//���룺��ͼ/��ͼ�豸�����x�����������ռ������x��
		int& nY,	//���룺��ͼ/��ͼ�豸�����y�����������ռ������y��
		int  nZ		//���������õ��Ŀռ������z��
		);
	void ViewPortCoordToSpaceCoordNew(
		int& nW,
		int& nX,	//���룺��ͼ/��ͼ�豸�����x�����������ռ������x��
		int& nY,	//���룺��ͼ/��ͼ�豸�����y�����������ռ������y��
		int  nZ		//���������õ��Ŀռ������z��
		);	
	void GetRegionLeftTopPos(int nRegionX, int nRegionY, int& nLeft, int& nTop);
	
	long GetObstacleInfo(int nX, int nY);
	long GetObstacleInfoMin(int nX, int nY, int nOffX, int nOffY);

	void RepresentShellReset();


	//���ó�����һ�����򱻼�����Ϻ�Ļص�����
	void	SetRegionLoadedCallback(funScenePlaceRegionLoadedCallback pfunCallback);
	
	void	SetHightLightSpecialObject(int nRegionX, int nRegionY, int nBioIndex);
	void	UnsetHightLightSpecialObject();

	void	GetSceneNameAndFocus(char* pszName, int& nId, int& nX, int& nY);
	void	GetSceneNameAndFocusOften(char* pszName, int& nId, int& nX, int& nY);

	// ���û��������ɫ������
	void SetAmbient(DWORD dwAmbient);
	void SetCurrentTime(DWORD dwCurrentTime);
	// ����24Сʱ������仯����ĵ�idx����Ԫ��idxȡֵ0��6
	void SetEnvironmentLight(int idx, BYTE r, BYTE g, BYTE b);

	// �����Ƿ�����ͼ�ڽ���Դ�����ڱ��ְ��������
	void EnableBioLights(bool bEnable);
	// �����Ƿ�����̬����
	void EnableDynamicLights(bool bEnable);
	
	void ChangeWeather(int nWeatherID);

	//��ȡ������С��ͼ��Ϣ
	int GetMapInfo(KSceneMapInfo* pInfo);
	//���õ�ͼ����
	void SetMapParam(unsigned int uShowElems, int nSize);
	//���ó����ĵ�ͼ�Ľ���(��λ:��������)
	void SetMapFocusPositionOffset(int nOffsetX, int nOffsetY);
	//С��ͼ����
	void  PaintMap(int nX, int nY);
	//�����Ƿ�����ͼ���ƶ����ƶ�
	void FollowMapMove(int nbEnable);

	BOOL OnDirectMap(int nX, int nY);

	void DoDirectMap(int nX, int nY);

	void EnableWeather(int nbEnable);

	//////////////////////////////////////////////////////////////////////////
	bool	m_bBGImg;
	int		m_nBGNum;
	RECT	BGArea[MAX_BACKGROUND_IMAGE];
	bool	m_bOCImg;
	int		m_nOCNum;
	RECT	OCArea;
	bool	m_bBCImg;
	int		m_nBCNum;
	RECT	BCArea;

	void LoadSymbol(int nSubWorldID);
	bool bFlagMode;
	bool bPaintMode;
	void DirectFindPos(int nX, int nY, BOOL bSync, BOOL bPaintLine);
	void LoadIni(KIniFile *pIni);
	
	BOOL PaintBackGround();
	void PaintOverCloud();
	void PaintBackGroundCloud();
	//////////////////////////////////////////////////////////////////////////

private:

	//##ModelId=3DCE68BB0238
	//##Documentation
	//## �ı���ط�Χ��
	void ChangeLoadArea();

	//##ModelId=3DBF946D0053
	//##Documentation
	//## �ȶ�ȫ���뿪������Χ��������󣬵���FreePrerender�������ͷ���Щ�������Ԥ��Ⱦ�õĴ��ŵر�ͼ��Ȼ������Ԥ�����źš�
	void ChangeProcessArea();

	//##ModelId=3DBFA1460230
	//##Documentation
	//## ����������Χ�ڵ���������Ԥ������
	void Preprocess();

	//##ModelId=3DCCBD7B0239
	//##Documentation
	//## ���/�ͷ�Ԥ������������������Ϣ��
	void ClearPreprocess(int bIncludeRto);

	//##ModelId=3DBDBC7200B4
	//##Documentation
	//## �����µ�Ҫ���ص�����
	void SetRegionsToLoad();

	//##ModelId=3DCB6BC90345
	//##Documentation
	//## �����������ݵĹ��̣��������ݼ��ͼ��
	//## Use Case View/����/�ͻ���/������ͼ����/State/Activity Model/��������Ļ��� �����߳̿ռ��ִ������
	void LoadProcess();

	//##ModelId=3DCCD131018C
	//##Documentation
	//## �����̵߳�ִ����ں�����
	//## ��pParam��ΪKScenePlaceC�����ָ�룬��������LoadAndPreprocess������
	//## ����ֵ�̶�Ϊ0��
	static DWORD WINAPI LoadThreadEntrance(void* pParam);

	void PrerenderGround(bool bForce);

	void ARegionLoaded(KScenePlaceRegionC* pRegion);
	
	KRUImage* GetFreeGroundImage();

	KScenePlaceRegionC*	GetLoadedRegion(int h, int v);

	void SetLoadingStatus(bool bLoading);

    void PreLoadProcess();

private:

	//##ModelId=3DCAC1D103D7
	//##Documentation
	//## KScenePlaceC���в���
	enum SP_WORKING_PARAM { 
		//##Documentation
		//## (��̬)���ط�Χ����/���������������Ĳ໹Ҫ������ķ�Χ��(��λ������)
		//## ��ȡֵ����(���ط�Χ��/�����Խ���������Ŀ-1)/2
		SPWP_LOAD_EXTEND_RANGE = 3,
		//##Documentation
		//## �����ص��������Ŀ
		SPWP_MAX_NUM_REGIONS = 49,
		//##Documentation
		//## ����ܹ����봦����Χ���������Ŀ
		SPWP_NUM_REGIONS_IN_PROCESS_AREA = 49,
		//##Documentation
		//## �����ƶ�������������Խ���ٸ�����֮�����趯̬���ط�Χ
		SPWP_TRIGGER_RANGE = 2,
		//## �����ƶ������������ͻȻ��Խ���ٸ�����ᴥ�����������״̬
		SPWP_TRIGGER_LOADING_RANGE = 4,
		//##Documentation
		//## �����Ŀ��
		SPWP_PROCESS_RANGE = 3,
		//##Documentation
		//## ��ʾһ��ңԶ������ֵ��Զ����Ч�����귶Χ��
	    SPWP_FARAWAY_COORD = -2147476129,

		SPWP_REPRESENT_RECT_WINDAGE_X = 140,
		SPWP_REPRESENT_RECT_WINDAGE_T = 200,
		SPWP_REPRESENT_RECT_WINDAGE_B = 200,
		//##Documentation
		//##�ܴ���Ԥ�ȴ�����Ԥ��Ⱦ���Ѽ���������Ŀ
		SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER = 25,
		//##Documentation
		//##�޼�����ʾ�Ķ���ı�־
		SPWP_NO_HL_SPECAIL_OBJECT = -1,
		//##Documentation
		//##�л���ͼ���صĳ�ʱʱ��(��λ������)
        SPWP_SWITCH_SCENE_TIMEOUT = 30000,
		//##Documentation
		//##rto����Ĵ�Լһ���/�߶�
        SPWP_RTO_HALF_RANGE = 80,
		//û�г���
		SPWP_NO_SCENE = -1,
	};

private:

	//������һ�����򱻼�����Ϻ�Ļص�������ָ�롣
	funScenePlaceRegionLoadedCallback m_pfunRegionLoadedCallback;

	//##ModelId=3DD2CA84021C
	//##Documentation
	//## ��־�����Ƿ��Ѿ��ɹ�ִ�ر���ʼ���ˡ�
	bool	m_bInited;

	//## ���ڼ�����
	bool	m_bLoading;

	bool	m_bEnableWeather;

	//�Ƿ�����ͼ���ƶ����ƶ�
	bool	m_bFollowWithMap;

	//ԭʼ�Ľ������꣬��ֵֻ��m_bFollowWithMapΪ��ֵʱ��Ч
	POINT	m_OrigFocusPosition;

	//��ͼ���������ƫ��
	POINT	m_MapFocusOffset;

	//##ModelId=3DCE5BF203D6
	//##Documentation
	//## ��������,�˵���z=0ƽ���ϡ�
	POINT m_FocusPosition;

	//##ModelId=3DCD42F60221
	//##Documentation
	//## �������ڵ�����������ݺ���������
	POINT m_FocusRegion;

	//##ModelId=3DD3B98002E1
	//##Documentation
	//## ��ǰ�Ľ������ڵ������������ɵĽ������ڵ����������ֵ�ӵĲ�ֵ��
	//## ���ۼƽ����ƶ�����������Խ'����'����Ŀ��
	SIZE m_FocusMoveOffset;

	//##ModelId=3DCB792F02D7
	//##Documentation
	//## ��������������źţ���Ƽ����źţ�
	HANDLE m_hLoadRegionEvent;

	//## �л���ͼ���ؽ������ź�
	HANDLE m_hSwitchLoadFinishedEvent;

	//##ModelId=3DCD478001BA
	//##Documentation
	//## ����Ԥ����������ź�(���Ԥ�����ź�)
	int	m_bPreprocessEvent;

	//##ModelId=3DCB83F30160
	//##Documentation
	//## ���������ݴ���/�������ٽ�����
	CRITICAL_SECTION m_ProcessCritical;

	//##ModelId=3DCAB77A01E9
	//##Documentation
	//## ���س��������޸Ĺؼ����ݵ��ٽ�������
	CRITICAL_SECTION m_LoadCritical;

	//##ModelId=3DCB84480342
	//##Documentation
	//## ����m_RegionListAdjustCritical���ٽ�����
	CRITICAL_SECTION m_RegionListAdjustCritical;


	//##ModelId=3DB907ED02B7
	//##Documentation
	//## ���̶���һ����Ŀ��Region����ʵ����
	KScenePlaceRegionC m_RegionObjs[SPWP_MAX_NUM_REGIONS];

	//##ModelId=3DDBC73803A4
	//##Documentation
	//## ���봦����Χ���Լ��������ָ�롣
	KRUImage m_pBGImg[MAX_BACKGROUND_IMAGE];
	KRUImage m_pBCImg[MAX_CLOUD_IMAGE];
	KRUImage m_pOCImg[MAX_CLOUD_IMAGE];
	KScenePlaceRegionC* m_pInProcessAreaRegions[SPWP_NUM_REGIONS_IN_PROCESS_AREA];

	KRUImage	m_RegionGroundImages[SPWP_NUM_REGIONS_IN_PROCESS_AREA];
	int			m_nNumGroundImagesAvailable;

	//##ModelId=3DCAB33A01CE
	//##Documentation
	//## ������ͼ�ļ��ĸ�Ŀ¼���߳�����ͼ���ļ����ڵ�Ŀ¼����
	//## KScenePlaceC����m_szPlaceRootPath[0]�Ƿ�Ϊ0����Ϊ�Ƿ�򿪳�����ͼ���ж����ݡ�
	char m_szPlaceRootPath[80];
	char m_szSceneName[80];
	int	 m_nSceneId;
	//##ModelId=3DD2E1410028
	//##Documentation
	//## m_pRegions�б������������m_nFirstToLoadIndex��ֵ����Ч��Χ�ڣ����ʾm_pRegions�б���m_nFirstToLoadIndex����֮ǰ��Ԫ��Ϊ�Ѿ��������������ݵ���������ָ�룬ʣ�ڵ�Ԫ��Ϊ���������ݵ���������ָ�롣���m_nFirstToLoadIndex��ֵ������Ч��Χ�����ʾ�б���ȫ��ָ������������������Ѿ���������������ݡ�
	int m_nFirstToLoadIndex;

	//##ModelId=3DD2E1410064
	//##Documentation
	//## �������ָ���б���
	//## �б��е�ÿ��Ԫ�����ݶ���һ����������ָ�룬��Щָ��ֱ����ָ��m_RegionObjs�е�һ��Ԫ�أ���һһ��Ӧ�Ĺ�ϵ���������ָ���б��и�Ԫ�ص�˳���������㷢���仯��
	KScenePlaceRegionC* m_pRegions[SPWP_MAX_NUM_REGIONS];

	//##ModelId=3DD3A6140179
	//##Documentation
	//## ִ�м�������������Ԥ�������̵߳ľ����
	HANDLE m_hLoadAndPreprocessThread;

	//##ModelId=3DD960150394
	//##Documentation
	//## ���ڴ�����Χ�ڵĲ��ֶ���ļ��ϡ�
	KIpoTree m_ObjectsTree;

	RECT m_RepresentArea;
	SIZE m_RepresentExactHalfSize;
	
	long m_bRenderGround;

	int		m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY;
	int		m_nHLSpecialObjectBioIndex;

	KBuildinObj**	m_pObjsAbove;
	unsigned int	m_nNumObjsAbove;

	// ��ǰʱ�䣬һ�쿪ʼ�ķ�����
	int m_nCurrentTime;

	// ����������Ķ���
	EnvironmentLight m_EnLight;
	// ��������ָ��
	KWeather *m_pWeather;
	//С��ͼ
	KScenePlaceMapC	m_Map;

private:
	//##ModelId=3DDB39BA029B
	//##Documentation
	//## ���������������ȴ���ı�
	static int m_PRIIdxTable[SPWP_MAX_NUM_REGIONS];

	//##ModelId=3DDB39150334
	//##Documentation
	//## ������ط�Χ�ڳ�����������������õı�
	static POINT m_RangePosTable[SPWP_MAX_NUM_REGIONS];

	// �������ǰ���ķ�����Ҫ���ص������ҵ�ǰRegion��ƫ��
    static const KPrevLoadPosItem m_PrevLoadPosOffset[3][3];

	// �������ǰ���ķ�����Ҫ���ص�Region
    KPrevLoadPosItem    m_PreLoadPosItem;

    int                 m_nPrevLoadFileCount;
    KPrevLoadFileNameAndFrame   m_PrevLoadFileNameAndFrames[MAX_PREV_LOAD_FILE_COUNT];

};

extern KScenePlaceC	g_ScenePlace;
#endif /* KSCENEPLACEC_H_HEADER_INCLUDED_C22D00B5 */
