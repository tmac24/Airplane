#pragma once

#include "base/HmsImage.h"
#include "HmsTerrainTiffDef.h"

struct HmsTerrainScaledImage
{
	TerrainTiffIndex			gtIndex;
	TerrainTiffStatus			eLoadStatus;			//加载状态
	HmsAviPf::Image *			pImage;
	HmsAviPf::Texture2D *		pTexture;
	HmsAviPf::Vec2				upperLeft;
	HmsAviPf::Vec2				lowerLeft;
	HmsAviPf::Vec2				upperRight;
	HmsAviPf::Vec2				lowerRight;
	HmsAviPf::Vec2				center;
	HmsAviPf::Size				sizeGeo;
	int							nBitScale;				//缩放标志位	0原图片，1缩放两倍，2缩放四倍

	HmsTerrainScaledImage()
	{
		pImage = nullptr;
		pTexture = nullptr;
		eLoadStatus = TerrainTiffStatus::Init;
		nBitScale = 0;
	}

	void Release()
	{
		if (pImage)
		{
			delete pImage;
			pImage = nullptr;
		}

		if (pTexture)
		{
			//delete pTexture;
			HMS_SAFE_RELEASE_NULL(pTexture);
		}
	}
};