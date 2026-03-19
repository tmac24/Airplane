#include "HmsGeopackageDatabase.h"
#include <sstream>

int code_convert(const char *from_charset, const char *to_charset, const char *inbuf, size_t inlen, char *outbuf, size_t outlen);
std::string u2a(const char *inbuf);
std::string a2u(const char *inbuf);

CHmsGeopackageDatabase * CHmsGeopackageDatabase::GetInstance()
{
	static CHmsGeopackageDatabase * s_pInstance = nullptr;
	if (nullptr == s_pInstance)
	{
		s_pInstance = new CHmsGeopackageDatabase();
		//s_pInstance->InitDatabase("F:\\AVIData\\Administrative\\gadm36_CHN.gpkg");
		s_pInstance->OpenDb("F:\\AVIData\\Administrative\\test.gpkg");
	}
	return s_pInstance;
}


#include "time.h"
int CHmsGeopackageDatabase::CheckGeopackageDatabase()
{
	std::stringstream s;
	s << "CREATE VIRTUAL TABLE demo_index USING rtree(id,minX, maxX,minY, maxY,+axucol INTEGER NOT NULL);";
	auto pStmt1 = Exec(s.str(), [](int nCnt, char ** strArrayTitle, char ** strArrayContent){
		return 0; 
	});

	if (m_pSqlite)
	{
		return false;
	}

	char* errmsg;

	// 开启事物
	if (sqlite3_exec(m_pSqlite, "begin", NULL, NULL, &errmsg) != SQLITE_OK) {
		printf("%4d Error:%sn", __LINE__, errmsg);
		return false;
	}

	// 开始计时
	clock_t start = clock();

	srand(time(NULL));  // 初始化随机数种子
	sqlite3_stmt * pStmt = NULL;

	// 预处理SQL语句
	if (sqlite3_prepare_v2(m_pSqlite,
		"INSERT INTO demo_index VALUES(?,?,?,?,?,?)",
		-1, &pStmt, NULL) != SQLITE_OK) {
		printf("%4d Error:%sn", __LINE__, errmsg);
		return -3;
	}
	// 逐个插入
	for (int i = 0; i < 100000; ++i) {
		// 生成在经纬度范围内的x,y
		double x0 = ((double)rand() / (double)RAND_MAX) * 360 - 180;
		double y0 = ((double)rand() / (double)RAND_MAX) * 180 - 90;
		double x1 = x0 + 0.002 + ((double)rand() / (double)RAND_MAX)*0.2;
		double y1 = y0 + 0.002 + ((double)rand() / (double)RAND_MAX)*0.2;
		// 绑定数据
		sqlite3_bind_int64(pStmt, 1, i);
		sqlite3_bind_double(pStmt, 2, x0);
		sqlite3_bind_double(pStmt, 3, x1);
		sqlite3_bind_double(pStmt, 4, y0);
		sqlite3_bind_double(pStmt, 5, y1);
		sqlite3_bind_int(pStmt, 6, rand() % 3);
		// 执行
		sqlite3_step(pStmt);
		// 重置
		sqlite3_reset(pStmt);
	}
	sqlite3_finalize(pStmt); //结束语句，释放语句句柄

	// 结束事物
	if (sqlite3_exec(m_pSqlite, "commit", NULL, NULL, &errmsg) != SQLITE_OK){
		printf("%4d Error:%sn", __LINE__, errmsg);
		return -2;
	}


	// 结束计时
	clock_t end = clock();
	double hs = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
	printf("插入总耗时: %lf msn", hs);
	// 查询
	// select * from test where NOT(maxX<74.254915 OR minX>79.765758 OR maxY< 24.214285 OR minY>29.725129) AND auxcol==2 ORDER BY id;
	// 预处理SQL语句
	pStmt = NULL;
	if (sqlite3_prepare_v2(m_pSqlite,
		"SELECT id,minX,minY,auxcol FROM demo_index WHERE NOT(maxX<? OR minX>? OR maxY<?  OR minY>?) AND auxcol==1;",
		-1, &pStmt, NULL) != SQLITE_OK) {
		printf("%4d Error:%sn", __LINE__, errmsg);
		return -4;
	}

	//-------------------------------------------------------------------------

	// 输入查询的范围框数据
	puts("Input x0,x1,y0,y1:");
	double x0, x1, y0, y1;
	scanf("%lf,%lf,%lf,%lf", &x0, &x1, &y0, &y1);
	printf("-----------[%lf,%lf,%lf,%lf]-------------n", x0, x1, y0, y1);

	// 开始计时
	start = clock();

	// 绑定查询范围数据
	sqlite3_bind_double(pStmt, 1, x0);
	sqlite3_bind_double(pStmt, 2, x1);
	sqlite3_bind_double(pStmt, 3, y0);
	sqlite3_bind_double(pStmt, 4, y1);
	while (sqlite3_step(pStmt) == SQLITE_ROW) {
		int id = sqlite3_column_int(pStmt, 0);
		double x = sqlite3_column_double(pStmt, 1);
		double y = sqlite3_column_double(pStmt, 2);
		int auxcol = sqlite3_column_int(pStmt, 3);
		// 可以把输出去掉，减少对时间统计的影响
		printf("%dt %lf,%lfn", id, x, y);
	}
	sqlite3_reset(pStmt); // 这里只查询一次可以没有，如果需要多次使用这个查询语句，则必须有，不然查出数据不对
	sqlite3_finalize(pStmt); //结束语句，释放语句句柄

	// 结束计时
	end = clock();
	hs = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
	printf("本次查询总耗时: %lf msn", hs);

	return true;
}

CHmsGeopackageDatabase::CHmsGeopackageDatabase()
{

}


CHmsGeopackageDatabase::~CHmsGeopackageDatabase()
{
	Close();
}

