#include "HmsNode.h"
#include "HmsAviDisplayUnit.h"
#include "base/HmsScreen.h"
#include "base/HmsCamera.h"
#include "render/HmsRenderer.h"

#define USE_MAT4_STACK 0

NS_HMS_BEGIN

int32_t CHmsNode::s_nGlobalOrderOfArrivalCounter = 1;

bool HMS_DLL HmsNodeComparisonLess(CHmsNode* n1, CHmsNode* n2)
{
	return(n1->GetLocalZOrder() < n2->GetLocalZOrder() ||
		(n1->GetLocalZOrder() == n2->GetLocalZOrder() && n1->GetOrderOfArrival() < n2->GetOrderOfArrival())
		);
}

CHmsNode::CHmsNode()
	: m_pParent(nullptr)
	, m_pUserData(nullptr)
	, m_pUserObject(nullptr)
	, m_bVisible(true)
	, m_bRunning(false)
	, m_bUpdateSchedule(false)
	, m_rotationX(.0f)
	, m_rotationY(.0f)
	, m_rotationZ(.0f)
	, m_scaleX(1.0f)
	, m_scaleY(1.0f)
	, m_scaleZ(1.0f)
	, m_posZ(0)
	, m_ignoreAnchorPointForPosition(false)
	, m_sizeContentDirty(true)
	, m_transformDirty(true)
	, m_inverseDirty(true)
	, m_useAdditionalTransform(false)
	, m_transformUpdated(true)
	, m_localZOrder(0)
	, m_globalZOrder(0)
	, m_orderOfArrival(0)
	, m_maskCamera(1)
	, m_nTag(INVALID_TAG)
	, m_pGlProgramState(nullptr)
	, m_colorDisplay(Color3B::WHITE)
	, m_opacityDisplay(255)
{
	//Mat4							m_transformModeView;		// mode view transform of the node		
	
	m_pDisplayUnit = CHmsAviDisplayUnit::GetInstance();
}


CHmsNode::~CHmsNode()
{
	HMS_SAFE_RELEASE_NULL(m_pUserObject);
	//HMS_SAFE_RELEASE_NULL()

	for (auto & c:m_vChild)
	{
		c->m_pParent = nullptr;
	}
}

void CHmsNode::Draw(Renderer * renderer, const Mat4 & transform, uint32_t flags)
{

}

void CHmsNode::Draw()
{
	auto renderer = m_pDisplayUnit->GetRenderer();
	Draw(renderer, m_transformModeView, true);
}

void CHmsNode::Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags)
{
	if (!m_bVisible)
	{
		return;
	}

	uint32_t flags = ProcessParentFlags(parentTransform, parentFlags);
#if USE_MAT4_STACK
	// IMPORTANT:
	// To ease the migration to v3.0, we still support the Mat4 stack,
	// but it is deprecated and your code should not rely on it
	m_pDisplayUnit->PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	m_pDisplayUnit->LoadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, m_transformModeView);
#endif

	bool visibleByCamera = IsVisitableByVisitingCamera();

	int i = 0;

	if (!m_vChild.empty())
	{
		SortAllChildren();
		// draw children zOrder < 0
		for (; i < m_vChild.size(); i++)
		{
			auto node = m_vChild.at(i);

			if (node && node->m_localZOrder < 0)
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

#if USE_MAT4_STACK
	m_pDisplayUnit->PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
#endif
}

void CHmsNode::Visit()
{
	auto renderer = m_pDisplayUnit->GetRenderer();
	auto & parentTransform = m_pDisplayUnit->GetMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	Visit(renderer, parentTransform, true);

}

ssize_t CHmsNode::GetChildrenCount() const
{
	return m_vChild.size();
}

void CHmsNode::SetParent(CHmsNode * parent)
{
	m_pParent = parent;
	SetTransformNeedToUpdate();
}

void CHmsNode::RemoveFromParent()
{
	RemoveFromParentAndCleanup(true);
}

void CHmsNode::RemoveFromParentAndCleanup(bool bCleanup)
{
	if (m_pParent)
	{
		m_pParent->RemoveChild(this, bCleanup);
	}
}

void CHmsNode::RemoveChild(CHmsNode * child, bool bCleanup /*= true*/)
{
	if (m_vChild.empty())
	{
		return;
	}

	auto index = m_vChild.getIndex(child);
	if (index != HMS_INVALID_INDEX)
	{
		DetachChild(child, index, bCleanup);
	}
}

void CHmsNode::DetachChild(CHmsNode * child, ssize_t childIndex, bool doCleanUp)
{
	if (m_bRunning)
	{
		child->OnExit();
	}

	if (doCleanUp)
	{
		child->Cleanup();
	}

	child->SetParent(nullptr);

	m_vChild.erase(childIndex);
}

void CHmsNode::RemoveChildByTag(int tag, bool bCleanup /*= true*/)
{
	auto child = GetChildByTag(tag);
	if (child)
	{
		RemoveChild(child, bCleanup);
	}

}

void CHmsNode::RemoveChildByName(const std::string & strName, bool bCleanup)
{
	auto child = GetChildByName(strName);
	if (child)
	{
		RemoveChild(child, bCleanup);
	}
}

void CHmsNode::RemoveAllChildren()
{
	RemoveAllChildrenWithCleanup(true);
}

void CHmsNode::RemoveAllChildrenWithCleanup(bool bCleanup)
{
	for (const auto& c : m_vChild)
	{
		// IMPORTANT:
		//  -1st do onExit
		//  -2nd cleanup
		if (m_bRunning)
		{
			//c->onExitTransitionDidStart();
			c->OnExit();
		}

		if (bCleanup)
		{
			c->Cleanup();
		}
		// set parent nil at the end
		c->SetParent(nullptr);
	}

	m_vChild.clear();
}

void CHmsNode::SortAllChildren()
{
	if (m_reorderChildDirty)
	{
		std::sort(std::begin(m_vChild), std::end(m_vChild), HmsNodeComparisonLess);
		m_reorderChildDirty = false;
	}
}

int CHmsNode::GetTag() const
{
	return m_nTag;
}

void CHmsNode::SetTag(int tag)
{
	m_nTag = tag;
}

void CHmsNode::SetUserData(void * userData)
{
	m_pUserData = userData;
}

void CHmsNode::SetUserObject(Ref * userObject)
{
	HMS_SAFE_RETAIN(userObject);
	HMS_SAFE_RELEASE(m_pUserObject);
	m_pUserObject = userObject;
}

GLProgram * CHmsNode::GetGLProgram() const
{
	return m_pGlProgramState ? m_pGlProgramState->getGLProgram() : nullptr;
}

void CHmsNode::SetGLProgram(GLProgram * glProgram)
{
	if (nullptr == m_pGlProgramState || (m_pGlProgramState && m_pGlProgramState->getGLProgram() != glProgram))
	{
		HMS_SAFE_RELEASE(m_pGlProgramState);
		m_pGlProgramState = GLProgramState::getOrCreateWithGLProgram(glProgram);
		m_pGlProgramState->retain();
		m_pGlProgramState->setNodeBinding(this);
	}
}

GLProgramState * CHmsNode::GetGLProgramState() const
{
	return m_pGlProgramState;
}

void CHmsNode::SetGLProgramState(GLProgramState * glProgramState)
{
	if (m_pGlProgramState != glProgramState)
	{
		HMS_SAFE_RETAIN(glProgramState);
		HMS_SAFE_RELEASE(m_pGlProgramState);
		m_pGlProgramState = glProgramState;

		if (m_pGlProgramState)
		{
			m_pGlProgramState->setNodeBinding(this);
		}
	}
}

void CHmsNode::OnEnter()
{
	for (const auto & child : m_vChild)
	{
		child->OnEnter();
	}

	m_bRunning = true;
}

void CHmsNode::OnExit()
{
	for (const auto & child : m_vChild)
	{
		child->OnExit();
	}

	m_bRunning = false;
}

void CHmsNode::Cleanup()
{
	for (auto & c : m_vChild)
	{
		c->Cleanup();
	}

	StopAllActions();
}

const Mat4& CHmsNode::GetNodeToParentTransform() const
{
	if (m_transformDirty)
	{
		// Translate values
		float x = m_pos.x;
		float y = m_pos.y;
		float z = m_posZ;

		if (m_ignoreAnchorPointForPosition)
		{
			x += m_anchorPointInPoints.x;
			y += m_anchorPointInPoints.y;
		}

		//bool needsSkewMatrix = (_skewX || _skewY);


		Vec2 anchorPoint(m_anchorPointInPoints.x * m_scaleX, m_anchorPointInPoints.y * m_scaleY);

		// calculate real position
		if (/*!needsSkewMatrix &&*/ !m_anchorPointInPoints.isZero())
		{
			x += -anchorPoint.x;
			y += -anchorPoint.y;
		}

		// Build Transform Matrix = translation * rotation * scale
		Mat4 translation;
		//move to anchor point first, then rotate
		Mat4::createTranslation(x + anchorPoint.x, y + anchorPoint.y, z, &translation);

		Mat4::createRotation(m_rotationQuat, &m_transform);

		m_transform = translation * m_transform;
		//move by (-anchorPoint.x, -anchorPoint.y, 0) after rotation
		m_transform.translate(-anchorPoint.x, -anchorPoint.y, 0);


		if (m_scaleX != 1.f)
		{
			m_transform.m[0] *= m_scaleX, m_transform.m[1] *= m_scaleX, m_transform.m[2] *= m_scaleX;
		}
		if (m_scaleY != 1.f)
		{
			m_transform.m[4] *= m_scaleY, m_transform.m[5] *= m_scaleY, m_transform.m[6] *= m_scaleY;
		}
		if (m_scaleZ != 1.f)
		{
			m_transform.m[8] *= m_scaleZ, m_transform.m[9] *= m_scaleZ, m_transform.m[10] *= m_scaleZ;
		}

		if (m_useAdditionalTransform)
		{
			m_transform = m_transform * m_additionalTransform;
		}

		m_transformDirty = false;
	}

	return m_transform;
}

HmsAviPf::Mat4 CHmsNode::GetNodeToParentTransform(CHmsNode * ancestor) const
{
	Mat4 t(this->GetNodeToParentTransform());

	for (CHmsNode *p = m_pParent; p != nullptr && p != ancestor; p = p->GetParent())
	{
		t = p->GetNodeToParentTransform() * t;
	}

	return t;
}

HmsAviPf::AffineTransform CHmsNode::GetNodeToParentAffineTransform() const
{
	AffineTransform ret;
	GLToCGAffine(GetNodeToParentTransform().m, &ret);

	return ret;
}

HmsAviPf::AffineTransform CHmsNode::GetNodeToParentAffineTransform(CHmsNode* ancestor) const
{
	AffineTransform t(this->GetNodeToParentAffineTransform());

	for (CHmsNode *p = m_pParent; p != nullptr && p != ancestor; p = p->GetParent())
		t = AffineTransformConcat(t, p->GetNodeToParentAffineTransform());

	return t;
}

void CHmsNode::SetNodeToParentTransform(const Mat4& transform)
{
	m_transform = transform;
	m_transformDirty = false;
	m_transformUpdated = true;
}

const Mat4& CHmsNode::GetParentToNodeTransform() const
{
	if (m_inverseDirty)
	{
		m_inverse = GetNodeToParentTransform().getInversed();
		m_inverseDirty = false;
	}

	return m_inverse;
}

HmsAviPf::AffineTransform CHmsNode::GetParentToNodeAffineTransform() const
{
	AffineTransform ret;

	GLToCGAffine(GetParentToNodeTransform().m, &ret);
	return ret;
}

HmsAviPf::Mat4 CHmsNode::GetNodeToWorldTransform() const
{
	return this->GetNodeToParentTransform(nullptr);
}

HmsAviPf::AffineTransform CHmsNode::GetNodeToWorldAffineTransform() const
{
	return this->GetNodeToParentAffineTransform(nullptr);
}

HmsAviPf::Mat4 CHmsNode::GetWorldToNodeTransform() const
{
	return GetNodeToWorldTransform().getInversed();
}

HmsAviPf::AffineTransform CHmsNode::GetWorldToNodeAffineTransform() const
{
	return AffineTransformInvert(this->GetNodeToWorldAffineTransform());
}

HmsAviPf::Vec2 CHmsNode::ConvertToNodeSpace(const Vec2& worldPoint) const
{
	Mat4 tmp = GetWorldToNodeTransform();
	Vec3 vec3(worldPoint.x, worldPoint.y, 0);
	Vec3 ret;
	tmp.transformPoint(vec3, &ret);
	return Vec2(ret.x, ret.y);
}

HmsAviPf::Vec2 CHmsNode::ConvertToWorldSpace(const Vec2& nodePoint) const
{
	Mat4 tmp = GetNodeToWorldTransform();
	Vec3 vec3(nodePoint.x, nodePoint.y, 0);
	Vec3 ret;
	tmp.transformPoint(vec3, &ret);
	return Vec2(ret.x, ret.y);
}

HmsAviPf::Vec2 CHmsNode::ConvertToNodeSpaceAR(const Vec2& worldPoint) const
{
	Vec2 nodePoint(ConvertToNodeSpace(worldPoint));
	return nodePoint - m_anchorPointInPoints;
}

HmsAviPf::Vec2 CHmsNode::ConvertToWorldSpaceAR(const Vec2& nodePoint) const
{
	return ConvertToWorldSpace(nodePoint + m_anchorPointInPoints);
}

void CHmsNode::SetAdditionalTransform(Mat4* additionalTransform)
{
	if (additionalTransform == nullptr)
	{
		m_useAdditionalTransform = false;
	}
	else
	{
		m_additionalTransform = *additionalTransform;
		m_useAdditionalTransform = true;
	}
	m_transformUpdated = m_transformDirty = m_inverseDirty = true;
}

void CHmsNode::SetAdditionalTransform(const AffineTransform& additionalTransform)
{
	Mat4 tmp;
	CGAffineToGL(additionalTransform, tmp.m);
	SetAdditionalTransform(&tmp);
}

HmsAviPf::Mat4 CHmsNode::GetTransform(const Mat4 &parentTransform)
{
	return parentTransform * this->GetNodeToParentTransform();
}

uint32_t CHmsNode::ProcessParentFlags(const Mat4& parentTransform, uint32_t parentFlags)
{
	uint32_t flags = parentFlags;
	flags |= (m_transformUpdated ? FLAGS_TRANSFORM_DIRTY : 0);
	flags |= (m_sizeContentDirty ? FLAGS_CONTENT_SIZE_DIRTY : 0);


	if (flags & FLAGS_DIRTY_MASK)
		m_transformModeView = this->GetTransform(parentTransform);

	m_transformUpdated = false;
	m_sizeContentDirty = false;

	return flags;
}

void CHmsNode::SetLocalZOrder(int localZOrder)
{
	if (m_localZOrder == localZOrder)
	{
		return;
	}

	m_localZOrder = localZOrder;
	if (m_pParent)
	{
		m_pParent->ReorderChild(this, localZOrder);
	}
}

void CHmsNode::SetOrderOfArrival(int orderOfArrival)
{
	m_orderOfArrival = orderOfArrival;
}

int CHmsNode::GetOrderOfArrival() const
{
	return m_orderOfArrival;
}

CHmsScreen * CHmsNode::GetScreen() const
{
	if (!m_pParent)
	{
		return nullptr;
	}

	auto screen = m_pParent;
	while (screen->m_pParent)
	{
		screen = screen->m_pParent;
	}

	return dynamic_cast<CHmsScreen*>(screen);
}

bool CHmsNode::IsVisitableByVisitingCamera() const
{
	auto camera = Camera::GetVisitingCamera();
	bool visibleByCamera = camera ? ((uint32_t)camera->getCameraFlag() & m_maskCamera) != 0 : true;
	return visibleByCamera;
}

CHmsNode * CHmsNode::GetChildByTag(int nTag) const
{
	for (const auto & c : m_vChild)
	{
		if (c && c->m_nTag == nTag)
		{
			return c;
		}
	}
	return nullptr;
}

CHmsNode * CHmsNode::GetChildByName(const std::string & strName) const
{
	for (const auto & c : m_vChild)
	{
		if (c && c->m_strName == strName)
		{
			return c;
		}
	}
	return nullptr;
}

void CHmsNode::SetAnchorPoint(const Vec2 & anchorPoint)
{
	if (!anchorPoint.equals(m_anchorPoint))
	{
		m_anchorPoint = anchorPoint;
		m_anchorPointInPoints.set(m_sizeContent.width*m_anchorPoint.x, m_sizeContent.height*m_anchorPoint.y);
		SetTransformNeedToUpdate();
		m_sizeContentDirty = true;
	}
}

void CHmsNode::SetAnchorPoint(float xOffset, float yOffset)
{
	SetAnchorPoint(Vec2(xOffset, yOffset));
}

const Vec2 & CHmsNode::GetAnchorPoint() const
{
	return m_anchorPoint;
}

void CHmsNode::SetContentSize(const Size & size)
{
	if (!m_sizeContent.equals(size))
	{
		m_sizeContent = size;
		m_anchorPointInPoints.set(m_sizeContent.width*m_anchorPoint.x, m_sizeContent.height*m_anchorPoint.y);
		SetTransformNeedToUpdate();
		m_sizeContentDirty = true;
	}
}

Size CHmsNode::GetContentSize() const
{
	return m_sizeContent;
}

void CHmsNode::InsertChild(CHmsNode * child, int zOrder)
{
	m_transformUpdated = true;
	m_reorderChildDirty = true;
	m_vChild.pushBack(child);
	child->m_localZOrder = zOrder;
}

void CHmsNode::ReorderChild(CHmsNode * child, int zOrder)
{
	m_reorderChildDirty = true;
	child->SetOrderOfArrival(s_nGlobalOrderOfArrivalCounter++);
	child->m_localZOrder = zOrder;
}

void CHmsNode::AddChild(CHmsNode * child)
{
	if (child)
	{
		AddChild(child, child->m_localZOrder, child->m_nTag);
	}
}

void CHmsNode::AddChild(CHmsNode * child, int localZOrder)
{
	if (child)
	{
		AddChild(child, localZOrder, child->m_nTag);
	}
}

void CHmsNode::AddChild(CHmsNode * child, int localZOrder, int tag)
{
	if (nullptr == child)
	{
		return;
	}

	if (m_vChild.empty())
	{
		m_vChild.reserve(2);
	}

	InsertChild(child, localZOrder);

	child->SetTag(tag);
	child->SetParent(this);
	child->SetOrderOfArrival(s_nGlobalOrderOfArrivalCounter++);

	if (m_bRunning)
	{
		child->OnEnter();
	}
}

bool CHmsNode::Init()
{
	return true;
}


void CHmsNode::SetRotation3D(const Vec3 & rot)
{
	if (m_rotationX == rot.x && m_rotationY == rot.y && m_rotationZ == rot.z)
	{
		return;
	}

	m_transformUpdated = m_transformDirty = m_inverseDirty = true;

	m_rotationX = rot.x;
	m_rotationY = rot.y;
	m_rotationZ = rot.z;

	UpdateRotationQuat();
}

HmsAviPf::Vec3 CHmsNode::GetRotation3D() const
{
	return Vec3(m_rotationX, m_rotationY, m_rotationZ);
}

void CHmsNode::UpdateRotationQuat()
{
	float 
		halfRadx = MATH_DEG_TO_RAD(m_rotationX / 2.f),
		halfRady = MATH_DEG_TO_RAD(m_rotationY / 2.f),
		halfRadz = -MATH_DEG_TO_RAD(m_rotationZ / 2.f);
	float 
		coshalfRadx = cosf(halfRadx), 
		sinhalfRadx = sinf(halfRadx), 
		coshalfRady = cosf(halfRady), 
		sinhalfRady = sinf(halfRady), 
		coshalfRadz = cosf(halfRadz), 
		sinhalfRadz = sinf(halfRadz);

	m_rotationQuat.x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
	m_rotationQuat.y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
	m_rotationQuat.z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
	m_rotationQuat.w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;
}

void CHmsNode::UpdateRotation3D()
{
	//convert quaternion to Euler angle
	float 
		x = m_rotationQuat.x, 
		y = m_rotationQuat.y, 
		z = m_rotationQuat.z, 
		w = m_rotationQuat.w;
	
	m_rotationX = atan2f(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
	float sy = 2.f * (w * y - z * x);
	sy = clampf(sy, -1, 1);
	m_rotationY = asinf(sy);
	m_rotationZ = atan2f(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));

	m_rotationX = MATH_RAD_TO_DEG(m_rotationX);
	m_rotationY = MATH_RAD_TO_DEG(m_rotationY);
	m_rotationZ = -MATH_RAD_TO_DEG(m_rotationZ);
}

void CHmsNode::SetRotation(float rotation)
{
	if (m_rotationZ == rotation)
	{
		return;
	}

	m_rotationZ = rotation;
	m_transformUpdated = m_transformDirty = m_inverseDirty = true;

	UpdateRotationQuat();
}

float CHmsNode::GetRotation() const
{
	return m_rotationZ;
}

void CHmsNode::SetRotationQuat(const Quaternion& quat)
{
	m_rotationQuat = quat;
	UpdateRotation3D();
	m_transformUpdated = m_transformDirty = m_inverseDirty = true;
}

Quaternion CHmsNode::GetRotationQuat() const
{
	return m_rotationQuat;
}

void CHmsNode::SetPosition(float x, float y)
{
	if (m_pos.x == x && m_pos.y == y)
	{
		return;
	}

	m_pos.x = x;
	m_pos.y = y;

	SetTransformNeedToUpdate();
}

void CHmsNode::SetPosition(const Vec2 & pos)
{
	SetPosition(pos.x, pos.y);
}

void CHmsNode::SetPositionX(float x)
{
	if (m_pos.x == x)
	{
		return;
	}
	m_pos.x = x;
	SetTransformNeedToUpdate();
}

void CHmsNode::SetPositionY(float y)
{
	if (m_pos.y == y)
	{
		return;
	}
	m_pos.y = y;
	SetTransformNeedToUpdate();
}

void CHmsNode::SetPositionZ(float z)
{
	if (m_posZ == z)
	{
		return;
	}

	m_posZ = z;
	SetTransformNeedToUpdate();
}

void CHmsNode::SetPosition3D(const Vec3 & pos)
{
	if (m_pos.x == pos.x && m_pos.y == pos.y && m_posZ == pos.z)
	{
		return;
	}

	m_pos.x = pos.x;
	m_pos.y = pos.y;
	m_posZ = pos.z;

	SetTransformNeedToUpdate();
}

Vec2 CHmsNode::GetPoistion() const
{
	return m_pos;
}

float CHmsNode::GetPoistionX() const
{
	return m_pos.x;
}

float CHmsNode::GetPoistionY() const
{
	return m_pos.y;
}

float CHmsNode::GetPoistionZ() const
{
	return m_posZ;
}

Vec3 CHmsNode::GetPoistion3D() const
{
	return Vec3(m_pos.x, m_pos.y, m_posZ);
}

void CHmsNode::SetGlobalZOrder(float globalZOrder)
{
	if (m_globalZOrder != globalZOrder)
	{
		m_globalZOrder = globalZOrder;
	}
}

void CHmsNode::SetScaleX(float scaleX)
{
	if (m_scaleX == scaleX)
	{
		return;
	}

	m_scaleX = scaleX;
	SetTransformNeedToUpdate();
}

void CHmsNode::SetScaleY(float scaleY)
{
	if (m_scaleY == scaleY)
	{
		return;
	}

	m_scaleY = scaleY;
	SetTransformNeedToUpdate();
}

void CHmsNode::SetScaleZ(float scaleZ)
{
	if (m_scaleZ == scaleZ)
	{
		return;
	}

	m_scaleZ = scaleZ;
	SetTransformNeedToUpdate();
}

void CHmsNode::SetScale(float scale)
{
	if (m_scaleX == scale && m_scaleY == scale && m_scaleZ == scale)
	{
		return;
	}

	m_scaleX = scale;
	m_scaleY = scale;
	m_scaleZ = scale;
	SetTransformNeedToUpdate();
}

void CHmsNode::SetScale(float scaleX, float scaleY)
{
	if (m_scaleX == scaleX && m_scaleY == scaleY)
	{
		return;
	}

	m_scaleX = scaleX;
	m_scaleY = scaleY;
	SetTransformNeedToUpdate();
}

float CHmsNode::GetScaleX() const
{
	return m_scaleX;
}

float CHmsNode::GetScaleY() const
{
	return m_scaleY;
}

float CHmsNode::GetScaleZ() const
{
	return m_scaleZ;
}

float CHmsNode::GetScale() const
{
	return m_scaleX;
}

void CHmsNode::Update(float delta)
{
	for (auto & c : m_vChild)
	{
		c->Update(delta);
	}
}

void CHmsNode::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

void CHmsNode::IgnoreAnchorPointForPosition(bool ignore)
{
	if (ignore != m_ignoreAnchorPointForPosition)
	{
		m_ignoreAnchorPointForPosition = ignore;
		SetTransformNeedToUpdate();
	}
}

bool CHmsNode::IsIgnoreAnchorPointForPosition() const
{
	return m_ignoreAnchorPointForPosition;
}

const HmsAviPf::Color3B & CHmsNode::GetColor() const
{
	return m_colorDisplay;
}

void CHmsNode::SetColor(const Color3B& color)
{
	m_colorDisplay = color;
	UpdateColor();
}

void CHmsNode::SetColorWithChild(const Color3B& color)
{
	m_colorDisplay = color;

	for (auto & c:m_vChild)
	{
		c->SetColorWithChild(color);
	}
	UpdateColor();
}

GLubyte CHmsNode::GetOpacity() const
{
	return m_opacityDisplay;
}

void CHmsNode::SetOpacity(GLubyte opacity)
{
	m_opacityDisplay = opacity;
	UpdateColor();
}

void CHmsNode::SetOpacityWithChild(GLubyte opacity)
{
	m_opacityDisplay = opacity;
	UpdateColor();
	
	for (auto & c:m_vChild)
	{
		c->SetOpacityWithChild(opacity);
	}
}

void CHmsNode::SetScheduleUpdate()
{
	if (!m_bUpdateSchedule)
	{
		m_bUpdateSchedule = true;
		CHmsAviDisplayUnit::GetInstance()->GetScheduler()->AddSchedule(this);
	}
}

void CHmsNode::SetUnSchedule()
{
	if (m_bUpdateSchedule)
	{
		CHmsAviDisplayUnit::GetInstance()->GetScheduler()->RemoveSchedule(this);
		m_bUpdateSchedule = false;
	}
}

void CHmsNode::SetScheduleOnce(const HmsSchedulerFunc & schedule)
{
	CHmsAviDisplayUnit::GetInstance()->GetScheduler()->AddScheduleOnce(schedule);
}

Vec2 CHmsNode::ConvertTouchToNodeSpace(Touch * touch) const
{
	Vec2 point = touch->GetLocation();
	return this->ConvertToNodeSpace(point);
}

Vec2 CHmsNode::ConvertTouchToNodeSpaceAR(Touch * touch) const
{
	Vec2 point = touch->GetLocation();
	return this->ConvertToNodeSpaceAR(point);
}

Rect CHmsNode::GetRectFromParent()
{
	Vec2 anchorPoint(m_anchorPointInPoints.x * m_scaleX, m_anchorPointInPoints.y * m_scaleY);
	auto pos = m_pos - anchorPoint;
	Rect out(pos.x, pos.y, m_sizeContent.width*m_scaleX, m_sizeContent.height*m_scaleY);
	return out;
}

bool CHmsNode::ContainParentPosition(const Vec2 & pos)
{
	//auto rect = GetRectFromParent();
	auto rect = GetRectByAnchorPoint();
	return rect.containsPoint(pos);
}

Rect CHmsNode::GetRectByAnchorPoint()
{
	//Vec2 pos = GetPoistion();
	Vec2 anchorPoint(m_anchorPointInPoints.x*m_scaleX, m_anchorPointInPoints.y*m_scaleY);
	float scaleWidth = m_sizeContent.width*m_scaleX;
	float scaleHeight = m_sizeContent.height*m_scaleY;
	float leftBottomX = -anchorPoint.x;
	float leftBottomY = -anchorPoint.y;
	Rect out(/*pos.x + */leftBottomX, /*pos.y + */leftBottomY, scaleWidth, scaleHeight);
	return out;
}

Rect CHmsNode::GetRectByOrigin()
{
	float scaleWidth = m_sizeContent.width*m_scaleX;
	float scaleHeight = m_sizeContent.height*m_scaleY;
	if (scaleHeight < 0)
	{
		return Rect(0, scaleHeight, scaleWidth, -scaleHeight);
	}
	return Rect(0, 0, scaleWidth, scaleHeight);
}

HmsAviPf::Vec2 CHmsNode::ConvertOriginPointToAR(const Vec2 & posOrigin)
{
	return posOrigin + m_anchorPointInPoints;
}

HmsAviPf::Vec2 CHmsNode::ConvertARPointToOrigin(const Vec2 & posAR)
{
	return posAR - m_anchorPointInPoints;
}

Rect CHmsNode::ConvertOriginRectToAR(const Rect & rectOrigin)
{
	Vec2 anchorPoint(m_anchorPointInPoints.x*m_scaleX, m_anchorPointInPoints.y*m_scaleY);
	Rect out(rectOrigin.origin + anchorPoint, rectOrigin.size);
	return out;
}

Rect CHmsNode::ConvertARRectToOrigin(const Rect & rectAR)
{
	Vec2 anchorPoint(m_anchorPointInPoints.x*m_scaleX, m_anchorPointInPoints.y*m_scaleY);
	Rect out(rectAR.origin - anchorPoint, rectAR.size);
	return out;
}

void CHmsNode::RunAction(CHmsAction * action)
{
	CHmsAviDisplayUnit::GetInstance()->GetActionMgr()->AddAction(action, this);
}

void CHmsNode::StopActioin(CHmsAction * action)
{
	CHmsAviDisplayUnit::GetInstance()->GetActionMgr()->RemoveAction(action);
}

void CHmsNode::StopActionByTag(int nTag)
{
	CHmsAviDisplayUnit::GetInstance()->GetActionMgr()->RemoveActionByTag(nTag, this);
}

void CHmsNode::StopAllActions()
{
	CHmsAviDisplayUnit::GetInstance()->GetActionMgr()->RemoveNodeActions(this);
}

void CHmsNode::ForceUpdateAllChild()
{
	SetTransformNeedToUpdate();
	for (auto &c : m_vChild)
	{
		c->ForceUpdateAllChild();
	}
}


NS_HMS_END