#pragma once
#include "base/HmsNode.h"
#include "display/HmsImageNode.h"
#include "base/HmsCamera.h"
#include "ui/HmsUiEventInterface.h"
#include "../HmsGlobal.h"
#include "base/HmsRay.h"

#include "HmsNavMathHelper.h"
#include "HmsMapMeasure.h"

using namespace HmsAviPf;

class CHmsFrame2DInterface : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
protected:
	CHmsFrame2DInterface(CHmsNode *pNode)
		:HmsUiEventInterface(pNode)
		, m_fNearPct(0)
		, m_bNeedUpdate2DEles(false)
#if _NAV_SVS_LOGIC_JUDGE
		, m_pCamera(nullptr)
#endif
	{

	}
public:
	virtual ~CHmsFrame2DInterface()
	{

	}

	static Vec2 EarthPointToLonLat(const Vec3 &pos)
	{
		return CHmsNavMathHelper::EarthPointToLonLat(pos);
	}
	static Vec3 LonLatToEarthPoint(const Vec2 &lonLat)
	{
		return CHmsNavMathHelper::LonLatToEarthPoint(lonLat);
	}
	static Vec2 NextPointLonLatByBearing(Vec2 curPos, float dis, float bearing)
	{
		return CHmsNavMathHelper::NextPointLonLatByBearing(curPos, dis, bearing);
	}
	static void InsertLonLat(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec2> &vecLonLatSerials, float dis)
	{
		CHmsNavMathHelper::InsertLonLat(lonLat1, lonLat2, vecLonLatSerials, dis);
	}
	static void InsertEarthPos(const Vec2 &lonLat1, const Vec2 &lonLat2, std::vector<HmsAviPf::Vec3> &vecEarthSerials, float dis)
	{
		CHmsNavMathHelper::InsertEarthPos(lonLat1, lonLat2, vecEarthSerials, dis);
	}

	virtual bool Frame2DPointToEarthPoint(const Vec2 &pos, Vec3 &dst)
	{
		Vec3 nearP(pos.x, pos.y, -1.0f), farP(pos.x, pos.y, 1.0f);
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pCamera)
			return false;
#endif
		nearP = m_pCamera->unproject(m_sizeContent, nearP);
		farP = m_pCamera->unproject(m_sizeContent, farP);
		Vec3 dir(farP - nearP);

		Ray ray(nearP, dir);
		Sphere sphere(Vec3(0, 0, 0), 1.0f);
		return ray.intersects(sphere, dst);
	}
	virtual Vec2 EarthPointToFramePoint(const Vec3 &pos)
	{
#if _NAV_SVS_LOGIC_JUDGE
		if (!m_pCamera)
			return Vec2(0,0);
#endif
		return m_pCamera->project(m_sizeContent, pos);
	}
    virtual Vec3 EarthPointToGLWorld(const Vec3 & pos)
    {
        if (m_pCamera)
        {
            return m_pCamera->projectToGLWorld(pos);
        }
        return Vec3();
    }
	virtual void Frame2DClipping(float fNearPct)
	{
		m_fNearPct = fNearPct;
	}
	virtual void SetNeedUpdate2DEles(bool b)
	{
		m_bNeedUpdate2DEles = b;
	}
	virtual bool Init(HmsAviPf::Camera *pCamera, Size layerSize)
	{
		if (!CHmsNode::Init())
		{
			return false;
		}
		this->SetContentSize(layerSize);

		m_pCamera = pCamera;

		return true;
	}

	virtual void Resize(const HmsAviPf::Size & size, int nLevel)
	{

	}

	virtual void OnClickOnEarth(const HmsAviPf::Vec2 & pos) = 0;
	virtual void Update2DElements() = 0;
	virtual void SetEarthLayer(int n) = 0;
	virtual CHmsMapMeasure* GetMapMeasure() = 0;

protected:
	HmsAviPf::Camera *m_pCamera;
	float m_fNearPct;
	bool m_bNeedUpdate2DEles;
};
