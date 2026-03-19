#pragma once

#include "HmsGlobal.h"
#include "math/HmsMath.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "HmsNavSvsObjectBase.h"
#include <vector>

class CHmsNavSvsObjectRouteTrack : public CHmsNavSvsObjectBase
{
public:
    CHmsNavSvsObjectRouteTrack();
    ~CHmsNavSvsObjectRouteTrack();
    virtual void Update(float delta) override;
    virtual void OnDraw() override;
    virtual void UpdateVertexData() override;

    void InitUnitData(float length);
    void SetData(std::vector<PositionRouteViewData> &vecLonlat);

private:
    GLuint m_nVBO;
    GLuint m_nVBOIndex;

    std::vector<V3F_T2F> m_vecVertexData;
    std::vector<GLuint> m_vecIndexData;

    std::vector<V3F_T2F> m_vecUnitVertexData;
    std::vector<GLuint> m_vecUnitIndexData;

    std::vector<PositionRouteViewData> m_vecData;
    bool m_bDataDirty;

    Texture2D *m_pTexture;
};








