#pragma once

#include "mathd/Mat4d.h"
#include "mathd/Vec3d.h"
#include "mathd/Vec2d.h"
#include "math/HmsMath.h"
#include "base/HmsTypes.h"
#include "base/HmsAABB.h"
#include "HmsStandardDef.h"
#include "base/HmsImage.h"
#include <functional>
using namespace HmsAviPf;

class CHmsNavSvsTile;



typedef enum _SvsTileLoadEnum
{
    TILELOADENUM_LOAD_NONE = 0,
    TILELOADENUM_LOAD_SEND,
    TILELOADENUM_LOAD_FINISHED

}SvsTileLoadEnum;

struct SvsTileLayerInfo
{
    int nLayer;
    int nX;
    int nY;

    SvsTileLayerInfo()
    {
        nLayer = 0;
        nX = 0;
        nY = 0;
    }
    bool IsEqual(int nLayer, int nX, int nY)
    {
        return (this->nLayer == nLayer && this->nX == nX && this->nY == nY) ? true : false;
    }
    void Set(int nLayer, int nX, int nY)
    {
        this->nLayer = nLayer;
        this->nX = nX;
        this->nY = nY;
    }

};

/**/

struct SvsTileLoadStateStu
{
    SvsTileLoadEnum demState;
    SvsTileLayerInfo demLayerInfo;
    bool bInFrustum;

    SvsTileLoadEnum textureState;
    SvsTileLayerInfo textureLayerInfo;

    SvsTileLoadStateStu()
    {
        Init();
    }

    void Init()
    {
        demState = TILELOADENUM_LOAD_NONE;
        bInFrustum = false;
        textureState = TILELOADENUM_LOAD_NONE;
    }

};

/**/

typedef enum _SvsTileDrawState
{
    TILEDRAWSTATE_DRAWSELF = 0,
    TILEDRAWSTATE_DRAWCHILD,
    TILEDRAWSTATE_NODRAW,

}SvsTileDrawState;

struct SvsTileVertexStu
{
    Vec3 vertices;
    Tex2F texCoords;
    Color4F color;
    float altitude;
};

struct SvsLoadDemCallbackStu
{
    void *pObj;
    AABB aabb;
    int nLayer;
    int nX;
    int nY;
    bool bSuccess;
    SvsTileVertexStu *pVertexData;
};

struct SvsLoadDemTaskStu
{
    void *pObj;
    HmsUint64 idByLayer;
    int nLayer;
    int nX;
    int nY;
    int verticalsCnt;
    int numRows;
    int numCols;
    std::function<void(SvsLoadDemCallbackStu callbackStu)> callbackFunc;
};

struct SvsLoadImageCallbackStu
{
    void *pObj;
    Image *pImage;
    int nLayer;
    int nX;
    int nY;
};

struct SvsLoadImageTaskStu
{
    void *pObj;
    HmsUint64 idByLayer;
    int nLayer;
    int nX;
    int nY;
    std::function<void(SvsLoadImageCallbackStu callbackStu)> callbackFunc;
};

struct ObjectPoolLayerInfo
{
    int nLayer;
    int nX;
    int nY;
    bool bHasChild;
    CHmsNavSvsTile *pSvsTile;

    ObjectPoolLayerInfo()
    {
        nLayer = 0;
        nX = 0;
        nY = 0;
        bHasChild = false;
        pSvsTile = nullptr;
    }
};

struct SvsMgrLayerModelStu
{
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    ObjectPoolLayerInfo model[10][10];

    SvsMgrLayerModelStu()
    {
        xmin = 0;
        xmax = 0;
        ymin = 0;
        ymax = 0;
    }
};

enum class NavSvsCameraType
{
    SVSCAMERATYPE_FIRST = 0,
    SVSCAMERATYPE_THIRD,
    SVSCAMERATYPE_OVERLOOK,
    SVSCAMERATYPE_THIRD_AIRPORT,
    SVSCAMERATYPE_BUTT
};

enum class NavSvsPositionMgrType
{
    POSMGR_DATABUS = 0,
    POSMGR_FIX,
    POSMGR_ROUTE,
    POSMGR_ROAM
};

enum class NavSvsCtrlPageType
{
    CTRLPAGE_BASE = 0,
    CTRLPAGE_ROUTEVIEW,
    CTRLPAGE_ROAM
};

struct CalAttitudeStu
{
    Vec2d LngLat;
    float Height;
    float yaw;
    float pitch;
    float roll;
    float speed;

    CalAttitudeStu()
    {
        Height = 0;
        yaw = 0;
        pitch = 0;
        roll = 0;
        speed = 0;
    }
};

struct PositionRouteViewData
{
    double lon;
    double lat;
    double height;
    std::string ident;
    bool bGetTerrainAlt;

    PositionRouteViewData()
    {
        lon = 0;
        lat = 0;
        height = -1;
        bGetTerrainAlt = true;
    }
};

enum class NavSvsObjectType
{
    OBJECTTYPE_NONE = 0,
    OBJECTTYPE_PLANE,
    OBJECTTYPE_FLYPIPE,
    OBJECTTYPE_RUNWAY,
    OBJECTTYPE_ROUTETRACK,
    OBJECTTYPE_ADSB
};

struct SvsFlyPipeDataStu
{
    HmsAviPf::Vec2 lonLat;
    float height;
};