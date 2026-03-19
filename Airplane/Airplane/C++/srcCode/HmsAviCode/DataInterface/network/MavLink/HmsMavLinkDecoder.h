#ifndef CHMSMAVLINKDECODER_H
#define CHMSMAVLINKDECODER_H

#include "mavlink_v2/common/mavlink.h"

typedef struct _HmsMavLinkAttitude
{
	uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
	float roll; /*< [rad] Roll angle (-pi..+pi)*/
	float pitch; /*< [rad] Pitch angle (-pi..+pi)*/
	float yaw; /*< [rad] Yaw angle (-pi..+pi)*/
	float rollspeed; /*< [rad/s] Roll angular speed*/
	float pitchspeed; /*< [rad/s] Pitch angular speed*/
	float yawspeed; /*< [rad/s] Yaw angular speed*/
	int lastedUpdate;//最近更新时间 距离1970年1月1日到接收时的毫秒数
}HmsMavLinkAttitude;

typedef struct _HmsMavLinkPosition
{
	uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
	int32_t lat; /*< [degE7] Latitude, expressed*/
	int32_t lon; /*< [degE7] Longitude, expressed*/
	int32_t alt; /*< [mm] Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL.*/
	int32_t relative_alt; /*< [mm] Altitude above ground*/
	int16_t vx; /*< [cm/s] Ground X Speed (Latitude, positive north)*/
	int16_t vy; /*< [cm/s] Ground Y Speed (Longitude, positive east)*/
	int16_t vz; /*< [cm/s] Ground Z Speed (Altitude, positive down)*/
	uint16_t hdg; /*< [cdeg] Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX*/
	int lastedUpdate;//最近更新时间 距离1970年1月1日到接收时的毫秒数
}HmsMavLinkPosition;

typedef struct _HmsMavLinkGpsStatus
{
	uint8_t satellites_visible; /*<  Number of satellites visible*/
	uint8_t satellite_prn[20]; /*<  Global satellite ID*/
	uint8_t satellite_used[20]; /*<  0: Satellite not used, 1: used for localization*/
	uint8_t satellite_elevation[20]; /*< [deg] Elevation (0: right on top of receiver, 90: on the horizon) of satellite*/
	uint8_t satellite_azimuth[20]; /*< [deg] Direction of satellite, 0: 0 deg, 255: 360 deg.*/
	uint8_t satellite_snr[20]; /*< [dB] Signal to noise ratio of satellite*/
	uint16_t satellite_prn_count;//satellite_prn总个数
	uint16_t satellite_used_count;//satellite_used总个数
	uint16_t satellite_elevation_count;//satellite_elevation总个数
	uint16_t satellite_azimuth_count;//satellite_azimuth总个数
	uint16_t satellite_snr_count;//satellite_snr总个数
	int lastedUpdate;//最近更新时间 距离1970年1月1日到接收时的毫秒数
}HmsMavLinkGpsStatus;

typedef struct _HmsMavLinkVFRHud
{
	float airspeed; /*< [m/s] Current indicated airspeed (IAS).*/
	float groundspeed; /*< [m/s] Current ground speed.*/
	float alt; /*< [m] Current altitude (MSL).*/
	float climb; /*< [m/s] Current climb rate.*/
	int16_t heading; /*< [deg] Current heading in compass units (0-360, 0=north).*/
	uint16_t throttle; /*< [%] Current throttle setting (0 to 100).*/
	int lastedUpdate;//最近更新时间 距离1970年1月1日到接收时的毫秒数
}HmsMavLinkVFRHud;

typedef struct _HmsMavLinkPressure
{
	uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
	float press_abs; /*< [hPa] Absolute pressure*/
	float press_diff; /*< [hPa] Differential pressure 1*/
	int16_t temperature; /*< [cdegC] Temperature*/
	int lastedUpdate;//最近更新时间 距离1970年1月1日到接收时的毫秒数
}HmsMavLinkPressure;

typedef struct _HmsMavLinkImu {
	uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot).*/
	int16_t xacc; /*< [mG] X acceleration*/
	int16_t yacc; /*< [mG] Y acceleration*/
	int16_t zacc; /*< [mG] Z acceleration*/
	int16_t xgyro; /*< [mrad/s] Angular speed around X axis*/
	int16_t ygyro; /*< [mrad/s] Angular speed around Y axis*/
	int16_t zgyro; /*< [mrad/s] Angular speed around Z axis*/
	int16_t xmag; /*< [mgauss] X Magnetic field*/
	int16_t ymag; /*< [mgauss] Y Magnetic field*/
	int16_t zmag; /*< [mgauss] Z Magnetic field*/
	int16_t temperature; /*< [cdegC] Temperature, 0: IMU does not provide temperature values. If the IMU is at 0C it must send 1 (0.01C).*/
	int lastedUpdate;//最近更新时间 距离1970年1月1日到接收时的毫秒数
} HmsMavLinkImu;

class CHmsMavlinkAVIData
{
public:
	CHmsMavlinkAVIData()
	{
		attitudeValid = false;
		positionValid = false;
		gpsStatusValid = false;
		vfrHudValid = false;
		pressureValid = false;
		imuValid = false;
	}

	bool attitudeValid;
	bool positionValid;
	bool gpsStatusValid;
	bool vfrHudValid;
	bool pressureValid;
	bool imuValid;
	HmsMavLinkAttitude attitude;
	HmsMavLinkPosition position;
	HmsMavLinkGpsStatus gpsStatus;
	HmsMavLinkVFRHud vfrHud;
	HmsMavLinkPressure pressure;
	HmsMavLinkImu imu;
};

class CHmsMavLinkDecoder
{
public:
	CHmsMavLinkDecoder();

	CHmsMavlinkAVIData & decode(const char * msg, int length);

private:
	CHmsMavlinkAVIData m_aviData;
	mavlink_message_t m_mavlinkMsg;
	mavlink_status_t m_mavlinkStatus;

	uint8_t mavlinkMsgDecode(mavlink_message_t msg, CHmsMavlinkAVIData * output);
};

#endif // CHMSMAVLINKDECODER_H
