#ifndef KMathH
#define KMathH

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
extern int *g_nSin;

// 余弦表 (将浮点数 *1024 整型化)
extern int *g_nCos;

// 正弦余弦的查表函数代码缓冲区
//extern unsigned char *g_InternalDirSinCosCode;

//typedef int	__cdecl g_InternalDirSinCosFunction(int pSinCosTable[], int nDir, int nMaxDir);

inline int g_DirSin(int nDir, int nMaxDir)
{

    if (nDir < 0)
        nDir = MaxMissleDir + nDir;

    if (nDir >= MaxMissleDir)
        nDir -= MaxMissleDir;

    return g_nSin[nDir];   // (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nSin, nDir, nMaxDir);
}

inline int g_DirCos(int nDir, int nMaxDir)
{
    if (nDir < 0)
        nDir = MaxMissleDir + nDir;

    if (nDir >= MaxMissleDir)
        nDir -= MaxMissleDir;

    return g_nCos[nDir];   // (*(g_InternalDirSinCosFunction *)(&(g_InternalDirSinCosCode[0])))(g_nCos, nDir, nMaxDir);
}

//---------------------------------------------------------------------------
// 五行相生相克
//extern int		g_nAccrueSeries[series_num];
//extern int		g_nConquerSeries[series_num];

extern INT g_nAccrueSeries[series_num];
extern INT g_nConquerSeries[series_num];
extern INT g_nAccruedSeries[series_num];
extern INT g_nConqueredSeries[series_num];

// 五行相生相克函数代码缓冲区
//extern unsigned char *g_InternalIsAccrueConquerCode;

//typedef int __cdecl g_InternalIsAccrueConquerFunction(int pAccrueConquerTable[], int nSrcSeries, int nDesSeries);

inline int g_IsAccrue(int nSrcSeries, int nDesSeries)   // 相生
{
    if (nSrcSeries < series_metal || nSrcSeries >= series_num)
        return FALSE;

    if (g_nAccrueSeries[nSrcSeries] == nDesSeries)
        return TRUE;

    return FALSE;   //(*(g_InternalIsAccrueConquerFunction *)(&(g_InternalIsAccrueConquerCode[0])))(g_nAccrueSeries, nSrcSeries, nDesSeries);
}

inline int g_IsConquer(int nSrcSeries, int nDesSeries)   //相克
{
    if (nSrcSeries < series_metal || nSrcSeries >= series_num)
        return FALSE;

    if (g_nConquerSeries[nSrcSeries] == nDesSeries)
        return TRUE;

    return FALSE;   //(*(g_InternalIsAccrueConquerFunction *)(&(g_InternalIsAccrueConquerCode[0])))(g_nConquerSeries, nSrcSeries, nDesSeries);
}

//---------------------------------------------------------------------------
inline int g_GetNewDistance(int nX1, int nY1, int nX2, int nY2)
{
    return (int)sqrt(double((nX1 - nX2)) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2));
}

//---------------------------------------------------------------------------
inline int g_GetDistance(int nX1, int nY1, int nX2, int nY2)
{
    return (int)sqrt(double((nX1 - nX2)) * (nX1 - nX2) + (nY1 - nY2) * (nY1 - nY2));
}

inline INT g_GetLength(INT nDx, INT Dy)
{
    return (INT)sqrt((FLOAT)(nDx * nDx + Dy * Dy));
}

//平方
inline INT g_Square(INT n)
{
    return n * n;
}
// 返回距离的平方(减少开方运算)
inline INT g_GetDisSquare(INT nX1, INT nY1, INT nX2, INT nY2)
{
    return g_Square(nX1 - nX2) + g_Square(nY1 - nY2);
}
//-----------------------------------------------------------------------------

inline INT g_GetNewDirIndex(INT nDx, INT nDy)
{
    INT nRet = -1;

    if (!nDx && !nDy)
        return -1;

    INT nDistance = g_GetLength(nDx, nDy);

    if (nDistance == 0)
        return -1;

    INT nSin = (nDy << 10) / nDistance;   // 放大1024倍

    //find more than me as my dir
    for (INT i = 0; i < 32; i++)   // 顺时针方向 从270度到90度，sin值递减
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

inline int g_GetOldDirIndex(int nX1, int nY1, int nX2, int nY2)
{
    int nRet = -1;

    if (nX1 == nX2 && nY1 == nY2)
        return -1;

    //	int		nDistance = g_GetDistance(nX1, nY1 * 2, nX2, nY2 * 2);
    int nDistance = g_GetDistance(nX1, nY1, nX2, nY2);

    if (nDistance == 0)
        return -1;

    //	int		nYLength = (nY2 - nY1) * 2;
    int nYLength = nY2 - nY1;
    int nLeghthVal = nYLength << 10;
    int nSin = nLeghthVal / nDistance;   // 放大1024倍

    for (int i = 0; i < 32; i++)   // 顺时针方向 从270度到90度，sin值递减		   左边
    {
        if (nSin > g_nSin[i])
            break;
        nRet = i;
    }

    if ((nX2 - nX1) > 0)   //右边
    {
        nRet = 63 - nRet;
    }
    return nRet;
}

//----------------------------------------------------------
inline INT g_GetDirIndex(INT nX1, INT nY1, INT nX2, INT nY2)
{

    return g_GetNewDirIndex(nX2 - nX1, nY2 - nY1);

    //return g_GetDirIdxForFindPath(nX1, nY1, nX2, nY2);

    //return g_GetOldDirIndex(nX1, nY1,nX2,nY2);
}

inline int g_Dir2DirIndex(int nDir, int nMaxDir)
{
    int nRet = -1;

    if (nMaxDir <= 0)
        return nRet;

    nRet = (nDir << 6) / nMaxDir;
    return nRet;
}

inline int g_DirIndex2Dir(int nDir, int nMaxDir)
{
    int nRet = -1;

    if (nMaxDir <= 0)
        return nRet;

    nRet = (nMaxDir * nDir) >> 6;   // (nMaxDir / 64) * nDir
    return nRet;
}

inline BOOL g_RandPercent(int nPercent)
{
    return ((int)g_Random(100) < nPercent);
}

#endif   //KMathH
