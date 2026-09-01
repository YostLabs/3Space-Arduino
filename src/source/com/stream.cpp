#if defined(ARDUINO)

#include "tss/com/stream.h"

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TssStreamComClassBase::TssStreamComClassBase(Stream &stream, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size) :
    tss::ManagedComClass(/*supports_reenumeration=*/false, read_buf, read_buf_size, write_buf, write_buf_size),
    m_stream(stream)
{
}

TssStreamComClass::TssStreamComClass(Stream &stream) :
    TSS_MANAGED_COM_INIT(TssStreamComClassBase, stream)
{
}

// -----------------------------------------------------------------------
// Open / Close
//
// The underlying Stream is expected to already be open/configured by the
// caller - see the class doc comment in stream.h.
// -----------------------------------------------------------------------

int TssStreamComClassBase::openImpl()
{
    return 0;
}

int TssStreamComClassBase::closeImpl()
{
    return 0;
}

// -----------------------------------------------------------------------
// Read / Write
//
// Stream::readBytes() already does exactly what readImpl() needs - blocks
// until either `num_bytes` have been read or Stream::setTimeout()'s timeout
// expires - so there's no need to hand-roll an available()/read() polling
// loop here.
// -----------------------------------------------------------------------

int TssStreamComClassBase::readImpl(size_t num_bytes, uint8_t *out)
{
    if(num_bytes == 0) return 0;
    return (int)m_stream.readBytes(out, num_bytes);
}

int TssStreamComClassBase::writeImpl(const uint8_t *bytes, size_t len)
{
    if(len == 0) return 0;
    m_stream.write(bytes, len);
    return 0;
}

// -----------------------------------------------------------------------
// Timeout accessors
//
// Delegate directly to Stream::setTimeout()/getTimeout() rather than
// tracking a separate copy - Stream::readBytes() (used by readImpl() above)
// relies on the same value being set there.
// -----------------------------------------------------------------------

uint32_t TssStreamComClassBase::getTimeoutImpl()
{
    return (uint32_t)m_stream.getTimeout();
}

void TssStreamComClassBase::setTimeoutImpl(uint32_t timeout_ms)
{
    m_stream.setTimeout(timeout_ms);
}

#endif /* ARDUINO */
