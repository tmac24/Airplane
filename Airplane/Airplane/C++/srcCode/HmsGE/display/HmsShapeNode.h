#pragma once
#include "HmsDrawNode.h"
#include "HmsPainterPath.h"
#include <map>

NS_HMS_BEGIN

struct ShapeDrawInfo
{
	Rect		rect;
	Color4B		color;
	int			nRectIndex;

	ShapeDrawInfo()
	{
		nRectIndex = -1;
	}
};

class CHmsShapeNode : public CHmsDrawNode
{
public:
	CHmsShapeNode();
	~CHmsShapeNode();

	CREATE_FUNC(CHmsShapeNode);

	void DrawShape(const std::vector<Vec2> & vShape, const Color4B & color = Color4B::WHITE);

	void DrawPainterPath(const CHmsPainterPath & path, float fLineWidth, const Color4B & color = Color4B::WHITE);

	void FillPainterPath(const CHmsPainterPath & path, const Color4B & color = Color4B::GREEN);

	void DrawAndFillPainterPath(const CHmsPainterPath & path, float fLineWidth, const Color4B & color = Color4B::WHITE, const Color4B & colorFill = Color4B::GREEN);

protected:
	void OnStencilBegin();
	void OnStencilEnd();
	void drawFullScreenQuadClearStencil();
	void drawFullScreenQuadFillShape();

	int OnPrintShapeIndex(int nShapeIndex);

	virtual void onDrawShape();

private:
	GLboolean _currentStencilEnabled;
	GLuint _currentStencilWriteMask;
	GLenum _currentStencilFunc;
	GLint _currentStencilRef;
	GLuint _currentStencilValueMask;
	GLenum _currentStencilFail;
	GLenum _currentStencilPassDepthFail;
	GLenum _currentStencilPassDepthPass;
	GLboolean _currentDepthWriteMask;

	GLboolean _currentAlphaTestEnabled;
	GLenum _currentAlphaTestFunc;
	GLclampf _currentAlphaTestRef;

	GLint		_mask_layer_le;
	GLuint		_mask_layer;

private:
	std::map<int, ShapeDrawInfo>		m_mapShapeArea;

};

NS_HMS_END