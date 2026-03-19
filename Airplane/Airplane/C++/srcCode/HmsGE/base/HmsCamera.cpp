#include "base/HmsCamera.h"
#include "base/HmsCameraBackgroundBrush.h"
#include "HmsAviDisplayUnit.h"
#include "HmsScreen.h"
#include "render/HmsRenderer.h"
#include "render/HmsQuadCommand.h"
#include "render/HmsGLProgramCache.h"
#include "render/HmsGLStateCache.h"
#include "render/HmsFrameBuffer.h"
#include "render/HmsRenderState.h"

NS_HMS_BEGIN

Camera* Camera::s_pCameraVisiting = nullptr;
Viewport Camera::_defaultViewport;

Camera* Camera::getDefaultCamera()
{
//     auto scene = Director::getInstance()->getRunningScene();
//     if(scene)
//     {
//         return scene->getDefaultCamera();
//     }

    return nullptr;
}

Camera* Camera::create()
{
    Camera* camera = new  Camera();
	if (camera)
	{
		camera->initDefault();
		camera->autorelease();
		camera->setDepth(0);
	}
    
    return camera;
}

Camera* Camera::createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
    auto ret = new  Camera();
    if (ret)
    {
        ret->initPerspective(fieldOfView, aspectRatio, nearPlane, farPlane);
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

Camera* Camera::createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
{
    auto ret = new  Camera();
    if (ret)
    {
        ret->initOrthographic(zoomX, zoomY, nearPlane, farPlane);
        ret->autorelease();
        return ret;
    }
    HMS_SAFE_DELETE(ret);
    return nullptr;
}

Camera::Camera()
: m_pScreen(nullptr)
, _viewProjectionDirty(true)
, _cameraFlag(1)
, _frustumDirty(true)
, _depth(-1)
, _fbo(nullptr)
, m_pRenderRootSpecific(nullptr)
{
    _frustum.setClipZ(true);
    _clearBrush = CameraBackgroundBrush::createDepthBrush(1.f);
    _clearBrush->retain();
}

Camera::~Camera()
{
	HMS_SAFE_RELEASE_NULL(m_pRenderRootSpecific);
    HMS_SAFE_RELEASE_NULL(_fbo);
    HMS_SAFE_RELEASE(_clearBrush);
}

const Mat4& Camera::getProjectionMatrix() const
{
    return _projection;
}
const Mat4& Camera::getViewMatrix() const
{
    Mat4 viewInv(GetNodeToWorldTransform());
    static int count = sizeof(float) * 16;
    if (memcmp(viewInv.m, _viewInv.m, count) != 0)
    {
        _viewProjectionDirty = true;
        _frustumDirty = true;
        _viewInv = viewInv;
        _view = viewInv.getInversed();
    }
    return _view;
}
void Camera::lookAt(const Vec3& lookAtPos, const Vec3& up)
{
    Vec3 upv = up;
    upv.normalize();
    Vec3 zaxis;
    Vec3::subtract(this->GetPoistion3D(), lookAtPos, &zaxis);
    zaxis.normalize();
    
    Vec3 xaxis;
    Vec3::cross(upv, zaxis, &xaxis);
    xaxis.normalize();
    
    Vec3 yaxis;
    Vec3::cross(zaxis, xaxis, &yaxis);
    yaxis.normalize();
    Mat4  rotation;
    rotation.m[0] = xaxis.x;
    rotation.m[1] = xaxis.y;
    rotation.m[2] = xaxis.z;
    rotation.m[3] = 0;
    
    rotation.m[4] = yaxis.x;
    rotation.m[5] = yaxis.y;
    rotation.m[6] = yaxis.z;
    rotation.m[7] = 0;
    rotation.m[8] = zaxis.x;
    rotation.m[9] = zaxis.y;
    rotation.m[10] = zaxis.z;
    rotation.m[11] = 0;
    
    Quaternion  quaternion;
    Quaternion::createFromRotationMatrix(rotation,&quaternion);
    quaternion.normalize();
    SetRotationQuat(quaternion);
}

const Mat4& Camera::getViewProjectionMatrix() const
{
    getViewMatrix();
    if (_viewProjectionDirty)
    {
        _viewProjectionDirty = false;
        Mat4::multiply(_projection, _view, &_viewProjection);
    }
    
    return _viewProjection;
}

void Camera::setAdditionalProjection(const Mat4& mat)
{
    _projection = mat * _projection;
    getViewProjectionMatrix();
}

bool Camera::initDefault()
{
	auto du = CHmsAviDisplayUnit::GetInstance();
	auto size = du->GetViewSize();
	//auto size = Size(1920, 1080);
    //create default camera
	auto projection = du->GetProjection();
    switch (projection)
    {
	case CHmsAviDisplayUnit::Projection::_2D:
        {
            initOrthographic(size.width, size.height, -1024, 1024);
            SetPosition3D(Vec3(0.0f, 0.0f, 0.0f));
            SetRotation3D(Vec3(0.f, 0.f, 0.f));
            break;
        }
	case CHmsAviDisplayUnit::Projection::_3D:
        {
			float zeye = du->GetZEye();
            initPerspective(60, (GLfloat)size.width / size.height, 10, zeye + size.height / 2.0f);
            Vec3 eye(size.width/2, size.height/2.0f, zeye), center(size.width/2, size.height/2, 0.0f), up(0.0f, 1.0f, 0.0f);
            SetPosition3D(eye);
            lookAt(center, up);
            break;
        }
        default:
            CCLOG("unrecognized projection");
            break;
    }
    return true;
}

bool Camera::initPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
    _fieldOfView = fieldOfView;
    _aspectRatio = aspectRatio;
    _nearPlane = nearPlane;
    _farPlane = farPlane;
    Mat4::createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;
    
    return true;
}

bool Camera::initOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
{
    _zoom[0] = zoomX;
    _zoom[1] = zoomY;
    _nearPlane = nearPlane;
    _farPlane = farPlane;
    Mat4::createOrthographicOffCenter(0, _zoom[0], 0, _zoom[1], _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;
    
    return true;
}

bool Camera::resetOrthoNearPlane(float nearPlane)
{
    _nearPlane = nearPlane;
    Mat4::createOrthographicOffCenter(0, _zoom[0], 0, _zoom[1], _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;

    return true;
}

bool Camera::resetOrthoFarPlane(float farPlane)
{
    _farPlane = farPlane;
    Mat4::createOrthographicOffCenter(0, _zoom[0], 0, _zoom[1], _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;

    return true;
}

bool Camera::resetOrthoNearFarPlane(float nearPlane, float farPlane)
{
    _nearPlane = nearPlane;
    _farPlane = farPlane;
    Mat4::createOrthographicOffCenter(0, _zoom[0], 0, _zoom[1], _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;

    return true;
}

bool Camera::resetPerspectiveFarPlane(float farPlane)
{
    _farPlane = farPlane;
    Mat4::createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;

    return true;
}

bool Camera::resetPerspectiveNearPlane(float nearPlane)
{
    _nearPlane = nearPlane;
    Mat4::createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;

    return true;
}

bool Camera::resetPerspectiveNearFarPlane(float nearPlane, float farPlane)
{
    _nearPlane = nearPlane;
    _farPlane = farPlane;
    Mat4::createPerspective(_fieldOfView, _aspectRatio, _nearPlane, _farPlane, &_projection);
    _viewProjectionDirty = true;
    _frustumDirty = true;

    return true;
}

Vec2 Camera::project(const Vec3& src) const
{
    Vec2 screenPos;
    
	auto viewport = CHmsAviDisplayUnit::GetInstance()->GetViewSize();
    Vec4 clipPos;
    getViewProjectionMatrix().transformVector(Vec4(src.x, src.y, src.z, 1.0f), &clipPos);
    
    HMSASSERT(clipPos.w != 0.0f, "clipPos.w can't be 0.0f!");
    float ndcX = clipPos.x / clipPos.w;
    float ndcY = clipPos.y / clipPos.w;
    
    screenPos.x = (ndcX + 1.0f) * 0.5f * viewport.width;
    screenPos.y = (1.0f - (ndcY + 1.0f) * 0.5f) * viewport.height;
    return screenPos;
}

Vec2 Camera::project(const Size& viewport, const Vec3& src) const
{
	Vec2 screenPos;

	Vec4 clipPos;
	getViewProjectionMatrix().transformVector(Vec4(src.x, src.y, src.z, 1.0f), &clipPos);

	HMSASSERT(clipPos.w != 0.0f, "clipPos.w can't be 0.0f!");
	float ndcX = clipPos.x / clipPos.w;
	float ndcY = clipPos.y / clipPos.w;

	screenPos.x = (ndcX + 1.0f) * 0.5f * viewport.width;
	screenPos.y = (1.0f - (ndcY + 1.0f) * 0.5f) * viewport.height;
	return screenPos;
}

Vec3 Camera::projectToGLWorld(const Vec3 & src) const
{
    Vec4 clipPos;
    getViewProjectionMatrix().transformVector(Vec4(src.x, src.y, src.z, 1.0f), &clipPos);

    Vec3 glPos;
//     glPos.x = clipPos.x / clipPos.w;
//     glPos.y = clipPos.y / clipPos.w;
//     glPos.z = clipPos.z / clipPos.w;
    glPos.x = clipPos.x;
    glPos.y = clipPos.y;
    glPos.z = clipPos.z;
    return glPos;
}

Vec2 Camera::projectGL(const Vec3& src) const
{
    Vec2 screenPos;
    
	auto viewport = CHmsAviDisplayUnit::GetInstance()->GetViewSize();
    Vec4 clipPos;
    getViewProjectionMatrix().transformVector(Vec4(src.x, src.y, src.z, 1.0f), &clipPos);
    
    HMSASSERT(clipPos.w != 0.0f, "clipPos.w can't be 0.0f!");
    float ndcX = clipPos.x / clipPos.w;
    float ndcY = clipPos.y / clipPos.w;
    
    screenPos.x = (ndcX + 1.0f) * 0.5f * viewport.width;
    screenPos.y = (ndcY + 1.0f) * 0.5f * viewport.height;
    return screenPos;
}

HmsAviPf::Vec2 Camera::projectGL(const Size& viewport, const Vec3& src) const
{
	Vec2 screenPos;

	Vec4 clipPos;
	getViewProjectionMatrix().transformVector(Vec4(src.x, src.y, src.z, 1.0f), &clipPos);

	HMSASSERT(clipPos.w != 0.0f, "clipPos.w can't be 0.0f!");
	float ndcX = clipPos.x / clipPos.w;
	float ndcY = clipPos.y / clipPos.w;

	screenPos.x = (ndcX + 1.0f) * 0.5f * viewport.width;
	screenPos.y = (ndcY + 1.0f) * 0.5f * viewport.height;
	return screenPos;
}

Vec3 Camera::unproject(const Vec3& src) const
{
    Vec3 dst;
	unproject(CHmsAviDisplayUnit::GetInstance()->GetViewSize(), &src, &dst);
    return dst;
}

Vec3 Camera::unproject(const Size& size, const Vec3& src) const
{
	Vec3 dst;
	unproject(size, &src, &dst);
	return dst;
}

Vec3 Camera::unprojectGL(const Vec3& src) const
{
    Vec3 dst;
	unprojectGL(CHmsAviDisplayUnit::GetInstance()->GetViewSize(), &src, &dst);
    return dst;
}

Vec3 Camera::unprojectGL(const Size& size, const Vec3& src) const
{
	Vec3 dst;
	unprojectGL(size, &src, &dst);
	return dst;
}

void Camera::unproject(const Size& viewport, const Vec3* src, Vec3* dst) const
{
    HMSASSERT(src && dst, "vec3 can not be null");
    
    Vec4 screen(src->x / viewport.width, ((viewport.height - src->y)) / viewport.height, src->z, 1.0f);
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;
    
    getViewProjectionMatrix().getInversed().transformVector(screen, &screen);
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }
    
    dst->set(screen.x, screen.y, screen.z);
}



void Camera::unprojectGL(const Size& viewport, const Vec3* src, Vec3* dst) const
{
    HMSASSERT(src && dst, "vec3 can not be null");
    
    Vec4 screen(src->x / viewport.width, src->y / viewport.height, src->z, 1.0f);
    screen.x = screen.x * 2.0f - 1.0f;
    screen.y = screen.y * 2.0f - 1.0f;
    screen.z = screen.z * 2.0f - 1.0f;
    
    getViewProjectionMatrix().getInversed().transformVector(screen, &screen);
    if (screen.w != 0.0f)
    {
        screen.x /= screen.w;
        screen.y /= screen.w;
        screen.z /= screen.w;
    }
    
    dst->set(screen.x, screen.y, screen.z);
}

bool Camera::isVisibleInFrustum(const AABB* aabb) const
{
    if (_frustumDirty)
    {
        _frustum.initFrustum(this);
        _frustumDirty = false;
    }
    return !_frustum.isOutOfFrustum(*aabb);
}

Frustum * Camera::GetCameraFrustum()
{
	if (_frustumDirty)
	{
		_frustum.initFrustum(this);
		_frustumDirty = false;
	}
	return &_frustum;
}

Frustum * Camera::GetCameraFrustum(float curFarPct)
{
	if (_frustumDirty)
	{
		_frustum.initFrustum(this, curFarPct);
		_frustumDirty = false;
	}
	return &_frustum;
}

float Camera::getDepthInView(const Mat4& transform) const
{
    Mat4 camWorldMat = GetNodeToWorldTransform();
    const Mat4 &viewMat = camWorldMat.getInversed();
    float depth = -(viewMat.m[2] * transform.m[12] + viewMat.m[6] * transform.m[13] + viewMat.m[10] * transform.m[14] + viewMat.m[14]);
    return depth;
}

void Camera::setDepth(int8_t depth)
{
    if (_depth != depth)
    {
        _depth = depth;
        if (m_pScreen)
        {
            //notify scene that the camera order is dirty
            m_pScreen->SetCameraOrderDirty();
        }
    }
}

void Camera::OnEnter()
{
    if (m_pScreen == nullptr)
    {
        auto scene = GetScreen();
        if (scene)
        {
            setScreen(scene);
        }
    }
    CHmsNode::OnEnter();
}

void Camera::OnExit()
{
    // remove this camera from scene
    setScreen(nullptr);
    CHmsNode::OnExit();
}

void Camera::setScreen(CHmsScreen * screen)
{
    if (m_pScreen != screen)
    {
        //remove old scene
        if (m_pScreen)
        {
			auto& cameras = m_pScreen->m_vCameras;
            auto it = std::find(cameras.begin(), cameras.end(), this);
            if (it != cameras.end())
                cameras.erase(it);
            m_pScreen = nullptr;
        }
        //set new scene
        if (screen)
        {
            m_pScreen = screen;
			auto& cameras = m_pScreen->m_vCameras;
            auto it = std::find(cameras.begin(), cameras.end(), this);
            if (it == cameras.end())
            {
				m_pScreen->m_vCameras.push_back(this);
                //notify scene that the camera order is dirty
                m_pScreen->SetCameraOrderDirty();
            }
        }
    }
}

void Camera::clearBackground()
{
    if (_clearBrush)
    {
        _clearBrush->drawBackground(this);
    }
}

void Camera::setFrameBufferObject(FrameBuffer *fbo)
{
    HMS_SAFE_RETAIN(fbo);
    HMS_SAFE_RELEASE_NULL(_fbo);
    _fbo = fbo;
    if(m_pScreen)
    {
        m_pScreen->SetCameraOrderDirty();
    }
}

void Camera::applyFrameBufferObject()
{
    if(nullptr == _fbo)
    {
        FrameBuffer::applyDefaultFBO();
    }
    else
    {
        _fbo->applyFBO();
    }
}

void Camera::apply()
{
    applyFrameBufferObject();
    applyViewport();
}

void Camera::applyViewport()
{
    if(nullptr == _fbo)
    {
        glViewport(getDefaultViewport()._left, getDefaultViewport()._bottom, getDefaultViewport()._width, getDefaultViewport()._height);
    }
    else
    {
        glViewport(_viewport._left * _fbo->getWidth(), _viewport._bottom * _fbo->getHeight(),
                   _viewport._width * _fbo->getWidth(), _viewport._height * _fbo->getHeight());
    }
    
}

int Camera::getRenderOrder() const
{
    int result(0);
    if(_fbo)
    {
        result = _fbo->getFID()<<8;
    }
    else
    {
        result = 127 <<8;
    }
    result += _depth;
    return result;
}

void Camera::Visit(Renderer* renderer, const Mat4 &parentTransform, uint32_t parentFlags)
{
    _viewProjectionUpdated = m_transformUpdated;
    return CHmsNode::Visit(renderer, parentTransform, parentFlags);
}

void Camera::setBackgroundBrush(CameraBackgroundBrush* clearBrush)
{
    HMS_SAFE_RETAIN(clearBrush);
    HMS_SAFE_RELEASE(_clearBrush);
    _clearBrush = clearBrush;
}

NS_HMS_END
