#include "line.h"

line2d Line_CreateLine2d(pCVec2d start, pCVec2d end)
{
	line2d line;
	line.start = *start;
	line.end = *end;
	return line;
}

int Line_GetLengthLine2d(pline2d line)
{
	double len,x,y;
	x = line->end._v[0] - line->start._v[0];
	y = line->end._v[1] - line->start._v[1];
	len = sqrt(x*x+y*y);
	return len;
}

CVec2d Line_GetVectorLine2d(pline2d line)
{
	CVec2d v;
	double x,y;
	x = line->end._v[0] - line->start._v[0];
	y = line->end._v[1] - line->start._v[1];
	v = Vec_CreatVec2d(x,y);
	return v;
}

int Line_IsPointUpLine2d(pline2d line, pCVec2d point)
{
	CVec2d v = Line_GetVectorLine2d(line);
	return (Vec_DotProduct2d(&v,point) > 0.0);
}

double Line_DotProduce2d(pline2d line, pCVec2d point)
{
	CVec2d v = Line_GetVectorLine2d(line);
	return Vec_DotProduct2d(&v,point);
}
