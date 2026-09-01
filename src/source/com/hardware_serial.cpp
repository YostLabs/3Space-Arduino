#if defined(ARDUINO)

#include "tss/com/hardware_serial.h"

// -----------------------------------------------------------------------
// Construction
// -----------------------------------------------------------------------

TssHardwareSerialComClassBase::TssHardwareSerialComClassBase(uint32_t baudrate, HardwareSerial &serial, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size) :
    TssStreamComClassBase(serial, read_buf, read_buf_size, write_buf, write_buf_size),
    m_serial(serial),
    m_baudrate(baudrate)
{
}

TssHardwareSerialComClass::TssHardwareSerialComClass(uint32_t baudrate, HardwareSerial &serial) :
    TSS_MANAGED_COM_INIT(TssHardwareSerialComClassBase, baudrate, serial)
{
}

// -----------------------------------------------------------------------
// Open / Close
// -----------------------------------------------------------------------

int TssHardwareSerialComClassBase::openImpl()
{
    m_serial.begin(m_baudrate);
    return 0;
}

int TssHardwareSerialComClassBase::closeImpl()
{
    m_serial.end();
    return 0;
}

// -----------------------------------------------------------------------
// Baudrate accessors
// -----------------------------------------------------------------------

void TssHardwareSerialComClassBase::setBaudrate(uint32_t baudrate)
{
    m_baudrate = baudrate;
}

uint32_t TssHardwareSerialComClassBase::getBaudrate() const
{
    return m_baudrate;
}

#endif /* ARDUINO */
