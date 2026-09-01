/*
 * Shows using the HardwareSerial Com Class to communicate with a TSS sensor
 * over UART with an Ascii terminal. This will only compile on devices with at least
 * 2 Hardware Serial interfaces as it uses the primary interface to communicate
 * with the computer.
 *
 * This is a minimal example to show doing raw communication 
 * without using the TSS_Sensor object. The TSS_Sensor object
 * is not usable on devices with low memory (Arduino UNO) and so
 * the communication class is the largest abstraction that can be
 * obtained on these less powerful devices.
 */

#include <TSS.h>
#include <TSS/com/hardware_serial.h>

//---------------------------PINS---------------------------
// Full sized atomatic buffer allocation
// TssHardwareSerialComClass uartCom(115200, Serial1);

// Manual buffer allocation. Required for smaller memory devices
// such as Arduino UNO. Defaulting to this for compatability, but
// the above method is preferred if RAM space allows.
uint8_t readBuf[256];
uint8_t writeBuf[256];

TssHardwareSerialComClassBase uartCom(115200, Serial1, readBuf, sizeof(readBuf), writeBuf, sizeof(writeBuf));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting");

  //Configure and open the communication object
  uartCom.setTimeout(1000);
  if(uartCom.open()) {
    Serial.println("Failed to open.");
  }
  Serial.println("Initialized communication.");
}

void readAndPrintLine() {
  char result[100];
  bool done = false;

  while(!done) {
    int numRead = uartCom.readUntil('\n', (uint8_t*)result, sizeof(result));
    
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
  uartCom.clearImmediate();

  // Write ascii command to the sensor
  uartCom.beginWrite();
  uartCom.write((uint8_t*)incomingCommand.c_str(), incomingCommand.length());
  uartCom.endWrite();

  // Read response
  readAndPrintLine();
}
