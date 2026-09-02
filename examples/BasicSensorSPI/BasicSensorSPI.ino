/*
 * Example showing using the communication class with a sensor object
 * to take full advantage of the API. Shows simple command and settings
 * usage. Intended for 32 bit platforms.
 */

#include <TSS.h>
#include <TSS/com/spi.h>

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

/*
* ESP32 SPI pins default to:
* VSPI
* MOSI: 23
* MISO: 19
* SCK: 18
*/

// Can run up to 10MHz based on sensor settings and wiring.
// Defaulting to low speed of 100Khz to help example 
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

//---------------------------INITIALIZATION/APPLICATION---------------------------
// If using an interface other than SPI, all that needs to change is this.
TssSpiComClass spiCom(CS_PIN, SPI_DEFAULT_CLK);

// uint8_t readBuf[512];
// uint8_t writeBuf[512];
// TssSpiComClassBase spiCom(CS_PIN, SPI_DEFAULT_CLK, SPI, readBuf, sizeof(readBuf), writeBuf, sizeof(writeBuf));

// Will contain a generic reference to the Com Class to allow
// this example to function regardless of what interface is stored here.
TssComClass& com = spiCom;

static TSS_Sensor sensor;

void printArray(float *data, uint8_t size);

void setup() {
  // Set pin mode
  Serial.begin(115200);
  Serial.println("Starting");

  //Create and open the communication object
  com.setTimeout(1000);
  if(com.open()) {
    Serial.println("Failed to open.");
    while(true);
  }
  
  Serial.println("Com Opened");

  //Create the sensor object from the communication object
  printf("Creating sensor object...\n");
  tssCreateSensor(&sensor, com);
  tssInitSensor(&sensor);
}

void printArray(float *data, uint8_t size) {
  for(uint8_t i = 0; i < size; i++) {
    Serial.print(data[i]);
    Serial.print(" ");
  }
}

//Example functions of using the API
void loop() {
  while(!Serial.available());

  char output[100] = {0};
  sensorReadVersionFirmware(&sensor, output, sizeof(output));
  Serial.print("Firmware Version: ");
  Serial.println(output);
  sensorReadVersionHardware(&sensor, output, sizeof(output));
  Serial.print("Hardware Version: ");
  Serial.println(output);

  float quaternion[4];
  float accel[3];
  sensorGetTaredOrientation(&sensor, quaternion);
  sensorGetCorrectedAccelerometerVector(&sensor, accel);

  Serial.print("Quaternion: ");
  printArray(quaternion, 4);
  Serial.print("Accel: ");
  printArray(accel, 3);

  uint64_t time;
  sensorGetTimestamp(&sensor, &time);
  Serial.print("Time: ");
  Serial.println(time);

  sensorSoftwareReset(&sensor);

  time = 0;
  sensorGetTimestamp(&sensor, &time);
  Serial.print("Time: ");
  Serial.println(time);
}

