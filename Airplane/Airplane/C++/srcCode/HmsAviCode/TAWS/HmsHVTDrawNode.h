#pragma once
#include "base/HmsNode.h"
#include "HmsAviMacros.h"
#include "render/HmsCustomCommand.h"
#include "display/HmsDrawNode.h"
#include "base/RefPtr.h"
#include "NavSvs/mathd/Vec2d.h"
#include "render/HmsTexture2D.h"
#include "GLES2/gl2.h"

USING_NS_HMS;

class HmsHVTDrawNode : public CHmsDrawNode
{
public:
	HmsHVTDrawNode();
	~HmsHVTDrawNode();

	virtual bool Init() override;

	void SetTexturePath(std::string str);
	CREATE_FUNC(HmsHVTDrawNode);

	virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;

	void onDraw(const Mat4 &transform, uint32_t flags);

	//void DrawLineStrip(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color /* = Color4B::WHITE */);

	//void DrawLineStrip(Vec2 * verts, int nCount, float fLineWidth, float fStrokeWidth, const Color4B & color /*= Color4B::WHITE*/, const Color4B & colorStroke /*= Color4B::BLACK*/);
private:

	GLuint m_nColorIndex;
	Texture2D *	m_pTexture;
	Color4F m_color;
};

