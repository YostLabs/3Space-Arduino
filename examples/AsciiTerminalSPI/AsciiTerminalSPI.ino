/*
 * Shows using the SPI Com Class to communicate with a TSS sensor
 * over SPI with an Ascii terminal. 
 *
 * This is a minimal example to show doing raw communication 
 * without using the TSS_Sensor object. The TSS_Sensor object
 * is not usable on devices with low memory (Arduino UNO) and so
 * the communication class is the largest abstraction that can be
 * obtained on these less powerful devices.
 */

#include <TSS.h>
#include <TSS/com/spi.h>

/*
* Arduino UNO SPI pins:
* MOSI: pin 11
* MISO: pin 12
* SCK: pin 13
* Note: Arduino UNO is 5V logic
* and the 3-Space sensor is 3V logic.
* A bi-directional level converter is required.
*/

/*
* ESP32 SPI pins default to:
* VSPI
* MOSI: 23
* MISO: 19
* SCK: 18
*/

// Can run up to 10MHz based on sensor settings and wiring.
// Defaulting to low speed of 100Khz to ensure example 
// works with dupont cables and slow passive logic level converters.
// A resistor may still be needed in series on the CLK line to reduce
// communication noise depending on your circuit. Try 200-300 ohms if problematic.
#define SPI_DEFAULT_CLK 100000

// Controlling CS pin manually instead of using built in SPI clock pin to handle
// more advance CS control to go with the expected protocol.
#define CS_PIN 5

//---------------------------PINS---------------------------
#define AVAILABLE_DATA_PIN 27
#define LOADED_DATA_PIN 26

// Full sized atomatic buffer allocation
// TssSpiComClass m_spi_com(CS_PIN, SPI_DEFAULT_CLK);

// Manual buffer allocation. Required for smaller memory devices
// such as Arduino UNO. Defaulting to this for compatability, but
// the above method is preferred if RAM space allows.
uint8_t read_buf[256];
uint8_t write_buf[256];
TssSpiComClassBase m_spi_com(CS_PIN, SPI_DEFAULT_CLK, SPI, read_buf, sizeof(read_buf), write_buf, sizeof(write_buf));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting");

  //Configure and open the communication object
  m_spi_com.setTimeout(1000);
  if(m_spi_com.open()) {
    Serial.println("Failed to open.");
  }
  Serial.println("Initialized communication.");

  // This on its lonesome doesn't change the mode used, just gives 
  // pin info and configures the pins as inputs. Not required.
  // m_spi_com.setIrqPins(AVAILABLE_DATA_PIN, LOADED_DATA_PIN);
}

void loop() {
  Serial.println("Enter ascii command (With New Line):");
  while(!Serial.available());

  String incomingCommand = Serial.readStringUntil('\n');
  incomingCommand += '\n';

  // Clear any leftover response before sending command
  // (could occur due to limited buffer size or sensor already
  //  having responses available)
  m_spi_com.clearImmediate();

  // Write ascii command to the sensor
  m_spi_com.beginWrite();
  int write_result = m_spi_com.write(incomingCommand.c_str(), incomingCommand.length());
  m_spi_com.endWrite();

  // Read response
  char result[100] = {0};
  int num_read = m_spi_com.readUntil('\n', result, sizeof(result));
  if(num_read >= 0) {
    Serial.print("Response: ");
    Serial.println(result);
  }
  else {
    Serial.print("Failed to read - Error ");
    Serial.println(num_read);
  }
}
