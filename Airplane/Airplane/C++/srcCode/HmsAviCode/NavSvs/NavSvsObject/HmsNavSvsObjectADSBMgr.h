#pragma once

#include "HmsGlobal.h"
#include "math/HmsMath.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "HmsNavSvsObjectBase.h"
#include <vector>

#include "HmsNavSvsLabel.h"

struct NavSvsObjectADSBStu
{
    std::string ident;
    double lon;
    double lat;
    float height;

    CHmsNavSvsLabel *pLabel;

    NavSvsObjectADSBStu()
    {
        lon = 0;
        lat = 0;
        height = 0;
        pLabel = nullptr;
    }
};

class CHmsNavSvsObjectADSBMgr : public CHmsNavSvsObjectBase
{
public:
    CHmsNavSvsObjectADSBMgr();
    ~CHmsNavSvsObjectADSBMgr();
    virtual void Update(float delta) override;
    virtual void OnDraw() override;
    virtual void UpdateVertexData() override;

    void InitUnitData();
    void SetData(std::vector<NavSvsObjectADSBStu> &vec);

private:
    GLuint m_nVBO;
    GLuint m_nVBOIndex;

    std::vector<V3F_T2F> m_vecUnitVertexData;
    std::vector<GLuint> m_vecUnitIndexData;

    std::vector<CHmsNavSvsLabel*> m_vecLabel;
    std::vector<NavSvsObjectADSBStu> m_vecData;
    bool m_bDataDirty;

    Texture2D *m_pTexture;
};








