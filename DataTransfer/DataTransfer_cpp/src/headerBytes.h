#ifndef HEADER_BYTES_H
#define HEADER_BYTES_H

// General Control Bytes
#define NUL  0x00  // Null character (NUL) - Represents a zero value

// Transmission Control Characters
#define SOH   0x01  // Start of Heading (SOH) - Marks the start of a packet header
#define STX   0x3C  // Start of Text (STX) - Indicates the beginning of the main payload
#define ETX   0x3E  // End of Text (ETX) - Marks the end of the payload content
#define EOT   0x04  // End of Transmission (EOT) - Indicates the end of an entire message

// Acknowledgment Control
#define ACK   0x06  // Acknowledge (ACK) - Confirms successful reception of data
#define NAK   0x15  // Negative Acknowledge (NAK) - Signals a transmission error or rejection

// Block Separators (For Chunked Data Transfer)
#define EOB   0x17  // End of Block (ETB) - Marks the end of a block of data (not full transmission)

// Flow Control Characters
#define XON   0x11  // Device Control One (DC1) - Resume transmission (used in software flow control)
#define XOFF  0x13  // Device Control Three (DC3) - Pause transmission (used in software flow control)

// Data Structure Separators
#define US    0x1F  // Unit Separator (US) - Separates individual fields within a record
#define RS    0x1E  // Record Separator (RS) - Separates complete records in a transmission
#define GS    0x1D  // Group Separator (GS) - Separates groups of records
#define FS    0x1C  // File Separator (FS) - Marks logical file boundaries

#endif // HEADER_BYTES_H
