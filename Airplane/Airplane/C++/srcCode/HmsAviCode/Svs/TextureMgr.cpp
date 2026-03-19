
#include "Log.h"
#include "Common.h"
#include "TextureMgr.h"
#include <stdlib.h>
#include "GLES3/gl3.h"


static pCImage  sTerrainImage = NULL;
static  pCImage sTotalImage[MAX_IMAGE_NUMBER] = { NULL };
static unsigned char sInit = 0;

//************************************
// Method:    GetBarrierTexture
// FullName:  GetBarrierTexture
// Access:    public 
// Desc:      
// Returns:   bool, true: succeed,  false:failed
// Qualifier:
// Parameter: pCImage image
// Parameter: int type
// Author:    yan.jiang
//************************************
bool GetBarrierTexture(pCImage image, int type)
{
    if (0 == image){ return false; }

    char filepath[256] = { 0 };
    const char *path = GetTawsDataPath();
#if TAWS_USE_PNG
    char * ext = ".png";
#else
    char *ext = ".raw";
#endif

    switch (type)
    {
    case 0:
        sprintf(image->name, "Obstacle");
        sprintf(filepath, "%s/Texture/Obstacle_border%s", path, ext);
        break;
    case 1:
        sprintf(image->name, "Obstacle_with_Light");
        sprintf(filepath, "%s/Texture/Obstacle_with_Light_border%s", path, ext);
        break;
    case 2:
        sprintf(image->name, "Tall_Obstacle");
        sprintf(filepath, "%s/Texture/Tall_Obstacle%s", path, ext);
        break;
    case 3:
        sprintf(image->name, "Tall_Obstacle_with_Light");
        sprintf(filepath, "%s/Texture/Tall_Obstacle_with_Light%s", path, ext);
        break;
    default:
        return false;
    }

    if (CImage_LoadData(image, filepath))
    {
        image->m_bMipmap = 1;
        image->wrap_s = GL_CLAMP_TO_EDGE;
        image->wrap_t = GL_CLAMP_TO_EDGE;
        image->mag_filter = GL_LINEAR;
        image->min_filter = GL_LINEAR_MIPMAP_NEAREST;
        if (CImage_BindTexture(image) == 0)
        {
            printf("Bind Texture %s failed!\n", filepath);
        }
        return true;
    }
    return false;
}

void GetFlightPathTexture(pCImage image)
{
    char filepath[256] = { 0 };
    const char *path = GetTawsDataPath();
    sprintf(filepath, "%s/Texture/flightpath.png", path);
    if (CImage_LoadData(image, filepath))
    {
        CImage_BindTexture(image);
        if (0)
        {
            int iType;
            if (image == NULL || image->m_bIsLoaded != 1 || image->m_ucpData == NULL)
            {
                return;
            }
            //set the image's OpenGL BPP type
            if (image->m_uiBPP == 24)
                iType = GL_RGB;
            else
                iType = GL_RGBA;

            image->m_iType = iType;
            glGenTextures(1, &image->m_ID);
            if (glIsTexture(image->m_ID))
            {
                glDeleteTextures(1, &(image->m_ID));
                glGenTextures(1, &image->m_ID);
            }
            glBindTexture(GL_TEXTURE_2D, image->m_ID);
            glTexImage2D(GL_TEXTURE_2D, 0, iType, image->m_uiWidth, image->m_uiHeight, 0, iType,
                GL_UNSIGNED_BYTE, image->m_ucpData);
#if 0//opengl es 2 参数不支持
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // 设置纹理过滤方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // 设置纹理过滤方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 设置纹理过滤方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 设置纹理过滤方式
#endif
            image->m_bIsLoaded = 1;
        }
    }
    else
    {
        printf("read image file : %s  ->>> failed!\n ", filepath);
    }
}

//jy: All element in of sTotalImage, is shared Image(Texture).
void TextureMgr_Init()
{
    if (sInit)
    {
        return;
    }

    char filepath[256] = { 0 };
    const char *path = GetTawsDataPath();

    sTotalImage[TextureType_Airport] = CImage_Create();
    sprintf(filepath, "%s/Texture/Airport1.raw", path);
    if (CImage_LoadData(sTotalImage[TextureType_Airport], filepath))
    {
        sprintf(sTotalImage[TextureType_Airport]->name, "tex airport");
        sTotalImage[TextureType_Airport]->m_bMipmap = 1;
        sTotalImage[TextureType_Airport]->wrap_s = GL_CLAMP_TO_EDGE;
        sTotalImage[TextureType_Airport]->wrap_t = GL_CLAMP_TO_EDGE;
        sTotalImage[TextureType_Airport]->mag_filter = GL_LINEAR;
        sTotalImage[TextureType_Airport]->min_filter = GL_LINEAR_MIPMAP_NEAREST;
        CImage_BindTexture(sTotalImage[TextureType_Airport]);
        sTotalImage[TextureType_Airport]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_Airport]);
        sTotalImage[TextureType_Airport] = NULL;
    }

    sTotalImage[TextureType_PLANE] = CImage_Create();
    sprintf(filepath, "%s/Texture/PLANE.RAW", path);
    if (CImage_LoadData(sTotalImage[TextureType_PLANE], filepath))
    {
        sprintf(sTotalImage[TextureType_PLANE]->name, "tex plane");
        sTotalImage[TextureType_PLANE]->m_bMipmap = 0;
        sTotalImage[TextureType_PLANE]->wrap_s = GL_REPEAT;
        sTotalImage[TextureType_PLANE]->wrap_t = GL_REPEAT;
        sTotalImage[TextureType_PLANE]->mag_filter = GL_LINEAR;
        sTotalImage[TextureType_PLANE]->min_filter = GL_LINEAR;
        CImage_BindTexture(sTotalImage[TextureType_PLANE]);
        sTotalImage[TextureType_PLANE]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_PLANE]);
        sTotalImage[TextureType_PLANE] = NULL;
    }

    sTotalImage[TextureType_3DTerrain_NEW] = CImage_Create();
    sprintf(filepath, "%s/Texture/DMT.RAW", path);
    //sprintf(filepath, "%s/Texture/DMT.bmp", path);
    //sprintf(filepath, "%s/Texture/DMT1.bmp", path);
    if (CImage_LoadData(sTotalImage[TextureType_3DTerrain_NEW], filepath))
    {
        sprintf(sTotalImage[TextureType_3DTerrain_NEW]->name, "tex 3DTerrain_NEW");
        sTotalImage[TextureType_3DTerrain_NEW]->m_bMipmap = 1;
        sTotalImage[TextureType_3DTerrain_NEW]->wrap_s = GL_REPEAT;
        sTotalImage[TextureType_3DTerrain_NEW]->wrap_t = GL_REPEAT;
        sTotalImage[TextureType_3DTerrain_NEW]->mag_filter = GL_LINEAR;
        sTotalImage[TextureType_3DTerrain_NEW]->min_filter = GL_NEAREST_MIPMAP_NEAREST;
        CImage_BindTexture(sTotalImage[TextureType_3DTerrain_NEW]);
        sTotalImage[TextureType_3DTerrain_NEW]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_3DTerrain_NEW]);
        sTotalImage[TextureType_3DTerrain_NEW] = NULL;
    }

    sTotalImage[TextureType_DetailMap] = CImage_Create();
    sprintf(filepath, "%s/Texture/detailmap.raw", path);
    if (CImage_LoadData(sTotalImage[TextureType_DetailMap], filepath))
    {
        sprintf(sTotalImage[TextureType_DetailMap]->name, "tex detailmap");
        sTotalImage[TextureType_DetailMap]->m_bMipmap = 0;
        sTotalImage[TextureType_DetailMap]->wrap_s = GL_REPEAT;
        sTotalImage[TextureType_DetailMap]->wrap_t = GL_REPEAT;
        sTotalImage[TextureType_DetailMap]->mag_filter = GL_LINEAR;
        sTotalImage[TextureType_DetailMap]->min_filter = GL_LINEAR;
        CImage_BindTexture(sTotalImage[TextureType_DetailMap]);
        sTotalImage[TextureType_DetailMap]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_DetailMap]);
        sTotalImage[TextureType_DetailMap] = NULL;
    }

    sTotalImage[TextureType_3DTerrain] = CImage_Create();
    //...need more init
    sTotalImage[TextureType_3DTerrain]->m_isShare = 1; //jy: All element in of sTotalImage, is shared Image(Texture). The same below.

    sTotalImage[TextureType_2DTerrain] = CImage_Create();
    //...
    sTotalImage[TextureType_2DTerrain]->m_isShare = 1;

    sTotalImage[TextureType_Warning] = CImage_Create();
    //...
    sTotalImage[TextureType_Warning]->m_isShare = 1;

    sTotalImage[TextureType_Obstacle] = CImage_Create();
    if (GetBarrierTexture(sTotalImage[TextureType_Obstacle], 0))
    {
        sprintf(sTotalImage[TextureType_Obstacle]->name, "tex Obstacle");
        sTotalImage[TextureType_Obstacle]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_Obstacle]);
        sTotalImage[TextureType_Obstacle] = NULL;
    }

    sTotalImage[TextureType_Obstacle_with_Light] = CImage_Create();
    if (GetBarrierTexture(sTotalImage[TextureType_Obstacle_with_Light], 1))
    {
        sprintf(sTotalImage[TextureType_Obstacle_with_Light]->name, "tex obstacle Light");
        sTotalImage[TextureType_Obstacle_with_Light]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_Obstacle_with_Light]);
        sTotalImage[TextureType_Obstacle_with_Light] = NULL;
    }

    sTotalImage[TextureType_Tall_Obstacle] = CImage_Create();
    if (GetBarrierTexture(sTotalImage[TextureType_Tall_Obstacle], 2))
    {
        sprintf(sTotalImage[TextureType_Tall_Obstacle]->name, "tex tall Obstacle");
        sTotalImage[TextureType_Tall_Obstacle]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_Tall_Obstacle]);
        sTotalImage[TextureType_Tall_Obstacle] = NULL;
    }

    sTotalImage[TextureType_Tall_Obstacle_with_Light] = CImage_Create();
    if (GetBarrierTexture(sTotalImage[TextureType_Tall_Obstacle_with_Light], 3))
    {
        sprintf(sTotalImage[TextureType_Tall_Obstacle_with_Light]->name, "tex tall obstacle light");
        sTotalImage[TextureType_Tall_Obstacle_with_Light]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_Tall_Obstacle_with_Light]);
        sTotalImage[TextureType_Tall_Obstacle_with_Light] = NULL;
    }

    for (int i = TextureType_RunWay0; i <= TextureType_RunWayLine; i++)
    {
        LogWrite(LOG_LEVEL_ALWAYS, "RunWay %d.....", i);
        sTotalImage[i] = CImage_Create();
        LogWrite(LOG_LEVEL_ALWAYS, "CImage_Creat over", i);

        if (i == TextureType_RunWayThr)
        {
            sprintf(filepath, "%s/RunwayTexture/rway_thr1.raw", path);
        }
        else if (i == TextureType_RunWayLeft)
        {
            sprintf(filepath, "%s/RunwayTexture/rway_L.raw", path);
        }
        else if (i == TextureType_RunWayRight)
        {
            sprintf(filepath, "%s/RunwayTexture/rway_R.raw", path);
        }
        else if (i == TextureType_RunWayLine)
        {
            sprintf(filepath, "%s/RunwayTexture/rway_line.raw", path);
        }
        else
        {
            //run way number
            sprintf(filepath, "%s/RunwayTexture/RWAY_%02d.RAW", path, i);
        }
        if (CImage_LoadData(sTotalImage[i], filepath))
        {
            sprintf(sTotalImage[i]->name, "tex enum id %d", i);
            sTotalImage[i]->m_bMipmap = 0;
            sTotalImage[i]->wrap_s = GL_REPEAT;
            sTotalImage[i]->wrap_t = GL_REPEAT;
            sTotalImage[i]->mag_filter = GL_LINEAR;
            sTotalImage[i]->min_filter = GL_LINEAR;
            CImage_BindTexture(sTotalImage[i]);
            sTotalImage[i]->m_isShare = 1;
        }
        else
        {
            free(sTotalImage[i]);
            sTotalImage[i] = NULL;
        }
    }

    sTotalImage[TextureType_Aircraft] = CImage_Create();
#if TAWS_USE_PNG
    sprintf(filepath, "%s/Texture/Aircraft.png", path);
#else
    sprintf(filepath, "%s/Texture/Aircraft_GuDingYi.raw", path);
#endif
    if (CImage_LoadData(sTotalImage[TextureType_Aircraft], filepath))
    {
        sTotalImage[TextureType_Aircraft]->m_bMipmap = 1;
        sTotalImage[TextureType_Aircraft]->wrap_s = GL_CLAMP_TO_EDGE;
        sTotalImage[TextureType_Aircraft]->wrap_t = GL_CLAMP_TO_EDGE;
        sTotalImage[TextureType_Aircraft]->mag_filter = GL_LINEAR;
        sTotalImage[TextureType_Aircraft]->min_filter = GL_LINEAR_MIPMAP_NEAREST;
        CImage_BindTexture(sTotalImage[TextureType_Aircraft]);
        sTotalImage[TextureType_Aircraft]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_Aircraft]);
        sTotalImage[TextureType_Aircraft] = NULL;
    }

    sTotalImage[TextureType_WarningPoint_Yellow] = CImage_Create();
    sprintf(filepath, "%s/Warning/GY_yellow.raw", path);
    if (CImage_LoadData(sTotalImage[TextureType_WarningPoint_Yellow], filepath))
    {
        CImage_BindTexture(sTotalImage[TextureType_WarningPoint_Yellow]);
        sTotalImage[TextureType_WarningPoint_Yellow]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_WarningPoint_Yellow]);
        sTotalImage[TextureType_WarningPoint_Yellow] = NULL;
    }

    sTotalImage[TextureType_WarningPoint_Red] = CImage_Create();
    sprintf(filepath, "%s/Warning/GY_red.raw", path);
    if (CImage_LoadData(sTotalImage[TextureType_WarningPoint_Red], filepath))
    {
        CImage_BindTexture(sTotalImage[TextureType_WarningPoint_Red]);
        sTotalImage[TextureType_WarningPoint_Red]->m_isShare = 1;
    }
    else
    {
        free(sTotalImage[TextureType_WarningPoint_Red]);
        sTotalImage[TextureType_WarningPoint_Red] = NULL;
    }

    sInit = 1;
}

pCImage TextureMgr_Get2DTerrainImage(char* name)
{
    pCImage image = CImage_Create();
    if (image)
    {
        char filepath[256] = { 0 };
        const char *path = GetTawsDataPath();

        sprintf(filepath, "%s/Texture/TEST/%s.raw", path, name);
        image->m_isLoadOverDel = 1;
        if (CImage_LoadData(image, filepath))
        {
            CImage_BindTexture(image);
        }
        else
        {
            CImage_Destory(&image);
            image = 0;
        }
    }
    return image;
}

pCImage TextureMgr_GetImage(TextureType type)
{
    TextureMgr_Init();
    return sTotalImage[type];
}


void TextureMgr_SetImage(TextureType type, pCImage p)
{
    switch (type)
    {
    case TextureType_3DTerrain:
        sTotalImage[TextureType_3DTerrain] = p;
        break;
    case TextureType_2DTerrain:
        sTotalImage[TextureType_2DTerrain] = p;
        break;
    default:
        break;
    }
}

pCImage TextureMgr_GetFlightPathImage()
{
    if (!sTotalImage[TextureType_Tall_FlightPath])
    {
        TextureMgr_Init();
    }
    return sTotalImage[TextureType_Tall_FlightPath];
}
