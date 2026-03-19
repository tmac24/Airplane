#include "TerrainCommon.h"


pTTerrain TTerrain_Create()
{
	pTTerrain p = (pTTerrain)malloc(sizeof(TTerrain));
	if (p == NULL)
	{
		printf("pTTerrain p = (pTTerrain)malloc(sizeof(TTerrain)) failed\n");
		exit(1);
	}
	memset(p, 0, sizeof(TTerrain));
	p->pTerTile = (pCTerrainTile)(CNode_CreateNode(TerrainTile)->pNodeData);
	return p;
}

void TTerrain_Free(pTTerrain p)
{
	if (p)
	{
		CTerrainTile_Free(p->pTerTile);
		p = NULL;
	}
}

