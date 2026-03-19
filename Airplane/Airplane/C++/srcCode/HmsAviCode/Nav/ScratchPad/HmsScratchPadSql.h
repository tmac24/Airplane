#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "base/HmsFileUtils.h"

#ifdef _WIN32
#include "sqlite3/include/sqlite3.h"
#pragma comment(lib,"sqlite3.lib")  
#endif

#ifdef __vxworks
#include "sqlite3/include/sqlite3.h"
#endif

#ifdef __linux
#include "sqlite3.h"
#endif

#ifdef __APPLE__
#include "sqlite3.h"
#endif

using namespace HmsAviPf;

#include "HmsScratchPadAddDlg.h"

struct OneLineStu
{
	int lineWidth;
	Color4B lineColor;
	std::vector<Vec2> vecPoints;
#if _NAV_SVS_LOGIC_JUDGE
	OneLineStu()
	{
		lineWidth = 12;
		lineColor = Color4B::WHITE;
	}
#endif
};

struct ScratchPadInfoStu
{
	int id;
	int lineWidth;
	Color4B lineColor;
	ScratchPadBgType bgType;
	std::vector< OneLineStu > vecLineStu;

	ScratchPadInfoStu() :id(-1), lineWidth(2), lineColor(Color4B::WHITE),bgType(ScratchPadBgType::TYPE_BLANK)
	{}
};

class CHmsScratchPadSql
{
public:
	~CHmsScratchPadSql();

	static CHmsScratchPadSql& GetInstance();

	void InsertScratchPadInfoStu(ScratchPadInfoStu &stu);

	void ModScratchPadInfoStu(ScratchPadInfoStu &stu);

	void DelScratchPadInfoStu(int id);

	std::vector<ScratchPadInfoStu> GetScratchPadInfoStu();

private:
	CHmsScratchPadSql();
	CHmsScratchPadSql(const CHmsScratchPadSql&);

	void VectorToCharPointer(
		const std::vector< OneLineStu > &vecLineStu,
		char **pBuffer, int &dataLen,
		char **pIndex, int &indexLen);

	unsigned int Color4BToUInt(Color4B c);
	Color4B UIntToColor4B(unsigned int c);

private:
	sqlite3 *m_pDb;
};


