#if !defined(TSS_HAS_SOFTWARE_SERIAL)
#if defined(ARDUINO) && (defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_MEGAAVR) || (defined(TSS_FORCE_SOFTWARE_SERIAL) && TSS_FORCE_SOFTWARE_SERIAL))
#define TSS_HAS_SOFTWARE_SERIAL 1
#else
#define TSS_HAS_SOFTWARE_SERIAL 0
#endif
#endif

#if TSS_HAS_SOFTWARE_SERIAL

#include "tss/com/software_serial.h"

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TssSoftwareSerialComClassBase::TssSoftwareSerialComClassBase(uint32_t baudrate, SoftwareSerial &serial, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size) :
    TssStreamComClassBase(serial, read_buf, read_buf_size, write_buf, write_buf_size),
    m_serial(serial),
    m_baudrate(baudrate)
{
}

TssSoftwareSerialComClass::TssSoftwareSerialComClass(uint32_t baudrate, SoftwareSerial &serial) :
    TSS_MANAGED_COM_INIT(TssSoftwareSerialComClassBase, baudrate, serial)
{
}

// -----------------------------------------------------------------------
// Open / Close
// -----------------------------------------------------------------------

int TssSoftwareSerialComClassBase::openImpl()
{
    m_serial.begin(m_baudrate);
    return 0;
}

int TssSoftwareSerialComClassBase::closeImpl()
{
    m_serial.end();
    return 0;
}

// -----------------------------------------------------------------------
// Baudrate accessors
// -----------------------------------------------------------------------

void TssSoftwareSerialComClassBase::setBaudrate(uint32_t baudrate)
{
    m_baudrate = baudrate;
}

uint32_t TssSoftwareSerialComClassBase::getBaudrate() const
{
    return m_baudrate;
}

#endif /* TSS_HAS_SOFTWARE_SERIAL */
