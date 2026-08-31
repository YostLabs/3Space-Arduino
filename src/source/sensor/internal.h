#ifndef __TSS_SENSOR_INTERNAL_H__
#define __TSS_SENSOR_INTERNAL_H__

#include "tss/api/sensor.h"

typedef int (*SensorInternalReadFunction)(TSS_Sensor *sensor, const struct TSS_Command *command, va_list outputs);
typedef int (*SensorInternalReadFunctionArray)(TSS_Sensor *sensor, const struct TSS_Command *command, void **outputs);

//------------------Core Functions------------------------
int sensorInternalExecuteCommand(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, ...);
int sensorInternalExecuteCommandV(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, va_list outputs);
int sensorInternalExecuteCommandCustom(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, SensorInternalReadFunction read_func, ...);

//These needs implemented per sensor type
int sensorInternalBaseCommandRead(TSS_Sensor *sensor, const struct TSS_Command *command, va_list outputs);
int sensorInternalExecuteCommandCustomV(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, SensorInternalReadFunction read_func, va_list outputs);
int sensorInternalExecuteCommandCustomArray(TSS_Sensor *sensor, const struct TSS_Command *command, const void **input, SensorInternalReadFunctionArray read_func, void **outputs);
int sensorInternalBootloaderCheckActive(TSS_Sensor *sensor, uint8_t *active);

//-----------------------Control-------------------------
void sensorInternalForceStopStreaming(TSS_Sensor *sensor);

//Frequently used helper
static inline void sensorInternalHandleHeader(TSS_Sensor *sensor) {
    if(sensor->_header_enabled) {
        tssReadHeader(sensor->com, &sensor->header_cfg, &sensor->last_header);
    }
}

//-----------------------Custom parsing-----------------------
int sensorInternalProcessStreamingBatch(TSS_Sensor *sensor, const struct TSS_Command *command, va_list outputs);
int sensorInternalReadStreamingBatch(TSS_Sensor *sensor, const struct TSS_Command *command, va_list outputs);

int sensorInternalProcessStreamingBatchArray(TSS_Sensor *sensor, const struct TSS_Command *command, void **outputs);
int sensorInternalReadStreamingBatchArray(TSS_Sensor *sensor, const struct TSS_Command *command, void **outputs);

int sensorInternalReadStreamingBatchChecksumOnly(TSS_Sensor *sensor);
int sensorInternalUpdateDataStreaming(TSS_Sensor *sensor);

int sensorInternalUpdateFileStreaming(TSS_Sensor *sensor);
int sensorInternalUpdateLogStreaming(TSS_Sensor *sensor);

int sensorInternalUpdateDebugMessage(TSS_Sensor *sensor);

//Internal Utility
//TODO: Move Me
#include "tss/constants.h"
struct TSS_Stream_Slot {
    uint8_t cmd_num;
    uint8_t param;
    bool has_param;
};

int tssUtilStreamSlotStringToCommands(const char * str, const struct TSS_Command* out[TSS_NUM_STREAM_SLOTS+1]);

#endif /* __TSS_SENSOR_INTERNAL_H__ */