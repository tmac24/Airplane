#ifndef _NODE_DEFINE__H_
#define _NODE_DEFINE__H_

#if 0
#define USE_NODE_LIST   0

#define SCENCE_NODE_DATA_RELEASE_SUCCESS 1
#define SCENCE_NODE_DATA_RELEASE_FAILED 0

#define SCENCE_NODE_RELEASE_SUCCESS 1
#define SCENCE_NODE_RELEASE_FAILED 0

#define SCENCE_NODE_DETACH_SUCCESS 1
#define SCENCE_NODE_DETACH_FAILED 0
#endif

enum EN_NodeType
{
    Node,
    Group,
    MatrixTransform,
    Camera,
    DrawFunc,
    Geometry,
    NT_Drawable,
    TerrainTile,
    TerrainChunk,
    BillBoard,
    Model,
};

#endif
