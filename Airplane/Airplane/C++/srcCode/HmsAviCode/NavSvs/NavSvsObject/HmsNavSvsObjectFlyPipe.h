#pragma once

#include "HmsGlobal.h"
#include "math/HmsMath.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "HmsNavSvsObjectBase.h"

class CHmsNavSvsObjectFlyPipe : public CHmsNavSvsObjectBase
{
public:
    CHmsNavSvsObjectFlyPipe();
    ~CHmsNavSvsObjectFlyPipe();
    virtual void Update(float delta) override;
    virtual void OnDraw() override;
    virtual void UpdateVertexData() override;

    void InitUnitData(double scale, double space);
    void SetData(SvsFlyPipeDataStu *pData, int dataSize, float rnp); //NM
    void SetRnp(double rnp);
    void CalCurPipeDataIndex();
    void CalCurOffsetDistance();

    void SetFlyPipeData(std::vector<PositionRouteViewData> &vecLonlat);

private:
    GLuint m_nVBO;
    GLuint m_nVBOIndex;
    int m_nVertexCount;
    int m_nIndexCount;
    V3F_T2F *m_pVertexData;
    GLuint *m_pIndexData;

    int m_nUnitVertexCount;
    V3F_T2F *m_pUnitVertexData;
    int m_nUnitIndexCount;
    GLuint *m_pUnitIndexData;

    SvsFlyPipeDataStu *m_pPipeData;
    int m_nPipeDataSize;
    float m_fRnp;
    bool m_bPipeDataDirty;
    int m_nCurPipeDataIndex;

    bool m_bDrawFlyPipe;
    bool m_bTwinkleFlyPipe;
    bool m_bPlaneOutFlyPipe;

    Texture2D *m_pTexture;
};






