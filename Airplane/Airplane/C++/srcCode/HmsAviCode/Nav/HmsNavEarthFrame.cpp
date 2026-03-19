#include "HmsNavEarthFrame.h"
#include "render/HmsTextureCache.h"
#include "display/HmsImageNode.h"
#include "display/HmsStretchImageNode.h"
#include "math/HmsMathBase.h"
#include "HmsGlobal.h"
#include "DataInterface/HmsDataBus.h"

#include "base/HmsRay.h"
#include "DataInterface/TerrainServer/HmsTerrainServer.h"
#include <math.h>


using namespace std;

USING_NS_HMS;

#define USE_CAMERA_ROOT  1

#define MACRO_ENABLE_EARTH_INERTIA

CHmsNavEarthFrame * CHmsNavEarthFrame::Create(const HmsAviPf::Size & size)
{
	CHmsNavEarthFrame *ret = new CHmsNavEarthFrame();
	if (ret && ret->Init(size))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsNavEarthFrame::CHmsNavEarthFrame()
{
#if USE_CAMERA_ROOT
	m_nCameraFlag = (uint32_t)CameraFlag::DEFAULT;
#else
	m_nCameraFlag = (uint32_t)CameraFlag::USER1;
#endif

	m_posEye = Vec3(0, 0, 2);
	m_posEyeUp = Vec3(0, 1, 0);
	m_posTraget = Vec3(0, 0, 0);

	m_fNear = 1.0003f;
	m_fFar = 2.0f;
	m_fCameraNear = 0.00001f;
	m_fCameraFar = 5.0f;
	m_fFieldOfView = 60.0f;
	m_fFieldOfViewRadian = MATH_DEG_TO_RAD(m_fFieldOfView);
	m_fAspectRatio = 1.0f;
	m_fDistanceEye2Traget = m_posEye.length();

	m_fSeeAbleRadian = m_fFieldOfViewRadian;
	m_fCameraViewDistanceCut = fabsf(m_fDistanceEye2Traget - cos(m_fFieldOfViewRadian / 2.0f));

	m_frame2DInterface = nullptr;

	m_bPlaneAlwaysCenter = false;
	m_funcPlaneAlwaysCenterCallback = nullptr;

#ifdef MACRO_ENABLE_EARTH_INERTIA
	m_bEnableInertia = false;
	m_inertiaAngle = 0.0f;
#endif
	m_fFieldOfViewInit = m_fFieldOfView;
	m_fFieldOfViewRadianInit = m_fFieldOfViewRadian;
}

CHmsNavEarthFrame::~CHmsNavEarthFrame()
{

}

bool CHmsNavEarthFrame::Init(const HmsAviPf::Size & size)
{
	if (!CHmsNode::Init())
	{
		return false;
	}

	SetContentSize(size);
	m_sizeInit = size;

	auto & fboSize = size;
	auto fbo = FrameBuffer::create(1, fboSize.width, fboSize.height);
	auto rt = RenderTarget::create(fboSize.width, fboSize.height);
	auto rtDS = RenderTargetDepthStencil::create(fboSize.width, fboSize.height);
#if _NAV_SVS_LOGIC_JUDGE
	if (!fbo || !rt || !rtDS)
		return false;
#endif
	fbo->attachRenderTarget(rt);
	fbo->attachDepthStencilTarget(rtDS);
	fbo->setClearColor(Color4F(0, 0, 1.0, 1.0));
	m_frameBuffer = fbo;

	//m_mapTempFrameBuffer[size.width] = m_frameBuffer;

	m_fAspectRatio = (GLfloat)fboSize.width / fboSize.height;
	m_fFieldOfViewRadian = MATH_DEG_TO_RAD(m_fFieldOfView);
	auto camera = Camera::createPerspective(m_fFieldOfView, m_fAspectRatio, m_fCameraNear, m_fCameraFar);
#if _NAV_SVS_LOGIC_JUDGE
	if (!camera)
		return false;
#endif
	camera->setCameraFlag(CameraFlag(m_nCameraFlag));
	camera->SetPosition3D(m_posEye);
	camera->lookAt(m_posTraget, m_posEyeUp);
	camera->setFrameBufferObject(fbo);
	camera->setDepth(-1);
	m_camera = camera;
	
#if USE_CAMERA_ROOT
	m_frameRoot = CHmsNode::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_frameRoot)
		return false;
#endif
	m_frameRoot->SetCameraMask(m_nCameraFlag);
	m_frameRoot->AddChild(camera);
	m_camera->setSpecificRenderRoot(m_frameRoot);

	m_mgrMercatorEarthTile = CHmsMercatorEarthTileMgr::Create();
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_mgrMercatorEarthTile)
		return false;
#endif
	m_mgrMercatorEarthTile->SetScreenSize(size);
	AddEarthNode(m_mgrMercatorEarthTile);

#else
	this->AddChild(camera);
#endif

	//SetCameraMask(m_nCameraFlag);

	UpdateCameraPosition();
	//TestEarth();
	return true;
}

HmsAviPf::Texture2D * CHmsNavEarthFrame::GetTexture()
{
	HmsAviPf::Texture2D * tex = nullptr;

	if (m_frameBuffer && m_frameBuffer->getRenderTarget())
	{
		tex = m_frameBuffer->getRenderTarget()->getTexture();
	}

	return tex;
}

HmsAviPf::CHmsImageNode * CHmsNavEarthFrame::CreateImageFrameNode()
{
	HmsAviPf::CHmsImageNode * node = nullptr;

	if (m_frameBuffer && m_frameBuffer->getRenderTarget())
	{
		node = CHmsImageNode::CreateWithTexture(m_frameBuffer->getRenderTarget()->getTexture());
	}
	return node;
}

CHmsNavViewer * CHmsNavEarthFrame::CreateNavViewer()
{
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_frameBuffer
		|| !m_frameBuffer->getRenderTarget())
		return nullptr;
#endif
	auto temp = CHmsNavViewer::Create(m_frameBuffer->getRenderTarget()->getTexture(), this);
	if (m_frame2DInterface)
	{
		temp->SetFrame2DRoot(m_frame2DInterface);
	}
	return temp;
}

void CHmsNavEarthFrame::AddEarthNode(HmsAviPf::CHmsNode * node)
{
	if (node)
	{
		node->SetCameraMask(m_nCameraFlag);

#if USE_CAMERA_ROOT
#if _NAV_SVS_LOGIC_JUDGE
		if (m_frameRoot)
#endif
		m_frameRoot->AddChild(node);
#else
		AddChild(node);
#endif
	}
}

void CHmsNavEarthFrame::AttachToEarthDrawNode(HmsAviPf::CHmsNode * node)
{
	if (m_mgrMercatorEarthTile)
	{
		m_mgrMercatorEarthTile->AddOther3DLayer(node);
	}
}

#include "HmsMercatorEarthTile.h"
void CHmsNavEarthFrame::TestEarth()
{
#if 0
	auto tile = CHmsMercatorEarthTile::Create(1, 0, 0);
	tile->SetPosition3D(Vec3(0, 0, 0));
	this->AddEarthNode(tile);
	tile = CHmsMercatorEarthTile::Create(1, 0, 1);
	tile->SetPosition3D(Vec3(0, 0, 0));
	this->AddEarthNode(tile);
	tile = CHmsMercatorEarthTile::Create(1, 1, 0);
	tile->SetPosition3D(Vec3(0, 0, 0));
	this->AddEarthNode(tile);
	tile = CHmsMercatorEarthTile::Create(1, 1, 1);
	tile->SetPosition3D(Vec3(0, 0, 0));
	this->AddEarthNode(tile);
#else
	int nLayer = 3;
	int nLayerCnt = 1 << nLayer;
	for (int i = 0; i < nLayerCnt; i++)
	{
		for (int j = 0; j < nLayerCnt; j++)
		{
			auto tile = CHmsMercatorEarthTile::Create(nLayer, i, j);
#if _NAV_SVS_LOGIC_JUDGE
			if (tile)
#endif
			{
				tile->SetPosition3D(Vec3(0, 0, 0));
				this->AddEarthNode(tile);
			}
		}
	}
#endif
}

void CHmsNavEarthFrame::MoveCameraToMinDis()
{
	auto normal = m_posEye.getNormalized();
	m_posEye = normal*m_fNear;
	UpdateCameraPosition();
	if (m_frame2DInterface)
	{
		m_frame2DInterface->SetNeedUpdate2DEles(true);
	}
}
void CHmsNavEarthFrame::MoveCameraToMaxDis()
{
	auto normal = m_posEye.getNormalized();
	m_posEye = normal*m_fFar;
	UpdateCameraPosition();
	if (m_frame2DInterface)
	{
		m_frame2DInterface->SetNeedUpdate2DEles(true);
	}
}

void CHmsNavEarthFrame::MoveNear()
{
#if 0
	auto normal = m_posEye.getNormalized();
	auto length = m_fDistanceEye2Traget;
	length -= 0.1;
	if (length < m_fNear)
	{
		length = m_fNear;
	}
	m_posEye = normal*length;
	UpdateCameraPosition();
#endif
	auto normal = m_posEye.getNormalized();
	auto length = m_fDistanceEye2Traget;
	length -= (length - 1)*0.1;
	length = length < m_fNear ? m_fNear : length;
	m_posEye = normal*length;
	UpdateCameraPosition();

    Update3DClipZBufer();
}

void CHmsNavEarthFrame::MoveFar()
{
// 	auto normal = m_posEye.getNormalized();
// 	auto length = m_fDistanceEye2Traget;
// 	length += 0.1;
// 	if (length > m_fFar)
// 	{
// 		length = m_fFar;
// 	}
// 	m_posEye = normal*length;
// 	UpdateCameraPosition();
	auto normal = m_posEye.getNormalized();
	auto length = m_fDistanceEye2Traget;
	length += (length - 1)*0.1;
	length = length > m_fFar ? m_fFar : length;
	m_posEye = normal*length;
	UpdateCameraPosition();

    Update3DClipZBufer();
}

void CHmsNavEarthFrame::UpdateCameraPosition(bool bUpdateLayer)
{
	int nLayer = 0;
	if (m_camera)
	{
		m_fDistanceEye2Traget = m_posEye.length();
		m_camera->SetPosition3D(m_posEye);
		m_camera->lookAt(m_posTraget, m_posEyeUp);

		m_fSeeAbleRadian = GetEarthShowRadius(true);
		//m_fFieldOfView = MATH_RAD_TO_DEG(m_fFieldOfViewRadian);
		m_fCameraViewDistanceCut = fabsf(m_fDistanceEye2Traget + 0.3 - cos(m_fSeeAbleRadian / 2.0f));

		if (m_mgrMercatorEarthTile && bUpdateLayer)
		{
			nLayer = m_mgrMercatorEarthTile->SetSeeAbleRadian(m_fSeeAbleRadian);
		}
	}

	//printf("distance=%f Angle=%f layer=%d\n", m_posEye.length(), GetEarthShowRadiusDegree(true), nLayer);

	if (m_frame2DInterface)
	{
		m_frame2DInterface->SetEarthLayer(nLayer);
		m_frame2DInterface->SetNeedUpdate2DEles(true);
	}
}
#include "math/Quaternion.h"
void CHmsNavEarthFrame::RotationEarth(const HmsAviPf::Vec2 & delta)
{
#if 1
	auto length = m_fDistanceEye2Traget;
	{
		auto x = atan2f(delta.x, length);
		auto y = atan2f(delta.y, length);

		Vec3 zAxis(0.0f, 0.0f, 1.0f);
		Vec3 xAxis(1.0f, 0.0f, 0.0f);
		Vec3 yAxis(0.0f, 1.0f, 0.0f);
		auto qy = Quaternion(yAxis,-x);
		auto qx = Quaternion(xAxis,  -y);
		qy *= qx;
		m_posEye = qy*m_posEye;

		//ConvertTouchToNodeSpace();
	}
#else
	Vec3 nearP(delta.x, delta.y, -1.0f), farP(delta.x, delta.y, 1.0f);

	nearP = m_camera->unproject(nearP);
	farP = m_camera->unproject(farP);
	Vec3 dir(farP - nearP);
	float dist = 0.0f;
	float ndd = Vec3::dot(Vec3(0, 1, 0), dir);
	if (ndd == 0)
		dist = 0.0f;
	float ndo = Vec3::dot(Vec3(0, 1, 0), nearP);
	dist = (0 - ndo) / ndd;
	Vec3 p = nearP + dist *  dir;
	int a = 0;
#endif

	UpdateCameraPosition();
}

void CHmsNavEarthFrame::RotationEarth(const HmsAviPf::Vec2 & begin, const HmsAviPf::Vec2 & end, bool bUpdateLayer)
{
	if (begin == end)
	{
		return;
	}
	Vec3 intersection_begin, intersection_end;
	{
		Vec3 nearP(begin.x, begin.y, -1.0f), farP(begin.x, begin.y, 1.0f);
		nearP = m_camera->unproject(m_sizeContent, nearP);
		farP = m_camera->unproject(m_sizeContent, farP);
		Vec3 dir(farP - nearP);
		Ray ray(nearP, dir);
		Sphere sphere(Vec3(0, 0, 0), 1.0f);
		Vec3 intersection;
		ray.intersects(sphere, intersection_begin);
	}
	{
		Vec3 nearP(end.x, end.y, -1.0f), farP(end.x, end.y, 1.0f);
		nearP = m_camera->unproject(m_sizeContent, nearP);
		farP = m_camera->unproject(m_sizeContent, farP);
		Vec3 dir(farP - nearP);
		Ray ray(nearP, dir);
		Sphere sphere(Vec3(0, 0, 0), 1.0f);
		Vec3 intersection;
		ray.intersects(sphere, intersection_end);
	}

	Vec3 axis;	/* Axis of rotation */
	float phi;  /* how much to rotate about axis */
	Vec3 p1, p2, d;
	float t;

// 	if ((begin.x == end.x)&&(begin.y == end.y))
// 	{
// 		//return;
// 	}
// 	auto fRadius = m_fDistanceEye2Traget;
// 
// 	/*
// 	* First, figure out z-coordinates for projection of P1 and P2 to
// 	* deformed sphere
// 	*/
// 	p1.x = begin.x;
// 	p1.y = begin.y;
// 	p1.z = ProjectToSphere(fRadius, p1.x, p1.y);
// 
// 	p2.x = end.x;
// 	p2.y = end.y;
// 	p2.z = ProjectToSphere(fRadius, p2.x, p2.y);

	p1 = intersection_begin;
	p2 = intersection_end;

	/*
	*  Now, we want the cross product of P1 and P2
	*/
	Vec3::cross(p2, p1, &axis);  //calculate rotation axis
	axis.normalize();

	/*
	*  Figure out how much to rotate around that axis.
	*/
	d = p1 - p2;

	// 求出正弦值
	t = d.length() / (2.0f);

	/*
	* Avoid problems with out-of-control values...
	*/
	if (t > 1.0f) t = 1.0f;
	if (t < -1.0f) t = -1.0f;

    phi = HmsMath_asin(t) * 2;

	Quaternion q(axis, phi);
	m_posEye = q*m_posEye;

	UpdateCameraPosition(bUpdateLayer);

#ifdef MACRO_ENABLE_EARTH_INERTIA
	m_inertiaAxis = axis;
	m_inertiaAngle = phi;
#endif

	SetPlaneAlwaysCenter(false);
}

float CHmsNavEarthFrame::ProjectToSphere(float r, float x, float y)
{
	float d, t, z;
	d = sqrt(x*x + y*y);
	if (d < r * 0.70710678118654752440)//inside sphere
	{
		z = sqrt(r*r - d*d);
	}
	else                               //on hyperbola
	{
		t = r / 1.41421356237309504880;
		z = t*t / d;
	}
	return z;
}

void CHmsNavEarthFrame::UpdateFieldOfView()
{
	auto size = GetContentSize();
	m_fFieldOfView = m_fFieldOfView * size.width/m_sizeInit.width;
	m_fFieldOfViewRadian = MATH_DEG_TO_RAD(m_fFieldOfView);
}

float CHmsNavEarthFrame::GetViewsScale()
{
	return GetContentSize().width / m_sizeInit.width;
}

extern float g_fHmsTrimDistance;
void CHmsNavEarthFrame::Update3DClipZBufer()
{
    if (m_frame2DInterface)
    {
        Vec3 ea(0, 0, 0);
        Vec3 ret = m_frame2DInterface->EarthPointToGLWorld(ea);
        g_fHmsTrimDistance = ret.z - 0.7f;
    }
}

float CHmsNavEarthFrame::GetEarthShowRadius(bool bVertical)
{
	//c = bcosA + Sqrt(a*a - b*bsinA*sinA)
	auto aRadian = m_fFieldOfViewRadian/2.0f;
	if (!bVertical)
	{
		aRadian *= m_fAspectRatio;
	}
	auto sinA = sinf(aRadian);
	auto cosA = cosf(aRadian);
	auto b = m_fDistanceEye2Traget;
	auto a = 1;
	auto bSinA = b*sinA;
	float c = b*cosA;
	if (bSinA < a)
	{
		c -= sqrt(1 - bSinA*bSinA);
	}
	else if (bSinA > a)
	{
		auto cosC = a / b;
		return (acosf(cosC));
	}
	auto sinC = c*sinA / a;
	auto fAngle = (asinf(sinC));
	return fAngle;
}

float CHmsNavEarthFrame::GetEarthShowRadiusDegree(bool bVertical)
{
	return MATH_RAD_TO_DEG(GetEarthShowRadius(bVertical));
}

void CHmsNavEarthFrame::OnEnter()
{
	CHmsNode::OnEnter();
	if (m_camera)
	{
		auto screen = GetScreen();
		m_camera->setScreen(screen);
		m_camera->OnEnter();
	}

	SetScheduleUpdate();
	m_bRunning = true;
}

void CHmsNavEarthFrame::OnExit()
{
	CHmsNode::OnExit();
	if (m_camera)
	{
		m_camera->OnExit();
	}
	SetUnSchedule();
	m_bRunning = false;
}

void CHmsNavEarthFrame::Update(float delta)
{
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
#if _NAV_SVS_LOGIC_JUDGE
	if (!dataBus || !m_camera)
		return;
#endif
    auto aftData = dataBus->GetData();

    if (m_bPlaneAlwaysCenter && aftData->bLatLonValid)
    {
#if _NAV_SVS_LOGIC_JUDGE
		if (aftData)
#endif
        SetViewCenterByLonLat(aftData->longitude, aftData->latitude);
    }

	float fNearPct = (m_fCameraViewDistanceCut - m_fCameraNear) / (m_fCameraFar - m_fCameraNear);
	m_mgrMercatorEarthTile->EarthClipping(m_camera->GetCameraFrustum(fNearPct));

	if (m_frame2DInterface)
	{
		m_frame2DInterface->Frame2DClipping(fNearPct);
		m_frame2DInterface->Update(delta);
	}

#ifdef MACRO_ENABLE_EARTH_INERTIA
	if (m_bEnableInertia)
	{
		m_inertiaAngle *= 0.95f;
		if (m_inertiaAngle < 0.001)
		{
			m_inertiaAngle = 0;
			m_bEnableInertia = false;
		}
		else
		{
			Quaternion q(m_inertiaAxis, m_inertiaAngle);
			m_posEye = q*m_posEye;

			UpdateCameraPosition(false);
		}	
	}
#endif
}


void CHmsNavEarthFrame::OnClickOnEarth(const HmsAviPf::Vec2 & pos)
{
	if (m_frame2DInterface)
	{
		m_frame2DInterface->OnClickOnEarth(pos);
	}
	return;


	//Vec3 nearP(pos.x, m_sizeContent.height - pos.y, -1.0f), farP(pos.x, m_sizeContent.height - pos.y, 1.0f);
// 	Vec3 nearP(pos.x, pos.y, -1.0f), farP(pos.x, pos.y, 1.0f);
// 
// 	nearP = m_camera->unproject(m_sizeContent, nearP);
// 	farP = m_camera->unproject(m_sizeContent, farP);
// 	Vec3 dir(farP - nearP);
// 
// 	Ray ray(nearP, dir);
// 	Sphere sphere(Vec3(0, 0, 0), 1.0f);
// 	Vec3 intersection;
// 	if (ray.intersects(sphere, intersection))
// 	{
// 		auto lat = asin(intersection.y);
// 		auto lon = atan2f(intersection.x, intersection.z);
// 		auto str = CHmsGlobal::FormatString("Lon:%f Lat:%f", MATH_RAD_TO_DEG(lon), MATH_RAD_TO_DEG(lat));
// 		auto label = CHmsGlobal::CreateLabel(str.c_str(), 12);
// // 		label->SetPosition3D(intersection);
// // 		label->SetScaleX(1/30);
// // 		label->SetScaleY(1 / 30);
// //		AddEarthNode(label);
// //		asin(intersection.y);
// 		auto pos = m_camera->project(m_sizeContent, intersection);
// 		//pos.y = m_sizeContent.height - pos.y;
// 		label->SetPosition(pos);
// 		label->SetUserObject(EarthBindData::Create(intersection));
// 		m_frame2DRoot->AddChild(label);
// 
// 		auto server = CHmsGlobal::GetInstance()->GetTerrainServer();
// 		std::vector<AltitudeDataStu> * vAltData = new std::vector<AltitudeDataStu>;
// 
// 		AltitudeDataStu ads;
// 		ads.lat = 30.58;
// 		ads.lon = 103.9483333;
// 		vAltData->push_back(ads);
// 		server->GetAltDataByAsyn(vAltData, [=](bool bRet, std::vector<AltitudeDataStu> * vAltData)
// 		{
// 			float feet = 3.2808399f*vAltData->at(0).altitude;
// 			int a = 0;
// 		});
// 		int a = 0;
// 	}
	

// 	float dist = 0.0f;
// 	float ndd = Vec3::dot(Vec3(0, 1, 0), dir);
// 	if (ndd == 0)
// 		dist = 0.0f;
// 	float ndo = Vec3::dot(Vec3(0, 1, 0), nearP);
// 	dist = (0 - ndo) / ndd;
// 	Vec3 p = nearP + dist *  dir;
//	int a = 0;
}

void CHmsNavEarthFrame::SetCurLayerToTen()
{
	m_posEye = m_posEye.getNormalized() * 1.02541804f;

	UpdateCameraPosition();

	if (m_frame2DInterface)
	{
		m_frame2DInterface->SetNeedUpdate2DEles(true);
	}
}

void CHmsNavEarthFrame::SetViewToAft()
{
    CHmsDataBus *dataBus = CHmsGlobal::GetInstance()->GetDataBus();
#if _NAV_SVS_LOGIC_JUDGE
	if (!dataBus)
		return;
#endif
    auto aftData = dataBus->GetData();
#if _NAV_SVS_LOGIC_JUDGE
	if (!aftData)
		return;
#endif
    auto earthPos = CHmsFrame2DInterface::LonLatToEarthPoint(Vec2(aftData->longitude, aftData->latitude));
    m_posEye = earthPos * 1.1f;
    UpdateCameraPosition();

    if (m_frame2DInterface)
    {
        m_frame2DInterface->SetNeedUpdate2DEles(true);
    }
}

void CHmsNavEarthFrame::SetViewCenterByLonLat(float lon, float lat)
{
	auto len = m_posEye.length();
	auto earthPos = CHmsFrame2DInterface::LonLatToEarthPoint(Vec2(lon, lat));
	m_posEye = earthPos * len;
	UpdateCameraPosition();
}

void CHmsNavEarthFrame::ReloadEarthTile()
{
	if (m_mgrMercatorEarthTile)
		m_mgrMercatorEarthTile->ReloadEarthTile();
}

void CHmsNavEarthFrame::Resize(const HmsAviPf::Size & size, int nLevel, CHmsNavViewer * viewer)
{
	//return;
	SetContentSize(size);

	auto & fboSize = size;
#if 0
	auto & tempFbo = m_mapTempFrameBuffer[size.width];
	if (nullptr == tempFbo)
	{
		auto fbo = FrameBuffer::create(1, fboSize.width, fboSize.height);
		auto rt = RenderTarget::create(fboSize.width, fboSize.height);
		auto rtDS = RenderTargetDepthStencil::create(fboSize.width, fboSize.height);
		fbo->attachRenderTarget(rt);
		fbo->attachDepthStencilTarget(rtDS);
		fbo->setClearColor(Color4F(0, 0, 1.0, 1.0));
		tempFbo = fbo;
	}

	m_frameBuffer = tempFbo;
#endif
	m_fAspectRatio = (GLfloat)fboSize.width / fboSize.height;
	//UpdateFieldOfView();
	m_fFieldOfViewRadian = MATH_DEG_TO_RAD(m_fFieldOfView);
	if (m_camera)
	{
		m_camera->initPerspective(m_fFieldOfView, m_fAspectRatio, m_fCameraNear, m_fCameraFar);
		m_camera->setFrameBufferObject(m_frameBuffer);
	}
	//SetCameraMask(m_nCameraFlag);

	if (m_frame2DInterface)
	{
		m_frame2DInterface->Resize(size, nLevel);
	}

	UpdateCameraPosition();

	if (viewer)
	{
#if 0
		auto texture = m_frameBuffer->getRenderTarget()->getTexture();
		viewer->SetTexture(texture);
		Rect rect = Rect::ZERO;
		if (texture)
			rect.size = texture->getContentSize();
		viewer->SetTextureRect(rect);
#else
		Rect rectTexture(Vec2(), m_sizeInit);
		//viewer->SetContentSize(size);
		viewer->SetTextureRect(rectTexture, false, size);
		//viewer->SetScaleX(GetViewsScale());
#endif
		
	}

	if (m_mgrMercatorEarthTile)
		m_mgrMercatorEarthTile->SetScreenSize(size);
}

void CHmsNavEarthFrame::ScaleAndRotationEarth(const HmsAviPf::Vec2 & begin1, const HmsAviPf::Vec2 & begin2, const HmsAviPf::Vec2 & end1, const HmsAviPf::Vec2 & end2, bool bUpdateLayer)
{
	auto posLastDelta = begin2 - begin1;
	auto posDelta = end2 - end1;

	auto scale = posDelta.getLength() / posLastDelta.getLength();

	if (!isnormal(scale))
	{
		return;
	}

	auto fDestSeeAbleRadian = m_fSeeAbleRadian / scale;
	//   a/sina = b/sinb
	auto fDestDistanceEye2Traget = sin(SIM_Pi - m_fFieldOfViewRadian/2.0f - fDestSeeAbleRadian) / sin(m_fFieldOfViewRadian / 2.0f);

	bool bUpdataCenter = false;
	Vec3 p1;
	Vec2 posScreenEnd;
	Vec2 posScreenBegin;

	if (m_frame2DInterface)
	{
		if (m_frame2DInterface->Frame2DPointToEarthPoint(begin1, p1))
		{
			bUpdataCenter = true;
			posScreenEnd = end1;
		}
		else if (m_frame2DInterface->Frame2DPointToEarthPoint(begin2, p1))
		{
			posScreenEnd = end2;
			bUpdataCenter = true;
		}
	}

	//距离剪裁
	//m_fDistanceEye2Traget;
	fDestDistanceEye2Traget = fmax(fmin(m_fFar, fDestDistanceEye2Traget), m_fNear);
	m_posEye = m_posEye.getNormalized() * fDestDistanceEye2Traget;

	//重新确定眼睛位置
	if (bUpdataCenter)
	{
		
		if (m_camera)
		{
			m_camera->SetPosition3D(m_posEye);
			if (m_frame2DInterface)
			{
				posScreenBegin = m_frame2DInterface->EarthPointToFramePoint(p1);
				RotationEarth(posScreenBegin, posScreenEnd, bUpdateLayer);
				//UpdateCameraPosition();
			}
		}	
	}
	else
	{
		UpdateCameraPosition(bUpdateLayer);
	}

    Update3DClipZBufer();
}

void CHmsNavEarthFrame::SetEnableInertia(bool b)
{
	m_bEnableInertia = b;
}

void CHmsNavEarthFrame::SetInertiaAxisAndAgnle(Vec3 axis, float angle)
{
	m_inertiaAxis = axis;
	m_inertiaAngle = angle;
}

void CHmsNavEarthFrame::GetInertiaAxisAndAgnle(Vec3 &axis, float &angle)
{
	axis = m_inertiaAxis;
	angle = m_inertiaAngle;
}

void CHmsNavEarthFrame::SetFrame2DRoot(CHmsFrame2DInterface *pFrame)
{
	if (pFrame)
	{
		m_frame2DInterface = pFrame;
		m_mgrMercatorEarthTile->SetFrame2DRoot(m_frame2DInterface);
	}
}

Camera * CHmsNavEarthFrame::GetCamera()
{
	return m_camera.get();
}

void CHmsNavEarthFrame::SetPlaneAlwaysCenter(bool b)
{
	m_bPlaneAlwaysCenter = b;
	if (m_funcPlaneAlwaysCenterCallback)
	{
		m_funcPlaneAlwaysCenterCallback(m_bPlaneAlwaysCenter);
	}
}

void CHmsNavEarthFrame::SetPlaneAlwaysCenterCallback(std::function<void(bool b)> func)
{
	m_funcPlaneAlwaysCenterCallback = func;
}

CHmsNavViewer * CHmsNavViewer::Create(Texture2D *texture, CHmsNavEarthFrame * frame)
{
	CHmsNavViewer *ret = new CHmsNavViewer(frame);
	if (ret && ret->InitWithTexture(texture))
	{
		ret->autorelease();
		return ret;
	}
	HMS_SAFE_DELETE(ret);
	return nullptr;
}

CHmsNavViewer::CHmsNavViewer(CHmsNavEarthFrame * frame)
	: HmsUiEventInterface(this)
	, m_earthFrame(frame)
	, m_frame2DInterface(nullptr)
	, m_bPointInFrame2D(false)
	, m_bTouchDistribute(false)
	, m_bNeedCloseMesure(false)
	, m_eTouchType(NAV_TOUCH_TYPE::none_used)
	, m_pClickDelayAction(nullptr)
	, m_bEnableClickDelay(false)
{

}

bool CHmsNavViewer::OnPressed(const Vec2 & posOrigin)
{
	if (m_frame2DInterface)
	{
		if (m_bNeedCloseMesure)
		{
			auto mapMeasure = m_frame2DInterface->GetMapMeasure();
			if (mapMeasure)
			{
				mapMeasure->CloseMeasure();
			}
			m_bNeedCloseMesure = false;
		}

		if (m_frame2DInterface->OnPressed(posOrigin))
		{
			m_bPointInFrame2D = true;
			return true;
		}
		else
		{
			AddClickDelay(posOrigin);
		}
	}
	m_posLast = posOrigin;
	m_posBegin = m_posLast;

#ifdef MACRO_ENABLE_EARTH_INERTIA
	m_time.RecordCurrentTime();
	if (m_earthFrame)
	{
		m_earthFrame->SetEnableInertia(false);
		m_earthFrame->SetInertiaAxisAndAgnle(Vec3(), 0);
	}
#endif

	return true;
}


auto GetRelativelyPos = [](const Vec2 & pos, const Size & size)
{
	Vec2 out;
	out.x = 2.0 * (pos.x) / (size.width) - 1.0f;
	//out.y = 2.0 * (pos.y) / (size.height) - 1.0f;
	out.y = 2.0 * (size.height - pos.y) / (size.height) - 1.0f;
	return out;
};

void CHmsNavViewer::OnMove(const Vec2 & posOrigin)
{
	{
		auto tmp = posOrigin - m_posLast;
		if (tmp.length() > 5)
		{
			CancelClickDelay();
		}
	}
	

	if (m_bPointInFrame2D)
	{
		if (m_frame2DInterface)
		{
			m_frame2DInterface->OnMove(posOrigin);
			return;
		}
	}
	else if (m_earthFrame)
	{
		m_earthFrame->RotationEarth(m_posLast, posOrigin);
	}

	m_posLast = posOrigin;
}

void CHmsNavViewer::OnReleased(const Vec2 & posOrigin)
{
	CancelClickDelay();

	if (m_bPointInFrame2D)
	{
		m_bPointInFrame2D = false;
		if (m_frame2DInterface)
		{
			m_frame2DInterface->OnReleased(posOrigin);
			return;
		}
	}
	else
	{
		auto delat = posOrigin - m_posBegin;
		if (delat.length() < 5)
		{
// 			if (m_earthFrame)
// 			{
// 				m_earthFrame->OnClickOnEarth(posOrigin);
// 			}
		}
		else if (m_earthFrame)
		{
			m_earthFrame->RotationEarth(m_posLast, posOrigin);

#ifdef MACRO_ENABLE_EARTH_INERTIA
			auto elapsed = m_time.GetElapsed();
			if (elapsed < 0.5)
			{
				m_earthFrame->SetEnableInertia(true);
			}
#endif
		}
	}

	m_posLast = posOrigin;
}

void CHmsNavViewer::AddClickDelay(HmsAviPf::Vec2 pos)
{
	CancelClickDelay();

	m_bEnableClickDelay = true;
	
	m_pClickDelayAction = CHmsActionDelayCallback::Create(0.8f, HMS_CALLBACK_0(CHmsNavViewer::PressClickDelay, this, pos.x, pos.y));
#if _NAV_SVS_LOGIC_JUDGE
	if (!m_pClickDelayAction)
		return;
#endif
	this->RunAction(m_pClickDelayAction);
}
void CHmsNavViewer::CancelClickDelay()
{
	m_bEnableClickDelay = false;
	if (m_pClickDelayAction)
	{
		this->StopActioin(m_pClickDelayAction);
		m_pClickDelayAction = nullptr;
	}
}
void CHmsNavViewer::PressClickDelay(float x, float y)
{
	if (m_bEnableClickDelay)
	{
		m_bEnableClickDelay = false;
		m_pClickDelayAction = nullptr;
		if (m_earthFrame)
		{
			m_earthFrame->OnClickOnEarth(Vec2(x, y));
		}
	}
}

bool CHmsNavViewer::OnMutiPressed(const std::vector<UiEventPoint> &pos)
{
	m_bTouchDistribute = true;
	
	m_mapViewPoint.clear();

	for (auto & c:pos)
	{
		NavViewPoint nvp;
		nvp.nID = c.Id;
		nvp.pos = c.uiPoint;
		m_mapViewPoint[c.Id] = nvp;
	}

	auto delay = CHmsActionDelayCallback::Create(1.1f, HMS_CALLBACK_0(CHmsNavViewer::MutiTouchDelay, this));
#if _NAV_SVS_LOGIC_JUDGE
	if(delay)
#endif
	this->RunAction(delay);
	return true;
}

void CHmsNavViewer::OnMutiMove(const std::vector<UiEventPoint> &pos)
{
	CancelClickDelay();

	FillPosData(pos);

	if (m_bTouchDistribute)
	{
		bool bMoved = false;
		for (auto & c:m_mapViewPoint)
		{
#define MOVE_CHANGE  8
			auto delta = c.second.posLast - c.second.pos;
			if (delta.getLengthSq() > MOVE_CHANGE * MOVE_CHANGE)
			{
				bMoved = true;
			}
		}

		if (bMoved)
		{
			m_bTouchDistribute = false;
			CheckAndChangeTouchType(NAV_TOUCH_TYPE::map_scale);
		}
	}

	if (pos.size() == 1)
	{
		int a = 0;
	}

	

	DistributeTouchMsg(false);
}

void CHmsNavViewer::OnMutiReleased(const std::vector<UiEventPoint> &pos)
{
	CancelClickDelay();

	if (m_bTouchDistribute)
	{
		m_bTouchDistribute = false;
	}

	FillPosData(pos);

	DistributeTouchMsg(true);

	for (auto & c:pos)
	{
		m_mapViewPoint.erase(c.Id);
	}
	if (!m_mapViewPoint.size())
	{
		CheckAndChangeTouchType(NAV_TOUCH_TYPE::none_used, true);
	}
}

void CHmsNavViewer::MutiTouchDelay()
{
	if (m_bTouchDistribute)
	{
		m_bTouchDistribute = false;
		CheckAndChangeTouchType(NAV_TOUCH_TYPE::measure_distance);
		DistributeTouchMsg(false);
	}
	
}

bool CHmsNavViewer::CheckAndChangeTouchType(NAV_TOUCH_TYPE eType, bool bMeasurecloseDelay)
{
	if (m_bNeedCloseMesure)
	{
		if (m_frame2DInterface)
		{
			auto mapMeasure = m_frame2DInterface->GetMapMeasure();
			if (mapMeasure)
			{
				mapMeasure->CloseMeasure();
			}
		}
		
		m_bNeedCloseMesure = false;
	}

	if (m_eTouchType != eType)
	{
		if (m_eTouchType == NAV_TOUCH_TYPE::measure_distance)
		{
			if (bMeasurecloseDelay)
			{
				m_bNeedCloseMesure = true;
			}
			else
			{
				if (m_frame2DInterface)
				{
					auto mapMeasure = m_frame2DInterface->GetMapMeasure();
					if (mapMeasure)
					{
						mapMeasure->CloseMeasure();
					}
				}
			}
		}

		m_eTouchType = eType;
		return true;
	}
	return false;
}

void CHmsNavViewer::FillPosData(const std::vector<UiEventPoint> &pos)
{
	for (auto & c:pos)
	{
		auto & tempPos = m_mapViewPoint[c.Id];
		tempPos.posLast = tempPos.pos;
		tempPos.pos = c.uiPoint;
	}
}

void CHmsNavViewer::DistributeTouchMsg(bool bUpdateLayer)
{
	switch (m_eTouchType)
	{
	case NAV_TOUCH_TYPE::map_scale:
		DoTouchMapScale(bUpdateLayer);
		break;
	case NAV_TOUCH_TYPE::measure_distance:
		DoTouchMeasureDistance();
		break;
	case NAV_TOUCH_TYPE::none_used:
	default:
		break;
	}
}

void CHmsNavViewer::DoTouchMapScale(bool bUpdateLayer)
{
	if (m_mapViewPoint.size() > 1)
	{
		//move and scale
		auto item = m_mapViewPoint.begin();

		auto & posOne = item->second;
		item++;
		auto & posTwo = item->second;

		if (m_earthFrame)
		{
			//m_earthFrame->RotationEarth(posOne.posLast, posOne.pos);
			m_earthFrame->ScaleAndRotationEarth(posOne.posLast, posTwo.posLast, posOne.pos, posTwo.pos, bUpdateLayer);
		}
	}
	else if (m_mapViewPoint.size() == 1)
	{
		//just move
		auto & temp = m_mapViewPoint.begin()->second;
#if 0
		m_posLast = temp.posLast;
		OnMove(temp.pos);
#else
		if (m_earthFrame)
		{
			m_earthFrame->RotationEarth(temp.posLast, temp.pos);
		}

#endif
	}
}

void CHmsNavViewer::DoTouchMeasureDistance()
{
	if (m_mapViewPoint.size() >= 2)
	{
		if (m_frame2DInterface)
		{
			Vec2 pos1 = m_mapViewPoint.begin()->second.pos;
			Vec2 pos2 = (++m_mapViewPoint.begin())->second.pos;
			Vec3 earth1, earth2;
			if (m_frame2DInterface->Frame2DPointToEarthPoint(pos1, earth1)
				&& m_frame2DInterface->Frame2DPointToEarthPoint(pos2, earth2))
			{
				Vec2 lonlat1 = CHmsFrame2DInterface::EarthPointToLonLat(earth1);
				Vec2 lonlat2 = CHmsFrame2DInterface::EarthPointToLonLat(earth2);

				auto mapMeasure = m_frame2DInterface->GetMapMeasure();
				if (mapMeasure)
				{
					mapMeasure->MeasurePos(pos1, pos2, lonlat1, lonlat2);
				}
			}
		}
	}
}