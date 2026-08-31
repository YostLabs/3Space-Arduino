/**
 * @ Author: Andy Riedlinger
 * @ Create Time: 2025-6-20 11:45:00
 *
 * @ Description:
 * A wrapper class to add the peek functionality and
 * buffered writing to a minimal com class.
 * 
 * Also contains default implementations for some of the other
 * functions, such as read_until and clear that can be used
 * by minimal com classes.
 * 
 * @note:
 * If both TSS_MINIMAL_SENSOR=1 and TSS_BUFFERED_WRITES=0,
 * this class only wraps calls and provides no additional functionality
 */

#ifndef __TSS_RING_COM_H__
#define __TSS_RING_COM_H__

#include "tss/export.h"

#include "tss/com/com_class.h"
#include "tss/utility/ring_buf2.h"

#ifdef __cplusplus
extern "C" {
#endif

struct TSS_Managed_Com_Class {
    struct TSS_Com_Class base;

    //Com class that is being wrapped, and pointer to the "self" that is fed into that com classes functions.
    //Child data is separated out because a traditional structure will have managed_com_class at the top of the struct,
    //and so can't cast the child com class to the parent type.
    const struct TSS_Com_Class *child;
    struct TSS_Com_Class *child_container;

    //For peeking
    struct TSS_Ring_Buf2 read_ring;

    //For building writes before sending
    uint8_t *write_buffer;
    size_t write_buffer_size;
    uint16_t write_buffer_index;
};

//TODO: Document me
TSS_API int tssCreateManagedComDynamic(struct TSS_Com_Class *child, uint8_t *read_buf, size_t read_size, uint8_t *write_buf, size_t write_size, struct TSS_Managed_Com_Class *out);
TSS_API int tssCreateManagedCom(struct TSS_Com_Class *child, struct TSS_Com_Class *child_container, uint8_t *read_buf, size_t read_size, uint8_t *write_buf, size_t write_size, struct TSS_Managed_Com_Class *out);

//Base functions that can be used for reading/clearing on any com class where user_data is a struct TSS_Com_Class and
//the read function and get/set timeout functions are implemented.
TSS_API int tssManagedComBaseReadUntil(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size);
TSS_API void tssManagedComBaseClear(struct TSS_Com_Class *com);
TSS_API void tssManagedComBaseClearTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif /* __TSS_RING_COM_H__ */