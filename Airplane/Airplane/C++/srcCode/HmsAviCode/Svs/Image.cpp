
#include <stdio.h>
#include "Common.h"
#include <string.h>
#include <stdlib.h>
#include "Global.h"
#include <ctype.h>
#import <OpenGLES/ES3/gl.h>

#include "Image.h"

#include "GLHeader.h"

#if TAWS_USE_PNG
#include "LibPng/include/png.h"
#ifdef _DEBUG 
#pragma  comment(lib,"../SRC/LibPng/lib/libpng13d.lib")
#pragma  comment(lib,"../SRC/LibPng/lib/zlib1d.lib")
#else
#pragma  comment(lib,"../SRC/LibPng/lib/libpng13.lib")
#pragma  comment(lib,"../SRC/LibPng/lib/zlib1.lib")
#endif 
#define PNG_BYTES_TO_CHECK 4   

int load_png_image( const char *filepath,pCImage image)
{
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep* row_pointers;
    char buf[PNG_BYTES_TO_CHECK];
    int w, h, x, y, temp, color_type;
    int returnValue = 0;
    fp = fopen( filepath, "rb" );
    if( fp == NULL ) 
    { 
        return returnValue;
    }

    png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
    info_ptr = png_create_info_struct( png_ptr );

    setjmp( png_jmpbuf(png_ptr) ); 
    /* 读取PNG_BYTES_TO_CHECK个字节的数据 */
    temp = fread( buf, 1, PNG_BYTES_TO_CHECK, fp );
    /* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
    if( temp < PNG_BYTES_TO_CHECK ) 
    {
        fclose(fp);
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
        return returnValue;
    }
    /* 检测数据是否为PNG的签名 */
    temp = png_sig_cmp( (png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK );
    /* 如果不是PNG的签名，则说明该文件不是PNG文件 */
    if( temp != 0 ) {
        fclose(fp);
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
        return returnValue;
    }

    /* 复位文件指针 */
    rewind( fp );
    /* 开始读文件 */
    png_init_io( png_ptr, fp ); 
    /* 读取PNG图片信息 */
    png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0 );
    /* 获取图像的色彩类型 */
    color_type = png_get_color_type( png_ptr, info_ptr );
    /* 获取图像的宽高 */
    w = png_get_image_width( png_ptr, info_ptr );
    h = png_get_image_height( png_ptr, info_ptr ); 


    image->m_uiWidth = w;
    image->m_uiHeight = h;
    /* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
    row_pointers = png_get_rows( png_ptr, info_ptr );
    /* 根据不同的色彩类型进行相应处理 */
    switch( color_type ) 
    {
    case PNG_COLOR_TYPE_RGB_ALPHA:
    {
                                     unsigned int pos=0;
                                     image->m_ucpData  = (unsigned char*) malloc(image->m_uiWidth*image->m_uiHeight*4);
                                     memset(image->m_ucpData,0,image->m_uiWidth*image->m_uiHeight*4);
                                     image->m_uiBPP = 32;
                                     for (y=0; y < h; y++)
                                     {
                                         memcpy_op(image->m_ucpData+y*image->m_uiWidth*4,row_pointers[h-y-1], image->m_uiWidth*4);
                                     }
                                     returnValue = 1;
                                     image->m_bIsLoaded =1;
    }
        break;
    case PNG_COLOR_TYPE_RGB:
    {
                               unsigned int pos=0;
                               image->m_ucpData  = (unsigned char*) malloc(image->m_uiWidth*image->m_uiHeight*3);
                               memset(image->m_ucpData,0,image->m_uiWidth*image->m_uiHeight*3);
                               image->m_uiBPP = 24; 

                               for (y=0; y < h; y++)
                               {
                                   memcpy_op(image->m_ucpData+y*image->m_uiWidth*3,row_pointers[h-1-y], image->m_uiWidth*3);
                               }
                               returnValue = 1;
                               image->m_bIsLoaded =1;
    }

        break;
        /* 其它色彩类型的图像就不读了 */
    default:
        ;
    }
    fclose(fp);
    png_destroy_read_struct( &png_ptr, &info_ptr, 0); 
    return returnValue;
}


#endif


unsigned char g_ucUTGAcompare[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
unsigned char g_ucCTGAcompare[12] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


int load_raw_image(const char *filepath, pCImage image)
{
    FILE *fp = 0;
    unsigned int w, h, dataType, pixType;
    int returnValue = 0;
    fp = fopen(filepath, "rb");
    if (fp == NULL)
    {
        printf("fopen:%s raw failed\n", filepath);
        return returnValue;
    }
    fread(&w, sizeof(unsigned int), 1, fp);
    fread(&h, sizeof(unsigned int), 1, fp);
    fread(&dataType, sizeof(unsigned int), 1, fp);
    fread(&pixType, sizeof(unsigned int), 1, fp);
    image->m_uiWidth = w;
    image->m_uiHeight = h;
    /* 根据不同的色彩类型进行相应处理 */
    switch (pixType)
    {
    case GL_RGBA:
        image->m_ucpData = (unsigned char *)malloc(image->m_uiWidth * image->m_uiHeight * 4);
        if (image->m_ucpData)
        {
			memset(image->m_ucpData, 0, image->m_uiWidth * image->m_uiHeight * 4);
			image->m_uiBPP = 32;
			fread(image->m_ucpData, image->m_uiWidth * image->m_uiHeight * 4, 1, fp);
			returnValue = 1;
			image->m_bIsLoaded = 1;
        }
        break;
    case GL_RGB:
        image->m_ucpData = (unsigned char *)malloc(image->m_uiWidth * image->m_uiHeight * 3);
		if (image->m_ucpData)
        {
			memset(image->m_ucpData, 0, image->m_uiWidth * image->m_uiHeight * 3);
			image->m_uiBPP = 24;
			fread(image->m_ucpData, image->m_uiWidth * image->m_uiHeight * 3, 1, fp);
			returnValue = 1;
			image->m_bIsLoaded = 1;
        }
        break;
    default:
        break;
    }
    fclose(fp);
    return returnValue;
}

CBool load_bmp_image(const char *filepath, pCImage image)
{
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    long iEnd, iStart, iSize;
    unsigned int iImageSize;
    unsigned char uiTempRGB;
    unsigned int i, tmpSize;
    unsigned int pixelBufSize = 0;

    //unsigned char* ucpFile= image->m_ucpData;
    FILE *pFile = 0;
    char buf[32] = { 0 };

    int returnValue = 0;
    pFile = fopen(filepath, "rb");
    if (pFile == NULL)
    {
        printf("Could not open file %s.\n", filepath);
        return returnValue;
    }
    fread(buf, 2, 1, pFile);
    if (strcmp(buf, "BM"))
    {
        fclose(pFile);
        return returnValue;
    }

    fseek(pFile, 0, SEEK_END);
    iEnd = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    iStart = ftell(pFile);
    iSize = iEnd - iStart;
    tmpSize = sizeof(BMPFileHeader);
    fread(&fileHeader, tmpSize, 1, pFile);
    //advance the buffer, and load in the file information header
    tmpSize = sizeof(BMPInfoHeader);
    fread(&infoHeader, tmpSize, 1, pFile);

    iImageSize = fileHeader.uiSize - fileHeader.uiOffBits;

    //CImage_Create(image, infoHeader->lWidth, infoHeader->lHeight, infoHeader->usBitCount );*
    image->m_uiWidth = infoHeader.lWidth;
    image->m_uiHeight = infoHeader.lHeight;
    image->m_uiBPP = infoHeader.usBitCount;
    if (image->m_uiBPP == 32)
    {
        pixelBufSize = image->m_uiWidth * image->m_uiHeight * 4;
        image->m_ucpData = (unsigned char *)malloc(pixelBufSize);
        memset(image->m_ucpData, 0, pixelBufSize);
        fread(image->m_ucpData, pixelBufSize, 1, pFile);
    }
    if (image->m_uiBPP == 24)
    {
        pixelBufSize = image->m_uiWidth * image->m_uiHeight * 3;
        image->m_ucpData = (unsigned char *)malloc(pixelBufSize);
        memset(image->m_ucpData, 0, pixelBufSize);
        fread(image->m_ucpData, pixelBufSize, 1, pFile);
    }


    //swap the R and B values to get RGB since the bitmap color format is in BGR
    if (image->m_uiBPP == 32/*RGBA*/)
    {
        unsigned char r, g, b, a;
        for (i = 0; i < pixelBufSize; i += 4)
        {
            a = image->m_ucpData[i];
            r = image->m_ucpData[i + 1];
            g = image->m_ucpData[i + 2];
            b = image->m_ucpData[i + 3];

            image->m_ucpData[i] = r;
            image->m_ucpData[i] = g;
            image->m_ucpData[i + 2] = b;
            image->m_ucpData[i + 3] = a;
            //	uiTempRGB = image->m_ucpData[i+3];
        }
    }
    else if (image->m_uiBPP == 24/*RGB*/)
    {
        for (i = 0; i < pixelBufSize; i += 3)
        {
            uiTempRGB = image->m_ucpData[i];
            image->m_ucpData[i] = image->m_ucpData[i + 2];
            image->m_ucpData[i + 2] = uiTempRGB;
        }
    }
    image->m_bIsLoaded = 1;
    fclose(pFile);
    return 1;
}

void PrintBMPFileHeader(BMPFileHeader *fileHeader)
{
    printf("fileHeader->usType:%d\n", fileHeader->usType);
    printf("fileHeader->uiSize:%d\n", fileHeader->uiSize);
    printf("fileHeader->usReserved1:%d\n", fileHeader->usReserved1);
    printf("fileHeader->usReserved2:%d\n", fileHeader->usReserved2);
    printf("fileHeader->uiOffBits:%d\n", fileHeader->uiOffBits);
}

void LittleEndianToBigEndianBMPFileHeader(BMPFileHeader *fileHeader)
{
    fileHeader->usType = LittleEndianToBigEndianUSHORT((fileHeader->usType));
    fileHeader->uiSize = LittleEndianToBigEndianUINT((fileHeader->uiSize));
    fileHeader->usReserved1 = LittleEndianToBigEndianUSHORT((fileHeader->usReserved1));
    fileHeader->usReserved2 = LittleEndianToBigEndianUSHORT((fileHeader->usReserved2));
    fileHeader->uiOffBits = LittleEndianToBigEndianUINT((fileHeader->uiOffBits));
}
void LittleEndianToBigEndianBMPInfoHeader(BMPInfoHeader *fileHeader)
{
    fileHeader->uiSize = LittleEndianToBigEndianUINT((fileHeader->uiSize));
    fileHeader->lWidth = LittleEndianToBigEndianLong((fileHeader->lWidth));

    fileHeader->lHeight = LittleEndianToBigEndianLong((fileHeader->lHeight));
    fileHeader->usPlanes = LittleEndianToBigEndianUSHORT((fileHeader->usPlanes));
    fileHeader->usBitCount = LittleEndianToBigEndianUSHORT((fileHeader->usBitCount));
    fileHeader->uiCompression = LittleEndianToBigEndianUINT((fileHeader->uiCompression));
    fileHeader->uiSizeImage = LittleEndianToBigEndianUINT((fileHeader->uiSizeImage));
    fileHeader->lXPelsPerMeter = LittleEndianToBigEndianLong((fileHeader->lXPelsPerMeter));
    fileHeader->lYPelsPerMeter = LittleEndianToBigEndianLong((fileHeader->lYPelsPerMeter));
    fileHeader->uiClrUsed = LittleEndianToBigEndianUINT((fileHeader->uiClrUsed));
    fileHeader->uiClrImportant = LittleEndianToBigEndianUINT((fileHeader->uiClrImportant));
}
void PrintBMPInfoHeader(BMPInfoHeader *fileHeader)
{
    printf("fileHeader->uiSize:%d\n", fileHeader->uiSize);
    printf("fileHeader->lWidth:%ld\n", fileHeader->lWidth);
    printf("fileHeader->lHeight:%ld\n", fileHeader->lHeight);
    printf("fileHeader->usPlanes:%d\n", fileHeader->usPlanes);
    printf("fileHeader->usBitCount:%d\n", fileHeader->usBitCount);
    printf("fileHeader->uiCompression:%d\n", fileHeader->uiCompression);
    printf("fileHeader->uiSizeImage:%d\n", fileHeader->uiSizeImage);
    printf("fileHeader->lXPelsPerMeter:%ld\n", fileHeader->lXPelsPerMeter);
    printf("fileHeader->lYPelsPerMeter:%ld\n", fileHeader->lYPelsPerMeter);
    printf("fileHeader->uiClrUsed:%d\n", fileHeader->uiClrUsed);
    printf("fileHeader->uiClrImportant:%d\n", fileHeader->uiClrImportant);
}
CBool CImage_LoadBMP(pCImage image)
{
    BMPFileHeader fileHeader;
    BMPInfoHeader infoHeader;
    unsigned int iImageSize;
    unsigned char uiTempRGB;
    unsigned int i;
    unsigned char* ucpFile = image->m_ucpData;
	if (0 == ucpFile){ return 0; }

    //load in the file header

    memcpy_op(&fileHeader, ucpFile, sizeof(BMPFileHeader));
    //advance the buffer, and load in the file information header
    ucpFile += sizeof(BMPFileHeader);
    memcpy_op(&infoHeader, ucpFile, sizeof(BMPInfoHeader));
    //advance the buffer to load int he actual image data
    ucpFile += sizeof(BMPInfoHeader);

    //ucpFile  += fileHeader->uiOffBits;

    //initialize the image memory
    iImageSize = fileHeader.uiSize - fileHeader.uiOffBits;
    //CImage_Create(image, infoHeader->lWidth, infoHeader->lHeight, infoHeader->usBitCount );*
    image->m_uiWidth = infoHeader.lWidth;
    image->m_uiHeight = infoHeader.lHeight;
    image->m_uiBPP = infoHeader.usBitCount;

    //	printf("width:%d,\theight:%d,\tuiBPP:%d\n",image->m_uiWidth,image->m_uiHeight,image->m_uiBPP);

    //copy the data to the class's data buffer
    memcpy_op(image->m_ucpData, ucpFile, iImageSize);

    //swap the R and B values to get RGB since the bitmap color format is in BGR
    if (image->m_uiBPP == 32/*RGBA*/)
    {
        unsigned char r, g, b, a;
        for (i = 0; i < infoHeader.uiSizeImage; i += 4)
        {
            a = image->m_ucpData[i];
            r = image->m_ucpData[i + 1];
            g = image->m_ucpData[i + 2];
            b = image->m_ucpData[i + 3];

            image->m_ucpData[i] = r;
            image->m_ucpData[i] = g;
            image->m_ucpData[i + 2] = b;
            image->m_ucpData[i + 3] = a;
            //	uiTempRGB = image->m_ucpData[i+3];
        }
    }
    else if (image->m_uiBPP == 24/*RGB*/)
    {
        for (i = 0; i < infoHeader.uiSizeImage; i += 3)
        {
            uiTempRGB = image->m_ucpData[i];
            image->m_ucpData[i] = image->m_ucpData[i + 2];
            image->m_ucpData[i + 2] = uiTempRGB;

        }
    }

    //the BMP has been successfully loaded

    return 1;
}
CBool CImage_SaveBMP(pCImage image, char* szFilename)
{

    FILE*		   pFile = 0;
    BMPFileHeader bitmapFileHeader;
    BMPInfoHeader bitmapInfoHeader;
    unsigned char  ucTempRGB;
    unsigned int   i;

    //open a file that we can write to
    pFile = fopen(szFilename, "wb");
    if (!pFile)
    {
        printf("Could not open a file to save %s in", szFilename);
        return 0;
    }

    //define the BMP file header
    bitmapFileHeader.uiSize = image->m_uiWidth*image->m_uiHeight * 3 + sizeof(BMPFileHeader)+sizeof(BMPInfoHeader);
    bitmapFileHeader.usType = BITMAP_ID;
    bitmapFileHeader.usReserved1 = 0;
    bitmapFileHeader.usReserved2 = 0;
    bitmapFileHeader.uiOffBits = sizeof(BMPFileHeader)+sizeof(BMPInfoHeader);

    //write the .bmp file header to the file
    fwrite(&bitmapFileHeader, 1, sizeof(BMPFileHeader), pFile);

    //define the BMP information header
    bitmapInfoHeader.uiSize = sizeof(BMPInfoHeader);
    bitmapInfoHeader.usPlanes = 1;
    bitmapInfoHeader.usBitCount = 24;
    bitmapInfoHeader.uiCompression = BI_RGB;
    bitmapInfoHeader.uiSizeImage = image->m_uiWidth*image->m_uiHeight * 3;
    bitmapInfoHeader.lXPelsPerMeter = 0;
    bitmapInfoHeader.lYPelsPerMeter = 0;
    bitmapInfoHeader.uiClrUsed = 0;
    bitmapInfoHeader.uiClrImportant = 0;
    bitmapInfoHeader.lWidth = image->m_uiWidth;
    bitmapInfoHeader.lHeight = image->m_uiHeight;

    //write the .bmp file information header to the file
    fwrite(&bitmapInfoHeader, 1, sizeof(BMPInfoHeader), pFile);

    //swap the image bit order (RGB to BGR)
    for (i = 0; i < bitmapInfoHeader.uiSizeImage; i += 3)
    {
        ucTempRGB = image->m_ucpData[i];
        image->m_ucpData[i] = image->m_ucpData[i + 2];
        image->m_ucpData[i + 2] = ucTempRGB;
    }

    //now write the actual image data
    fwrite(image->m_ucpData, 1, bitmapInfoHeader.uiSizeImage, pFile);

    //the file has been successfully saved
    fclose(pFile);
    return 1;
}
CBool CImage_LoadCompressedTGA(pCImage image)
{
    TGAInformationHeader pTGAinfo;
    unsigned char* ucpFile;
    unsigned char* ucpColorBuffer;
    unsigned char  ucChunkHeader;
    unsigned int   uiPixelCount;
    unsigned int   uiCurrentPixel;
    unsigned int   uiCurrentByte;
    short		   i;

    ucpFile = image->m_ucpData;

    //skip past the image header
    ucpFile += 12;

    memcpy_op(pTGAinfo.m_ucHeader, ucpFile, sizeof(unsigned char[6]));

    //allocate memory for the image data buffer
    CImage_Create(image, pTGAinfo.m_ucHeader[1] * 256 + pTGAinfo.m_ucHeader[0],
        pTGAinfo.m_ucHeader[3] * 256 + pTGAinfo.m_ucHeader[2],
        pTGAinfo.m_ucHeader[4]);

    //set the class's member variables
    pTGAinfo.m_uiBPP = image->m_uiBPP;
    pTGAinfo.m_uiHeight = image->m_uiHeight;
    pTGAinfo.m_uiWidth = image->m_uiWidth;

    //advance the file buffer
    ucpFile += 6;

    //make sure that the image's dimensions are supported by this loaded
    if ((image->m_uiWidth <= 0) || (image->m_uiHeight <= 0) || ((image->m_uiBPP != 24) && (image->m_uiBPP != 32)))
        return 0;

    pTGAinfo.m_uiBytesPerPixel = pTGAinfo.m_uiBPP / 8;
    pTGAinfo.m_uiImageSize = (pTGAinfo.m_uiBytesPerPixel*pTGAinfo.m_uiWidth*pTGAinfo.m_uiHeight);

    uiPixelCount = pTGAinfo.m_uiHeight * pTGAinfo.m_uiWidth;
    uiCurrentPixel = 0;
    uiCurrentByte = 0;
    ucpColorBuffer = (unsigned char*)malloc(sizeof(unsigned char)*pTGAinfo.m_uiBytesPerPixel);

    do
    {
        ucChunkHeader = 0;

        ucChunkHeader = *(unsigned char*)ucpFile;
        ucpFile++;

        if (ucChunkHeader < 128)
        {
            ucChunkHeader++;

            //read RAW color values
            for (i = 0; i < ucChunkHeader; i++)
            {
                memcpy_op(ucpColorBuffer, ucpFile, pTGAinfo.m_uiBytesPerPixel);
                ucpFile += pTGAinfo.m_uiBytesPerPixel;

                //flip R and B color values around
                image->m_ucpData[uiCurrentByte] = ucpColorBuffer[2];
                image->m_ucpData[uiCurrentByte + 1] = ucpColorBuffer[1];
                image->m_ucpData[uiCurrentByte + 2] = ucpColorBuffer[0];

                if (pTGAinfo.m_uiBytesPerPixel == 4)
                    image->m_ucpData[uiCurrentByte + 3] = ucpColorBuffer[3];

                uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
                uiCurrentPixel++;

                //make sure too many pixels have not been read in
                if (uiCurrentPixel > uiPixelCount)
                {
                    if (ucpColorBuffer != NULL)
                    {
                        free(ucpColorBuffer);
                        ucpColorBuffer = NULL;
                    }
                    if (image->m_ucpData != NULL)
                    {
                        free(image->m_ucpData);
                        image->m_ucpData = 0;
                    }
                    return 0;
                }
            }
        }

        //the chunk header is greater than 128 RLE data
        else
        {
            ucChunkHeader -= 127;

            memcpy_op(ucpColorBuffer, ucpFile, pTGAinfo.m_uiBytesPerPixel);
            ucpFile += pTGAinfo.m_uiBytesPerPixel;

            //copy the color into the image data as many times as needed
            for (i = 0; i < ucChunkHeader; i++)
            {
                //switch R and B bytes around while copying
                image->m_ucpData[uiCurrentByte] = ucpColorBuffer[2];
                image->m_ucpData[uiCurrentByte + 1] = ucpColorBuffer[1];
                image->m_ucpData[uiCurrentByte + 2] = ucpColorBuffer[0];

                if (pTGAinfo.m_uiBytesPerPixel == 4)
                    image->m_ucpData[uiCurrentByte + 3] = ucpColorBuffer[3];

                uiCurrentByte += pTGAinfo.m_uiBytesPerPixel;
                uiCurrentPixel++;

                //make sure that we have not written too many pixels
                if (uiCurrentPixel > uiPixelCount)
                {
                    if (ucpColorBuffer != NULL)
                    {
                        free(ucpColorBuffer);
                        ucpColorBuffer = NULL;
                    }
                    if (image->m_ucpData != NULL)
                    {
                        free(image->m_ucpData);
                        image->m_ucpData = 0;
                    }

                    return 0;
                }
            }
        }
    }

    //loop while there are still pixels left
    while (uiCurrentPixel < uiPixelCount);

    //the compressed TGA has been successfully loaded
    return 1;
}
CBool CImage_LoadUncompressedTGA(pCImage image)
{
    TGAInformationHeader pTGAinfo;
    unsigned int uiCSwap;
    unsigned char* ucpFile = image->m_ucpData;

    //skip past the TGA header in the data buffer
    ucpFile += 12;
    //copy the header data
    memcpy_op(pTGAinfo.m_ucHeader, ucpFile, sizeof(unsigned char[6]));

    //allocate memory for the image's data buffer
    CImage_Create(image, pTGAinfo.m_ucHeader[1] * 256 + pTGAinfo.m_ucHeader[0],
        pTGAinfo.m_ucHeader[3] * 256 + pTGAinfo.m_ucHeader[2],
        pTGAinfo.m_ucHeader[4]);

    //set the class's member variables
    pTGAinfo.m_uiBPP = image->m_uiBPP;
    pTGAinfo.m_uiWidth = image->m_uiWidth;
    pTGAinfo.m_uiHeight = image->m_uiHeight;

    ucpFile += 6;

    if ((image->m_uiWidth <= 0) || (image->m_uiHeight <= 0) || ((image->m_uiBPP != 24) && (image->m_uiBPP != 32)))
        return 0;

    pTGAinfo.m_uiBytesPerPixel = image->m_uiBPP / 8;
    pTGAinfo.m_uiImageSize = (pTGAinfo.m_uiBytesPerPixel*image->m_uiWidth*image->m_uiHeight);

    //copy the image data
    memcpy_op(image->m_ucpData, ucpFile, pTGAinfo.m_uiImageSize);

    //byte swapping ( optimized by Steve Thomas )
    //for( uiCSwap=0; uiCSwap<( int )pTGAinfo.m_uiImageSize; uiCSwap+=pTGAinfo.m_uiBytesPerPixel )
    //{
    //	image->m_ucpData[uiCSwap]^= image->m_ucpData[uiCSwap+2]^=
    //		image->m_ucpData[uiCSwap]^= image->m_ucpData[uiCSwap+2];
    //}
    if (1)
    {
        unsigned int sizeofTga = pTGAinfo.m_uiImageSize;
        unsigned char r, g, b, a;
        for (uiCSwap = 0; uiCSwap < sizeofTga; uiCSwap += 4)
        {
            r = image->m_ucpData[uiCSwap];
            g = image->m_ucpData[uiCSwap + 1];
            b = image->m_ucpData[uiCSwap + 2];
            a = image->m_ucpData[uiCSwap + 3];
            image->m_ucpData[uiCSwap] = r;
            image->m_ucpData[uiCSwap + 1] = g;
            image->m_ucpData[uiCSwap + 2] = b;
            image->m_ucpData[uiCSwap + 3] = a;
        }
    }

    //the uncompressed TGA has been successfully loaded
    return 1;
}

CBool CImage_Reset(pCImage image)
{
    if (image != NULL && image->m_ucpData)
    {
        memset(image->m_ucpData, 0, sizeof(unsigned char)*image->m_uiWidth*image->m_uiHeight*(image->m_uiBPP / 8));
    }
    return 1;
}
CBool CImage_ReBind(pCImage image)
{
    if (image != NULL && image->m_ucpData)
    {
        memset(image->m_ucpData, 0, sizeof(unsigned char)*image->m_uiWidth*image->m_uiHeight*(image->m_uiBPP / 8));
    }
    return 1;
}


CBool CImage_Create(pCImage image, unsigned int uiWidth, unsigned int uiHeight, unsigned int uiBPP)
{
    //set the member variables
    image->m_uiWidth = uiWidth;
    image->m_uiHeight = uiHeight;
    image->m_uiBPP = uiBPP;

    //allocate memory
    image->m_ucpData = (unsigned char*)malloc(sizeof(unsigned char)*uiWidth*uiHeight*(image->m_uiBPP / 8));
    if (!image->m_ucpData)
    {
        printf("out of memory for image memory allocation,内存分配错误\n");
        return 0;
    }
    else
    {
        memset(image->m_ucpData, 255, uiWidth*uiHeight*(image->m_uiBPP / 8));
    }

    //set the loaded flag
    image->m_bIsLoaded = 1;
    image->m_isShare = 0;

#if 0//opengl es 2 参数不支持
    image->TextureEnvMode = GL_TEXTURE_ENV_MODE;
    image->TextureEnvParameter = GL_MODULATE;

    image->wrap_s = image->wrap_t = GL_CLAMP;
#endif
    image->mag_filter = image->min_filter = GL_LINEAR;
    return 1;
}


int load_tga_image(const char *filepath, pCImage image)
{
    return 0;
}

CBool CImage_LoadData(pCImage image, char* szFilename)
{
    int resValue = 0;
    char fileExt[4] = { 0 };
    if (strlen(szFilename) <= 3 || image == NULL)
    {
        return resValue;
    }
    memcpy_op(fileExt, szFilename + strlen(szFilename) - 3, 3);

    fileExt[0] = tolower(fileExt[0]);
    fileExt[1] = tolower(fileExt[1]);
    fileExt[2] = tolower(fileExt[2]);

    if (!strcmp(fileExt, "raw"))
    {
        resValue = load_raw_image(szFilename, image);
        return resValue;
    }
    else if (!strcmp(fileExt, "png"))
    {
#if TAWS_USE_PNG
        return	resValue = load_png_image(szFilename,image);
#else
        return 0;
#endif

    }
    else if (!strcmp(fileExt, "bmp"))
    {
        resValue = load_bmp_image(szFilename, image);
        return resValue;
    }
    else if (!strcmp(fileExt, "tga"))
    {
        resValue = load_tga_image(szFilename, image);
        return resValue;
    }
    return resValue;
}

CBool CImage_Load(pCImage image, char* szFilename, float fMinFilter, float fMaxFilter, CBool bMipmap/*= 0*/)
{
    int	iType;

    //load the file's data in
    if (!CImage_LoadData(image, szFilename))
        return 0;

    //set the image's OpenGL BPP type
    if (image->m_uiBPP == 24)
        iType = GL_RGB;
    else
        iType = GL_RGBA;

    //build the texture for use with OpenGL
    glGenTextures(1, &image->m_ID);
    glBindTexture(GL_TEXTURE_2D, image->m_ID);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fMinFilter);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fMaxFilter);
    glTexImage2D(GL_TEXTURE_2D, 0, iType, image->m_uiWidth, image->m_uiHeight,
        0, iType, GL_UNSIGNED_BYTE, image->m_ucpData);
    //create the texture normally
    if (0/*bMipmap*/)
    {
#if 0//opengl es 2 参数不支持
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 4);
#endif
    }

    //the image has been successfully loaded
    image->m_bIsLoaded = 1;
    return 1;
}

void CImage_Unload(pCImage image)
{
	if (0 == image || image->m_bIsLoaded != 1)
    {
        return;
    }

    if (image->m_isShare != 1)
    {
        if (glIsTexture(image->m_ID))
        {
            glDeleteTextures(1, &image->m_ID);
        }
        if (image->m_ucpData)
        {
            free(image->m_ucpData);
            image->m_ucpData = NULL;
        }
    }
    else
    {
        printf("一个共享纹理:%s\n", image->name);
    }

    image->m_uiWidth = 0;
    image->m_uiHeight = 0;
    image->m_uiBPP = 0;
    image->m_bIsLoaded = 0;
}

CBool CImage_Save(pCImage image, char* szFilename)
{
    //save the file as a windows bitmap (.BMP)
    if (!CImage_SaveBMP(image, szFilename))
    {
        printf("Could not save %s.", szFilename);
        return 0;
    }

    //the file has been successfully saved
    printf("%s has been successfully saved", szFilename);
    return 1;
}

void CImage_GetColor(pCImage image, unsigned int x, unsigned  int y, unsigned char* ucpRed, unsigned char* ucpGreen, unsigned char* ucpBlue)
{
	if (image)
	{
		unsigned int uiBPP = image->m_uiBPP / 8;

		if ((x < image->m_uiWidth) && (y < image->m_uiHeight))
		{
			*ucpRed = image->m_ucpData[((y*image->m_uiHeight) + x)*uiBPP];
			*ucpGreen = image->m_ucpData[((y*image->m_uiHeight) + x)*uiBPP + 1];
			*ucpBlue = image->m_ucpData[((y*image->m_uiHeight) + x)*uiBPP + 2];
		}
	}
}

void CImage_SetColor(pCImage image, unsigned int x, unsigned  int y, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue, unsigned char ucAlpha)
{
	if (image)
	{
		unsigned int uiBPP = image->m_uiBPP / 8;
		if ((x < image->m_uiWidth) && (y < image->m_uiHeight))
		{
			image->m_ucpData[((y*image->m_uiHeight) + x)*uiBPP] = ucRed;
			image->m_ucpData[((y*image->m_uiHeight) + x)*uiBPP + 1] = ucGreen;
			image->m_ucpData[((y*image->m_uiHeight) + x)*uiBPP + 2] = ucBlue;
			if (uiBPP == 4)
			{
				image->m_ucpData[((y*image->m_uiHeight) + x)*uiBPP + 3] = ucAlpha;
			}
		}
	}
}

unsigned char* CImage_GetData(pCImage image)
{
    return (0 == image) ? 0 : image->m_ucpData;
}

unsigned int CImage_GetWidth(pCImage image)
{
	return (0 == image) ? 0 : image->m_uiWidth;
}


unsigned int CImage_GetHeight(pCImage image)
{
	return (0 == image) ? 0 : image->m_uiHeight;
}


unsigned int CImage_GetBPP(pCImage image)
{
    return image->m_uiBPP;
}


unsigned int CImage_GetID(pCImage image)
{
    return image->m_ID;
}


void CImage_SetID(pCImage image, unsigned int uiID)
{
    image->m_ID = uiID;
}


CBool CImage_IsLoaded(pCImage image)
{
    return image->m_bIsLoaded;
}

pCImage CImage_Create()//in fact ,pCImage is texture not just image
{
    pCImage image = (pCImage)malloc(sizeof(CImage));
    if (image)
    {
		memset(image, 0, sizeof(CImage));

		//deafult Texture parameters
		image->m_bMipmap = 0;
		image->wrap_s = GL_CLAMP_TO_EDGE;
		image->wrap_t = GL_CLAMP_TO_EDGE;
		image->mag_filter = GL_LINEAR;
		image->min_filter = GL_LINEAR;
		//	printf("image->m_isShare:%d\n",img->m_isShare);
		image->m_isLoadOverDel = 1;
    }

    return image;
}

int CImage_Destory(pCImage *pImage)
{
    pCImage p = *pImage;
    if (p != NULL)
    {
        CImage_Unload(p);
        if ((p->m_isShare != 1))
        {
            free(p);
            *pImage = NULL;
        }
    }
    return 1;
}

CBool CImage_ReBindTexture(pCImage image)
{
    int iType;
    if (image == NULL || image->m_bIsLoaded != 1 || image->m_ucpData == NULL)
    {
        return 0;
    }
    if (image->m_uiBPP == 24)
        iType = GL_RGB;
    else
        iType = GL_RGBA;

    image->m_iType = iType;
    if (!image->m_ID)
    {
        glGenTextures(1, &image->m_ID);
    }
    glBindTexture(GL_TEXTURE_2D, image->m_ID);

    glTexImage2D(GL_TEXTURE_2D, 0, iType, image->m_uiWidth, image->m_uiHeight, 0, iType,
        GL_UNSIGNED_BYTE, image->m_ucpData);

    return 1;
}

CBool CImage_BindTexture(pCImage image)
{
    int iType;
    if (image == NULL || image->m_bIsLoaded != 1 || image->m_ucpData == NULL)
    {
        return 0;
    }

    if (image->m_uiBPP == 24)
    {
        iType = GL_RGB;
    }
    else
    {
        iType = GL_RGBA;
    }

    image->m_iType = iType;
    glEnable(GL_TEXTURE_2D);

    if (glIsTexture(image->m_ID))
    {
        glDeleteTextures(1, &(image->m_ID));
    }
    glGenTextures(1, &image->m_ID);
    GLHeader_PrintGlError();
    glBindTexture(GL_TEXTURE_2D, image->m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, iType, image->m_uiWidth, image->m_uiHeight, 0, iType,
        GL_UNSIGNED_BYTE, image->m_ucpData);
    if (image->m_bMipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, image->wrap_s); // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, image->wrap_t); // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, image->mag_filter); // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, image->min_filter); // 设置纹理过滤方式

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    if (image->m_isLoadOverDel)
    {
        free(image->m_ucpData);
        image->m_ucpData = 0;
    }

    return 1;
}

CBool CImage_BindMipmapTexture(pCImage image)
{
    int	iType;
    if (image == NULL || image->m_bIsLoaded != 1 || image->m_ucpData == NULL)
    {
        return 0;
    }

    if (image->m_uiBPP == 24)
    {
        iType = GL_RGB;
    }
    else
    {
        iType = GL_RGBA;
    }

    image->m_iType = iType;

    glEnable(GL_TEXTURE_2D);
    if (glIsTexture(image->m_ID))
    {
        glDeleteTextures(1, &(image->m_ID));
    }
    glGenTextures(1, &image->m_ID);

    glBindTexture(GL_TEXTURE_2D, image->m_ID);
    glTexImage2D(GL_TEXTURE_2D, 0, iType, image->m_uiWidth, image->m_uiHeight, 0, iType, GL_UNSIGNED_BYTE, image->m_ucpData);
    //create the texture normally
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // 设置纹理过滤方式 

    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);

    if (image->m_isLoadOverDel)
    {
        free(image->m_ucpData);
        image->m_ucpData = 0;
    }

    return 1;
}

