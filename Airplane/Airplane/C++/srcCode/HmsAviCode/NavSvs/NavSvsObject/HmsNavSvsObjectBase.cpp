#include "HmsNavSvsObjectBase.h"


CHmsNavSvsMgr* CHmsNavSvsObjectBase::s_pSvsMgr = nullptr;
CHmsNavSvsObjectMgr* CHmsNavSvsObjectBase::s_pSvsObjectMgr = nullptr;

CHmsNavSvsObjectBase::CHmsNavSvsObjectBase()
{
    m_objectType = NavSvsObjectType::OBJECTTYPE_NONE;
    m_bEnable = true;
}

CHmsNavSvsObjectBase::~CHmsNavSvsObjectBase()
{

}