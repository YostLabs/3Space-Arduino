#ifndef __TSS_ARDUINO_HARDWARE_SERIAL_COM_CLASS_H__
#define __TSS_ARDUINO_HARDWARE_SERIAL_COM_CLASS_H__

#if defined(ARDUINO)

#include "tss/com/stream.h"

/**
 * @brief UartComClass wrapping a hardware UART (a HardwareSerial, e.g.
 * Serial/Serial1/Serial2 - whichever ones a given board exposes).
 *
 * Derives from TssStreamComClassBase (see stream.h), which implements
 * readImpl()/writeImpl()/setTimeoutImpl()/getTimeoutImpl() generically in
 * terms of the Arduino Stream interface - this class only adds
 * openImpl()/closeImpl() overrides that additionally call
 * HardwareSerial::begin()/end() with the configured baud rate, since
 * (unlike a plain Stream) a HardwareSerial does have that concept.
 *
 * Reenumeration/auto-detection are intentionally left unimplemented (falls
 * back to ManagedComClass's default "unsupported" behaviour), since a
 * hardware UART lives at a fixed set of pins.
 */
class TssHardwareSerialComClassBase : public TssStreamComClassBase {
public:

    /**
     * @param baudrate Baud rate to configure the UART with when opened.
     * @param serial   The hardware UART to use.
     * @param read_buf/read_buf_size/write_buf/write_buf_size Buffers
     */
    TssHardwareSerialComClassBase(uint32_t baudrate, HardwareSerial &serial, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size);

    /**
     * @brief Opens the UART at the configured baud rate (HardwareSerial::begin()).
     * @return Always 0.
     */
    int openImpl() override;

    /**
     * @brief Closes the UART (HardwareSerial::end()).
     * @return Always 0.
     */
    int closeImpl() override;

    /** @brief Sets the UART baud rate. Takes effect the next time the UART is opened. */
    void setBaudrate(uint32_t baudrate);
    /** @return Current UART baud rate. */
    uint32_t getBaudrate() const;

private:
    HardwareSerial &m_serial;
    uint32_t m_baudrate;
};

/**
 * @brief Arduino specific hardware UART communication class, using the
 * default (4096/512 byte) read/write buffer sizes. See
 * TssHardwareSerialComClassBase's doc comment above for implementation
 * details, and for how to use a different buffer size instead (e.g. on a
 * memory-constrained board).
 *
 * Usage:
 * @code
 * TssHardwareSerialComClass uart(115200); // Uses Serial by default.
 *
 * void setup() {
 *     uart.open();
 *     tssCreateSensor(&sensor, uart); // Implicitly converts to a TSS_Com_Class*
 *     tssInitSensor(&sensor);
 * }
 * @endcode
 */
class TssHardwareSerialComClass : public TssHardwareSerialComClassBase {
public:
    /**
     * @param baudrate Baud rate to configure the UART with when opened.
     * @param serial   The hardware UART to use. Defaults to the board's primary UART.
     */
    explicit TssHardwareSerialComClass(uint32_t baudrate = 115200, HardwareSerial &serial = Serial);

private:
    TSS_DECLARE_MANAGED_COM_BUFFERS(tss::kManagedComDefaultReadBufferSize, tss::kManagedComDefaultWriteBufferSize)
};

#endif /* ARDUINO */

#endif /* __TSS_ARDUINO_HARDWARE_SERIAL_COM_CLASS_H__ */
