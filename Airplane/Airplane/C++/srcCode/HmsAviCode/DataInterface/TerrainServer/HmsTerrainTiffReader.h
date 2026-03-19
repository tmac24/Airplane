#pragma once
#include <string>
#include <mutex>
#include "HmsTerrainTiffDef.h"
#include "HmsTerrainScaledImage.h"

class CHmsTerrainTiffReader
{
public:
	CHmsTerrainTiffReader();
	~CHmsTerrainTiffReader();

	bool GetGeoTiff(const TerrainTiffIndex & index, HmsTerrainScaledImage & geoTiff);
	bool GetGeoTiff(unsigned int nIndexLong, unsigned int nIndexLat, HmsTerrainScaledImage & geoTiff);

	bool GetTerrainData(const TerrainTiffIndex & index, HmsTerrainRawData & terrainData);
	bool GetTerrainData(unsigned int nIndexLong, unsigned int nIndexLat, HmsTerrainRawData & terrainData);

public:
	//设置路径
	static void SetGeoTiffPath(const std::string & strPath);

protected:
	//解码TIf
	bool DecodeTif(unsigned char * data, unsigned int nDataLenth, HmsTerrainScaledImage & geoTiff);
	//解码Hdr
	bool DecodeHdr(char * data, unsigned int nDataLenth, HmsTerrainScaledImage & geoTiff);
	bool DecodeHdr(char * data, unsigned int nDataLenth, HmsTerrainRawData & terrainData);

	//查找（x,x）;
	HmsAviPf::Vec2 FindSizeByString(const std::string & strContent, int & nStart);

	bool ReadGeoTif(HmsAviPf::Image * pImage, int nScale, const unsigned char * data, ssize_t dataLen);
	bool FillTerrainData(HmsTerrainRawData & terrainData, const unsigned char * data, ssize_t dataLen);

private:
	//获取文件名称
	std::string GetName(unsigned int nIndexLong, unsigned int nIndexLat);
	//获取Hdr名称
	std::string GetNameForHdr(const std::string & strName);
	//获取Tfw名称
	std::string GetNameForTfw(const std::string & strName);
	//获取Tif名称
	std::string GetNameForTif(const std::string & strName);
	//获取压缩包路径
	std::string GetPackagePath(const std::string & strName);

private:
	static std::string		s_strGeoTiffPath;				//路径,默认 GeoTiff
    std::mutex m_mutex;
};

