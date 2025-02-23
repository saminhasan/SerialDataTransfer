// make processBinaryPayload non blocking but sending can't be non-blocking, can it?
// implement read
// implement all vars arrays and structs
// impplement rounting
#include <Arduino.h>
#include "headerBytes.h"
#include "binaryParser.h"
#include "textParser.h"

void init_theta();

void setup()
{
  if (CrashReport)
    pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(4608000);
  while (!Serial)
    delay(1);
  if (CrashReport)
  {
    Serial.print("<");
    Serial.print(CrashReport);
    Serial.print(">");
  }
  Serial.println("<READY>");
}

void loop(void)
{

}

void serialEvent(void)
{
  if (Serial.available() > 0)
  {
    char startByte = Serial.read();

    switch (startByte)
    {
    case SOH:
      processBinaryInput();
      break;
    case STX:
      processTextInput();
      break;
    default:
      Serial.printf("<ERR->startByte : %u>\n", startByte);
      break;
    }
  }
  else
    return;
}
