#ifndef BINARY_PARSER_H
#define BINARY_PARSER_H
#define USB_BUFFER_SIZE 511872

#include <Arduino.h>
#include "headerBytes.h"
#include "utils.h"
#include <FastCRC.h>
FastCRC32 CRC32;


const int N = 2731;
struct MotorCommand
{
  int speed;
  int direction;
};

struct SensorData
{
  float temperature;
  float pressure;
};

DMAMEM union
{
  float thetas[N][6];
  uint8_t rawBytes[sizeof(float) * N * 6];
} thetaBuffer;
void init_theta()
{
  for (int row = 0; row < N; row++)
  {
    for (int col = 0; col < 6; col++)

    {
      thetaBuffer.thetas[row][col] = 0;
    }
  }
}
MotorCommand motorCmd;
SensorData sensorData;

void sendBinaryPayload(uint8_t *payload, size_t payloadSize)
{
  Serial.write(payload, payloadSize);
}

void processBinaryPayload(byte header, int length)
{
  int bytesRead = Serial.readBytes(reinterpret_cast<char *>(thetaBuffer.rawBytes), length);
  if (bytesRead != length)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    while (1)
    {
      ;
    }
  }
  while (Serial.available() < 1)
  {
    delay(1);
  }

  Serial.printf("<EOT : %u>\n", Serial.read());

  // Deserialize into the correct struct based on header
  switch (header)
  {
    // case 0x01: // Header for MotorCommand
    //   deserializePayload(payload, motorCmd);
    //   Serial.printf("<Motor Command -> Speed: %d, Direction: %d>\n", motorCmd.speed, motorCmd.direction);
    //   break;

    // case 0x02: // Header for SensorData
    //   deserializePayload(payload, sensorData);
    //   Serial.printf("<Sensor Data -> Temperature: %.2f, Pressure: %.2f>\n", sensorData.temperature, sensorData.pressure);
    //   break;

  case 0x03:
    break;
  default:
    // Serial.printf("<Unknown header: %u>\n", header);
    break;
  }
  // Serial.printf("<DONE>");
  delay(1);
  sendBinaryPayload(thetaBuffer.rawBytes, length);
  uint32_t crc32 = CRC32.crc32(thetaBuffer.rawBytes, sizeof(thetaBuffer.rawBytes));
  sendBinaryPayload((uint8_t *)&crc32, sizeof(crc32));
}
void processBinaryInput(void)
{
  byte header;
  int length_payload;
  byte endByte;
  // Wait until at least 6 bytes are available
  while (Serial.available() < 6)
    ;
  header = Serial.read();
  Serial.readBytes(reinterpret_cast<char *>(&length_payload), sizeof(length_payload));
  endByte = Serial.read();

  // Handle different end byte cases
  switch (endByte)
  {
  case EOT:
    Serial.printf("<READ : %u>\n", header);
    break;
  case EOB:
    Serial.printf("<HEADER : %u, SIZE : %d>\n", header, length_payload);
    processBinaryPayload(header, length_payload);
    break;
  default:
    Serial.printf("<ERR->endByte : %u>\n", endByte);
    break;
  }
}

#endif // BINARY_PARSER_H
