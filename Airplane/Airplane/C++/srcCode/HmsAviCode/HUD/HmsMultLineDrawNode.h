#pragma once
#include "display/HmsDrawNode.h"
#include "render/HmsTexture2D.h"
#ifndef HMS_ARRAYSIZE
#define HMS_ARRAYSIZE(arr) (sizeof(arr)/sizeof((arr)[0]))
#endif

NS_HMS_BEGIN

class CHmsMultLineDrawNode : public CHmsNode
{
public:
	CHmsMultLineDrawNode();
	~CHmsMultLineDrawNode();

	CREATE_FUNC(CHmsMultLineDrawNode);

	virtual bool Init() override;

	virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;

	void onDraw(const Mat4 &transform, uint32_t flags);

	virtual void onDrawShape();

	void clear();

	const BlendFunc& getBlendFunc() const;

	void setBlendFunc(const BlendFunc &blendFunc);

	void SetDrawColor(const Color4F & color);

public:

	void DrawFan(const Vec2& center, float fAngleBegin, float fAngleEnd, float fRadius, unsigned int nSegments = 50, const Color4F & color = Color4F::WHITE);
	void DrawFan(float fAngleBegin, float fAngleEnd, float fRadius, unsigned int nSegments = 50, const Color4F & color = Color4F::WHITE);

	void DrawFanBorder(const Vec2& center, float fAngleBegin, float fAngleEnd, float fRadius, float fBorder = 2.0f, unsigned int nSegments = 50, const Color4F & color = Color4F::WHITE);
	void DrawFanBorder(float fAngleBegin, float fAngleEnd, float fRadius, float fBorder = 2.0f, unsigned int nSegments = 50, const Color4F & color = Color4F::WHITE);

	void DrawRing(const Vec2& center, float fRadius, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);
	void DrawRing(float fRadius, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);

	void DrawCicleByGLLine(const Vec2& center, float fRadius, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);
	void DrawCicleByGLLine(float fRadius, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);

	void DrawDisk(float fRadius, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);
	void DrawDisk(const Vec2& center, float fRadius, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);

	void DrawDashLine(const Vec2 & posStart, float fAngle, float fLineLength,  float fDotLength, float fBankLength, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);

	void DrawDashCicle(const Vec2& center, float fRadius, float fDashRate = 0.5, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);
	void DrawDashCicle(float fRadius, float fDashRate = 0.5, float fLineWidth = 2.0, unsigned int nSegment = 50, const Color4F & color = Color4F::WHITE);

	void DrawOutterRect(float x1, float y1, float x2, float y2, float fLineWidthH , float fLineWidthV , const Color4F & color = Color4F::WHITE);
	void DrawOutterRect(float x1, float y1, float x2, float y2, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);
	void DrawOutterRect(const Vec2 & pos1, const Vec2 & pos2, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);
	void DrawInnerRect(float x1, float y1, float x2, float y2, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);
	void DrawVerticalLine(float x, float y1, float y2, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);
	void DrawHorizontalLine(float x1, float x2, float y, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);

	void DrawSolidRect(float x1, float y1, float x2, float y2, const Color4F & color = Color4F::WHITE);
	void DrawSolidRect(const Rect & rect, const Color4F & color = Color4F::WHITE);


	void DrawGradualSolidRect(const Rect & rect, const std::vector<GradualCtrlPoint> & vCtrlPoint);


	void DrawRoundRect(float x1, float y1, float x2, float y2, float fRadius = 5.0f, unsigned int nSegment = 10, const Color4F & color = Color4F::WHITE);
	void DrawRoundRect(const Rect & rect, float fRadius = 5.0f, unsigned int nSegment = 10, const Color4F & color = Color4F::WHITE);
	void DrawInnerRoundRect(float x1, float y1, float x2, float y2, float fRadius = 5.0f, float fLineWidth = 2.0, unsigned int nSegment = 10, const Color4F & color = Color4F::WHITE);

	void DrawLineAngle(float fStart, float fEnd, float fAngle, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);
	void DrawLineAngle(const Vec2 & center, float fStart, float fEnd, float fAngle, float fLineWidth = 2.0, const Color4F & color = Color4F::WHITE);

	void DrawLineStrip(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLineStrip(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLineStrip(Vec2 * vertexs, int nCnt, float fLineWidth, float fStrokeWidth, const Color4B & color = Color4B::WHITE, const Color4B & colorStroke = Color4B::BLACK);
	void DrawLineLoop(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLineLoop(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLineLoop(Vec2 * vertexs, int nCnt, float fLineWidth, float fStrokeWidth, const Color4B & color = Color4B::WHITE, const Color4B & colorStroke = Color4B::BLACK);
	void DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLines(const std::vector<Vec2> & vPos, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, float fStrokeWidth, const Color4B & color = Color4B::WHITE, const Color4B & colorStroke = Color4B::BLACK);

	void DrawSolidRectWithSegment(float x1, float y1, float x2, float y2, int nSegment, const Color4F & color = Color4F::WHITE);
	void DrawTriangles(const std::vector<Vec2> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawTriangles(const Vec2 & center, const std::vector<Vec2> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawTriangleStrip(const std::vector<Vec2> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawTriangleStrip(const Vec2 & center, const std::vector<Vec2> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawTriangleFan(const std::vector<Vec2> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawTriangleFan(const Vec2 & center, const std::vector<Vec2> & vPos, const Color4F & color = Color4F::WHITE);
	void DrawQuad(const Vec2 & leftTop, const Vec2 & rightBottom, const Color4F & color = Color4F::WHITE);
	void DrawQuad(const Vec2 & center, const Vec2 & leftTop, const Vec2 & rightBottom, const Color4F & color = Color4F::WHITE);

	void DrawDashLines(const std::vector<Vec2> & vPos, float fLineWidth, float fSolidLength, float fDashLenght, const Color4B & color = Color4B::WHITE);

	void DrawArc(const Vec2& center, float fAngleBegin, float fAngleEnd, float fRadius, float fLineWidth = 2.0, unsigned int nSegments = 50, const Color4F & color = Color4F::WHITE);
	void DrawArc(float fAngleBegin, float fAngleEnd, float fRadius, float fLineWidth = 2.0, unsigned int nSegments = 50, const Color4F & color = Color4F::WHITE);

protected:
	void ensureCapacity(int count);
	V2F_C4B_T2F * GetCurrentPosBegin();
	void SetDrawTriangleStrip(int nVertexCount);
	void SetDrawLineLoop(int nVertexCount, float nLineWidth);
	void SetDrawPoints(int nVertexCount, float lineWidth);
	void SetDrawLines(int nVertexCount, float lineWidth);
	void SetDrawLineStrip(int nVertexCount, float lineWidth);
	void SetDrawTriangles(int nVertexCount);
	void SetDrawTriangleFan(int nVertexCount);
	
	int GetDrawIndex();
	void DrawIndex(int nIndex);

protected:

	GLuint								m_vao;
	GLuint								m_vbo;
	int									m_bufferCapacity;
	int									m_bufferCount;
	V2F_C4B_T2F		*					m_pBuffer;
	HmsAviPf::Texture2D *				m_pTexture;
	HmsAviPf::Color4F					m_color;
	GLuint								m_nColorIndex;

	BlendFunc							m_blendFunc;
	CustomCommand						m_customCommand;
protected:
	std::vector<HmsDrawCtrl>			m_vFanDraw;

	bool								m_bDirty;

private:
	HMS_DISALLOW_COPY_AND_ASSIGN(CHmsMultLineDrawNode);
};

NS_HMS_END