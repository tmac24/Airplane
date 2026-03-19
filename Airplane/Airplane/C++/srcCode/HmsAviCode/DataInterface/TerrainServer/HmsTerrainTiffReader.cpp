#include <HmsAviPf.h>
#include <HmsGE/HmsLog.h>
#include "HmsTerrainTiffReader.h"
#include "base/ZipUtils.h"
#include "HmsLog.h"

USING_NS_HMS;

std::string CHmsTerrainTiffReader::s_strGeoTiffPath = "GeoTiff/";



void CHmsTerrainTiffReader::SetGeoTiffPath(const std::string & strPath)
{
	if (strPath.size() > 1)
	{
		switch (strPath.at(strPath.size() - 1))
		{
		case '\\':
		case '/':
			s_strGeoTiffPath = strPath;
			break;
		default:
			s_strGeoTiffPath = strPath + '/';
			break;
		}
	}
}

CHmsTerrainTiffReader::CHmsTerrainTiffReader()
{

}

CHmsTerrainTiffReader::~CHmsTerrainTiffReader()
{

}

bool CHmsTerrainTiffReader::GetGeoTiff(unsigned int nIndexLong, unsigned int nIndexLat, HmsTerrainScaledImage & geoTiff)
{
	bool bRet = false;
#if 1
	auto fileName = GetName(nIndexLong, nIndexLat);
	auto packagePath = GetPackagePath(fileName);

	//Data dataZip = FileUtils::getInstance()->getDataFromFile(packagePath);
	ZipFile zFile(packagePath);
	if (zFile.getFirstFilename().size() != 0)
	{
		ssize_t nFileLenth = 0;
		unsigned char * pFileData = nullptr;

		auto hdrName = GetNameForHdr(fileName);
		if (zFile.fileExists(hdrName))
		{
			pFileData = zFile.getFileData(hdrName, &nFileLenth);
			if (pFileData)
			{
				bRet = DecodeHdr((char*)pFileData, nFileLenth, geoTiff);
				free(pFileData);
				pFileData = nullptr;
			}
			
		}

		if (bRet)
		{
			auto tifName = GetNameForTif(fileName);
			if (zFile.fileExists(tifName))
			{
				pFileData = zFile.getFileData(tifName, &nFileLenth);
				if (pFileData)
				{
					bRet = DecodeTif(pFileData, nFileLenth, geoTiff);
					free(pFileData);
					pFileData = nullptr;
				}
// 				if (bRet)
// 				{
// 					std::string strPath = "E:\\tempTerrain\\";
// 					
// 					char strNameSuffix[20] = { 0 };
// 					sprintf(strNameSuffix, "_%d.png", geoTiff.nBitScale);
// 
// 					strPath += fileName + strNameSuffix;
// 					geoTiff.pImage->saveToFile(strPath);
// 				}
			}
		}
	}
#endif
	return bRet;
}

bool CHmsTerrainTiffReader::GetGeoTiff(const TerrainTiffIndex & index, HmsTerrainScaledImage & geoTiff)
{
	return GetGeoTiff(index.nLong, index.nLat, geoTiff);
}

std::string CHmsTerrainTiffReader::GetName(unsigned int nIndexLong, unsigned int nIndexLat)
{
	char strFileName[64] = { 0 };
	sprintf(strFileName, "srtm_%02d_%02d", nIndexLong, nIndexLat);
	return strFileName;
}

std::string CHmsTerrainTiffReader::GetNameForHdr(const std::string & strName)
{
	return strName + ".hdr";
}

std::string CHmsTerrainTiffReader::GetNameForTfw(const std::string & strName)
{
	return strName + ".tfw";
}

std::string CHmsTerrainTiffReader::GetNameForTif(const std::string & strName)
{
	return strName + ".tif";
}

std::string CHmsTerrainTiffReader::GetPackagePath(const std::string & strName)
{
	return s_strGeoTiffPath + strName + ".zip";
}


bool CHmsTerrainTiffReader::DecodeTif(unsigned char * data, unsigned int nDataLenth, HmsTerrainScaledImage & geoTiff)
{
	bool bRet = false;
	if (geoTiff.pImage)
	{
		delete geoTiff.pImage;
		geoTiff.pImage = nullptr;
	}

	do 
	{
		geoTiff.pImage = new STD_NOTHROW Image();
		if (geoTiff.pImage)
		{
#if 1
			int nScale = 1 << geoTiff.nBitScale;
			bRet = this->ReadGeoTif(geoTiff.pImage, nScale, data, nDataLenth);
#else
			if (0 == geoTiff.nBitScale)
			{
			bRet = geoTiff.pImage->initWithImageData(data, nDataLenth);
		}
			else
			{
				int nScale = 1 << geoTiff.nBitScale;
				auto tempImage = new STD_NOTHROW Image();
				if (tempImage)
				{
					if (tempImage->initWithImageData(data, nDataLenth))
					{
						int nWidth = tempImage->getWidth()/nScale + ((tempImage->getWidth()%nScale)?1:0);
						int nHeight = tempImage->getHeight() / nScale + ((tempImage->getHeight() % nScale) ? 1 : 0);

						auto bitPerPixel = tempImage->getBitPerPixel()/8;
						int nNewPicSize = nWidth*nHeight * 4;
						auto tempData = new unsigned char[nNewPicSize];
						//memset(tempData, 0, nNewPicSize);
						if (tempData && 4 == bitPerPixel)
						{
							auto tempImageData = tempImage->getData();
							for (int nRow = 0, nRowNew = 0; nRow < tempImage->getWidth(); nRow += nScale, nRowNew++)
							{
								for (int nCol = 0, nColNew = 0; nCol < tempImage->getHeight(); nCol += nScale, nColNew++)
								{
									memcpy(tempData + (nRowNew*nWidth + nColNew) * 4, tempImageData + (nRow*tempImage->getWidth() + nCol) * 4, 4);
								}
							}
							
							geoTiff.pImage->initWithRawData(tempData, nNewPicSize, nWidth, nHeight, 4);

							delete tempData;
							tempData = nullptr;
						}	
						
						bRet = true;
					}
				}
			}	
#endif
		}
	} while (0);

	if (!bRet)
	{
		delete geoTiff.pImage;
		geoTiff.pImage = nullptr;
	}
	return bRet;
}

bool CHmsTerrainTiffReader::DecodeHdr(char * data, unsigned int nDataLenth, HmsTerrainScaledImage & geoTiff)
{
	bool bRet = false;
	if (data && nDataLenth > 0)
	{
		if (memcmp(data, "Geotiff", 7) != 0)
		{
			return bRet;
		}

		std::string strFindTag = "Corner Coordinates:";
		char * strCornerCoord = strstr(data, strFindTag.c_str());
		if (strCornerCoord)
		{
			int nPosStart = strCornerCoord - data + strFindTag.size() + 2;
			std::string cornerCoord(data + nPosStart, nDataLenth - nPosStart);
			int nSizePos = 0;
			geoTiff.upperLeft = FindSizeByString(cornerCoord, nSizePos);
			geoTiff.lowerLeft = FindSizeByString(cornerCoord, nSizePos);
			geoTiff.upperRight = FindSizeByString(cornerCoord, nSizePos);
			geoTiff.lowerRight = FindSizeByString(cornerCoord, nSizePos);
			geoTiff.center = FindSizeByString(cornerCoord, nSizePos);

			if (geoTiff.upperRight != geoTiff.lowerLeft)
			{
				geoTiff.sizeGeo = Size(geoTiff.upperRight - geoTiff.lowerLeft);
				bRet = true;
			}
		}
	}
	return bRet;
}

bool CHmsTerrainTiffReader::DecodeHdr(char * data, unsigned int nDataLenth, HmsTerrainRawData & terrainData)
{
	bool bRet = false;
	if (data && nDataLenth > 0)
	{
		if (memcmp(data, "Geotiff", 7) != 0)
		{
			return bRet;
		}

		std::string strFindTag = "Corner Coordinates:";
		char * strCornerCoord = strstr(data, strFindTag.c_str());
		if (strCornerCoord)
		{
			int nPosStart = strCornerCoord - data + strFindTag.size() + 2;
			std::string cornerCoord(data + nPosStart, nDataLenth - nPosStart);
			int nSizePos = 0;
			terrainData.upperLeft = FindSizeByString(cornerCoord, nSizePos);
			terrainData.lowerLeft = FindSizeByString(cornerCoord, nSizePos);
			terrainData.upperRight = FindSizeByString(cornerCoord, nSizePos);
			terrainData.lowerRight = FindSizeByString(cornerCoord, nSizePos);
			terrainData.center = FindSizeByString(cornerCoord, nSizePos);

			if (terrainData.upperRight != terrainData.lowerLeft)
			{
				terrainData.sizeGeo = Size(terrainData.upperRight - terrainData.lowerLeft);
				bRet = true;
			}
		}
	}
	return bRet;
}

HmsAviPf::Vec2 CHmsTerrainTiffReader::FindSizeByString(const std::string & strContent, int & nStart)
{
	HmsAviPf::Vec2 outPos;
	size_t nPosLeft = strContent.find('(', nStart);
	if (nPosLeft)
	{
		size_t nPosCenter = strContent.find(',', nPosLeft);
		if (nPosCenter)
		{
			size_t nPosRight = strContent.find(')', nPosCenter);
			if (nPosRight)
			{
				std::string strX = strContent.substr(nPosLeft+1, nPosCenter-nPosLeft-1);
				std::string strY = strContent.substr(nPosCenter+1, nPosRight-nPosCenter-1);

				outPos.x = atof(strX.c_str());
				outPos.y = atof(strY.c_str());
				nStart = nPosRight + 1;
			}
			else
			{
				nStart = strContent.size();
			}
		}
		else
		{
			nStart = strContent.size();
		}
	}
	else
	{
		nStart = strContent.size();
	}
	return outPos;
}
#define HMS_USE_TIFF 1
#if HMS_USE_TIFF

#if (HMS_TARGET_PLATFORM == HMS_PLATFORM_ANDROID)
#include "tiffio.h"
#else
#include "tiff/tiffio.h"
#endif

namespace
{
	typedef struct
	{
		const unsigned char * data;
		ssize_t size;
		int offset;
	}tImageSource;

	static tmsize_t tiffReadProc(thandle_t fd, void* buf, tmsize_t size)
	{
		tImageSource* isource = (tImageSource*)fd;
		uint8* ma;
		uint64 mb;
		unsigned long n;
		unsigned long o;
		tmsize_t p;
		ma = (uint8*)buf;
		mb = size;
		p = 0;
		while (mb>0)
		{
			n = 0x80000000UL;
			if ((uint64)n>mb)
				n = (unsigned long)mb;


			if ((int)(isource->offset + n) <= isource->size)
			{
				memcpy(ma, isource->data + isource->offset, n);
				isource->offset += n;
				o = n;
			}
			else
			{
				return 0;
			}

			ma += o;
			mb -= o;
			p += o;
			if (o != n)
			{
				break;
			}
		}
		return p;
	}

	static tmsize_t tiffWriteProc(thandle_t fd, void* buf, tmsize_t size)
	{
		HMS_UNUSED_PARAM(fd);
		HMS_UNUSED_PARAM(buf);
		HMS_UNUSED_PARAM(size);
		return 0;
	}


	static uint64 tiffSeekProc(thandle_t fd, uint64 off, int whence)
	{
		tImageSource* isource = (tImageSource*)fd;
		uint64 ret = -1;
		do
		{
			if (whence == SEEK_SET)
			{
				HMS_BREAK_IF(off >= (uint64)isource->size);
				ret = isource->offset = (uint32)off;
			}
			else if (whence == SEEK_CUR)
			{
				HMS_BREAK_IF(isource->offset + off >= (uint64)isource->size);
				ret = isource->offset += (uint32)off;
			}
			else if (whence == SEEK_END)
			{
				HMS_BREAK_IF(off >= (uint64)isource->size);
				ret = isource->offset = (uint32)(isource->size - 1 - off);
			}
			else
			{
				HMS_BREAK_IF(off >= (uint64)isource->size);
				ret = isource->offset = (uint32)off;
			}
		} while (0);

		return ret;
	}

	static uint64 tiffSizeProc(thandle_t fd)
	{
		tImageSource* imageSrc = (tImageSource*)fd;
		return imageSrc->size;
	}

	static int tiffCloseProc(thandle_t fd)
	{
		HMS_UNUSED_PARAM(fd);
		return 0;
	}

	static int tiffMapProc(thandle_t fd, void** base, toff_t* size)
	{
		HMS_UNUSED_PARAM(fd);
		HMS_UNUSED_PARAM(base);
		HMS_UNUSED_PARAM(size);
		return 0;
	}

	static void tiffUnmapProc(thandle_t fd, void* base, toff_t size)
	{
		HMS_UNUSED_PARAM(fd);
		HMS_UNUSED_PARAM(base);
		HMS_UNUSED_PARAM(size);
	}
}
#endif // HMS_USE_TIFF

bool CHmsTerrainTiffReader::ReadGeoTif(HmsAviPf::Image * pImage, int nScale, const unsigned char * data, ssize_t dataLen)
{
	bool ret = false;
#if 1
	do
	{
		// set the read call back function
		tImageSource imageSource;
		imageSource.data = data;
		imageSource.size = dataLen;
		imageSource.offset = 0;

		TIFF* tif = TIFFClientOpen("file.tif", "r", (thandle_t)&imageSource,
			tiffReadProc, tiffWriteProc,
			tiffSeekProc, tiffCloseProc, tiffSizeProc,
			tiffMapProc,
			tiffUnmapProc);

		HMS_BREAK_IF(nullptr == tif);

		uint32 w = 0, h = 0;
		uint16 bitsPerSample = 0, samplePerPixel = 0, planarConfig = 0;

		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
		TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig);

		auto nStripSize = TIFFStripSize(tif);
		
		auto stripNo = TIFFNumberOfStrips(tif);

		int nPicWidth = w / nScale + ((w % nScale) ? 1 : 0);
		int nPicHeight = h / nScale + ((h % nScale) ? 1 : 0);
		auto dataPicLen = nPicWidth * nPicWidth * sizeof(uint32);
		auto data = static_cast<unsigned int*>(malloc(dataPicLen * sizeof(unsigned char)));

		if (data)
		{
			auto buf = (unsigned short*)_TIFFmalloc(nStripSize);
			if (buf)
			{
				tstrip_t strip;
				tmsize_t nRet = 0;
				int nPicRow = 0;
#if 1
				for (strip = 0; strip < TIFFNumberOfStrips(tif); strip++)
				{	
					if (strip%nScale == 0)
					{
						nRet = TIFFReadEncodedStrip(tif, strip, buf, (tsize_t)-1) / 2;
						tmsize_t pos = nPicRow*nPicWidth;
						for (tmsize_t i = 0; i < nRet; i += nScale)
						{
							data[pos++] = buf[i];
						}
						nPicRow++;
					}
				}
#else
				int nRowJump = 0;
				for (strip = 0; strip < TIFFNumberOfStrips(tif); strip++)
				{
					if (nRowJump == 0)
					{
						nRet = TIFFReadEncodedStrip(tif, strip, buf, (tsize_t)-1) / 2;
						tmsize_t pos = nPicRow*nPicWidth;
						for (tmsize_t i = 0; i < nRet; i += nScale)
						{
							data[pos++] = buf[i];
						}
						nPicRow++;
					}

					nRowJump++;
					if (nRowJump == nScale)
					{
						nRowJump = 0;
					}
				}
#endif
				_TIFFfree(buf);
			}

			ret = pImage->initWithRawData((unsigned char *)data, dataPicLen, nPicWidth, nPicHeight, 4);

			free(data);
			data = nullptr;
		}

		TIFFClose(tif);
	} while (0);
#endif
	return ret;
}

bool CHmsTerrainTiffReader::GetTerrainData(const TerrainTiffIndex & index, HmsTerrainRawData & terrainData)
{
	return GetTerrainData(index.nLong, index.nLat, terrainData);
}

bool CHmsTerrainTiffReader::GetTerrainData(unsigned int nIndexLong, unsigned int nIndexLat, HmsTerrainRawData & terrainData)
{
    std::lock_guard<std::mutex> locker(m_mutex);

	bool bRet = false;
#if 1
	auto fileName = GetName(nIndexLong, nIndexLat);
	auto packagePath = GetPackagePath(fileName);

	//Data dataZip = FileUtils::getInstance()->getDataFromFile(packagePath);
	ZipFile zFile(packagePath);
	if (zFile.getFirstFilename().size() != 0)
	{
		ssize_t nFileLenth = 0;
		unsigned char * pFileData = nullptr;

		auto hdrName = GetNameForHdr(fileName);
		if (zFile.fileExists(hdrName))
		{
			pFileData = zFile.getFileData(hdrName, &nFileLenth);
			if (pFileData)
			{
				bRet = DecodeHdr((char*)pFileData, nFileLenth, terrainData);
				free(pFileData);
				pFileData = nullptr;
			}
		}

		if (bRet)
		{
			auto tifName = GetNameForTif(fileName);
			if (zFile.fileExists(tifName))
			{
				pFileData = zFile.getFileData(tifName, &nFileLenth);
				if (pFileData)
				{
					bRet = FillTerrainData(terrainData, pFileData, nFileLenth);
					free(pFileData);
					pFileData = nullptr;
				}
				else
					terrainData.eLoadStatus = TerrainTiffStatus::Failed;
			}
			else
				terrainData.eLoadStatus = TerrainTiffStatus::Failed;
		}
		else
			terrainData.eLoadStatus = TerrainTiffStatus::Failed;
	}
	else
		terrainData.eLoadStatus = TerrainTiffStatus::Failed;
#endif
	return bRet;
}

bool CHmsTerrainTiffReader::FillTerrainData(HmsTerrainRawData & terrainData, const unsigned char * data, ssize_t dataLen)
{
	bool bRet = false;

	do
	{
		// set the read call back function
		tImageSource imageSource;
		imageSource.data = data;
		imageSource.size = dataLen;
		imageSource.offset = 0;

		TIFF* tif = TIFFClientOpen("file.tif", "r", (thandle_t)&imageSource,
			tiffReadProc, tiffWriteProc,
			tiffSeekProc, tiffCloseProc, tiffSizeProc,
			tiffMapProc,
			tiffUnmapProc);

		if (nullptr == tif)
		{
			return false;
		}

		HMS_BREAK_IF(nullptr == tif);

		uint32 w = 0, h = 0;
		uint16 bitsPerSample = 0, samplePerPixel = 0, planarConfig = 0;

		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplePerPixel);
		TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planarConfig);

		auto nStripSize = TIFFStripSize(tif);

		auto stripNo = TIFFNumberOfStrips(tif);

		try
		{
		if (terrainData.pData)
		{
			if (terrainData.nDataWidth != w || terrainData.nDataHeight != h)
			{
				delete[] terrainData.pData;
				terrainData.pData = nullptr;

				terrainData.nDataWidth = w;
				terrainData.nDataHeight = h;
				terrainData.pData = new short[terrainData.nDataWidth * terrainData.nDataHeight];
			}
		}
		else
		{
			terrainData.nDataWidth = w;
			terrainData.nDataHeight = h;
			terrainData.pData = new short[terrainData.nDataWidth * terrainData.nDataHeight];
		}
		}
		catch (std::bad_alloc& e1)
		{
			HMSLOG("FillTerrainData error nLon[%d], nLat[%d], %s\n",
					terrainData.gtIndex.nLong, terrainData.gtIndex.nLat, e1.what());
			TIFFClose(tif);
			bRet = false;
			break;
		}

        if (terrainData.pData == nullptr)
        {
//            HMSLOGERROR("new failed!");
            return false;
        }

		auto & tempData = terrainData.pData;

		if (tempData)
		{
			auto buf = (unsigned short*)_TIFFmalloc(nStripSize);
			if (buf)
			{
				tstrip_t strip;
				tmsize_t nRet = 0;
				for (strip = 0; strip < stripNo; strip++)
				{
					nRet = TIFFReadEncodedStrip(tif, strip, buf, (tsize_t)-1);
					memcpy(tempData + strip * w, buf, nRet);
				}

				bRet = true;

				_TIFFfree(buf);
			}
			else
			{
				auto dataPicLen = terrainData.nDataWidth * terrainData.nDataHeight * sizeof(uint16);
				memset(tempData, 0, dataPicLen);
			}
		}

		TIFFClose(tif);

	} while (0);

	if (bRet)
	{
		terrainData.eLoadStatus = TerrainTiffStatus::Loaded;
	}
	else
	{
		terrainData.eLoadStatus = TerrainTiffStatus::Failed;
	}

	return bRet;
}






