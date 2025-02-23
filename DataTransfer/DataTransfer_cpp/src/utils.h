#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <FastCRC.h>

// Convert 1-byte array to bool using bitwise operations
void bytesToBool(byte arr[1], bool *value) {
  *value = arr[0] & 1;  // Ensure it's strictly 0 or 1
}
// Convert 4-byte array to int (little-endian)
void bytesToInt(byte arr[4], int *value) {
    *value = (arr[0]) | (arr[1] << 8) | (arr[2] << 16) | (arr[3] << 24);
}

void bytesToFloat(byte arr[4], float *value) {
    *value = *reinterpret_cast<float*>(arr);
}



// Generic function to deserialize a payload into any struct
template <typename T>
void deserializePayload(uint8_t *payload, T &var) {
    uint8_t *varPtr = reinterpret_cast<uint8_t *>(&var);
    for (size_t i = 0; i < sizeof(T); i++) {
        varPtr[i] = payload[i]; // Assigning byte-by-byte
    }
}


uint32_t computeCRC32(uint8_t *data, size_t dataSize) {
    FastCRC32 CRC32;
    return CRC32.crc32(data, dataSize);
}

#endif // UTILS_H
