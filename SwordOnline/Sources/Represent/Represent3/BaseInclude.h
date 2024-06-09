#ifndef __BASEINCLUDE_H__
#define __BASEINCLUDE_H__

#define DEFAULT_SCREENWIDTH		1600//800//			// ȱʡ��Ļ����
#define DEFAULT_SCREENHEIGHT	900//600//			// ȱʡ��Ļ�߶�

#define DEFAULT_BITDEPTH		16				// ȱʡλ���

#define MAX_TEXTURE_SIZE		1024			// �����ͼ�ߴ�
#define MIN_TEXTURE_SIZE		8				// ��С��ͼ�ߴ�

enum RenderModel
{
	RenderModel2D,
	RenderModel3DOrtho,
	RenderModel3DPerspective,
};

extern HWND		g_hWnd;

extern D3DFORMAT g_16BitFormat;

extern bool		g_bRefRast;						// �Ƿ�ʹ������3D����
extern bool		g_bRunWindowed;					// �Ƿ�ʹ�ô���ģʽ

extern int32	g_nScreenWidth;					// ��ʾ���ڿ���
extern int32	g_nScreenHeight;				// ��ʾ���ڸ߶�

extern uint32	g_nAntiAliasFSOverSample;		// ���������
extern uint32	g_nBackBufferCount;				// BackBuffer����
extern bool		g_bForceSwapEffectBlt;			// �Ƿ�ǿ���Դ潻����ʽΪ����
extern bool		g_bForce32Bit;					// �Ƿ�ǿ����32λ��ʽ����ISI_T_BITMAP16��Դ,��ʱ���ã�����32λ����

extern int		g_nFogEnable;					// �Ƿ������Ч��

extern int		g_nWireframe;					// �Ƿ���߿�ģʽ

extern RenderModel		g_renderModel;				// �Ƿ��͸��ģʽ

extern bool		g_bUse4444Texture;				// spr��Դ�Ƿ�ʹ��4444��ʽ��ͼ���������ʹ��8888��ʽ

extern bool		g_bNonPow2Conditional;			// �Ƿ������Ƕ����ݴγ�������ͼ

extern int		g_ntest;

#define YESNO(X) (X ? "Yes" : "No")

//#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

#endif