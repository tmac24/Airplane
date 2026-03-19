#pragma once

#include "NavSvs/HmsNavSvsCommDef.h"

class CHmsNavSvsMgr;
class CHmsNavSvsObjectMgr;

class CHmsNavSvsObjectBase
{
public:
    CHmsNavSvsObjectBase();
    virtual ~CHmsNavSvsObjectBase();
    virtual void Update(float delta) = 0;
    virtual void OnDraw() = 0;
    virtual void UpdateVertexData() = 0;

    virtual NavSvsObjectType GetNavSvsObjectType(){ return m_objectType; }
    virtual void SetEnable(bool b){ m_bEnable = b; }
    virtual bool IsEnable(){ return m_bEnable; }

protected:
    static CHmsNavSvsMgr *s_pSvsMgr;
    static CHmsNavSvsObjectMgr *s_pSvsObjectMgr;

    NavSvsObjectType m_objectType;
    bool m_bEnable;

    friend class CHmsNavSvsObjectMgr;
};


