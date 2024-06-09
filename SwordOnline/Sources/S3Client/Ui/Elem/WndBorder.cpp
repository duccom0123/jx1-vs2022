#include "KWin32.h"
#include "../../Represent/iRepresent/iRepresentShell.h"
#include "../../Engine/Src/KDebug.h"
#include <math.h>
#include "WndBorder.h"


extern iRepresentShell*	g_pRepresentShell;


#define  FRAME_BORDER_BACK 20
#define	 FRAME_BORDER_GOBACK 6
#define	 FRAME_BDLIGHT_BACK 16
#define	 FRAME_BDLIGHT_GOBACK 8



int vsColors[][4][3] = {
	{{0, 0, 0}, {0, 0, 0},{0, 0, 0}, {0, 0, 0}},
	{{100, 78, 107}, {66, 48, 74},{189, 40, 255}, {49, 24, 66}},
	{{97, 83, 29}, {56, 48, 16}, {243, 194, 70}, {56, 48, 16}},
	{{107, 103, 107}, {74, 73, 74},{247, 243, 247}, {74, 77, 74}},
};

/*int vsColors[][4][3] = {
	{{0, 0, 0}, {0, 0, 0},{0, 0, 0}, {0, 0, 0}},
	{{100, 78, 107}, {66, 48, 74},{189, 40, 255}, {49, 24, 66}},
	{{100, 78, 26}, {66, 48, 16},{247, 195, 74}, {82, 65, 49}},
	{{107, 103, 107}, {74, 73, 74},{247, 243, 247}, {74, 77, 74}},
};*/

int rgbColor[3] = {0,0,0};

int* get_VS_ColorRGB(int* fromColor, int* toColor, int maxFrame, int step)
{
	float stepRBG[3] = {0,0,0};
	stepRBG[0] = (float)(fromColor[0] - toColor[0]) / maxFrame;
	stepRBG[1] = (float)(fromColor[1] - toColor[1]) / maxFrame;
	stepRBG[2] = (float)(fromColor[2] - toColor[2]) / maxFrame;
	rgbColor[0] = (int)floor(fromColor[0] - (stepRBG[0] * step));
	rgbColor[1] = (int)floor(fromColor[1] - (stepRBG[1] * step));
	rgbColor[2] = (int)floor(fromColor[2] - (stepRBG[2] * step));
	return rgbColor;
}


void _DrawBorder(KRUPoint *zdata,int zsizeline,int &zpos,unsigned char &zdir,int x,int y,int xe,int ye,int nNature)
{
	int i,temp,temp1;

	temp = zsizeline/2;
	for(i=0;i<zsizeline;i++)
	{
		temp1 = i;
		if(temp1 >= temp)
			temp1-=temp;
		int* getVSColor = get_VS_ColorRGB(vsColors[nNature][2], vsColors[nNature][3], temp, temp1);
		zdata[i].Color.Color_b.r = getVSColor[0];
		zdata[i].Color.Color_b.g = getVSColor[1];
		zdata[i].Color.Color_b.b = getVSColor[2];

	}
	switch(zdir)
	{
	case  0:
		for(i=0;i<zsizeline;i++)
		{	temp=zpos-i;
			if(temp>=x)
			{
				zdata[i].oPosition.nX=temp;
				zdata[i].oPosition.nY=y;	
			}
			else 
			{
				temp1=y+x-temp;
				if(temp1<=ye)
				{zdata[i].oPosition.nX=x;
				zdata[i].oPosition.nY=temp1;
				}
				else
				{
					zdata[i].oPosition.nX=x+temp1-ye;
					zdata[i].oPosition.nY=ye;
						
				}

			}
		}
		break;
	case  1:
		for(i=0;i<zsizeline;i++)
		{	temp=zpos-i;
		if(temp>=y)
		{
			zdata[i].oPosition.nY=temp;
			zdata[i].oPosition.nX=xe;	
		}
		else 
		{
			temp1=xe-(y-temp);
			if(temp1>=x)
			{zdata[i].oPosition.nX=temp1;
			zdata[i].oPosition.nY=y;
			}
			else
			{
				zdata[i].oPosition.nY=y+x-temp1;
				zdata[i].oPosition.nX=x;

			}

		}
		}
		break;
	case  2:
		for(i=0;i<zsizeline;i++)
		{	temp=zpos+i;
		if(temp<=xe)
		{
			zdata[i].oPosition.nX=temp;
			zdata[i].oPosition.nY=ye;	
		}
		else 
		{
			temp1=ye-(temp-xe);
			if(temp1>=y)
			{zdata[i].oPosition.nX=xe;
			zdata[i].oPosition.nY=temp1;
			}
			else
			{
				zdata[i].oPosition.nX=xe-(y-temp1);
				zdata[i].oPosition.nY=y;

			}

		}
		}
		break;
	case  3:
		for(i=0;i<zsizeline;i++)
		{	temp=zpos+i;
		if(temp<=ye)
		{
			zdata[i].oPosition.nX=x;
			zdata[i].oPosition.nY=temp;	
		}
		else 
		{
			temp1=x+(temp-ye);
			if(temp1<=xe)
			{zdata[i].oPosition.nX=temp1;
			zdata[i].oPosition.nY=ye;
			}
			else
			{
				zdata[i].oPosition.nY=ye-(temp1-xe);
				zdata[i].oPosition.nX=xe;

			}

		}
		}
		break;

	}

	g_pRepresentShell->DrawPrimitives(zsizeline,zdata,RU_T_POINT,true);
}

void DrawBorder(unsigned char &zframe, unsigned char &uctimedelay,int &zpos,unsigned char &zdir,unsigned char &ztimedelay,int x,int y,int width,int height,int nNature)
{
	if(nNature <= 0)
		return;

	KRURect        Rect;

	uctimedelay++;
    if(uctimedelay >= FRAME_BDLIGHT_BACK)
    {    
        uctimedelay = 0;
        zframe++;
        if(zframe >= FRAME_BDLIGHT_BACK)
            zframe = 0;
	}
	int tempval= zframe;
	if(tempval >FRAME_BDLIGHT_GOBACK)
		tempval = FRAME_BDLIGHT_GOBACK -(tempval-FRAME_BDLIGHT_GOBACK);
	int* getVSColor = get_VS_ColorRGB(vsColors[nNature][0], vsColors[nNature][1], FRAME_BDLIGHT_GOBACK, tempval);
	Rect.Color.Color_b.r = getVSColor[0];
	Rect.Color.Color_b.g = getVSColor[1];
	Rect.Color.Color_b.b = getVSColor[2];
         
	Rect.oPosition.nX = x - 1;
	Rect.oPosition.nY = y - 1;
	Rect.oEndPos.nX = x + width;
	Rect.oEndPos.nY = y + height;
	Rect.oEndPos.nZ = Rect.oPosition.nZ = 0;
	g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);
	Rect.oPosition.nX = x;
	Rect.oPosition.nY = y;
	Rect.oEndPos.nX = x + width - 1;
	Rect.oEndPos.nY = y + height - 1;
	g_pRepresentShell->DrawPrimitives(1, &Rect, RU_T_RECT, true);


	x-=1;
	y-=1;
	width+=1;
	height+=1;
	ztimedelay+=FRAME_BORDER_GOBACK;

	if(ztimedelay>=FRAME_BORDER_BACK)
	{
		ztimedelay-=FRAME_BORDER_BACK;
		if(zdir<=1)
		zpos++;
		else
		zpos--;
	}
	if(zdir==0)
	{	if(zpos<x)zpos=x;
		else if(zpos>=x+width)
		{	zpos=y+1;zdir=1;
		}
	}
	else if(zdir==1)
	{	if(zpos<y)zpos=y;
		else if(zpos>=y+height)
		{	zpos=x+width-1;zdir=2;
		}
	}
	else if(zdir==2)
	{	if(zpos>x+width)zpos=x+width;
		else if(zpos<=x)
		{	zpos=y+height-1;zdir=3;
		}
	}
	else if(zdir==3)
	{	if(zpos>y+height)zpos=y+height;
		else if(zpos<=y)
		{	zpos=x+1;zdir=0;
		}
	}

	tempval = width+height;
	if(tempval<0)
		return;
	else
	{
		KRUPoint *temp = new KRUPoint[tempval];
		_DrawBorder(temp,tempval,zpos,zdir, x,y,x+width,y+height,nNature);
		delete [] temp;
	}
}
