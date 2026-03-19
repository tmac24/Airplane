#pragma once
#include "base/HmsTypes.h"

NS_HMS_BEGIN

static Color4B Color4F24B(const Color4F &color)
{
	return Color4B(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
}

#define HMS_PI   3.14159265358979323846f
#define HMS_2PI  (HMS_PI*2)	 
#define HMS_PI_2 (HMS_PI*0.5)
#define HMS_PI_4 (HMS_PI*0.25)
#define ONETENTH (0.003921568627f)
#define MAX_LINE_WIDTH     (6.0f)

#define ANGLE_TO_RADIAN(_angle_)  (float(_angle_) * HMS_PI / 180.0f)
#define RADIAN_TO_ANGLE(_radian_) (float(_radian_) * 180.0f / HMS_PI)
#define SIN_ANGLE(_angle_) (sin(ANGLE_TO_RADIAN(fAngle)))
#define COS_ANGLE(_angle_) (cos(ANGLE_TO_RADIAN(fAngle)))
#define COLOR4F_4B(_color) Color4F24B(_color)

struct HmsDrawCtrl
{
	GLenum	drawType;
	GLuint	nStartPos;
	GLuint	nDrawCnt;
	GLfloat fLineWidth;
	Color4F fColor;

	void SetPoints(GLuint nStart, GLuint nCnt, GLfloat lineWidth = 2.0)
	{
		drawType = GL_POINTS;
		nStartPos = nStart;
		nDrawCnt = nCnt;
		fLineWidth = lineWidth;
	}

	void SetLines(GLuint nStart, GLuint nCnt, GLfloat lineWidth = 2.0)
	{
		drawType = GL_LINES;
		nStartPos = nStart;
		nDrawCnt = nCnt;
		fLineWidth = lineWidth;
	}

	void SetLineLoop(GLuint nStart, GLuint nCnt, GLfloat lineWidth = 2.0)
	{
		drawType = GL_LINE_LOOP;
		nStartPos = nStart;
		nDrawCnt = nCnt;
		fLineWidth = lineWidth;
	}

	void SetLineStrip(GLuint nStart, GLuint nCnt, GLfloat lineWidth = 2.0)
	{
		drawType = GL_LINE_STRIP;
		nStartPos = nStart;
		nDrawCnt = nCnt;
		fLineWidth = lineWidth;
	}

	void SetTriangles(GLuint nStart, GLuint nCnt)
	{
		drawType = GL_TRIANGLES;
		nStartPos = nStart;
		nDrawCnt = nCnt;
		fLineWidth = 2.0;
	}

	void SetTriangleStrip(GLuint nStart, GLuint nCnt)
	{
		drawType = GL_TRIANGLE_STRIP;
		nStartPos = nStart;
		nDrawCnt = nCnt;
		fLineWidth = 2.0;
	}

	void SetTriangleFan(GLuint nStart, GLuint nCnt)
	{
		drawType = GL_TRIANGLE_FAN;
		nStartPos = nStart;
		nDrawCnt = nCnt;
		fLineWidth = 2.0;
	}

	void SetColor(const Color4F &color)
	{
		fColor = color;
	}
};

struct GradualCtrlPoint
{
	float		fPosProportion;		//the position of the ctrl point
	Color4B		color;
};

NS_HMS_END