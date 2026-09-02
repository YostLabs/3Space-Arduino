#include "tss/api/command.h"
#include "tss/sys/stdinc.h"
#include "tss/sys/config.h"

#if TSS_INCLUDE_PARAM_TYPE
#define PARAM(_count, _size, _type) TSS_PARAM_INITIALIZER(_count, _size, _type)
#else
#define PARAM(_count, _size, _type) TSS_PARAM_INITIALIZER(_count, _size)
#endif
#define NULL_PARAM TSS_PARAM_NULL_INITIALIZER
#define DYNAMIC_TYPE(x) NULL_PARAM
#define FLOAT(_count) PARAM(_count, 4, TSS_ParamTypeFloat)
#define DOUBLE(_count) PARAM(_count, 8, TSS_ParamTypeDouble)
#define U8(_count)  PARAM(_count, 1, TSS_ParamTypeU8)
#define U16(_count) PARAM(_count, 2, TSS_ParamTypeU16)
#define U32(_count) PARAM(_count, 4, TSS_ParamTypeU32)
#define U64(_count) PARAM(_count, 8, TSS_ParamTypeU64)
#define S8(_count)  PARAM(_count, 1, TSS_ParamTypeS8)
#define S16(_count) PARAM(_count, 2, TSS_ParamTypeS16)
#define S32(_count) PARAM(_count, 4, TSS_ParamTypeS32)
#define S64(_count) PARAM(_count, 8, TSS_ParamTypeS64)
#define STRING(_count) PARAM(_count, 0, TSS_ParamTypeString)

#define CMD(_num, _in_format, _out_format) \
(const struct TSS_Command[]) { \
            {   \
                .num = _num, \
                .out_format = (const struct TSS_Param[]){ _out_format, NULL_PARAM },    \
                .in_format = (const struct TSS_Param[]){ _in_format, NULL_PARAM }    \
            }  \
        }   

//Note: Once the cmd table gets full enough, it will be better to directly store the structs rather than pointers to the structs.
#define CMD_START(_num, _name) [_num] = (const struct TSS_Command[]) { {.num = _num, 
#define CMD_END } },

#define NO_OUTPUT .out_format = NULL,
#define NO_INPUT .in_format = NULL,
#define OUTPUT(...) .out_format = (const struct TSS_Param[]){ __VA_ARGS__, NULL_PARAM },
#define INPUT(...) .in_format = (const struct TSS_Param[]){ __VA_ARGS__, NULL_PARAM },

//Useful for things that frequently have the same input and output, EG settings
#define TYPE(...) OUTPUT(__VA_ARGS__) INPUT(__VA_ARGS__)

#define ACTION_CMD(_num, _name) CMD_START((_num), (_name)) NO_INPUT NO_OUTPUT CMD_END
#define READ_CMD(_num, _name, ...) CMD_START((_num), (_name)) NO_INPUT OUTPUT(__VA_ARGS__) CMD_END
#define WRITE_CMD(_num, _name, ...) CMD_START((_num), (_name)) INPUT(__VA_ARGS__) NO_OUTPUT CMD_END
//No RW cmd because the input types are frequently different from the output, unlike settings
//It is just considered a base cmd.

static const struct TSS_Command * const m_commands[256] = {
//-------------------------AUTO GENERATED COMMANDS START---------------------------------------

    READ_CMD(0, "GetTaredOrientation", FLOAT(4))
    READ_CMD(1, "GetTaredOrientationAsEulerAngles", FLOAT(3))
    READ_CMD(2, "GetTaredOrientationAsRotationMatrix", FLOAT(9))
    READ_CMD(3, "GetTaredOrientationAsAxisAngles", FLOAT(3), FLOAT(1))
    READ_CMD(4, "GetTaredOrientationAsTwoVector", FLOAT(3), FLOAT(3))
    READ_CMD(5, "GetDifferenceQuaternion", FLOAT(4))
    READ_CMD(6, "GetUntaredOrientation", FLOAT(4))
    READ_CMD(7, "GetUntaredOrientationAsEulerAngles", FLOAT(3))
    READ_CMD(8, "GetUntaredOrientationAsRotationMatrix", FLOAT(9))
    READ_CMD(9, "GetUntaredOrientationAsAxisAngles", FLOAT(3), FLOAT(1))
    READ_CMD(10, "GetUntaredOrientationAsTwoVector", FLOAT(3), FLOAT(3))
    READ_CMD(11, "GetTaredTwoVectorInSensorFrame", FLOAT(3), FLOAT(3))
    READ_CMD(12, "GetUntaredTwoVectorInSensorFrame", FLOAT(3), FLOAT(3))
    READ_CMD(13, "GetPrimaryBarometerPressure", FLOAT(1))
    READ_CMD(14, "GetPrimaryBarometerAltitude", FLOAT(1))

    CMD_START(15, "GetBarometerAltitudeByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(1))
    CMD_END

    CMD_START(16, "GetBarometerPressureByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(1))
    CMD_END

    READ_CMD(17, "GetHeading", FLOAT(1))
    ACTION_CMD(19, "SetOffsetWithCurrentOrientation")
    ACTION_CMD(20, "ResetBaseOffset")
    ACTION_CMD(22, "SetBaseOffsetWithCurrentOrientation")
    READ_CMD(31, "GetInterruptStatus", U8(1))
    READ_CMD(32, "GetAllNormalizedComponentSensorData", FLOAT(3), FLOAT(3), FLOAT(3))
    READ_CMD(33, "GetNormalizedGyroRateVector", FLOAT(3))
    READ_CMD(34, "GetNormalizedAccelerometerVector", FLOAT(3))
    READ_CMD(35, "GetNormalizedMagnetometerVector", FLOAT(3))
    READ_CMD(37, "GetAllCorrectedComponentSensorData", FLOAT(3), FLOAT(3), FLOAT(3))
    READ_CMD(38, "GetCorrectedGyroRateVector", FLOAT(3))
    READ_CMD(39, "GetCorrectedAccelerometerVector", FLOAT(3))
    READ_CMD(40, "GetCorrectedMagnetometerVector", FLOAT(3))
    READ_CMD(41, "GetCorrectedGlobalLinearAcceleration", FLOAT(3))
    READ_CMD(42, "GetCorrectedLocalLinearAcceleration", FLOAT(3))
    READ_CMD(43, "GetTemperatureCelsius", FLOAT(1))
    READ_CMD(44, "GetTemperatureFahrenheit", FLOAT(1))
    READ_CMD(45, "GetMotionlessConfidenceFactor", FLOAT(1))

    CMD_START(48, "CorrectRawGyroRateVector")
    INPUT(FLOAT(3), U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(49, "CorrectRawAccelerometerVector")
    INPUT(FLOAT(3), U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(50, "CorrectRawMagnetometerVector")
    INPUT(FLOAT(3), U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(51, "GetNormalizedGyroRateByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(52, "GetNormalizedAccelerometerVectorByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(53, "GetNormalizedMagnetometerVectorByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(54, "GetCorrectedGyroRateByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(55, "GetCorrectedAccelerometerVectorByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(56, "GetCorrectedMagnetometerVectorByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    ACTION_CMD(57, "MassStorageControllerEnable")
    ACTION_CMD(58, "MassStorageControllerDisable")
    ACTION_CMD(59, "FormatSDCard")
    ACTION_CMD(60, "LoggingStart")
    ACTION_CMD(61, "LoggingStop")
    WRITE_CMD(62, "SetClockValues", U16(1), U8(1), U8(1), U8(1), U8(1), U8(1))
    READ_CMD(63, "GetClockValues", U16(1), U8(1), U8(1), U8(1), U8(1), U8(1))
    READ_CMD(64, "LoggingGetStatus", U8(1))

    CMD_START(65, "GetRawGyroRateByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(66, "GetRawAccelerometerVectorByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    CMD_START(67, "GetRawMagnetometerVectorByID")
    INPUT(U8(1))
    OUTPUT(FLOAT(3))
    CMD_END

    ACTION_CMD(68, "EEPTSStart")
    ACTION_CMD(69, "EEPTSStop")
    READ_CMD(70, "EEPTSGetOldestStep", U32(1), U32(1), DOUBLE(1), DOUBLE(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), U8(1), U8(1), FLOAT(1), FLOAT(1))
    READ_CMD(71, "EEPTSGetNewestStep", U32(1), U32(1), DOUBLE(1), DOUBLE(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), U8(1), U8(1), FLOAT(1), FLOAT(1))
    READ_CMD(72, "EEPTSGetAvailableStepCount", U8(1))
    WRITE_CMD(73, "EEPTSInsertGPS", DOUBLE(1), DOUBLE(1))
    ACTION_CMD(74, "EEPTSAutoOffset")

    CMD_START(83, "StreamingGetCommandLabel")
    INPUT(U8(1))
    OUTPUT(STRING(1))
    CMD_END

    READ_CMD(84, "StreamingGeneratePacket", DYNAMIC_TYPE(1))
    ACTION_CMD(85, "StreamingStart")
    ACTION_CMD(86, "StreamingStop")
    WRITE_CMD(87, "LoggingPauseStreaming", U8(1))
    READ_CMD(93, "GetClockValuesString", STRING(1))
    READ_CMD(94, "GetTimestamp", U64(1))
    WRITE_CMD(95, "SetTimestamp", U64(1))
    ACTION_CMD(96, "SetTareWithCurrentOrientation")
    ACTION_CMD(97, "SetBaseTareWithCurrentOrientation")
    ACTION_CMD(120, "ResetFilter")
    READ_CMD(126, "GetDebugMessageCount", U16(1))
    READ_CMD(127, "GetDebugMessage", STRING(1))
    READ_CMD(128, "SelfTest", U32(1))
    WRITE_CMD(164, "SetTimesyncMode", U8(1))
    WRITE_CMD(165, "BeginPassiveCalibration", U8(1))
    READ_CMD(166, "GetPassiveCalibrationActive", U8(1))
    ACTION_CMD(167, "BeginActiveCalibration")
    READ_CMD(168, "GetActiveCalibrationActive", U8(1))
    READ_CMD(170, "LoggingGetLastLiveLocation", U64(1), STRING(1))
    READ_CMD(171, "FsGetNextDirectoryItem", U8(1), STRING(1), U64(1))
    WRITE_CMD(172, "FsChangeDirectory", STRING(1))
    WRITE_CMD(173, "FsOpenFile", STRING(1))
    ACTION_CMD(174, "CloseFile")
    READ_CMD(175, "FileGetRemainingSize", U64(1))
    READ_CMD(176, "FileReadLine", STRING(1))

    CMD_START(177, "FileReadBytes")
    INPUT(U16(1))
    OUTPUT(DYNAMIC_TYPE(1))
    CMD_END

    WRITE_CMD(178, "FsDeleteFileOrFolder", STRING(1))
    WRITE_CMD(179, "FileSetCursorIndex", U64(1))
    READ_CMD(180, "FileStreamingStart", U64(1))
    ACTION_CMD(181, "FileStreamingStop")
    READ_CMD(200, "BatteryGetCurrent", S16(1))
    READ_CMD(201, "BatteryGetVoltage", FLOAT(1))
    READ_CMD(202, "BatteryGetPercent", U8(1))
    READ_CMD(203, "BatteryGetStatus", U8(1))
    READ_CMD(208, "WMMGetField", FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1))
    ACTION_CMD(209, "WMMUpdatePosition")
    ACTION_CMD(210, "WMMUpdateDate")
    ACTION_CMD(214, "GPSGetIsActive")
    READ_CMD(215, "GPSGetLatitudeandLongitude", DOUBLE(1), DOUBLE(1))
    READ_CMD(216, "GPSGetAltitude", FLOAT(1))
    READ_CMD(217, "GPSGetFixStatus", U8(1))
    READ_CMD(218, "GPSGetHDOP", U8(1))
    READ_CMD(219, "GPSGetSatellites", U8(1))
    WRITE_CMD(220, "GPSReset", U8(1))
    WRITE_CMD(221, "GPSSetData", DOUBLE(1), DOUBLE(1), FLOAT(1))
    ACTION_CMD(225, "CommitSettings")
    ACTION_CMD(226, "SoftwareReset")
    ACTION_CMD(229, "EnterBootloader")
    READ_CMD(238, "GetCurrentLEDColor", FLOAT(3))
    READ_CMD(250, "GetButtonState", U8(1))
//-------------------------AUTO GENERATED COMMANDS END---------------------------------------
};

const struct TSS_Command* tssGetCommand(uint8_t num)
{
    return m_commands[num];
}

void tssGetParamListSize(const struct TSS_Param *params, uint16_t *min_size, uint16_t *max_size)
{   
    uint8_t uncapped;
    uint16_t size;

    size = 0;
    uncapped = 0;
    while(!TSS_PARAM_IS_NULL(params)) {
        //Treat variable length as max
        if(TSS_PARAM_IS_STRING(params)) {
            uncapped = 1;
        }

        size += (uint16_t)(params->count * params->size);
        params++;
    }

    *min_size = size;
    *max_size = (uncapped) ? UINT16_MAX : size;
}

#define SETTING_START(_name) { .name = _name, 
#define SETTING_END },

#define RW_SETTING(_name, ...) SETTING_START((_name)) TYPE(__VA_ARGS__) SETTING_END
#define R_SETTING(_name, ...) SETTING_START((_name)) NO_INPUT OUTPUT(__VA_ARGS__) SETTING_END
#define W_SETTING(_name, ...) SETTING_START((_name)) INPUT(__VA_ARGS__) NO_OUTPUT SETTING_END
#define CMD_SETTING(_name) SETTING_START((_name)) .in_format = (const struct TSS_Param[]){ NULL_PARAM }, NO_OUTPUT SETTING_END

//Aggregate settings technically don't need to be in the setting table because there is no information needed
//for them other then the string. However, it is still nice for thoroughness and the ability to identify all available
//settings. However, things like Query Settings won't be in the table of course since those are purely dynamic
#define AGGREGATE_SETTING(_name) SETTING_START((_name)) NO_INPUT .out_format = (const struct TSS_Param[]){ NULL_PARAM }, SETTING_END

static const struct TSS_Setting m_settings[] = {
//-------------------------AUTO GENERATED SETTINGS START---------------------------------------

    //System
    CMD_SETTING("default")
    CMD_SETTING("commit")
    CMD_SETTING("reboot")
    AGGREGATE_SETTING("all")
    AGGREGATE_SETTING("settings")
    R_SETTING("serial_number", U64(1))
    RW_SETTING("timestamp", U64(1))
    RW_SETTING("led_mode", U8(1))
    RW_SETTING("led_rgb", FLOAT(3))
    R_SETTING("version_firmware", STRING(1))
    R_SETTING("version_hardware", STRING(1))
    R_SETTING("update_rate_sensor", U32(1))
    RW_SETTING("header", U8(1))
    RW_SETTING("header_status", U8(1))
    RW_SETTING("header_timestamp", U8(1))
    RW_SETTING("header_echo", U8(1))
    RW_SETTING("header_checksum", U8(1))
    RW_SETTING("header_serial", U8(1))
    RW_SETTING("header_length", U8(1))
    R_SETTING("valid_commands", STRING(1))
    RW_SETTING("timesync_accel_threshold", FLOAT(1))
    RW_SETTING("timesync_startup_mode", U8(1))
    //Power Management
    RW_SETTING("cpu_speed", U32(1))
    R_SETTING("cpu_speed_cur", U32(1))
    W_SETTING("pm_mode", U8(1))
    RW_SETTING("pm_idle_enabled", U8(1))
    //Streaming
    RW_SETTING("stream_slots", STRING(1))
    RW_SETTING("stream_interval", U64(1))
    RW_SETTING("stream_hz", FLOAT(1))
    RW_SETTING("stream_duration", FLOAT(1))
    RW_SETTING("stream_delay", FLOAT(1))
    RW_SETTING("stream_mode", U8(1))
    RW_SETTING("stream_count", U64(1))
    R_SETTING("streamable_commands", STRING(1))
    //Debug
    RW_SETTING("debug_level", U32(1))
    RW_SETTING("debug_module", U32(1))
    RW_SETTING("debug_mode", U8(1))
    RW_SETTING("debug_led", U8(1))
    RW_SETTING("debug_fault", U8(1))
    RW_SETTING("debug_wdt", U8(1))
    //Filter
    RW_SETTING("axis_order", STRING(1))
    RW_SETTING("axis_order_c", STRING(1))
    RW_SETTING("axis_offset_enabled", U8(1))
    RW_SETTING("euler_order", STRING(1))
    R_SETTING("update_rate_filter", U32(1))
    R_SETTING("update_rate_sms", U32(1))
    RW_SETTING("offset", FLOAT(4))
    RW_SETTING("base_offset", FLOAT(4))
    RW_SETTING("tare_quat", FLOAT(4))
    RW_SETTING("tare_auto_base", U8(1))
    RW_SETTING("base_tare", FLOAT(4))
    RW_SETTING("tare_mat", FLOAT(9))
    RW_SETTING("running_avg_orient", FLOAT(1))
    RW_SETTING("filter_mode", U8(1))
    RW_SETTING("filter_mref_mode", U8(1))
    RW_SETTING("filter_mref", FLOAT(3))
    W_SETTING("filter_mref_gps", DOUBLE(2))
    RW_SETTING("filter_mref_dip", FLOAT(1))
    RW_SETTING("filter_conf_thresholds", FLOAT(1), FLOAT(1), FLOAT(1))
    RW_SETTING("heading_vector", FLOAT(3))
    W_SETTING("heading_vector_axis", STRING(1))
    RW_SETTING("heading_offset", FLOAT(1), U8(1))
    //Components
    R_SETTING("valid_accels", STRING(1))
    R_SETTING("valid_gyros", STRING(1))
    R_SETTING("valid_mags", STRING(1))
    R_SETTING("valid_baros", STRING(1))
    R_SETTING("valid_components", STRING(1))
    RW_SETTING("primary_accel", STRING(1))
    RW_SETTING("primary_gyro", STRING(1))
    RW_SETTING("primary_mag", STRING(1))
    RW_SETTING("primary_sensor_rfade", FLOAT(1))
    RW_SETTING("mag_bias_mode", U8(1))
    W_SETTING("odr_all", U32(1))
    W_SETTING("odr_accel", U32(1))
    W_SETTING("odr_gyro", U32(1))
    W_SETTING("odr_mag", U32(1))
    W_SETTING("odr_baro", U32(1))
    RW_SETTING("accel_enabled", U8(1))
    RW_SETTING("gyro_enabled", U8(1))
    RW_SETTING("mag_enabled", U8(1))
    //Accel
    RW_SETTING("calib_mat_accel%d", FLOAT(9))
    RW_SETTING("calib_bias_accel%d", FLOAT(3))
    RW_SETTING("range_accel%d", U16(1))
    R_SETTING("valid_ranges_accel%d", STRING(1))
    RW_SETTING("oversample_accel%d", U16(1))
    RW_SETTING("running_avg_accel%d", FLOAT(1))
    RW_SETTING("odr_accel%d", U32(1))
    R_SETTING("update_rate_accel%d", FLOAT(1))
    R_SETTING("noise_profile_accel%d", FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), U32(1))
    //Gyro
    RW_SETTING("calib_mat_gyro%d", FLOAT(9))
    RW_SETTING("calib_bias_gyro%d", FLOAT(3))
    RW_SETTING("range_gyro%d", U16(1))
    R_SETTING("valid_ranges_gyro%d", STRING(1))
    RW_SETTING("oversample_gyro%d", U16(1))
    RW_SETTING("running_avg_gyro%d", FLOAT(1))
    RW_SETTING("odr_gyro%d", U32(1))
    R_SETTING("update_rate_gyro%d", FLOAT(1))
    R_SETTING("noise_profile_gyro%d", FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), U32(1))
    //Mag
    RW_SETTING("calib_mat_mag%d", FLOAT(9))
    RW_SETTING("calib_bias_mag%d", FLOAT(3))
    RW_SETTING("range_mag%d", U16(1))
    R_SETTING("valid_ranges_mag%d", STRING(1))
    RW_SETTING("oversample_mag%d", U16(1))
    RW_SETTING("running_avg_mag%d", FLOAT(1))
    RW_SETTING("odr_mag%d", U32(1))
    R_SETTING("update_rate_mag%d", FLOAT(1))
    R_SETTING("noise_profile_mag%d", FLOAT(1), FLOAT(1), FLOAT(1), FLOAT(1), U32(1))
    //Baro
    RW_SETTING("calib_bias_baro%d", FLOAT(1))
    W_SETTING("calib_altitude_baro%d", FLOAT(1))
    RW_SETTING("odr_baro%d", U32(1))
    R_SETTING("update_rate_baro%d", FLOAT(1))
    //EEPTS
    RW_SETTING("pts_offset_quat", FLOAT(4))
    CMD_SETTING("pts_default")
    AGGREGATE_SETTING("pts_settings")
    W_SETTING("pts_preset_hand", U8(1))
    W_SETTING("pts_preset_motion", U8(1))
    W_SETTING("pts_preset_heading", U8(1))
    RW_SETTING("pts_debug_level", U32(1))
    RW_SETTING("pts_debug_module", U32(1))
    RW_SETTING("pts_heading_mode", U32(1))
    RW_SETTING("pts_initial_heading_mode", U32(1))
    RW_SETTING("pts_hand_heading_mode", U32(1))
    RW_SETTING("pts_mag_declination", FLOAT(1))
    RW_SETTING("pts_auto_declination", U8(1))
    RW_SETTING("pts_discard_slow", U8(1))
    RW_SETTING("pts_segment_axis", U32(1))
    RW_SETTING("pts_seg_noise", FLOAT(1))
    RW_SETTING("pts_classifier_mode", U32(1))
    RW_SETTING("pts_classifier_mode2", U32(1))
    RW_SETTING("pts_location_classifier_mode", U32(1))
    RW_SETTING("pts_hand_classifier_threshold", FLOAT(1))
    RW_SETTING("pts_disabled_truth_motions", U32(1))
    RW_SETTING("pts_dynamic_segmenter_enabled", U8(1))
    RW_SETTING("pts_estimator_scalars", FLOAT(7))
    RW_SETTING("pts_auto_estimator_scalar_rate", U32(1))
    RW_SETTING("pts_running_correction", U8(1))
    RW_SETTING("pts_hand_correction", U8(1))
    RW_SETTING("pts_heading_correction_mode", U32(1))
    RW_SETTING("pts_heading_min_dif", FLOAT(1))
    RW_SETTING("pts_heading_reset_consistencies", U8(1))
    RW_SETTING("pts_heading_backtrack_enabled", U8(1))
    RW_SETTING("pts_motion_correction_radius", U32(1))
    RW_SETTING("pts_motion_correction_consistency_req", U32(1))
    RW_SETTING("pts_orient_ref_y_threshold", FLOAT(1))
    R_SETTING("pts_version", STRING(1))
    RW_SETTING("pts_date", U32(1), U32(1), U32(1))
    R_SETTING("pts_wmm_version", STRING(1))
    W_SETTING("pts_wmm_set", STRING(1))
    RW_SETTING("pts_force_out_gps", U8(1))
    RW_SETTING("pts_initial_heading_tolerance", FLOAT(1))
    RW_SETTING("pts_heading_consistency_req", S32(1))
    RW_SETTING("pts_heading_root_err_mul", FLOAT(1))
    RW_SETTING("pts_heading_consistent_bias", FLOAT(1))
    RW_SETTING("pts_strict_bias_enabled", U8(1))
    //Embedded
    RW_SETTING("pin_mode0", U8(1))
    RW_SETTING("pin_mode1", U8(1))
    RW_SETTING("uart_baudrate", U32(1))
    RW_SETTING("i2c_addr", U8(1))
    //Data Logger
    RW_SETTING("power_hold_time", FLOAT(1))
    RW_SETTING("power_hold_state", U8(1))
    RW_SETTING("power_initial_hold_state", U8(1))
    CMD_SETTING("fs_cfg_load")
    RW_SETTING("fs_msc_enabled", U8(1))
    RW_SETTING("fs_msc_auto", U8(1))
    RW_SETTING("log_slots", STRING(1))
    RW_SETTING("log_interval", U64(1))
    RW_SETTING("log_hz", FLOAT(1))
    RW_SETTING("log_start_event", STRING(1))
    RW_SETTING("log_start_motion_threshold", FLOAT(1))
    RW_SETTING("log_stop_event", STRING(1))
    RW_SETTING("log_stop_motion_threshold", FLOAT(1))
    RW_SETTING("log_stop_motion_delay", FLOAT(1))
    RW_SETTING("log_stop_count", U64(1))
    RW_SETTING("log_stop_duration", FLOAT(1))
    RW_SETTING("log_stop_period_count", U32(1))
    RW_SETTING("log_style", U8(1))
    RW_SETTING("log_periodic_capture_time", FLOAT(1))
    RW_SETTING("log_periodic_rest_time", FLOAT(1))
    RW_SETTING("log_base_filename", STRING(1))
    RW_SETTING("log_file_mode", U8(1))
    RW_SETTING("log_data_mode", U8(1))
    RW_SETTING("log_output_settings", U8(1))
    RW_SETTING("log_header_enabled", U8(1))
    RW_SETTING("log_folder_mode", U8(1))
    RW_SETTING("log_immediate_output", U8(1))
    RW_SETTING("log_immediate_output_header_enabled", U8(1))
    RW_SETTING("log_immediate_output_header_mode", U8(1))
    RW_SETTING("rtc_year", U16(1))
    RW_SETTING("rtc_month", U8(1))
    RW_SETTING("rtc_day", U8(1))
    RW_SETTING("rtc_hour", U8(1))
    RW_SETTING("rtc_minute", U8(1))
    RW_SETTING("rtc_second", U8(1))
    RW_SETTING("rtc_datetime", U16(1), U8(1), U8(1), U8(1), U8(1), U8(1))
    RW_SETTING("rtc_source", U8(1))
    RW_SETTING("utc_offset", FLOAT(1))
    RW_SETTING("bat_chg_rate", U16(1))
    RW_SETTING("bat_cold_threshold", FLOAT(1), FLOAT(1))
    RW_SETTING("bat_warm_threshold", FLOAT(1), FLOAT(1))
    RW_SETTING("bat_hot_threshold", FLOAT(1), FLOAT(1))
    RW_SETTING("bat_offset_threshold", FLOAT(1))
    R_SETTING("bat_mah", U16(1))
    //BLE
    RW_SETTING("ble_name", STRING(1))
    R_SETTING("ble_connected", S8(1))
    CMD_SETTING("ble_disconnect")
    //GPS
    RW_SETTING("gps_standby", U8(1))
    RW_SETTING("gps_led", U8(1))
    RW_SETTING("gps_periodic_fix_times", U16(2))
    RW_SETTING("gps_periodic_unfixed_times", U16(2))
    RW_SETTING("gps_periodic_enabled", U8(1))
    RW_SETTING("rtc_gps_sync_interval", U64(1))
    RW_SETTING("rtc_gps_force_sync", U8(1))
    RW_SETTING("usb_timeout", FLOAT(1))
    //DEPRECATED
    //These settings are considered deprecated and are present for backwards compatability
    CMD_SETTING("sd_cfg_load")
    RW_SETTING("sd_msc_enabled", U8(1))
    RW_SETTING("sd_msc_auto", U8(1))
    RW_SETTING("cat", STRING(1))
    RW_SETTING("running_avg", FLOAT(1))
//-------------------------AUTO GENERATED SETTINGS END---------------------------------------
};

int tssSettingKeyCmp(const char* key, const char* key_format)
{
    //Stop once atleast one string ends
    while(*key != '\0' && *key_format != '\0') {
        //Advance until no match
        if(tolower(*key) == *key_format) {
            key++;
            key_format++;
        }
        //Check to see if no match was due to a format specifier and parse it
        else if (*key_format == '%') {
            key_format++; //Go to the format character
            //Decimal Formatter
            if(*key_format == 'd') {
                //Read pass the number and then check for equivalence again
                while(*key >= '0' && *key <= '9' ) { 
                    key++;
                }
            }
            else { //Invalid format string
                return -2;
            }
            key_format++; //Advance past the format character
        }
        //No match!
        else {
            break;
        }
    }
    
    if(tolower(*key) == *key_format) return 0; //They are both '\0'
    return (tolower(*key) < *key_format) ? -1 : 1;
}

const struct TSS_Setting* tssGetSetting(const char *name)
{
    for(uint16_t i = 0; i < sizeof(m_settings) / sizeof(m_settings[0]); i++) {
        const struct TSS_Setting *setting = &m_settings[i];
        if(tssSettingKeyCmp(name, setting->name) == 0) {
            return setting;
        }
    }
    return NULL;
}