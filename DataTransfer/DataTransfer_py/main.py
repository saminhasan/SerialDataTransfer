import time
import serial
import struct
import numpy as np
import binascii
from headerBytes import *
from itertools import batched

chunk_size = 16


def compute_crc32(data: bytes) -> int:
    """Computes the CRC32 checksum for the entire data in one go."""
    return binascii.crc32(data) & 0xFFFFFFFF  # Ensure 32-bit unsigned result


SERIAL_PORT = "COM8"
BAUD_RATE = 4608000

N, M = 2731, 6  # DMAMEM MAX
float_values = np.arange(N * M, dtype=np.float32).reshape(N, M)

struct_format_string = f"{float_values.size}f"
payload = struct.pack(struct_format_string, *float_values.flatten())
payload_length = len(payload)

with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1) as teensy:
    print("Connected")
    print(f"Payload size : {payload_length} bytes")

    if teensy.in_waiting:
        print(f"Teensy : {teensy.readline().decode().strip()}")
    teensy.reset_input_buffer()

    header = bytearray([SOH, 0x03])
    header += payload_length.to_bytes(4, byteorder="little", signed=True)
    header.append(EOB)
    teensy.write(header)

    print(f"Teensy : {teensy.readline().decode().strip()}")

    total_start_time = time.time_ns()
    for batch in batched(payload, chunk_size):
        teensy.write(batch)
    # teensy.write(payload)
    total_end_time = time.time_ns()

    elapsed_time = (total_end_time - total_start_time) / 1_000_000_000
    elapsed_time = max(elapsed_time, 1 / 1_000_000_000)
    speed_kb_s = (payload_length / 1024 / 1024) / elapsed_time

    print(f"Data Transfer Speed: {speed_kb_s:.2f} MB/s.")
    print(f"Payload Size : {payload_length} bytes | Time taken {elapsed_time}s.")

    teensy.write(bytes([EOT]))
    print(f"Teensy : {teensy.readline().decode().strip()}")

    recv_buffer_bin = bytearray(payload_length)
    total_start_time = time.time_ns()
    recv_buffer_bin = teensy.read(payload_length)
    total_end_time = time.time_ns()

    elapsed_time = (total_end_time - total_start_time) / 1_000_000_000
    elapsed_time = max(elapsed_time, 1 / 1_000_000_000)
    speed_kb_s = (payload_length / 1024 / 1024) / elapsed_time

    print(f"Data Transfer Speed: {speed_kb_s:.2f} MB/s.")
    print(f"Payload Size : {payload_length} bytes | Time taken {elapsed_time}s.")

    array_recv = np.array(struct.unpack(struct_format_string, recv_buffer_bin)).reshape(N, M)

    print(np.max(float_values - array_recv))
    if np.allclose(float_values, array_recv, atol=1e-12):
        print("Arrays are the same (within floating-point precision).")
    else:
        print("Arrays are different.")

    if np.array_equal(float_values, array_recv):
        print("Arrays are exactly the same.")
    else:
        print("Arrays are different.")

    print(array_recv[:5])

    crc32_bytes = teensy.read(4)
    formatted_crc_recv = " ".join(f"0x{byte:02X}" for byte in crc32_bytes)
    print(f"Received CRC32 : [{formatted_crc_recv}]")

    computed_crc32 = compute_crc32(float_values.tobytes())
    formatted_computed_crc32 = " ".join(f"0x{byte:02X}" for byte in computed_crc32.to_bytes(4, "little"))
    print(f"Computed CRC32 : [{formatted_computed_crc32}]")

    print(
        "CRC32 Match: Data integrity verified."
        if crc32_bytes == computed_crc32.to_bytes(4, "little")
        else "CRC32 Mismatch: Data corruption detected!"
    )
