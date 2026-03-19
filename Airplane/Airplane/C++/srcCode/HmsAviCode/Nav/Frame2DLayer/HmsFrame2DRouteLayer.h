#pragma once

#include "HmsFrame2DLayerBase.h"
#include "../HmsRouteLineDrawNode.h"
#include "base/HmsActionTimeRef.h"
#include "base/HmsFrustum.h"

typedef std::function<void(WptNodeStu2D *pStu, int params)> t_RouteClickCallback;
typedef std::function<void()> t_RouteEleChangeCallback;

struct ClickDelayParamsStu
{
	WptNodeStu2D *pNodeStu2D;
	FPLNodeIndex index;
	Vec2 posOrigin;
	bool bExecuted;

	ClickDelayParamsStu()
	{
		Clear();
	}
	void Clear()
	{
		pNodeStu2D = nullptr;
		bExecuted = false;
	}
};

class CHmsFrame2DRouteLayer : public CHmsFrame2DLayerBase
{
	friend class CHmsFrame2DRoot;
public:
	CHmsFrame2DRouteLayer();
	~CHmsFrame2DRouteLayer();
	static CHmsFrame2DRouteLayer* Create(Size layerSize);
    static CHmsNode * CreateRouteNode(WptPointType type, std::string &strText, float textSize, FIX_TYPE fixType);
	virtual bool Init(Size layerSize);

	virtual void Update(float delta) override;
	virtual void Update2DElements() override;
	virtual void SetEarthLayer(int n) override;
	
	virtual bool OnPressed(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void OnMove(const HmsAviPf::Vec2 & posOrigin) override;
	virtual void OnReleased(const HmsAviPf::Vec2 & posOrigin) override;
#if 0
	void SendSyncData();
	void OnSyncDataUpdate(ESyncDataID dataID, char *data, int length);
	void UseSyncData();
#endif

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
	void ExpandOneProgram(FPLNodeIndex index);
	std::vector<FPLNodeIndex> FindSameProgram(std::string airport, FPL2DNodeType subType);
	void DeleteAirportProgram(std::string airport);
	bool InsertFPLNodeByInputRouteString(FPLNodeIndex index, std::string &str);
	bool DecodeInputRouteString(std::vector<WptNodeBrief> &vecRet, std::string strRoute);
	void DirectTo(FPLNodeIndex index);
	bool GetRouteInfoForSave(std::string &nameToSave, std::vector<int> &vecIndex);
	FPLNodeIndex ConvertIntIndexToFPLIndex(int index);

	void RegisterPressNodeShort(t_RouteClickCallback func);
	void RegisterPressNodeLong(t_RouteClickCallback func);
	void RegisterPressLineShort(t_RouteClickCallback func);
	void RegisterPressLineLong(t_RouteClickCallback func);
	void RegisterEleChangeCallback(t_RouteEleChangeCallback func);

	FrameEventState GetFrameEventState();
	const std::vector<FPL2DNodeInfoStu>& GetWptNodes();
	const WptNodeStu2D* GetWptNodeStu2D(FPLNodeIndex index);

private:
	void ReDrawLines();
	void DrawMoveCicle(const Vec2 & pos);
	//verticalPos 垂点   true点和向量在误差范围类相交
	bool DotInLine(const Vec2 & pos, const Vec2 & posA, const Vec2 & posB, Vec2 & verticalPos, float fError = 30);

	//只能输入主索引
	void CalInsertLonLat(FPLNodeIndex beginIndex, FPLNodeIndex endIndex);
	void ProcessInsertNormal(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust);
	void ProcessInsertHoldAt(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust);

	void ProcessInsertSpiral(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust);
	void ProcessInsertRectSpiral(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust);
	void ProcessInsertRect(WptNodeStu2D *pLastStu, WptNodeStu2D *pCurStu, HmsAviPf::Frustum *pFrust);

	bool TestPressOnOldNode(const HmsAviPf::Vec2 & posOrigin);
	bool TestPressOnLine(const HmsAviPf::Vec2 & posOrigin);
	void ProcessNodeEnableOrNot();
	
	void AddClickDelay();
	void CancelClickDelay();
	void PressClickDelay();

private:
	std::vector<FPL2DNodeInfoStu> m_vecWptNode;
	WptNodeStu2D *m_pCurDragNodeStu2D;
	FrameEventState m_frameEventState;
	WptNodeStu2D m_copyNode;
	RefPtr<CHmsRouteLineDrawNode> m_pRouteLineDrawNode;

	Vec2 m_posPress;
	Vec2 m_posImageWhenPress;
	FPLNodeIndex m_flySegment;
	
	CHmsActionDelayCallback *m_pClickDelayAction;
	bool m_bEnableClickDelay;
	ClickDelayParamsStu m_clickDelayParamsStu;

	std::vector<HistoryRouteOnePointStu> m_vecSyncStu;
	std::vector<FplSyncProgramAttrStu> m_vecSyncAttr;

    float m_routeNodeFontSize;

	t_RouteClickCallback m_funcPressNodeShort;
	t_RouteClickCallback m_funcPressNodeLong;
	t_RouteClickCallback m_funcPressLineShort;
	t_RouteClickCallback m_funcPressLineLong;
	t_RouteEleChangeCallback m_funcEleChangeCallback;
};



