#include "aabbox.h"
#include "GLHeader.h"


pAabbox Aabbox_Create()
{
    pAabbox box = (pAabbox)malloc(sizeof(Aabbox));
    if (!box)
    {
        printf("there's something wrong with alloc pool");
    }
    memset(box, 0, sizeof(Aabbox));
    return box;
}

pAabbox Aabbox_CreatValue(pCVec3d min, pCVec3d max)
{
    pAabbox box = (pAabbox)malloc(sizeof(Aabbox));
    if (!box)
    {
        printf("there's something wrong with alloc pool");
    }
    box->maxedge = *max;
    box->minedge = *min;

    box->inited = 1;

    return box;
}

void Aabbox_Destory(pAabbox box)
{
    if (box)
    {
        free(box);
    }
}

void Aabbox_AddInternalPoint(pAabbox box, pCVec3d point)
{
    int i;

    if (!box->inited)
    {
        box->maxedge = box->minedge = *point;
        box->inited = 1;
        return;
    }

    if (point->_v[1] > 45000.0)
    {
        int a = 0;
        a++;
    }

    for (i = 0; i < 3; i++) if (point->_v[i] > box->maxedge._v[i]) box->maxedge._v[i] = point->_v[i];
    for (i = 0; i < 3; i++) if (point->_v[i] < box->minedge._v[i]) box->minedge._v[i] = point->_v[i];
}

/*
    Edges are stored in this way:
    Hey, am I an ascii artist, or what? :) niko.
    /3--------/7
    / |       / |
    /  |      /  |
    1---------5   |
    |  /2- - -|- -6
    | /       |  /
    |/        | /
    0---------4/
    */

void Aabbox_GetEdges(pAabbox box, pCVec3d edges)
{
    CVec3d middle = Aabbox_GetCenter(box);
    CVec3d diag = Vec_Vec3dSubVec3d(&middle, &box->maxedge);
    //diag = Vec_Vec3dDivDouble(&diag, 2.0);

    Vec_Set3d(&edges[0], middle._v[0] + diag._v[0], middle._v[1] + diag._v[1], middle._v[2] + diag._v[2]);
    Vec_Set3d(&edges[1], middle._v[0] + diag._v[0], middle._v[1] - diag._v[1], middle._v[2] + diag._v[2]);
    Vec_Set3d(&edges[2], middle._v[0] + diag._v[0], middle._v[1] + diag._v[1], middle._v[2] - diag._v[2]);
    Vec_Set3d(&edges[3], middle._v[0] + diag._v[0], middle._v[1] - diag._v[1], middle._v[2] - diag._v[2]);
    Vec_Set3d(&edges[4], middle._v[0] - diag._v[0], middle._v[1] + diag._v[1], middle._v[2] + diag._v[2]);
    Vec_Set3d(&edges[5], middle._v[0] - diag._v[0], middle._v[1] - diag._v[1], middle._v[2] + diag._v[2]);
    Vec_Set3d(&edges[6], middle._v[0] - diag._v[0], middle._v[1] + diag._v[1], middle._v[2] - diag._v[2]);
    Vec_Set3d(&edges[7], middle._v[0] - diag._v[0], middle._v[1] - diag._v[1], middle._v[2] - diag._v[2]);
}

/*
    Edges are stored in this way:
    Hey, am I an ascii artist, or what? :) niko.
    /3--------/7
    / |       / |
    /  |      /  |
    1---------5   |
    |  /2- - -|- -6
    | /       |  /
    |/        | /
    0---------4/
    */

void Aabbox_PushVertexs(pAabbox box, std::vector<CVec3d>& v)
{
    CVec3d vertex[8];
    Aabbox_GetEdges(box, vertex);
    v.push_back(vertex[5]);
    v.push_back(vertex[1]);
    v.push_back(vertex[1]);
    v.push_back(vertex[0]);
    v.push_back(vertex[0]);
    v.push_back(vertex[4]);
    v.push_back(vertex[4]);
    v.push_back(vertex[5]);

    v.push_back(vertex[3]);
    v.push_back(vertex[2]);
    v.push_back(vertex[2]);
    v.push_back(vertex[6]);
    v.push_back(vertex[6]);
    v.push_back(vertex[7]);
    v.push_back(vertex[7]);
    v.push_back(vertex[3]);

    v.push_back(vertex[1]);
    v.push_back(vertex[3]);
    v.push_back(vertex[0]);
    v.push_back(vertex[2]);
    v.push_back(vertex[5]);
    v.push_back(vertex[7]);
    v.push_back(vertex[4]);
    v.push_back(vertex[6]);
}

CVec3d Aabbox_GetCenter(pAabbox box)
{
    CVec3d add = Vec_Vec3dAddVec3d(&box->minedge, &box->maxedge);
    return Vec_Vec3dDivDouble(&add, 2.0);
}

CVec3d Aabbox_GetExtent(pAabbox box)
{
    return Vec_Vec3dSubVec3d(&(box->maxedge), &(box->minedge));
}

int Aabbox_IsPointInside(pAabbox box, CVec3d point)
{
    return ((point._v[0] >= box->minedge._v[0] && point._v[1] >= box->minedge._v[1] && point._v[2] >= box->minedge._v[2]) &&
        (point._v[0] <= box->maxedge._v[0] && point._v[1] <= box->maxedge._v[1] && point._v[2] <= box->maxedge._v[2]));
}

int Aabbox_intersectBox(pAabbox box1, pAabbox box2)
{
    return Aabbox_IsPointInside(box1, box2->minedge) || Aabbox_IsPointInside(box1, box2->maxedge);
}

