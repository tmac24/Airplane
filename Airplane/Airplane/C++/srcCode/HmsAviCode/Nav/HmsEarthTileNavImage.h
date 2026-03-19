#pragma once
#include "base/HmsTypes.h"
#include "base/RefPtr.h"
#include "base/HmsNode.h"
#include "render/HmsCustomCommand.h"
#include "render/HmsMeshCommand.h"
#include "base/HmsAABB.h"
#include "base/HmsFrustum.h"
#include "base/HmsMap.h"

#include "HmsMercatorEarthTile.h"

struct NavImageTileStu
{
	HmsAviPf::Vec2 lonLatLT;
	HmsAviPf::Vec2 lonLatRB;
	HmsAviPf::Vec2 pixelLT;
	HmsAviPf::Vec2 pixelRB;
	std::string imagePath;
};

class CHmsEarthTileNavImage : public CHmsMercatorEarthTile
{
public:
	static CHmsEarthTileNavImage * Create();

	CHmsEarthTileNavImage();

	virtual bool Init() override;

	virtual void Draw(HmsAviPf::Renderer *renderer, const HmsAviPf::Mat4& transform, uint32_t flags) override;

	virtual void UpdateDrawState(HmsAviPf::Frustum * frustum, int nLayer) override;

	void InitTileInfo3DEarth(const NavImageTileStu &tileStu);

private:

	NavImageTileStu m_tileStu;
	const int c_nNumRows = 17;
	const int c_nNumCols = 17;
};

