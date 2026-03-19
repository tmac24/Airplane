
#include "HmsScissorNode.h"
#include "render/HmsGLProgram.h"
#include "render/HmsGLProgramState.h"
#include "render/HmsGLProgramCache.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsRenderer.h"
#include "render/HmsRenderState.h"
#include "HmsAviDisplayUnit.h"


NS_HMS_BEGIN


CHmsScissorNode::CHmsScissorNode()
: HmsUiEventInterface(this)
, m_bClipDirty(false)
{
	
}

CHmsScissorNode::~CHmsScissorNode()
{

}

CHmsScissorNode* CHmsScissorNode::create()
{
    CHmsScissorNode *ret = new STD_NOTHROW CHmsScissorNode();
    if (ret && ret->Init())
    {
        ret->autorelease();
    }
    else
    {
        HMS_SAFE_DELETE(ret);
    }
    
    return ret;
}

bool CHmsScissorNode::Init()
{
    return CHmsNode::Init();
}

void CHmsScissorNode::Visit(Renderer *renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    if (!m_bVisible)
        return;
    
    uint32_t flags = ProcessParentFlags(parentTransform, parentFlags);
	if (flags & FLAGS_DIRTY_MASK)
	{
		m_bClipDirty = true;
	}

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
	auto director = HmsAviPf::CHmsAviDisplayUnit::GetInstance();
    HMSASSERT(nullptr != director, "Director is null when setting matrix stack");
    director->PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->LoadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW,  m_transformModeView);

    //Add group command
        
	_groupCommand.init(m_globalZOrder);
    renderer->addCommand(&_groupCommand);

    renderer->pushGroup(_groupCommand.getRenderQueueID());

	_beforeVisitCmd.init(m_globalZOrder);
    _beforeVisitCmd.func = HMS_CALLBACK_0(CHmsScissorNode::onBeforeVisit, this);
    renderer->addCommand(&_beforeVisitCmd);

    int i = 0;
    bool visibleByCamera = IsVisitableByVisitingCamera();
    
    if(!m_vChild.empty())
    {
        SortAllChildren();
        // draw children zOrder < 0
		for (; i < m_vChild.size(); i++)
        {
			auto node = m_vChild.at(i);
            
            if ( node && node->GetLocalZOrder() < 0 )
				node->Visit(renderer, m_transformModeView, flags);
            else
                break;
        }
        // self draw
        if (visibleByCamera)
			this->Draw(renderer, m_transformModeView, flags);
        
		for (auto it = m_vChild.cbegin() + i; it != m_vChild.cend(); ++it)
			(*it)->Visit(renderer, m_transformModeView, flags);
    }
    else if (visibleByCamera)
    {
		this->Draw(renderer, m_transformModeView, flags);
    }

	_afterVisitCmd.init(m_globalZOrder);
    _afterVisitCmd.func = HMS_CALLBACK_0(CHmsScissorNode::onAfterVisit, this);
    renderer->addCommand(&_afterVisitCmd);

    renderer->popGroup();
    
    director->PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void CHmsScissorNode::onBeforeVisit()
{
	if (m_bClipDirty)
	{
		m_bClipDirty = false;

		Mat4 tmp = GetNodeToWorldTransform();
		Vec3 scale;
		tmp.getScale(&scale);
		Vec3 trans;
		tmp.getTranslation(&trans);
		tmp = Mat4::IDENTITY;
        tmp.translate(trans);
		tmp.scale(scale);	

		auto pos = m_clipRect.origin;
		Vec3 vec3(pos.x, pos.y, 0);
		Vec3 ret;
		tmp.transformPoint(vec3, &ret);
		pos = Vec2(ret.x, ret.y);

		auto size = m_clipRect.size;
        size.width *= scale.x;
        size.height *= scale.y;
		if (size.width < 0)
		{
			size.width = -size.width;
			pos.x -= size.width;
		}
		if (size.height < 0)
		{
			size.height = -size.height;
			pos.y -= size.height;
		}
		m_clipWorldRect = Rect(pos, size);
	}
	glEnable(GL_SCISSOR_TEST);

	auto x = m_clipWorldRect.origin.x;
	auto y = m_clipWorldRect.origin.y;
	auto width = m_clipWorldRect.size.width;
	auto height = m_clipWorldRect.size.height;
	glScissor(x, y, width, height);
}

void CHmsScissorNode::onAfterVisit()
{
	glDisable(GL_SCISSOR_TEST);
}

void CHmsScissorNode::SetClipRect(Rect rect)
{
    this->SetContentSize(rect.size);
	m_clipRect = rect;
	m_bClipDirty = true;
}

NS_HMS_END