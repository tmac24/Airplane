#include "HmsScratchPadSql.h"

CHmsScratchPadSql::CHmsScratchPadSql()
{
	char *pErrMsg = 0;
	int ret = 0;

	{
		std::string dbPath = CHmsFileUtils::getInstance()->fullPathForFilename("navData/scratchpad.db");
		ret = sqlite3_open(dbPath.c_str(), &m_pDb);
		if (ret != SQLITE_OK)
		{
			fprintf(stderr, "open NavImage fail：%s\n", sqlite3_errmsg(m_pDb));
			sqlite3_close(m_pDb);
		}
	}
	printf("open ScratchPadSql success\n");
}

CHmsScratchPadSql::CHmsScratchPadSql(const CHmsScratchPadSql&)
{

}


CHmsScratchPadSql::~CHmsScratchPadSql()
{
}

CHmsScratchPadSql& CHmsScratchPadSql::GetInstance()
{
	static CHmsScratchPadSql sql;
	return sql;
}

void CHmsScratchPadSql::InsertScratchPadInfoStu(ScratchPadInfoStu &stu)
{
	std::string strSql;
	strSql = "insert into scratchpadinfo(lineWidth,lineColor,bgType,vecPoints,vecPointsIndex) values(?,?,?,?,?)";
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return;
	}
	
	char *pBuffer = nullptr;
	int dataLen = 0;
	char *pIndex = nullptr;
	int indexLen = 0;
	VectorToCharPointer(stu.vecLineStu, &pBuffer, dataLen, &pIndex, indexLen);

	if (pBuffer && pIndex)
	{
		sqlite3_bind_int(pStmt, 1, stu.lineWidth);
		sqlite3_bind_int(pStmt, 2, Color4BToUInt(stu.lineColor));
		sqlite3_bind_int(pStmt, 3, (int)stu.bgType);
		sqlite3_bind_blob(pStmt, 4, pBuffer, dataLen, nullptr);
		sqlite3_bind_blob(pStmt, 5, pIndex, indexLen, nullptr);

		int ret = sqlite3_step(pStmt);

		delete[] pBuffer;
		pBuffer = nullptr;
		delete[] pIndex;
		pIndex = nullptr;
	}
	sqlite3_finalize(pStmt);
}

void CHmsScratchPadSql::ModScratchPadInfoStu(ScratchPadInfoStu &stu)
{
	std::string strSql;
	strSql = "update scratchpadinfo set lineWidth=?,lineColor=?,bgType=?,vecPoints=?,vecPointsIndex=? where id=?";
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return;
	}

	char *pBuffer = nullptr;
	int dataLen = 0;
	char *pIndex = nullptr;
	int indexLen = 0;
	VectorToCharPointer(stu.vecLineStu, &pBuffer, dataLen, &pIndex, indexLen);

	if ((pBuffer && pIndex) || stu.vecLineStu.empty())
	{
		sqlite3_bind_int(pStmt, 1, stu.lineWidth);
		sqlite3_bind_int(pStmt, 2, Color4BToUInt(stu.lineColor));
		sqlite3_bind_int(pStmt, 3, (int)stu.bgType);
		sqlite3_bind_blob(pStmt, 4, pBuffer, dataLen, nullptr);
		sqlite3_bind_blob(pStmt, 5, pIndex, indexLen, nullptr);
		sqlite3_bind_int(pStmt, 6, stu.id);
		int ret = sqlite3_step(pStmt);

		if (pBuffer && pIndex)
		{
			delete[] pBuffer;
			pBuffer = nullptr;
			delete[] pIndex;
			pIndex = nullptr;
		}
	}
	sqlite3_finalize(pStmt);
}

void CHmsScratchPadSql::DelScratchPadInfoStu(int id)
{
	std::string strSql;
	strSql = "delete from scratchpadinfo where id=?";
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, strSql.c_str(), -1, &pStmt, nullptr);
	if (!pStmt)
	{
		return;
	}

	sqlite3_bind_int(pStmt, 1, id);

	int ret = sqlite3_step(pStmt);

	sqlite3_finalize(pStmt);
}

std::vector<ScratchPadInfoStu> CHmsScratchPadSql::GetScratchPadInfoStu()
{
	std::vector<ScratchPadInfoStu> vecRet;
	std::stringstream s;
	s << "select id,lineWidth,lineColor,bgType,vecPoints,vecPointsIndex from scratchpadinfo";
	sqlite3_stmt * pStmt = nullptr;
	sqlite3_prepare(m_pDb, s.str().c_str(), -1, &pStmt, nullptr);
	if (pStmt)
	{
		while (SQLITE_ROW == sqlite3_step(pStmt))
		{
			ScratchPadInfoStu infoStu;
			infoStu.id = sqlite3_column_int(pStmt, 0);
			infoStu.lineWidth = sqlite3_column_int(pStmt, 1);
			infoStu.lineColor = UIntToColor4B(sqlite3_column_int(pStmt, 2));
			infoStu.bgType = (ScratchPadBgType)sqlite3_column_int(pStmt, 3);

			unsigned char * pData = (unsigned char *)sqlite3_column_blob(pStmt, 4);
			int dataLen = sqlite3_column_bytes(pStmt, 4);
			unsigned char * pIndex = (unsigned char *)sqlite3_column_blob(pStmt, 5);
			int indexLen = sqlite3_column_bytes(pStmt, 5);
			if (pIndex && pData)
			{
				std::vector<int> vecIndex(indexLen / sizeof(int));
				memcpy(vecIndex.data(), pIndex, indexLen);
				vecIndex.insert(vecIndex.begin(), -1);//为了能统一处理，插入一个值
                for (int i = 1; i < (int)vecIndex.size(); ++i)
				{
					int pointLen = vecIndex[i] - vecIndex[i - 1] - 2 * sizeof(int);
					OneLineStu stu;
					stu.vecPoints.resize(pointLen / sizeof(Vec2));
					memcpy(stu.vecPoints.data(), pData + (vecIndex[i - 1] + 1), pointLen);
					int width;
					unsigned int color;
					memcpy(&width, pData + (vecIndex[i - 1] + 1) + pointLen, sizeof(int));
					memcpy(&color, pData + (vecIndex[i - 1] + 1) + pointLen + sizeof(int), sizeof(int));
					stu.lineWidth = width;
					stu.lineColor = UIntToColor4B(color);

					infoStu.vecLineStu.push_back(stu);
				}
			}
			vecRet.push_back(infoStu);
		}
		sqlite3_finalize(pStmt);
	}
	return vecRet;
}

void CHmsScratchPadSql::VectorToCharPointer(
	const std::vector< OneLineStu > &vecLineStu,
	char **pBuffer, int &dataLen, 
	char **pIndex, int &indexLen)
{
	int len = 0;
	for (auto &stu : vecLineStu)
	{
		len += stu.vecPoints.size()*sizeof(Vec2);
		len += 2 * sizeof(int);
	}
	if (0 == len)
	{
		return;
	}
	char *pTempBuffer = new char[len]();
	if (pTempBuffer)
	{
		std::vector<int> vecIndex;
		int index = 0;
		for (auto &stu : vecLineStu)
		{
			int tempLen = stu.vecPoints.size()*sizeof(Vec2);
			memcpy(pTempBuffer + index, stu.vecPoints.data(), tempLen);

			memcpy(pTempBuffer + index + tempLen, &stu.lineWidth, sizeof(int));
			unsigned int color = Color4BToUInt(stu.lineColor);
			memcpy(pTempBuffer + index + tempLen + sizeof(int), &color, sizeof(int));
			index += tempLen + 2*sizeof(int);

			vecIndex.push_back(index - 1);
		}
		int tempIndexLen = vecIndex.size()*sizeof(int);
		char *pTempIndex = new char[tempIndexLen]();
		if (pTempIndex)
		{
			memcpy(pTempIndex, vecIndex.data(), tempIndexLen);

			*pBuffer = pTempBuffer;
			dataLen = len;
			*pIndex = pTempIndex;
			indexLen = tempIndexLen;
			return;
		}
		else
		{
			delete[] pTempBuffer;
			pTempBuffer = nullptr;
			return;
		}
	}
}

unsigned int CHmsScratchPadSql::Color4BToUInt(Color4B c)
{
	unsigned int ret = c.r << 24 | c.g << 16 | c.b << 8 | c.a;
	return ret;
}

HmsAviPf::Color4B CHmsScratchPadSql::UIntToColor4B(unsigned int c)
{
	return Color4B((c >> 24) & 0xFF, (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF);
}




