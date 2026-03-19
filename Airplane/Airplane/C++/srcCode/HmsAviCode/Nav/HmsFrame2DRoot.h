#pragma once

#include "HmsWptInfoDlg.h"
#include "HmsMapMeasure.h"
#include "HmsRouteNodeWin.h"
#include "HmsNavInfoWindow.h"
#include "ui/HmsKeyboard.h"
#include "FMS/HmsDuControlData.h"
#include "HmsFrame2DInterface.h"
#include "Procedure/HmsProcedureDlg.h"
#include "HmsHistoryRouteDlg.h"
#include "HmsHistoryRouteInputName.h"
#include "HmsFrame2DToolbox.h"
#include "HmsInputAltitude.h"
#include "HmsSearchShape.h"
#include "HmsEditAirspace.h"
#include <time.h>
#include "HmsUserWptEditDlg.h"
#include "DataInterface/HmsDataBus.h"
#include "HmsNavComm.h"

#include "Frame2DLayer/HmsFrame2DAirplaneLayer.h"
#include "Frame2DLayer/HmsFrame2DAirspaceLayer.h"
#include "Frame2DLayer/HmsFrame2DObsLayer.h"
#include "Frame2DLayer/HmsFrame2DRouteLayer.h"
#include "Frame2DLayer/HmsFrame2DAirspaceEditLayer.h"
#include "Frame2DLayer/HmsFrame2DAdminAreasLayer.h"
#include "AirSpaceData/HmsFrame2DAirspaceDBLayer.h"
#include "Frame2DLayer/HmsFrame2DAirportLayer.h"
#include "Frame2DLayer/HmsFrame2DAftTrackLayer.h"
#include "Frame2DLayer/HmsFrame2DADSBLayer.h"

class CHmsFrame2DRoot : public CHmsFrame2DInterface
{
public:
	CHmsFrame2DRoot();
	~CHmsFrame2DRoot();

	virtual void OnClickOnEarth(const HmsAviPf::Vec2 & pos) override;
	virtual void Update2DElements() override;
	virtual void SetEarthLayer(int n) override;
	virtual CHmsMapMeasure* GetMapMeasure() override;
	virtual void SetNeedUpdate2DEles(bool b) override;
	virtual void Resize(const HmsAviPf::Size & size, int nLevel) override;

	static CHmsFrame2DRoot* Create(HmsAviPf::Camera *pCamera, Size layerSize);
	virtual bool Init(HmsAviPf::Camera *pCamera, Size layerSize) override;
	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void Update(float delta) override;

	void CallbackAdd(WptPointType t, float lon, float lat, std::string ident, int params);
	void CallbackCancel(int params);
	void CallbackDelete(int params);

	void Reverse();
	void ClearAll();
	void InsertWptNode(WptPointType t, float lon, float lat, std::string ident, FPLNodeIndex index, bool bExpand = true);
	void InsertWptNode(WptNodeBrief brief, FPLNodeIndex index, bool bExpand = true);
	void DeleteWptNode(FPLNodeIndex index, bool bExpand = true, bool bDetectAirport = true);
	
	void InsertProgramInfo(FPL2DNodeInfoStu &info, FPLNodeIndex curStuIndex);
	FPLNodeIndex FindNodeIndex(WptNodeStu2D *stu);
	std::vector<FPLNodeIndex> FindSameProgram(std::string airport, FPL2DNodeType subType);
	bool InsertFPLNodeByInputRouteString(FPLNodeIndex index);
	void DirectTo(FPLNodeIndex index);
	void ShowProcedureDlg();
	void SaveRoute();
	void OpenHistoryRouteDlg();
	void InsertProgram(std::string airport, std::string program, std::string trans, std::string runway, FPL2DNodeType type);
	FPLNodeIndex ConvertIntIndexToFPLIndex(int index);
	void ShowInputAltitude(FPLNodeIndex index);
	void ShowSearchShape(FPLNodeIndex index);
	void ShowAirspace(bool bShow);
	bool AirspaceIsVisiable();
	void ShowEditAirspaceDlg();
	Frustum* GetFrustum();
	const WptNodeStu2D* GetWptNodeStu2D(FPLNodeIndex index);
	FPLNodeIndex GetFlySegment();
	Vec2 GetPlaneLonLat();
	Vec3 GetPlaneEarthPos();
	bool IsPlaneLonLatValid();
	float GetPlaneYaw();
	void OnNodePressShort(WptNodeStu2D *pStu, int params);
	void OnNodeOrLinePressLong(WptNodeStu2D *pStu, int params);
	void ShowUserWptEditDlg();
	void ShowOrHideDragButton(bool bShow, std::function<void(float, float)> funcOnRelease);
	void ShowAirspaceEditLayer(const std::vector<AirspaceEditStu> &vec);
	void HideAirspaceEditLayer();
	void UpdateAirspaceEditData(const std::vector<AirspaceEditStu> &vecStu);
	void Set2DLayerDisplayByName(const std::string & strLayerName, bool bShow);
	bool Is2DLayerDisplay(const std::string & strLayerName);
	CHmsNode * Get2DLayerByName(const std::string & strLayerName);

    CHmsFrame2DAirspaceDBLayer* GetAirspaceDBLayer();
    CHmsFrame2DAirportLayer* GetAirportLayer();
    CHmsFrame2DLayerBase* GetFrame2DLayer(NavFrame2DLayerType type);

private:
	Vec2 GetShowDlgPos(const HmsAviPf::Vec2 & posOrigin);
	void OnRouteEleChanged();
	void InitNameNode();

private:
	RefPtr<CHmsWptInfoDlg> m_pWptInfoDlg;
	int m_earthLayer;

	FPLNodeIndex m_flySegment;
	Vec2 m_planeLonLat;
	bool m_bPlaneLonLatValid;
	Vec3 m_planeEarthPos;
	float m_planeYaw;

	RefPtr<CHmsMapMeasure> m_mapMeasure;

	RefPtr<CHmsFrame2DToolbox> m_pToolbox;

	std::vector<CHmsFrame2DLayerBase*>			m_vecChildLayer;
	RefPtr<CHmsFrame2DRouteLayer>				m_pRouteLayer;
	RefPtr<CHmsFrame2DAirspaceLayer>			m_pAirspaceLayer;
	RefPtr<CHmsFrame2DAirspaceEditLayer>		m_pAirspaceEditLayer;
    RefPtr<CHmsFrame2DAirspaceDBLayer>			m_pAirspaceDBLayer;
	RefPtr<CHmsFrame2DAdminAreasLayer>			m_pAdminAreasLayer;
    RefPtr<CHmsFrame2DAirportLayer>             m_pAirportLayer;
    RefPtr<CHmsFrame2DAftTrackLayer>            m_pAftTrackLayer;

	RefPtr<CHmsUiStretchButton>					m_pDragButtonLonLat;
	std::function<void(float, float)>			m_funcDragButtonRelease;
	bool m_bIsPressOnDragButton;
	Vec2 m_dragButtonInitPos;

	Map<std::string, CHmsNode*>					m_map2dLayerNameNode;

    friend class CHmsNavFplLayer;
};

