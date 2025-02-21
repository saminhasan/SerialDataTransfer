import time
import serial
import struct
import numpy as np
from headerBytes import *
from itertools import batched

# Serial Configuration
SERIAL_PORT = "COM8"
BAUD_RATE = 4608000
CHUNK_SIZE = 511872

N, M = 21328, 6
float_values = np.arange(N * M, dtype=np.float32).reshape(N, M)
print(float_values[0:5])
# Pack data
struct_format_string = f"{float_values.size}f"
payload = struct.pack(struct_format_string, *float_values.flatten())
payload_length = len(payload)

with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0.1) as teensy:
    print("Connected")
    print(f"Payload size : {payload_length} bytes")

    if teensy.in_waiting:
        response = teensy.readline().decode().strip()
        print(f"Teensy : {response}")
    teensy.reset_input_buffer()

    header = bytearray([SOH, 0x03])
    header += payload_length.to_bytes(4, byteorder="little", signed=True)  # Append bytes correctly
    header.append(EOB)  # End of block
    teensy.write(header)

    ack = teensy.readline().decode().strip()
    print(f"Teensy : {ack}")

    # Start measuring time before transmission
    total_start_time = time.time_ns()

    for chunk in batched(payload, CHUNK_SIZE):
        # print(".", end="")
        teensy.write(bytes(chunk))
        # time.sleep(1e-5)

    print()
    total_end_time = time.time_ns()
    elapsed_time = (total_end_time - total_start_time) / 1_000_000_000  # Convert ns to seconds
    if elapsed_time == 0:
        elapsed_time = 1 / 1_000_000_000
    speed_kb_s = (payload_length / 1024 / 1024) / elapsed_time
    print(f"Data Transfer Speed: {speed_kb_s:.2f} MB/s.")
    print(f"Payload Size : {payload_length} bytes | Time taken {elapsed_time}s.")

    teensy.write(bytes([EOT]))  # Send EOT
    eot = teensy.readline().decode().strip()
    print(f"Teensy : {eot}")

    recv_buffer_txt = []
    recv_buffer = []
    inside_packet = False  # Flag to track if we are inside a packet
    temp = []
    TxtinProgress = False
    while True:
        if teensy.in_waiting > 0:
            temp = teensy.read(teensy.in_waiting)  # Read raw bytes
            recv_buffer.append(temp)
        if temp:
            if temp[0] == STX:
                TxtinProgress = True
            if temp[-1] == "\n":
                TxtinProgress = False
            if TxtinProgress:
                recv_buffer_txt.append(temp)
                # Check for "DONE" signal
                if "<DONE>".encode("utf-8") in temp:
                    print("\nDone!")
                    break

    # Preallocate buffer
    recv_buffer_bin = bytearray(payload_length)
    totalBytesRead = 0
    # Start measuring time before transmission
    total_start_time = time.time_ns()
    while totalBytesRead < payload_length:
        if teensy.in_waiting > 0:
            bytes_to_read = min(teensy.in_waiting, payload_length - totalBytesRead)  # Read only remaining bytes
            recv_buffer_bin[totalBytesRead : totalBytesRead + bytes_to_read] = teensy.read(bytes_to_read)  # Store directly
            totalBytesRead += bytes_to_read
            # print(totalBytesRead)  # Debugging: Track progress
    total_end_time = time.time_ns()

    elapsed_time = (total_end_time - total_start_time) / 1_000_000_000  # Convert ns to seconds
    if elapsed_time == 0:
        elapsed_time = 1 / 1_000_000_000
    speed_kb_s = (payload_length / 1024 / 1024) / elapsed_time
    print(f"Data Transfer Speed: {speed_kb_s:.2f} MB/s.")
    print(f"Payload Size : {payload_length} bytes | Time taken {elapsed_time}s.")
print(f"{totalBytesRead=}")
data_recv = struct.unpack(struct_format_string, recv_buffer_bin)
array_recv = np.array(data_recv).reshape(N, M)
print(array_recv.shape)
print(np.max(float_values - array_recv))
if np.allclose(float_values, array_recv, atol=1e-12):
    print("Arrays are the same (within floating-point precision).")
else:
    print("Arrays are different.")
if np.array_equal(float_values, array_recv):
    print("Arrays are exactly the same.")
else:
    print("Arrays are different.")

print(array_recv[0:5])
