#include <mutex>
#include <HmsGE/HmsLog.h>
#include "AltManager.h"
#include <cmath>
#include "math/Vec2.h"
#include "DataInterface/TerrainServer/HmsTerrainServer.h"
#include "HmsGlobal.h"
#include "Global.h"
#include "Common.h"
#include "FileManager.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
}
#endif

static pCAltDataMgr  spAltmgr = nullptr;
static double unit = 1.0 / (double)TERRAIN2D_ALT_PRECISION;
static double unit_2d = 1.0 / (double)TERRAIN2D_ALT_PRECISION_2D;

std::mutex mutex_alt_cache;

pCAltDataMgr GetCAltDataMgr()
{
    if (!spAltmgr)
    {
        //spAltmgr = (pCAltDataMgr)malloc(sizeof(CAltDataMgr));
        //memset(spAltmgr, 0, sizeof(CAltDataMgr));
        //spAltmgr->m_cacheDataNum = 16;

        spAltmgr = new CAltDataMgr;
        if (spAltmgr)
        {
            spAltmgr->m_cacheDataNum = 16;
        }
    }
    return spAltmgr;
}

short isOceanToZero(short alt)
{
    switch (alt)
    {
    case -32768:
        return -1;
    case -500:
        return -1;
    default:
        return alt;
    }


#if TAWS_TERRAIN_JEP_DATA
    return  alt==-32768?-100:alt;
#else 
    if (alt == -32768)
    {
        alt = -1;
    }
    return  alt;
#endif	 		
}

unsigned char* CAltManager_ReadElevation(double lon, double lat)
{
    char buff[256] = { 0 }, name[32] = { 0 }, path2[32] = { 0 };
    unsigned char* data = NULL;
    unsigned int len;

    const char* taws_data_path = GetTawsDataPath();
    if (0 == taws_data_path)
    {
        return 0;
    }
#if TAWS_TERRAIN_JEP_DATA
    sprintf(path2, "JEP");
    sprintf(name, "jep_%03d_%03d.data", abs((int)lon), abs((int)lat));
#else
    sprintf(path2, "SRTM");
    sprintf(name, "srtm_%03d_%03d.data", abs((int)lon), abs((int)lat));
#endif
    sprintf(buff, "%s/Elevation/%s/%sx%sy/%s", taws_data_path, path2, lon >= 0 ? "+" : "-", lat >= 0 ? "+" : "-", name);

    if (!loadBinaryFile(buff, &data, &len))
    {
        //LogWrite(LOG_LEVEL_FATAL, "读取高程%s数据:---->失败! \n",buff);		
        printf("[SVS]Reading Elevation file [%s] ---->Failed! \n", buff);
    }
    return data;
}

pAltData GetAltDataBlock(double lon, double lat)
{
    int nLon = (int)floorf(lon);
    int nLat = (int)floorf(lat);

    pCAltDataMgr mgr = GetCAltDataMgr();
    if (mgr
        && mgr->m_curBlock
        && mgr->m_curBlock->lon == nLon
        && mgr->m_curBlock->lat == nLat
        )
    {
        return mgr->m_curBlock;
    }

    unsigned int index = 0;
    //unsigned int num = c_vector_size(mgr->m_altData);
    unsigned int num = mgr->m_altData2.size();
    //c_iterator iter;
    //c_iterator beg = c_vector_begin(mgr->m_altData);
    //c_iterator end = c_vector_end(mgr->m_altData);
    //for (iter = beg; !ITER_EQUAL(iter, end); ITER_INC(iter))
    //{
    //    pAltData pAD = (pAltData)(ITER_REF(iter));
    //    if ((pAD->lon == (int)lon) && (pAD->lat == (int)lat))
    //    {
    //        mgr->m_curBlock = pAD;
    //        return pAD;
    //    }
    //}

    for (unsigned int i = 0; i < num; i++)
    {
        auto pAD = mgr->m_altData2[i];
        if ((pAD->lon == nLon) && (pAD->lat == nLat))
        {
            mgr->m_curBlock = pAD;
            return pAD;
        }
    }

    return ReadAltDataBlock(lon, lat);
}

pAltData ReadAltDataBlock(double lon, double lat)
{
    pCAltDataMgr mgr = GetCAltDataMgr();

    pAltData altdata = NULL;
    int sLon = (int)floorf(lon);
    int sLat = (int)floorf(lat);
    /*读取新文件的数据*/

    unsigned char* orgdata = 0;
    unsigned int w = 0, h = 0, length = 0;
    altdata = AltData_Creatp();
    if (0 == altdata || 0 == mgr){
        return 0;
    }
    altdata->lon = sLon;
    altdata->lat = sLat;
    altdata->data = CAltManager_ReadElevation(lon, lat);
    if (altdata->data == NULL)
    {
        altdata->isExist = 0;
        printf("[SVS] read data, Elevation data %d,%d not exist\n", sLon, sLat);
    }
    else
    {
        orgdata = altdata->data;
        memcpy_op(&w, orgdata, 4);
        memcpy_op(&h, orgdata + 4, 4);
        //	 printf("ReadAltDataBlock %d,%d地形数据:%d宽,%d高\n",sLon,sLat,w,h);

        length = w*h * 2;
        altdata->data = (unsigned char*)malloc(length);
        if (altdata->data)
        {
            memset(altdata->data, 0, length);
            altdata->count = w;
            altdata->unit = 1.0 / (float)(altdata->count);
            memcpy_op(&(altdata->averageAlt), orgdata + 8, 2);
            memcpy_op(altdata->data, orgdata + 8, length);
        }
        free(orgdata);
    }

    //unsigned int blockNum = c_vector_size(mgr->m_altData);
    unsigned int blockNum = mgr->m_altData2.size();
    /*存储到vector*/
    if (blockNum >= mgr->m_cacheDataNum)//缓冲数据存满了
    {
        //c_iterator beg = c_vector_begin(mgr->m_altData);
        //c_iterator end = c_vector_end(mgr->m_altData);
        //if (!ITER_EQUAL(beg, end))
        //{
        //    pAltData ch = (pAltData)(ITER_REF(beg));
        //    if (ch->data)
        //    {
        //        free(ch->data);
        //        ch->data = NULL;
        //    }
        //    free(ch);
        //    /*删除第一个数据*/
        //    c_vector_erase(mgr->m_altData, beg);
        //}           

        //auto end = mgr->m_altData2.end();

        //if (beg != end)
        if (!mgr->m_altData2.empty())
        {
            //auto ch = *beg;
            auto beg = mgr->m_altData2.begin();
            auto ch = *beg;

            if (ch->data)
            {
                free(ch->data);
                ch->data = NULL;
            }
            free(ch);
            /*删除第一个数据*/
            //c_vector_erase(mgr->m_altData, beg);            
            mgr->m_altData2.erase(beg);
        }
    }
    //c_vector_push_back(mgr->m_altData, altdata);
    mgr->m_altData2.push_back(altdata);
    mgr->m_curBlock = altdata;

    return altdata;
}

int CAltManager_GetAltByLonLat(double lon, double lat)
{
    short altitude = 0;
#if GLOBAL_ALT_DATA_SOURCE == 1
    HmsAviPf::Vec2 pos = HmsAviPf::Vec2(lon, lat);
    altitude = CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude(&pos);
#else// GLOBAL_ALT_DATA_SOURCE == 0
    mutex_alt_cache.lock();//TODO, need optimization

    pAltData data = GetAltDataBlock(lon, lat);
#if !TERRAIN2D_OPTMIZITION
    if (data && data->isExist)
    {
        altitude = GetAnysisBlockAlt(data,lon, lat);
    }
#else
    if (data && data->isExist)
    {
        int w = (int)((lon - data->lon) / unit);
        int h = (int)((lat - data->lat) / unit);
        int index = w + h * TERRAIN2D_ALT_PRECISION;

        if (index < 0 || index >= sizeof(data->altcache))
        {
            HMSLOG("ERROR, altcache idx out of range. %d, %f, %f, %d, %d\n", index, lon, lat, data->lon, data->lat);
        }
        else
        {
            if (data->altcache[index].init)
            {
                altitude = data->altcache[index].alt;
            }
            else
            {
                data->altcache[index].alt = GetAnysisBlockAlt(data, data->lon + unit*w, data->lat + unit*h);
                data->altcache[index].init = 1;
                altitude = data->altcache[index].alt;
            }
        }
    }
#endif

    mutex_alt_cache.unlock();
#endif// GLOBAL_ALT_DATA_SOURCE == 0
    return altitude;
}

short GetAnysisBlockAlt(pAltData m_altData, double lon, double lat)
{
#if TAWS_AIRPORT_ZUUU 
    unsigned char isShuangLiu = 0;
#endif

#if TAWS_AIRPORT_ZUCK 
    unsigned char isQiaoBei = 0;
#endif

#if TAWS_AIRPORT_ZSSS
    unsigned char isPudong = 0;
#endif

    int sLon = (int)floorf(lon);
    int sLat = (int)floorf(lat);

    //pCAltDataMgr mgr = GetCAltDataMgr();
    //m_altData = mgr->m_curBlock;

    //return 0;
#if TAWS_AIRPORT_ZUUU 
    if ((103 == (int)lon) && (30 == (int)lat))
    {
        isShuangLiu = 1;
    }
#endif

#if TAWS_AIRPORT_ZUCK 
    if ((106 == (int)lon) && (29 == (int)lat))
    {
        isQiaoBei = 1;
    }
#endif

#if TAWS_AIRPORT_ZSSS 
    if ((121 == (int)lon) && (31 == (int)lat))
    {
        isPudong = 1;
    }
#endif 

    /*从数据中找到海拔*/
    int alt = 0;
    pAltData ptr = m_altData;
    if (ptr)
    {
        double clon = fabs(lon - sLon) / ptr->unit;
        double clat = fabs(lat - sLat) / ptr->unit;
        int cILon = clon;
        int cILat = clat;
        int tmplon, tmplat;
        short alt1, alt2, alt3, alt4;

        memcpy_op(&alt1, ptr->data + cILat*ptr->count * 2 + cILon * 2, 2);
        alt1 = isOceanToZero(alt1);

        tmplon = clon + 1;
        if (tmplon >= ptr->count)
        {
            alt2 = alt1;
        }
        else
        {
            memcpy_op(&alt2, ptr->data + cILat*ptr->count * 2 + tmplon * 2, 2);
            alt2 = isOceanToZero(alt2);
        }

        tmplat = cILat + 1;
        if (tmplat >= ptr->count)
        {
            alt3 = alt1;
        }
        else
        {
            memcpy_op(&alt3, ptr->data + tmplat*ptr->count * 2 + cILon * 2, 2);
            alt3 = isOceanToZero(alt3);
        }

        if (tmplon >= ptr->count || tmplat >= ptr->count)
        {
            alt4 = alt1;
        }
        else
        {
            memcpy_op(&alt4, ptr->data + tmplat*ptr->count * 2 + tmplon * 2, 2);
            alt4 = isOceanToZero(alt4);
        }

        /**/
        double lonValue = clon - cILon, latValue = clat - cILat;
        if (lonValue > latValue)
        {
            alt = alt1 + (alt2 - alt1)*lonValue + (alt4 - alt2)*latValue;
        }
        else
        {
            alt = alt1 + (alt4 - alt3)*lonValue + (alt3 - alt1)*latValue;
        }

#if TAWS_AIRPORT_ZUUU 
        if (isShuangLiu)
        {
            //alt -= 18;
        }
#endif
#if TAWS_AIRPORT_ZUCK 
        if (isQiaoBei == 1)
        {
            alt -= 8;
        }
#endif
#if TAWS_AIRPORT_ZSSS
        if (isPudong)
        {
            if (!(alt == -1))
            {
                if (alt <= 50)
                {
                    alt = 0;
                }
            }
        }
#endif
    }
    return alt;
}

pAltData  AltData_Creatp()
{
    pAltData alt = (pAltData)malloc(sizeof(AltData));
    if (alt)
    {
        memset(alt, 0, sizeof(AltData));
        alt->isExist = 1;
    }

    return alt;
}
