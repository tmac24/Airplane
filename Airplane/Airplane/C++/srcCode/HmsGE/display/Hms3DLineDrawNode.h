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

class CHms3DLineDrawNode : public CHmsNode
{
public:
	static CHms3DLineDrawNode * Create(int nPreAssignCapacity = 128);

	CHms3DLineDrawNode();
	~CHms3DLineDrawNode();

	virtual bool Init(int nPreAssignCapacity);

	virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;

	void OnDraw(const Mat4 &transform, uint32_t flags);

	virtual void Visit(Renderer * renderer, const Mat4 & parentTransform, uint32_t parentFlags) override;

	void SetLineWidth(float fLineWidth);

	void SetDepthTest(bool bTest);

	void clear();

	void PrintUsedSize();

	void DrawLines(const std::vector<Vec3> & vPos);
	void DrawLineLoop(const std::vector<Vec3> & vPos);
	void DrawLineStrip(const std::vector<Vec3> & vPos);

	virtual void SetColor(const Color4F & color);
	virtual void SetColor(const Color3B& color) override;

	virtual void SetOpacity(GLubyte opacity) override;

protected:
	void ensureBufferCapacity(int count);
	void ensureIndicesCapacity(int count);

	Vec3 * GetCurrentVerticalPosBegin();
	GLuint * GetCurrentIndicesPosBegin();

	void UpdateUColor();
	void UpdatePointSize();
protected:
	//buffer
	Vec3	*														m_pBuffer;
	int																m_bufferCapacity;
	int																m_bufferCount;
	//indices
	GLuint		*													m_pIndices;
	int																m_indicesCapacity;
	int																m_indicesCount;

	GLuint															m_nVBO;
	GLuint															m_nVEAB;

	float															m_fLineWidth;
	bool															m_bDirty;

	CustomCommand													m_customCommand;
	RefPtr<RenderState::StateBlock>									m_stateBlock;

	Color4F															m_colorDraw;

    GLuint								                            m_nColorIndex;
    GLuint                                                          m_nPointIndex;
    GLuint                                                          m_nTrimDistance;
    Vec4                                                            m_uColor;
	//AABB															m_aabb;

};

NS_HMS_END