#pragma once
#include "base/HmsTypes.h"
#include "base/RefPtr.h"
#include "base/HmsNode.h"
#include "render/HmsCustomCommand.h"
#include "render/HmsMeshCommand.h"
#include "base/HmsAABB.h"
#include "base/HmsFrustum.h"
#include "base/HmsMap.h"

NS_HMS_BEGIN

class CHms3DColorLineDrawNode : public CHmsNode
{
public:
	static CHms3DColorLineDrawNode * Create(int nPreAssignCapacity = 128);

	CHms3DColorLineDrawNode();
	~CHms3DColorLineDrawNode();

	virtual bool Init(int nPreAssignCapacity);

	virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;

	void OnDraw(const Mat4 &transform, uint32_t flags);

	virtual void Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags) override;

	void SetLineWidth(float fLineWidth);

	void SetDepthTest(bool bTest);

	void SetTexture(Texture2D * texture){ m_texture = texture; }

	void clear();

	void DrawLines(const std::vector<Vec3> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawLineLoop(const std::vector<Vec3> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawLineStrip(const std::vector<Vec3> & vPos, const Color4F & color = Color4F::WHITE);

protected:
	void ensureBufferCapacity(int count);
	void ensureIndicesCapacity(int count);

	V3F_C4B * GetCurrentVerticalPosBegin();
	GLuint * GetCurrentIndicesPosBegin();
protected:
	//buffer
	V3F_C4B		*								m_pBuffer;
	int											m_bufferCapacity;
	int											m_bufferCount;
	//indices
	GLuint		*								m_pIndices;
	int											m_indicesCapacity;
	int											m_indicesCount;

	GLuint										m_nVBO;
	GLuint										m_nVEAB;

	float										m_fLineWidth;
	bool										m_bDirty;

	CustomCommand								m_customCommand;
	RefPtr<RenderState::StateBlock>				m_stateBlock;
	RefPtr<Texture2D>							m_texture;

	//AABB										m_aabb;
};

NS_HMS_END