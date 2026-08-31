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
// Defaulting to low speed of 100Khz to guarantee example 
// works with dupont cables and slow passive logic level converters.
#define SPI_DEFAULT_CLK 100000

// Controlling CS pin manually instead of using built in SPI clock pin to handle
// more advance CS control to go with the expected protocol.
#define CS_PIN 5

//---------------------------PINS---------------------------
#define AVAILABLE_DATA_PIN 27
#define LOADED_DATA_PIN 26

//---------------------------INITIALIZATION/APPLICATION---------------------------
// If using an interface other than SPI, all that needs to change is this.
TssSpiComClass m_spi_com(CS_PIN, SPI_DEFAULT_CLK);
// uint8_t read_buf[512];
// uint8_t write_buf[512];
// TssSpiComClassBase m_spi_com(CS_PIN, SPI_DEFAULT_CLK, SPI, read_buf, sizeof(read_buf), write_buf, sizeof(write_buf));

// Will contain a generic reference to the Com Class to allow
// this example to function regardless of what interface is stored here.
struct TSS_Com_Class *m_com;

static TSS_Sensor m_sensor;

void printArray(float *data, uint8_t size);

void setup() {
  // Set pin mode
  Serial.begin(115200);
  Serial.println("Starting");

  //Create and open the communication object
  m_com = m_spi_com;
  tss_com_set_timeout(m_com, 1000);
  tss_com_open(m_com);
  Serial.println("Com Opened");

  //Create the sensor object from the communication object
  printf("Creating sensor object...\n");
  tssCreateSensor(&m_sensor, m_spi_com);
  tssInitSensor(&m_sensor);
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
  sensorReadVersionFirmware(&m_sensor, output, sizeof(output));
  Serial.print("Firmware Version: ");
  Serial.println(output);
  sensorReadVersionHardware(&m_sensor, output, sizeof(output));
  Serial.print("Hardware Version: ");
  Serial.println(output);

  float quaternion[4];
  float accel[3];
  sensorGetTaredOrientation(&m_sensor, quaternion);
  sensorGetCorrectedAccelerometerVector(&m_sensor, accel);

  Serial.print("Quaternion: ");
  printArray(quaternion, 4);
  Serial.print("Accel: ");
  printArray(accel, 3);

  uint64_t time;
  sensorGetTimestamp(&m_sensor, &time);
  Serial.print("Time: ");
  Serial.println(time);

  sensorSoftwareReset(&m_sensor);

  time = 0;
  sensorGetTimestamp(&m_sensor, &time);
  Serial.print("Time: ");
  Serial.println(time);
}

