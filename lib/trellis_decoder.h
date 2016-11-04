#ifndef INCLUDED_TRELLIS_DECODER_CC_H
#define INCLUDED_TRELLIS_DECODER_CC_H
#include <stdint.h>

unsigned short calcCRC(uint8_t crcData, unsigned short crcReg);
unsigned short fecDecode(uint8_t *pDecData, uint8_t* pInData, unsigned short nRemBytes);

#endif