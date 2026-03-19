#include "HmsAviDisplayUnit.h"
#include "render/HmsRenderer.h"
#include "render/HmsFrameBuffer.h"
#include "render/HmsRenderState.h"
#include "render/HmsTextureCache.h"
#include "base/HmsScreen.h"
#include "base/AutoreleasePool.h"

NS_HMS_BEGIN

static CHmsAviDisplayUnit * s_SharedHmsAviView = nullptr;

CHmsAviDisplayUnit::CHmsAviDisplayUnit()
	: m_pRenderer(nullptr)
	, m_pScreen(nullptr)
	, m_pNextScreen(nullptr)
	, m_pTextureCache(nullptr)
	, m_pScheduler(nullptr)
	, m_pEventDispatcher(nullptr)
	, m_projection(Projection::_2D)
    , m_esContext(nullptr)
{
	m_pTextureCache = new TextureCache();
	m_pScheduler = new CHmsScheduler();
	m_pEventDispatcher = new CHmsEventDispatcher();
	m_pActionMgr = new CHmsActionMgr();

	m_pFrameBuffer = FrameBuffer::getOrCreateDefaultFBO(nullptr);
	m_pFrameBuffer->retain();
}


CHmsAviDisplayUnit::~CHmsAviDisplayUnit()
{
	if (m_pTextureCache)
	{
		m_pTextureCache->waitForQuit();
		HMS_SAFE_RELEASE_NULL(m_pTextureCache);
	}
}

CHmsAviDisplayUnit * CHmsAviDisplayUnit::GetInstance()
{
	if (!s_SharedHmsAviView)
	{
		s_SharedHmsAviView = new CHmsAviDisplayUnit();
		HMSASSERT(s_SharedDirector, "FATAL: Not enough memory");
		s_SharedHmsAviView->Init();
	}

	return s_SharedHmsAviView;
}

bool CHmsAviDisplayUnit::Init()
{
	InitMatrixStack();
	m_pRenderer = new  Renderer;
	RenderState::initialize();
	return true;
}

void CHmsAviDisplayUnit::SetProjection(Projection projection)
{
	Size size = m_sizeViewInPoints;
	switch (projection)
	{
	case Projection::_2D:
	{
		LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

		Mat4 orthoMatrix;
		Mat4::createOrthographicOffCenter(0, size.width, 0, size.height, -1024, 1024, &orthoMatrix);
		MultiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, orthoMatrix);
		LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		break;
	}

	case Projection::_3D:
	{
		float zeye = this->GetZEye();

		Mat4 matrixPerspective, matrixLookup;

		LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

		// issue #1334
		Mat4::createPerspective(60, (GLfloat)size.width / size.height, 10, zeye + size.height / 2, &matrixPerspective);

		MultiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, matrixPerspective);

		Vec3 eye(size.width / 2, size.height / 2, zeye), center(size.width / 2, size.height / 2, 0.0f), up(0.0f, 1.0f, 0.0f);
		Mat4::createLookAt(eye, center, up, &matrixLookup);
		MultiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, matrixLookup);

		LoadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
		break;
	}

	case Projection::CUSTOM:
		// Projection Delegate is no longer needed
		// since the event "PROJECTION CHANGED" is emitted
		break;

	default:
		CCLOG("HmsAviPf: Director: unrecognized projection");
		break;
	}

	m_projection = projection;
}

void CHmsAviDisplayUnit::PushMatrix(MATRIX_STACK_TYPE type)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		m_matrixStackModeView.push(m_matrixStackModeView.top());
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		m_matrixStackProjection.push(m_matrixStackProjection.top());
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
	{
		m_matrixStackTexture.push(m_matrixStackTexture.top());
	}
	else
	{
		HMSASSERT(false, "unknow matrix stack type");
	}
}

void CHmsAviDisplayUnit::PopMatrix(MATRIX_STACK_TYPE type)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_matrixStackModeView.pop();
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_matrixStackProjection.pop();
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_matrixStackTexture.pop();
	}
	else
	{
		HMSASSERT(false, "unknow matrix stack type");
	}
}

void CHmsAviDisplayUnit::LoadIdentityMatrix(MATRIX_STACK_TYPE type)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_matrixStackModeView.top() = Mat4::IDENTITY;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_matrixStackProjection.top() = Mat4::IDENTITY;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_matrixStackTexture.top() = Mat4::IDENTITY;
	}
	else
	{
		HMSASSERT(false, "unknow matrix stack type");
	}
}

void CHmsAviDisplayUnit::LoadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_matrixStackModeView.top() = mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_matrixStackProjection.top() = mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_matrixStackTexture.top() = mat;
	}
	else
	{
		HMSASSERT(false, "unknow matrix stack type");
	}
}

void CHmsAviDisplayUnit::MultiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
	if (MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW == type)
	{
		m_matrixStackModeView.top() *= mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION == type)
	{
		m_matrixStackProjection.top() *= mat;
	}
	else if (MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE == type)
	{
		m_matrixStackTexture.top() *= mat;
	}
	else
	{
		HMSASSERT(false, "unknow matrix stack type");
	}
}

const Mat4& CHmsAviDisplayUnit::GetMatrix(MATRIX_STACK_TYPE type)
{
	if (type == MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW)
	{
		return m_matrixStackModeView.top();
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION)
	{
		return m_matrixStackProjection.top();
	}
	else if (type == MATRIX_STACK_TYPE::MATRIX_STACK_TEXTURE)
	{
		return m_matrixStackTexture.top();
	}

	HMSASSERT(false, "unknow matrix stack type, will return modelview matrix instead");
	return  m_matrixStackModeView.top();
}

void CHmsAviDisplayUnit::ResetMatrixStack()
{
	InitMatrixStack();
}

void CHmsAviDisplayUnit::InitMatrixStack()
{
	while (!m_matrixStackModeView.empty())
	{
		m_matrixStackModeView.pop();
	}

	while (!m_matrixStackProjection.empty())
	{
		m_matrixStackProjection.pop();
	}

	while (!m_matrixStackTexture.empty())
	{
		m_matrixStackTexture.pop();
	}

	m_matrixStackModeView.push(Mat4::IDENTITY);
	m_matrixStackProjection.push(Mat4::IDENTITY);
	m_matrixStackTexture.push(Mat4::IDENTITY);
}

float CHmsAviDisplayUnit::GetZEye() const
{
	return (m_sizeViewInPoints.height / 1.1566f);
}

void CHmsAviDisplayUnit::SetViewSize(const HmsAviPf::Size & size)
{
	m_sizeViewInPoints = size;
}

const Size & CHmsAviDisplayUnit::GetViewSize() const
{
	return m_sizeViewInPoints;
}

void CHmsAviDisplayUnit::DrawScreen(float fDelta)
{
	if (m_pEventDispatcher)
		m_pEventDispatcher->UpdateEventListener();

	if (m_funcUpdateData)
		m_funcUpdateData(fDelta);

	if (m_pActionMgr)
		m_pActionMgr->Update(fDelta);

	if (m_pScheduler)
		m_pScheduler->Update(fDelta);

	m_pRenderer->clear();
	FrameBuffer::clearAllFBOs();

	PushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	if (m_pScreen)
	{
		m_pRenderer->clearDrawStats();
		m_pScreen->Render(m_pRenderer);
	}

	m_pRenderer->render();
	PopMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);

	PoolManager::getInstance()->getCurrentPool()->clear();
	if (m_pNextScreen)
	{
		RunScreen(m_pNextScreen);
		m_pNextScreen->autorelease();
		m_pNextScreen = nullptr;
	}
}

void CHmsAviDisplayUnit::RunScreen(CHmsScreen * pScreen)
{
	if (m_pScreen)
	{
		if (m_pScreen)
		{
			m_pScreen->OnExit();
		}
		m_pScreen->autorelease();
		m_pScreen = pScreen;
		if (m_pScreen)
		{
			m_pScreen->OnEnter();
		}
	}
	else
	{
		m_pScreen = pScreen;
        if (m_pScreen)
        {
            m_pScreen->OnEnter();
        }
	}
	m_pScreen->retain();
}

void HmsAviPf::CHmsAviDisplayUnit::SetNextScreen(CHmsScreen * pScreen)
{
	if (m_pNextScreen)
	{
		m_pNextScreen->autorelease();
		m_pNextScreen = nullptr;
	}
	m_pNextScreen = pScreen;
	if (m_pNextScreen)
	{
		m_pNextScreen->retain();
	}
}

void CHmsAviDisplayUnit::InitDisplayUnit()
{
	this->SetProjection(m_projection);
	m_pRenderer->initGLView();

	if (m_pScreen)
	{
		m_pScreen->OnEnter();
	}
}

static void GLToClipTransform(Mat4 *transformOut)
{
	if (nullptr == transformOut) return;

	CHmsAviDisplayUnit * du = CHmsAviDisplayUnit::GetInstance();
	HMSASSERT(nullptr != du, "Director is null when setting matrix stack");

	auto projection = du->GetMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	auto modelview = du->GetMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	*transformOut = projection * modelview;
}

HmsAviPf::Vec2 CHmsAviDisplayUnit::ConvertToGL(const Vec2& uiPoint)
{
	Mat4 transform;
	GLToClipTransform(&transform);

	Mat4 transformInv = transform.getInversed();

	// Calculate z=0 using -> transform*[0, 0, 0, 1]/w
	float zClip = transform.m[14] / transform.m[15];

	Size glSize = m_sizeViewInPoints;
	Vec4 clipCoord(2.0f*uiPoint.x / glSize.width - 1.0f, 1.0f - 2.0f*uiPoint.y / glSize.height, zClip, 1);

	Vec4 glCoord;
	//transformInv.transformPoint(clipCoord, &glCoord);
	transformInv.transformVector(clipCoord, &glCoord);
	float factor = 1.0 / glCoord.w;
	return Vec2(glCoord.x * factor, glCoord.y * factor);
}

HmsAviPf::Vec2 CHmsAviDisplayUnit::ConvertToUI(const Vec2& glPoint)
{
	Mat4 transform;
	GLToClipTransform(&transform);

	Vec4 clipCoord;
	// Need to calculate the zero depth from the transform.
	Vec4 glCoord(glPoint.x, glPoint.y, 0.0, 1);
	transform.transformVector(glCoord, &clipCoord);

	/*
	BUG-FIX #5506

	a = (Vx, Vy, Vz, 1)
	b = (a×M)T
	Out = 1 ? bw(bx, by, bz)
	*/

	clipCoord.x = clipCoord.x / clipCoord.w;
	clipCoord.y = clipCoord.y / clipCoord.w;
	clipCoord.z = clipCoord.z / clipCoord.w;

	Size glSize = m_sizeViewInPoints;
	float factor = 1.0 / glCoord.w;
	return Vec2(glSize.width*(clipCoord.x*0.5 + 0.5) * factor, glSize.height*(-clipCoord.y*0.5 + 0.5) * factor);
}

NS_HMS_END