#pragma once
#include <vector>
#include "HmsCS.h"
#include "Buffer.h"
#include "HmsDataRecord.h"

typedef struct
{
    unsigned char		status;
    unsigned char		addressType;
    unsigned int        icaoAddress;
    float				latitude;
    float				longitude;
    int					altitude;
    unsigned char		miscIndicators;
    unsigned char		nic;
    unsigned char		nacP;
    unsigned short      horizVelocity;
    short				vertVelocity;
    float				trackHeading;
    unsigned char       emitterCategory;
    char				callsign[8];
    unsigned char		emergencyPriorityCode;
} ShortTrafficObjectInfo;

struct iLevilStatusMessage
{
	float	version;			//Firmware version x 10. Unsigned byte.
								//Resolution: 0.1 (starting at 1.0 to 25.5)
	int		batteryLevel;		///battery Level[0, 100].
	bool	isCharging;
	union
	{
		unsigned short  uErrorFlag;
		struct
		{
			unsigned short selfTestRollGyro : 1;			//1			Self Test Roll Gyro				Roll gyro did not respond at initialization
			unsigned short selfTestPitchGyro : 1;			//2			Self Test Pitch Gyro			Pitch gyro did not respond at initialization
			unsigned short selfTestYawGyro : 1;				//3			Self Test Yaw Gyro				Yaw gyro did not respond at initialization
			unsigned short selfTestAccX : 1;				//4			Self Test Acc X					Accelerometer X did not respond at initialization
			unsigned short selfTestAccY : 1;				//5			Self Test Acc Y					Accelerometer Y did not respond at initialization
			unsigned short selfTestAccZ : 1;				//6			Self Test Acc Z					Accelerometer Z did not respond at initialization
			unsigned short watchdogTest : 1;				//7			Watchdog Test					At start up, the watchdog is reset to make sure it is functioning.
			unsigned short RamTest : 1;						//8			RAM Test						The ram is tested for integrity
			unsigned short EEPROMAccessTest : 1;			//9			EEPROM Access test				EEPROM access test is done to make sure memory access is achievable.
			unsigned short EEPROMChecksum : 1;				//10		EEPROM Checksum					EEPROM Checksum is performed to make sure the data is correct
			unsigned short FlashChecksum : 1;				//11		Flash Checksum					A RAM checksum is done to test the integrity of the program
			unsigned short lowVoltage : 1;					//12		low voltage error				Low voltage or Low battery
			unsigned short ExcessTemperature : 1;			//13		Excess temperature				Temperature greater than 60 degrees Celsius
			unsigned short rollExcessError : 1;				//14		Roll excess error				Roll error : There is an error when comparing the accelerometer data with gyro data in Roll.
			unsigned short pitchExcessError : 1;			//15		pitch excess error				Pitch error : There is an error when comparing the accelerometer data with gyro data in Pitch.
			unsigned short yawExcessError : 1;				//16		Yaw - excess error				Yaw error : There is an error when comparing the accelerometer data with gyro data in Yaw.
		}errorFlag;
	};
	unsigned char   WaasGpsStatus;		//1-WAAS enabled 0-No Was
	unsigned char   AuxPortType;		/*
										0 – None, Default
										1 – Levil EIS ASCII
										2 – Levil EIS binary
										3 – GRT EIS
										4 – Reserved
										5 – External ADS-B (GDL90)
										6 – Vertical Power
										7 – Zaon XRX
										8 – Zaon M1090
										9 – Autopilot
										10 – NMEA GPS output
										11 – FLARM
										20 – GDL90 out	
										*/
};

struct iLevilGpsStatusMessage
{
	unsigned char WaasStatus;				//0 – No SBAS solution, 1 – SBAS solution valid
	unsigned char nSatellites;				//Number of satellites used in SV
	float		  power;					//Average signal level of all channels being used in SV (0.1 resolution)
											/*
													0 - 29.9 : Low
													30.0 – 39.9 : Nominal
													40.0 and up : High											
											*/
	unsigned char  nOutputRate;				// 1 = 1Hz  5 = 5Hz
	float		   fAltitudeVariation;		/*
											Signed float representing the difference between Ellipsoid Altitude and MSL Altitude. To display MSL on the display use the following formula:
											MSL = EllipsoidAlt* + Altitude variation
											*/
};

struct iLevilOwnshipReport
{
	double latitude;
	double longitude;
	float altitude;					//Vertical Velocity: Signed integer in units of 64 fpm
	union
	{
		unsigned char uMiscellaneousIndicators;
		struct
		{
			unsigned char tt:2;		//00 not valid   01 true track angle  10 heading(Magnetic)   11 heading(True)
			unsigned char reportUpdate:1;		//0 report is updated  1 report is extrapolated
			unsigned char isInAirborne : 1;		//0 On Ground  1 Airborne
		};
	};
	float horizontalVelocity;		//Horizontal velocity. Resolution = 1 kt.
	float verticalVelocity;			//Vertical Velocity
	float trackOrHeading;			//Track/Heading

    struct
    {
        unsigned char bHorizontalVelocityValid : 1;//Effectiveness of roll(7FFF when data is invalid).
        unsigned char bAltitudeValid : 1;//Effectiveness of Pitch(7FFF when data is invalid).
    };
};

struct iLevilGeoAltitude    
{
	/*
	When configured for either the Traffic Alert or Pass-through interfaces, the GDL 90
	will output an Ownship Geometric Altitude message once per second, when geometric altitude is
	available.
	*/
	float           altitude;
    float           meterVFOM;          //Vertical Figure of Merit in meters
    struct
    {
        unsigned int    bVerticalWarnng:1;    //Vertical Warning
        unsigned int    bMeterVFOMValid:1;
    };  
};

struct iLevilAhrsAirMessage
{
	float Roll; //Roll in degrees x 10 (Positive clockwise)Resolution: 0.1 deg(-180 to 180 degrees).
	float Pitch;//Pitch in degrees x 10 (Positive up)Resolution: 0.1 deg(-90 to 90 degrees).
	float Yaw;  //Magnetic Heading in degrees x 10 Resolution: 0.1 deg(0 - 360 degrees).
	float TurnCoordinator;//Slip-indicator in degrees x 10 (+ when roll is positive)Resolution: 0.1 deg(-90 to 90).
	float YawRate;//Yaw rate in degrees/sec x 10 (+ to the right) Resolution: 0.1 deg / sec.
	float GLoad;//G load in Z axis x 10 Resolution: 0.1 g’s(-4 to 4 g’s) 7FFF when data is invalid.
	float IndicatedAirspeed;//Speed in KNOTS x 10 Resolution: 0.1 KNOT(-3200 to 3200 KTS).
	short PressureAltitude; //Altitude in feet at 29.92 inHg.Unsigned word where sea level is represented by 5000 ft.Resolution: 1 ft(-5, 000 to 60, 000 feet).
	short VerticalSpeed;//Feet per minute (positive up)Res: 1 feet / min(-32, 000 to 32, 000 feet / min).
	char  Reserved[2];  //Reserved bytes for future use.

	struct
	{
		unsigned char bRollValid : 1;//Effectiveness of roll(7FFF when data is invalid).
		unsigned char bPitchValid : 1;//Effectiveness of Pitch(7FFF when data is invalid).
		unsigned char bYawValid : 1;  //Effectiveness of Yaw(7FFF when data is invalid).
		unsigned char bInclinationValid : 1;//Effectiveness of Inclination(7FFF when data is invalid).
		unsigned char bTurnCoordinatorValid : 1;//Effectiveness of TurnCoordinator(7FFF when data is invalid).
		unsigned char bGLoadValid : 1;//Effectiveness of GLoad(7FFF when data is invalid or no transducers installed(iLevil SW)).
		unsigned char bIndicatedAirspeedValid : 1;//Effectiveness of IndicatedAirspeed(FFFF when data is invalid or no transducers installed(iLevil SW)).
		unsigned char bPressureAltitudeValid : 1; //Effectiveness of PressureAltitude(7FFF when data is invalid or no transducers installed(iLevil SW)).
		unsigned char bVerticalSpeedValid : 1;//Effectiveness of VerticalSpeed.
	};
};

enum class AHRS_DEVICE_TYPE
{
	noDevice = 0,
	iLevil,
	XGPS,
};

/**
 * iLevil data. 
 * Note:this is only part of the information in the iLevil device.
*/
struct AftDataiLevil
{
	int							nUtcSeconds;
	bool						bLatLonValid;///whether or not latitude and longitude is valid: 0 :false 1:true.
    bool                        bLatLonDecodeValid;

	iLevilStatusMessage			statusMsg;				//status message
	iLevilGpsStatusMessage		statusGpsMsg;			//gps status message
	iLevilOwnshipReport			ownshipReport;			//the gps data
	iLevilAhrsAirMessage		ahrsAirMsg;				//ahrsAirMsg;
	iLevilGeoAltitude			geoAltitude;			//
	AHRS_DEVICE_TYPE			eDeviceType;
};



class CDataProc
{
public:
	/**
	 * Get a unique instance.
	*/
	static CDataProc* GetInstance();

	/**
	 * set current data which to be processed.
	 * parameter:
	 *		pdata data to be processed.
	 *		len   length of data.
	*/
	void Data(const char *pdata, int len);

	/**
	 * Get resolved data.
	 * parameter:
			pInfo resolved data.
	 * return:void.
	*/
	bool GetData(AftDataiLevil *pInfo);

    void GetShortTraffic(std::vector<ShortTrafficObjectInfo> &vec);

	AHRS_DEVICE_TYPE GetDeviceType(){ return m_eDeviceType; }

	CHmsDataRecord * GetDataRecord(){ return &m_dataRecord; }

	int GetCmdPeply();

protected:
	std::vector<CBuffer*>  SegmentationMsgs();
	void DecodeMsg(CBuffer & buffer);
	void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);


	/**
	 * constructed function.
	 * type: protection.
	 * prohibit external creation.
	*/
	CDataProc();
	~CDataProc();


public:
	/**
	 * Match crc.
	 * After receiving a message, 
		it must be converted into their original values before checking Checksum or decoding data.
	 * Before decoding data,
	    it must be use the interface.
	 * parameter:
			pdata data to be processed.
			len   lenght of data.
	 * return crc value.
	*/
    unsigned short MatchCRC(const char* pdata, unsigned short len);

private:
	/**
	* construct crc list.
	* you must be create the crc list before begining of program.
	*/
	void CreateCRC();

	/**
	 * Make valid message.
	 * The “byte-stuffing” technique is used wherever a 0x7D or 0x7E byte is found in between the two Flag Bytes.
	 * when making a message, it with "0x7D 0x5E" becomes 0x7E and "0x7D 0x5D" becomes 0x7D.
	 * parameter :
			ch current char.
	 * return: void.
	*/
	void MakeMessage(const char ch);

	/**
	 * Resolve data.
	 * parameter:
			pdata data to be processed.
			len   length of data.
	 * return :void.
	*/
	void ResolveData(const char *pdata, int len);
	
	/**
	 * Messages.
	 * parameter:
			pdata data to be processed.
			len   length of data.
	 * return:void.
	*/
	void Handle_HeartBeat(const char *pdata, int len);
	void Handle_Ownship(const char *pdata, int len);
	void Handle_GeoAltitude(const char *pdata, int len);
	void Handle_LevilStatus(const char *pdata, int len);
	void Handle_LevilAHRS(const char *pdata, int len);
	void Handle_LevilEngine(const char *pdata, int len);
	void Handle_LevilAUX(const char *pdata, int len);
	void Handle_LevilConfigurationCommand(const char *pdata, int len);
	void Handle_LevilGpsStatus(const char * pdata, int len);
	void Handle_Hms_Ownship(const char *pdata, int len);
	void Handle_Hms_GeoAltitude(const char *pdata, int len);
	void Handle_XGPS190_AHRS_V2(const char *pdata, int len);
	void Handle_XGPS190_DeviceReport(const char *pdata, int len);
	void Handle_XGPS190_DetailedSatData(const char *pdata, int len);

	void Handle_UplinkMessage(const char *pdata, int len);
	void Handle_ShortTrafficReport(const char *pdata, int len);
	void Handle_BasicAndLongTrafficReport(const char *pdata, int len);
	void Handle_DeviceResponse(const char *pdata, int len);

private:
	void DecodeSingleIFrame(unsigned char * data, int iFrameLength);

	// Decode NOTAMS (including TFRs) and Service Status - Product ID 8
	void DecodeProductID8(unsigned char * data, int iFrameLength);
	// Decode Aerodrome and Airspace D-ATIS - Product ID 9
	void DecodeProductID9(unsigned char * data, int iFrameLength);
	// Decode Aerodrome and Airspace TWIP - Product ID 10
	void DecodeProductID10(unsigned char * data, int iFrameLength);
	// Decode Aerodrome and Airspace AIRMET - Product ID 11
	void DecodeProductID11(unsigned char * data, int iFrameLength);
	// Decode Aerodrome and Airspace SIGMET & Conductive SIGMET - Product ID 12
	void DecodeProductID12(unsigned char * data, int iFrameLength);
	// Decode Aerodrome and Airspace SUA Status - Product ID 13
	void DecodeProductID13(unsigned char * data, int iFrameLength);
	// Decode Global Block Representation - Regional NEXRAD - Product ID 63
	void DecodeProductID63(unsigned char * data, int iFrameLength);
	// Decode Global Block Representation - CONUS NEXRAD - Product ID 64
	void DecodeProductID64(unsigned char * data, int iFrameLength);
	// Decode Generic Textual Block Product Type 2 DLAC - Product ID 413
	void DecodeProductID413(unsigned char * data, int iFrameLength);
	// decode the incoming NEXRAD FISB data into uncompressed data
	void DecodeGraphicalWeatherData(unsigned char * data, unsigned short iFrameLength, bool isCONUS);

	void SetDeviceType(AHRS_DEVICE_TYPE eType);

private:
#if 0
	char *m_buffer;  //buffer for message.
	unsigned int m_total; //total length of buffer.
	unsigned int m_curent;//current used lenght of buffer.

	bool  m_bMakeMsg; //sign making message.
	int   m_MakeIndex;//make index.
#endif

private:
	typedef std::vector<unsigned short> CRCS;			//the crc data define

	CRCS				m_vCrc;				//the crc list, is used for checking.

	AftDataiLevil		m_data;//current data.
	AHRS_DEVICE_TYPE	m_eDeviceType;
	bool				m_bIsRunning;

    CHmsDataRecord		m_dataRecord;
	
	CBuffer				m_bufferRemain;		//the remain buffer
	CHmsCS				m_cs;

	int					m_nCmdReply;

    std::vector<ShortTrafficObjectInfo> m_vecShortTraffic;
};