#pragma once

#include "HmsGlobal.h"
#include "math/HmsMath.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "HmsNavSvsObjectBase.h"

struct SvsLogicRunwayStu
{
    char ident[8];
    Vec2d lonLat;
    double height;
    double bearing;
};

struct SvsRunwayDataStu
{
    SvsLogicRunwayStu runwayOne;
    SvsLogicRunwayStu runwayTwo;
    
    float width;
    float length;
   
    int indexOffsetInVbo;
};

class CHmsNavSvsObjectRunway : public CHmsNavSvsObjectBase
{
public:
    CHmsNavSvsObjectRunway();
    ~CHmsNavSvsObjectRunway();
    virtual void Update(float delta) override;
    virtual void OnDraw() override;
    virtual void UpdateVertexData() override;

    void InitUnitData(const SvsRunwayDataStu *pRunwayData);
    void SetData(SvsRunwayDataStu *pData, int dataSize);
    void CalRunwayToDraw();

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

    SvsRunwayDataStu *m_pRunwayData;
    int m_nRunwayDataSize;
    bool m_bRunwayDataDirty;

    int m_indexRunwayToDraw[50];

    Texture2D *m_pTexture;
};






