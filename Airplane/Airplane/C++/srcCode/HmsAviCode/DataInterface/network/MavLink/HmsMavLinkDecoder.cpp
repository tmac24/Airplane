#include "HmsMavLinkDecoder.h"

CHmsMavLinkDecoder::CHmsMavLinkDecoder()
{

}

CHmsMavlinkAVIData & CHmsMavLinkDecoder::decode(const char * msg, int length)
{
	for (int i = 0; i<length; ++i)
	{
		if (mavlink_parse_char(MAVLINK_COMM_0, msg[i], &m_mavlinkMsg, &m_mavlinkStatus))
		{
			mavlinkMsgDecode(m_mavlinkMsg, &m_aviData);
		}
	}
	return m_aviData;
}

uint8_t CHmsMavLinkDecoder::mavlinkMsgDecode(mavlink_message_t msg, CHmsMavlinkAVIData *output)
{
	int index = 0;
	switch (msg.msgid)
	{
	case MAVLINK_MSG_ID_HEARTBEAT:
	{
		//ppPwork_data[index][0] = ppPwork_data[index][0] + 1;
		break;
	}
	case MAVLINK_MSG_ID_SET_ATTITUDE_TARGET:     //没有四元数
	{
		//        ppPwork_data[index][0] = (int16_t)mavlink_msg_set_attitude_target_get_time_boot_ms(&msg);
		//        ppPwork_data[index][1] = (int16_t)mavlink_msg_set_attitude_target_get_target_system(&msg);
		//        ppPwork_data[index][2] = (int16_t)mavlink_msg_set_attitude_target_get_target_component(&msg);
		//        ppPwork_data[index][3] = (int16_t)mavlink_msg_set_attitude_target_get_type_mask(&msg);
		//        //ppPwork_data[index][3] = (int16_t)mavlink_msg_set_attitude_target_get_q(&msg,q);
		//        ppPwork_data[index][4] = (int16_t)mavlink_msg_set_attitude_target_get_body_roll_rate(&msg);
		//        ppPwork_data[index][5] = (int16_t)mavlink_msg_set_attitude_target_get_body_pitch_rate(&msg);
		//        ppPwork_data[index][6] = (int16_t)mavlink_msg_set_attitude_target_get_body_yaw_rate(&msg);
		//        ppPwork_data[index][7] = (int16_t)mavlink_msg_set_attitude_target_get_thrust(&msg);
		break;
	}
	case MAVLINK_MSG_ID_SET_POSITION_TARGET_LOCAL_NED:
	{
		//        ppPwork_data[index][0] = (int16_t)mavlink_msg_set_position_target_local_ned_get_time_boot_ms(&msg);
		//        ppPwork_data[index][1] = (int16_t)mavlink_msg_set_position_target_local_ned_get_target_system(&msg);
		//        ppPwork_data[index][2] = (int16_t)mavlink_msg_set_position_target_local_ned_get_target_component(&msg);
		//        ppPwork_data[index][3] = (int16_t)mavlink_msg_set_position_target_local_ned_get_coordinate_frame(&msg);
		//        ppPwork_data[index][4] = (int16_t)mavlink_msg_set_position_target_local_ned_get_type_mask(&msg);
		//        ppPwork_data[index][5] = (int16_t)mavlink_msg_set_position_target_local_ned_get_x(&msg);
		//        ppPwork_data[index][6] = (int16_t)mavlink_msg_set_position_target_local_ned_get_y(&msg);
		//        ppPwork_data[index][7] = (int16_t)mavlink_msg_set_position_target_local_ned_get_z(&msg);
		//        ppPwork_data[index][8] = (int16_t)mavlink_msg_set_position_target_local_ned_get_vx(&msg);
		//        ppPwork_data[index][9] = (int16_t)mavlink_msg_set_position_target_local_ned_get_afx(&msg);
		break;
	}
	case MAVLINK_MSG_ID_ATTITUDE:
	{
		m_aviData.attitudeValid = true;
		m_aviData.attitude.lastedUpdate = 1;
		m_aviData.attitude.roll = -mavlink_msg_attitude_get_roll(&msg);
		m_aviData.attitude.pitch = mavlink_msg_attitude_get_pitch(&msg);
		m_aviData.attitude.yaw = mavlink_msg_attitude_get_yaw(&msg);
		m_aviData.attitude.rollspeed = mavlink_msg_attitude_get_rollspeed(&msg);
		m_aviData.attitude.pitchspeed = mavlink_msg_attitude_get_pitchspeed(&msg);
		m_aviData.attitude.yawspeed = mavlink_msg_attitude_get_yawspeed(&msg);
		break;
	}
	case MAVLINK_MSG_ID_RAW_IMU:
	{
		        //ppPwork_data[index][0] = (int16_t)mavlink_msg_raw_imu_get_xacc(&msg);
		        //ppPwork_data[index][1] = (int16_t)mavlink_msg_raw_imu_get_yacc(&msg);
		        //ppPwork_data[index][2] = (int16_t)mavlink_msg_raw_imu_get_zacc(&msg);
		        //ppPwork_data[index][3] = (int16_t)mavlink_msg_raw_imu_get_xgyro(&msg);
		        //ppPwork_data[index][4] = (int16_t)mavlink_msg_raw_imu_get_ygyro(&msg);
		        //ppPwork_data[index][5] = (int16_t)mavlink_msg_raw_imu_get_zgyro(&msg);
		        //ppPwork_data[index][6] = (int16_t)mavlink_msg_raw_imu_get_xmag(&msg);
		        //ppPwork_data[index][7] = (int16_t)mavlink_msg_raw_imu_get_ymag(&msg);
		        //ppPwork_data[index][8] = (int16_t)mavlink_msg_raw_imu_get_zmag(&msg);
		break;
	}
	case MAVLINK_MSG_ID_VFR_HUD:
	{
		m_aviData.vfrHudValid = true;
		m_aviData.vfrHud.lastedUpdate = 1;
		m_aviData.vfrHud.alt = mavlink_msg_vfr_hud_get_alt(&msg);
		m_aviData.vfrHud.airspeed = mavlink_msg_vfr_hud_get_airspeed(&msg);
		m_aviData.vfrHud.groundspeed = mavlink_msg_vfr_hud_get_groundspeed(&msg);
		m_aviData.vfrHud.heading = mavlink_msg_vfr_hud_get_heading(&msg);
		m_aviData.vfrHud.climb = mavlink_msg_vfr_hud_get_climb(&msg);
		m_aviData.vfrHud.throttle = mavlink_msg_vfr_hud_get_throttle(&msg);
		break;
	}
	case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
	{
		m_aviData.positionValid = true;
		m_aviData.position.lastedUpdate = 1;
		m_aviData.position.alt = mavlink_msg_global_position_int_get_alt(&msg);
		m_aviData.position.hdg = mavlink_msg_global_position_int_get_hdg(&msg);
		m_aviData.position.lon = mavlink_msg_global_position_int_get_lon(&msg);
		m_aviData.position.lat = mavlink_msg_global_position_int_get_lat(&msg);
		m_aviData.position.relative_alt = mavlink_msg_global_position_int_get_relative_alt(&msg);
		m_aviData.position.time_boot_ms = mavlink_msg_global_position_int_get_time_boot_ms(&msg);
		m_aviData.position.vx = mavlink_msg_global_position_int_get_vx(&msg);
		m_aviData.position.vy = mavlink_msg_global_position_int_get_vy(&msg);
		m_aviData.position.vz = mavlink_msg_global_position_int_get_vz(&msg);
		break;
	}
	case MAVLINK_MSG_ID_RC_CHANNELS_SCALED:
	{
		//        ppPwork_data[index][0] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan1_scaled(&msg)) / 1000;
		//        ppPwork_data[index][1] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan2_scaled(&msg)) / 1000;
		//        ppPwork_data[index][2] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan3_scaled(&msg)) / 1000;
		//        ppPwork_data[index][3] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan4_scaled(&msg)) / 1000;
		//        ppPwork_data[index][4] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan5_scaled(&msg)) / 1000;
		//        ppPwork_data[index][5] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan6_scaled(&msg)) / 1000;
		//        ppPwork_data[index][6] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan7_scaled(&msg)) / 1000;
		//        ppPwork_data[index][7] = ((int16_t)mavlink_msg_rc_channels_scaled_get_chan8_scaled(&msg)) / 1000;
		break;
	}
	case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
	{
		//        ppPwork_data[index][0] = (int16_t)mavlink_msg_servo_output_raw_get_servo1_raw(&msg);
		//        ppPwork_data[index][1] = (int16_t)mavlink_msg_servo_output_raw_get_servo2_raw(&msg);
		//        ppPwork_data[index][2] = (int16_t)mavlink_msg_servo_output_raw_get_servo3_raw(&msg);
		//        ppPwork_data[index][3] = (int16_t)mavlink_msg_servo_output_raw_get_servo4_raw(&msg);
		break;
	}
	case MAVLINK_MSG_ID_GPS_STATUS:
	{
		m_aviData.gpsStatusValid = true;
		m_aviData.gpsStatus.lastedUpdate = 1;
		m_aviData.gpsStatus.satellites_visible = mavlink_msg_gps_status_get_satellites_visible(&msg);
		m_aviData.gpsStatus.satellite_prn_count = mavlink_msg_gps_status_get_satellite_azimuth(&msg, m_aviData.gpsStatus.satellite_prn);
		m_aviData.gpsStatus.satellite_used_count = mavlink_msg_gps_status_get_satellite_azimuth(&msg, m_aviData.gpsStatus.satellite_used);
		m_aviData.gpsStatus.satellite_elevation_count = mavlink_msg_gps_status_get_satellite_azimuth(&msg, m_aviData.gpsStatus.satellite_elevation);
		m_aviData.gpsStatus.satellite_azimuth_count = mavlink_msg_gps_status_get_satellite_azimuth(&msg, m_aviData.gpsStatus.satellite_azimuth);
		m_aviData.gpsStatus.satellite_snr_count = mavlink_msg_gps_status_get_satellite_azimuth(&msg, m_aviData.gpsStatus.satellite_snr);
		break;
	}
	case MAVLINK_MSG_ID_SCALED_PRESSURE:
	{
		m_aviData.pressureValid = true;
		m_aviData.pressure.lastedUpdate = 1;
		m_aviData.pressure.time_boot_ms = mavlink_msg_scaled_pressure_get_time_boot_ms(&msg);
		m_aviData.pressure.press_abs = mavlink_msg_scaled_pressure_get_press_abs(&msg);
		m_aviData.pressure.press_diff = mavlink_msg_scaled_pressure_get_press_diff(&msg);
		m_aviData.pressure.temperature = mavlink_msg_scaled_pressure_get_temperature(&msg);
		break;
	}
	case MAVLINK_MSG_ID_SCALED_IMU:
	{
		m_aviData.imuValid = true;
		m_aviData.imu.lastedUpdate = 1;
		m_aviData.imu.time_boot_ms = mavlink_msg_scaled_imu_get_time_boot_ms(&msg);
		m_aviData.imu.xacc = mavlink_msg_scaled_imu_get_xacc(&msg);
		m_aviData.imu.yacc = mavlink_msg_scaled_imu_get_yacc(&msg);
		m_aviData.imu.zacc = mavlink_msg_scaled_imu_get_zacc(&msg);
		m_aviData.imu.xgyro = mavlink_msg_scaled_imu_get_xgyro(&msg);
		m_aviData.imu.ygyro = mavlink_msg_scaled_imu_get_ygyro(&msg);
		m_aviData.imu.zgyro = mavlink_msg_scaled_imu_get_zgyro(&msg);
		m_aviData.imu.xmag = mavlink_msg_scaled_imu_get_xmag(&msg);
		m_aviData.imu.ymag = mavlink_msg_scaled_imu_get_ymag(&msg);
		m_aviData.imu.zmag = mavlink_msg_scaled_imu_get_zmag(&msg);
		m_aviData.imu.temperature = mavlink_msg_scaled_imu_get_temperature(&msg);
		break;
	}
	default:
		break;
	}

	return (uint8_t)msg.msgid;
}
