#pragma once

#include "../HmsGlobal.h"
#include "math/HmsMath.h"
#include "math/HmsGeometry.h"
#include "base/HmsNode.h"
#include "HmsNavSvsTile.h"
#include "base/HmsCamera.h"
#include "render/HmsCustomCommand.h"
#include "HmsNavSvsMathHelper.h"
#include "render/HmsTexture2D.h"
#include <vector>
#include <map>
#include "HmsNavSvsCommDef.h"

#define SVS_MGR_LAYER_MIN 10
#define SVS_MGR_LAYER_MAX 14

#define SVS_TEN_LAYER_OFFSET 4

class CHmsNavSvsObjectMgr;

class CHmsNavSvsMgr : public CHmsNode
{
public:
    static CHmsNavSvsMgr* GetInstance();

    virtual void Update(float delta) override;
    virtual void Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags) override;
    virtual void Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags) override;

    bool IsEnableSatelliteModel();
    void SetSatelliteModel(bool b);
    bool IsEnableTerrainAlarm();
    void SetTerrainAlarm(bool b);

    void MoveCameraDirectTarget(float speed);
    void MoveCameraUpOrDown(float step);

    const CalAttitudeStu* GetAttitudeStu();
    const Mat4d* GetMat4dPVRelative();
    const Mat4d* GetMat4dTrans();
    const Mat4d* GetPlaneModel();

private:
    CHmsNavSvsMgr();
    ~CHmsNavSvsMgr();
    void InitContent(Size size);
    void OnDraw(const Mat4 &transform, uint32_t flags);
    bool IsLonLatInValidRange(double lon, double lat);
    void GetMapDataRange();

private:
    Frustum m_frustum;

    SvsMgrLayerModelStu m_layerModel[SVS_MGR_LAYER_MAX - SVS_MGR_LAYER_MIN + 1];
    std::vector<CHmsNavSvsTile*> m_pVecBackgroundTiles;
    std::vector<CHmsNavSvsTile*> m_pVecPrepareDraw;

    int m_layerMin;
    int m_layerMax;

    Mat4d m_mat4dTrans;
    Mat4d m_mat4dP;
    Mat4d m_mat4dVRelative;
    Mat4d m_mat4dPVRelative;
    Mat4d m_mat4dVWorld;
    Mat4d m_mat4dPVWorld;

    Vec3d m_relativeOrigin;

    bool m_bEnableSatelliteModel;
    bool m_bEnableTerrainAlarm;
    bool m_bEnableLightColor;
    Texture2D *m_pTextureDetail;

    GLuint m_nVBO;
    GLuint m_nVBOIndex;

    GLuint m_locAttrColorLight;
    GLuint m_locAttrAltitude;
    GLuint m_locUniformEnableTerrainAlarm;
    GLuint m_locUniformEnableLightColor;
    GLuint m_locUniformPlaneAltitude;
    GLuint m_locUniformRelativeOrigin;

    CustomCommand m_customCommand;

    CalAttitudeStu m_attitudeStu;

    CHmsNavSvsObjectMgr *m_pSvsObjectMgr;

    double m_mapDataMinLon;
    double m_mapDataMaxLon;
    double m_mapDataMinLat;
    double m_mapDataMaxLat;

    friend class CHmsNavSvsLayer;
    friend class CHmsNavSvsObjectMgr;
};