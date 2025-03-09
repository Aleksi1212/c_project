#ifndef CONSTANTS_H
#define CONSTANTS_H

#define COMMAND_LIST_ARR_SIZE 5
#define HEADER_SIZE 16       // 3 x 4 bytes (uint32_t) + 2 x 2 bytes (uint16_t) = 16 bytes
#define CRC32_SIZE 4         // 4 bytes for CRC32 (uint32_t)
#define CRC32_OFFSET (CRC32_SIZE)
#define HEADER_OFFSET (HEADER_SIZE)
#define CRC32_HEADER_OFFSET (CRC32_OFFSET + HEADER_OFFSET)

#endif
