#pragma once
#include "base/HmsTypes.h"
#include "base/RefPtr.h"
#include "base/HmsNode.h"
#include "render/HmsCustomCommand.h"
#include "render/HmsMeshCommand.h"
#include "base/HmsAABB.h"
#include "base/HmsFrustum.h"
#include "base/HmsMap.h"

#include "HmsLoadTextureThread.h"
#include "HmsTextureFactory.h"


#include <thread>

enum class TitleDrawState
{
	drawSelf,
	drawChild,
	drawChildAndSelf,
	noDraw,
};

class CHmsMercatorEarthTile;

struct MercatorEarthTileInfo
{
	int		nLayer;
	int		nX;
	int		nY;

	MercatorEarthTileInfo();
	MercatorEarthTileInfo(int nLayer, int nX, int nY);
	MercatorEarthTileInfo(const MercatorEarthTileInfo & info, int nChildIndex);

	void SetInfo(int nLayer, int nX, int nY);
	MercatorEarthTileInfo GetChild(int nChildIndex);
};

class CHmsMercatorEarthTile : public HmsAviPf::CHmsNode
{
public:
	static double MercatorPosYToLatDegree(double dPosY);
	static double MercatorPosYToLatRadian(double dPosY);

	static CHmsMercatorEarthTile * Create(int nLayer, int nX, int nY);
	static CHmsMercatorEarthTile * Create(const MercatorEarthTileInfo & info);
	static void SetDrawMode(bool bDrawTexture);

	static void SetEarthTilePath(const std::string & strPath, const std::string & strSuffix = ".jpg");

	CHmsMercatorEarthTile();
	~CHmsMercatorEarthTile();

	virtual bool Init(int nLayer, int nX, int nY);

	virtual void Draw(HmsAviPf::Renderer *renderer, const HmsAviPf::Mat4& transform, uint32_t flags) override;

	void OnDraw(const HmsAviPf::Mat4 &transform, uint32_t flags);

	bool IsVisitableByFrustum(HmsAviPf::Frustum * frustum);

	virtual void UpdateDrawState(HmsAviPf::Frustum * frustum, int nLayer);

	virtual void Visit(HmsAviPf::Renderer * renderer, const HmsAviPf::Mat4 & parentTransform, uint32_t parentFlags) override;

	HmsAviPf::V3F_T2F * GetNorthWest();
	HmsAviPf::V3F_T2F * GetNorthEast();
	HmsAviPf::V3F_T2F * GetSouthWest();
	HmsAviPf::V3F_T2F * GetSouthEast();
	HmsAviPf::V3F_T2F * GetCentre();

	void InitTileInfo3DEarth(int nLayer, int nX, int nY);

	//the child index is 0,1,2,3
	CHmsMercatorEarthTile * GetOrCreateChildTile(int nChildIndex);
	//=======
	void CreateChildTile(int nChildIndex);
	bool IsTextureOk(){ return m_texture; }
	void TextureLoadCallback(HmsAviPf::Image *p, int nLayer, int nX, int nY);
	void SendLoadTextureRequest(int nLayer, int nX, int nY, bool bAlwaysLoad);
	void CheckTexture(HmsAviPf::Texture2D *pTexture, int nLayer, int nX, int nY);

	void ReloadTexture();

	static HmsAviPf::Texture2D * GetTileTexture(const std::string &path);
	//=======

protected:
	std::string GetTilePath(int nLayer, int nX, int nY);
	

	void InitTileInfoPlane(int nLayer, int nX, int nY);
	


	HmsAviPf::V3F_T2F	*											m_pData;
	GLuint															m_nVBO;
	bool															m_bDirty;
	HmsAviPf::CustomCommand											m_customCommand;
	HmsAviPf::MeshCommand											m_meshCommand;
	HmsAviPf::RefPtr<HmsAviPf::RenderState::StateBlock>				m_stateBlock;
	HmsAviPf::RefPtr<HmsAviPf::Texture2D>							m_texture;
	HmsAviPf::AABB													m_aabb;
	TitleDrawState													m_eDrawState;
	MercatorEarthTileInfo											m_tileInfo;
	float															m_fLatNorth;
	float															m_fLatSouth;
	float															m_fLongEast;
	float															m_fLongWest;

	HmsAviPf::Map<int, CHmsMercatorEarthTile*>						m_mapTileChild;

	static GLuint			s_nVEAB;
	static GLushort		*	s_pIndices;
	static GLuint			s_nVerticalsCnt;
	static GLuint			s_nIndicesCnt;
	static bool				s_bDrawTexture;
	static std::string		s_strTilePathFormat;



	enum class ChildState
	{
		Child_NO,
		Child_LOADING,
		Child_OK
	};
	std::vector<ChildState>											m_vecTileChildState;
	HmsAviPf::Image *m_pImage;
	bool m_bIsTextureRelease;

	friend class CHmsMercatorEarthTileMgr;

	int m_nLayer;
	int m_nX;
	int m_nY;
	bool m_bReload;
};

