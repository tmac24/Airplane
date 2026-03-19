#include "HmsFrame2DLayerBase.h"
#include "Nav/HmsFrame2DRoot.h"

CHmsFrame2DLayerBase::CHmsFrame2DLayerBase(CHmsNode *pNode, NavFrame2DLayerType type)
    : HmsUiEventInterface(pNode)
    , m_bNeedUpdate2DEles(false)
    , m_bEleChanged(false)
    , m_bNeedSync(false)
    , m_earthLayer(0)
    , m_bNeedUseSyncData(false)
    , m_layerType(type)
    , m_pFrame2DRoot(nullptr)
{

}

CHmsFrame2DLayerBase::~CHmsFrame2DLayerBase()
{
    if (m_pFrame2DRoot)
    {
        m_pFrame2DRoot->autorelease();
    }
}

void CHmsFrame2DLayerBase::SetFrame2DRoot(CHmsFrame2DRoot *p)
{
    m_pFrame2DRoot = p;
    m_pFrame2DRoot->retain();
}

