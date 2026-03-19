#pragma once

#include "display/HmsDrawNode.h"
#include "render/HmsTexture2D.h"
#include "../HmsGlobal.h"

NS_HMS_BEGIN

struct RouteLineDrawStu
{
	std::vector<std::vector<Vec2> > vecArrayVertexs;
	float fLineWidth;
	Color4B color;

	RouteLineDrawStu()
	{
		fLineWidth = 0;
		color = Color4B::WHITE;
	}
};

class CHmsRouteLineDrawNode : public CHmsDrawNode
{
public:
	CHmsRouteLineDrawNode();
	~CHmsRouteLineDrawNode();

	CREATE_FUNC(CHmsRouteLineDrawNode);

	virtual bool Init();

	virtual void Draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;

	void onDraw(const Mat4 &transform, uint32_t flags);

	void clear();

	const BlendFunc& getBlendFunc() const;

	void setBlendFunc(const BlendFunc &blendFunc);

	void drawLine(float fLength, float fLineWidth = 2.0);
	void drawLineAngle(float fStart, float fEnd, float fAngle, float fLineWidth = 2.0);
	void drawCicleFan(float fAngleStart, float fAngleEnd, float fRadius, float fLineWidth = 2.0, unsigned int nSegment = 10);

	void DrawLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLines(const std::vector<RouteLineDrawStu> &vecStu);

	void DrawDashLines(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawLinesPrimitive(Vec2 * vertexs, int nCnt, float fLineWidth, const Color4B & color = Color4B::WHITE);
	void DrawRing(float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/);
	void DrawRing(const Vec2& center, float fRadius, float fLineWidth /*= 2.0*/, unsigned int nSegment /*= 50*/, const Color4F & color /*= Color4F::WHITE*/);

	void SetColor(const Color4F & color);

	void SetTexturePath(std::string str);

protected:
	void ensureCapacity(int count);

private:

	GLuint								m_vao;
	GLuint								m_vbo;
	int									m_bufferCapacity;
	int									m_bufferCount;
	V2F_C4B_T2F *						m_pBuffer;
	Texture2D *							m_pTexture;
	Color4F								m_color;
	GLuint								m_nColorIndex;

	BlendFunc							m_blendFunc;
	CustomCommand						m_customCommand;

	std::vector<HmsDrawCtrl>			m_vFanDraw;

	bool								m_bDirty;

private:
	HMS_DISALLOW_COPY_AND_ASSIGN(CHmsRouteLineDrawNode);

	static float						s_fAngleOutside;	//最大角度
	static float						s_fAngleInside;		//中间角度
	static float						s_fDistanceNear;	//近距离
	static float						s_fDistanceFar;		//远距离
};

NS_HMS_END

