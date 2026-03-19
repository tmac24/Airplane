#pragma once
#include "HmsDatabase.h"
#include "HmsGeomDef.h"

class CHmsGeopackageDatabase : public CHmsDatabase
{
public:
	CHmsGeopackageDatabase();
	~CHmsGeopackageDatabase();

	static CHmsGeopackageDatabase * GetInstance();

	int CheckGeopackageDatabase();

};

