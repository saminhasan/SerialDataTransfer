#ifndef BINARY_PARSER_H
#define BINARY_PARSER_H
#define USB_BUFFER_SIZE 511872

#include <Arduino.h>
#include "headerBytes.h"
#include "utils.h"
const int N = 21328;
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
  uint8_t rawBytes[N * 6];
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

void sendBinaryPayload(uint8_t *payload, size_t payloadSize, size_t chunkSize)
{
  // Serial.write(SOH);  // Start of transmission
  // delay(5);  // Small delay to prevent buffer overflow

  size_t bytesSent = 0;
  while (bytesSent < payloadSize)
  {
    size_t currentChunkSize = min(chunkSize, payloadSize - bytesSent);
    Serial.write(&payload[bytesSent], currentChunkSize);
    // Serial.send_now();
    bytesSent += currentChunkSize;

    // // Optional: Wait for ACK from receiver
    // while (Serial.available() == 0);
    // Serial.read();  // Read ACK (ignore content)
    // Serial.write(EOT);  // Start of transmission
  }

  // Serial.write(EOT);  // End of transmission
}

void processBinaryPayload(byte header, int length)
{
  // byte payload[length];
  int bytesRead = 0;
  while (bytesRead < length)
  {
    int availableBytes = Serial.available();
    if (availableBytes > 0)
    {
      int readSize = min(availableBytes, length - bytesRead);
      // Serial.readBytes(reinterpret_cast<char *>(payload + bytesRead), readSize);
      Serial.readBytes(reinterpret_cast<char *>(thetaBuffer.rawBytes + bytesRead), readSize);
      bytesRead += readSize;
      // Serial.write(ACK);
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

  case 0x03: // Header for SensorData
    // deserializePayload(payload, thetaBuffer.thetas);
    Serial.println("<Thetas:");
    for (int i = 0; i < N; i++)
    {
      Serial.print("<\t"); // Tab for spacing
      Serial.print("Row ");
      Serial.print(i);
      Serial.print(": ");
      for (int j = 0; j < 6; j++)
      {
        thetaBuffer.thetas[i][j] = 1;
        Serial.print(thetaBuffer.thetas[i][j], 4); // Print with 4 decimal places
        Serial.print("\t");                        // Tab for spacing
      }
      Serial.println(">");
      // delay(1);
    }
    Serial.println(">");

    break;
  default:
    Serial.printf("<Unknown header: %u>\n", header);
    break;
  }
  Serial.printf("<DONE>");
  delay(1);
  sendBinaryPayload(thetaBuffer.rawBytes, length, USB_BUFFER_SIZE);
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
