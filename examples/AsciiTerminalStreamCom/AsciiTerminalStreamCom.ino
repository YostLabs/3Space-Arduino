/*
 * Shows using the StreamComClass with a manually created
 * communication interface that implements the Stream protocol.
 * This example will not compile if your device does not support
 * SoftwareSerial.
 * 
 * This example utilizes the SoftwareSerial stream to showcase
 * this functionality. Depending on your processor, there is a decent
 * chance you may see data corruption in responses, especially longer
 * ones, as the software UART may not be able to achieve 115200HZ.
 */

#include <TSS.h>
#include <TSS/com/stream.h>
#include <SoftwareSerial.h>

//---------------------------PINS---------------------------

// Set the pins to perform software serial on
#define RX_PIN 2
#define TX_PIN 3

// Full sized atomatic buffer allocation
// TssStreamComClass streamCom(mySerial);

// Manual buffer allocation. Required for smaller memory devices
// such as Arduino UNO. Defaulting to this for compatability, but
// the above method is preferred if RAM space allows.
uint8_t readBuf[256];
uint8_t writeBuf[256];

SoftwareSerial mySerial(RX_PIN, TX_PIN);
TssStreamComClassBase streamCom(mySerial, readBuf, sizeof(readBuf), writeBuf, sizeof(writeBuf));

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting");

  // Default baudrate for YostLabs sensors
  mySerial.begin(115200);

  //Configure and open the communication object
  streamCom.setTimeout(1000);
  if(streamCom.open()) {
    Serial.println("Failed to open.");
    while(true);
  }
  Serial.println("Initialized communication.");
}

void readAndPrintLine() {
  char result[100];
  bool done = false;

  while(!done) {
    int numRead = streamCom.readUntil('\n', (uint8_t*)result, sizeof(result));
    
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
  // Would only occur in an error state
  streamCom.clearImmediate();

  // Write ascii command to the sensor
  streamCom.beginWrite();
  streamCom.write((uint8_t*)incomingCommand.c_str(), incomingCommand.length());
  streamCom.endWrite();

  // Read response
  readAndPrintLine();
}
