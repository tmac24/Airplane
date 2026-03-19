#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "GLHeader.h"

#define BITMAP_ID 0x4D42
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L

typedef int CBool;
typedef struct _TGAInformationHeader
{
    unsigned char m_ucHeader[6];
    unsigned int  m_uiBytesPerPixel;
    unsigned int  m_uiImageSize;
    unsigned int  m_uiTemp;
    unsigned int  m_uiType;
    unsigned int  m_uiHeight;
    unsigned int  m_uiWidth;
    unsigned int  m_uiBPP;
}TGAInformationHeader, *pTGAInformationHeader;

#pragma pack( push, 1 )
typedef struct _BMPFileHeader
{
    unsigned short usType; 
    unsigned int uiSize; 
    unsigned short usReserved1; 
    unsigned short usReserved2; 
    unsigned int uiOffBits;
}BMPFileHeader,*pBMPFileHeader;
#pragma pack( pop )

typedef struct _BMPInfoHeader
{
    unsigned int  uiSize;
    long   lWidth;
    long   lHeight;
    unsigned short usPlanes;
    unsigned short usBitCount;
    unsigned int uiCompression;
    unsigned int uiSizeImage;
    long lXPelsPerMeter;
    long lYPelsPerMeter;
    unsigned int uiClrUsed;
    unsigned int uiClrImportant;
}BMPInfoHeader, *pBMPInfoHeader;


extern unsigned char g_ucUTGAcompare[12];
extern unsigned char g_ucCTGAcompare[12];

void LittleEndianToBigEndianBMPFileHeader(BMPFileHeader *fileHeader);
void LittleEndianToBigEndianBMPInfoHeader(BMPInfoHeader *fileHeader);

typedef struct _CImage
{
    /*数据*/
    unsigned char* m_ucpData;
    /*宽*/
    unsigned int   m_uiWidth;
    /*长*/
    unsigned int   m_uiHeight;
    /*位数  8 8 8 (RGB) 则m_uiBPP=24*/
    unsigned int   m_uiBPP;
    /*GL_RGBA GL_RGB*/
    int m_iType;
    /*纹理ID*/
    unsigned int   m_ID;
    /*是否绑定jiazai*/
    int m_bIsLoaded;
    /*是否共享Id*/
    int m_isShare;
    /*加载后是否删除内存段数据*/
    CBool m_isLoadOverDel;

    unsigned int TextureEnvMode;
    unsigned int TextureEnvParameter;

    char name[32];

    //MIPMAP
    int m_bMipmap;//0:normal, 1:mipmap
    //WRAP
    int wrap_s;
    int wrap_t;
    //FILTER
    int mag_filter;
    int min_filter;
}CImage, *pCImage;


pCImage CImage_Create();
int CImage_Destory(pCImage *p);


CBool CImage_LoadBMP(pCImage image);
CBool CImage_SaveBMP(pCImage image, char* szFilename);
CBool CImage_LoadCompressedTGA(pCImage image);
CBool CImage_LoadUncompressedTGA(pCImage image);
/*******************************************************************************
名称：CImage_Create
功能：创建一个图像
参数：无
时间:  21:28:32 5/20/2015
用户 : xiafei
版本：1.0
注意：
返回值：
*******************************************************************************/
CBool CImage_Create(pCImage image, unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP);

CBool CImage_Reset(pCImage image);

CBool CImage_LoadData(pCImage image, char* szFilename);
CBool CImage_Load(pCImage image, char* a_szFilename, float a_fMinFilter, float a_fMaxFilter, CBool a_bMipmap/*= false*/);
void CImage_Unload(pCImage image);
CBool CImage_Save(pCImage image, char* szFilename);
void CImage_GetColor(pCImage image, unsigned int x, unsigned  int y, unsigned char* ucpRed, unsigned char* ucpGreen, unsigned char* ucpBlue);
void CImage_SetColor(pCImage image, unsigned int x, unsigned  int y, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha);

unsigned char* CImage_GetData(pCImage image);
unsigned int CImage_GetWidth(pCImage image);
unsigned int CImage_GetHeight(pCImage image);

unsigned int CImage_GetBPP(pCImage image);

unsigned int CImage_GetID(pCImage image);

void CImage_SetID(pCImage image, unsigned int uiID);

CBool CImage_IsLoaded(pCImage image);


CBool CImage_Load(pCImage image, char* szFilename, float fMinFilter, float fMaxFilter, CBool bMipmap/*= 0*/);

CBool CImage_ReBindTexture(pCImage image);

CBool CImage_BindTexture(pCImage image);

CBool CImage_BindMipmapTexture(pCImage image);


CBool CImage_LoadPng(pCImage image);

#endif	//__IMAGE_H__
