#ifndef _ZS_PT_POSITION_DEF_H____
#define _ZS_PT_POSITION_DEF_H____

struct  EarthCoord2D
{
	double dLongitude;
    double dLatitude;

	EarthCoord2D();
	EarthCoord2D(double dLat, double dLong);

	void SetData(double dLat, double dLong);

	/************************************************************************
	函数名称：	double GetAngle(const EarthCoord2D & destEc);
	函数功能：	获取目标地，相对于当前地点的角度
	函数参数：	destEc		[in]		目标地
	返回值：	方向角（角度）
	作者：		ZS	
	修改日期：	<2013:10:31 16>   
	************************************************************************/
	double GetAngle(const EarthCoord2D & destEc) const;

	/************************************************************************
	函数名称：	double GetRandian(const EarthCoord2D & destEc);
	函数功能：	获取目标地，相对于当前地点的弧度
	函数参数：	destEc		[in]		目标地
	返回值：	方向角（弧度）
	作者：		ZS	
	修改日期：	<2013:10:31 16>   
	************************************************************************/
	double GetRandian(const EarthCoord2D & destEc) const;

	/************************************************************************
	函数名称：	double GetDistanceKm(const EarthCoord2D & destEc);
	函数功能：	获取目标地，相对于当前地点的距离
	函数参数：	destEc		[in]		目标地
	返回值：	距离（公里）
	作者：		ZS	
	修改日期：	<2013:10:31 16>   
	************************************************************************/
	double GetDistanceKm(const EarthCoord2D & destEc) const;

	/************************************************************************
	函数名称：	double GetDistanceNm(const EarthCoord2D & destEc);
	函数功能：	获取目标地，相对于当前地点的距离
	函数参数：	destEc		[in]		目标地
	返回值：	距离（海里）
	作者：		ZS	
	修改日期：	<2013:10:31 16>   
	************************************************************************/
	double GetDistanceNm(const EarthCoord2D & destEc) const;

	/************************************************************************
	函数名称：	EarthCoord2D GetPoistion(double dAngle, double dDistanceNm);
	函数功能：	获取相对与当前点，指定方向上、固定距离的点
	函数参数：	dAngle		[in]		角度
				dDistanceNm	[in]		距离
	返回值：	目标点
	作者：		ZS	
	修改日期：	<2013:10:31 17>   
	************************************************************************/
	EarthCoord2D GetPoistion(double dAngle, double dDistanceNm) const;

	/************************************************************************
	函数名称：	EarthCoord2D GetPoistion(const EarthCoord2D & directiveEc, double dDistanceRate);
	函数功能：	以当前点和指向点为标尺，求相应比例尺的位置
	函数参数：	dest			[in]		指向点
				dDistanceRate	[in]		到目标点的比率 
	返回值：	目标点
	作者：		ZS	
	修改日期：	<2013:10:31 17>   
	************************************************************************/
	EarthCoord2D GetPoistion(const EarthCoord2D & directiveEc, double dDistanceRate) const;

	/************************************************************************
	函数名称：	EarthCoord2D GetCrossPointByCircle(double dCurDirection, const EarthCoord2D & originEc, 
						double dRadiusNm, bool * pbResult = NULL);
	函数功能：	获取当前位置在指定方向上的，与指定圆的交点
	函数参数：	dCurDirection		[in]		当前点方向
				originEc			[in]		圆心位置
				dRadiusNm			[in]		圆心半径
				pbResult			[out]		返回结果是否确定位置	true：yes  false：is just one answer maybe failed
	返回值：	交点位置
	作者：		ZS	
	修改日期：	<2013:10:31 17>   
	************************************************************************/
	EarthCoord2D GetCrossPointByCircle(double dCurDirection, const EarthCoord2D & originEc, double dRadiusNm, bool * pbResult = 0) const;

	/************************************************************************
	函数名称：	EarthCoord2D GetCrossPointByVector(double dCurDirection, const EarthCoord2D & vectorEc, 
							double dVectorDirection, bool * pbResult = NULL);
	函数功能：	获取当前位置在指定方向上的，与指定点指定方向的交点
	函数参数：	dCurDirection		[in]		当前点方向
				vectorEc			[in]		方向向量起点
				dVectorDirection	[in]		方向向量方向
				pbResult			[out]		返回结果是否确定位置	true：yes  false：is just one answer maybe failed
	返回值：	交点位置
	作者：		ZS	
	修改日期：	<2013:10:31 17>   
	************************************************************************/
	EarthCoord2D GetCrossPointByVector(double dCurDirection, const EarthCoord2D & vectorEc, double dVectorDirection, bool * pbResult = 0) const;


	void Clean();
};

struct  EarthCoord3D : public EarthCoord2D
{
// 	double dLatitude;
// 	double dLongitude;
	double dAltitude;

	EarthCoord3D();
	EarthCoord3D(double dLat, double dLong, double dAlt);

	void Clean();
};

#endif