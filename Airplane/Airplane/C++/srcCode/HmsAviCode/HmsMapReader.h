#pragma once
#include "base/HmsImage.h"
#include <mutex>

class CHmsMapReader
{
public:
	CHmsMapReader();
	~CHmsMapReader();

	//获取地图
	HmsAviPf::Image * GetMapTile(int nLayer, int nLngIndex, int nLatIndex);

	//获取地图级别
	static bool GetLayerMap(std::vector<int> & vDirectory);

public:
	//获取地图目录
	static std::string GetRootFolder();
	//设置地图目录
	static void SetRootFolder(const std::string & strPath);
	static void SetRootFolder(const std::string & strPath, const std::string & strSuffix);
	//获取地图路径
	static std::string GetTilePath(int nLayer, int nLngIndex, int nLatIndex);


private:
	static std::mutex s_mutexRootFolder;				//地图跟目录锁
	static std::string s_strMapRootFolder;				//地图跟目录
	static std::string s_strMapFileSuffix;
};

