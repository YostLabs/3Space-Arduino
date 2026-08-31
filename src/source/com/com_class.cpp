#include "tss/cpp/com/com_class.hpp"
#include "tss/errors.h"

namespace tss {

ComClass::ComClass(bool supports_reenumeration)
{
    m_com.base.api = &s_api;
    m_com.base.reenumerates = supports_reenumeration;
    m_com.context = this;
}

ComClass::operator struct TSS_Com_Class *()
{
    return &m_com.base;
}

//-----------------------------------------------------------------------
// Optional overrides with generic defaults.
//-----------------------------------------------------------------------

int ComClass::readUntil(uint8_t value, uint8_t *out, size_t size)
{
    return tssManagedComBaseReadUntil(&m_com.base, value, out, size);
}

void ComClass::clearImmediate()
{
    tssManagedComBaseClear(&m_com.base);
}

void ComClass::clearTimeout(uint32_t timeout_ms)
{
    tssManagedComBaseClearTimeout(&m_com.base, timeout_ms);
}

#if !(TSS_MINIMAL_SENSOR)
int ComClass::peek(size_t /*start*/, size_t /*num_bytes*/, uint8_t * /*out*/)
{
    // No buffer to peek into without tss::ManagedComClass.
    return TSS_ERR_INSUFFICIENT_BUFFER;
}

int ComClass::peekUntil(size_t /*start*/, uint8_t /*value*/, uint8_t * /*out*/, size_t /*size*/)
{
    return TSS_ERR_INSUFFICIENT_BUFFER;
}

size_t ComClass::peekCapacity()
{
    return 0;
}

size_t ComClass::length()
{
    return 0;
}
#endif

#if TSS_BUFFERED_WRITES
int ComClass::beginWrite()
{
    return TSS_SUCCESS;
}

int ComClass::endWrite()
{
    return TSS_SUCCESS;
}
#endif

int ComClass::reenumerate(TssComAutoDetectCallback /*cb*/, void * /*detect_data*/)
{
    return TSS_ERR_UNIMPLEMENTED_DETECTION;
}

int ComClass::autoDetect(TssComAutoDetectCallback /*cb*/, void * /*detect_data*/)
{
    return TSS_ERR_UNIMPLEMENTED_DETECTION;
}

//-----------------------------------------------------------------------
// Trampolines - forward the C API callbacks into this instance's virtual
// methods. `com` always points at this instance's m_com (a
// TSS_Context_Com_Class), whose `.context` is exactly the ComClass* that
// was stored in the constructor above, so it round-trips safely and
// virtual dispatch takes care of calling into the most-derived override.
//-----------------------------------------------------------------------

ComClass *ComClass::owner(struct TSS_Com_Class *com)
{
    return static_cast<ComClass *>(reinterpret_cast<struct TSS_Context_Com_Class *>(com)->context);
}

int ComClass::trampolineOpen(struct TSS_Com_Class *com)
{
    return owner(com)->open();
}

int ComClass::trampolineClose(struct TSS_Com_Class *com)
{
    return owner(com)->close();
}

int ComClass::trampolineRead(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out)
{
    return owner(com)->read(num_bytes, out);
}

int ComClass::trampolineWrite(struct TSS_Com_Class *com, const uint8_t *bytes, size_t len)
{
    return owner(com)->write(bytes, len);
}

int ComClass::trampolineReadUntil(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size)
{
    return owner(com)->readUntil(value, out, size);
}

void ComClass::trampolineSetTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms)
{
    owner(com)->setTimeout(timeout_ms);
}

uint32_t ComClass::trampolineGetTimeout(struct TSS_Com_Class *com)
{
    return owner(com)->getTimeout();
}

void ComClass::trampolineClearImmediate(struct TSS_Com_Class *com)
{
    owner(com)->clearImmediate();
}

void ComClass::trampolineClearTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms)
{
    owner(com)->clearTimeout(timeout_ms);
}

#if !(TSS_MINIMAL_SENSOR)
int ComClass::trampolinePeek(struct TSS_Com_Class *com, size_t start, size_t num_bytes, uint8_t *out)
{
    return owner(com)->peek(start, num_bytes, out);
}

int ComClass::trampolinePeekUntil(struct TSS_Com_Class *com, size_t start, uint8_t value, uint8_t *out, size_t size)
{
    return owner(com)->peekUntil(start, value, out, size);
}

size_t ComClass::trampolinePeekCapacity(struct TSS_Com_Class *com)
{
    return owner(com)->peekCapacity();
}

size_t ComClass::trampolineLength(struct TSS_Com_Class *com)
{
    return owner(com)->length();
}
#endif

#if TSS_BUFFERED_WRITES
int ComClass::trampolineBeginWrite(struct TSS_Com_Class *com)
{
    return owner(com)->beginWrite();
}

int ComClass::trampolineEndWrite(struct TSS_Com_Class *com)
{
    return owner(com)->endWrite();
}
#endif

int ComClass::trampolineReenumerate(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data)
{
    return owner(com)->reenumerate(cb, detect_data);
}

int ComClass::trampolineAutoDetect(struct TSS_Com_Class *out, TssComAutoDetectCallback cb, void *detect_data)
{
    return owner(out)->autoDetect(cb, detect_data);
}

//-----------------------------------------------------------------------
// The shared TSS_Com_Class_API vtable - every slot maps 1:1 to the
// identically-purposed virtual method above, so every one of them can be
// overridden directly by a derived class for full manual control.
//-----------------------------------------------------------------------

const struct TSS_Com_Class_API ComClass::s_api = {
    // in (struct TSS_Input_Stream)
    {
        trampolineRead,
        trampolineReadUntil,
#if !(TSS_MINIMAL_SENSOR)
        trampolinePeek,
        trampolinePeekUntil,
        trampolinePeekCapacity,
        trampolineLength,
#endif
        trampolineSetTimeout,
        trampolineGetTimeout,
        trampolineClearImmediate,
        trampolineClearTimeout,
    },
    // out (struct TSS_Output_Stream)
    {
        trampolineWrite,
#if TSS_BUFFERED_WRITES
        trampolineBeginWrite,
        trampolineEndWrite,
#endif
    },
    trampolineOpen,
    trampolineClose,
    trampolineReenumerate,
    trampolineAutoDetect,
};

} // namespace tss
