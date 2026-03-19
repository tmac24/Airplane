#pragma once
#include "base/HmsTypes.h"
#include "base/RefPtr.h"
#include "base/HmsNode.h"
#include "render/HmsCustomCommand.h"
#include "render/HmsMeshCommand.h"
#include "base/HmsAABB.h"
#include "base/HmsFrustum.h"
#include "Calculate/HmsAlphaFilter.h"
#include "Svs/plane3d_.h"
#include "math/HmsGeometry.h"
#include "Svs/Coordinate.h"

typedef FILE esFile;

#ifdef __cplusplus
extern "C" {
#endif

#include "Svs/_Vec3.h"

#ifdef __cplusplus
}
#endif

class HmsSvsMgr : public HmsAviPf::CHmsNode
{
public:
	explicit HmsSvsMgr();
	virtual ~HmsSvsMgr();

	CREATE_FUNC(HmsSvsMgr);

	virtual bool Init() override;
	virtual void Draw(HmsAviPf::Renderer *renderer, const HmsAviPf::Mat4& transform, uint32_t flags) override;	
	virtual void Visit(HmsAviPf::Renderer * renderer, const HmsAviPf::Mat4 & parentTransform, uint32_t parentFlags) override;	
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float delta) override;
    

	void onDraw(const HmsAviPf::Mat4 &transform, uint32_t flags);

	static void SetSvsDataPath(const std::string & strPath);
    void SetSvsDrawSize(const HmsAviPf::Size& sz);
	void SetDyamicSize(const HmsAviPf::Size & sz);
private:
	//************************************
	// Method:    GpsPositonLinearInterpolation
	// FullName:  HmsSvsMgr::GpsPositonLinearInterpolation
	// Access:    private 
	// Desc:      
	// Returns:   CVec3d
	// Qualifier:
	// Parameter: const CVec3d & start
	// Parameter: const double & proportion
	// Parameter: const CVec3d & end
	// Author:    yan.jiang
	//************************************
	CVec3d GpsPositonLinearInterpolation(const CVec3d &start, const double& proportion, const CVec3d &end)
	{
		CVec3d interpolated;
		interpolated._v[0] = start._v[0] + proportion*(end._v[0] - start._v[0]);
		interpolated._v[1] = start._v[1] + proportion*(end._v[1] - start._v[1]);
		interpolated._v[2] = start._v[2] + proportion*(end._v[2] - start._v[2]);
		return interpolated;
	}

	HmsSvsMgr(const HmsSvsMgr&);
	HmsSvsMgr& operator=(const HmsSvsMgr&);
	
	HmsAviPf::CustomCommand											m_customCommand;	

	HmsAviPf::GLProgram* m_glprogram;
	HmsAviPf::GLProgram* m_glprogram2;
	HmsAviPf::GLProgram* m_glprogram3;
    HmsAviPf::GLProgram* m_glprogram4;
	HmsAviPf::GLProgram* m_glprogramHvWire;
    HmsAviPf::GLProgram* m_glprogramHvTower;
    HmsAviPf::GLProgram* m_glprogramAircraft;

	CVec3d m_vLLA1;	//lon lat alt
	CVec3d m_vLLA0;	//lon lat alt

	clock_t m_clocksTag = 0;
	const clock_t m_clocksPerPeriod = CLOCKS_PER_SEC / 4;//here the period is 0.25 second
	//const clock_t m_clocksPerPeriod = CLOCKS_PER_SEC / 2;//here the period is 0.25 second
	//const clock_t m_clocksPerPeriod = CLOCKS_PER_SEC;//Here simulate the true airplane GPS lon/lat/alt updating once per second.

	CHmsAlphaFilter   m_filterLatitude;
	CHmsAlphaFilter   m_filterLongitude;
	CHmsAlphaFilter   m_filterAltitude;
};

