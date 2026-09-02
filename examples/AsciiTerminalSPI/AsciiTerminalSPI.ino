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
// Defaulting to low speed of 100Khz to help the example 
// work with DuPont cables and slow passive logic level converters.
// If using a logic level converter, then it is recommended to use 
// an active logic level converter for reliable high speed SPI communication.
#define SPI_DEFAULT_CLK 100000

// Controlling CS pin manually instead of using built in SPI clock pin to handle
// more advance CS control to go with the expected protocol.
#define CS_PIN 5

//---------------------------PINS---------------------------
#define AVAILABLE_DATA_PIN 27
#define LOADED_DATA_PIN 26

// Full sized atomatic buffer allocation
// TssSpiComClass spiCom(CS_PIN, SPI_DEFAULT_CLK);

// Manual buffer allocation. Required for smaller memory devices
// such as Arduino UNO. Defaulting to this for compatability, but
// the above method is preferred if RAM space allows.
uint8_t readBuf[256];
uint8_t writeBuf[256];
TssSpiComClassBase spiCom(CS_PIN, SPI_DEFAULT_CLK, SPI, readBuf, sizeof(readBuf), writeBuf, sizeof(writeBuf));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting");

  //Configure and open the communication object
  spiCom.setTimeout(1000);
  if(spiCom.open()) {
    Serial.println("Failed to open.");
    while(true);
  }
  Serial.println("Initialized communication.");

  // This on its lonesome doesn't change the mode used, just gives 
  // pin info and configures the pins as inputs. Not required.
  // spiCom.setIrqPins(AVAILABLE_DATA_PIN, LOADED_DATA_PIN);
}

void readAndPrintLine() {
  char result[100];
  bool done = false;

  while(!done) {
    int numRead = spiCom.readUntil('\n', (uint8_t*)result, sizeof(result));
    
    if(numRead < 0) {
      Serial.print("  Failed to read - Error ");
      Serial.print(numRead);
      break;
    }

    // Print out each received character
    for(int i = 0; i < numRead; i++) {
      // Reached the end of the line. Break for
      // consistent new line printing
      if(result[i] == '\n') {
        done = true;
        break;
      }
      Serial.print(result[i]);
    }

    // Reached end of available data
    if(numRead < sizeof(result)) {
      break;
    }
  }

  Serial.println();
}

void loop() {
  Serial.println("Enter ascii command (With New Line):");
  while(!Serial.available());

  String incomingCommand = Serial.readStringUntil('\n');
  incomingCommand += '\n';

  // Clear any leftover response before sending command
  // (could occur due to limited buffer size or sensor already
  //  having responses available)
  spiCom.clearImmediate();

  // Write ascii command to the sensor
  spiCom.beginWrite();
  spiCom.write((uint8_t*)incomingCommand.c_str(), incomingCommand.length());
  spiCom.endWrite();

  // Read response
  readAndPrintLine();
}
