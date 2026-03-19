#pragma once
#include "base/RefPtr.h"
#include "base/HmsNode.h"
#include "base/HmsCamera.h"
#include "base/HmsTimer.h"
#include "render/HmsTexture2D.h"
#include "display/HmsImageNode.h"
#include "render/HmsFrameBuffer.h"
#include "ui/HmsUiEventInterface.h"
#include "HmsMercatorEarthTileMgr.h"
#include "../Calculate/CalculateZs.h"
#include "base/HmsActionTimeRef.h"

#include "HmsFrame2DInterface.h"


class CHmsNavEarthFrame;

enum class NAV_TOUCH_TYPE
{
	none_used,			//鏈娇鐢?
	measure_distance,	//涓ょ偣娴嬭窛
	map_scale,			//鍦板浘缂╂斁
};

struct NavViewPoint
{
	int					nID;
	HmsAviPf::Vec2		pos;
	HmsAviPf::Vec2		posLast;		
};

class CHmsNavViewer : public HmsAviPf::CHmsImageNode, public HmsAviPf::HmsUiEventInterface
{
public:
	CHmsNavViewer(CHmsNavEarthFrame * frame);

	static CHmsNavViewer * Create(HmsAviPf::Texture2D *texture, CHmsNavEarthFrame * frame);

	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin);

	virtual void OnMove(const HmsAviPf::Vec2 & posOrigin);

	virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin);

	void SetFrame2DRoot(CHmsFrame2DInterface *pFrame){ m_frame2DInterface = pFrame; }

	virtual bool OnMutiPressed(const std::vector<UiEventPoint> &pos);

	virtual void OnMutiMove(const std::vector<UiEventPoint> &pos);

	virtual void OnMutiReleased(const std::vector<UiEventPoint> &pos);

	void MutiTouchDelay();

	void AddClickDelay(HmsAviPf::Vec2 pos);
	void CancelClickDelay();
	void PressClickDelay(float x, float y);
	CHmsActionDelayCallback *m_pClickDelayAction;
	bool m_bEnableClickDelay;

protected:
	bool CheckAndChangeTouchType(NAV_TOUCH_TYPE eType, bool bMeasurecloseDelay = false);
	void FillPosData(const std::vector<UiEventPoint> &pos);
	void DistributeTouchMsg(bool bUpdateLayer);
	void DoTouchMapScale(bool bUpdateLayer);
	void DoTouchMeasureDistance();

private:
	HmsAviPf::Vec2							m_posLast;
	HmsAviPf::Vec2							m_posBegin;
	HmsAviPf::RefPtr<CHmsNavEarthFrame>		m_earthFrame;
	CHmsFrame2DInterface					*m_frame2DInterface;
	bool									m_bPointInFrame2D;
	bool									m_bTouchDistribute;				//鏄惁鍒嗛厤touch浜嬩欢
	bool									m_bNeedCloseMesure;				//闇€瑕佸叧闂祴閲?
	NAV_TOUCH_TYPE							m_eTouchType;
	std::map<int,NavViewPoint>				m_mapViewPoint;

	CHmsTime m_time;
};

class CHmsNavEarthFrame : public HmsAviPf::CHmsNode
{
public:
	CHmsNavEarthFrame();
	~CHmsNavEarthFrame();

	static CHmsNavEarthFrame * Create(const HmsAviPf::Size & size);

	//get the texture of the frame
	HmsAviPf::Texture2D * GetTexture();

	//create a image frame node to show the frame
	HmsAviPf::CHmsImageNode * CreateImageFrameNode();

	CHmsNavViewer * CreateNavViewer();

	void Resize(const HmsAviPf::Size & size, int nLevel, CHmsNavViewer * viewer);

	void AddEarthNode(HmsAviPf::CHmsNode * node);
	void AttachToEarthDrawNode(HmsAviPf::CHmsNode * node);

	void TestEarth();

	void MoveCameraToMinDis();
	void MoveCameraToMaxDis();
	void MoveNear();
	void MoveFar();
	void RotationEarth(const HmsAviPf::Vec2 & delta);
	void RotationEarth(const HmsAviPf::Vec2 & begin, const HmsAviPf::Vec2 & end, bool bUpdateLayer = true);
	void ScaleAndRotationEarth(const HmsAviPf::Vec2 & begin1, const HmsAviPf::Vec2 & begin2, const HmsAviPf::Vec2 & end1, const HmsAviPf::Vec2 & end2, bool bUpdateLayer = true);
	void ReloadEarthTile();

	void OnClickOnEarth(const HmsAviPf::Vec2 & pos);

	float GetEarthShowRadius(bool bVertical = true);
	float GetEarthShowRadiusDegree(bool bVertical = true);

	void SetViewToAft();
	void SetViewCenterByLonLat(float lon, float lat);
	void SetCurLayerToTen();

	virtual void OnEnter() override;
	virtual void OnExit() override;

	virtual void Update(float delta) override;

	void SetEnableInertia(bool b);
	void SetInertiaAxisAndAgnle(Vec3 axis, float angle);
	void GetInertiaAxisAndAgnle(Vec3 &axis, float &angle);

	void SetFrame2DRoot(CHmsFrame2DInterface *pFrame);
	Camera * GetCamera();

	void SetPlaneAlwaysCenter(bool b);
	void SetPlaneAlwaysCenterCallback(std::function<void(bool b)> func);

protected:
	bool Init(const HmsAviPf::Size & size);

	void UpdateCameraPosition(bool bUpdateLayer = true);

	/*
	project an x,y pair onto a sphere of radius r or a
	hyperbolic sheet if we are away from the center of the sphere.
	*/
	float ProjectToSphere(float r, float x, float y);
	void UpdateFieldOfView();
	float GetViewsScale();

    void Update3DClipZBufer();

private:
	HmsAviPf::RefPtr<HmsAviPf::FrameBuffer>			m_frameBuffer;
	HmsAviPf::RefPtr<HmsAviPf::Camera>				m_camera;
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>			m_frameRoot;
	HmsAviPf::RefPtr<CHmsMercatorEarthTileMgr>		m_mgrMercatorEarthTile;
	uint32_t										m_nCameraFlag;

	HmsAviPf::Vec3									m_posEye;			//鐪肩潧鐨勪綅缃?
	HmsAviPf::Vec3									m_posEyeUp;			//鐪肩潧鍚戜笂鐨勪綅缃?
	HmsAviPf::Vec3									m_posTraget;		//鐪嬬殑鐐?
	float											m_fNear;
	float											m_fFar;
	float											m_fCameraNear;
	float											m_fCameraFar;


	float											m_fFieldOfView;
	float											m_fFieldOfViewRadian;
	float											m_fSeeAbleRadian;
	float											m_fCameraViewDistanceCut;
	float											m_fAspectRatio;
	float											m_fDistanceEye2Traget;

	CHmsFrame2DInterface							*m_frame2DInterface;

	//std::map<float, HmsAviPf::RefPtr<HmsAviPf::FrameBuffer>>  m_mapTempFrameBuffer;

	bool m_bEnableInertia;
	Vec3 m_inertiaAxis;
	float m_inertiaAngle;

	bool m_bPlaneAlwaysCenter;
	std::function<void(bool b)> m_funcPlaneAlwaysCenterCallback;
	HmsAviPf::Size									m_sizeInit;
	float											m_fFieldOfViewInit;
	float											m_fFieldOfViewRadianInit;
};