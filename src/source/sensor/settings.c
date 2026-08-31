#include "tss/api/sensor.h"
#include "tss/sys/stdinc.h"

//-----------------------------------AUTO GENERATED START---------------------------------------

/**
 * @brief Restores the sensor to its default settings.
 */
int sensorRestoreDefaultSettings(TSS_Sensor *sensor) {
    return sensorWriteSettings(sensor, (const char*[]) { "default" }, 1, NULL);
}

int sensorReadAllSettings(TSS_Sensor *sensor, TssGetSettingsCallback cb, void *user_data) {
    return sensorReadSettingsQuery(sensor, "all", cb, user_data);
}

int sensorReadAllWritableSettings(TSS_Sensor *sensor, TssGetSettingsCallback cb, void *user_data) {
    return sensorReadSettingsQuery(sensor, "settings", cb, user_data);
}

int sensorReadSerialNumber(TSS_Sensor *sensor, uint64_t *out) {
    return sensorReadSettings(sensor, "serial_number", out);
}

int sensorWriteTimestamp(TSS_Sensor *sensor, uint64_t microseconds) {
    return sensorWriteSettings(sensor, (const char*[]) { "timestamp" }, 1, (const void*[]) { &microseconds });
}

int sensorReadTimestamp(TSS_Sensor *sensor, uint64_t *out_microseconds) {
    return sensorReadSettings(sensor, "timestamp", out_microseconds);
}

int sensorWriteLedMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "led_mode" }, 1, (const void*[]) { &value });
}

int sensorReadLedMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "led_mode", out);
}

int sensorWriteLedRgb(TSS_Sensor *sensor, const float value[3]) {
    return sensorWriteSettings(sensor, (const char*[]) { "led_rgb" }, 1, (const void*[]) { value });
}

int sensorReadLedRgb(TSS_Sensor *sensor, float out[3]) {
    return sensorReadSettings(sensor, "led_rgb", out);
}

int sensorReadVersionFirmware(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "version_firmware", out, size);
}

int sensorReadVersionHardware(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "version_hardware", out, size);
}

int sensorReadUpdateRateSensor(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "update_rate_sensor", out);
}

int sensorWriteHeader(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "header" }, 1, (const void*[]) { &value });
}

int sensorReadHeader(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "header", out);
}

int sensorWriteHeaderStatusEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "header_status" }, 1, (const void*[]) { &value });
}

int sensorReadHeaderStatusEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "header_status", out);
}

int sensorWriteHeaderTimestampEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "header_timestamp" }, 1, (const void*[]) { &value });
}

int sensorReadHeaderTimestampEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "header_timestamp", out);
}

int sensorWriteHeaderEchoEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "header_echo" }, 1, (const void*[]) { &value });
}

int sensorReadHeaderEchoEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "header_echo", out);
}

int sensorWriteHeaderChecksumEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "header_checksum" }, 1, (const void*[]) { &value });
}

int sensorReadHeaderChecksumEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "header_checksum", out);
}

int sensorWriteHeaderSerialEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "header_serial" }, 1, (const void*[]) { &value });
}

int sensorReadHeaderSerialEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "header_serial", out);
}

int sensorWriteHeaderLengthEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "header_length" }, 1, (const void*[]) { &value });
}

int sensorReadHeaderLengthEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "header_length", out);
}

int sensorReadValidCommands(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "valid_commands", out, size);
}

int sensorWriteTimesyncAccelThreshold(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "timesync_accel_threshold" }, 1, (const void*[]) { &value });
}

int sensorReadTimesyncAccelThreshold(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "timesync_accel_threshold", out);
}

int sensorWriteTimesyncStartupMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "timesync_startup_mode" }, 1, (const void*[]) { &value });
}

int sensorReadTimesyncStartupMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "timesync_startup_mode", out);
}

int sensorWriteCpuSpeed(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "cpu_speed" }, 1, (const void*[]) { &value });
}

int sensorReadCpuSpeed(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "cpu_speed", out);
}

int sensorReadCpuSpeedCur(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "cpu_speed_cur", out);
}

int sensorWritePmMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pm_mode" }, 1, (const void*[]) { &value });
}

int sensorWritePmIdleEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pm_idle_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadPmIdleEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pm_idle_enabled", out);
}

int sensorWriteStreamSlots(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "stream_slots" }, 1, (const void*[]) { value });
}

int sensorReadStreamSlots(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "stream_slots", out, size);
}

int sensorWriteStreamInterval(TSS_Sensor *sensor, uint64_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "stream_interval" }, 1, (const void*[]) { &value });
}

int sensorReadStreamInterval(TSS_Sensor *sensor, uint64_t *out) {
    return sensorReadSettings(sensor, "stream_interval", out);
}

int sensorWriteStreamHz(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "stream_hz" }, 1, (const void*[]) { &value });
}

int sensorReadStreamHz(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "stream_hz", out);
}

int sensorWriteStreamDuration(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "stream_duration" }, 1, (const void*[]) { &value });
}

int sensorReadStreamDuration(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "stream_duration", out);
}

int sensorWriteStreamDelay(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "stream_delay" }, 1, (const void*[]) { &value });
}

int sensorReadStreamDelay(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "stream_delay", out);
}

int sensorWriteStreamMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "stream_mode" }, 1, (const void*[]) { &value });
}

int sensorReadStreamMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "stream_mode", out);
}

int sensorWriteStreamCount(TSS_Sensor *sensor, uint64_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "stream_count" }, 1, (const void*[]) { &value });
}

int sensorReadStreamCount(TSS_Sensor *sensor, uint64_t *out) {
    return sensorReadSettings(sensor, "stream_count", out);
}

int sensorReadStreamableCommands(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "streamable_commands", out, size);
}

int sensorWriteDebugLevel(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "debug_level" }, 1, (const void*[]) { &value });
}

int sensorReadDebugLevel(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "debug_level", out);
}

int sensorWriteDebugModule(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "debug_module" }, 1, (const void*[]) { &value });
}

int sensorReadDebugModule(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "debug_module", out);
}

int sensorWriteDebugMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "debug_mode" }, 1, (const void*[]) { &value });
}

int sensorReadDebugMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "debug_mode", out);
}

int sensorWriteDebugLed(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "debug_led" }, 1, (const void*[]) { &value });
}

int sensorReadDebugLed(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "debug_led", out);
}

int sensorWriteDebugFault(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "debug_fault" }, 1, (const void*[]) { &value });
}

int sensorReadDebugFault(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "debug_fault", out);
}

int sensorWriteDebugWdt(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "debug_wdt" }, 1, (const void*[]) { &value });
}

int sensorReadDebugWdt(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "debug_wdt", out);
}

int sensorWriteAxisOrder(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "axis_order" }, 1, (const void*[]) { value });
}

int sensorReadAxisOrder(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "axis_order", out, size);
}

int sensorWriteAxisOrderC(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "axis_order_c" }, 1, (const void*[]) { value });
}

int sensorReadAxisOrderC(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "axis_order_c", out, size);
}

int sensorWriteAxisOffsetEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "axis_offset_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadAxisOffsetEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "axis_offset_enabled", out);
}

int sensorWriteEulerOrder(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "euler_order" }, 1, (const void*[]) { value });
}

int sensorReadEulerOrder(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "euler_order", out, size);
}

int sensorReadUpdateRateFilter(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "update_rate_filter", out);
}

int sensorReadUpdateRateSms(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "update_rate_sms", out);
}

int sensorWriteOffset(TSS_Sensor *sensor, const float value[4]) {
    return sensorWriteSettings(sensor, (const char*[]) { "offset" }, 1, (const void*[]) { value });
}

int sensorReadOffset(TSS_Sensor *sensor, float out[4]) {
    return sensorReadSettings(sensor, "offset", out);
}

int sensorWriteBaseOffset(TSS_Sensor *sensor, const float value[4]) {
    return sensorWriteSettings(sensor, (const char*[]) { "base_offset" }, 1, (const void*[]) { value });
}

int sensorReadBaseOffset(TSS_Sensor *sensor, float out[4]) {
    return sensorReadSettings(sensor, "base_offset", out);
}

int sensorWriteTareQuat(TSS_Sensor *sensor, const float value[4]) {
    return sensorWriteSettings(sensor, (const char*[]) { "tare_quat" }, 1, (const void*[]) { value });
}

int sensorReadTareQuat(TSS_Sensor *sensor, float out[4]) {
    return sensorReadSettings(sensor, "tare_quat", out);
}

int sensorWriteTareAutoBase(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "tare_auto_base" }, 1, (const void*[]) { &value });
}

int sensorReadTareAutoBase(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "tare_auto_base", out);
}

int sensorWriteBaseTare(TSS_Sensor *sensor, const float value[4]) {
    return sensorWriteSettings(sensor, (const char*[]) { "base_tare" }, 1, (const void*[]) { value });
}

int sensorReadBaseTare(TSS_Sensor *sensor, float out[4]) {
    return sensorReadSettings(sensor, "base_tare", out);
}

int sensorWriteTareMat(TSS_Sensor *sensor, const float value[9]) {
    return sensorWriteSettings(sensor, (const char*[]) { "tare_mat" }, 1, (const void*[]) { value });
}

int sensorReadTareMat(TSS_Sensor *sensor, float out[9]) {
    return sensorReadSettings(sensor, "tare_mat", out);
}

int sensorWriteRunningAvgOrient(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "running_avg_orient" }, 1, (const void*[]) { &value });
}

int sensorReadRunningAvgOrient(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "running_avg_orient", out);
}

int sensorWriteFilterMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "filter_mode" }, 1, (const void*[]) { &value });
}

int sensorReadFilterMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "filter_mode", out);
}

int sensorWriteFilterMrefMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "filter_mref_mode" }, 1, (const void*[]) { &value });
}

int sensorReadFilterMrefMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "filter_mref_mode", out);
}

int sensorWriteFilterMref(TSS_Sensor *sensor, const float value[3]) {
    return sensorWriteSettings(sensor, (const char*[]) { "filter_mref" }, 1, (const void*[]) { value });
}

int sensorReadFilterMref(TSS_Sensor *sensor, float out[3]) {
    return sensorReadSettings(sensor, "filter_mref", out);
}

int sensorWriteFilterMrefGps(TSS_Sensor *sensor, const double value[2]) {
    return sensorWriteSettings(sensor, (const char*[]) { "filter_mref_gps" }, 1, (const void*[]) { value });
}

int sensorWriteFilterMrefDip(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "filter_mref_dip" }, 1, (const void*[]) { &value });
}

int sensorReadFilterMrefDip(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "filter_mref_dip", out);
}

int sensorWriteFilterConfThresholds(TSS_Sensor *sensor, float min, float max, float cap) {
    return sensorWriteSettings(sensor, (const char*[]) { "filter_conf_thresholds" }, 1, (const void*[]) { &min, &max, &cap });
}

int sensorReadFilterConfThresholds(TSS_Sensor *sensor, float *out_min, float *out_max, float *out_cap) {
    return sensorReadSettings(sensor, "filter_conf_thresholds", out_min, out_max, out_cap);
}

int sensorWriteHeadingVector(TSS_Sensor *sensor, const float value[3]) {
    return sensorWriteSettings(sensor, (const char*[]) { "heading_vector" }, 1, (const void*[]) { value });
}

int sensorReadHeadingVector(TSS_Sensor *sensor, float out[3]) {
    return sensorReadSettings(sensor, "heading_vector", out);
}

int sensorWriteHeadingVectorAxis(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "heading_vector_axis" }, 1, (const void*[]) { value });
}

int sensorWriteHeadingOffset(TSS_Sensor *sensor, float offset, uint8_t mode) {
    return sensorWriteSettings(sensor, (const char*[]) { "heading_offset" }, 1, (const void*[]) { &offset, &mode });
}

int sensorReadHeadingOffset(TSS_Sensor *sensor, float *out_offset, uint8_t *out_mode) {
    return sensorReadSettings(sensor, "heading_offset", out_offset, out_mode);
}

int sensorReadValidAccels(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "valid_accels", out, size);
}

int sensorReadValidGyros(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "valid_gyros", out, size);
}

int sensorReadValidMags(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "valid_mags", out, size);
}

int sensorReadValidBaros(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "valid_baros", out, size);
}

int sensorReadValidComponents(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "valid_components", out, size);
}

int sensorWritePrimaryAccel(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "primary_accel" }, 1, (const void*[]) { value });
}

int sensorReadPrimaryAccel(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "primary_accel", out, size);
}

int sensorWritePrimaryGyro(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "primary_gyro" }, 1, (const void*[]) { value });
}

int sensorReadPrimaryGyro(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "primary_gyro", out, size);
}

int sensorWritePrimaryMag(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "primary_mag" }, 1, (const void*[]) { value });
}

int sensorReadPrimaryMag(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "primary_mag", out, size);
}

int sensorWritePrimarySensorRfade(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "primary_sensor_rfade" }, 1, (const void*[]) { &value });
}

int sensorReadPrimarySensorRfade(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "primary_sensor_rfade", out);
}

int sensorWriteMagBiasMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "mag_bias_mode" }, 1, (const void*[]) { &value });
}

int sensorReadMagBiasMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "mag_bias_mode", out);
}

int sensorWriteOdrAll(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "odr_all" }, 1, (const void*[]) { &value });
}

int sensorWriteOdrAccelAll(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "odr_accel" }, 1, (const void*[]) { &value });
}

int sensorWriteOdrGyroAll(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "odr_gyro" }, 1, (const void*[]) { &value });
}

int sensorWriteOdrMagAll(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "odr_mag" }, 1, (const void*[]) { &value });
}

int sensorWriteOdrBaroAll(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "odr_baro" }, 1, (const void*[]) { &value });
}

int sensorWriteAccelEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "accel_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadAccelEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "accel_enabled", out);
}

int sensorWriteGyroEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "gyro_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadGyroEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "gyro_enabled", out);
}

int sensorWriteMagEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "mag_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadMagEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "mag_enabled", out);
}

int sensorWriteCalibMatAccel(TSS_Sensor *sensor, uint8_t id, const float value[9]) {
    char key[19];
    snprintf(key, sizeof(key), "calib_mat_accel%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { value });
}

int sensorReadCalibMatAccel(TSS_Sensor *sensor, uint8_t id, float out[9]) {
    char key[19];
    snprintf(key, sizeof(key), "calib_mat_accel%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteCalibBiasAccel(TSS_Sensor *sensor, uint8_t id, const float value[3]) {
    char key[20];
    snprintf(key, sizeof(key), "calib_bias_accel%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { value });
}

int sensorReadCalibBiasAccel(TSS_Sensor *sensor, uint8_t id, float out[3]) {
    char key[20];
    snprintf(key, sizeof(key), "calib_bias_accel%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteRangeAccel(TSS_Sensor *sensor, uint8_t id, uint16_t value) {
    char key[15];
    snprintf(key, sizeof(key), "range_accel%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadRangeAccel(TSS_Sensor *sensor, uint8_t id, uint16_t *out) {
    char key[15];
    snprintf(key, sizeof(key), "range_accel%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadValidRangesAccel(TSS_Sensor *sensor, uint8_t id, char *out, uint32_t size) {
    char key[22];
    snprintf(key, sizeof(key), "valid_ranges_accel%d", id);
    return sensorReadSettings(sensor, key, out, size);
}

int sensorWriteOversampleAccel(TSS_Sensor *sensor, uint8_t id, uint16_t value) {
    char key[20];
    snprintf(key, sizeof(key), "oversample_accel%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadOversampleAccel(TSS_Sensor *sensor, uint8_t id, uint16_t *out) {
    char key[20];
    snprintf(key, sizeof(key), "oversample_accel%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteRunningAvgAccel(TSS_Sensor *sensor, uint8_t id, float value) {
    char key[21];
    snprintf(key, sizeof(key), "running_avg_accel%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadRunningAvgAccel(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[21];
    snprintf(key, sizeof(key), "running_avg_accel%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteOdrAccel(TSS_Sensor *sensor, uint8_t id, uint32_t value) {
    char key[13];
    snprintf(key, sizeof(key), "odr_accel%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadOdrAccel(TSS_Sensor *sensor, uint8_t id, uint32_t *out) {
    char key[13];
    snprintf(key, sizeof(key), "odr_accel%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadUpdateRateAccel(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[21];
    snprintf(key, sizeof(key), "update_rate_accel%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadNoiseProfileAccel(TSS_Sensor *sensor, uint8_t id, float *out_noise_density, float *out_variance, float *out_bandwidth, float *out_sensitivity, uint32_t *out_odr) {
    char key[23];
    snprintf(key, sizeof(key), "noise_profile_accel%d", id);
    return sensorReadSettings(sensor, key, out_noise_density, out_variance, out_bandwidth, out_sensitivity, out_odr);
}

int sensorWriteCalibMatGyro(TSS_Sensor *sensor, uint8_t id, const float value[9]) {
    char key[18];
    snprintf(key, sizeof(key), "calib_mat_gyro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { value });
}

int sensorReadCalibMatGyro(TSS_Sensor *sensor, uint8_t id, float out[9]) {
    char key[18];
    snprintf(key, sizeof(key), "calib_mat_gyro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteCalibBiasGyro(TSS_Sensor *sensor, uint8_t id, const float value[3]) {
    char key[19];
    snprintf(key, sizeof(key), "calib_bias_gyro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { value });
}

int sensorReadCalibBiasGyro(TSS_Sensor *sensor, uint8_t id, float out[3]) {
    char key[19];
    snprintf(key, sizeof(key), "calib_bias_gyro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteRangeGyro(TSS_Sensor *sensor, uint8_t id, uint16_t value) {
    char key[14];
    snprintf(key, sizeof(key), "range_gyro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadRangeGyro(TSS_Sensor *sensor, uint8_t id, uint16_t *out) {
    char key[14];
    snprintf(key, sizeof(key), "range_gyro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadValidRangesGyro(TSS_Sensor *sensor, uint8_t id, char *out, uint32_t size) {
    char key[21];
    snprintf(key, sizeof(key), "valid_ranges_gyro%d", id);
    return sensorReadSettings(sensor, key, out, size);
}

int sensorWriteOversampleGyro(TSS_Sensor *sensor, uint8_t id, uint16_t value) {
    char key[19];
    snprintf(key, sizeof(key), "oversample_gyro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadOversampleGyro(TSS_Sensor *sensor, uint8_t id, uint16_t *out) {
    char key[19];
    snprintf(key, sizeof(key), "oversample_gyro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteRunningAvgGyro(TSS_Sensor *sensor, uint8_t id, float value) {
    char key[20];
    snprintf(key, sizeof(key), "running_avg_gyro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadRunningAvgGyro(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[20];
    snprintf(key, sizeof(key), "running_avg_gyro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteOdrGyro(TSS_Sensor *sensor, uint8_t id, uint32_t value) {
    char key[12];
    snprintf(key, sizeof(key), "odr_gyro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadOdrGyro(TSS_Sensor *sensor, uint8_t id, uint32_t *out) {
    char key[12];
    snprintf(key, sizeof(key), "odr_gyro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadUpdateRateGyro(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[20];
    snprintf(key, sizeof(key), "update_rate_gyro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadNoiseProfileGyro(TSS_Sensor *sensor, uint8_t id, float *out_noise_density, float *out_variance, float *out_bandwidth, float *out_sensitivity, uint32_t *out_odr) {
    char key[22];
    snprintf(key, sizeof(key), "noise_profile_gyro%d", id);
    return sensorReadSettings(sensor, key, out_noise_density, out_variance, out_bandwidth, out_sensitivity, out_odr);
}

int sensorWriteCalibMatMag(TSS_Sensor *sensor, uint8_t id, const float value[9]) {
    char key[17];
    snprintf(key, sizeof(key), "calib_mat_mag%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { value });
}

int sensorReadCalibMatMag(TSS_Sensor *sensor, uint8_t id, float out[9]) {
    char key[17];
    snprintf(key, sizeof(key), "calib_mat_mag%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteCalibBiasMag(TSS_Sensor *sensor, uint8_t id, const float value[3]) {
    char key[18];
    snprintf(key, sizeof(key), "calib_bias_mag%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { value });
}

int sensorReadCalibBiasMag(TSS_Sensor *sensor, uint8_t id, float out[3]) {
    char key[18];
    snprintf(key, sizeof(key), "calib_bias_mag%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteRangeMag(TSS_Sensor *sensor, uint8_t id, uint16_t value) {
    char key[13];
    snprintf(key, sizeof(key), "range_mag%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadRangeMag(TSS_Sensor *sensor, uint8_t id, uint16_t *out) {
    char key[13];
    snprintf(key, sizeof(key), "range_mag%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadValidRangesMag(TSS_Sensor *sensor, uint8_t id, char *out, uint32_t size) {
    char key[20];
    snprintf(key, sizeof(key), "valid_ranges_mag%d", id);
    return sensorReadSettings(sensor, key, out, size);
}

int sensorWriteOversampleMag(TSS_Sensor *sensor, uint8_t id, uint16_t value) {
    char key[18];
    snprintf(key, sizeof(key), "oversample_mag%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadOversampleMag(TSS_Sensor *sensor, uint8_t id, uint16_t *out) {
    char key[18];
    snprintf(key, sizeof(key), "oversample_mag%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteRunningAvgMag(TSS_Sensor *sensor, uint8_t id, float value) {
    char key[19];
    snprintf(key, sizeof(key), "running_avg_mag%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadRunningAvgMag(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[19];
    snprintf(key, sizeof(key), "running_avg_mag%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteOdrMag(TSS_Sensor *sensor, uint8_t id, uint32_t value) {
    char key[11];
    snprintf(key, sizeof(key), "odr_mag%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadOdrMag(TSS_Sensor *sensor, uint8_t id, uint32_t *out) {
    char key[11];
    snprintf(key, sizeof(key), "odr_mag%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadUpdateRateMag(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[19];
    snprintf(key, sizeof(key), "update_rate_mag%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadNoiseProfileMag(TSS_Sensor *sensor, uint8_t id, float *out_noise_density, float *out_variance, float *out_bandwidth, float *out_sensitivity, uint32_t *out_odr) {
    char key[21];
    snprintf(key, sizeof(key), "noise_profile_mag%d", id);
    return sensorReadSettings(sensor, key, out_noise_density, out_variance, out_bandwidth, out_sensitivity, out_odr);
}

int sensorWriteCalibBiasBaro(TSS_Sensor *sensor, uint8_t id, float value) {
    char key[19];
    snprintf(key, sizeof(key), "calib_bias_baro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadCalibBiasBaro(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[19];
    snprintf(key, sizeof(key), "calib_bias_baro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWriteCalibAltitudeBaro(TSS_Sensor *sensor, uint8_t id, float value) {
    char key[23];
    snprintf(key, sizeof(key), "calib_altitude_baro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorWriteOdrBaro(TSS_Sensor *sensor, uint8_t id, uint32_t value) {
    char key[12];
    snprintf(key, sizeof(key), "odr_baro%d", id);
    return sensorWriteSettings(sensor, (const char*[]) { key }, 1, (const void*[]) { &value });
}

int sensorReadOdrBaro(TSS_Sensor *sensor, uint8_t id, uint32_t *out) {
    char key[12];
    snprintf(key, sizeof(key), "odr_baro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorReadUpdateRateBaro(TSS_Sensor *sensor, uint8_t id, float *out) {
    char key[20];
    snprintf(key, sizeof(key), "update_rate_baro%d", id);
    return sensorReadSettings(sensor, key, out);
}

int sensorWritePtsOffsetQuat(TSS_Sensor *sensor, const float value[4]) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_offset_quat" }, 1, (const void*[]) { value });
}

int sensorReadPtsOffsetQuat(TSS_Sensor *sensor, float out[4]) {
    return sensorReadSettings(sensor, "pts_offset_quat", out);
}

int sensorRestorePtsDefaultSettings(TSS_Sensor *sensor) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_default" }, 1, NULL);
}

int sensorReadPtsSettings(TSS_Sensor *sensor, TssGetSettingsCallback cb, void *user_data) {
    return sensorReadSettingsQuery(sensor, "pts_settings", cb, user_data);
}

int sensorWritePtsPresetHand(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_preset_hand" }, 1, (const void*[]) { &value });
}

int sensorWritePtsPresetMotion(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_preset_motion" }, 1, (const void*[]) { &value });
}

int sensorWritePtsPresetHeading(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_preset_heading" }, 1, (const void*[]) { &value });
}

int sensorWritePtsDebugLevel(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_debug_level" }, 1, (const void*[]) { &value });
}

int sensorReadPtsDebugLevel(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_debug_level", out);
}

int sensorWritePtsDebugModule(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_debug_module" }, 1, (const void*[]) { &value });
}

int sensorReadPtsDebugModule(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_debug_module", out);
}

int sensorWritePtsHeadingMode(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_mode" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingMode(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_heading_mode", out);
}

int sensorWritePtsInitialHeadingMode(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_initial_heading_mode" }, 1, (const void*[]) { &value });
}

int sensorReadPtsInitialHeadingMode(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_initial_heading_mode", out);
}

int sensorWritePtsHandHeadingMode(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_hand_heading_mode" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHandHeadingMode(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_hand_heading_mode", out);
}

int sensorWritePtsMagDeclination(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_mag_declination" }, 1, (const void*[]) { &value });
}

int sensorReadPtsMagDeclination(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_mag_declination", out);
}

int sensorWritePtsAutoDeclination(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_auto_declination" }, 1, (const void*[]) { &value });
}

int sensorReadPtsAutoDeclination(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_auto_declination", out);
}

int sensorWritePtsDiscardSlow(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_discard_slow" }, 1, (const void*[]) { &value });
}

int sensorReadPtsDiscardSlow(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_discard_slow", out);
}

int sensorWritePtsSegmentAxis(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_segment_axis" }, 1, (const void*[]) { &value });
}

int sensorReadPtsSegmentAxis(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_segment_axis", out);
}

int sensorWritePtsSegNoise(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_seg_noise" }, 1, (const void*[]) { &value });
}

int sensorReadPtsSegNoise(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_seg_noise", out);
}

int sensorWritePtsClassifierMode(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_classifier_mode" }, 1, (const void*[]) { &value });
}

int sensorReadPtsClassifierMode(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_classifier_mode", out);
}

int sensorWritePtsClassifierMode2(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_classifier_mode2" }, 1, (const void*[]) { &value });
}

int sensorReadPtsClassifierMode2(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_classifier_mode2", out);
}

int sensorWritePtsLocationClassifierMode(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_location_classifier_mode" }, 1, (const void*[]) { &value });
}

int sensorReadPtsLocationClassifierMode(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_location_classifier_mode", out);
}

int sensorWritePtsHandClassifierThreshold(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_hand_classifier_threshold" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHandClassifierThreshold(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_hand_classifier_threshold", out);
}

int sensorWritePtsDisabledTruthMotions(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_disabled_truth_motions" }, 1, (const void*[]) { &value });
}

int sensorReadPtsDisabledTruthMotions(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_disabled_truth_motions", out);
}

int sensorWritePtsDynamicSegmenterEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_dynamic_segmenter_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadPtsDynamicSegmenterEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_dynamic_segmenter_enabled", out);
}

int sensorWritePtsEstimatorScalars(TSS_Sensor *sensor, const float value[7]) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_estimator_scalars" }, 1, (const void*[]) { value });
}

int sensorReadPtsEstimatorScalars(TSS_Sensor *sensor, float out[7]) {
    return sensorReadSettings(sensor, "pts_estimator_scalars", out);
}

int sensorWritePtsAutoEstimatorScalarRate(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_auto_estimator_scalar_rate" }, 1, (const void*[]) { &value });
}

int sensorReadPtsAutoEstimatorScalarRate(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_auto_estimator_scalar_rate", out);
}

int sensorWritePtsRunningCorrection(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_running_correction" }, 1, (const void*[]) { &value });
}

int sensorReadPtsRunningCorrection(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_running_correction", out);
}

int sensorWritePtsHandCorrection(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_hand_correction" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHandCorrection(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_hand_correction", out);
}

int sensorWritePtsHeadingCorrectionMode(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_correction_mode" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingCorrectionMode(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_heading_correction_mode", out);
}

int sensorWritePtsHeadingMinDif(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_min_dif" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingMinDif(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_heading_min_dif", out);
}

int sensorWritePtsHeadingResetConsistencies(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_reset_consistencies" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingResetConsistencies(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_heading_reset_consistencies", out);
}

int sensorWritePtsHeadingBacktrackEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_backtrack_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingBacktrackEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_heading_backtrack_enabled", out);
}

int sensorWritePtsMotionCorrectionRadius(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_motion_correction_radius" }, 1, (const void*[]) { &value });
}

int sensorReadPtsMotionCorrectionRadius(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_motion_correction_radius", out);
}

int sensorWritePtsMotionCorrectionConsistencyReq(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_motion_correction_consistency_req" }, 1, (const void*[]) { &value });
}

int sensorReadPtsMotionCorrectionConsistencyReq(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "pts_motion_correction_consistency_req", out);
}

int sensorWritePtsOrientRefYThreshold(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_orient_ref_y_threshold" }, 1, (const void*[]) { &value });
}

int sensorReadPtsOrientRefYThreshold(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_orient_ref_y_threshold", out);
}

int sensorReadPtsVersion(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "pts_version", out, size);
}

int sensorWritePtsDate(TSS_Sensor *sensor, uint32_t day, uint32_t month, uint32_t year) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_date" }, 1, (const void*[]) { &day, &month, &year });
}

int sensorReadPtsDate(TSS_Sensor *sensor, uint32_t *out_day, uint32_t *out_month, uint32_t *out_year) {
    return sensorReadSettings(sensor, "pts_date", out_day, out_month, out_year);
}

int sensorReadPtsWmmVersion(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "pts_wmm_version", out, size);
}

int sensorWritePtsWmmSet(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_wmm_set" }, 1, (const void*[]) { value });
}

int sensorWritePtsForceOutGps(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_force_out_gps" }, 1, (const void*[]) { &value });
}

int sensorReadPtsForceOutGps(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_force_out_gps", out);
}

int sensorWritePtsInitialHeadingTolerance(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_initial_heading_tolerance" }, 1, (const void*[]) { &value });
}

int sensorReadPtsInitialHeadingTolerance(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_initial_heading_tolerance", out);
}

int sensorWritePtsHeadingConsistencyReq(TSS_Sensor *sensor, int32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_consistency_req" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingConsistencyReq(TSS_Sensor *sensor, int32_t *out) {
    return sensorReadSettings(sensor, "pts_heading_consistency_req", out);
}

int sensorWritePtsHeadingRootErrMul(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_root_err_mul" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingRootErrMul(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_heading_root_err_mul", out);
}

int sensorWritePtsHeadingConsistentBias(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_heading_consistent_bias" }, 1, (const void*[]) { &value });
}

int sensorReadPtsHeadingConsistentBias(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "pts_heading_consistent_bias", out);
}

int sensorWritePtsStrictBiasEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pts_strict_bias_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadPtsStrictBiasEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pts_strict_bias_enabled", out);
}

int sensorWritePinMode0(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pin_mode0" }, 1, (const void*[]) { &value });
}

int sensorReadPinMode0(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pin_mode0", out);
}

int sensorWritePinMode1(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "pin_mode1" }, 1, (const void*[]) { &value });
}

int sensorReadPinMode1(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "pin_mode1", out);
}

int sensorWriteUartBaudrate(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "uart_baudrate" }, 1, (const void*[]) { &value });
}

int sensorReadUartBaudrate(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "uart_baudrate", out);
}

int sensorWriteI2cAddr(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "i2c_addr" }, 1, (const void*[]) { &value });
}

int sensorReadI2cAddr(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "i2c_addr", out);
}

int sensorWritePowerHoldTime(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "power_hold_time" }, 1, (const void*[]) { &value });
}

int sensorReadPowerHoldTime(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "power_hold_time", out);
}

int sensorWritePowerHoldState(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "power_hold_state" }, 1, (const void*[]) { &value });
}

int sensorReadPowerHoldState(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "power_hold_state", out);
}

int sensorWritePowerInitialHoldState(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "power_initial_hold_state" }, 1, (const void*[]) { &value });
}

int sensorReadPowerInitialHoldState(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "power_initial_hold_state", out);
}

int sensorFsCfgLoad(TSS_Sensor *sensor) {
    return sensorWriteSettings(sensor, (const char*[]) { "fs_cfg_load" }, 1, NULL);
}

int sensorWriteFsMscEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "fs_msc_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadFsMscEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "fs_msc_enabled", out);
}

int sensorWriteFsMscAuto(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "fs_msc_auto" }, 1, (const void*[]) { &value });
}

int sensorReadFsMscAuto(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "fs_msc_auto", out);
}

int sensorWriteLogSlots(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_slots" }, 1, (const void*[]) { value });
}

int sensorReadLogSlots(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "log_slots", out, size);
}

int sensorWriteLogInterval(TSS_Sensor *sensor, uint64_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_interval" }, 1, (const void*[]) { &value });
}

int sensorReadLogInterval(TSS_Sensor *sensor, uint64_t *out) {
    return sensorReadSettings(sensor, "log_interval", out);
}

int sensorWriteLogHz(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_hz" }, 1, (const void*[]) { &value });
}

int sensorReadLogHz(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "log_hz", out);
}

int sensorWriteLogStartEvent(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_start_event" }, 1, (const void*[]) { value });
}

int sensorReadLogStartEvent(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "log_start_event", out, size);
}

int sensorWriteLogStartMotionThreshold(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_start_motion_threshold" }, 1, (const void*[]) { &value });
}

int sensorReadLogStartMotionThreshold(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "log_start_motion_threshold", out);
}

int sensorWriteLogStopEvent(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_stop_event" }, 1, (const void*[]) { value });
}

int sensorReadLogStopEvent(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "log_stop_event", out, size);
}

int sensorWriteLogStopMotionThreshold(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_stop_motion_threshold" }, 1, (const void*[]) { &value });
}

int sensorReadLogStopMotionThreshold(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "log_stop_motion_threshold", out);
}

int sensorWriteLogStopMotionDelay(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_stop_motion_delay" }, 1, (const void*[]) { &value });
}

int sensorReadLogStopMotionDelay(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "log_stop_motion_delay", out);
}

int sensorWriteLogStopCount(TSS_Sensor *sensor, uint64_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_stop_count" }, 1, (const void*[]) { &value });
}

int sensorReadLogStopCount(TSS_Sensor *sensor, uint64_t *out) {
    return sensorReadSettings(sensor, "log_stop_count", out);
}

int sensorWriteLogStopDuration(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_stop_duration" }, 1, (const void*[]) { &value });
}

int sensorReadLogStopDuration(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "log_stop_duration", out);
}

int sensorWriteLogStopPeriodCount(TSS_Sensor *sensor, uint32_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_stop_period_count" }, 1, (const void*[]) { &value });
}

int sensorReadLogStopPeriodCount(TSS_Sensor *sensor, uint32_t *out) {
    return sensorReadSettings(sensor, "log_stop_period_count", out);
}

int sensorWriteLogStyle(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_style" }, 1, (const void*[]) { &value });
}

int sensorReadLogStyle(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_style", out);
}

int sensorWriteLogPeriodicCaptureTime(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_periodic_capture_time" }, 1, (const void*[]) { &value });
}

int sensorReadLogPeriodicCaptureTime(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "log_periodic_capture_time", out);
}

int sensorWriteLogPeriodicRestTime(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_periodic_rest_time" }, 1, (const void*[]) { &value });
}

int sensorReadLogPeriodicRestTime(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "log_periodic_rest_time", out);
}

int sensorWriteLogBaseFilename(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_base_filename" }, 1, (const void*[]) { value });
}

int sensorReadLogBaseFilename(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "log_base_filename", out, size);
}

int sensorWriteLogFileMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_file_mode" }, 1, (const void*[]) { &value });
}

int sensorReadLogFileMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_file_mode", out);
}

int sensorWriteLogDataMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_data_mode" }, 1, (const void*[]) { &value });
}

int sensorReadLogDataMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_data_mode", out);
}

int sensorWriteLogOutputSettings(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_output_settings" }, 1, (const void*[]) { &value });
}

int sensorReadLogOutputSettings(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_output_settings", out);
}

int sensorWriteLogHeaderEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_header_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadLogHeaderEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_header_enabled", out);
}

int sensorWriteLogFolderMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_folder_mode" }, 1, (const void*[]) { &value });
}

int sensorReadLogFolderMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_folder_mode", out);
}

int sensorWriteLogImmediateOutput(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_immediate_output" }, 1, (const void*[]) { &value });
}

int sensorReadLogImmediateOutput(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_immediate_output", out);
}

int sensorWriteLogImmediateOutputHeaderEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_immediate_output_header_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadLogImmediateOutputHeaderEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_immediate_output_header_enabled", out);
}

int sensorWriteLogImmediateOutputHeaderMode(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "log_immediate_output_header_mode" }, 1, (const void*[]) { &value });
}

int sensorReadLogImmediateOutputHeaderMode(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "log_immediate_output_header_mode", out);
}

int sensorWriteRtcYear(TSS_Sensor *sensor, uint16_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_year" }, 1, (const void*[]) { &value });
}

int sensorReadRtcYear(TSS_Sensor *sensor, uint16_t *out) {
    return sensorReadSettings(sensor, "rtc_year", out);
}

int sensorWriteRtcMonth(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_month" }, 1, (const void*[]) { &value });
}

int sensorReadRtcMonth(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "rtc_month", out);
}

int sensorWriteRtcDay(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_day" }, 1, (const void*[]) { &value });
}

int sensorReadRtcDay(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "rtc_day", out);
}

int sensorWriteRtcHour(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_hour" }, 1, (const void*[]) { &value });
}

int sensorReadRtcHour(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "rtc_hour", out);
}

int sensorWriteRtcMinute(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_minute" }, 1, (const void*[]) { &value });
}

int sensorReadRtcMinute(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "rtc_minute", out);
}

int sensorWriteRtcSecond(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_second" }, 1, (const void*[]) { &value });
}

int sensorReadRtcSecond(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "rtc_second", out);
}

int sensorWriteRtcDatetime(TSS_Sensor *sensor, uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_datetime" }, 1, (const void*[]) { &year, &month, &day, &hour, &minute, &second });
}

int sensorReadRtcDatetime(TSS_Sensor *sensor, uint16_t *out_year, uint8_t *out_month, uint8_t *out_day, uint8_t *out_hour, uint8_t *out_minute, uint8_t *out_second) {
    return sensorReadSettings(sensor, "rtc_datetime", out_year, out_month, out_day, out_hour, out_minute, out_second);
}

int sensorWriteRtcSource(TSS_Sensor *sensor, uint8_t mode) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_source" }, 1, (const void*[]) { &mode });
}

int sensorReadRtcSource(TSS_Sensor *sensor, uint8_t *out_mode) {
    return sensorReadSettings(sensor, "rtc_source", out_mode);
}

int sensorWriteUtcOffset(TSS_Sensor *sensor, float offset) {
    return sensorWriteSettings(sensor, (const char*[]) { "utc_offset" }, 1, (const void*[]) { &offset });
}

int sensorReadUtcOffset(TSS_Sensor *sensor, float *out_offset) {
    return sensorReadSettings(sensor, "utc_offset", out_offset);
}

int sensorWriteBatChgRate(TSS_Sensor *sensor, uint16_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "bat_chg_rate" }, 1, (const void*[]) { &value });
}

int sensorReadBatChgRate(TSS_Sensor *sensor, uint16_t *out) {
    return sensorReadSettings(sensor, "bat_chg_rate", out);
}

int sensorWriteBatColdThreshold(TSS_Sensor *sensor, float temperature_c, float chg_rate) {
    return sensorWriteSettings(sensor, (const char*[]) { "bat_cold_threshold" }, 1, (const void*[]) { &temperature_c, &chg_rate });
}

int sensorReadBatColdThreshold(TSS_Sensor *sensor, float *out_temperature_c, float *out_chg_rate) {
    return sensorReadSettings(sensor, "bat_cold_threshold", out_temperature_c, out_chg_rate);
}

int sensorWriteBatWarmThreshold(TSS_Sensor *sensor, float temperature_c, float chg_rate) {
    return sensorWriteSettings(sensor, (const char*[]) { "bat_warm_threshold" }, 1, (const void*[]) { &temperature_c, &chg_rate });
}

int sensorReadBatWarmThreshold(TSS_Sensor *sensor, float *out_temperature_c, float *out_chg_rate) {
    return sensorReadSettings(sensor, "bat_warm_threshold", out_temperature_c, out_chg_rate);
}

int sensorWriteBatHotThreshold(TSS_Sensor *sensor, float temperature_c, float chg_rate) {
    return sensorWriteSettings(sensor, (const char*[]) { "bat_hot_threshold" }, 1, (const void*[]) { &temperature_c, &chg_rate });
}

int sensorReadBatHotThreshold(TSS_Sensor *sensor, float *out_temperature_c, float *out_chg_rate) {
    return sensorReadSettings(sensor, "bat_hot_threshold", out_temperature_c, out_chg_rate);
}

int sensorWriteBatOffsetThreshold(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "bat_offset_threshold" }, 1, (const void*[]) { &value });
}

int sensorReadBatOffsetThreshold(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "bat_offset_threshold", out);
}

int sensorReadBatMah(TSS_Sensor *sensor, uint16_t *out) {
    return sensorReadSettings(sensor, "bat_mah", out);
}

int sensorWriteBleName(TSS_Sensor *sensor, const char *value) {
    return sensorWriteSettings(sensor, (const char*[]) { "ble_name" }, 1, (const void*[]) { value });
}

int sensorReadBleName(TSS_Sensor *sensor, char *out, uint32_t size) {
    return sensorReadSettings(sensor, "ble_name", out, size);
}

int sensorReadBleConnected(TSS_Sensor *sensor, int8_t *out) {
    return sensorReadSettings(sensor, "ble_connected", out);
}

int sensorBleDisconnect(TSS_Sensor *sensor) {
    return sensorWriteSettings(sensor, (const char*[]) { "ble_disconnect" }, 1, NULL);
}

int sensorWriteGpsStandby(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "gps_standby" }, 1, (const void*[]) { &value });
}

int sensorReadGpsStandby(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "gps_standby", out);
}

int sensorWriteGpsLed(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "gps_led" }, 1, (const void*[]) { &value });
}

int sensorReadGpsLed(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "gps_led", out);
}

int sensorWriteGpsPeriodicFixTimes(TSS_Sensor *sensor, const uint16_t value[2]) {
    return sensorWriteSettings(sensor, (const char*[]) { "gps_periodic_fix_times" }, 1, (const void*[]) { value });
}

int sensorReadGpsPeriodicFixTimes(TSS_Sensor *sensor, uint16_t out[2]) {
    return sensorReadSettings(sensor, "gps_periodic_fix_times", out);
}

int sensorWriteGpsPeriodicUnfixedTimes(TSS_Sensor *sensor, const uint16_t value[2]) {
    return sensorWriteSettings(sensor, (const char*[]) { "gps_periodic_unfixed_times" }, 1, (const void*[]) { value });
}

int sensorReadGpsPeriodicUnfixedTimes(TSS_Sensor *sensor, uint16_t out[2]) {
    return sensorReadSettings(sensor, "gps_periodic_unfixed_times", out);
}

int sensorWriteGpsPeriodicEnabled(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "gps_periodic_enabled" }, 1, (const void*[]) { &value });
}

int sensorReadGpsPeriodicEnabled(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "gps_periodic_enabled", out);
}

int sensorWriteRtcGpsSyncInterval(TSS_Sensor *sensor, uint64_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_gps_sync_interval" }, 1, (const void*[]) { &value });
}

int sensorReadRtcGpsSyncInterval(TSS_Sensor *sensor, uint64_t *out) {
    return sensorReadSettings(sensor, "rtc_gps_sync_interval", out);
}

int sensorWriteRtcGpsForceSync(TSS_Sensor *sensor, uint8_t value) {
    return sensorWriteSettings(sensor, (const char*[]) { "rtc_gps_force_sync" }, 1, (const void*[]) { &value });
}

int sensorReadRtcGpsForceSync(TSS_Sensor *sensor, uint8_t *out) {
    return sensorReadSettings(sensor, "rtc_gps_force_sync", out);
}

int sensorWriteUsbTimeout(TSS_Sensor *sensor, float value) {
    return sensorWriteSettings(sensor, (const char*[]) { "usb_timeout" }, 1, (const void*[]) { &value });
}

int sensorReadUsbTimeout(TSS_Sensor *sensor, float *out) {
    return sensorReadSettings(sensor, "usb_timeout", out);
}

//-----------------------------------AUTO GENERATED END---------------------------------------
