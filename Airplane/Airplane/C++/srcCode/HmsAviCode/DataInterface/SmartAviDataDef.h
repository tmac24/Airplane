#pragma once

#include <string.h>
#include <string>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

struct InsGnssState
{
	InsGnssState()
	{
		initDataState = 0;
		adcDataState = 0;
		gnssState = 0;
		windState = 0;
		insState = 0;
		navState = 0;
		attitudeState = 0;
		installState = 0;
		reserve = 0;
	}

	uchar initDataState : 1;			//初始数据状态 0未收到出事数据 1收到初始数据
	uchar adcDataState : 1;				//大气数据状态 0未收到大气数据 1收到大气数据
	uchar gnssState : 2;				//GNSS状态     0未收到GNSS数据 1GNSS数据无效 2数据有效
	uchar windState : 1;				//风向风速状态 0数据无效 1数据有效
	uchar insState : 3;					//INS系统状态  0准备 1对准 2INS/GNS组合 3INS/ADS组合 4INS存惯导
	uchar navState : 1;					//导航数据有效性 0数据无效 1数据有效
	uchar attitudeState : 1;			//姿态数据有效性 0数据无效 1数据有效
	uchar installState : 1;				//安装误差修正有效性 0未修正 1已修正
	uchar reserve : 5;					//保留 置零
};

struct InsGnssError
{
	InsGnssError()
	{
		powerErr = 0;
		imuErr = 0;
		xGyroErr = 0;
		yGyroErr = 0;
		zGyroErr = 0;
		xAccErr = 0;
		yAccErr = 0;
		zAccErr = 0;
		alignmentErr = 0;
		gnssRecErr = 0;
		gnssAntennaError = 0;
	}

	uchar powerErr : 1;					//电源故障 1故障 0正常
	uchar imuErr : 1;					//IMU通信故障 1故障 0正常
	uchar xGyroErr : 1;					//X轴陀螺仪故障
	uchar yGyroErr : 1;					//Y轴陀螺仪故障
	uchar zGyroErr : 1;					//Z轴陀螺仪故障
	uchar xAccErr : 1;					//X轴加速度故障
	uchar yAccErr : 1;					//Y轴加速度故障
	uchar zAccErr : 1;					//Z轴加速度故障
	uchar alignmentErr : 1;				//对准不通过 1故障 0正常
	uchar gnssRecErr : 1;				//GNSS接收机故障
	uchar gnssAntennaError : 1;			//GNSS天线故障

};

/*INS/GNSS 转换后给UI的惯导数据*/
struct InsGnssData
{
	InsGnssData()
	{
		valid = false;
		sysTime = 0;
		gpsWeek = 0;
		gpsTime = 0;
		gnssLng = 0;
		gnssLat = 0;
		gnssHeight = 0;
		gnssEastSpeed = 0;
		gnssNorthSpeed = 0;
		gnssSkySpeed = 0;
		vdop = 0;
		hdop = 0;
		gpsPositionStar = 0;
		dbPositionStar = 0;
		Lng = 0;
		Lat = 0;
		Height = 0;
		eastSpeed = 0;
		northSpeed = 0;
		skySpeed = 0;
		axialAcceleration = 0;
		lateralAcceleration = 0;
		normalAcceleration = 0;
		yaw = 0;
		pitch = 0;
		roll = 0;
		yawSpeed = 0;
		pitchSpeed = 0;
		rollSpeed = 0;
		windSpeed = 0;
		windDirection = 0;
		gnssGroundSpeed = 0;
		tracks = 0;
		yawErrorCorrection = 0;
		pitchErrorCorrection = 0;
		rollErrorCorrection = 0;
		softwareVersion = "";
	}

	bool  valid;					//数据有效性 0无效 1有效
	float sysTime;					//系统工作时间,lsb 0.01
	float gpsWeek;					//GPSWeek LSB=1
	float gpsTime;					//GPSTime LSB=0.01
	double gnssLng;					//gnss经度
	double gnssLat;					//gnss纬度
	float gnssHeight;				//gnss高度
	float gnssEastSpeed;			//GNSS东向速度
	float gnssNorthSpeed;			//GNSS北向速度
	float gnssSkySpeed;				//GNSS天向速度
	float vdop;						//VDOP
	float hdop;						//HDOP
	float gpsPositionStar;			//GPS定位星数
	float dbPositionStar;			//BD定位星数
	double Lng;						//经过融合后经度
	double Lat;						//纬度
	float Height;					//高度
	float eastSpeed;				//东向速度
	float northSpeed;				//北向速度
	float skySpeed;					//天向速度
	float axialAcceleration;		//轴向加速度	单位：m/s2
	float lateralAcceleration; 		//侧向加速度
	float normalAcceleration;		//法向加速度
	float yaw;						//偏航角
	float pitch;					//俯仰角
	float roll;						//横滚角
	float yawSpeed;					//偏航角速度
	float pitchSpeed;				//俯仰角速度
	float rollSpeed;				//横滚角速度
	float windSpeed;				//风速
	float windDirection; 			//风向
	float gnssGroundSpeed;			//gnss地速		单位：节
	float tracks;					//gnss航迹角
	float yawErrorCorrection;		//航向误差修正值
	float pitchErrorCorrection;		//俯仰误差修正值
	float rollErrorCorrection;		//横滚误差修正值
	InsGnssState insGnssState;		//惯导/卫星系统状态
	InsGnssError sysError;			//系统故障字
	std::string softwareVersion;	//软件版本
};

struct AdcValid
{
	AdcValid()
	{
		aosValid = 0;
		aoaValid = 0;
		stdasValid = 0;
		tasValid = 0;
		machValid = 0;
		baroHeightValid = 0;
		TsValid = 0;
		hprValid = 0;
		ttValid = 0;
		hqneValid = 0;
		psValid = 0;
		ptValid = 0;
		stdBaroValid = 0;
		reserve = 0;
	}

	uchar aosValid : 1;
	uchar aoaValid : 1;
	uchar stdasValid : 1;
	uchar tasValid : 1;
	uchar machValid : 1;
	uchar baroHeightValid : 1;
	uchar TsValid : 1;
	uchar hprValid : 1;
	uchar ttValid : 1;
	uchar hqneValid : 1;
	uchar psValid : 1;
	uchar ptValid : 1;
	uchar stdBaroValid : 1;
	uchar reserve : 3;
};

//ADC故障
struct AdcError
{
	AdcError()
	{
		psErr = 0;
		ptErr = 0;
		tsErr = 0;
		powerErr = 0;
		pttVolErr = 0;
		ptCycleErr = 0;
		pstVolErr = 0;
		psCycleErr = 0;
		ptFactorErr = 0;
		psFactorErr = 0;
		aoaErr = 0;
		aosErr = 0;
		adcErr = 0;
		stdPreErr = 0;
		reserve = 0;
	}

	uchar psErr : 1;				//static pressure静压传感器故障
	uchar ptErr : 1;				//total pressure总压传感器故障
	uchar tsErr : 1;				//static temperature静温传感器故障
	uchar powerErr : 1;				//电源故障
	uchar pttVolErr : 1;			//total pressure temperature voltage总压温度电压故障
	uchar ptCycleErr : 1;			//总压周期故障
	uchar pstVolErr : 1;			//static pressure temperature voltage静压温度电压故障
	uchar psCycleErr : 1;			//静压周期故障
	uchar ptFactorErr : 1;			//总压传感器特征系数故障
	uchar psFactorErr : 1;			//静压传感器特征系数故障
	uchar aoaErr : 1;				//angle of attack 攻角故障
	uchar aosErr : 1;				//angle of sideslip 侧滑角故障
	uchar adcErr : 1;				//大气数据计算机故障
	uchar stdPreErr : 1;			//基准气压设定故障
	uchar reserve : 2;				//保留 置零
};

//大气数据
struct AdcData
{
	AdcData()
	{
		valid = false;
		aos = 0;
		aoa = 0;
		Vc = 0;
		Vt = 0;
		mach = 0;
		baroHeight = 0;
		Ts = 0;
		hpr = 0;
		Tt = 0;
		Hqne = 0;
		Ps = 0;
		Pt = 0;
		baroSet = 0;
		stdBaro = 0;
		softwareVersion = "";
	}

	bool  valid;					//数据有效性 0无效 1有效
	float aos;						//侧滑角AOS
	float aoa;						//攻角AOA
	float Vc;						//校准空速 单位节
	float Vt;						//真空速 单位节
	float mach;						//马赫数
	float baroHeight;				//显示气压高度H 单位Feet
	float Ts;						//静温对应机外温度（OAT）
	float hpr;						//升降速度HPR 单位Feet/Minute
	float Tt;						//总温Tt
	float Hqne;						//标准气压高度HQNE
	float Ps;						//静压PS
	float Pt; 						//全压Pt
	float baroSet;					//气压设定值 inHg
	uchar stdBaro;					//气压基准 0QNE标准气压 1QNH修正海平面气压 2QFE场面气压
	AdcValid dataValidity;			//数据有效性
	AdcError sysError;				//系统故障
	std::string softwareVersion;	//软件版本
};


//VHF数据 对应 COM
struct VhfData
{
	VhfData()
	{
		valid = false;
		workMode = 0;
		modulationType = 0;
		workFreq = 0.0;
		freqInterval = 0;
		muteNoiseStatus = 0;
		selcal = "";
		pttStatus = 0;
		stationState = 0;
	}
	bool valid;				//数据有效性 0无效 1有效
	uchar stationState;			//电台状态 0不在线 1在线
	uchar workMode;				//工作模式 0正常 1应急
	uchar modulationType;	//调制方式 0调幅 AM 1调频 FM
	float workFreq;				//工作频率
	uchar freqInterval;			//频率间隔 1 AM25000Hz 2 AM8330Hz 3 FM25000Hz
	uchar muteNoiseStatus;		//静噪状态 0-5级
	std::string selcal;			//选呼码
	uchar pttStatus;			//PTT状态 0无效 1有效
};

//ADF数据
struct AdfData
{
	AdfData()
	{
		valid = false;
		stationState = 0;
		workState = 0;
		azimuthValid = 0;
		workFreq = 0.0;
		azimuth = 0.0;
		ndbCode = "";
	}

	bool valid;				//数据有效性 0无效 1有效
	uchar stationState;			//电台状态 0不在线 1在线
	uchar workState;			//工作状态 0定向ADF 1收讯ANT
	uchar azimuthValid;		//方向数据有效性 0无效 1有效
	float workFreq;				//工作频率 190-1799.0单位kHz
	float azimuth;				//方位角 0-360 单位°
	std::string ndbCode;		//ADF站台识别码 
};

//DME数据
struct DmeData
{
	DmeData()
	{
		valid = false;
		stationState = 0;
		rsMode = 0;
		xyMode = 0;
		holdState = 0;
		distanceValid = 0;
		gsValid = 0;
		timeValid = 0;
		lockInState = 0;
		lockOutState = 0;
		dmeChannel = 0.0;
		distance = 0.0;
		groundSpeed = 0.0;
		time = 0.0;
		dmeCode = "";
	}

	bool valid;					//数据有效性 0无效 1有效
	uchar stationState;			//电台状态 0不在线 1在线
	uchar rsMode;				//收发模式 0standby 1R/S（收发模式）
	uchar xyMode;				//X/Y模式 0X模式 1Y模式
	uchar holdState;			//HOLD状态 0无效 1有效
	uchar distanceValid;		//距离数据有效性 0无效 1有效
	uchar gsValid;				//地速数据有效性 0无效 1有效
	uchar timeValid;			//到台时间有效性 0无效 1有效
	uchar lockInState;			//闭锁输入状态 0不闭锁 1闭锁
	uchar lockOutState;			//闭锁输出状态 0不闭锁 1闭锁
	ushort dmeChannel;			//DME当前波道号
	float distance;				//DME距离 单位海里 nm
	float groundSpeed;			//DME地速 0-7400 单位KM/H
	float time;					//到台时间 0-65535 单位秒
	std::string dmeCode;		//台站识别码
};

//VORILS数据 对应NAV
struct VorilsData
{
	VorilsData()
	{
		valid = false;
		stationState = 0;
		mmrMode = 0;
		azimuthValid = 0;
		locDeviationValid = 0;
		mkbSensitivity = 0;
		omBeaconValid = 0;
		mmBeaconValid = 0;
		imBeaconValid = 0;
		gsDeviationValid = 0;
		workFreq = 0.0;
		azimuth = 0.0;
		locDeviation = 0.0;
		gsDeviation = 0.0;
		mmrCode = "";
	}

	bool valid;					//数据有效性 0无效 1有效
	uchar stationState;			//电台状态 0不在线 1在线
	uchar mmrMode;				//MMR工作模式 0 VOR模式 1 ILS模式
	uchar azimuthValid;			//VOR方位数据有效性 0 无效 1有效
	uchar locDeviationValid;	//航向道偏差数据有效性 0无效 1有效
	uchar mkbSensitivity;		//MKB灵敏度 0High 1Low
	uchar omBeaconValid;		//远指点信标有效性 0无效 1有效   蓝色显示
	uchar mmBeaconValid;		//中指点信标有效性 0无效 1有效   琥珀色显示
	uchar imBeaconValid;		//近指点信标有效性 0无效 1有效   白色
	uchar gsDeviationValid;		//下滑道偏差数据有效性 0无效 1有效
	float workFreq;				//工作频率
	float azimuth;				//VOR方位角
	float locDeviation;			//LOC偏差	范围-2~2
	float gsDeviation;			//GS偏差	范围-0.7~0.7
	std::string mmrCode;		//MMR台站识别码
};

//XPDR数据
struct XpdrData
{
	XpdrData()
	{
		valid = false;
		stationState = 0;
		workMode = 0;
		vfrState = 0;
		ssrState = 0;
		identState = 0;
		sgState = 0;
		lockInState = 0;
		lockOutState = 0;
		ssrCode = "";
	}

	bool  valid;				//数据有效性 0无效 1有效
	uchar stationState;			//电台状态 0不在线 1在线
	uchar workMode;				//工作模式 0STANDBY 1ALT 2ON
	uchar vfrState;				//VFR状态 0退出VFR状态 1进入VFR状态
	uchar ssrState;				//SSR代码状态 0禁止输入 1SSR代码输入（STBY下）
	uchar identState;			//IDENT 0无效 1有效
	uchar sgState;				//空/地状态 0地面状态 1空中状态
	uchar lockInState;			//闭锁输入状态 0不闭锁 1闭锁
	uchar lockOutState;			//闭锁输出状态 0不闭锁 1闭锁
	std::string ssrCode;		//SSR回传编码

};

//无线电高度数据
struct RadioAltData
{
	RadioAltData()
	{
		valid = false;
		stationState = 0;
		workMode = 0;
		altitude = 0.0;
	}

	bool  valid;				//数据有效性 0无效 1有效
	uchar stationState;			//电台状态 0不在线 1在线
	uchar altValid;				//高度数据有效性 0无效 1有效
	uchar workMode;				//工作模式 0搜索模式 1跟踪模式
	float altitude;				//无线电高度 单位英尺 Feet
};


//轮载信号
struct Wow
{
	Wow()
	{
		font = 0;
		left = 0;
		right = 0;
		reserve = 0;
	}
	uchar font : 1;		//前轮轮载信号 0前轮无轮载 1前轮有轮载
	uchar left : 1;		//左轮轮载信号 0左轮无轮载 1左轮有轮载	
	uchar right : 1;	//右轮轮载信号 0右轮无轮载 1右轮有轮载
	uchar reserve : 5;	//保留 置零
};
//起落架状态
struct Undercart
{
	Undercart()
	{
		valid = false;
		font = 0;
		left = 0;
		right = 0;
		reserve = 0;
	}

	bool  valid;		//数据有效性
	uchar font : 1;		//前轮状态 0前轮收起 1前轮放下
	uchar left : 1;		//左轮状态 0左轮收起 1左轮放下	
	uchar right : 1;	//右轮状态 0右轮收起 1右轮放下
	uchar reserve : 5;	//保留 置零
};

//轮载襟翼数据
struct WowWingData
{
	WowWingData()
	{
		valid = 0;
		wingPos = 0;
	}

	bool  valid;				//数据有效性
	Wow wow;					//轮载信号
	Undercart undercart;		//起落架状态
	float wingPos;				//襟翼位置
};

struct DiuErr
{
	DiuErr()
	{
		adc1CommErr = 0;
		adc2CommErr = 0;
		insGnss1CommErr = 0;
		insGnss2CommErr = 0;
		engCommErr = 0;
		radioWarnCommErr = 0;
		radioTuneCommErr = 0;
		altCommErr = 0;
		du1CommErr = 0;
		du2CommErr = 0;
		hudCommErr = 0;
		powerErr = 0;
		card1Err = 0;
		card2Err = 0;
		rtcErr = 0;
		diuErr = 0;
		reserve1 = 0;
		reserve2 = 0;
	}

	uchar adc1CommErr : 1;		//ADC_1接收通信故障
	uchar adc2CommErr : 1;		//ADC_2接收通信故障
	uchar insGnss1CommErr : 1;	//INS/Gnss-1接收通信故障
	uchar insGnss2CommErr : 1;	//INS/Gnss-2接收通信故障
	uchar engCommErr : 1;		//发动机参数采集器接收通信故障
	uchar radioWarnCommErr : 1;	//无线电告警接收通信故障
	uchar radioTuneCommErr : 1;	//无线电调谐接收通信故障
	uchar altCommErr : 1;		//高度表（备份）接收通信故障
	uchar du1CommErr : 1;		//DU1以太网通信故障
	uchar du2CommErr : 1;		//DU2以太网通信故障
	uchar hudCommErr : 1;		//HUD通信故障
	uchar powerErr : 1;			//电源故障
	uchar card1Err : 1;			//板卡1故障
	uchar card2Err : 1;			//板卡2故障
	uchar rtcErr : 1;			//RTC故障
	uchar diuErr : 1;			//DIU故障
	uchar reserve1;				//保留 置零
	uchar reserve2;				//保留 置零
};

//DIU数据
struct DiuData
{
	DiuData()
	{
		valid = false;
		diuVersion = "";
		fpgaVersion = "";
	}
	bool  valid;				//数据有效性
	DiuErr diu1Err;				//DIU1故障
	DiuErr diu2Err;				//DIU2故障
	std::string diuVersion;		//DIU版本
	std::string fpgaVersion;	//FPGA版本
};

//DCT 转换后的发动机数据
struct DctData
{
	DctData()
	{
		valid = false;
		warnCode = 0;
		space = 0;
		vibrate = 0;
		oat = 0;
		N1 = 0;
		BUS_Voltage = 0;
		LHPUMP_ON = 0;
		CHIP_R = 0;
		PROP = 0;
		Amps = 0;
		MAN_CTRL = 0;
		Feather = 0;
		R_Wing = 0;
		Elevator = 0;
		PWR = 0;
		OIL_T = 0;
		L_Wing = 0;
		Aileron = 0;
		RHPUMP_ON = 0;
		EGT = 0;
		CENTER = 0;
		RUDDER = 0;
		PWR_RETARD = 0;
		RUN = 0;
		PL = 0;
		OIL_P = 0;
		F_PRESS = 0;
		CHIP_T = 0;
		REVERSE = 0;
		FUEL_P = 0;
		Ref_Diode = 0;
		FUEL_LOW = 0;
		BOOST_ENG = 0;
		TORQ = 0;
		Fuel_Flow = 0;
	}

	bool  valid;		//数据有效性
	uchar warnCode;			//告警码
	uchar space;			//预留
	uint vibrate;			//Vibration
	float oat;				//oat		LSB=0.1		UNIT=摄氏度
	float N1;				//N1		LSB=0.1		UNIT=%
	float BUS_Voltage;		//VOLTS		LSB=0.1		UNIT=V
	uint LHPUMP_ON;			//LHPUMP_ON
	uint CHIP_R;			//CHIP_R	
	float PROP;				//PROP		LSB=1		UNIT=RPM
	float Amps;				//AMPS		LSB=1		UNIT=A
	uint MAN_CTRL;			//MAN CTRL
	uint Feather;			//FEATHER
	float R_Wing;			//R_WING	LSB=0.1		UNIT=GAL
	float Elevator;			//ELEVATOR	LSB=0.01	UNIT=DEG
	float PWR;				//PWR		LSB=1		UNIT=HP
	float OIL_T;			//OIL_T		LSB=1		UNIT=摄氏度
	float L_Wing;			//L_WING	LSB=0.1		UNIT=GAL
	float Aileron;			//AILERON	LSB=0.01	UNIT=DEG
	uint RHPUMP_ON;			//R_PUM_ON
	float EGT;				//EGT		LSB=1		UNIT=摄氏度
	float CENTER;			//CENTER	LSB=0.1		UNIT=GAL
	float RUDDER;			//RUDDER	LSB=0.01	UNIT=DEG
	uint PWR_RETARD;		//PWR_RETARD
	uint RUN;				//RUN
	float PL;				//PL		LSB=1		UNIT=DEG
	float OIL_P;			//OIL_P		LSB=0.001	UNIT=BAR
	uint F_PRESS;			//F_PRESS
	uint CHIP_T;			//CHIP_T	
	uint REVERSE;			//REV_OPEN
	float FUEL_P;			//FUEL_P	LSB=0.001	UNIT=BAR
	uint Ref_Diode;			//n.a.
	uint FUEL_LOW;			//FUEL_LOW
	uint BOOST_ENG;			//BOOST_ENG	
	float TORQ;				//TORQ		LSB=0.1		UNIT=%
	float Fuel_Flow;		//F.FLOW	LSB=1		UNIT=GAL/H
};

//DU数据
struct DuData
{
	DuData()
	{
		version = "";
	}

	std::string version;
};

struct HudErr
{
	HudErr()
	{
		enet1CommErr = 0;
		enet2CommErr = 0;
		diu422_1CommErr = 0;
		diu422_2CommErr = 0;
		ctvs422CommErr = 0;
		imgSrc485CommErr = 0;
		envLightCommErr = 0;
		outVedioErr = 0;
		power28Err = 0;
		power5Err = 0;
		imgSrcTempErr = 0;
		imgSrcScreenErr = 0;
		imgSrcBackLightErr = 0;
		imgSrcHeatErr = 0;
		reserve1 = 0;
		reserve2 = 0;
		initDataEnsure = 0;
		reserve3 = 0;

	}

	uchar enet1CommErr : 1;				//以太网1通信故障 1故障 0正常
	uchar enet2CommErr : 1;				//以太网1通信故障 1故障 0正常
	uchar diu422_1CommErr : 1;			//和DIU的422通道1通信故障 1故障 0正常
	uchar diu422_2CommErr : 1;			//和DIU的422通道2通信故障 1故障 0正常
	uchar ctvs422CommErr : 1;			//和CTVS的422通道1通信故障 1故障 0正常
	uchar imgSrc485CommErr : 1;			//和像源的485通信故障 1故障 0正常
	uchar envLightCommErr : 1;			//环境光亮度信息采集故障 1故障 0正常
	uchar outVedioErr : 1;				//外视频输入采集故障 1故障 0正常
	uchar power28Err : 1;				//电源模块28V稳压电源输出故障 1故障 0正常
	uchar power5Err : 1;				//电源模块5V电源输出故障 1故障 0正常
	uchar imgSrcTempErr : 1;			//像源超温（大于95℃） 1故障 0正常
	uchar imgSrcScreenErr : 1;			//像源屏状态故障 1故障 0正常
	uchar imgSrcBackLightErr : 1;		//像源背光故障 1故障 0正常
	uchar imgSrcHeatErr : 1;			//像源加热故障 1故障 0正常
	uchar reserve1 : 2;					//保留位 2位 置零
	uchar reserve2;						//保留位 8位 置零
	uchar initDataEnsure : 1;			//初始化数据确认 1故障 0正常
	uchar reserve3 : 7;					//保留位 7位 置零

};

//HUD故障数据
struct HudData
{
	HudData()
	{
		valid = 0;
		reserve4 = 0;
	}
	bool valid;
	HudErr hudErr;
	uint reserve4;						//保留位 32位 置零
};

//机通数据 对应音频面板
struct MavolData
{
	MavolData()
	{
		valid = false;
		insCode = 0;
		vhf1Channel = 0;
		vhf2Channel = 0;
		interComChannel = 0;
		vhf1Selcal = 0;
		vhf2Selcal = 0;
		vhf1Volume = 0;
		vhf2Volume = 0;
		adfVolume = 0;
		interComVolume = 0;
		warnVolume = 0;
		mbVolume = 0;
		dmeVolume = 0;
		vorilsVolume = 0;
		voxLevel = 0;
	}

	bool  valid;				//数据有效性
	uchar insCode;				//指令码 0x0A前舱 0x0B后舱
	uchar vhf1Channel;			//VHF1发话通道 0关闭 1选中
	uchar vhf2Channel;			//VHF2发话通道 0关闭 1选中
	uchar interComChannel;		//内话通道 0关闭 1选中
	uchar vhf1Selcal;			//VHF1 SELCAL有效性 0无效 1呼叫
	uchar vhf2Selcal;			//VHF2 SELCAL有效性 0无效 1呼叫
	uchar vhf1Volume;			//VHF1音量大小 0-9级
	uchar vhf2Volume;			//VHF1音量大小 0-9级
	uchar adfVolume;			//VHF1音量大小 0-9级
	uchar interComVolume;		//VHF2音量大小 0-9级
	uchar warnVolume;			//WARN音量大小 0-9级
	uchar mbVolume;				//MB音量大小 0-9级
	uchar dmeVolume;			//DME音量大小 0-9级
	uchar vorilsVolume;			//VOR/ILS音量大小 0-9级
	uchar voxLevel;				//静噪等级 0-9级
};

struct SettingDataValid
{
	SettingDataValid()
	{
		selectAirSpeedValid = 0;
		selectHeightValid = 0;
		selectHeadingValid = 0;
		selectCourse1Valid = 0;
		selectCourse2Valid = 0;
		baroSetValid = 0;
		stallValid = 0;
		courseDeviationValid = 0;
		decisionHeightValid = 0;
		space = 0;
	}
	uchar selectAirSpeedValid : 1;
	uchar selectHeightValid : 1;
	uchar selectHeadingValid : 1;
	uchar selectCourse1Valid : 1;
	uchar selectCourse2Valid : 1;
	uchar baroSetValid : 1;
	uchar stallValid : 1;
	uchar courseDeviationValid : 1;
	uchar decisionHeightValid : 1;
	uchar space : 7;
};
//选择设定数据
struct SettingData
{
	SettingData()
	{
		fromOrTo = 0;
		baroUnit = 0;
		metricState = 0;
		tempUnit = 0;
		selectAirSpeed = 0.0f;
		selectHeight = 0.0f;
		selectHeading = 0.0f;
		selectCourse1 = 0.0f;
		selectCourse2 = 0.0f;
		baroSet = 0.0f;
		baroRef = 0;
		approachMode = 0;
		ctrlMode = 0;
		flightPhase = 0;
		stall = 0.0f;
		courseDeviation = 0.0f;
		decisionAltitude = 0.0f;
		memset(callId, 0, 8);
		space = 0;
	}
	
	SettingDataValid dataValid;	//数据有效性
	uchar fromOrTo;				//向背台飞行标识 0x01向台 0x02背台
	uchar baroUnit;				//气压单位 0x01 Hg   0x02 KPa
	uchar metricState;			//米制高度激活/禁用 0x01禁用 0x02激活
	uchar tempUnit;				//温度单位设定 0x01 ℃   0x02 ℉
	float selectAirSpeed;		//选择空速 单位km/h
	float selectHeight;			//选择高度 单位米
	float selectHeading;		//选择航向 单位度
	float selectCourse1;		//选择航路1 单位度
	float selectCourse2;		//选择航路2 单位度
	float baroSet;				//气压设定 单位英寸汞柱 inHg
	uchar baroRef;				//气压设定基准 0x01 STD  0x02 QNH  0x03 QFE
	uchar approachMode;			//进近方式  0x01精密进近 0x02非精密进近
	uchar ctrlMode;				//控制模式设定 0x01 NORM  0x02 Text   0x03  FPM
	uchar flightPhase;			//飞行阶段  0x01 NAV阶段 0x02 ILS阶段
	float stall;				//失速速度 单位km/h
	float courseDeviation;		//航道偏离 单位度
	float decisionAltitude;		//决断高度 单位米
	char callId[8];				//呼号	8个字符
	uchar space;				//备用  
};

struct BaroSettingData
{
	BaroSettingData()
	{
		baroSet = 0.0f;
		baroRef = 0;
		dataValid = 0;
		space = 0;
	}

	float baroSet;				//气压拨正值 单位inHg
	uchar baroRef;				//气压设定基准 0x00 QNE 0x01 QNH  0x02  QFE
	uchar dataValid;			//高度表拨正值有效性
	uchar space;				//备用
};

//告警数据
struct DuWarningData
{
    DuWarningData()
    {
        warningCode1 = 0;
        warningCode2 = 0;
        warningCode3 = 0;
    };

    union {
        struct{
            uchar engineFire : 1;	//D0 W 发动机起火
            uchar gearDown : 1;		//D1 W 起落架未放
            uchar oilPress : 1;		//D2 W 发动机无滑油压力
            uchar engFailure : 1;	//D3 W 发动机停车
            uchar tooLowGS : 1;		//D4 W 过低下滑道高度
            uchar tooLowTerrain : 1;//D5 W 过低下降率
            uchar outerSelALT : 1;  //D6   低于或高于预选高度
            uchar reachMinimum : 1; //D7   达到最低下降高度提示
        }warningCode1Bits;
        uchar warningCode1;			//告警码1
    };

    union {
        struct{
            uchar toGo1000 : 1;		//D0 1000英尺提示
            uchar toGo500 : 1;		//D1 500英尺提示
            uchar runwayWait : 1;	//D2 跑道线外等待提示
            uchar hotSpot : 1;		//D3 热点提示
            uchar stall : 1;		//D4 失速告警
            uchar fuelPress : 1;	//D5   燃油压力过低
            uchar stallAngle : 1;   //D6 W 失速迎角
            uchar genPowerOff : 1;  //D7 W 发电机断电
        }warningCode2Bits;
        uchar warningCode2;			//告警码2
    };

    union {
        struct{
            uchar engineOverHeat : 1; //D0 W 发动机超温
            uchar space : 7;		  //D1-7 预留
        }warningCode3Bits;
        uchar warningCode3;			  //告警码2
    };
};

//初始化数据
struct DuInitData
{
	DuInitData()
	{
		vFe = 0.0f;
		vMo = 0.0f;
		vMax = 0.0f;
		vs = 0.0f;
		minBaroHeight = 0.0f;
		vso = 0;
		vd = 0;
		space1 = 0;
	}

	float vFe;			//最大襟翼操作速度 km/h
	float vMo;		    //最大正常操作速度 km/h
	float vMax;			//最大演示安全速度 km/h
	float vs;			//失速速度 km/h
	float minBaroHeight;//最低气压高度 米
	float vso;			//失速速度 km/h
	float vd;			//最大俯冲速度 km/h
	uchar space1;		//预留1
};

enum class GaugeFailFlag{	//仪表故障标识
	GAUGE_OK = 0,			//数据正常
	GAUGE_ERROR = 1,		//数据无效或故障
	GAUGE_NODATA=2			//无数据
};

enum class NavToFrom{	//向背台标识
	TOFROM_OFF = 0,		//无向背台
	TOFROM_TO = 1,		//向台
	TOFROM_FROM = 2,	//背台
};

enum class BaroStandard{//气压基准
	BARO_QNE = 0,		//标准气压STD
	BARO_QNH = 1,		//修正海平面气压
	BARO_QFE = 2,		//场面气压

};

struct AirspeedLimitations	//空速限制
{
	AirspeedLimitations()
	{
		isChanged = false;
		VA = 0.0;
		VFE = 0.0;
		VLO = 0.0;
		VMax = 0.0;
		VMO = 0.0;
		VD = 0.0;
	}
	bool isChanged;	//数据是否发生变化
	float VA;		//机动速度
	float VFE;		//放下襟翼允许最大速度
	float VLO;		//放起落架允许最大速度
	float VMax;		//最大演示安全速度
	float VMO;		//最大操作极限速度
	float VD;		//最大俯冲速度
	float VS;		//失速速度 光洁形态下的最小操纵速度
	float VSO;		//失速速度 着陆形态最小可操作速度
};

struct AircraftInstrumentData	//飞行仪表数据
{
	float	tureAirspeed;			//单位：节
	float	indicatedAirspeed;		//单位：节
	float	groundSpeed;			//单位：节
	float	mach;
	float	speedTrend;
	AirspeedLimitations airspeedLimitations;
	float	verticalSpeed;			//单位：英尺/分钟
	float	indicatedAltitude;
	float	altitudeTrend;
	BaroStandard	baroStandard;
	float	baroSet;
	float	pitch;
	float	roll;
	float	slipskid;
	float	oat;
	float	aoa;
	float	radioHeight;
	float	windSpeed;
	float	windDirection;
	bool	imFlag;					//内指点信标有效性
	bool	mmFlag;					//中指点信标有效性
	bool	omFlag;					//外指点信标有效性
	float	magneticHeading;
	float	trueHeading;
	float	turnRate;
	bool	trackUsable;			//航迹角可用性
	float	track;
	float	selectedHeading;
	float	selectedCourse;
	bool	hasVor;
	bool	hasAdf;
	bool	hasDme;
	bool	hasLocalizer;
	bool	hasGlideslope;
	bool	cdiVailable;
	bool	gsiVailable;
	float	cdiNeedle;
	float	gsiNeedle;
	NavToFrom	toFrom;
	float	dmeDistance;
	float	dmeSpeed;
	bool	adfBearingValid;		//ADF方位角有效性
	float	adfRB;					//ADF电台相对方位角
	bool	vorBearingValid;		//VOR方位角有效性
	float	vorQDR;					//VOR飞机磁方位角
	std::string	adfName;
	std::string	vorName;
	GaugeFailFlag	airspeedError;
	GaugeFailFlag	verticalSpeedError;
	GaugeFailFlag	altimeterError;
	GaugeFailFlag	attitudeError;
	GaugeFailFlag	radioHeightError;
	GaugeFailFlag	windError;
	GaugeFailFlag	hsiError;
	GaugeFailFlag   aoaError;
	GaugeFailFlag   oatError;
};