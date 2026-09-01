#ifndef __TSS_ARDUINO_SOFTWARE_SERIAL_COM_CLASS_H__
#define __TSS_ARDUINO_SOFTWARE_SERIAL_COM_CLASS_H__

/**
 * @note This header (and its .cpp) is guarded on a known-good list of
 * architecture macros (ARDUINO_ARCH_AVR/SAMD/MEGAAVR - cores that bundle
 * SoftwareSerial directly with the board package).
 * For boards/cores not in the list below that are known to genuinely ship
 * SoftwareSerial, define `TSS_FORCE_SOFTWARE_SERIAL` to 1 (e.g. via a
 * build flag) before this header is included.
 */
#if !defined(TSS_HAS_SOFTWARE_SERIAL)
#if defined(ARDUINO) && (defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_MEGAAVR) || (defined(TSS_FORCE_SOFTWARE_SERIAL) && TSS_FORCE_SOFTWARE_SERIAL))
#define TSS_HAS_SOFTWARE_SERIAL 1
#else
#define TSS_HAS_SOFTWARE_SERIAL 0
#endif
#endif

#if TSS_HAS_SOFTWARE_SERIAL

#include "tss/com/stream.h"
#include <SoftwareSerial.h>

/**
 * @brief UartComClass wrapping a software (bit-banged) UART via the
 * SoftwareSerial library - useful on boards (e.g. the Uno) that only have a
 * single HardwareSerial, which is normally reserved for the USB/programming
 * connection.
 *
 * Derives from TssStreamComClassBase (see stream.h), which implements
 * readImpl()/writeImpl()/setTimeoutImpl()/getTimeoutImpl() generically in
 * terms of the Arduino Stream interface - this class only adds
 * openImpl()/closeImpl() overrides that additionally call
 * SoftwareSerial::begin()/end() with the configured baud rate, since
 * (unlike a plain Stream) a SoftwareSerial does have that concept.
 *
 * Reenumeration/auto-detection are intentionally left unimplemented (falls
 * back to ManagedComClass's default "unsupported" behaviour), since a
 * software UART lives at a fixed set of pins.
 */
class TssSoftwareSerialComClassBase : public TssStreamComClassBase {
public:

    /**
     * @param baudrate Baud rate to configure the UART with when opened.
     * @param serial   The SoftwareSerial instance to use (already
     * constructed with its RX/TX pins).
     * @param read_buf/read_buf_size/write_buf/write_buf_size Buffers owned
     * and supplied by the most-derived subclass - see TSS_MANAGED_COM_INIT()
     * (stream.h) for the easiest way to provide these.
     */
    TssSoftwareSerialComClassBase(uint32_t baudrate, SoftwareSerial &serial, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size);

    /**
     * @brief Opens the UART at the configured baud rate (SoftwareSerial::begin()).
     * @return Always 0.
     */
    int openImpl() override;

    /**
     * @brief Closes the UART (SoftwareSerial::end()).
     * @return Always 0.
     */
    int closeImpl() override;

    /** @brief Sets the UART baud rate. Takes effect the next time the UART is opened. */
    void setBaudrate(uint32_t baudrate);
    /** @return Current UART baud rate. */
    uint32_t getBaudrate() const;

private:
    SoftwareSerial &m_serial;
    uint32_t m_baudrate;
};

/**
 * @brief Arduino specific software UART communication class, using the
 * default (4096/512 byte) read/write buffer sizes. See
 * TssSoftwareSerialComClassBase's doc comment above for implementation
 * details, and for how to use a different buffer size instead (e.g. on a
 * memory-constrained board).
 *
 * Usage:
 * @code
 * SoftwareSerial softSerial(rxPin, txPin);
 * TssSoftwareSerialComClass uart(9600, softSerial);
 *
 * void setup() {
 *     uart.open();
 *     tssCreateSensor(&sensor, uart); // Implicitly converts to a TSS_Com_Class*
 *     tssInitSensor(&sensor);
 * }
 * @endcode
 */
class TssSoftwareSerialComClass : public TssSoftwareSerialComClassBase {
public:
    /**
     * @param baudrate Baud rate to configure the UART with when opened.
     * @param serial   The SoftwareSerial instance to use (already
     * constructed with its RX/TX pins). There is no default - unlike a
     * hardware UART, there's no single "primary" software UART to fall
     * back to.
     */
    TssSoftwareSerialComClass(uint32_t baudrate, SoftwareSerial &serial);

private:
    TSS_DECLARE_MANAGED_COM_BUFFERS(tss::kManagedComDefaultReadBufferSize, tss::kManagedComDefaultWriteBufferSize)
};

#endif /* TSS_HAS_SOFTWARE_SERIAL */

#endif /* __TSS_ARDUINO_SOFTWARE_SERIAL_COM_CLASS_H__ */
