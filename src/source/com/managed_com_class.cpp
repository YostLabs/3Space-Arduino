#include "tss/cpp/com/managed_com_class.hpp"
#include "tss/errors.h"

namespace tss {

ManagedComClass::ManagedComClass(bool supports_reenumeration, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size) :
    ComClass(supports_reenumeration)
{
    m_hw.base.api = &s_impl_api;
    m_hw.base.reenumerates = supports_reenumeration;
    m_hw.context = this;

    int result = tssCreateManagedComDynamic(&m_hw.base, read_buf, read_buf_size, write_buf, write_buf_size, &m_managed);
    if(result != TSS_SUCCESS) {
        // tssCreateManagedComDynamic() failed (currently: read_buf_size
        // wasn't a power of 2) without writing anything to m_managed, so it
        // otherwise holds indeterminate values. Routing m_managed.base.api
        // to s_failed_api instead means every ManagedComClass method below
        // can keep unconditionally dispatching through m_managed.base -
        // exactly like the success path - and just ends up safely reporting
        // failure/no-op instead of reading that indeterminate state.
        m_managed.base.api = &s_failed_api;
    }
}

int ManagedComClass::reenumerateImpl(struct TSS_Com_Class * /*com*/, TssComAutoDetectCallback /*cb*/, void * /*detect_data*/)
{
    return TSS_ERR_UNIMPLEMENTED_DETECTION;
}

int ManagedComClass::autoDetectImpl(struct TSS_Com_Class * /*com*/, TssComAutoDetectCallback /*cb*/, void * /*detect_data*/)
{
    return TSS_ERR_UNIMPLEMENTED_DETECTION;
}

//-----------------------------------------------------------------------
// tss::ComClass overrides - thin, one-line delegations to the embedded
// TSS_Managed_Com_Class, reusing the existing, tested managed_com.c logic
// for ring buffering/peeking/write buffering wholesale.
//-----------------------------------------------------------------------

int ManagedComClass::open()
{
    return tss_com_open(&m_managed.base);
}

int ManagedComClass::close()
{
    return tss_com_close(&m_managed.base);
}

int ManagedComClass::read(size_t num_bytes, uint8_t *out)
{
    return tss_com_read(&m_managed.base, num_bytes, out);
}

int ManagedComClass::write(const uint8_t *bytes, size_t len)
{
    return tss_com_write(&m_managed.base, bytes, len);
}

void ManagedComClass::setTimeout(uint32_t timeout_ms)
{
    tss_com_set_timeout(&m_managed.base, timeout_ms);
}

uint32_t ManagedComClass::getTimeout()
{
    return tss_com_get_timeout(&m_managed.base);
}

int ManagedComClass::readUntil(uint8_t value, uint8_t *out, size_t size)
{
    return tss_com_read_until(&m_managed.base, value, out, size);
}

void ManagedComClass::clearImmediate()
{
    tss_com_clear_immediate(&m_managed.base);
}

void ManagedComClass::clearTimeout(uint32_t timeout_ms)
{
    tss_com_clear_timeout(&m_managed.base, timeout_ms);
}

#if !(TSS_MINIMAL_SENSOR)
int ManagedComClass::peek(size_t start, size_t num_bytes, uint8_t *out)
{
    return tss_com_peek(&m_managed.base, start, num_bytes, out);
}

int ManagedComClass::peekUntil(size_t start, uint8_t value, uint8_t *out, size_t size)
{
    return tss_com_peek_until(&m_managed.base, start, value, out, size);
}

size_t ManagedComClass::peekCapacity()
{
    return tss_com_peek_capacity(&m_managed.base);
}

size_t ManagedComClass::length()
{
    return tss_com_length(&m_managed.base);
}
#endif

#if TSS_BUFFERED_WRITES
int ManagedComClass::beginWrite()
{
    return TSS_COM_BEGIN_WRITE(&m_managed.base);
}

int ManagedComClass::endWrite()
{
    return TSS_COM_END_WRITE(&m_managed.base);
}
#endif

int ManagedComClass::reenumerate(TssComAutoDetectCallback cb, void *detect_data)
{
    return tss_com_reenumerate(&m_managed.base, cb, detect_data);
}

int ManagedComClass::autoDetect(TssComAutoDetectCallback cb, void *detect_data)
{
    // auto_detect takes the destination com object directly (rather than an
    // existing instance) - see TSS_Com_Class_API::auto_detect - so this is
    // called directly against m_managed's own vtable/object instead of
    // through a tss_com_* convenience wrapper.
    if(m_managed.base.api->auto_detect == nullptr) {
        return TSS_ERR_UNIMPLEMENTED_DETECTION;
    }
    return m_managed.base.api->auto_detect(&m_managed.base, cb, detect_data);
}

//-----------------------------------------------------------------------
// Trampolines - forward the C API callbacks used by m_hw (the "child" that
// m_managed wraps) into this instance's *Impl methods.
//-----------------------------------------------------------------------

ManagedComClass *ManagedComClass::implOwner(struct TSS_Com_Class *com)
{
    return static_cast<ManagedComClass *>(reinterpret_cast<struct TSS_Context_Com_Class *>(com)->context);
}

int ManagedComClass::trampolineOpenImpl(struct TSS_Com_Class *com)
{
    return implOwner(com)->openImpl();
}

int ManagedComClass::trampolineCloseImpl(struct TSS_Com_Class *com)
{
    return implOwner(com)->closeImpl();
}

int ManagedComClass::trampolineReadImpl(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out)
{
    return implOwner(com)->readImpl(num_bytes, out);
}

int ManagedComClass::trampolineWriteImpl(struct TSS_Com_Class *com, const uint8_t *bytes, size_t len)
{
    return implOwner(com)->writeImpl(bytes, len);
}

void ManagedComClass::trampolineSetTimeoutImpl(struct TSS_Com_Class *com, uint32_t timeout_ms)
{
    implOwner(com)->setTimeoutImpl(timeout_ms);
}

uint32_t ManagedComClass::trampolineGetTimeoutImpl(struct TSS_Com_Class *com)
{
    return implOwner(com)->getTimeoutImpl();
}

int ManagedComClass::trampolineReenumerateImpl(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data)
{
    return implOwner(com)->reenumerateImpl(com, cb, detect_data);
}

int ManagedComClass::trampolineAutoDetectImpl(struct TSS_Com_Class *out, TssComAutoDetectCallback cb, void *detect_data)
{
    return implOwner(out)->autoDetectImpl(out, cb, detect_data);
}

//-----------------------------------------------------------------------
// m_hw's vtable. read_until/clear_immediate/clear_timeout reuse the
// exported tssManagedComBase* helpers as their implementation - the same
// optional defaults tss::ComClass itself uses - operating on m_hw (a
// genuine, valid TSS_Com_Class), which is safe since owner()/implOwner()
// recover the instance via `.context` rather than assuming any particular
// pointer arithmetic on `com` itself.
//-----------------------------------------------------------------------

const struct TSS_Com_Class_API ManagedComClass::s_impl_api = {
    // in (struct TSS_Input_Stream)
    {
        trampolineReadImpl,
        tssManagedComBaseReadUntil,
#if !(TSS_MINIMAL_SENSOR)
        nullptr, // peek       - not needed; m_hw is only ever used as the wrapped child.
        nullptr, // peek_until
        nullptr, // peek_capacity
        nullptr, // length
#endif
        trampolineSetTimeoutImpl,
        trampolineGetTimeoutImpl,
        tssManagedComBaseClear,
        tssManagedComBaseClearTimeout,
    },
    // out (struct TSS_Output_Stream)
    {
        trampolineWriteImpl,
#if TSS_BUFFERED_WRITES
        nullptr, // begin_write - not needed; m_managed provides its own write buffering.
        nullptr, // end_write
#endif
    },
    trampolineOpenImpl,
    trampolineCloseImpl,
    trampolineReenumerateImpl,
    trampolineAutoDetectImpl,
};

//-----------------------------------------------------------------------
// s_failed_api - used as m_managed.base.api instead of s_impl_api's normal
// counterpart when tssCreateManagedComDynamic() fails in the constructor
// (see above). Every entry ignores its arguments and just returns/no-ops
// immediately - no state to touch, so nothing here can ever fail further.
//-----------------------------------------------------------------------

int ManagedComClass::failedOpenOrClose(struct TSS_Com_Class * /*com*/)
{
    return TSS_ERR_INVALID_SIZE;
}

int ManagedComClass::failedRead(struct TSS_Com_Class * /*com*/, size_t /*num_bytes*/, uint8_t * /*out*/)
{
    return TSS_ERR_INVALID_SIZE;
}

int ManagedComClass::failedReadUntil(struct TSS_Com_Class * /*com*/, uint8_t /*value*/, uint8_t * /*out*/, size_t /*size*/)
{
    return TSS_ERR_INVALID_SIZE;
}

int ManagedComClass::failedWrite(struct TSS_Com_Class * /*com*/, const uint8_t * /*bytes*/, size_t /*len*/)
{
    return TSS_ERR_INVALID_SIZE;
}

void ManagedComClass::failedSetTimeout(struct TSS_Com_Class * /*com*/, uint32_t /*timeout_ms*/)
{
}

uint32_t ManagedComClass::failedGetTimeout(struct TSS_Com_Class * /*com*/)
{
    return 0;
}

void ManagedComClass::failedClear(struct TSS_Com_Class * /*com*/)
{
}

void ManagedComClass::failedClearTimeout(struct TSS_Com_Class * /*com*/, uint32_t /*timeout_ms*/)
{
}

int ManagedComClass::failedReenumerateOrAutoDetect(struct TSS_Com_Class * /*com*/, TssComAutoDetectCallback /*cb*/, void * /*detect_data*/)
{
    return TSS_ERR_INVALID_SIZE;
}

#if !(TSS_MINIMAL_SENSOR)
int ManagedComClass::failedPeek(struct TSS_Com_Class * /*com*/, size_t /*start*/, size_t /*num_bytes*/, uint8_t * /*out*/)
{
    return TSS_ERR_INVALID_SIZE;
}

int ManagedComClass::failedPeekUntil(struct TSS_Com_Class * /*com*/, size_t /*start*/, uint8_t /*value*/, uint8_t * /*out*/, size_t /*size*/)
{
    return TSS_ERR_INVALID_SIZE;
}

size_t ManagedComClass::failedPeekCapacityOrLength(struct TSS_Com_Class * /*com*/)
{
    return 0;
}
#endif

#if TSS_BUFFERED_WRITES
int ManagedComClass::failedBeginOrEndWrite(struct TSS_Com_Class * /*com*/)
{
    return TSS_ERR_INVALID_SIZE;
}
#endif

const struct TSS_Com_Class_API ManagedComClass::s_failed_api = {
    // in (struct TSS_Input_Stream)
    {
        failedRead,
        failedReadUntil,
#if !(TSS_MINIMAL_SENSOR)
        failedPeek,
        failedPeekUntil,
        failedPeekCapacityOrLength,
        failedPeekCapacityOrLength, // length
#endif
        failedSetTimeout,
        failedGetTimeout,
        failedClear,
        failedClearTimeout,
    },
    // out (struct TSS_Output_Stream)
    {
        failedWrite,
#if TSS_BUFFERED_WRITES
        failedBeginOrEndWrite, // begin_write
        failedBeginOrEndWrite, // end_write
#endif
    },
    failedOpenOrClose, // open
    failedOpenOrClose, // close
    failedReenumerateOrAutoDetect, // reenumerate
    failedReenumerateOrAutoDetect, // auto_detect
};

} // namespace tss
