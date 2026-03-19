#pragma once
#include "HmsDatabase.h"
#include "HmsGeomDef.h"

struct ShapeRange
{
	int nMinGx;
	int nMinGy;
	int nMaxGx;
	int nMaxGy;

	void SetData(double dLeft, double dRight, double dBottom, double dTop, const HmsGeomColumns & geomColums);
};

class CHmsAdminAreaDatabase : public CHmsDatabase
{
public:
	CHmsAdminAreaDatabase();
	~CHmsAdminAreaDatabase();

	bool ResetDatabase(const std::string & strPath);

	bool IsGDBDatabase();

	bool FillAdminAreasDescribe(HmsAdminAreaDescribe & describe);

	bool GetGeomColumns(std::vector<HmsGeomColumns> & vGeomGolumn);

    bool GetChinaLandAdmin(std::vector<HmsGeomData> & vGeomData);

	bool GetGeomData(const char * strTable, const char * strNameCol, std::vector<HmsGeomData> & vGeomData);
	bool GetGeomData(int nAdmIndex, const char * strNameCol, std::vector<HmsGeomData> & vGeomData);

	bool GetGeomDataById(const char * strTable, const char * strNameCol, int objectId, HmsGeomData & geomData);
	bool GetGeomDataById(int nAdmIndex, const char * strNameCol, int objectId, HmsGeomData & geomData);

	bool GetRangeGeomData(const char * strTable, const char * strNameCol, const ShapeRange & shapeRange, std::vector<HmsGeomData*> & vGeomData);

	void SetMultUsedIndex(unsigned int nIndex){ m_nMultUsedIndex = nIndex; }

	std::string TrimMultiData(const std::string & strData);

private:
	unsigned int	m_nMultUsedIndex;
};

