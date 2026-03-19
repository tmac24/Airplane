#pragma once

#include "HmsGlobal.h"
#include "math/HmsMath.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "HmsNavSvsObjectBase.h"

class CHmsNavSvsObjectPlane : public CHmsNavSvsObjectBase
{
public:
    CHmsNavSvsObjectPlane();
    ~CHmsNavSvsObjectPlane();
    virtual void Update(float delta) override;
    virtual void OnDraw() override;
    virtual void UpdateVertexData() override;

private:
    void ObjectPlaneInitVertex(int t, float xratio, float yratio, float zratio);

private:
    GLuint m_nVBO;
    GLuint m_nVBOIndex;
    int m_nVertexCount;
    int m_nIndexCount;
    V3F_T2F *m_pVertexData;
    GLuint *m_pIndexData;
    Texture2D *m_pTexture;
};



