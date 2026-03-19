#pragma once
#include <HmsAviPf.h>
#include "display/HmsDrawNode.h"

class CHmsVSDByShader : public HmsAviPf::CHmsNode
{
public:
	CHmsVSDByShader();
	~CHmsVSDByShader();

	CREATE_FUNC(CHmsVSDByShader);

	virtual bool Init() override;

	virtual void Draw(HmsAviPf::Renderer *renderer, const HmsAviPf::Mat4& transform, uint32_t flags) override;
 
	void onDraw(const HmsAviPf::Mat4 &transform, uint32_t flags);

	virtual void onDrawShape();

	void clear();

	const HmsAviPf::BlendFunc& getBlendFunc() const;

	void setBlendFunc(const HmsAviPf::BlendFunc &blendFunc);

	void SetTexture(HmsAviPf::Texture2D * texture, const HmsAviPf::Vec2 & mapCenter, const HmsAviPf::Rect & mapRect, const HmsAviPf::Vec2 & mapDstOffset, float fLonLatOffset);

	void SetLevelPosAndHeight(float posY, float height);

	void SetFlightData(const HmsAviPf::Vec2 & wgcPos, float fCurAlt, float fCurHeading);

	void DrawSolidRectWithSegment(float x1, float y1, float x2, float y2, int nSegment, const HmsAviPf::Color4F & color = HmsAviPf::Color4F::WHITE);

	void SetZeroStartPos(float fPos);

	void SetCurMapRange(float fRange);

protected:
	void ensureCapacity(int count);
	HmsAviPf::V2F_C4B_T2F * GetCurrentPosBegin();
	void SetDrawTriangleStrip(int nVertexCount);
	void SetDrawQuadStrip(int nVertexCount);
	void SetDrawQuads(int nVertexCount);
	/*获取刚刚插入的HmsDrawCtrl的索引*/
	int GetDrawIndex();
	void DrawIndex(int nIndex);

private:
	void UpdateRangeRelation();

private:

	GLuint					m_vao;
	GLuint					m_vbo;
	int						m_bufferCapacity;
	int						m_bufferCount;
	HmsAviPf::V2F_C4B_T2F		*		m_pBuffer;

	HmsAviPf::BlendFunc				m_blendFunc;
	HmsAviPf::CustomCommand			m_customCommand;

	std::vector<HmsAviPf::HmsDrawCtrl>			m_vFanDraw;

	bool							m_bDirty;

	int								m_nTextureUniform;
	int								m_nCenterPosUniform;
	int								m_nHeadingSinCosUniform;
	int								m_nShowRangeUniform;
	int								m_nMapRangeInPicUniform;
	int								m_nLevelInGLPosUniform;
	int								m_nShowRangeZeroPosUniform;
	int								m_nCurAltUniform;

	HmsAviPf::Vec2					m_vCenterPos;
	HmsAviPf::Vec2					m_vHeadingSinCos;
	HmsAviPf::Vec2					m_vShowRange;
	HmsAviPf::Vec2					m_vMapPicRange;			//0-1
	HmsAviPf::Vec2					m_vLevelPosAndHeight;
	float							m_fCurAlt;
	float							m_fZeroPosStart;

	HmsAviPf::RefPtr<HmsAviPf::Texture2D>			m_textureImage;

	HmsAviPf::Rect					m_mapRect;
	HmsAviPf::Vec2					m_mapCenter;
	HmsAviPf::Vec2					m_curCenter;
	HmsAviPf::Vec2					m_mapDstOffset;
	float							m_fLonLatOffset;
	float							m_fCurMapRange;

private:
	HMS_DISALLOW_COPY_AND_ASSIGN(CHmsVSDByShader);
};

