#pragma once

#include "base/HmsNode.h"
#include "display/HmsImageNode.h"
#include "ui/HmsUiEventInterface.h"
#include "../HmsNavDataStruct.h"
#include <mutex>

using namespace HmsAviPf;
class CHmsFrame2DRoot;

enum class NavFrame2DLayerType
{
    NavFrame2D_Route = 0,
    NavFrame2D_Obs,
    NavFrame2D_AirspaceUser,
    NavFrame2D_AirspaceEdit,
    NavFrame2D_Airport,
    NavFrame2D_Airplane,
    NavFrame2D_ADSB,
    NavFrame2D_Admin,
    NavFrame2D_AirspaceDB,
    NavFrame2D_AftTrackLayer
};

class CHmsFrame2DLayerBase : public HmsAviPf::CHmsNode, public HmsAviPf::HmsUiEventInterface
{
public:
    virtual ~CHmsFrame2DLayerBase();
	
    virtual NavFrame2DLayerType GetLayerType()
    {
        return m_layerType;
    }
	virtual void SetNeedUpdate2DEles(bool b)
	{
		m_bNeedUpdate2DEles = b;
	}
	virtual bool GetNeedUpdate2DEles()
	{
		return m_bNeedUpdate2DEles;
	}
    virtual void SetFrame2DRoot(CHmsFrame2DRoot *p);

	virtual void SetEarthLayer(int n)
	{
		if (m_earthLayer != n)
		{
			m_earthLayer = n;
			SetNeedUpdate2DEles(true);
		}
    }
    virtual void SetLayerVisible(bool b)
    {
        this->SetVisible(b);
    }

	virtual void Update(float delta) override = 0;
	//主要用于当元素没有变化时，重新计算二维坐标
	virtual void Update2DElements() = 0;

protected:
    CHmsFrame2DLayerBase(CHmsNode *pNode, NavFrame2DLayerType type);

	virtual void SetEleChanged(bool b)
	{
		m_bEleChanged = b;
	}
	virtual bool GetEleChanged()
	{
		return m_bEleChanged;
	}
	virtual void SetNeedSync(bool b)
	{
		m_bNeedSync = b;
	}
	virtual bool GetNeedSync()
	{
		return m_bNeedSync;
	}

protected:
	bool m_bNeedUpdate2DEles;
	bool m_bEleChanged;
	bool m_bNeedSync;
	CHmsFrame2DRoot * m_pFrame2DRoot;
	int m_earthLayer;
	std::mutex m_syncMutex;
	bool m_bNeedUseSyncData;

private:
    NavFrame2DLayerType m_layerType;
};



