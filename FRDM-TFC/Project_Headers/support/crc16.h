#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>

#define CRC16_FIRSTPASS 0xFFFF

uint16_t CRC16(uint16_t crc_in, uint8_t *data_p, uint16_t length);

#endif