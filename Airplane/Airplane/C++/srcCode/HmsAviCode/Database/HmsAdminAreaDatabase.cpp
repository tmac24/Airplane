#include "HmsAdminAreaDatabase.h"
#include <sstream>
#include "HmsLog.h"

int code_convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen);
std::string u2a(const char *inbuf);
std::string a2u(const char *inbuf);

void ShapeRange::SetData(double dLeft, double dRight, double dBottom, double dTop, const HmsGeomColumns & geomColums)
{
	nMaxGx = (int)((dRight - geomColums.dIdxOriginX) / geomColums.dIdxGridSize);
	nMinGx = (int)((dLeft - geomColums.dIdxOriginX) / geomColums.dIdxGridSize);
	nMaxGy = (int)((dTop - geomColums.dIdxOriginY) / geomColums.dIdxGridSize);
	nMinGy = (int)((dBottom - geomColums.dIdxOriginY) / geomColums.dIdxGridSize);
}


CHmsAdminAreaDatabase::CHmsAdminAreaDatabase()
	: m_nMultUsedIndex(0)
{

}


CHmsAdminAreaDatabase::~CHmsAdminAreaDatabase()
{
	Close();
}

bool CHmsAdminAreaDatabase::ResetDatabase(const std::string & strPath)
{
	if (InitDatabase(strPath))
	{
		return IsGDBDatabase();
	}
	return false;
}

bool CHmsAdminAreaDatabase::IsGDBDatabase()
{
	bool bRet = false;
	std::string strSql = "select count(*) from sqlite_master where type='table' and tbl_name='GDB_GeomColumns';";

	if (1 == ExecGetData(strSql))
	{
		bRet = true;
	}

	return bRet;
}

bool CHmsAdminAreaDatabase::FillAdminAreasDescribe(HmsAdminAreaDescribe & describe)
{
	auto funcTable = [=, &describe](char ** strResult, int nRow, int nCol)
	{
		for (int i = 0; i <= nRow; i++)
		{
			HmsGeomColumns gc;
			int nPosStart = i*nCol;
			if (2 == atoi(strResult[nPosStart++]))
			{
				gc.strTableName = strResult[nPosStart++];
				gc.strFieldName = strResult[nPosStart++];
				gc.nShapeType = atoi(strResult[nPosStart++]);
				gc.dExtentLeft = atof(strResult[nPosStart++]);
				gc.dExtentBottom = atof(strResult[nPosStart++]);
				gc.dExtentRight = atof(strResult[nPosStart++]);
				gc.dExtentTop = atof(strResult[nPosStart++]);
				gc.dIdxOriginX = atof(strResult[nPosStart++]);
				gc.dIdxOriginY = atof(strResult[nPosStart++]);
				gc.dIdxGridSize = atof(strResult[nPosStart++]);

				char strIsoName[20] = { 0 };
				int nLayerIndex = 0;
				sscanf(gc.strTableName.c_str(), "%[^_]_adm%d", strIsoName, &nLayerIndex);
				gc.strIsoName = strIsoName;
				gc.nLayerIndex = nLayerIndex;
				describe.vLayerInfo.push_back(gc);
			}
		}
		return 0;
	};

	std::string strSql = "SELECT SRID, TableName, FieldName, ShapeType, ExtentLeft, ExtentBottom, ExtentRight, ExtentTop, IdxOriginX, IdxOriginY, IdxGridSize FROM GDB_GeomColumns WHERE SRID = 2;";


	bool bRet = ExecWithTable(strSql, funcTable);

	if (describe.vLayerInfo.size() > 0)
	{
		auto & info = describe.vLayerInfo.at(0);
		describe.boundary.dLeft = info.dExtentLeft;
		describe.boundary.dRight = info.dExtentRight;
		describe.boundary.dTop = info.dExtentTop;
		describe.boundary.dBottom = info.dExtentBottom;

		describe.boundaryRect = HmsAviPf::Rect(info.dExtentLeft, info.dExtentBottom, info.dExtentRight - info.dExtentLeft, info.dExtentTop - info.dExtentBottom);
	}
	describe.nLayerCnt = describe.vLayerInfo.size();

	return bRet;
}

bool CHmsAdminAreaDatabase::GetGeomColumns(std::vector<HmsGeomColumns> & vGeomGolumn)
{
	auto funcTable = [=, &vGeomGolumn](char ** strResult, int nRow, int nCol)
	{
		if (nRow <= 1)
		{
			return 0;
		}

		for (int i = 1; i <= nRow; i++)
		{
			HmsGeomColumns gc;
			int nPosStart = i*nCol;
			if (2 == atoi(strResult[nPosStart++]))
			{
				gc.strTableName = strResult[nPosStart++];
				gc.strFieldName = strResult[nPosStart++];
				gc.nShapeType = atoi(strResult[nPosStart++]);
				gc.dExtentLeft = atof(strResult[nPosStart++]);
				gc.dExtentBottom = atof(strResult[nPosStart++]);
				gc.dExtentRight = atof(strResult[nPosStart++]);
				gc.dExtentTop = atof(strResult[nPosStart++]);
				gc.dIdxOriginX = atof(strResult[nPosStart++]);
				gc.dIdxOriginY = atof(strResult[nPosStart++]);
				gc.dIdxGridSize = atof(strResult[nPosStart++]);

				char strIsoName[20] = { 0 };
				int nLayerIndex = 0;
				sscanf(gc.strTableName.c_str(), "%[^_]_adm%d", strIsoName, &nLayerIndex );
				gc.strIsoName = strIsoName;
				gc.nLayerIndex = nLayerIndex;
				vGeomGolumn.push_back(gc);
			}
		}
		return 0;
	};

	std::string strSql = "SELECT SRID, TableName, FieldName, ShapeType, ExtentLeft, ExtentBottom, ExtentRight, ExtentTop, IdxOriginX, IdxOriginY, IdxGridSize FROM GDB_GeomColumns;";

	bool bRet = ExecWithTable(strSql, funcTable);
	return bRet;
}

bool CHmsAdminAreaDatabase::GetChinaLandAdmin(std::vector<HmsGeomData> & vGeomData)
{
    std::stringstream s;
    s << "SELECT Shape FROM admin;";
    auto pStmt = CreateSqlObject(s.str());
    if (pStmt)
    {
        auto colCnt = sqlite3_column_count(pStmt);

        if (colCnt != 1)
        {
            return false;
        }

        while (SQLITE_ROW == sqlite3_step(pStmt))
        {
            HmsGeomData geoData;
            geoData.strName = "CHINA";
            //geoData.strName = u2a((char*)sqlite3_column_text(pStmt, 2));
            geoData.id = 0;

            char * pData = (char*)sqlite3_column_blob(pStmt, 0);
            auto nLen = sqlite3_column_bytes(pStmt, 0);
            HmsRawGeomHeader * pHeader = (HmsRawGeomHeader*)(pData);
            if (sizeof(HmsRawGeomHeader) + pHeader->nIndexSize * sizeof(int) + pHeader->nPositionSize * sizeof(GeomPosition) == nLen)
            {
                geoData.header = *pHeader;
                int nPos = sizeof(HmsRawGeomHeader);
                int nSize = pHeader->nIndexSize * sizeof(int);

                geoData.vIndex.resize(pHeader->nIndexSize);
                memcpy(geoData.vIndex.data(), pData + nPos, nSize);


                nPos += nSize;
                nSize = pHeader->nPositionSize * sizeof(GeomPosition);

                geoData.vPosition.resize(pHeader->nPositionSize);
                memcpy(geoData.vPosition.data(), pData + nPos, nSize);
            }
            vGeomData.push_back(geoData);
        }
        ReleaseSqlObject(pStmt);
    }
    return true;
}

bool CHmsAdminAreaDatabase::GetGeomData(const char * strTable, const char * strNameCol, std::vector<HmsGeomData> & vGeomData)
{
	std::stringstream s;
	s << "SELECT Shape, OBJECTID, " << strNameCol << ", CENTROID_X, CENTROID_Y FROM " << strTable << ";";
	auto pStmt = CreateSqlObject(s.str());
	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 5)
		{
			return false;
		}

		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			HmsGeomData geoData;
			geoData.strName =(char*)sqlite3_column_text(pStmt, 2);
			//geoData.strName = u2a((char*)sqlite3_column_text(pStmt, 2));
			geoData.id = sqlite3_column_int(pStmt, 1);
			
			char * pData = (char*)sqlite3_column_blob(pStmt, 0);
			auto nLen = sqlite3_column_bytes(pStmt, 0);
			HmsRawGeomHeader * pHeader = (HmsRawGeomHeader*)(pData);
			if (sizeof(HmsRawGeomHeader) + pHeader->nIndexSize * sizeof(int) + pHeader->nPositionSize * sizeof(GeomPosition) == nLen)
			{
				geoData.header = *pHeader;
				int nPos = sizeof(HmsRawGeomHeader);
				int nSize = pHeader->nIndexSize * sizeof(int);

				geoData.vIndex.resize(pHeader->nIndexSize);
				memcpy(geoData.vIndex.data(), pData + nPos, nSize);


				nPos += nSize;
				nSize = pHeader->nPositionSize * sizeof(GeomPosition);

				geoData.vPosition.resize(pHeader->nPositionSize);
				memcpy(geoData.vPosition.data(), pData + nPos, nSize);
			}
            geoData.centralPos.dX = sqlite3_column_double(pStmt, 3);
            geoData.centralPos.dY = sqlite3_column_double(pStmt, 4);
			vGeomData.push_back(geoData);
		}
		ReleaseSqlObject(pStmt);
	}
	return true;
}

bool CHmsAdminAreaDatabase::GetGeomData(int nAdmIndex, const char * strNameCol, std::vector<HmsGeomData> & vGeomData)
{
	std::stringstream s;
	s << "CHN_adm" << nAdmIndex << ";";
	auto strTable = s.str();

	return GetGeomData(strTable.c_str(), strNameCol, vGeomData);
}

bool CHmsAdminAreaDatabase::GetGeomDataById(const char * strTable, const char * strNameCol, int objectId, HmsGeomData & geomData)
{
	bool bRet = false;
	std::stringstream s;
	s << "SELECT Shape, OBJECTID, " << strNameCol << ", CENTROID_X, CENTROID_Y FROM " << strTable << " WHERE OBJECTID = " << objectId << ";";
	auto pStmt = CreateSqlObject(s.str());
	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 5)
		{
			return bRet;
		}

		if (SQLITE_ROW == sqlite3_step(pStmt))
		{
			HmsGeomData & geoData = geomData;
			geoData.strName = (char*)sqlite3_column_text(pStmt, 2);
			//geoData.strName = u2a((char*)sqlite3_column_text(pStmt, 2));
			geoData.id = sqlite3_column_int(pStmt, 1);
            geoData.centralPos.dX = sqlite3_column_double(pStmt, 3);
            geoData.centralPos.dY = sqlite3_column_double(pStmt, 4);

			char * pData = (char*)sqlite3_column_blob(pStmt, 0);
			auto nLen = sqlite3_column_bytes(pStmt, 0);
			HmsRawGeomHeader * pHeader = (HmsRawGeomHeader*)(pData);
			if (sizeof(HmsRawGeomHeader) + pHeader->nIndexSize * sizeof(int) + pHeader->nPositionSize * sizeof(GeomPosition) == nLen)
			{
				geoData.header = *pHeader;
				int nPos = sizeof(HmsRawGeomHeader);
				int nSize = pHeader->nIndexSize * sizeof(int);

				geoData.vIndex.resize(pHeader->nIndexSize);
				memcpy(geoData.vIndex.data(), pData + nPos, nSize);


				nPos += nSize;
				nSize = pHeader->nPositionSize * sizeof(GeomPosition);

				geoData.vPosition.resize(pHeader->nPositionSize);
				memcpy(geoData.vPosition.data(), pData + nPos, nSize);
			}

			bRet = true;
		}
		ReleaseSqlObject(pStmt);
	}
	return bRet;
}

// 	{
// 		sqlite3_prepare(m_pSqlite, "insert into bList values ('hex',?);", -1, &pStmt, nullptr);
// 		sqlite3_bind_blob(pStmt, 1, Data, datasize, nullptr);
// 		sqlite3_step(pStmt);
// 	}

bool CHmsAdminAreaDatabase::GetGeomDataById(int nAdmIndex, const char * strNameCol, int objectId, HmsGeomData & geomData)
{
	std::stringstream s;
	s << "CHN_adm" << nAdmIndex << ";";
	auto strTable = s.str();

	return GetGeomDataById(strTable.c_str(), strNameCol, objectId, geomData);
}
#if 1
bool CHmsAdminAreaDatabase::GetRangeGeomData(const char * strTable, const char * strNameCol, const ShapeRange & shapeRange, std::vector<HmsGeomData*> & vGeomData)
{
	std::stringstream s; 
	s << "SELECT Shape, OBJECTID, " << strNameCol << ", CENTROID_X, CENTROID_Y FROM " << strTable << " INNER JOIN " << strTable << "_Shape_Index ON ";
	s << strTable << ".OBJECTID = " << strTable << "_Shape_Index.IndexedObjectId";
	s << " WHERE ";
	s << "((((MinGX >= " << shapeRange.nMinGx << ")AND(MinGX <= " << shapeRange.nMaxGx << "))OR((MaxGX >= " << shapeRange.nMinGx << ") AND(MaxGX <= " << shapeRange.nMaxGx;
	s << ")))AND(((MinGY >= " << shapeRange.nMinGy << ")AND(MinGY <= " << shapeRange.nMaxGy << "))OR((MaxGY >= " << shapeRange.nMinGy << ")AND(MaxGY <= " << shapeRange.nMaxGy;
#if 0
//	s << "))AND((MinGY <= " << shapeRange.nMinGy << ")AND(MaxGY >= " << shapeRange.nMaxGy << "))OR((MinGX <= " << shapeRange.nMinGx << ")AND(MaxGX >= " << shapeRange.nMaxGx;
	s << "))));";
#else
	s << "))))OR(((MinGY <= " << shapeRange.nMinGy << ")AND(MaxGY >= " << shapeRange.nMaxGy << "))OR((MinGX <= " << shapeRange.nMinGx << ")AND(MaxGX >= " << shapeRange.nMaxGx;
	s << ")));";
#endif

	auto pStmt = CreateSqlObject(s.str());
	if (pStmt)
	{
		auto colCnt = sqlite3_column_count(pStmt);

		if (colCnt != 5)
		{
			return false;
		}

		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			auto pGeoData = new HmsGeomData;
			if (pGeoData)
			{
				HmsGeomData & geoData = *pGeoData;
				auto tempText = (char*)sqlite3_column_text(pStmt, 2);
				if (tempText)
				{
					geoData.strName = TrimMultiData(tempText);
				}
				//geoData.strName = u2a((char*)sqlite3_column_text(pStmt, 2));
				geoData.id = sqlite3_column_int(pStmt, 1);
                geoData.centralPos.dX = sqlite3_column_double(pStmt, 3);
                geoData.centralPos.dY = sqlite3_column_double(pStmt, 4);

				char * pData = (char*)sqlite3_column_blob(pStmt, 0);
				auto nLen = sqlite3_column_bytes(pStmt, 0);
				HmsRawGeomHeader * pHeader = (HmsRawGeomHeader*)(pData);
				if (sizeof(HmsRawGeomHeader) + pHeader->nIndexSize * sizeof(int) + pHeader->nPositionSize * sizeof(GeomPosition) == nLen)
				{
					geoData.header = *pHeader;
					int nPos = sizeof(HmsRawGeomHeader);
					int nSize = pHeader->nIndexSize * sizeof(int);

					geoData.vIndex.resize(pHeader->nIndexSize);
					memcpy(geoData.vIndex.data(), pData + nPos, nSize);


					nPos += nSize;
					nSize = pHeader->nPositionSize * sizeof(GeomPosition);

					geoData.vPosition.resize(pHeader->nPositionSize);
					memcpy(geoData.vPosition.data(), pData + nPos, nSize);
				}
				vGeomData.push_back(pGeoData);
			}
            else
            {
                HMSLOGERROR("new failed!");
            }
		}
		ReleaseSqlObject(pStmt);
	}
	return true;
}
#else
bool CHmsAdminAreaDatabase::GetRangeGeomData(const char * strTable, const char * strNameCol, const ShapeRange & shapeRange, std::vector<HmsGeomData*> & vGeomData)
{
    std::stringstream s;
    s << "SELECT Shape, OBJECTID, " << strNameCol << " FROM " << strTable << " INNER JOIN " << strTable << "_Shape_Index ON ";
    s << strTable << ".OBJECTID = " << strTable << "_Shape_Index.IndexedObjectId";
    s << " WHERE ";
    s << "((((MinGX >= " << shapeRange.nMinGx << ")AND(MinGX <= " << shapeRange.nMaxGx << "))OR((MaxGX >= " << shapeRange.nMinGx << ") AND(MaxGX <= " << shapeRange.nMaxGx;
    s << ")))AND(((MinGY >= " << shapeRange.nMinGy << ")AND(MinGY <= " << shapeRange.nMaxGy << "))OR((MaxGY >= " << shapeRange.nMinGy << ")AND(MaxGY <= " << shapeRange.nMaxGy;
#if 0
    //	s << "))AND((MinGY <= " << shapeRange.nMinGy << ")AND(MaxGY >= " << shapeRange.nMaxGy << "))OR((MinGX <= " << shapeRange.nMinGx << ")AND(MaxGX >= " << shapeRange.nMaxGx;
    s << "))));";
#else
    s << "))))OR(((MinGY <= " << shapeRange.nMinGy << ")AND(MaxGY >= " << shapeRange.nMaxGy << "))OR((MinGX <= " << shapeRange.nMinGx << ")AND(MaxGX >= " << shapeRange.nMaxGx;
    s << ")));";
#endif

    std::string strSql = s.str();
    HMSLOG("%s\n", (char*)strSql.c_str());


    auto pStmt = CreateSqlObject(s.str());
    if (pStmt)
    {
        auto colCnt = sqlite3_column_count(pStmt);

        if (colCnt != 3)
        {
            return false;
        }

        while (SQLITE_ROW == sqlite3_step(pStmt))
        {
            auto pGeoData = new HmsGeomData;
            if (pGeoData)
            {
                HmsGeomData & geoData = *pGeoData;
                auto tempText = (char*)sqlite3_column_text(pStmt, 2);
                if (tempText)
                {
                    geoData.strName = TrimMultiData(tempText);
                }
                //geoData.strName = u2a((char*)sqlite3_column_text(pStmt, 2));
                geoData.id = sqlite3_column_int(pStmt, 1);
               // geoData.centralPos.dX = sqlite3_column_double(pStmt, 3);
               // geoData.centralPos.dY = sqlite3_column_double(pStmt, 4);

                char * pData = (char*)sqlite3_column_blob(pStmt, 0);
                auto nLen = sqlite3_column_bytes(pStmt, 0);
                HmsRawGeomHeader * pHeader = (HmsRawGeomHeader*)(pData);
                if (sizeof(HmsRawGeomHeader) + pHeader->nIndexSize * sizeof(int) + pHeader->nPositionSize * sizeof(GeomPosition) == nLen)
                {
                    geoData.header = *pHeader;
                    int nPos = sizeof(HmsRawGeomHeader);
                    int nSize = pHeader->nIndexSize * sizeof(int);

                    geoData.vIndex.resize(pHeader->nIndexSize);
                    memcpy(geoData.vIndex.data(), pData + nPos, nSize);


                    nPos += nSize;
                    nSize = pHeader->nPositionSize * sizeof(GeomPosition);

                    geoData.vPosition.resize(pHeader->nPositionSize);
                    memcpy(geoData.vPosition.data(), pData + nPos, nSize);
                }
                vGeomData.push_back(pGeoData);
            }
            else
            {
                HMSLOGERROR("new failed!");
            }
        }
        ReleaseSqlObject(pStmt);
    }
    return true;
}
#endif

std::string CHmsAdminAreaDatabase::TrimMultiData(const std::string & strData)
{
	std::string::size_type pos1 = std::string::npos;
	std::string::size_type pos2 = std::string::npos;

	pos1 = strData.find('|', 0);
	if (pos1 == std::string::npos)
	{
		return strData;
	}
	else if (0 == m_nMultUsedIndex)
	{
		return strData.substr(0, pos1);
	}
	else
	{
		pos2 = pos1;
		for (unsigned int i = 1; i <= m_nMultUsedIndex; i++)
		{
			pos2 = strData.find('|', pos1 + 1);
			if (pos2 != std::string::npos)
			{
				return strData.substr(pos1 + 1, strData.length() - pos1 - 1);
			}
			else if (i == m_nMultUsedIndex)
			{
				return strData.substr(pos1 + 1, pos2 - pos1 - 1);
			}
			else
			{
				pos1 = pos2;
			}
		}
	}
	return strData;
}
