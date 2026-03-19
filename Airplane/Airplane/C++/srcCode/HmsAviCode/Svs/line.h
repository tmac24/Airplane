#ifndef _LINE_H_
#define _LINE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "_Vec2.h"

#ifdef __cplusplus
}
#endif



struct line2d
{
    CVec2d start;
    CVec2d end;
};
typedef line2d* pline2d;

line2d Line_CreateLine2d(pCVec2d start, pCVec2d end);

int Line_GetLengthLine2d(pline2d line);

CVec2d Line_GetVectorLine2d(pline2d line);

//点是否在线的上方 当线性方程kx1+b-y1>0时，（x1,y1）在线型方程的上方
int Line_IsPointUpLine2d(pline2d line, pCVec2d point);

double Line_DotProduce2d(pline2d line, pCVec2d point);

#endif