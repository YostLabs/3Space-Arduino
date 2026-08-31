#ifndef __TSS_ARDUINO_SPI_COM_CLASS_H__
#define __TSS_ARDUINO_SPI_COM_CLASS_H__

#if defined(ARDUINO)

#include "tss/cpp/com/managed_com_class.hpp"

#include <Arduino.h>
#include <SPI.h>

/**
 * @brief SPIComClass that allows specifying buffers sizes.
 *
 * Implemented on top of tss::ManagedComClass (see bindings/cpp/include/tss/
 * cpp/com/managed_com_class.hpp), which provides peek/length/read_until/
 * clear support and buffered writes for free by wrapping this class's *Impl
 * hooks in a struct TSS_Managed_Com_Class - so this class only has to
 * implement the underlying hardware operations (openImpl/closeImpl/
 * readImpl/writeImpl/setTimeoutImpl/getTimeoutImpl) plus the SPI transaction
 * protocol itself (readNoIrq/readWithDataAvailableIrq/readWithFullIrq). 
 * Reenumeration/auto-detection are intentionally left unimplemented (falls back to
 * ManagedComClass's default "unsupported" behaviour), since SPI devices
 * live at a fixed hardware address.
 */
class TssSpiComClassBase : public tss::ManagedComClass {
public:

    /**
     * @brief Controls which read function implementation is used (see
     * setReadMode()/getReadMode()). This is the *desired* mode - the actual
     * function used is capped by whichever IRQ pins are currently configured
     * via setIrqPins().
     */
    enum ReadMode {
        ReadModeBasic = 0,          ///< No IRQ Pins
        ReadModeDataAvailable = 1,  ///< Data Available IRQ Pins
        ReadModeFullIRQ = 2         ///< Both Data Available and Data Loaded IRQ Pins
    };

    /**
     * @param cs_pin     Chip select pin used to communicate with the sensor.
     * @param clock_rate SPI clock rate, in Hz.
     * @param spi        The SPI bus to use.
     * @param read_buf/read_buf_size/write_buf/write_buf_size Buffers owned
     * and supplied by the most-derived subclass - see TSS_MANAGED_COM_INIT()
     * above for the easiest way to provide these.
     */
    TssSpiComClassBase(uint8_t cs_pin, uint32_t clock_rate, SPIClass &spi, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size);

    /**
     * @brief Opens/Initializes the SPI bus and CS pin.
     * @return 0 on success, non-zero on error.
     */
    int openImpl() override;

    /**
     * @brief Closes the SPI bus.
     * @return 0 on success, non-zero on error.
     */
    int closeImpl() override;

    /**
     * @brief High-level read that reads up to num_bytes, in chunks, using the
     * current read strategy (see setIrqPins()/setReadMode()), until the
     * timeout set via setTimeout() expires.
     * @return Number of bytes read, or negative on error.
     */
    int readImpl(size_t num_bytes, uint8_t *out) override;

    /**
     * @brief Writes len bytes out over SPI.
     * @return 0 on success, non-zero on error.
     */
    int writeImpl(const uint8_t *bytes, size_t len) override;

    /** @brief Sets the timeout, in milliseconds, used by readImpl(). 0 = non-blocking. */
    void setTimeoutImpl(uint32_t timeout_ms) override;
    /** @return Current timeout, in milliseconds. */
    uint32_t getTimeoutImpl() override;

    /** @brief Sets the SPI clock rate, in Hz. */
    void setClockRate(uint32_t clock_rate);
    /** @return Current SPI clock rate, in Hz. */
    uint32_t getClockRate() const;

    /**
     * @brief Optionally sets up pins to utilize the Data Available and Data Loaded
     * GPIO IRQ lines from the sensor. Not required for operation, but may improve
     * performance/reduce CPU usage. See the SPI backend documentation for details.
     *
     * This only assigns the pins - it does not by itself decide which read
     * function is used. The actual read function is re-evaluated afterwards
     * based on the current ReadMode (see setReadMode()) capped by which pins
     * are available here.
     * @param data_available_pin_num GPIO pin number for the Data Available line. -1 to disable.
     * @param data_loaded_pin_num    GPIO pin number for the Data Loaded line. -1 to disable.
     */
    void setIrqPins(int data_available_pin_num, int data_loaded_pin_num);

    /**
     * @brief Sets the desired read mode, i.e. the highest read strategy
     * allowed to be used. The actual read function used is re-evaluated
     * immediately, and is the highest of readMode achievable given the pins
     * currently configured via setIrqPins() - e.g. requesting
     * ReadModeFullIRQ with only the Data Available pin configured results in
     * readWithDataAvailableIrq() being used instead.
     * @param mode Desired read mode. Defaults to ReadModeBasic.
     */
    void setReadMode(ReadMode mode);

    /** @return The currently desired read mode, as set via setReadMode(). */
    ReadMode getReadMode() const;

private:
    SPIClass &m_spi;
    uint8_t m_cs_pin;
    uint32_t m_clock_rate;
    SPISettings m_settings;

    // Timeout used by readImpl() (milliseconds). 0 = non-blocking.
    uint32_t m_timeout;
    // Timeout for specifically the header portion of a Transactional Response.
    uint32_t m_header_timeout;

    // Optional, so can be negative.
    int m_data_available_pin_num;
    int m_data_loaded_pin_num;

    // Highest read mode requested via setReadMode(). Defaults to ReadModeBasic.
    ReadMode m_desired_read_mode;

    // Read strategy used by readImpl(). Swapped by updateReadFn() to change
    // the chunked-read behaviour without altering any higher-level code.
    typedef int (TssSpiComClassBase::*ReadFn)(uint8_t *out, uint8_t length, uint32_t timeout_ms);
    ReadFn m_read_fn;

    // Recomputes m_read_fn based on m_desired_read_mode and the IRQ pins
    // currently assigned via setIrqPins(). Called by both of those setters.
    void updateReadFn();

    int readNoIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms);
    int readWithDataAvailableIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms);
    int readWithFullIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms);
};

/**
 * @brief Arduino specific SPI communication class, using the default
 * (4096/512 byte) read/write buffer sizes. See TssSpiComClassBase's doc
 * comment above for implementation details, and for how to use a different
 * buffer size instead (e.g. on a memory-constrained board).
 *
 * Usage:
 * @code
 * TssSpiComClass spi(CS_PIN, 5000000);
 *
 * void setup() {
 *     spi.open();
 *     tssCreateSensor(&sensor, spi); // Implicitly converts to a TSS_Com_Class*
 *     tssInitSensor(&sensor);
 * }
 * @endcode
 */
class TssSpiComClass : public TssSpiComClassBase {
public:
    /**
     * @param cs_pin     Chip select pin used to communicate with the sensor.
     * @param clock_rate SPI clock rate, in Hz.
     * @param spi        The SPI bus to use. Defaults to the board's primary SPI bus.
     */
    TssSpiComClass(uint8_t cs_pin, uint32_t clock_rate, SPIClass &spi = SPI);

private:
    TSS_DECLARE_MANAGED_COM_BUFFERS(tss::kManagedComDefaultReadBufferSize, tss::kManagedComDefaultWriteBufferSize)
};

#endif /* ARDUINO */

#endif /* __TSS_ARDUINO_SPI_COM_CLASS_H__ */
