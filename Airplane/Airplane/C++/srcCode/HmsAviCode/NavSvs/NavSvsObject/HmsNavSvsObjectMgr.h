#pragma once

#include <vector>
#include "HmsGlobal.h"
#include "math/HmsMath.h"
#include "render/HmsGLProgram.h"
#include "NavSvs/HmsNavSvsMathHelper.h"
#include "HmsStandardDef.h"
#include "base/HmsNode.h"

#include "HmsNavSvsObjectPlane.h"
#include "HmsNavSvsObjectRunway.h"
#include "HmsNavSvsObjectFlyPipe.h"
#include "HmsNavSvsObjectRouteTrack.h"
#include "HmsNavSvsObjectADSBMgr.h"

class CHmsNavSvsMgr;
class CHmsNavSvsObjectMgr : public CHmsNode
{
public:
    static CHmsNavSvsObjectMgr* GetInstance();
    virtual void Update(float delta) override;
    void OnDraw();

    void SetNavSvsObjectEnable(NavSvsObjectType type, bool bEnable);
    CHmsNavSvsObjectBase* GetNavSvsObject(NavSvsObjectType type);

private:
    CHmsNavSvsObjectMgr();
    ~CHmsNavSvsObjectMgr();

public:
    GLProgram *m_pGlProgram;
    GLuint m_locUniformColor;
    GLuint m_locUniformMVP;
    GLuint m_locUniformRelativeOrigin;
    std::vector<CHmsNavSvsObjectBase*> m_vecObject;

private:
    CHmsNavSvsMgr *m_pSvsMgr;
};