/*
 * Shows using the SoftwareSerial Com Class to communicate with a TSS sensor
 * over UART with an Ascii terminal. This example will not work if your board
 * does not support SoftwareSerial
 *
 * This is a minimal example to show doing raw communication 
 * without using the TSS_Sensor object. The TSS_Sensor object
 * is not usable on devices with low memory (Arduino UNO) and so
 * the communication class is the largest abstraction that can be
 * obtained on these less powerful devices.
 */

#include <TSS.h>
#include <TSS/com/software_serial.h>
#include <SoftwareSerial.h>

//---------------------------PINS---------------------------
#define RX_PIN 2
#define TX_PIN 3

// Full sized atomatic buffer allocation
SoftwareSerial mySerial(RX_PIN, TX_PIN);
// TssSoftwareSerialComClass uartCom(115200, mySerial);

// Manual buffer allocation. Required for smaller memory devices
// such as Arduino UNO. Defaulting to this for compatability, but
// the above method is preferred if RAM space allows.
uint8_t readBuf[256];
uint8_t writeBuf[256];

TssSoftwareSerialComClassBase uartCom(115200, mySerial, readBuf, sizeof(readBuf), writeBuf, sizeof(writeBuf));

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

  // Depending on your processor, it may not be able to achieve the 115200 default
  // speed used by yostlabs sensors with SoftwareSerial. When this occurs, you will
  // see incomplete/corrupt responses. The following code can be uncommented to
  // reduce the baud rate.
  // Note: 19200 was selected in our testing with an Arduino Uno as the largest baudrate
  // that did not return any corrupted data when sending '?all\n'
  
  // const char uartSetting[] = "!uart_baudrate=19200\n";
  // uartCom.beginWrite();
  // uartCom.write(uartSetting, sizeof(uartSetting)-1);
  // uartCom.endWrite();
  // uartCom.clearImmediate();
  // uartCom.setBaudrate(19200);
  // uartCom.close();
  // uartCom.open();
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
