#ifndef KMathH
#define	KMathH

#include <math.h>
#include "GameDataDef.h"

int g_InitMath();
int g_UnInitMath();

void g_InitSeries();

#ifdef __linux
#define __cdecl
#endif

//---------------------------------------------------------------------------
// 正弦表 (将浮点数 *1024 整型化)
extern int* g_nSin;

// 余弦表 (将浮点数 *1024 整型化)
extern int* g_nCos;

// 正弦余弦的查表函数代码缓冲区
//extern unsigned char *g_InternalDirSinCosCode;

//typedef int	__cdecl g_InternalDirSinCosFunction(int pSinCosTable[], int nDir, int nMaxDir);

inline int g_DirSin(int nDir, int nMaxDir)
{

	if (nDir < 0)
		nDir = MaxMissleDir + nDir;

	if (nDir >= MaxMissleDir)
		nDir -= MaxMissleDir;

	return g_nSin[nDir];// (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nSin, nDir, nMaxDir);
}

inline int g_DirCos(int nDir, int nMaxDir)
{
	if (nDir < 0)
		nDir = MaxMissleDir + nDir;

	if (nDir >= MaxMissleDir)
		nDir -= MaxMissleDir;

	return g_nCos[nDir];// (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nCos, nDir, nMaxDir);
}

//---------------------------------------------------------------------------
// 五行相生相克
//extern int		g_nAccrueSeries[series_num];
//extern int		g_nConquerSeries[series_num];

extern INT	g_nAccrueSeries[series_num];
extern INT	g_nConquerSeries[series_num];
extern INT	g_nAccruedSeries[series_num];
extern INT	g_nConqueredSeries[series_num];

// 五行相生相克函数代码缓冲区
//extern unsigned char *g_InternalIsAccrueConquerCode;

//typedef int __cdecl g_InternalIsAccrueConquerFunction(int pAccrueConquerTable[], int nSrcSeries, int nDesSeries);

inline int g_IsAccrue(int nSrcSeries, int nDesSeries)  // 相生
{
	if (nSrcSeries < series_metal || nSrcSeries >= series_num)
		return FALSE;

	if (g_nAccrueSeries[nSrcSeries] == nDesSeries)
		return TRUE;

	return FALSE;//(*(g_InternalIsAccrueConquerFunction *)(&(g_InternalIsAccrueConquerCode[0])))(g_nAccrueSeries, nSrcSeries, nDesSeries);
}

inline int g_IsConquer(int nSrcSeries, int nDesSeries) //相克
{
	if (nSrcSeries < series_metal || nSrcSeries >= series_num)
		return FALSE;

	if (g_nConquerSeries[nSrcSeries] == nDesSeries)
		return TRUE;

	return FALSE;//(*(g_InternalIsAccrueConquerFunction *)(&(g_InternalIsAccrueConquerCode[0])))(g_nConquerSeries, nSrcSeries, nDesSeries);
}


//extern INT g_InternalIsAccrueConquer(INT pAccrueConquerTable[], INT nSrcSeries, INT nDesSeries);

/*inline BOOL g_IsAccrue(INT nSrcSeries, INT nDesSeries)
{
	return g_InternalIsAccrueConquer(g_nAccrueSeries, nSrcSeries, nDesSeries);
}

inline BOOL g_IsConquer(INT nSrcSeries, INT nDesSeries)
{
	return g_InternalIsAccrueConquer(g_nConquerSeries, nSrcSeries, nDesSeries);
}*/

//---------------------------------------------------------------------------
//Add by DNT
// 优化平方根函数
/*typedef union
{
	int     i;          // as integer
	float   f;          // as float
	struct              // as bit fields
	{
		unsigned int    sign:1;
		unsigned int    biasedexponent:8;
		unsigned int    significand;
	}bits;
}INTORFLOAT;

 Bias constant used for fast conversions between int and float. First element
in INTORFLOAT union is integer -- we are storing biased exponent 23, mantissa .1, which
is equivalent to 1.5 x 2^23. */
/*
const  INTORFLOAT  bias = {((23 + 127) << 23) + (1 << 22)};

#define SQRTTABLESIZE       256
const unsigned int sqrttable[SQRTTABLESIZE] =
{
		531980127, 532026288, 532072271, 532118079, 532163712, 532209174, 532254465, 532299589,
		532344546, 532389339, 532433970, 532478440, 532522750, 532566903, 532610900, 532654744,
		532698434, 532741974, 532785365, 532828607, 532871704, 532914655, 532957463, 533000129,
		533042654, 533085041, 533127289, 533169401, 533211378, 533253220, 533294931, 533336509,
		533377958, 533419278, 533460470, 533501535, 533542475, 533583291, 533623984, 533664554,
		533705004, 533745334, 533785545, 533825638, 533865615, 533905476, 533945222, 533984855,
		534024374, 534063782, 534103079, 534142267, 534181345, 534220315, 534259178, 534297934,
		534336585, 534375132, 534413574, 534451914, 534490152, 534528288, 534566324, 534604260,
		534642098, 534679837, 534717478, 534755023, 534792473, 534829827, 534867086, 534904252,
		534941325, 534978305, 535015194, 535051992, 535088699, 535125317, 535161846, 535198287,
		535234640, 535270905, 535307085, 535343178, 535379187, 535415110, 535450950, 535486706,
		535522379, 535557970, 535593480, 535628908, 535664255, 535699523, 535734711, 535769820,
		535804850, 535839803, 535874678, 535909476, 535944198, 535978844, 536013414, 536047910,
		536082331, 536116678, 536150952, 536185153, 536219281, 536253337, 536287322, 536321235,
		536355078, 536388850, 536422553, 536456186, 536489750, 536523246, 536556673, 536590033,
		536623325, 536656551, 536689709, 536722802, 536755829, 536788791, 536821688, 536854520,
		536887280, 536919921, 536952436, 536984827, 537017094, 537049241, 537081267, 537113174,
		537144963, 537176637, 537208195, 537239640, 537270972, 537302193, 537333304, 537364306,
		537395200, 537425987, 537456669, 537487246, 537517720, 537548091, 537578361, 537608530,
		537638600, 537668572, 537698446, 537728224, 537757906, 537787493, 537816986, 537846387,
		537875696, 537904913, 537934040, 537963078, 537992027, 538020888, 538049662, 538078350,
		538106952, 538135470, 538163903, 538192254, 538220521, 538248707, 538276812, 538304837,
		538332781, 538360647, 538388434, 538416144, 538443776, 538471332, 538498812, 538526217,
		538553548, 538580804, 538607987, 538635097, 538662136, 538689102, 538715997, 538742822,
		538769577, 538796263, 538822880, 538849428, 538875909, 538902322, 538928668, 538954949,
		538981163, 539007312, 539033396, 539059416, 539085373, 539111265, 539137095, 539162863,
		539188568, 539214212, 539239794, 539265316, 539290778, 539316180, 539341522, 539366806,
		539392031, 539417197, 539442306, 539467358, 539492352, 539517290, 539542171, 539566997,
		539591768, 539616483, 539641143, 539665749, 539690301, 539714800, 539739245, 539763637,
		539787976, 539812264, 539836499, 539860682, 539884815, 539908896, 539932927, 539956907,
		539980838, 540004718, 540028549, 540052332, 540076065, 540099750, 540123387, 540146976,
		540170517, 540194011, 540217458, 540240858, 540264211, 540287519, 540310780, 540333996,
};

inline float qsqrt( float f )
{
	INTORFLOAT      fi;
	unsigned int    e, n;

	fi.f = f;
	n = fi.i;

	e = (n >> 1) & 0x3f800000;

	n = (n >> 16) & 0xff;

	fi.i = e + sqrttable[n];

	return fi.f;
}
*/
//---------------------------------------------------------------------------
inline int	g_GetNewDistance(int nX1, int nY1, int nX2, int nY2)
{
	// Fixed By DNT 2012.02.02
	/*INTORFLOAT tmp;
	tmp.f = qsqrt((float)((nX1 - nX2) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2)));
	tmp.f += bias.f;
	tmp.i -= bias.i;
	return tmp.i;*/
	return (int)sqrt(double((nX1 - nX2)) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2));
}

//---------------------------------------------------------------------------
inline int	g_GetDistance(int nX1, int nY1, int nX2, int nY2)
{
	return (int)sqrt(double((nX1 - nX2)) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2));
}

inline INT	g_GetLength(INT nDx, INT Dy)
{
	return (INT)sqrt((FLOAT)(nDx * nDx + Dy * Dy));
}

//平方
inline INT	g_Square(INT n)
{
	return n * n;
}
// 返回距离的平方(减少开方运算)
inline INT	g_GetDisSquare(INT nX1, INT nY1, INT nX2, INT nY2)
{
	return g_Square(nX1 - nX2) + g_Square(nY1 - nY2);
}
//-----------------------------------------------------------------------------

inline INT	g_GetNewDirIndex(INT nDx, INT nDy)
{
	INT nRet = -1;

	if (!nDx && !nDy)
		return -1;

	INT nDistance = g_GetLength(nDx, nDy);

	if (nDistance == 0) return -1;

	INT		nSin = (nDy << 10) / nDistance;	// 放大1024倍

	//find more than me as my dir
	for (INT i = 0; i < 32; i++)		// 顺时针方向 从270度到90度，sin值递减
	{
		if (nSin > g_nSin[i])
			break;
		nRet = i;
	}

	INT nD1, nD2;
	if (g_nSin[nRet] != nSin)
	{
		nD1 = g_nSin[nRet] - nSin;
		nD2 = nSin - g_nSin[nRet + 1];
		if (nD1 > nD2)
			nRet++;
	}

	if (nDx >= 0 && nRet != 0)
	{
		nRet = 64 - nRet;
	}
	return nRet;
}

inline int	g_GetOldDirIndex(int nX1, int nY1, int nX2, int nY2)
{
	int		nRet = -1;

	if (nX1 == nX2 && nY1 == nY2)
		return -1;

	//	int		nDistance = g_GetDistance(nX1, nY1 * 2, nX2, nY2 * 2);
	int		nDistance = g_GetDistance(nX1, nY1, nX2, nY2);

	if (nDistance == 0)
		return -1;

	//	int		nYLength = (nY2 - nY1) * 2;
	int		nYLength = nY2 - nY1;
	int     nLeghthVal = nYLength << 10;
	int     nSin = nLeghthVal / nDistance;	// 放大1024倍
	//			nCha = nLeghthVal%nDistance;

	/*	if (nLeghthVal>0)
		{
			   if (nCha>=nDistance/2)
				  nSin ++;
		}
		else
		{
			  if (-nCha>=nDistance/2)
				 nSin --;

		} */

	for (int i = 0; i < 32; i++)		// 顺时针方向 从270度到90度，sin值递减		   左边
	{
		if (nSin > g_nSin[i])
			break;
		nRet = i;
	}

	if ((nX2 - nX1) > 0)	//右边
	{
		nRet = 63 - nRet;
	}
	return nRet;
}

// 下面的函数计算方向有偏差，这里专门写一个特殊版本为给寻路使用
/*inline int	g_GetDirIdxForFindPath(int nX1, int nY1, int nX2, int nY2)
{
	int		nRet = -1;

	if (nX1 == nX2 && nY1 == nY2)
		return -1;

	//	int		nDistance = g_GetDistance(nX1, nY1 * 2, nX2, nY2 * 2);
	int		nDistance = g_GetNewDistance(nX1, nY1, nX2, nY2);

	if (nDistance == 0 ) return -1;

	//	int		nYLength = (nY2 - nY1) * 2;
	int		nYLength = nY2 - nY1;
	int		nSin = (nYLength << 10) / nDistance;	// 放大1024倍

	if(nSin > 1024)
		nSin = 1024;
	else if(nSin < -1024)
		nSin = -1024;

	for (int i = 0; i < 32; i++)		// 顺时针方向 从270度到90度，sin值递减
	{
		if (nSin > g_nSin[i])
			break;
		nRet = i;
	}

	if ((nX2 - nX1) > 0)
	{
		nRet = 63 - nRet;
	}

	return nRet;
} */


//----------------------------------------------------------
inline INT	g_GetDirIndex(INT nX1, INT nY1, INT nX2, INT nY2)
{

	return g_GetNewDirIndex(nX2 - nX1, nY2 - nY1);

	//return g_GetDirIdxForFindPath(nX1, nY1, nX2, nY2);

	//return g_GetOldDirIndex(nX1, nY1,nX2,nY2);
}



inline	int g_Dir2DirIndex(int nDir, int nMaxDir)
{
	int nRet = -1;

	if (nMaxDir <= 0)
		return nRet;

	nRet = (nDir << 6) / nMaxDir;
	return nRet;
}

inline int	g_DirIndex2Dir(int nDir, int nMaxDir)
{
	int		nRet = -1;

	if (nMaxDir <= 0)
		return nRet;

	nRet = (nMaxDir * nDir) >> 6;	// (nMaxDir / 64) * nDir
	return nRet;
}


inline BOOL g_RandPercent(int nPercent)
{
	return ((int)g_Random(100) < nPercent);

	/*	int i = g_Random(100);

		if (i >= nPercent)
			return FALSE;
		else
			return TRUE;*/
}

#endif //KMathH
