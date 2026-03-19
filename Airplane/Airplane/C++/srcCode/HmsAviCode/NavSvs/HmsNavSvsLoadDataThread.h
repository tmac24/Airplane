#pragma once

#include "math/HmsMath.h"
#include "base/HmsTypes.h"
#include "base/HmsAABB.h"
#include "base/HmsImage.h"
#include "HmsNavSvsTile.h"
#include "HmsNavSvsMathHelper.h"
#include "HmsNavSvsMgr.h"
#include "Nav/HmsMapTileSql.h"
#include "NavSvs/HmsNavSvsCommDef.h"
#include <vector>

class CHmsNavSvsLoadDataThread
{
public:
    static CHmsNavSvsLoadDataThread* GetInstance();

    void SetPlaneInfo(CalAttitudeStu *pStu);
    void CalFirstStrikePoint();

    void AddLoadDemTask(SvsLoadDemTaskStu *pTaskStu);
    void FilterSortDemTask();

    void AddLoadImageTask(SvsLoadImageTaskStu *pTaskStu);
    void FilterSortImageTask();

private:
    CHmsNavSvsLoadDataThread();
    ~CHmsNavSvsLoadDataThread();

    Image * GetTileImage(int nLayer, int nX, int nY, CHmsMapTileSql *pIns);
    void GetRowOrColumn(Vec3d *pData, int size, int nLayer, int nX, int nY, int nRowIndex, int nColumnIndex);
    void GetAltColor(Color4F *pColor, float alt);
    void CaculateNormal(const SvsLoadDemTaskStu *pTask, SvsTileVertexStu *pDstVertexData);
    void ThreadFunc();

private:
    std::vector<SvsLoadDemTaskStu> m_vecDemTask; //SvsLoadDemTaskStu
    bool m_bDemTaskNeedSort;

    std::vector<SvsLoadImageTaskStu> m_vecImageTask; //SvsLoadImageTaskStu
    bool m_bImageTaskNeedSort;
    CHmsMapTileSql* m_pMapTileSqlArray[8];

    std::mutex m_mutex;
    bool m_bRunning;

    CalAttitudeStu m_planeInfo;
    double m_strikeDisKM;
    int m_strikeSec;
};



