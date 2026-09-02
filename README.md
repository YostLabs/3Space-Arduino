<center><h2>YostLabs 3-Space Sensors - Arduino Library</h2></center>

This library is the Arduino port of Yost Labs' [3Space-C_API](https://github.com/YostLabs/3Space-C_API), providing the same sensor API along with ready-to-use SPI, I2C, and Serial/UART communication classes. Includes limited support for memory-constrained boards like the Arduino UNO.

## Features
* Ready-made SPI, I2C, and Serial/UART Communication Classes -- no need to implement your own
* Low-level communication access for memory-constrained boards where the full sensor object isn't practical
* Handles communication errors and data misalignment automatically
* Compatible with 32-bit boards (e.g. ESP32) and limited features for AVR (e.g. Arduino UNO) 

## Installation

### Arduino Library Manager
Open the Arduino IDE, go to **Sketch > Include Library > Manage Libraries...**, search for **YostLabs 3-Space Sensors**, and click Install.

### Manual
Download this repository as a ZIP (**Code > Download ZIP**), then in the Arduino IDE go to **Sketch > Include Library > Add .ZIP Library...** and select the downloaded file.

## Basic Usage
```cpp
#include <TSS.h>
#include <TSS/com/spi.h>

#define CS_PIN 5

TssSpiComClass m_spi_com(CS_PIN, 5000000);
TSS_Sensor m_sensor;

void setup() {
  m_spi_com.open();
  tssCreateSensor(&m_sensor, m_spi_com);
  tssInitSensor(&m_sensor);
}

void loop() {
  float quaternion[4];
  sensorGetTaredOrientation(&m_sensor, quaternion);
}
```

## Examples
* **BasicSensorSPI** -- Full sensor object usage over SPI: reading orientation, acceleration, timestamps, and firmware info.
* **AsciiTerminalX** -- Raw ASCII command communication using only the Communication Class, without the sensor object. Intended for low-memory boards (e.g. Arduino UNO) where the full sensor object isn't practical.

See the [examples folder](./examples/) for the full sketches.

## Hardware Notes
3-Space sensors use 3.3V logic. Boards with 5V logic (e.g. Arduino UNO) require a bi-directional logic level converter between the board and the sensor. It is recommended to use an active level converter rather than passive level converter for communication reliability.

## Documentation
This library wraps the [3Space-C_API](https://github.com/YostLabs/3Space-C_API), which contains the full API reference and additional platform-agnostic documentation. For Communication Class details, see the [wiki](https://github.com/YostLabs/3Space-C_API/wiki/Communication-Class).

## Support
Questions or issues: [open an Issue](../../issues) or email techsupport@yostlabs.com.

## License
See [LICENSE](./LICENSE) for details.
