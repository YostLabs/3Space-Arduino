 /**
 * @ Author: Andy Riedlinger
 * @ Create Time: 2025-5-22 1:01:00
 *
 * @ Description:
 */
#ifndef __TSS_SENSOR_H__
#define __TSS_SENSOR_H__

#include "tss/export.h"

#include "tss/com/com_class.h"
#include "tss/api/header.h"
#include "tss/api/command.h"
#include "tss/api/core.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------TYPE DECLARATIONS-----------------------------------

enum TSS_DataCallbackState {
    TSS_DataCallbackStateError = -1,
    TSS_DataCallbackStateIgnored = 0,
    TSS_DataCallbackStateProcessed = 1
};

typedef struct TSS_Sensor TSS_Sensor;
typedef enum TSS_DataCallbackState (*TssDataCallback)(TSS_Sensor *sensor);

struct TSS_Sensor {
    struct TSS_Com_Class *com;

    //Config Info
    bool _header_enabled;
    struct TSS_Header_Info header_cfg;

    struct {
        TssDataCallback cb;
        uint16_t bytes_read;
        bool _immediate;
        bool message_processed;
    } debug;

    //Cached Data
    struct TSS_Header last_header;
    struct TSS_Setting_Response last_write_setting_response;
    uint16_t last_num_settings_read;

    //Streaming Information
    struct {
        struct {
            TssDataCallback cb;
            const struct TSS_Command* commands[17];
            uint16_t output_size;
            bool active;
        } data;
        struct {
            TssDataCallback cb;
            uint64_t remaining_len;
            uint16_t remaining_cur_packet_len;
            bool active;
        } file;
        struct {
            TssDataCallback cb;
            uint8_t header_enabled;
            bool active;
        } log;
    } streaming;

    //Control/Status Info
    bool _in_bootloader;
    bool dirty; //Unknown setting state. Cached values may be incorrect.

    //Cached Data (Either useful for user or required for some functionality)
    uint64_t serial_number;

    void *user_data;
};

//------------------------INITIALIZATION----------------------------------------
TSS_API void tssCreateSensor(TSS_Sensor *sensor, struct TSS_Com_Class *com);
TSS_API int tssInitSensor(TSS_Sensor *sensor);

//-------------------------------MANUAL MANAGEMENT-----------------------------------------
TSS_API int sensorUpdateCachedSettings(TSS_Sensor *sensor);

/// @brief Should be called if changes are made to the sensor without using the sensor API.
/// The sensor will automatically reinitialize to a known state next time an API function is called
/// via the sensorUpdateCachedSettings function.
/// @note This does not work with the Minimal API. If using the Minimal API, you must call sensorUpdateCachedSettings
/// manually.
/// @param sensor The sensor that had modifications made
static inline void sensorMarkSettingsDirty(TSS_Sensor *sensor) {
    sensor->dirty = true;
}

//----------------------------SETTERS--------------------------------------
static inline void sensorSetDebugCallback(TSS_Sensor *sensor, TssDataCallback callback) {
    sensor->debug.cb = callback;
}
static inline void sensorSetUserData(TSS_Sensor *sensor, void *user_data) {
    sensor->user_data = user_data;
}

//----------------------------GETTERS--------------------------------------
static inline struct TSS_Header sensorGetLastHeader(const TSS_Sensor *sensor) {
    return sensor->last_header;
}
static inline struct TSS_Setting_Response sensorGetLastSettingResponse(const TSS_Sensor *sensor) {
    return sensor->last_write_setting_response;
}
static inline uint16_t sensorGetLastSettingsNumRead(const TSS_Sensor *sensor) {
    return sensor->last_num_settings_read;
}
static inline bool sensorIsStreaming(const TSS_Sensor *sensor) {
    return sensor->streaming.data.active || sensor->streaming.file.active || sensor->streaming.log.active;
}
static inline void* sensorGetUserData(const TSS_Sensor *sensor) {
    return sensor->user_data;
}

//--------------------------------BASE FUNCTIONALITY-----------------------------------------
TSS_API int sensorReadSettings(TSS_Sensor *sensor, const char *key_string, ...);
TSS_API int sensorReadSettingsV(TSS_Sensor *sensor, const char *key_string, va_list outputs);
TSS_API int sensorReadSettingsArray(TSS_Sensor *sensor, const char *key_string, void **outputs);
TSS_API int sensorReadSettingsQuery(TSS_Sensor *sensor, const char *key_string, TssGetSettingsCallback cb, void *user_data);
TSS_API int sensorWriteSettings(TSS_Sensor *sensor, const char **keys, uint8_t num_keys, const void **data);

TSS_API int sensorUpdateStreaming(TSS_Sensor *sensor);
TSS_API int sensorProcessDataStreamingCallbackOutput(TSS_Sensor *sensor, ...);
TSS_API int sensorProcessDataStreamingCallbackOutputArray(TSS_Sensor *sensor, void **outputs);
/// @brief Reads file streaming data inside the file streaming callback
/// @param sensor The sensor object
/// @param output Where to read the data to
/// @param size The size of output
/// @return The number of bytes read, or negative on error.
/// @note No data left when return value < size
TSS_API int sensorProcessFileStreamingCallbackOutput(TSS_Sensor *sensor, void *output, uint16_t size);

TSS_API int sensorProcessDebugCallbackOutput(TSS_Sensor *sensor, char *output, size_t size);

//--------------------------------CUSTOM COMMAND DECLARATIONS START--------------------------------------
TSS_API int sensorStreamingStart(TSS_Sensor *sensor, TssDataCallback cb);
TSS_API int sensorFileStreamingStart(TSS_Sensor *sensor, TssDataCallback cb, uint64_t *out_size);
TSS_API int sensorLoggingStart(TSS_Sensor *sensor, TssDataCallback cb);

TSS_API int sensorStreamingGetPacketArray(TSS_Sensor *sensor, void **outputs);

/// @brief Disconnects and reconnects to the sensor. May
/// require additional com class functionality (Reenumerate/Auto Detect)
/// @param sensor The sensor to connect to
/// @param timeout_ms How long to attempt to reconnect
/// @return TSS_SUCCESS if successfully connected
/// @warning On failure, the provided sensor object is in an undefined state
TSS_API int sensorReconnect(TSS_Sensor *sensor, uint32_t timeout_ms);
TSS_API int sensorCleanup(TSS_Sensor *sensor);

//--------------------------------CUSTOM COMMAND DECLARATIONS END--------------------------------------

//---------------------------------BOOTLOADER COMMANDS-------------------------------------------
struct TSS_Bootloader_Info {
    int32_t memstart;
    int32_t memend;
    int16_t pagesize;
    int16_t version; 
};

TSS_API int sensorBootloaderIsActive(TSS_Sensor *sensor, uint8_t *active);
TSS_API int sensorBootloaderGetSerialNumber(TSS_Sensor *sensor, uint64_t *serial_number);
TSS_API int sensorBootloaderLoadFirmware(TSS_Sensor *sensor, uint32_t timeout_ms);
TSS_API int sensorBootloaderEraseFirmware(TSS_Sensor *sensor, uint32_t timeout_ms);
TSS_API int sensorBootloaderGetInfo(TSS_Sensor *sensor, struct TSS_Bootloader_Info *info);
TSS_API int sensorBootloaderProgram(TSS_Sensor *sensor, const uint8_t *bytes, uint16_t num_bytes, uint32_t timeout_ms);
TSS_API int sensorBootloaderGetStatus(TSS_Sensor *sensor, uint32_t *status);
TSS_API int sensorBootloaderRestoreFactorySettings(TSS_Sensor *sensor);


//-------------------------------AUTO GENERATED COMMANDS START--------------------------------------------

TSS_API int sensorGetTaredOrientation(TSS_Sensor *sensor, float out_quat[4]);
TSS_API int sensorGetTaredOrientationAsEulerAngles(TSS_Sensor *sensor, float out_euler[3]);
TSS_API int sensorGetTaredOrientationAsRotationMatrix(TSS_Sensor *sensor, float out_matrix[9]);
TSS_API int sensorGetTaredOrientationAsAxisAngles(TSS_Sensor *sensor, float out_axis[3], float *out_angle);
TSS_API int sensorGetTaredOrientationAsTwoVector(TSS_Sensor *sensor, float out_forward[3], float out_down[3]);
TSS_API int sensorGetDifferenceQuaternion(TSS_Sensor *sensor, float out_quat[4]);
TSS_API int sensorGetUntaredOrientation(TSS_Sensor *sensor, float out_quat[4]);
TSS_API int sensorGetUntaredOrientationAsEulerAngles(TSS_Sensor *sensor, float out_euler[3]);
TSS_API int sensorGetUntaredOrientationAsRotationMatrix(TSS_Sensor *sensor, float out_matrix[9]);
TSS_API int sensorGetUntaredOrientationAsAxisAngles(TSS_Sensor *sensor, float out_axis[3], float *out_angle);
TSS_API int sensorGetUntaredOrientationAsTwoVector(TSS_Sensor *sensor, float out_forward[3], float out_down[3]);
TSS_API int sensorGetTaredTwoVectorInSensorFrame(TSS_Sensor *sensor, float out_north[3], float out_down[3]);
TSS_API int sensorGetUntaredTwoVectorInSensorFrame(TSS_Sensor *sensor, float out_north[3], float out_down[3]);
TSS_API int sensorGetPrimaryBarometerPressure(TSS_Sensor *sensor, float *out_mbar);
TSS_API int sensorGetPrimaryBarometerAltitude(TSS_Sensor *sensor, float *out_meters);
TSS_API int sensorGetBarometerAltitudeByID(TSS_Sensor *sensor, uint8_t id, float *out_meters);
TSS_API int sensorGetBarometerPressureByID(TSS_Sensor *sensor, uint8_t id, float *out_mbar);
TSS_API int sensorGetHeading(TSS_Sensor *sensor, float *out_heading);
TSS_API int sensorSetOffsetWithCurrentOrientation(TSS_Sensor *sensor);
TSS_API int sensorResetBaseOffset(TSS_Sensor *sensor);
TSS_API int sensorSetBaseOffsetWithCurrentOrientation(TSS_Sensor *sensor);
TSS_API int sensorGetInterruptStatus(TSS_Sensor *sensor, uint8_t *out_status);
TSS_API int sensorGetAllNormalizedComponentSensorData(TSS_Sensor *sensor, float out_gyro[3], float out_accel[3], float out_mag[3]);
TSS_API int sensorGetNormalizedGyroRateVector(TSS_Sensor *sensor, float out_gyro[3]);
TSS_API int sensorGetNormalizedAccelerometerVector(TSS_Sensor *sensor, float out_accel[3]);
TSS_API int sensorGetNormalizedMagnetometerVector(TSS_Sensor *sensor, float out_mag[3]);
TSS_API int sensorGetAllCorrectedComponentSensorData(TSS_Sensor *sensor, float out_gyro[3], float out_accel[3], float out_mag[3]);
TSS_API int sensorGetCorrectedGyroRateVector(TSS_Sensor *sensor, float out_gyro[3]);
TSS_API int sensorGetCorrectedAccelerometerVector(TSS_Sensor *sensor, float out_accel[3]);
TSS_API int sensorGetCorrectedMagnetometerVector(TSS_Sensor *sensor, float out_mag[3]);
TSS_API int sensorGetCorrectedGlobalLinearAcceleration(TSS_Sensor *sensor, float out_accel[3]);
TSS_API int sensorGetCorrectedLocalLinearAcceleration(TSS_Sensor *sensor, float out_accel[3]);
TSS_API int sensorGetTemperatureCelsius(TSS_Sensor *sensor, float *out_celsius);
TSS_API int sensorGetTemperatureFahrenheit(TSS_Sensor *sensor, float *out_fahrenheit);
TSS_API int sensorGetMotionlessConfidenceFactor(TSS_Sensor *sensor, float *out_confidence);
TSS_API int sensorCorrectRawGyroRateVector(TSS_Sensor *sensor, const float raw_gyro[3], uint8_t id, float out_corrected[3]);
TSS_API int sensorCorrectRawAccelerometerVector(TSS_Sensor *sensor, const float raw_accel[3], uint8_t id, float out_corrected[3]);
TSS_API int sensorCorrectRawMagnetometerVector(TSS_Sensor *sensor, const float raw_mag[3], uint8_t id, float out_corrected[3]);
TSS_API int sensorGetNormalizedGyroRateByID(TSS_Sensor *sensor, uint8_t id, float out_gyro[3]);
TSS_API int sensorGetNormalizedAccelerometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_accel[3]);
TSS_API int sensorGetNormalizedMagnetometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_mag[3]);
TSS_API int sensorGetCorrectedGyroRateByID(TSS_Sensor *sensor, uint8_t id, float out_gyro[3]);
TSS_API int sensorGetCorrectedAccelerometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_accel[3]);
TSS_API int sensorGetCorrectedMagnetometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_mag[3]);
TSS_API int sensorMassStorageControllerEnable(TSS_Sensor *sensor);
TSS_API int sensorMassStorageControllerDisable(TSS_Sensor *sensor);
TSS_API int sensorFormatSDCard(TSS_Sensor *sensor);
TSS_API int sensorLoggingStop(TSS_Sensor *sensor);
TSS_API int sensorSetClockValues(TSS_Sensor *sensor, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
TSS_API int sensorGetClockValues(TSS_Sensor *sensor, uint16_t *out_year, uint8_t *out_month, uint8_t *out_day, uint8_t *out_hour, uint8_t *out_minute, uint8_t *out_second);
TSS_API int sensorLoggingGetStatus(TSS_Sensor *sensor, uint8_t *out_status);
TSS_API int sensorGetRawGyroRateByID(TSS_Sensor *sensor, uint8_t id, float out_gyro[3]);
TSS_API int sensorGetRawAccelerometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_accel[3]);
TSS_API int sensorGetRawMagnetometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_mag[3]);
TSS_API int sensorEEPTSStart(TSS_Sensor *sensor);
TSS_API int sensorEEPTSStop(TSS_Sensor *sensor);
/// @brief Retrieves the oldest EEPTS output struct
/// @param sensor 
/// @return 
/// @note Use this with a struct TSS_EEPTS_Output and the TSS_EEPTS_OUT(&struct) macro
/// This is done instead of directly taking a struct to be consistent with how an EEPTS struct
/// is obtained when retrieving via the Data Streaming callback.
TSS_API int sensorEEPTSGetOldestStep(TSS_Sensor *sensor, uint32_t *out_segment_count, uint32_t *out_timestamp, double *out_longitude, double *out_latitude, float *out_altitude, float *out_heading, float *out_distance, float *out_distance_x, float *out_distance_y, float *out_distance_z, uint8_t *out_motion, uint8_t *out_location, float *out_confidence, float *out_overall_confidence);
/// @brief Retrieves the newest EEPTS output struct
/// @param sensor 
/// @return 
/// @note Use this with a struct TSS_EEPTS_Output and the TSS_EEPTS_OUT(&struct) macro
/// This is done instead of directly taking a struct to be consistent with how an EEPTS struct
/// is obtained when retrieving via the Data Streaming callback.
TSS_API int sensorEEPTSGetNewestStep(TSS_Sensor *sensor, uint32_t *out_segment_count, uint32_t *out_timestamp, double *out_longitude, double *out_latitude, float *out_altitude, float *out_heading, float *out_distance, float *out_distance_x, float *out_distance_y, float *out_distance_z, uint8_t *out_motion, uint8_t *out_location, float *out_confidence, float *out_overall_confidence);
TSS_API int sensorEEPTSGetAvailableStepCount(TSS_Sensor *sensor, uint8_t *out_count);
TSS_API int sensorEEPTSInsertGPS(TSS_Sensor *sensor, double Latitude, double Longitude);
TSS_API int sensorEEPTSAutoOffset(TSS_Sensor *sensor);
TSS_API int sensorStreamingGetCommandLabel(TSS_Sensor *sensor, uint8_t cmd_number, char *out_label, uint32_t size);
TSS_API int sensorStreamingGetPacket(TSS_Sensor *sensor, ...);
TSS_API int sensorStreamingStop(TSS_Sensor *sensor);
TSS_API int sensorLoggingPauseStreaming(TSS_Sensor *sensor, uint8_t pause);
TSS_API int sensorGetClockValuesString(TSS_Sensor *sensor, char *out_datetime, uint32_t size);
TSS_API int sensorGetTimestamp(TSS_Sensor *sensor, uint64_t *out_timestamp);
TSS_API int sensorSetTimestamp(TSS_Sensor *sensor, uint64_t timestamp);
TSS_API int sensorSetTareWithCurrentOrientation(TSS_Sensor *sensor);
TSS_API int sensorSetBaseTareWithCurrentOrientation(TSS_Sensor *sensor);
TSS_API int sensorResetFilter(TSS_Sensor *sensor);
TSS_API int sensorGetDebugMessageCount(TSS_Sensor *sensor, uint16_t *out_count);
TSS_API int sensorGetDebugMessage(TSS_Sensor *sensor, char *out_msg, uint32_t size);
TSS_API int sensorSelfTest(TSS_Sensor *sensor, uint32_t *out_result);
TSS_API int sensorSetTimesyncMode(TSS_Sensor *sensor, uint8_t mode);
TSS_API int sensorBeginPassiveCalibration(TSS_Sensor *sensor, uint8_t mode);
TSS_API int sensorGetPassiveCalibrationActive(TSS_Sensor *sensor, uint8_t *out_state);
TSS_API int sensorBeginActiveCalibration(TSS_Sensor *sensor);
TSS_API int sensorGetActiveCalibrationActive(TSS_Sensor *sensor, uint8_t *out_state);
TSS_API int sensorLoggingGetLastLiveLocation(TSS_Sensor *sensor, uint64_t *out_cursor_index, char *out_path, uint32_t size);
TSS_API int sensorFsGetNextDirectoryItem(TSS_Sensor *sensor, uint8_t *out_type, char *out_name, uint32_t size, uint64_t *out_size);
TSS_API int sensorFsChangeDirectory(TSS_Sensor *sensor, const char *path);
TSS_API int sensorFsOpenFile(TSS_Sensor *sensor, const char *path);
TSS_API int sensorCloseFile(TSS_Sensor *sensor);
TSS_API int sensorFileGetRemainingSize(TSS_Sensor *sensor, uint64_t *out_size);
TSS_API int sensorFileReadLine(TSS_Sensor *sensor, char *out_line, uint32_t size);
TSS_API int sensorFileReadBytes(TSS_Sensor *sensor, uint16_t len, uint8_t *out_data);
TSS_API int sensorFsDeleteFileOrFolder(TSS_Sensor *sensor, const char *path);
TSS_API int sensorFileSetCursorIndex(TSS_Sensor *sensor, uint64_t index);
TSS_API int sensorFileStreamingStop(TSS_Sensor *sensor);
TSS_API int sensorBatteryGetCurrent(TSS_Sensor *sensor, int16_t *out_current);
TSS_API int sensorBatteryGetVoltage(TSS_Sensor *sensor, float *out_voltage);
TSS_API int sensorBatteryGetPercent(TSS_Sensor *sensor, uint8_t *out_percent);
TSS_API int sensorBatteryGetStatus(TSS_Sensor *sensor, uint8_t *out_status);
TSS_API int sensorWMMGetField(TSS_Sensor *sensor, float *out_declination, float *out_inclination, float *out_F, float *out_H, float *out_N, float *out_E, float *out_D);
TSS_API int sensorWMMUpdatePosition(TSS_Sensor *sensor);
TSS_API int sensorWMMUpdateDate(TSS_Sensor *sensor);
TSS_API int sensorGPSGetIsActive(TSS_Sensor *sensor);
TSS_API int sensorGPSGetLatitudeandLongitude(TSS_Sensor *sensor, double *out_latitude, double *out_longitude);
TSS_API int sensorGPSGetAltitude(TSS_Sensor *sensor, float *out_meters);
TSS_API int sensorGPSGetFixStatus(TSS_Sensor *sensor, uint8_t *out_fix);
TSS_API int sensorGPSGetHDOP(TSS_Sensor *sensor, uint8_t *out_hdop);
TSS_API int sensorGPSGetSatellites(TSS_Sensor *sensor, uint8_t *out_num_satellites);
TSS_API int sensorGPSReset(TSS_Sensor *sensor, uint8_t mode);
TSS_API int sensorGPSSetData(TSS_Sensor *sensor, double latitude, double longitude, float altitude);
TSS_API int sensorCommitSettings(TSS_Sensor *sensor);
TSS_API int sensorSoftwareReset(TSS_Sensor *sensor);
TSS_API int sensorEnterBootloader(TSS_Sensor *sensor);
TSS_API int sensorGetCurrentLEDColor(TSS_Sensor *sensor, float out_color[3]);
TSS_API int sensorGetButtonState(TSS_Sensor *sensor, uint8_t *out_state);
//-------------------------------AUTO GENERATED COMMANDS END--------------------------------------------

//--------------------------------AUTO GENERATED SETTINGS START-----------------------------------------

TSS_API int sensorRestoreDefaultSettings(TSS_Sensor *sensor);
TSS_API int sensorReadAllSettings(TSS_Sensor *sensor, TssGetSettingsCallback cb, void *user_data);
TSS_API int sensorReadAllWritableSettings(TSS_Sensor *sensor, TssGetSettingsCallback cb, void *user_data);
TSS_API int sensorReadSerialNumber(TSS_Sensor *sensor, uint64_t *out);
TSS_API int sensorWriteTimestamp(TSS_Sensor *sensor, uint64_t microseconds);
TSS_API int sensorReadTimestamp(TSS_Sensor *sensor, uint64_t *out_microseconds);
TSS_API int sensorWriteLedMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLedMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLedRgb(TSS_Sensor *sensor, const float value[3]);
TSS_API int sensorReadLedRgb(TSS_Sensor *sensor, float out[3]);
TSS_API int sensorReadVersionFirmware(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadVersionHardware(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadUpdateRateSensor(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWriteHeader(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadHeader(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteHeaderStatusEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadHeaderStatusEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteHeaderTimestampEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadHeaderTimestampEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteHeaderEchoEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadHeaderEchoEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteHeaderChecksumEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadHeaderChecksumEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteHeaderSerialEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadHeaderSerialEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteHeaderLengthEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadHeaderLengthEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorReadValidCommands(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteTimesyncAccelThreshold(TSS_Sensor *sensor, float value);
TSS_API int sensorReadTimesyncAccelThreshold(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteTimesyncStartupMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadTimesyncStartupMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteCpuSpeed(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadCpuSpeed(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorReadCpuSpeedCur(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePmMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorWritePmIdleEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPmIdleEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteStreamSlots(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadStreamSlots(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteStreamInterval(TSS_Sensor *sensor, uint64_t value);
TSS_API int sensorReadStreamInterval(TSS_Sensor *sensor, uint64_t *out);
TSS_API int sensorWriteStreamHz(TSS_Sensor *sensor, float value);
TSS_API int sensorReadStreamHz(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteStreamDuration(TSS_Sensor *sensor, float value);
TSS_API int sensorReadStreamDuration(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteStreamDelay(TSS_Sensor *sensor, float value);
TSS_API int sensorReadStreamDelay(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteStreamMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadStreamMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteStreamCount(TSS_Sensor *sensor, uint64_t value);
TSS_API int sensorReadStreamCount(TSS_Sensor *sensor, uint64_t *out);
TSS_API int sensorReadStreamableCommands(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteDebugLevel(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadDebugLevel(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWriteDebugModule(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadDebugModule(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWriteDebugMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadDebugMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteDebugLed(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadDebugLed(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteDebugFault(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadDebugFault(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteDebugWdt(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadDebugWdt(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteAxisOrder(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadAxisOrder(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteAxisOrderC(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadAxisOrderC(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteAxisOffsetEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadAxisOffsetEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteEulerOrder(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadEulerOrder(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadUpdateRateFilter(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorReadUpdateRateSms(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWriteOffset(TSS_Sensor *sensor, const float value[4]);
TSS_API int sensorReadOffset(TSS_Sensor *sensor, float out[4]);
TSS_API int sensorWriteBaseOffset(TSS_Sensor *sensor, const float value[4]);
TSS_API int sensorReadBaseOffset(TSS_Sensor *sensor, float out[4]);
TSS_API int sensorWriteTareQuat(TSS_Sensor *sensor, const float value[4]);
TSS_API int sensorReadTareQuat(TSS_Sensor *sensor, float out[4]);
TSS_API int sensorWriteTareAutoBase(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadTareAutoBase(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteBaseTare(TSS_Sensor *sensor, const float value[4]);
TSS_API int sensorReadBaseTare(TSS_Sensor *sensor, float out[4]);
TSS_API int sensorWriteTareMat(TSS_Sensor *sensor, const float value[9]);
TSS_API int sensorReadTareMat(TSS_Sensor *sensor, float out[9]);
TSS_API int sensorWriteRunningAvgOrient(TSS_Sensor *sensor, float value);
TSS_API int sensorReadRunningAvgOrient(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteFilterMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadFilterMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteFilterMrefMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadFilterMrefMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteFilterMref(TSS_Sensor *sensor, const float value[3]);
TSS_API int sensorReadFilterMref(TSS_Sensor *sensor, float out[3]);
TSS_API int sensorWriteFilterMrefGps(TSS_Sensor *sensor, const double value[2]);
TSS_API int sensorWriteFilterMrefDip(TSS_Sensor *sensor, float value);
TSS_API int sensorReadFilterMrefDip(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteFilterConfThresholds(TSS_Sensor *sensor, float min, float max, float cap);
TSS_API int sensorReadFilterConfThresholds(TSS_Sensor *sensor, float *out_min, float *out_max, float *out_cap);
TSS_API int sensorWriteHeadingVector(TSS_Sensor *sensor, const float value[3]);
TSS_API int sensorReadHeadingVector(TSS_Sensor *sensor, float out[3]);
TSS_API int sensorWriteHeadingVectorAxis(TSS_Sensor *sensor, const char *value);
TSS_API int sensorWriteHeadingOffset(TSS_Sensor *sensor, float offset, uint8_t mode);
TSS_API int sensorReadHeadingOffset(TSS_Sensor *sensor, float *out_offset, uint8_t *out_mode);
TSS_API int sensorReadValidAccels(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadValidGyros(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadValidMags(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadValidBaros(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadValidComponents(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWritePrimaryAccel(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadPrimaryAccel(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWritePrimaryGyro(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadPrimaryGyro(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWritePrimaryMag(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadPrimaryMag(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWritePrimarySensorRfade(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPrimarySensorRfade(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteMagBiasMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadMagBiasMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteOdrAll(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorWriteOdrAccelAll(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorWriteOdrGyroAll(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorWriteOdrMagAll(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorWriteOdrBaroAll(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorWriteAccelEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadAccelEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteGyroEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadGyroEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteMagEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadMagEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteCalibMatAccel(TSS_Sensor *sensor, uint8_t id, const float value[9]);
TSS_API int sensorReadCalibMatAccel(TSS_Sensor *sensor, uint8_t id, float out[9]);
TSS_API int sensorWriteCalibBiasAccel(TSS_Sensor *sensor, uint8_t id, const float value[3]);
TSS_API int sensorReadCalibBiasAccel(TSS_Sensor *sensor, uint8_t id, float out[3]);
TSS_API int sensorWriteRangeAccel(TSS_Sensor *sensor, uint8_t id, uint16_t value);
TSS_API int sensorReadRangeAccel(TSS_Sensor *sensor, uint8_t id, uint16_t *out);
TSS_API int sensorReadValidRangesAccel(TSS_Sensor *sensor, uint8_t id, char *out, uint32_t size);
TSS_API int sensorWriteOversampleAccel(TSS_Sensor *sensor, uint8_t id, uint16_t value);
TSS_API int sensorReadOversampleAccel(TSS_Sensor *sensor, uint8_t id, uint16_t *out);
TSS_API int sensorWriteRunningAvgAccel(TSS_Sensor *sensor, uint8_t id, float value);
TSS_API int sensorReadRunningAvgAccel(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorWriteOdrAccel(TSS_Sensor *sensor, uint8_t id, uint32_t value);
TSS_API int sensorReadOdrAccel(TSS_Sensor *sensor, uint8_t id, uint32_t *out);
TSS_API int sensorReadUpdateRateAccel(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorReadNoiseProfileAccel(TSS_Sensor *sensor, uint8_t id, float *out_noise_density, float *out_variance, float *out_bandwidth, float *out_sensitivity, uint32_t *out_odr);
TSS_API int sensorWriteCalibMatGyro(TSS_Sensor *sensor, uint8_t id, const float value[9]);
TSS_API int sensorReadCalibMatGyro(TSS_Sensor *sensor, uint8_t id, float out[9]);
TSS_API int sensorWriteCalibBiasGyro(TSS_Sensor *sensor, uint8_t id, const float value[3]);
TSS_API int sensorReadCalibBiasGyro(TSS_Sensor *sensor, uint8_t id, float out[3]);
TSS_API int sensorWriteRangeGyro(TSS_Sensor *sensor, uint8_t id, uint16_t value);
TSS_API int sensorReadRangeGyro(TSS_Sensor *sensor, uint8_t id, uint16_t *out);
TSS_API int sensorReadValidRangesGyro(TSS_Sensor *sensor, uint8_t id, char *out, uint32_t size);
TSS_API int sensorWriteOversampleGyro(TSS_Sensor *sensor, uint8_t id, uint16_t value);
TSS_API int sensorReadOversampleGyro(TSS_Sensor *sensor, uint8_t id, uint16_t *out);
TSS_API int sensorWriteRunningAvgGyro(TSS_Sensor *sensor, uint8_t id, float value);
TSS_API int sensorReadRunningAvgGyro(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorWriteOdrGyro(TSS_Sensor *sensor, uint8_t id, uint32_t value);
TSS_API int sensorReadOdrGyro(TSS_Sensor *sensor, uint8_t id, uint32_t *out);
TSS_API int sensorReadUpdateRateGyro(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorReadNoiseProfileGyro(TSS_Sensor *sensor, uint8_t id, float *out_noise_density, float *out_variance, float *out_bandwidth, float *out_sensitivity, uint32_t *out_odr);
TSS_API int sensorWriteCalibMatMag(TSS_Sensor *sensor, uint8_t id, const float value[9]);
TSS_API int sensorReadCalibMatMag(TSS_Sensor *sensor, uint8_t id, float out[9]);
TSS_API int sensorWriteCalibBiasMag(TSS_Sensor *sensor, uint8_t id, const float value[3]);
TSS_API int sensorReadCalibBiasMag(TSS_Sensor *sensor, uint8_t id, float out[3]);
TSS_API int sensorWriteRangeMag(TSS_Sensor *sensor, uint8_t id, uint16_t value);
TSS_API int sensorReadRangeMag(TSS_Sensor *sensor, uint8_t id, uint16_t *out);
TSS_API int sensorReadValidRangesMag(TSS_Sensor *sensor, uint8_t id, char *out, uint32_t size);
TSS_API int sensorWriteOversampleMag(TSS_Sensor *sensor, uint8_t id, uint16_t value);
TSS_API int sensorReadOversampleMag(TSS_Sensor *sensor, uint8_t id, uint16_t *out);
TSS_API int sensorWriteRunningAvgMag(TSS_Sensor *sensor, uint8_t id, float value);
TSS_API int sensorReadRunningAvgMag(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorWriteOdrMag(TSS_Sensor *sensor, uint8_t id, uint32_t value);
TSS_API int sensorReadOdrMag(TSS_Sensor *sensor, uint8_t id, uint32_t *out);
TSS_API int sensorReadUpdateRateMag(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorReadNoiseProfileMag(TSS_Sensor *sensor, uint8_t id, float *out_noise_density, float *out_variance, float *out_bandwidth, float *out_sensitivity, uint32_t *out_odr);
TSS_API int sensorWriteCalibBiasBaro(TSS_Sensor *sensor, uint8_t id, float value);
TSS_API int sensorReadCalibBiasBaro(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorWriteCalibAltitudeBaro(TSS_Sensor *sensor, uint8_t id, float value);
TSS_API int sensorWriteOdrBaro(TSS_Sensor *sensor, uint8_t id, uint32_t value);
TSS_API int sensorReadOdrBaro(TSS_Sensor *sensor, uint8_t id, uint32_t *out);
TSS_API int sensorReadUpdateRateBaro(TSS_Sensor *sensor, uint8_t id, float *out);
TSS_API int sensorWritePtsOffsetQuat(TSS_Sensor *sensor, const float value[4]);
TSS_API int sensorReadPtsOffsetQuat(TSS_Sensor *sensor, float out[4]);
TSS_API int sensorRestorePtsDefaultSettings(TSS_Sensor *sensor);
TSS_API int sensorReadPtsSettings(TSS_Sensor *sensor, TssGetSettingsCallback cb, void *user_data);
TSS_API int sensorWritePtsPresetHand(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorWritePtsPresetMotion(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorWritePtsPresetHeading(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorWritePtsDebugLevel(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsDebugLevel(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsDebugModule(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsDebugModule(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsHeadingMode(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsHeadingMode(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsInitialHeadingMode(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsInitialHeadingMode(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsHandHeadingMode(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsHandHeadingMode(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsMagDeclination(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsMagDeclination(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePtsAutoDeclination(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsAutoDeclination(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsDiscardSlow(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsDiscardSlow(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsSegmentAxis(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsSegmentAxis(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsSegNoise(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsSegNoise(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePtsClassifierMode(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsClassifierMode(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsClassifierMode2(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsClassifierMode2(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsLocationClassifierMode(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsLocationClassifierMode(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsHandClassifierThreshold(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsHandClassifierThreshold(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePtsDisabledTruthMotions(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsDisabledTruthMotions(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsDynamicSegmenterEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsDynamicSegmenterEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsEstimatorScalars(TSS_Sensor *sensor, const float value[7]);
TSS_API int sensorReadPtsEstimatorScalars(TSS_Sensor *sensor, float out[7]);
TSS_API int sensorWritePtsAutoEstimatorScalarRate(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsAutoEstimatorScalarRate(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsRunningCorrection(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsRunningCorrection(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsHandCorrection(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsHandCorrection(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsHeadingCorrectionMode(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsHeadingCorrectionMode(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsHeadingMinDif(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsHeadingMinDif(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePtsHeadingResetConsistencies(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsHeadingResetConsistencies(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsHeadingBacktrackEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsHeadingBacktrackEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsMotionCorrectionRadius(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsMotionCorrectionRadius(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsMotionCorrectionConsistencyReq(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadPtsMotionCorrectionConsistencyReq(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWritePtsOrientRefYThreshold(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsOrientRefYThreshold(TSS_Sensor *sensor, float *out);
TSS_API int sensorReadPtsVersion(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWritePtsDate(TSS_Sensor *sensor, uint32_t day, uint32_t month, uint32_t year);
TSS_API int sensorReadPtsDate(TSS_Sensor *sensor, uint32_t *out_day, uint32_t *out_month, uint32_t *out_year);
TSS_API int sensorReadPtsWmmVersion(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWritePtsWmmSet(TSS_Sensor *sensor, const char *value);
TSS_API int sensorWritePtsForceOutGps(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsForceOutGps(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePtsInitialHeadingTolerance(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsInitialHeadingTolerance(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePtsHeadingConsistencyReq(TSS_Sensor *sensor, int32_t value);
TSS_API int sensorReadPtsHeadingConsistencyReq(TSS_Sensor *sensor, int32_t *out);
TSS_API int sensorWritePtsHeadingRootErrMul(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsHeadingRootErrMul(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePtsHeadingConsistentBias(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPtsHeadingConsistentBias(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePtsStrictBiasEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPtsStrictBiasEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePinMode0(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPinMode0(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePinMode1(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPinMode1(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteUartBaudrate(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadUartBaudrate(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWriteI2cAddr(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadI2cAddr(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePowerHoldTime(TSS_Sensor *sensor, float value);
TSS_API int sensorReadPowerHoldTime(TSS_Sensor *sensor, float *out);
TSS_API int sensorWritePowerHoldState(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPowerHoldState(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWritePowerInitialHoldState(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadPowerInitialHoldState(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorFsCfgLoad(TSS_Sensor *sensor);
TSS_API int sensorWriteFsMscEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadFsMscEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteFsMscAuto(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadFsMscAuto(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogSlots(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadLogSlots(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteLogInterval(TSS_Sensor *sensor, uint64_t value);
TSS_API int sensorReadLogInterval(TSS_Sensor *sensor, uint64_t *out);
TSS_API int sensorWriteLogHz(TSS_Sensor *sensor, float value);
TSS_API int sensorReadLogHz(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteLogStartEvent(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadLogStartEvent(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteLogStartMotionThreshold(TSS_Sensor *sensor, float value);
TSS_API int sensorReadLogStartMotionThreshold(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteLogStopEvent(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadLogStopEvent(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteLogStopMotionThreshold(TSS_Sensor *sensor, float value);
TSS_API int sensorReadLogStopMotionThreshold(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteLogStopMotionDelay(TSS_Sensor *sensor, float value);
TSS_API int sensorReadLogStopMotionDelay(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteLogStopCount(TSS_Sensor *sensor, uint64_t value);
TSS_API int sensorReadLogStopCount(TSS_Sensor *sensor, uint64_t *out);
TSS_API int sensorWriteLogStopDuration(TSS_Sensor *sensor, float value);
TSS_API int sensorReadLogStopDuration(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteLogStopPeriodCount(TSS_Sensor *sensor, uint32_t value);
TSS_API int sensorReadLogStopPeriodCount(TSS_Sensor *sensor, uint32_t *out);
TSS_API int sensorWriteLogStyle(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogStyle(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogPeriodicCaptureTime(TSS_Sensor *sensor, float value);
TSS_API int sensorReadLogPeriodicCaptureTime(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteLogPeriodicRestTime(TSS_Sensor *sensor, float value);
TSS_API int sensorReadLogPeriodicRestTime(TSS_Sensor *sensor, float *out);
TSS_API int sensorWriteLogBaseFilename(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadLogBaseFilename(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorWriteLogFileMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogFileMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogDataMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogDataMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogOutputSettings(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogOutputSettings(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogHeaderEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogHeaderEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogFolderMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogFolderMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogImmediateOutput(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogImmediateOutput(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogImmediateOutputHeaderEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogImmediateOutputHeaderEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteLogImmediateOutputHeaderMode(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadLogImmediateOutputHeaderMode(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteRtcYear(TSS_Sensor *sensor, uint16_t value);
TSS_API int sensorReadRtcYear(TSS_Sensor *sensor, uint16_t *out);
TSS_API int sensorWriteRtcMonth(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadRtcMonth(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteRtcDay(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadRtcDay(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteRtcHour(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadRtcHour(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteRtcMinute(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadRtcMinute(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteRtcSecond(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadRtcSecond(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteRtcDatetime(TSS_Sensor *sensor, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
TSS_API int sensorReadRtcDatetime(TSS_Sensor *sensor, uint16_t *out_year, uint8_t *out_month, uint8_t *out_day, uint8_t *out_hour, uint8_t *out_minute, uint8_t *out_second);
TSS_API int sensorWriteRtcSource(TSS_Sensor *sensor, uint8_t mode);
TSS_API int sensorReadRtcSource(TSS_Sensor *sensor, uint8_t *out_mode);
TSS_API int sensorWriteUtcOffset(TSS_Sensor *sensor, float offset);
TSS_API int sensorReadUtcOffset(TSS_Sensor *sensor, float *out_offset);
TSS_API int sensorWriteBatChgRate(TSS_Sensor *sensor, uint16_t value);
TSS_API int sensorReadBatChgRate(TSS_Sensor *sensor, uint16_t *out);
TSS_API int sensorWriteBatColdThreshold(TSS_Sensor *sensor, float temperature_c, float chg_rate);
TSS_API int sensorReadBatColdThreshold(TSS_Sensor *sensor, float *out_temperature_c, float *out_chg_rate);
TSS_API int sensorWriteBatWarmThreshold(TSS_Sensor *sensor, float temperature_c, float chg_rate);
TSS_API int sensorReadBatWarmThreshold(TSS_Sensor *sensor, float *out_temperature_c, float *out_chg_rate);
TSS_API int sensorWriteBatHotThreshold(TSS_Sensor *sensor, float temperature_c, float chg_rate);
TSS_API int sensorReadBatHotThreshold(TSS_Sensor *sensor, float *out_temperature_c, float *out_chg_rate);
TSS_API int sensorWriteBatOffsetThreshold(TSS_Sensor *sensor, float value);
TSS_API int sensorReadBatOffsetThreshold(TSS_Sensor *sensor, float *out);
TSS_API int sensorReadBatMah(TSS_Sensor *sensor, uint16_t *out);
TSS_API int sensorWriteBleName(TSS_Sensor *sensor, const char *value);
TSS_API int sensorReadBleName(TSS_Sensor *sensor, char *out, uint32_t size);
TSS_API int sensorReadBleConnected(TSS_Sensor *sensor, int8_t *out);
TSS_API int sensorBleDisconnect(TSS_Sensor *sensor);
TSS_API int sensorWriteGpsStandby(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadGpsStandby(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteGpsLed(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadGpsLed(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteGpsPeriodicFixTimes(TSS_Sensor *sensor, const uint16_t value[2]);
TSS_API int sensorReadGpsPeriodicFixTimes(TSS_Sensor *sensor, uint16_t out[2]);
TSS_API int sensorWriteGpsPeriodicUnfixedTimes(TSS_Sensor *sensor, const uint16_t value[2]);
TSS_API int sensorReadGpsPeriodicUnfixedTimes(TSS_Sensor *sensor, uint16_t out[2]);
TSS_API int sensorWriteGpsPeriodicEnabled(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadGpsPeriodicEnabled(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteRtcGpsSyncInterval(TSS_Sensor *sensor, uint64_t value);
TSS_API int sensorReadRtcGpsSyncInterval(TSS_Sensor *sensor, uint64_t *out);
TSS_API int sensorWriteRtcGpsForceSync(TSS_Sensor *sensor, uint8_t value);
TSS_API int sensorReadRtcGpsForceSync(TSS_Sensor *sensor, uint8_t *out);
TSS_API int sensorWriteUsbTimeout(TSS_Sensor *sensor, float value);
TSS_API int sensorReadUsbTimeout(TSS_Sensor *sensor, float *out);
//--------------------------------AUTO GENERATED SETTINGS END-----------------------------------------

#ifdef __cplusplus
}
#endif

#endif /* __TSS_SENSOR_H__ */