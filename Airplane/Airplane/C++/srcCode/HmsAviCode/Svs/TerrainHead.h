#ifndef _TERRAIN_HEADER__H_
#define _TERRAIN_HEADER__H_


#ifdef WIN32
/*#include <windows.h>*/
#endif  

#include "GLHeader.h"

#define MAXTERRAIN 120000//120000///*地形可视范围*/
#define FOGEND 70000/*雾结束距离*/
/*
//地形颜色
static float TerColor[4] = { 0.5, 1.0, 0.5, 1.0 };
static float SkyColor[4] = { 0.5, 0.6, 0.95, 1.0 };
static float FogColor[4] = { 0.5, 0.6, 0.85, 1.0 };
*/

struct TileLayer
{
    double tileSpan;/* 一块地形的跨度   */
    unsigned int unitCellCount;/* 该层理论应该细分的格子数 */
    double minDistance;/*大于此距离则显示该层 */
};

#if 0
const static Layer LayerMap[23] = {  
    1, 32, 96000,
    0.5, 32, 58000,	
    0.25, 32, 34000,
    0.125, 32, 15000,
    0.0625, 28, 8000,	
    0.03125, 24, 4000,
    0.015625, 20, 2000,
    0.0078125, 20, 1000,
    0.00390625, 20, 500,
    0.001953125, 20, 400,
    0.0009765625, 20, 300,
    0.00048828125, 20, 26.9,		
    0.000244140625, 20, 13.4,	
    0.0001220703125, 20, 6.7,		
    0.00006103515625, 20, 0,
}; 

#else

extern const TileLayer LayerMap[23];

extern const TileLayer LayerMap1[23];

#endif







#endif // !__HEADER__