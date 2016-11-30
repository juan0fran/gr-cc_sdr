#ifndef INCLUDED_CONV_CODER_CC_H
#define INCLUDED_CONV_CODER_CC_H
#include <stdint.h>

unsigned short calcCRC(uint8_t crcData, unsigned short crcReg);
unsigned short fecDecode(uint8_t *pDecData, uint8_t* pInData, unsigned short nRemBytes);
unsigned short fecEncode(uint8_t * data_in, uint8_t * data_out, int len);

#endif