#pragma once
#include "HmsMercatorEarthTile.h"
#include "HmsEarthTileNavImage.h"
#include "HmsEarthSkyBox.h"

#include "HmsGlobal.h"
#include <list>

#include "HmsFrame2DInterface.h"

class CHmsNavImageTileMgr : public HmsAviPf::CHmsNode
{
	friend class CHmsMercatorEarthTileMgr;
public:
	CHmsNavImageTileMgr()
	{
		m_pNavImageTile = CHmsEarthTileNavImage::Create();
		m_pNavImageTile->SetPosition3D(HmsAviPf::Vec3(0, 0, 0));

		auto p = CHmsGlobal::GetInstance()->GetNavImageTileMgrPointer();
		if (!p)
		{
			CHmsGlobal::GetInstance()->SetNavImageTileMgrPointer(this);
		}
	}
	~CHmsNavImageTileMgr(){}
	CREATE_FUNC(CHmsNavImageTileMgr);
	void SetNavImageTileInfoStu(const NavImageTileStu &stu){ m_pNavImageTile->InitTileInfo3DEarth(stu); }
private:
	HmsAviPf::RefPtr<CHmsEarthTileNavImage> m_pNavImageTile;
};


class CHmsMercatorEarthTileMgr : public HmsAviPf::CHmsNode
{
public:
	CHmsMercatorEarthTileMgr();
	~CHmsMercatorEarthTileMgr();

	CREATE_FUNC(CHmsMercatorEarthTileMgr);

	virtual bool Init() override;

	void UpdateShowRadian(float vertical, float horizontal);
	void SetScreenSize(const HmsAviPf::Size & size);

	void EarthClipping(HmsAviPf::Frustum * frustum);
	int SetSeeAbleRadian(float fRadian);

	void Visit(HmsAviPf::Renderer * renderer, const HmsAviPf::Mat4 & parentTransform, uint32_t parentFlags);

	void SetFrame2DRoot(CHmsFrame2DInterface *p){ m_frame2DInterface = p; }

	void ReloadEarthTile();

	void AddOther3DLayer(HmsAviPf::CHmsNode * node);

private:
	void LeftColToRight(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count);
	void RightColToLeft(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count);
	void TopRowToBottom(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count);
	void BottomRowToTop(std::list< std::list<CHmsMercatorEarthTile*> > &list, const int &count);

	void PushToList(CHmsMercatorEarthTile *pTile, std::list< std::list<CHmsMercatorEarthTile*> > &retList);
	void GetListFrontRow(std::list< std::list<CHmsMercatorEarthTile*> > &list, std::list< std::list<CHmsMercatorEarthTile*> > &retList);
	void GetListBackRow(std::list< std::list<CHmsMercatorEarthTile*> > &list, std::list< std::list<CHmsMercatorEarthTile*> > &retList);
	void GetListLeftCol(std::list< std::list<CHmsMercatorEarthTile*> > &list, std::list< std::list<CHmsMercatorEarthTile*> > &retList);
	void GetListRightCol(std::list< std::list<CHmsMercatorEarthTile*> > &list, std::list< std::list<CHmsMercatorEarthTile*> > &retList);

private:
	int						m_nLayer;							//级别，通过水平显示弧度，确定显示级别
	HmsAviPf::Size			m_sizeScreen;						//theScreenSize;
	float					m_fShowRadianVertical;				//垂直显示弧度
	float					m_fShowRadianHorizontal;			//水平显示弧度
	int						m_nMaxLayer;

	HmsAviPf::Vector<CHmsMercatorEarthTile*>			m_rootTile;		//渲染及管理的的根节点

	HmsAviPf::RefPtr<CHmsNavImageTileMgr>				m_pNavImageTileMgr;

	HmsAviPf::RefPtr<CHmsEarthSkyBoxMgr>				m_pSkyboxMgr;

	Vector<CHmsNode*>									m_vOther3DChild;			//the child node

	//==========
	CHmsFrame2DInterface								*m_frame2DInterface;
	//每个元素是一列
	std::list< std::list<CHmsMercatorEarthTile*> >		m_listLayers;
	int m_layer_hCount;
	int m_layer_vCount;
	int m_layer_bgGrade;
	Vec2 m_vLastCenter;
	int m_nLastLayer;

	Vec2 GetScreenCenterXY(int nLayer);
};
