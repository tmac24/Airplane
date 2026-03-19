#include <HmsAviPf.h>
#include "HmsMapReader.h"
#include "base/HmsFileUtils.h"
#include <mutex>


#ifdef __vxworks
std::string CHmsMapReader::s_strMapRootFolder = "F:/mapTile/satellite/";//"D:\\Map\\";
#else
std::string CHmsMapReader::s_strMapRootFolder = "/ata1:1/MapTile/satellite";
#endif
std::string CHmsMapReader::s_strMapFileSuffix = "jpg";
std::mutex CHmsMapReader::s_mutexRootFolder;


CHmsMapReader::CHmsMapReader()
{
}


CHmsMapReader::~CHmsMapReader()
{
}

HmsAviPf::Image * CHmsMapReader::GetMapTile(int nLayer, int nLngIndex, int nLatIndex)
{
	std::string strPath = GetTilePath(nLayer, nLngIndex, nLatIndex);

	if (!HmsAviPf::CHmsFileUtils::getInstance()->isFileExist(strPath))
	{
		return nullptr;
	}
	
	HmsAviPf::Image * pImage = new STD_NOTHROW HmsAviPf::Image;
	if (pImage)
	{		
#if 0
		char * pBuffer = nullptr;
		int nFileLen = 0;
		do 
		{
			FILE * fp = nullptr;
			fopen_s(&fp, strPath.c_str(), "r");
			if (fp)
			{
				fseek(fp, 0, SEEK_END);

				nFileLen = ftell(fp);

				rewind(fp);
				pBuffer = new char[nFileLen];
				if (pBuffer)
				{
					fread_s(pBuffer, nFileLen, 1, nFileLen, fp);
				}
				fclose(fp);
				fp = nullptr;
			}
		} while (0);

		if (pBuffer)
		{
			if (pImage->initWithImageData((unsigned char *)pBuffer, nFileLen))
			{
				pImage->autorelease();
			}
			else
			{
				delete pImage;
				pImage = nullptr;
			}
			delete pBuffer;
			pBuffer = nullptr;
		}
		else
		{
			delete pImage;
			pImage = nullptr;
		}
#else
		if (pImage->initWithImageFile(strPath))
		{
			pImage->autorelease();
		}
		else
		{
			delete pImage;
			pImage = nullptr;
		}
#endif
	}

	return pImage;
}

std::string CHmsMapReader::GetRootFolder()
{
	std::lock_guard<std::mutex> locker(s_mutexRootFolder);
	return s_strMapRootFolder;
}

void CHmsMapReader::SetRootFolder(const std::string & strPath)
{
	std::lock_guard<std::mutex> locker(s_mutexRootFolder);
	switch (strPath.at(strPath.size() - 1))
	{
	case '\\':
	case '/':
		s_strMapRootFolder = strPath;
		break;
	default:
		s_strMapRootFolder = strPath + '\\';
		break;
	}
}

void CHmsMapReader::SetRootFolder(const std::string & strPath, const std::string & strSuffix)
{
	std::lock_guard<std::mutex> locker(s_mutexRootFolder);
	switch (strPath.at(strPath.size() - 1))
	{
	case '\\':
	case '/':
		s_strMapRootFolder = strPath;
		break;
	default:
		s_strMapRootFolder = strPath + '\\';
		break;
	}
	s_strMapFileSuffix = strSuffix;
}

std::string CHmsMapReader::GetTilePath(int nLayer, int nLngIndex, int nLatIndex)
{
	char strName[128] = { 0 };
	sprintf(strName, "%d\\%d\\%d.%s", nLayer, nLngIndex, nLatIndex, s_strMapFileSuffix.c_str());

	return s_strMapRootFolder + strName;
	//return "E:\\MapTileDownload\\googlemaps\\satellite\\12\\3235\\1665.jpg";
}

bool CHmsMapReader::GetLayerMap(std::vector<int> & vDirectory)
{
	char strTemp[32] = { 0 };
	auto sharedFileUtils = HmsAviPf::CHmsFileUtils::getInstance();
	vDirectory.clear();

	for (int i = 2; i <= 21; i++)
	{
		sprintf(strTemp, "%d", i);
		if (sharedFileUtils->isDirectoryExist(s_strMapRootFolder + strTemp))
		{
			vDirectory.push_back(i);
		}
	}

	return vDirectory.size() ? true : false;
}


