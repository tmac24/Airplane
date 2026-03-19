#include "Camera.h"

pCCamera CCamera_Create()
{
    pCCamera camera = (pCCamera)malloc(sizeof(CCamera));
    if (camera)
    {
        memset(camera, 0, sizeof(CCamera));
        camera->root = CNode_CreateNode(MatrixTransform);
    }
    return camera;
}

void CCamera_InitOpenGL(pCCamera camera)
{

}
void CCamera_ResizeOpenGL(pCCamera camera)
{

}
void CCamera_PaintOpenGL(pCCamera camera)
{

}
CVec3d CCamera_GetPosition(pCCamera camera)
{
    return Vec_CreatVec3d(0, 0, 0);
}
