//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KObj.h
// Date:	2002.01.06
// Code:	边城浪子
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KObj_H
#define KObj_H

#ifdef	_SERVER
#define	MAX_OBJECT	20000
#else
#define	MAX_OBJECT	512
#endif

#include "KPolygon.h"

#ifndef _SERVER
#include "KSprite.h"
#include "KWavSound.h"
#include "KSprControl.h"
#endif

#include "KIniFile.h"
#include "GameDataDef.h"
#ifndef	_SERVER
#include "../../Represent/iRepresent/KRepresentUnit.h"
#endif
#define		OBJ_BAR_WIDTH		3								// 物件障碍 长
#define		OBJ_BAR_HEIGHT		5								// 物件障碍 宽
#define		OBJ_BAR_SIZE		(OBJ_BAR_WIDTH*OBJ_BAR_HEIGHT)	// 物件障碍 长x宽

#define		OBJ_BOX_STATE_CLOSE		0		// 宝箱状态 关
#define		OBJ_BOX_STATE_OPEN		1		// 宝箱状态 开
#define		OBJ_DOOR_STATE_CLOSE	0		// 门状态 关
#define		OBJ_DOOR_STATE_OPEN		1		// 门状态 开
#define		OBJ_PROP_STATE_DISPLAY	0		// 道具状态 关 不显示
#define		OBJ_PROP_STATE_HIDE		1		// 道具状态 开 显示

#define		OBJ_BELONG_TIME		600

enum	Obj_Bar_Type			// 物件地面格子障碍类型
{
	Obj_Bar_Empty = 0,			// 空，无障碍
	Obj_Bar_Full,				// 完整障碍
	Obj_Bar_LT,					// 左上区域为障碍
	Obj_Bar_RT,					// 右上区域为障碍
	Obj_Bar_LB,					// 左下区域为障碍
	Obj_Bar_RB,					// 右下区域为障碍
	Obj_Bar_Type_Num,			// 障碍类型数
};

typedef struct SOBJ_LIGHT_DATA
{
	int			m_nRadius;			// 物件的光亮度
	int			m_nRed;				// 红偏色
	int			m_nGreen;			// 绿偏色
	int			m_nBlue;			// 蓝偏色
	int			m_nAlpha;			// alpha 值
	int			m_nReflectType;		// 受光方式（不受光、重心、水平、左斜、右斜）
	SOBJ_LIGHT_DATA() {Release();};
	void	Release() {	m_nRadius = 0; m_nRed = 0; m_nGreen = 0; m_nBlue = 0; m_nAlpha = 0; m_nReflectType = 0; };
} OBJ_LIGHT;		// 物件光线相关

//---------------------------------------------------------------------------

#ifdef _SERVER
class ServerImage
{
public:
	int			m_nTotalFrame;						// 总帧数
	int			m_nCurFrame;						// 当前帧
	int			m_nTotalDir;						// 总方向数
	int			m_nCurDir;							// 当前方向
	DWORD		m_dwTimer;							// 时间计数器
	DWORD		m_dwInterval;						// 帧间隔（动画速度）

protected:
	int			m_nDirFrames;						// 每方向帧数

public:
	ServerImage();									// 构造函数
	void		Release();							// 清空
	void		SetTotalFrame(int nTotalFrame);		// 设定总帧数
	void		SetCurFrame(int nCurFrame);			// 设定当前帧
	void		SetTotalDir(int nTotalDir);			// 设定总方向数
	BOOL		SetCurDir(int nDir);				// 设定当前方向
	void		SetInterVal(DWORD dwInterval);		// 设定帧间隔
	int			GetOneDirFrames();					// 获得单方向帧数

	BOOL		GetNextFrame(BOOL bLoop = TRUE);	// 自动向后换帧
	BOOL		GetPrevFrame(BOOL bLoop = TRUE);	// 自动向前换帧
	BOOL		CheckEnd();							// 判断动画是否播放到最后
	void		SetDirStart();						// 设定当前方向的当前帧为第一帧
	void		SetDirEnd();						// 设定当前方向的当前帧为最后一帧
	BOOL		SetCurDir64(int nDir);				// 设定当前方向(方向需从64方向转换到真正的方向)
	int			GetCurDirFrameNo();
};
#endif
#ifdef TOOLVERSION
class CORE_API KObj
#else
class KObj
#endif
{
	friend class KObjSet;
public:
	int			m_nID;					// 物件的世界唯一ID，客户端与服务器端一一对应，如果值为 0 ，此为客户端 obj ，服务器端没有对应的 obj
	int			m_nDataID;				// 此物件在物件数据表中的位置(就是在模板中的编号)
	int			m_nKind;				// 类型	对应 Obj_Kind_XXXX
	int			m_nIndex;				// 在 Object 数组中的位置
	int			m_nBelongRegion;		// 本物体属于哪个region，不是坐标，坐标可以变动，这个是随产生时那个region，客户端存储的是 region id

	int			m_nSubWorldID;			// 子世界ID
	int			m_nRegionIdx;			// 区域idx
	int			m_nMapX;				// 地图格子坐标 x
	int			m_nMapY;				// 地图格子坐标 y
	int			m_nOffX;				// 地图格子中的偏移 x (注：其值为像素点值 * 1024)
	int			m_nOffY;				// 地图格子中的偏移 y (注：其值为像素点值 * 1024)
	int			m_nDir;					// 方向，此方向为物件的逻辑方向，按64方向计算，不是具体的图像方向

	int			m_nState;				// 状态
	int			m_nLifeTime;			// 物件的消失时间（主要用于尸体类、装备类、宝箱类等）
	int			m_nBornTime;			// 重生倒计时（box打开状态经过lifetime自动关闭，prop隐身状态经过lifetime自动显示）
	int			m_nObjValue;

	char		m_szName[80];			// 物件名称
//	char		m_szScriptName[80];		// 所带脚本文件名
	DWORD		m_dwScriptID;			// 所带脚本文件ID
	int			m_nColorID;				// 物件名字显示时的颜色id，客户端通过查表找到相应颜色

	int			m_nItemDataID;			// 用于物品，指明物品的具体数据
	int			m_nItemWidth;			// 物品长（放置在容器中的时候）
	int			m_nItemHeight;			// 物品宽（放置在容器中的时候）
	int			m_nMoneyNum;			// 如果是掉在地上的钱，钱的数量
	int 		m_nGenre;
	int 		m_nDetailType;
	BOOL		m_bOverLook;

#ifdef _SERVER
	BOOL		m_bPickExecute;
	// 如果是打怪物掉出来的装备或钱：谁打出来的(记录的是player index) if == -1 不从属于任何人，任何人都能捡
	int			m_nBelong;
	// m_nBelong 起作用的时间(是个倒计时)
	int			m_nBelongTime;

	ServerImage m_cImage;				// 服务器端图像控制
#endif

#ifndef _SERVER
	char		m_szImageName[80];		// 图像文件名
	char		m_szSoundName[80];		// 声音文件名
	KSprControl	m_cImage;				// 物件图像

	char		m_szImageDropName[80];	// 如果是item类型，掉出来的时候播放的动画文件名
	KSprControl	m_cImageDrop;			// 如果是item类型，掉出来的时候播放的动画的图像控制
	int			m_nDropState;			// 如果是item类型，当前是掉出动画状态(1)还是放置在地板上状态(0)
	KRUImage	m_Image;
	unsigned int	m_SceneID;

	int			m_nLayer;				// 物件位于第几层
	int			m_nHeight;				// 物件的逻辑高度
	OBJ_LIGHT	m_sObjLight;			// 物件与亮度、发光相关的数据
	DWORD		m_dwNameColor;			// 物件名字的颜色
#endif

	KPolygon	Polygon;				// 底面多边形
	BYTE		m_btBar[OBJ_BAR_SIZE];	// 物件底面障碍信息

private:
	DWORD		m_dwTimer;				// 时间计数器(用于声音播放)

#ifndef _SERVER
	KCacheNode*	m_pSoundNode;			// 声效指针
	KWavSound*	m_pWave;				// 声效wav指针

	BOOL		m_bDrawFlag;			// 绘制是否特殊处理（FALSE 绘制   TRUE 不绘制）
#endif

public:
	KObj();
	void		GetMpsPos(int *pX, int *pY);
	void		Release();
	void		Activate();
	void		ExecScript(int nPlayerIdx);
	void		SetIndex(int nIndex);
	void		SetWorldID(int nID);			// 物件的世界唯一 ID
	void		SetLifeTime(int nLifeTime);
	void		SetObjValue(int nValue) {m_nObjValue = nValue;};
	int			GetObjValue() {return m_nObjValue;};
	BOOL		SetDir(int n64Dir);
	void		SetState(int nState, int nPlaySoundFlag = 0);
	void		SetImageDir(int nDir);
	void		Remove(BOOL bSoundFlag);
	void		SetScriptFile(char *lpszScriptFile);
	int			GetKind();

#ifdef _SERVER
	void		SetObjPickExecute(BOOL bFlag) {m_bPickExecute = (BOOL)bFlag;};
	BOOL		GetObjPickExecute() {return m_bPickExecute;};
	void		SetItemDataID(int nItemDataID);
	BOOL		SyncAdd(int nClient);
	void		SyncState();
	void		SyncDir();
	void		SyncRemove(BOOL bSoundFlag);
	int			GetItemDataID();
	void		SetItemBelong(int nPlayerIdx);
	void		SetEntireBelong(int nPlayerIdx);
#endif

#ifndef _SERVER
	void		Draw();
	void		DrawInfo(int nFontSize = 12);
	void		DrawBorder();
#endif

	// 凸多边形转换为障碍信息
	void		PolygonChangeToBar(
					KPolygon Polygon,		// 凸多边形
					int nGridWidth,			// 格子长
					int nGridHeight,		// 格子宽
					int nTableWidth,		// 表格长
					int nTableHeight,		// 表格宽
					BYTE *lpbBarTable);		// 表格内容
	int			GetDistanceSquare(int nNpcIndex);
private:
	void		BoxOpen();
	void		BoxClose();
	void		DoorOpen();
	void		DoorClose();
#ifndef _SERVER
	void		PlayLoopSound();
	void		PlayRandSound();
	int			GetSoundPan();
	int			GetSoundVolume();
	void		PlaySound();
#endif
};

extern	KObj	Object[MAX_OBJECT];
//---------------------------------------------------------------------------
#endif