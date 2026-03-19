#include "HmsNavSvsLoadDataThread.h"
#include "../DataInterface/TerrainServer/HmsTerrainServer.h"
#include "base/HmsTimer.h"
#include "Calculate/CalculateZs.h"

#include <thread>
#include <mutex>

#define VEC3D_TO_VEC3F(a,b) a.x = b.x;a.y=b.y;a.z=b.z

CHmsNavSvsLoadDataThread::CHmsNavSvsLoadDataThread()
{
    m_bDemTaskNeedSort = false;
    m_bImageTaskNeedSort = false;
    
    int len = sizeof(m_pMapTileSqlArray) / sizeof(m_pMapTileSqlArray[0]);
    for (int i = 0; i < len; ++i)
    {
        m_pMapTileSqlArray[i] = CHmsMapTileSql::CreateInstance();
        m_pMapTileSqlArray[i]->ChangeMapTypeName("satellite");
    }

    m_bRunning = true;
    new std::thread(&CHmsNavSvsLoadDataThread::ThreadFunc, this);
}

CHmsNavSvsLoadDataThread::~CHmsNavSvsLoadDataThread()
{
}

Image * CHmsNavSvsLoadDataThread::GetTileImage(int nLayer, int nX, int nY, CHmsMapTileSql *pIns)
{
    if (!pIns)
    {
        return NULL;
    }
    Image* pImage = NULL;
    do
    {
        pImage = new Image();
        bool ret = pIns->GetImageFromDB(nLayer, nX, nY, pImage);
        if (!ret)
        {
            HMS_SAFE_RELEASE(pImage);
            return nullptr;
        }
    } while (0);

    return pImage;
}

void CHmsNavSvsLoadDataThread::GetRowOrColumn(Vec3d *pData, int size, int nLayer, int nX, int nY, int nRowIndex, int nColumnIndex)
{
    int nNumRows = size;
    int nNumCols = size;

    int nLayerCnt = 1 << nLayer;
    int nLayerHalfCnt = 1 << (nLayer - 1);

    double xStart = (nX - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
    double xEnd = (nX + 1 - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
    double xLength = xEnd - xStart;

    double yStart = (nY - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
    double yEnd = (nY + 1 - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
    double yLength = yEnd - yStart;

    int rowStart, rowEnd;
    int colStart, colEnd;
    if (nRowIndex >= 0)
    {
        rowStart = nRowIndex;
        rowEnd = nRowIndex + 1;
        colStart = 0;
        colEnd = nNumCols;
    }
    else if(nColumnIndex >= 0)
    {
        rowStart = 0;
        rowEnd = nNumRows;
        colStart = nColumnIndex;
        colEnd = nColumnIndex + 1;
    }
    else
    {
        rowStart = 0;
        rowEnd = 0;
        colStart = 0;
        colEnd = 0;
    }
    int index = 0;
    for (int row = rowStart; row < rowEnd; ++row)
    {
        double ndc_y = ((double)row) / (double)(nNumRows - 1);
        double yTemp = yStart + yLength*ndc_y;
        double dLat = CHmsNavSvsMathHelper::MercatorPosYToLatRadian(yTemp);

        for (int col = colStart; col < colEnd; ++col)
        {
            double ndc_x = ((double)col) / (double)(nNumCols - 1);
            double dLong = (xStart + xLength * ndc_x);

            Vec2 ll = Vec2(MATH_RAD_TO_DEG_DOUBLE(dLong), MATH_RAD_TO_DEG_DOUBLE(-dLat));

            double alt = CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude(&ll);

            Vec2d lonlat = Vec2d(MATH_RAD_TO_DEG_DOUBLE(dLong), MATH_RAD_TO_DEG_DOUBLE(-dLat));
            Vec3d xyz = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat, alt);
            pData[index++] = xyz;
        }
    }
}

static float s_AltColor_Alt[16];
static Color4F s_AltColor_Color[16];

void CHmsNavSvsLoadDataThread::GetAltColor(Color4F *pColor, float alt)
{
    static bool bAltColorNeedInit = true;
    if (bAltColorNeedInit)
    {
        bAltColorNeedInit = false;
        int index = 0;
        s_AltColor_Color[index] = Color4F(0.57f, 0.85f, 0.15f, 1.0f); s_AltColor_Alt[index] = 1; ++index;
        s_AltColor_Color[index] = Color4F(0.02f, 0.44f, 0.33f, 1.0f); s_AltColor_Alt[index] = 10; ++index;
        s_AltColor_Color[index] = Color4F(0.04f, 0.62f, 0.46f, 1.0f); s_AltColor_Alt[index] = 20; ++index;
        s_AltColor_Color[index] = Color4F(0.04f, 0.69f, 0.46f, 1.0f); s_AltColor_Alt[index] = 30; ++index;
        s_AltColor_Color[index] = Color4F(0.04f, 0.81f, 0.46f, 1.0f); s_AltColor_Alt[index] = 50; ++index;
        s_AltColor_Color[index] = Color4F(0.24f, 0.60f, 0.14f, 1.0f); s_AltColor_Alt[index] = 70; ++index;
        s_AltColor_Color[index] = Color4F(0.41f, 0.62f, 0.16f, 1.0f); s_AltColor_Alt[index] = 90; ++index;
        s_AltColor_Color[index] = Color4F(0.43f, 0.64f, 0.18f, 1.0f); s_AltColor_Alt[index] = 200; ++index;
        s_AltColor_Color[index] = Color4F(0.64f, 0.66f, 0.18f, 1.0f); s_AltColor_Alt[index] = 400; ++index;
        s_AltColor_Color[index] = Color4F(0.85f, 0.79f, 0.21f, 1.0f); s_AltColor_Alt[index] = 600; ++index;
        s_AltColor_Color[index] = Color4F(0.70f, 0.42f, 0.17f, 1.0f); s_AltColor_Alt[index] = 800; ++index;
        s_AltColor_Color[index] = Color4F(0.43f, 0.43f, 0.43f, 1.0f); s_AltColor_Alt[index] = 1000; ++index;
        s_AltColor_Color[index] = Color4F(0.94f, 0.98f, 0.63f, 1.0f); s_AltColor_Alt[index] = 2000; ++index;
        s_AltColor_Color[index] = Color4F(0.90f, 0.86f, 0.67f, 1.0f); s_AltColor_Alt[index] = 4000; ++index;
        s_AltColor_Color[index] = Color4F(0.86f, 0.86f, 0.86f, 1.0f); s_AltColor_Alt[index] = 6000; ++index;
        s_AltColor_Color[index] = Color4F(0.98f, 0.98f, 0.98f, 1.0f); s_AltColor_Alt[index] = 20000; ++index;
    }
    for(int i = 0; i < sizeof(s_AltColor_Alt) / sizeof(s_AltColor_Alt[0]); ++i)
    {
        if (alt < s_AltColor_Alt[i])
        {
            *pColor = s_AltColor_Color[i];
            return;
        }
    }
}

void CHmsNavSvsLoadDataThread::CaculateNormal(const SvsLoadDemTaskStu *pTask, SvsTileVertexStu *pDstVertexData)
{
    Vec3d pVertexNormal[289];
    memset(pVertexNormal, 0, sizeof(pVertexNormal));

    Vec3d pVertexData[289];
    memset(pVertexData, 0, sizeof(pVertexData));
    Vec3d pLeftColumn[17];
    Vec3d pRightColumn[17];
    Vec3d pTopRow[17];
    Vec3d pBottomRow[17];
    GetRowOrColumn(pLeftColumn, pTask->numRows - 2, pTask->nLayer, pTask->nX - 1, pTask->nY, -1, pTask->numCols - 2 - 1 - 1);
    GetRowOrColumn(pRightColumn, pTask->numRows - 2, pTask->nLayer, pTask->nX + 1, pTask->nY, -1, 1);
    GetRowOrColumn(pTopRow, pTask->numRows - 2, pTask->nLayer, pTask->nX, pTask->nY - 1, pTask->numRows - 2 - 1 - 1, -1);
    GetRowOrColumn(pBottomRow, pTask->numRows - 2, pTask->nLayer, pTask->nX, pTask->nY + 1, 1, -1);

    for (int row = 0; row < pTask->numRows; ++row)
    {
        for (int col = 0; col < pTask->numCols; ++col)
        {
            int iv = row * (pTask->numCols) + col;
            if (row == 0)
            {
                if (col >= 1 && col <= pTask->numCols - 2)
                {
                    VEC3D_TO_VEC3F(pVertexData[iv], pTopRow[col - 1]);
                }
            }
            else if (row == pTask->numRows - 1)
            {
                if (col >= 1 && col <= pTask->numCols - 2)
                {
                    VEC3D_TO_VEC3F(pVertexData[iv], pBottomRow[col - 1]);
                }
            }
            else
            {
                if (col == 0)
                {
                    VEC3D_TO_VEC3F(pVertexData[iv], pLeftColumn[row - 1]);
                }
                else if (col == pTask->numCols - 1)
                {
                    VEC3D_TO_VEC3F(pVertexData[iv], pRightColumn[row - 1]);
                }
                else
                {
                    VEC3D_TO_VEC3F(pVertexData[iv], pDstVertexData[iv].vertices);
                }
            }
        }
    }

    Vec2d ll = { 160, 20 };
    Vec3d lightPosition = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&ll, 20000);

    Vec3d p0, p1, p2, p3, p4, p5, p6;
    Vec3d sub10, sub20, sub30, sub40, sub50, sub60;
    Vec3d cross21, cross32, cross43, cross54, cross65, cross16;
    unsigned int iv = 0;
    for (int row = 1; row <= pTask->numRows - 2; ++row)
    {
        for (int col = 1; col <= pTask->numCols - 2; ++col)
        {
            iv = row * pTask->numCols + col;
            VEC3D_TO_VEC3F(p0, pVertexData[iv]);
            VEC3D_TO_VEC3F(p1, pVertexData[iv - 1]);
            VEC3D_TO_VEC3F(p2, pVertexData[iv - pTask->numCols]);
            VEC3D_TO_VEC3F(p3, pVertexData[iv - pTask->numCols + 1]);
            VEC3D_TO_VEC3F(p4, pVertexData[iv + 1]);
            VEC3D_TO_VEC3F(p5, pVertexData[iv + pTask->numCols]);
            VEC3D_TO_VEC3F(p6, pVertexData[iv + pTask->numCols - 1]);
            sub10 = p1 - p0;
            sub20 = p2 - p0;
            sub30 = p3 - p0;
            sub40 = p4 - p0;
            sub50 = p5 - p0;
            sub60 = p6 - p0;
            Vec3d::cross(sub20, sub10, &cross21);
            Vec3d::cross(sub30, sub20, &cross32);
            Vec3d::cross(sub40, sub30, &cross43);
            Vec3d::cross(sub50, sub40, &cross54);
            Vec3d::cross(sub60, sub50, &cross65);
            Vec3d::cross(sub10, sub60, &cross16);
            cross21.normalize();
            cross32.normalize();
            cross43.normalize();
            cross54.normalize();
            cross65.normalize();
            cross16.normalize();
            Vec3d vertexNormal = cross21;
            vertexNormal = vertexNormal + cross32;
            vertexNormal = vertexNormal + cross43;
            vertexNormal = vertexNormal + cross54;
            vertexNormal = vertexNormal + cross65;
            vertexNormal = vertexNormal + cross16;
            vertexNormal.normalize();
            pVertexNormal[iv] = vertexNormal;
        }
    }

    for (int i = 0; i < c_CHmsNavSvsTile_vertexCount; ++i)
    {
        Vec3d *pNormal = &pVertexNormal[i];
        GetAltColor(&pDstVertexData[i].color, pDstVertexData[i].altitude);
        if (pNormal->x == 0)
        {
            pDstVertexData[i].color.r = 0.0;
            pDstVertexData[i].color.g = 0.0;
            pDstVertexData[i].color.b = 0.0;
            pDstVertexData[i].color.a = 0.0;
        }
        else
        {
            Vec3d vertex;
            VEC3D_TO_VEC3F(vertex, pDstVertexData[i].vertices);
            Vec3d lightDirect = lightPosition - vertex;
            lightDirect.normalize();

            double dotL = pVertexNormal[i].dot(lightDirect);
            dotL = HMS_MAX(0, dotL);

            pDstVertexData[i].color.r *= dotL;
            pDstVertexData[i].color.g *= dotL;
            pDstVertexData[i].color.b *= dotL;
            pDstVertexData[i].color.a *= dotL;
        }
    }
}

void CHmsNavSvsLoadDataThread::ThreadFunc()
{
    CHmsTime runTime;
    runTime.RecordCurrentTime();

    SvsTileVertexStu *pVertexData = nullptr;

    while (m_bRunning)
    {
#if 0
        //calculate first strike point
        if (runTime.GetElapsed() > 3)
        {
            runTime.RecordCurrentTime();
            CalFirstStrikePoint();
        }
#endif
        //load dem
        while (!m_bDemTaskNeedSort && !m_vecDemTask.empty())
        {
            m_mutex.lock();

            SvsLoadDemTaskStu task = m_vecDemTask.back();
            m_vecDemTask.pop_back();

            m_mutex.unlock();

            if (!pVertexData)
            {
                pVertexData = new SvsTileVertexStu[task.verticalsCnt];
            }
            //printf("\n deal dem task %d %d %d \n", task.nLayer, task.nX, task.nY);

            AABB retAABB;
            int nLayer = task.nLayer;
            int nX = task.nX;
            int nY = task.nY;

            int nNumRows = task.numRows - 2;//有裙边所以-2
            int nNumCols = task.numCols - 2;

            int nLayerCnt = 1 << nLayer;
            int nLayerHalfCnt = 1 << (nLayer - 1);

            double xStart = (nX - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
            double xEnd = (nX + 1 - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
            double xLength = xEnd - xStart;

            double yStart = (nY - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
            double yEnd = (nY + 1 - nLayerHalfCnt)*SIM_Pi / nLayerHalfCnt;
            double yLength = yEnd - yStart;
      
            Vec3d x = { 1.0, 0.0, 0.0 };
            Vec3d y = { 0.0, 1.0, 0.0 };
            Vec3d z = { 0.0, 0.0, 1.0 };

            for (int j = 0; j < nNumRows; ++j)
            {
                double ndc_y = ((double)j) / (double)(nNumRows - 1);
                double yTemp = yStart + yLength*ndc_y;
                double dLat = CHmsNavSvsMathHelper::MercatorPosYToLatRadian(yTemp);

                for (int i = 0; i < nNumCols; ++i)
                {
                    double ndc_x = ((double)i) / (double)(nNumCols - 1);
                    double dLong = (xStart + xLength * ndc_x);

                    //计算贴图坐标
                    unsigned int iv = (j + 1)*(nNumCols + 2) + (i + 1);

                    SvsTileVertexStu *pTempPos = &pVertexData[iv];
                    pTempPos->texCoords.u = ndc_x;
                    pTempPos->texCoords.v = 1.0 - ndc_y;

                    Vec2 ll = Vec2(MATH_RAD_TO_DEG_DOUBLE(dLong), MATH_RAD_TO_DEG_DOUBLE(-dLat));

                    double alt = CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude(&ll);
                    pTempPos->altitude = alt;

                    Vec2d lonlat = Vec2d(MATH_RAD_TO_DEG_DOUBLE(dLong), MATH_RAD_TO_DEG_DOUBLE(-dLat));
                    Vec3d xyz = CHmsNavSvsMathHelper::LonLatAltToXYZ_Double(&lonlat, alt);
                    VEC3D_TO_VEC3F(pTempPos->vertices, xyz);

                    retAABB.updateMinMax(&pTempPos->vertices, 1);
                }
            }
            //添加裙边遮挡裂缝
            for (int j = 0; j < nNumRows + 2; ++j)
            {
                if (j == 0 || j == nNumRows + 1)
                {
                    for (int i = 0; i < nNumCols + 2; ++i)
                    {
                        unsigned int iv = (j)*(nNumCols + 2) + i;
                        unsigned int next = 0;
                        if (j == 0)
                        {
                            next = (j + 1)*(nNumCols + 2) + i;
                        }
                        else
                        {
                            next = (j - 1)*(nNumCols + 2) + i;
                        }
                        pVertexData[iv] = pVertexData[next];
                        pVertexData[iv].vertices.normalize();
                    }
                }
            }
            for (int j = 0; j < nNumRows + 2; ++j)
            {
                for (int i = 0; i < nNumCols + 2; ++i)
                {
                    if (i == 0 || i == nNumCols + 1)
                    {
                        unsigned int iv = (j)*(nNumCols + 2) + i;
                        unsigned int next = 0;
                        if (i == 0)
                        {
                            next = j*(nNumCols + 2) + i + 1;
                        }
                        else
                        {
                            next = j*(nNumCols + 2) + i - 1;
                        }
                        pVertexData[iv] = pVertexData[next];
                        pVertexData[iv].vertices.normalize();
                    }
                }
            }	

            CaculateNormal(&task, pVertexData);

            if (task.callbackFunc)
            {
                SvsLoadDemCallbackStu callbackStu;
                callbackStu.pObj = task.pObj;
                callbackStu.aabb = retAABB;
                callbackStu.nLayer = task.nLayer;
                callbackStu.nX = task.nX;
                callbackStu.nY = task.nY;
                callbackStu.bSuccess = true;
                callbackStu.pVertexData = pVertexData;

                task.callbackFunc(callbackStu);
            }
        }

        //load image
        while (!m_bImageTaskNeedSort && !m_vecImageTask.empty())
        {
            m_mutex.lock();

            SvsLoadImageTaskStu task = m_vecImageTask.back();
            m_vecImageTask.pop_back();

            m_mutex.unlock();

            //printf("\n deal image task %d %d %d \n", task.nLayer, task.nX, task.nY);

            Image *p = GetTileImage(task.nLayer, task.nX, task.nY, m_pMapTileSqlArray[task.nLayer - 7]);
            if (task.callbackFunc)
            {
                SvsLoadImageCallbackStu callbackStu;
                callbackStu.pObj = task.pObj;
                callbackStu.pImage = p;
                callbackStu.nLayer = task.nLayer;
                callbackStu.nX = task.nX;
                callbackStu.nY = task.nY;

                task.callbackFunc(callbackStu);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

CHmsNavSvsLoadDataThread* CHmsNavSvsLoadDataThread::GetInstance()
{
    static CHmsNavSvsLoadDataThread *s_pIns = nullptr;
    if (!s_pIns)
    {
        s_pIns = new CHmsNavSvsLoadDataThread;
    }
    return s_pIns;
}

void CHmsNavSvsLoadDataThread::SetPlaneInfo(CalAttitudeStu *pStu)
{
    m_planeInfo = *pStu;
}

void CHmsNavSvsLoadDataThread::CalFirstStrikePoint()
{
    m_strikeDisKM = -1;
    m_strikeSec = -1;

    Vec2 lonlatStart = Vec2(m_planeInfo.LngLat.x, m_planeInfo.LngLat.y);
    Vec2 lonlatEnd = CHmsNavSvsMathHelper::NextPointLonLatByBearing(&lonlatStart, 100, m_planeInfo.yaw);
    if (m_planeInfo.speed > 0)
    { 
        std::vector<Vec2> vec;
        CHmsNavSvsMathHelper::InsertLonLat(&lonlatStart, &lonlatEnd, vec, 0.09F);
        int len = vec.size();
        for (int i = 1; i < len - 1; ++i)
        {
            Vec2 *pPos = &vec[i];
            double alt = CHmsGlobal::GetInstance()->GetTerrainServer()->GetAltitude(pPos);

            if (alt > m_planeInfo.Height)
            {
                m_strikeDisKM = CalculateDistanceKM(pPos->x, pPos->y, m_planeInfo.LngLat.x, m_planeInfo.LngLat.y);
                m_strikeSec = m_strikeDisKM / KnotToKMH(m_planeInfo.speed) * 3600;
                break;
            }
        }
    }
}

void CHmsNavSvsLoadDataThread::AddLoadDemTask(SvsLoadDemTaskStu *pTaskStu)
{
    m_mutex.lock();
    
    m_vecDemTask.push_back(*pTaskStu);
    m_bDemTaskNeedSort = true;

    //printf("\n add dem task %d %d %d \n", pTaskStu->nLayer, pTaskStu->nX, pTaskStu->nY);

    m_mutex.unlock();
}

void CHmsNavSvsLoadDataThread::FilterSortDemTask()
{
    if (m_bDemTaskNeedSort)
    {
        m_mutex.lock();

        m_bDemTaskNeedSort = false;
#if 0
        std::sort(m_vecDemTask.begin(), m_vecDemTask.end(), 
            [=](const SvsLoadDemTaskStu &stuA, const SvsLoadDemTaskStu &stuB){
            return stuA.idByLayer < stuB.idByLayer;
        });
#endif

        m_mutex.unlock();
    }
}

/*imagetask*/

void CHmsNavSvsLoadDataThread::AddLoadImageTask(SvsLoadImageTaskStu *pTaskStu)
{
    m_mutex.lock();

    m_vecImageTask.push_back(*pTaskStu);
    m_bImageTaskNeedSort = true;

    //printf("\n add image task %d %d %d \n", pTaskStu->nLayer, pTaskStu->nX, pTaskStu->nY);

    m_mutex.unlock();
}

void CHmsNavSvsLoadDataThread::FilterSortImageTask()
{
    if (m_bImageTaskNeedSort)
    {
        m_mutex.lock();

        m_bImageTaskNeedSort = false;

#if 0
        std::sort(m_vecImageTask.begin(), m_vecImageTask.end(),
            [=](const SvsLoadImageTaskStu &stuA, const SvsLoadImageTaskStu &stuB){
            return stuA.idByLayer < stuB.idByLayer;
        });
#endif

        m_mutex.unlock();
    } 
}
