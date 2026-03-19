#pragma once

#include "../Frame2DLayer/HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "HUD/HmsMultLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"
#include "DataInterface/HmsAdminAreasServer.h"
#include "display/Hms3DLineDrawNode.h"
#include "display/Hms3DColorLineDrawNode.h"
#include "display/Hms3DTextureColorLineDrawNode.h"
#include "HmsStandardDef.h"

struct AdminAreaShape
{
	std::vector<HmsAviPf::Vec3> vecPosEarth;
	std::vector<HmsAviPf::Vec2> vecPos2D;
	std::vector<bool>			vecIsVisible;
};

struct AdminAreaGroupStu
{
	HmsAviPf::CHmsNode		* pAreaLabel;

	bool					isLableVisable;
	HmsAviPf::Vec3			posEarthLabel;
	HmsAviPf::Vec2			pos2DLabel;
	std::string				strNameLabel;

	std::vector<AdminAreaShape>  vecEarthShape;

	AdminAreaGroupStu()
	{
		pAreaLabel = nullptr;
#if _NAV_SVS_LOGIC_JUDGE
		isLableVisable = true;
#endif
	}

	bool EstimateGeoeCenter(HmsAviPf::Vec2 & posCenter);
};

struct AdminAreaDrawStu
{
	std::vector<std::vector<Vec2> > vecArrayVertexs;
	float fLineWidth;
	Color4B color;

	AdminAreaDrawStu()
	{
		fLineWidth = 0;
		color = Color4B::WHITE;
	}
};

#define USE_COLOR_TEXTRUE 0


class CHmsFrame2DAdminAreasLayer : public CHmsFrame2DLayerBase
{
public:
    CHmsFrame2DAdminAreasLayer();
    ~CHmsFrame2DAdminAreasLayer();

    static CHmsFrame2DAdminAreasLayer* Create(Size layerSize);
    virtual bool Init(Size layerSize);

    virtual void Update(float delta) override;
    virtual void Update2DElements() override;
    virtual void SetEarthLayer(int n) override;

	void Update3DLabel();
	void Update3DElements();

    void ShowAdminAreasLayer();
    void HideAdminAreasLayer();

    void UpdateChinaAdmin3D();

	virtual void SetVisible(bool bVisible) override;

	virtual void SetColor(const Color3B& color) override;

protected:
	void OnAmdinAreasDataLoaded(HmsAdminAreasServerDataCell * dataCell);

	void FillAdminAreaGroupData();

	void PushPointToStu(AdminAreaDrawStu &stu, const std::vector<Vec2> &vec2DPos, const std::vector<bool> &vecVisible);

private:
    void ReDrawLines();

	void CleanDataCell();

private:
    RefPtr<CHmsMultLineDrawNode>						m_pAdminAreaLineDrawNode;
	RefPtr<CHmsNode>									m_pRootLabelNode;
#if (USE_COLOR_TEXTRUE == 1)
	RefPtr<HmsAviPf::CHms3DTextureColorLineDrawNode>	m_pAdminAreaLine3DDrawNode;
#else
	RefPtr<HmsAviPf::CHms3DLineDrawNode>				m_pAdminAreaLine3DDrawNode;
    RefPtr<HmsAviPf::CHms3DLineDrawNode>                m_pAdminChinaAdmin3DDrawNode;
#endif

	std::vector<AdminAreaGroupStu>			m_vecAdminAreaGroup;
	std::vector<AdminAreaGroupStu>			m_vecAdminAreaGroupTemp;
    Vec2									m_lastLonLat;
	Vec2									m_lastOffset;
	float									m_fReloadDataScale;
    float									m_updateTime;
	HmsAdminAreasServerDataCell				m_dataCell;
	bool									m_bDraw3D;
	int										m_nLayerLimit;
};



