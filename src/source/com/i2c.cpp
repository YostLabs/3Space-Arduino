#if defined(ARDUINO)

#include "tss/com/i2c.h"
#include "tss/constants.h"
#include "tss/sys/time.h"
#include "tss/errors.h"

#define IRQ_ACTIVE_STATE LOW
#define IRQ_INACTIVE_STATE (!IRQ_ACTIVE_STATE)

// Most Arduino cores' Wire implementation uses a small, fixed-size internal
// buffer for both TX and RX (32 bytes on classic AVR boards, via
// BUFFER_LENGTH). Writes must be chunked to fit within it (minus 2 bytes
// reserved for the transaction header), since all bytes of a write have to
// be buffered before endTransmission() sends them as a single transaction.
// This is also used as the per-requestFrom() chunk size in basicRead(),
// which works around the same limit on the read side by issuing multiple
// requestFrom() calls instead. On cores that support growing the buffer at
// runtime (see openImpl()), a larger size is assumed.
#if WIRE_HAS_BUFFER_SIZE
static const uint8_t kI2cMaxChunkSize = 255;
#elif defined(BUFFER_LENGTH)
static const uint8_t kI2cMaxChunkSize = BUFFER_LENGTH;
#else
static const uint8_t kI2cMaxChunkSize = 32;
#endif

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TssI2cComClassBase::TssI2cComClassBase(uint8_t i2c_address, uint32_t clock_rate, TwoWire &wire, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size) :
    tss::ManagedComClass(/*supports_reenumeration=*/false, read_buf, read_buf_size, write_buf, write_buf_size),
    m_wire(wire),
    m_i2c_address(i2c_address),
    m_clock_rate(clock_rate),
    m_timeout(1000),
    m_header_timeout(1),
    m_data_available_pin_num(-1),
    m_data_loaded_pin_num(-1),
    m_desired_read_mode(ReadModeBasic),
    m_read_fn(&TssI2cComClassBase::readNoIrq)
{
}

TssI2cComClass::TssI2cComClass(uint8_t i2c_address, uint32_t clock_rate, TwoWire &wire) :
    TSS_MANAGED_COM_INIT(TssI2cComClassBase, i2c_address, clock_rate, wire)
{
}

// -----------------------------------------------------------------------
// Open / Close
// -----------------------------------------------------------------------

int TssI2cComClassBase::openImpl()
{
#if WIRE_HAS_BUFFER_SIZE
    m_wire.setBufferSize(kI2cMaxChunkSize);
#endif
    m_wire.begin();
    m_wire.setClock(m_clock_rate);

    return 0;
}

int TssI2cComClassBase::closeImpl()
{
#if WIRE_HAS_END
    m_wire.end();
#endif
    return 0;
}

void TssI2cComClassBase::setIrqPins(int data_available_pin_num, int data_loaded_pin_num)
{
    m_data_available_pin_num = data_available_pin_num;
    m_data_loaded_pin_num = data_loaded_pin_num;

    if(data_available_pin_num >= 0) {
        pinMode(data_available_pin_num, INPUT);
    }

    if(data_loaded_pin_num >= 0) {
        pinMode(data_loaded_pin_num, INPUT);
    }

    updateReadFn();
}

void TssI2cComClassBase::setReadMode(ReadMode mode)
{
    m_desired_read_mode = mode;
    updateReadFn();
}

TssI2cComClassBase::ReadMode TssI2cComClassBase::getReadMode() const
{
    return m_desired_read_mode;
}

void TssI2cComClassBase::updateReadFn()
{
    // Highest read mode achievable given the currently assigned IRQ pins.
    ReadMode max_read_mode = ReadModeBasic;
    if(m_data_available_pin_num >= 0) {
        max_read_mode = (m_data_loaded_pin_num >= 0) ? ReadModeFullIRQ : ReadModeDataAvailable;
    }

    ReadMode actual_read_mode = (m_desired_read_mode < max_read_mode) ? m_desired_read_mode : max_read_mode;

    switch(actual_read_mode) {
        case ReadModeFullIRQ:
            m_read_fn = &TssI2cComClassBase::readWithFullIrq;
            break;
        case ReadModeDataAvailable:
            m_read_fn = &TssI2cComClassBase::readWithDataAvailableIrq;
            break;
        case ReadModeBasic:
        default:
            m_read_fn = &TssI2cComClassBase::readNoIrq;
            break;
    }
}

int TssI2cComClassBase::writeImpl(const uint8_t *bytes, size_t len)
{
    if (len == 0) return 0;

    static const uint8_t kI2cMaxWriteChunkSize = kI2cMaxChunkSize - 2; // Reserve 2 bytes for the transaction header.

    while(len > 0) {
        uint8_t send_len = (len > kI2cMaxWriteChunkSize) ? kI2cMaxWriteChunkSize : (uint8_t)len;

        m_wire.beginTransmission(m_i2c_address);
        m_wire.write(TSS_TRANSACTION_WRITE_DATA_BYTE);
        m_wire.write(send_len);
        m_wire.write(bytes, send_len);
        if(m_wire.endTransmission() != 0) {
            return -1;
        }

        len -= send_len;
        bytes += send_len;
    }
    return 0;
}

// -----------------------------------------------------------------------
// Protocol read (no-IRQ polling style)
// -----------------------------------------------------------------------

// Reads up to `length` bytes from the sensor, issuing as many requestFrom()
// calls as necessary to work around the Wire library's small, fixed-size
// internal RX buffer (see kI2cMaxChunkSize). This lets callers treat it as a
// single logical read of up to 255 bytes, matching the SPI backend's
// readNoIrq()/readWithFullIrq(), which aren't limited by any such buffer.
uint8_t TssI2cComClassBase::basicRead(uint8_t *out, uint8_t length, uint32_t timeout_ms)
{
    uint8_t total_read = 0;
    tss_time_t start_time = tssTimeGet();

    while(total_read < length) {
        // Only request more data once everything previously requested has been consumed.
        if(m_wire.available() == 0) {
            uint8_t remaining = length - total_read;
            uint8_t request_len = (remaining > kI2cMaxChunkSize) ? kI2cMaxChunkSize : remaining;
            m_wire.requestFrom(m_i2c_address, request_len);
        }

        // Read all available bytes from the Wire buffer.
        bool read_any = false;
        while(m_wire.available() > 0 && total_read < length) {
            out[total_read++] = m_wire.read();
            read_any = true;
        }

        // Only give up on timeout if nothing at all was read this iteration.
        if(!read_any && tssTimeDiff(start_time) >= timeout_ms) {
            break;
        }
    }

    return total_read;
}

int TssI2cComClassBase::readNoIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms)
{
    (void) timeout_ms;

    if (length == 0) return 0;

    // Send READ_DATA_WITH_SIZE command followed by the requested byte count.
    m_wire.beginTransmission(m_i2c_address);
    m_wire.write(TSS_TRANSACTION_READ_DATA_WITH_SIZE_BYTE);
    m_wire.write(length);
    if(m_wire.endTransmission() != 0) {
        return -1;
    }

    //Get the header
    uint8_t status = 0xFF, data_len = 0;
    tss_time_t start = tssTimeGet();
    uint32_t elapsed_time = 0;
    while (status == 0xFF && elapsed_time <= m_header_timeout) {
        if(m_wire.requestFrom(m_i2c_address, (uint8_t)2) == 2) {
            status   = m_wire.read();
            data_len = m_wire.read();

            //Corrupted status value, assume unknown and retry.
            if(status != 0xFF && status > TSS_TRANSACTION_STATUS_MAX_VALUE) {
                status = 0xFF;
            }
        }

        elapsed_time = tssTimeDiff(start);
    }

    // Header not found.
    if(status == 0xFF) {
        return TSS_ERR_TIMEOUT;
    }

    if (data_len > 0) {
        //Guard against buffer overrun. This should never occur unless there is an issue with the I2C lines.
        if(data_len > length) {
            return -1;
        }

        uint8_t received = basicRead(out, data_len, timeout_ms);
        if(received < data_len) {
            return -1;
        }
    }
    return data_len;
}

int TssI2cComClassBase::readWithDataAvailableIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms)
{
    if (length == 0) return 0;

    // Wait for the Data Available line to go low
    tss_time_t start = tssTimeGet();
    uint32_t elapsed_time = 0;
    while (digitalRead(m_data_available_pin_num) == IRQ_INACTIVE_STATE) {
        if(elapsed_time > timeout_ms) {
            return TSS_ERR_TIMEOUT;
        }
        elapsed_time = tssTimeDiff(start);
    }

    //Then do a normal read
    return readNoIrq(out, length, timeout_ms);
}

int TssI2cComClassBase::readWithFullIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms)
{
    if(length == 0) return 0;

    //Wait for data_loaded pin to reset
    //This should normally take no time, but it is possible to read so fast
    //back to back that his pin may not have been deasserted yet.
    //Doing this check here instead of after reading to avoid wasting time when could continue processing.
    tss_time_t start_time = tssTimeGet();
    tss_time_t elapsed_time = 0;

    while(digitalRead(m_data_loaded_pin_num) == IRQ_ACTIVE_STATE) {
        if(elapsed_time > timeout_ms + m_header_timeout) {
            //There might actually be data loaded that shouldn't be there if this times out.
            //Clear it.
            uint8_t clear_buffer[kI2cMaxChunkSize];
            do {
                basicRead(clear_buffer, sizeof(clear_buffer), timeout_ms);
            } while(digitalRead(m_data_loaded_pin_num) == IRQ_ACTIVE_STATE && tssTimeDiff(start_time) < (timeout_ms + m_header_timeout) * 2);
            return TSS_ERR_TIMEOUT;
        }
        elapsed_time = tssTimeDiff(start_time);
    }

    //Wait for data to be available
    elapsed_time = 0;
    while(digitalRead(m_data_available_pin_num) == IRQ_INACTIVE_STATE) {
        if(elapsed_time > timeout_ms) {
            return TSS_ERR_TIMEOUT;
        }
        elapsed_time = tssTimeDiff(start_time);
    }

    //Start the read
    m_wire.beginTransmission(m_i2c_address);
    m_wire.write(TSS_TRANSACTION_READ_DATA_WITH_SIZE_BYTE);
    m_wire.write(length);
    if(m_wire.endTransmission() != 0) {
        return -1;
    }

    //Wait until the data is loaded
    start_time = tssTimeGet();
    elapsed_time = 0;
    while(digitalRead(m_data_loaded_pin_num) == IRQ_INACTIVE_STATE) {
        if(elapsed_time > timeout_ms + m_header_timeout) {
            return -1; //Somehow failed to load data
        }
        elapsed_time = tssTimeDiff(start_time);
    }

    //Read the header
    if(m_wire.requestFrom(m_i2c_address, (uint8_t)2) != 2) {
        return -1;
    }
    uint8_t status = m_wire.read();
    uint8_t data_len = m_wire.read();

    //This should never occur when using the data loaded pin, but checking anyways
    if(status == 0xFF) {
        return -1;
    }

    if(data_len > 0) {
        //This shouldn't occur unless there is an
        //issue with the I2C lines. Checking anyways
        //to ensure no buffer overruns.
        if(data_len > length) {
            return -1;
        }

        uint8_t received = basicRead(out, data_len, timeout_ms);
        if(received < data_len) {
            return -1;
        }
    }

    return data_len;
}

// -----------------------------------------------------------------------
// High-level read (uses m_read_fn and m_timeout)
// -----------------------------------------------------------------------

int TssI2cComClassBase::readImpl(size_t num_bytes, uint8_t *out)
{
    if (m_read_fn == NULL || num_bytes == 0) return 0;

    size_t total = 0;
    tss_time_t start = tssTimeGet();
    uint32_t elapsed_time = 0;
    while (total < num_bytes && elapsed_time <= m_timeout) {
        size_t chunk = num_bytes - total;
        if (chunk > 255) chunk = 255;

        uint32_t remaining = m_timeout - elapsed_time;
        int n = (this->*m_read_fn)(out + total, (uint8_t)chunk, remaining);
        if(n >= 0) {
            total += (size_t)n;
        }
        else if(n != TSS_ERR_TIMEOUT) {
            //Hardware error. Timeout errors
            //do not propagate, just return less
            //data than requested. Other errors are fatal.
            return n;
        }

        elapsed_time = tssTimeDiff(start);
    }

    return (int)total;
}

// -----------------------------------------------------------------------
// Timeout / clock rate accessors
// -----------------------------------------------------------------------

uint32_t TssI2cComClassBase::getTimeoutImpl()
{
    return m_timeout;
}

void TssI2cComClassBase::setTimeoutImpl(uint32_t timeout_ms)
{
    m_timeout = timeout_ms;
}

void TssI2cComClassBase::setClockRate(uint32_t clock_rate)
{
    m_clock_rate = clock_rate;
    m_wire.setClock(m_clock_rate);
}

uint32_t TssI2cComClassBase::getClockRate() const
{
    return m_clock_rate;
}

#endif /* ARDUINO */
