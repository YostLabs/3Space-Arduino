#if defined(ARDUINO)

#include "tss/com/spi.h"
#include "tss/constants.h"
#include "tss/sys/time.h"
#include "tss/errors.h"

#define IRQ_ACTIVE_STATE LOW
#define IRQ_INACTIVE_STATE (!IRQ_ACTIVE_STATE)

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TssSpiComClassBase::TssSpiComClassBase(uint8_t cs_pin, uint32_t clock_rate, SPIClass &spi, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size) :
    tss::ManagedComClass(/*supports_reenumeration=*/false, read_buf, read_buf_size, write_buf, write_buf_size),
    m_spi(spi),
    m_cs_pin(cs_pin),
    m_clock_rate(clock_rate),
    m_settings(clock_rate, MSBFIRST, SPI_MODE0),
    m_timeout(1000),
    m_header_timeout(1),
    m_data_available_pin_num(-1),
    m_data_loaded_pin_num(-1),
    m_desired_read_mode(ReadModeBasic),
    m_read_fn(&TssSpiComClassBase::readNoIrq)
{
}

TssSpiComClass::TssSpiComClass(uint8_t cs_pin, uint32_t clock_rate, SPIClass &spi) :
    TSS_MANAGED_COM_INIT(TssSpiComClassBase, cs_pin, clock_rate, spi)
{
}

// -----------------------------------------------------------------------
// Open / Close
// -----------------------------------------------------------------------

int TssSpiComClassBase::openImpl()
{
    m_settings = SPISettings(m_clock_rate, MSBFIRST, SPI_MODE0);

    pinMode(m_cs_pin, OUTPUT);
    digitalWrite(m_cs_pin, HIGH); // Set CS high

    m_spi.begin();

    return 0;
}

int TssSpiComClassBase::closeImpl()
{
    m_spi.end();
    return 0;
}

void TssSpiComClassBase::setIrqPins(int data_available_pin_num, int data_loaded_pin_num)
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

void TssSpiComClassBase::setReadMode(ReadMode mode)
{
    m_desired_read_mode = mode;
    updateReadFn();
}

TssSpiComClassBase::ReadMode TssSpiComClassBase::getReadMode() const
{
    return m_desired_read_mode;
}

void TssSpiComClassBase::updateReadFn()
{
    // Highest read mode achievable given the currently assigned IRQ pins.
    ReadMode max_read_mode = ReadModeBasic;
    if(m_data_available_pin_num >= 0) {
        max_read_mode = (m_data_loaded_pin_num >= 0) ? ReadModeFullIRQ : ReadModeDataAvailable;
    }

    ReadMode actual_read_mode = (m_desired_read_mode < max_read_mode) ? m_desired_read_mode : max_read_mode;

    switch(actual_read_mode) {
        case ReadModeFullIRQ:
            m_read_fn = &TssSpiComClassBase::readWithFullIrq;
            break;
        case ReadModeDataAvailable:
            m_read_fn = &TssSpiComClassBase::readWithDataAvailableIrq;
            break;
        case ReadModeBasic:
        default:
            m_read_fn = &TssSpiComClassBase::readNoIrq;
            break;
    }
}

int TssSpiComClassBase::writeImpl(const uint8_t *bytes, size_t len)
{
    if (len == 0) return 0;

    m_spi.beginTransaction(m_settings);
    digitalWrite(m_cs_pin, LOW); // Set CS low
    while(len > 0) {
        uint8_t send_len = (len > 255) ? 255 : (uint8_t)len;
        m_spi.transfer(TSS_TRANSACTION_WRITE_DATA_BYTE);
        m_spi.transfer(send_len);
        for(size_t i = 0; i < send_len; ++i) {
            m_spi.transfer(bytes[i]);
        }

        len -= send_len;
        bytes += send_len;
    }
    digitalWrite(m_cs_pin, HIGH); // Set CS high
    m_spi.endTransaction();
    return 0;
}

// -----------------------------------------------------------------------
// Protocol read (no-IRQ polling style)
// -----------------------------------------------------------------------

int TssSpiComClassBase::readNoIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms)
{
    (void) timeout_ms;
    
    if (length == 0) return 0;
    // Send READ_DATA_WITH_SIZE command followed by the requested byte count.
    uint8_t header[2] = { TSS_TRANSACTION_READ_DATA_WITH_SIZE_BYTE, length };

    m_spi.beginTransaction(m_settings);

    //Start the read of up to length bytes
    digitalWrite(m_cs_pin, LOW); // Set CS low
    m_spi.transfer(header, 2);
    digitalWrite(m_cs_pin, HIGH); // Set CS high

    //Get the header
    uint8_t status = 0xFF, data_len = 0;
    tss_time_t start = tssTimeGet();
    uint32_t elapsed_time = 0;
    while (status == 0xFF && elapsed_time <= m_header_timeout) {
        //Doing in this order to ensure a toggle between iterations, and that it stays low after the while loop.
        digitalWrite(m_cs_pin, HIGH);
        digitalWrite(m_cs_pin, LOW);

        memset(header, 0xFF, sizeof(header));
        m_spi.transfer(header, sizeof(header));
        
        status   = header[0];
        data_len = header[1];
        
        //Corrupted status value, assume unknown and retry.
        if(status != 0xFF && status > TSS_TRANSACTION_STATUS_MAX_VALUE) {
            status = 0xFF;
        }

        elapsed_time = tssTimeDiff(start);
    }

    // Header not found.
    if(status == 0xFF) {
        digitalWrite(m_cs_pin, HIGH); // Set CS high
        m_spi.endTransaction();
        return TSS_ERR_TIMEOUT;
    }

    if (data_len > 0) {
        //Guard against buffer overrun. This should never occur unless there is an issue with the SPI lines.
        if(data_len > length) {
            digitalWrite(m_cs_pin, HIGH); // Set CS high
            m_spi.endTransaction();
            return -1;
        }
        m_spi.transfer(out, data_len);
    }
    digitalWrite(m_cs_pin, HIGH); // Set CS high
    m_spi.endTransaction();
    return data_len;
}

int TssSpiComClassBase::readWithDataAvailableIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms)
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

int TssSpiComClassBase::readWithFullIrq(uint8_t *out, uint8_t length, uint32_t timeout_ms)
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
            //Read data until cleared.
            uint8_t clear_buffer[40];
            m_spi.beginTransaction(m_settings);
            do {
                memset(clear_buffer, 0xFF, sizeof(clear_buffer));
                digitalWrite(m_cs_pin, LOW); // Set CS low
                m_spi.transfer(clear_buffer, sizeof(clear_buffer));
                digitalWrite(m_cs_pin, HIGH); // Set CS high
            } while(digitalRead(m_data_loaded_pin_num) == IRQ_ACTIVE_STATE && tssTimeDiff(start_time) < (timeout_ms + m_header_timeout) * 2);
            m_spi.endTransaction();
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
    uint8_t header[2] = { TSS_TRANSACTION_READ_DATA_WITH_SIZE_BYTE, length };
    m_spi.beginTransaction(m_settings);

    digitalWrite(m_cs_pin, LOW); // Set CS low
    m_spi.transfer(header, sizeof(header));
    digitalWrite(m_cs_pin, HIGH); // Set CS high

    //Wait until the data is loaded
    start_time = tssTimeGet();
    elapsed_time = 0;
    while(digitalRead(m_data_loaded_pin_num) == IRQ_INACTIVE_STATE) {
        if(elapsed_time > timeout_ms + m_header_timeout) {
            digitalWrite(m_cs_pin, HIGH); // Set CS high
            m_spi.endTransaction();
            return -1; //Somehow failed to load data
        }
        elapsed_time = tssTimeDiff(start_time);
    }

    //Read the header
    digitalWrite(m_cs_pin, LOW); // Set CS low
    m_spi.transfer(header, sizeof(header));
    
    uint8_t status = header[0];
    uint8_t data_len = header[1];

    //This should never occur when using the data loaded pin, but checking anyways
    if(status == 0xFF) {
        digitalWrite(m_cs_pin, HIGH); // Set CS high
        m_spi.endTransaction();
        return -1;
    }

    if(data_len > 0) {
        //This shouldn't occur unless there is an
        //issue with the SPI lines. Checking anyways
        //to ensure no buffer overruns.
        if(data_len > length) {
            digitalWrite(m_cs_pin, HIGH); // Set CS high
            m_spi.endTransaction();
            return -1;
        }
        m_spi.transfer(out, data_len);
    }

    digitalWrite(m_cs_pin, HIGH); // Set CS high
    m_spi.endTransaction();
    return data_len;
}

// -----------------------------------------------------------------------
// High-level read (uses m_read_fn and m_timeout)
// -----------------------------------------------------------------------

int TssSpiComClassBase::readImpl(size_t num_bytes, uint8_t *out)
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

uint32_t TssSpiComClassBase::getTimeoutImpl()
{
    return m_timeout;
}

void TssSpiComClassBase::setTimeoutImpl(uint32_t timeout_ms)
{
    m_timeout = timeout_ms;
}

void TssSpiComClassBase::setClockRate(uint32_t clock_rate)
{
    m_clock_rate = clock_rate;
    m_settings = SPISettings(clock_rate, MSBFIRST, SPI_MODE0);
}

uint32_t TssSpiComClassBase::getClockRate() const
{
    return m_clock_rate;
}

#endif /* ARDUINO */
