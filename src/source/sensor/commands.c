#include "tss/api/sensor.h"
#include "internal.h"
#include "tss/errors.h"
#include "tss/constants.h"

//---------------------------------------CUSTOM STREAMING------------------------------------------------

int sensorStreamingGeneratePacket(TSS_Sensor *sensor, ...) {
    va_list outputs;
    int result;

    va_start(outputs, sensor);
    result = sensorInternalExecuteCommandCustomV(sensor, tssGetCommand(84), NULL, sensorInternalProcessStreamingBatch, outputs);
    va_end(outputs);
    return result;
}

int sensorStreamingGeneratePacketArray(TSS_Sensor *sensor, void **outputs) {
    return sensorInternalExecuteCommandCustomArray(sensor, tssGetCommand(84), NULL, sensorInternalProcessStreamingBatchArray, outputs);
}

int sensorStreamingStart(TSS_Sensor *sensor, TssDataCallback cb) {
    int err;
    if(cb == NULL) return TSS_ERR_INVALID_STREAM_CALLBACK;
    err = sensorInternalExecuteCommand(sensor, tssGetCommand(85), NULL);
    if(!err) {
        sensor->streaming.data.active = true;
        sensor->streaming.data.cb = cb;
    }
    return err;
}

int sensorStreamingStop(TSS_Sensor *sensor) {
    sensor->streaming.data.active = false;
    return sensorInternalExecuteCommand(sensor, tssGetCommand(86), NULL);
}

//-------------------------------CUSTOM FILE-------------------------------

int sensorFileStreamingStart(TSS_Sensor *sensor, TssDataCallback cb, uint64_t *out_size) {
    int err;
    uint64_t file_len;

    if(cb == NULL) return TSS_ERR_INVALID_STREAM_CALLBACK;
    err = sensorInternalExecuteCommand(sensor, tssGetCommand(180), NULL, &file_len);
    if(!err) {
        sensor->streaming.file.active = true;
        sensor->streaming.file.cb = cb;
        sensor->streaming.file.remaining_len = file_len;
        if(out_size != NULL) {
            *out_size = file_len;
        }
    }
    return err;
}

int sensorFileStreamingStop(TSS_Sensor *sensor) {
    sensor->streaming.file.active = false;
    return sensorInternalExecuteCommand(sensor, tssGetCommand(181), NULL);
}

int sensorFileReadBytes(TSS_Sensor *sensor, uint16_t len, uint8_t *out_data) {
    //Build a version of the command with the out format set based on the incoming length
    struct TSS_Command readBytesCommand = *tssGetCommand(177);
    const struct TSS_Param params[] = { TSS_PARAM(1, len, TSS_ParamTypeBlob), TSS_PARAM_NULL };
    readBytesCommand.out_format = params;
    return sensorInternalExecuteCommand(sensor, &readBytesCommand, (const void*[]) { &len }, out_data);
}

//------------------------------CUSTOM LOGGING-----------------------------------

int sensorLoggingStart(TSS_Sensor *sensor, TssDataCallback cb) {
    uint8_t immediate_output, status;
    int err = sensorReadLogImmediateOutput(sensor, &immediate_output);
    if(err < 0) return err;

    if(immediate_output) {
#if !TSS_MINIMAL_SENSOR
        //For log immediate output to be correctly read, must have header enabled
        //with at least data_len enabled. Data_len is already guranteed enabled by
        //the base API so it is not set/checked here.
        err = sensorWriteLogImmediateOutputHeaderEnabled(sensor, 1);
        if(err) return err;
        err = sensorWriteLogImmediateOutputHeaderMode(sensor, TSS_OUTPUT_MODE_BINARY);
        if(err) return err;
        sensor->streaming.log.header_enabled = true;
#else
        err = sensorReadLogImmediateOutputHeaderEnabled(sensor, &sensor->streaming.log.header_enabled);
        if(err < 0) return err;
#endif        
    }
    err = sensorInternalExecuteCommand(sensor, tssGetCommand(60), NULL);
    if(err) return err;
    //Because we are not forcing the status bit in the header to be enabled, we check here instead
    //for success of the command.
    err = sensorLoggingGetStatus(sensor, &status);
    if(err) return err;
    if(status != 1) return TSS_ERR_FAILED_START_LOGGING;

    //Success!

    if(immediate_output) {
        sensor->streaming.log.active = true;
        sensor->streaming.log.cb = cb;
    }

    return TSS_SUCCESS;
}

int sensorLoggingStop(TSS_Sensor *sensor) {
    sensor->streaming.log.active = false;
    return sensorInternalExecuteCommand(sensor, tssGetCommand(61), NULL);
}

int sensorSoftwareReset(TSS_Sensor *sensor) {
    bool header_was_enabled;

    header_was_enabled = sensor->_header_enabled;
    sensor->_header_enabled = false;
    sensorInternalExecuteCommand(sensor, tssGetCommand(226), NULL);
    sensor->_header_enabled = header_was_enabled;

    //TODO: Should this be provided as a parameter? Or should the reconnect just be expected to be called separate?
    ///Im fine with either. Main issue with NOT having the reconnect in the function is if someone calls reset and doesn't know
    //to call reconnect.
    return sensorReconnect(sensor, 2000);
}

int sensorEnterBootloader(TSS_Sensor *sensor) {
    uint8_t header_was_enabled;
    if(sensor->_in_bootloader) return TSS_SUCCESS;

    header_was_enabled = sensor->_header_enabled;
    sensor->_header_enabled = false;
    sensorInternalExecuteCommand(sensor, tssGetCommand(229), NULL);
    sensor->_header_enabled = header_was_enabled;

    //TODO: Same question as above
    if(sensorReconnect(sensor, 2000) != TSS_SUCCESS) {
        return TSS_ERR_DETECTION;
    }

    //Ensure entered bootloader, will have been set
    //when reconnecting above.
    if(!sensor->_in_bootloader) {
        return TSS_ERR_NOT_IN_BOOTLOADER;
    }

    tss_com_clear_immediate(sensor->com);

    return TSS_SUCCESS;
}

//-------------------------------------TODO-----------------------------------------------

//-----------------------------------AUTO GENERATED START--------------------------------------------

int sensorGetTaredOrientation(TSS_Sensor *sensor, float out_quat[4]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(0), NULL, out_quat);
}

int sensorGetTaredOrientationAsEulerAngles(TSS_Sensor *sensor, float out_euler[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(1), NULL, out_euler);
}

int sensorGetTaredOrientationAsRotationMatrix(TSS_Sensor *sensor, float out_matrix[9]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(2), NULL, out_matrix);
}

int sensorGetTaredOrientationAsAxisAngles(TSS_Sensor *sensor, float out_axis[3], float *out_angle) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(3), NULL, out_axis, out_angle);
}

int sensorGetTaredOrientationAsTwoVector(TSS_Sensor *sensor, float out_forward[3], float out_down[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(4), NULL, out_forward, out_down);
}

int sensorGetDifferenceQuaternion(TSS_Sensor *sensor, float out_quat[4]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(5), NULL, out_quat);
}

int sensorGetUntaredOrientation(TSS_Sensor *sensor, float out_quat[4]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(6), NULL, out_quat);
}

int sensorGetUntaredOrientationAsEulerAngles(TSS_Sensor *sensor, float out_euler[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(7), NULL, out_euler);
}

int sensorGetUntaredOrientationAsRotationMatrix(TSS_Sensor *sensor, float out_matrix[9]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(8), NULL, out_matrix);
}

int sensorGetUntaredOrientationAsAxisAngles(TSS_Sensor *sensor, float out_axis[3], float *out_angle) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(9), NULL, out_axis, out_angle);
}

int sensorGetUntaredOrientationAsTwoVector(TSS_Sensor *sensor, float out_forward[3], float out_down[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(10), NULL, out_forward, out_down);
}

int sensorGetTaredTwoVectorInSensorFrame(TSS_Sensor *sensor, float out_north[3], float out_down[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(11), NULL, out_north, out_down);
}

int sensorGetUntaredTwoVectorInSensorFrame(TSS_Sensor *sensor, float out_north[3], float out_down[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(12), NULL, out_north, out_down);
}

int sensorGetPrimaryBarometerPressure(TSS_Sensor *sensor, float *out_mbar) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(13), NULL, out_mbar);
}

int sensorGetPrimaryBarometerAltitude(TSS_Sensor *sensor, float *out_meters) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(14), NULL, out_meters);
}

int sensorGetBarometerAltitudeByID(TSS_Sensor *sensor, uint8_t id, float *out_meters) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(15), (const void*[]) { &id }, out_meters);
}

int sensorGetBarometerPressureByID(TSS_Sensor *sensor, uint8_t id, float *out_mbar) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(16), (const void*[]) { &id }, out_mbar);
}

int sensorGetHeading(TSS_Sensor *sensor, float *out_heading) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(17), NULL, out_heading);
}

int sensorSetOffsetWithCurrentOrientation(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(19), NULL);
}

int sensorResetBaseOffset(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(20), NULL);
}

int sensorSetBaseOffsetWithCurrentOrientation(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(22), NULL);
}

int sensorGetInterruptStatus(TSS_Sensor *sensor, uint8_t *out_status) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(31), NULL, out_status);
}

int sensorGetAllNormalizedComponentSensorData(TSS_Sensor *sensor, float out_gyro[3], float out_accel[3], float out_mag[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(32), NULL, out_gyro, out_accel, out_mag);
}

int sensorGetNormalizedGyroRateVector(TSS_Sensor *sensor, float out_gyro[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(33), NULL, out_gyro);
}

int sensorGetNormalizedAccelerometerVector(TSS_Sensor *sensor, float out_accel[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(34), NULL, out_accel);
}

int sensorGetNormalizedMagnetometerVector(TSS_Sensor *sensor, float out_mag[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(35), NULL, out_mag);
}

int sensorGetAllCorrectedComponentSensorData(TSS_Sensor *sensor, float out_gyro[3], float out_accel[3], float out_mag[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(37), NULL, out_gyro, out_accel, out_mag);
}

int sensorGetCorrectedGyroRateVector(TSS_Sensor *sensor, float out_gyro[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(38), NULL, out_gyro);
}

int sensorGetCorrectedAccelerometerVector(TSS_Sensor *sensor, float out_accel[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(39), NULL, out_accel);
}

int sensorGetCorrectedMagnetometerVector(TSS_Sensor *sensor, float out_mag[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(40), NULL, out_mag);
}

int sensorGetCorrectedGlobalLinearAcceleration(TSS_Sensor *sensor, float out_accel[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(41), NULL, out_accel);
}

int sensorGetCorrectedLocalLinearAcceleration(TSS_Sensor *sensor, float out_accel[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(42), NULL, out_accel);
}

int sensorGetTemperatureCelsius(TSS_Sensor *sensor, float *out_celsius) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(43), NULL, out_celsius);
}

int sensorGetTemperatureFahrenheit(TSS_Sensor *sensor, float *out_fahrenheit) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(44), NULL, out_fahrenheit);
}

int sensorGetMotionlessConfidenceFactor(TSS_Sensor *sensor, float *out_confidence) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(45), NULL, out_confidence);
}

int sensorCorrectRawGyroRateVector(TSS_Sensor *sensor, const float raw_gyro[3], uint8_t id, float out_corrected[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(48), (const void*[]) { raw_gyro, &id }, out_corrected);
}

int sensorCorrectRawAccelerometerVector(TSS_Sensor *sensor, const float raw_accel[3], uint8_t id, float out_corrected[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(49), (const void*[]) { raw_accel, &id }, out_corrected);
}

int sensorCorrectRawMagnetometerVector(TSS_Sensor *sensor, const float raw_mag[3], uint8_t id, float out_corrected[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(50), (const void*[]) { raw_mag, &id }, out_corrected);
}

int sensorGetNormalizedGyroRateByID(TSS_Sensor *sensor, uint8_t id, float out_gyro[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(51), (const void*[]) { &id }, out_gyro);
}

int sensorGetNormalizedAccelerometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_accel[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(52), (const void*[]) { &id }, out_accel);
}

int sensorGetNormalizedMagnetometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_mag[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(53), (const void*[]) { &id }, out_mag);
}

int sensorGetCorrectedGyroRateByID(TSS_Sensor *sensor, uint8_t id, float out_gyro[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(54), (const void*[]) { &id }, out_gyro);
}

int sensorGetCorrectedAccelerometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_accel[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(55), (const void*[]) { &id }, out_accel);
}

int sensorGetCorrectedMagnetometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_mag[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(56), (const void*[]) { &id }, out_mag);
}

int sensorMassStorageControllerEnable(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(57), NULL);
}

int sensorMassStorageControllerDisable(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(58), NULL);
}

int sensorFormatSDCard(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(59), NULL);
}

int sensorSetClockValues(TSS_Sensor *sensor, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(62), (const void*[]) { &year, &month, &day, &hour, &minute, &second });
}

int sensorGetClockValues(TSS_Sensor *sensor, uint16_t *out_year, uint8_t *out_month, uint8_t *out_day, uint8_t *out_hour, uint8_t *out_minute, uint8_t *out_second) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(63), NULL, out_year, out_month, out_day, out_hour, out_minute, out_second);
}

int sensorLoggingGetStatus(TSS_Sensor *sensor, uint8_t *out_status) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(64), NULL, out_status);
}

int sensorGetRawGyroRateByID(TSS_Sensor *sensor, uint8_t id, float out_gyro[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(65), (const void*[]) { &id }, out_gyro);
}

int sensorGetRawAccelerometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_accel[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(66), (const void*[]) { &id }, out_accel);
}

int sensorGetRawMagnetometerVectorByID(TSS_Sensor *sensor, uint8_t id, float out_mag[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(67), (const void*[]) { &id }, out_mag);
}

int sensorEEPTSStart(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(68), NULL);
}

int sensorEEPTSStop(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(69), NULL);
}

int sensorEEPTSGetOldestStep(TSS_Sensor *sensor, uint32_t *out_segment_count, uint32_t *out_timestamp, double *out_longitude, double *out_latitude, float *out_altitude, float *out_heading, float *out_distance, float *out_distance_x, float *out_distance_y, float *out_distance_z, uint8_t *out_motion, uint8_t *out_location, float *out_confidence, float *out_overall_confidence) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(70), NULL, out_segment_count, out_timestamp, out_longitude, out_latitude, out_altitude, out_heading, out_distance, out_distance_x, out_distance_y, out_distance_z, out_motion, out_location, out_confidence, out_overall_confidence);
}

int sensorEEPTSGetNewestStep(TSS_Sensor *sensor, uint32_t *out_segment_count, uint32_t *out_timestamp, double *out_longitude, double *out_latitude, float *out_altitude, float *out_heading, float *out_distance, float *out_distance_x, float *out_distance_y, float *out_distance_z, uint8_t *out_motion, uint8_t *out_location, float *out_confidence, float *out_overall_confidence) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(71), NULL, out_segment_count, out_timestamp, out_longitude, out_latitude, out_altitude, out_heading, out_distance, out_distance_x, out_distance_y, out_distance_z, out_motion, out_location, out_confidence, out_overall_confidence);
}

int sensorEEPTSGetAvailableStepCount(TSS_Sensor *sensor, uint8_t *out_count) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(72), NULL, out_count);
}

int sensorEEPTSInsertGPS(TSS_Sensor *sensor, double Latitude, double Longitude) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(73), (const void*[]) { &Latitude, &Longitude });
}

int sensorEEPTSAutoOffset(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(74), NULL);
}

int sensorStreamingGetCommandLabel(TSS_Sensor *sensor, uint8_t cmd_number, char *out_label, uint32_t size) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(83), (const void*[]) { &cmd_number }, out_label, size);
}

int sensorLoggingPauseStreaming(TSS_Sensor *sensor, uint8_t pause) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(87), (const void*[]) { &pause });
}

int sensorGetClockValuesString(TSS_Sensor *sensor, char *out_datetime, uint32_t size) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(93), NULL, out_datetime, size);
}

int sensorGetTimestamp(TSS_Sensor *sensor, uint64_t *out_timestamp) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(94), NULL, out_timestamp);
}

int sensorSetTimestamp(TSS_Sensor *sensor, uint64_t timestamp) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(95), (const void*[]) { &timestamp });
}

int sensorSetTareWithCurrentOrientation(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(96), NULL);
}

int sensorSetBaseTareWithCurrentOrientation(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(97), NULL);
}

int sensorResetFilter(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(120), NULL);
}

int sensorGetDebugMessageCount(TSS_Sensor *sensor, uint16_t *out_count) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(126), NULL, out_count);
}

int sensorGetDebugMessage(TSS_Sensor *sensor, char *out_msg, uint32_t size) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(127), NULL, out_msg, size);
}

int sensorSelfTest(TSS_Sensor *sensor, uint32_t *out_result) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(128), NULL, out_result);
}

int sensorSetTimesyncMode(TSS_Sensor *sensor, uint8_t mode) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(164), (const void*[]) { &mode });
}

int sensorBeginPassiveCalibration(TSS_Sensor *sensor, uint8_t mode) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(165), (const void*[]) { &mode });
}

int sensorGetPassiveCalibrationActive(TSS_Sensor *sensor, uint8_t *out_state) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(166), NULL, out_state);
}

int sensorBeginActiveCalibration(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(167), NULL);
}

int sensorGetActiveCalibrationActive(TSS_Sensor *sensor, uint8_t *out_state) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(168), NULL, out_state);
}

int sensorLoggingGetLastLiveLocation(TSS_Sensor *sensor, uint64_t *out_cursor_index, char *out_path, uint32_t size) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(170), NULL, out_cursor_index, out_path, size);
}

int sensorFsGetNextDirectoryItem(TSS_Sensor *sensor, uint8_t *out_type, char *out_name, uint32_t size, uint64_t *out_size) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(171), NULL, out_type, out_name, size, out_size);
}

int sensorFsChangeDirectory(TSS_Sensor *sensor, const char *path) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(172), (const void*[]) { path });
}

int sensorFsOpenFile(TSS_Sensor *sensor, const char *path) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(173), (const void*[]) { path });
}

int sensorCloseFile(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(174), NULL);
}

int sensorFileGetRemainingSize(TSS_Sensor *sensor, uint64_t *out_size) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(175), NULL, out_size);
}

int sensorFileReadLine(TSS_Sensor *sensor, char *out_line, uint32_t size) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(176), NULL, out_line, size);
}

int sensorFsDeleteFileOrFolder(TSS_Sensor *sensor, const char *path) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(178), (const void*[]) { path });
}

int sensorFileSetCursorIndex(TSS_Sensor *sensor, uint64_t index) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(179), (const void*[]) { &index });
}

int sensorBatteryGetCurrent(TSS_Sensor *sensor, int16_t *out_current) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(200), NULL, out_current);
}

int sensorBatteryGetVoltage(TSS_Sensor *sensor, float *out_voltage) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(201), NULL, out_voltage);
}

int sensorBatteryGetPercent(TSS_Sensor *sensor, uint8_t *out_percent) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(202), NULL, out_percent);
}

int sensorBatteryGetStatus(TSS_Sensor *sensor, uint8_t *out_status) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(203), NULL, out_status);
}

int sensorWMMGetField(TSS_Sensor *sensor, float *out_declination, float *out_inclination, float *out_F, float *out_H, float *out_N, float *out_E, float *out_D) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(208), NULL, out_declination, out_inclination, out_F, out_H, out_N, out_E, out_D);
}

int sensorWMMUpdatePosition(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(209), NULL);
}

int sensorWMMUpdateDate(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(210), NULL);
}

int sensorGPSGetIsActive(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(214), NULL);
}

int sensorGPSGetLatitudeandLongitude(TSS_Sensor *sensor, double *out_latitude, double *out_longitude) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(215), NULL, out_latitude, out_longitude);
}

int sensorGPSGetAltitude(TSS_Sensor *sensor, float *out_meters) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(216), NULL, out_meters);
}

int sensorGPSGetFixStatus(TSS_Sensor *sensor, uint8_t *out_fix) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(217), NULL, out_fix);
}

int sensorGPSGetHDOP(TSS_Sensor *sensor, uint8_t *out_hdop) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(218), NULL, out_hdop);
}

int sensorGPSGetSatellites(TSS_Sensor *sensor, uint8_t *out_num_satellites) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(219), NULL, out_num_satellites);
}

int sensorGPSReset(TSS_Sensor *sensor, uint8_t mode) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(220), (const void*[]) { &mode });
}

int sensorGPSSetData(TSS_Sensor *sensor, double latitude, double longitude, float altitude) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(221), (const void*[]) { &latitude, &longitude, &altitude });
}

int sensorCommitSettings(TSS_Sensor *sensor) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(225), NULL);
}

int sensorGetCurrentLEDColor(TSS_Sensor *sensor, float out_color[3]) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(238), NULL, out_color);
}

int sensorGetButtonState(TSS_Sensor *sensor, uint8_t *out_state) {
    return sensorInternalExecuteCommand(sensor, tssGetCommand(250), NULL, out_state);
}

//-----------------------------------AUTO GENERATED END--------------------------------------------
