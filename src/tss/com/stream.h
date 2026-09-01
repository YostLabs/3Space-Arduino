#ifndef __TSS_ARDUINO_STREAM_COM_CLASS_H__
#define __TSS_ARDUINO_STREAM_COM_CLASS_H__

#if defined(ARDUINO)

#include "tss/cpp/com/managed_com_class.hpp"

#include <Arduino.h>

/**
 * @brief Generic com class wrapping any Arduino Stream (HardwareSerial,
 * SoftwareSerial, a TCP Client, etc.) as a plain byte pass-through.
 *
 * Implemented on top of tss::ManagedComClass (see bindings/cpp/include/tss/
 * cpp/com/managed_com_class.hpp), which provides peek/length/read_until/
 * clear support and buffered writes for free by wrapping this class's *Impl
 * hooks in a struct TSS_Managed_Com_Class - so this class only has to
 * implement the underlying operations (openImpl/closeImpl/readImpl/
 * writeImpl/setTimeoutImpl/getTimeoutImpl).
 *
 * This is deliberately generic - it wraps a Stream& rather than any
 * specific transport, so it works with anything that behaves like a plain
 * byte stream: a HardwareSerial (e.g. Serial1/Serial2 on boards that have
 * them), a SoftwareSerial, etc.
 * Since Stream itself has no open()/close()/baud-rate concept,
 * openImpl()/closeImpl() here are no-ops - the caller is responsible for
 * constructing and opening/configuring (e.g. `begin()`-ing) the concrete
 * object themselves *before* calling open() on this class.
 *
 * @note There are helper classes for hardware or software UARTs that
 * can be used (TssHardwareSerialComClass, TssSoftwareSerialComClass) to
 * avoid manual use of this Stream class.
 *
 * Reenumeration/auto-detection are intentionally left unimplemented (falls
 * back to ManagedComClass's default "unsupported" behaviour), since a
 * Stream-based transport doesn't have a generic notion of "other candidate
 * devices" to scan.
 */
class TssStreamComClassBase : public tss::ManagedComClass {
public:

    /**
     * @param stream   The already-configured Stream (e.g. Serial, Serial1,
     * a SoftwareSerial, a connected Client, etc.) to use. Unless a subclass
     * says otherwise, the caller must have already opened/configured it
     * (e.g. called its begin()) before opening this class.
     * @param read_buf/read_buf_size/write_buf/write_buf_size Buffers owned
     * and supplied by the most-derived subclass - see TSS_MANAGED_COM_INIT()
     * above for the easiest way to provide these.
     */
    TssStreamComClassBase(Stream &stream, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size);

    /**
     * @brief No-op - the underlying Stream is expected to already be open
     * (see the class doc comment above).
     * @return Always 0.
     */
    int openImpl() override;

    /**
     * @brief No-op - a generic Stream has no notion of closing.
     * @return Always 0.
     */
    int closeImpl() override;

    /**
     * @brief Reads up to num_bytes via Stream::readBytes(), which blocks
     * until either all bytes have been read or the timeout set via
     * setTimeout() (i.e. Stream::setTimeout()) expires.
     * @return Number of bytes read, or negative on error.
     */
    int readImpl(size_t num_bytes, uint8_t *out) override;

    /**
     * @brief Writes len bytes out over the stream.
     * @return 0 on success, non-zero on error.
     */
    int writeImpl(const uint8_t *bytes, size_t len) override;

    /** @brief Sets the timeout, in milliseconds, used by readImpl() (Stream::setTimeout()). 0 = non-blocking. */
    void setTimeoutImpl(uint32_t timeout_ms) override;
    /** @return Current timeout, in milliseconds (Stream::getTimeout()). */
    uint32_t getTimeoutImpl() override;

private:
    Stream &m_stream;
};

/**
 * @brief Arduino specific generic Stream communication class, using the
 * default (4096/512 byte) read/write buffer sizes. See
 * TssStreamComClassBase's doc comment above for implementation details, and
 * for how to use a different buffer size instead (e.g. on a
 * memory-constrained board).
 *
 * @note For a hardware or software UART specifically, prefer
 * TssHardwareSerialComClass/TssSoftwareSerialComClass instead, which
 * additionally handle begin()/end() - see stream.h's doc comment above.
 *
 * Usage:
 * @code
 * TssStreamComClass com(Serial); // Wraps the default Serial (UART0) stream.
 *
 * void setup() {
 *     Serial.begin(9600); // Caller opens/configures the stream.
 *     com.open();
 *     tssCreateSensor(&sensor, com); // Implicitly converts to a TSS_Com_Class*
 *     tssInitSensor(&sensor);
 * }
 * @endcode
 */
class TssStreamComClass : public TssStreamComClassBase {
public:
    /**
     * @param stream The already-configured Stream to use.
     */
    explicit TssStreamComClass(Stream &stream);

private:
    TSS_DECLARE_MANAGED_COM_BUFFERS(tss::kManagedComDefaultReadBufferSize, tss::kManagedComDefaultWriteBufferSize)
};

#endif /* ARDUINO */

#endif /* __TSS_ARDUINO_STREAM_COM_CLASS_H__ */

