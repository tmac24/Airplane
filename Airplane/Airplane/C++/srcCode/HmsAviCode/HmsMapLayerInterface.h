#pragma once
#include "HmsGlobal.h"
#include "SimEarthCoord.h"
#include "HmsMapDef.h"

class CHmsMapLayerInterface : public HmsAviPf::CHmsNode
{
public:
	CHmsMapLayerInterface();
	virtual ~CHmsMapLayerInterface();

	CREATE_FUNC(CHmsMapLayerInterface);

	virtual bool Init() override;

	HmsAviPf::Vec2 GetGooglePosition(double dX, double dY);
	HmsAviPf::Vec2 GetGooglePosition(const CSimEarthCoord & posEc);
	HmsAviPf::Vec2 GetWGSPositionByPix(double dPixX, double dPixY);

	void SetLayer(int nLayer){ m_nLayer = nLayer; }
	int GetLayer(){ return m_nLayer; }

	void SetScale(float fScale){ m_fScale = fScale; }
	float GetScale(){ return m_fScale; }

	void SetLayerAndScaled(int nLayer, float fLayerScale);

	void SetCurrentAftData(const HmsAviPf::Vec2 & pos, float fHeading);

	HmsAviPf::Vec2 GetPosCenter(){ return m_posCenter; }

	//¶¯Ì¬¸üÐÂÍ¼²ã 
	virtual void UpdateLayer();

	virtual void OnLayerPosition(const HmsAviPf::Vec2 & posLayer, float fScale){};

	void AddMapLayerChild(HmsAviPf::CHmsNode * pNode);

	float GetHeading(){ return m_fCurHeading; }

	void SetMapDescribe(HmsMapDescribe	* pMapDes){ m_pMapDescribe = pMapDes; }

	CSimEarthCoord GetLngLatByOffset(const HmsAviPf::Vec2 & posCenter, const HmsAviPf::Vec2 & deltaScreen);
	CSimEarthCoord GetLngLatByOffsetNotHeading(const HmsAviPf::Vec2 & posCenter, const HmsAviPf::Vec2 & deltaScreen);

	CSimEarthCoord GetEarthCoordByTouch(const HmsAviPf::Touch * touch);

	//get the current position by layer scale
	HmsAviPf::Vec2 GetCurDrawPosition(const CSimEarthCoord & ec);

protected:
	HmsAviPf::RefPtr<HmsAviPf::CHmsNode>		m_root;
	int									m_nLayer;
	float								m_fScale;
	float								m_fCurHeading;
	HmsAviPf::Vec2						m_posCenter;
	HmsMapDescribe	*					m_pMapDescribe;
};

