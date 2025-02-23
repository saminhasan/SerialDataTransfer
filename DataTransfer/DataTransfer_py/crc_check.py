import binascii
import numpy as np
import serial

SERIAL_PORT = "COM8"
BAUD_RATE = 4608000


def compute_crc32(data: bytes) -> int:
    return binascii.crc32(data) & 0xFFFFFFFF


# Example Usage
if __name__ == "__main__":
    # Example 1: Compute CRC32 of a float array (N x M)
    N, M = 21328, 6  # Same as C++
    float_arr = np.arange(N * M, dtype=np.float32).reshape(N, M)
    crc_float_array = compute_crc32(float_arr.tobytes())
    print(f"python : CRC32 of float array: 0x{crc_float_array:08X}")
    with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1) as teensy:
        recv = teensy.readline()
        print(f"teensy : {recv.decode()}")
"""
python  : CRC32 of float array: 0x6CCB2E56
arduino : CRC32 of float array: 0x6CCB2E56
pio     : CRC32 of float array: 0x6E3BCDC6
"""
