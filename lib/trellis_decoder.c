#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "trellis_decoder.h"
/* out of the class gr, an implementation of the FEC-Viterbi decoder is done */

/* Trellis transition definitions */
static const uint8_t aTrellisSourceStateLut[8][2] =
{
{0, 4},// State {0,4} -> State 0
{0, 4},// State {0,4} -> State 1
{1, 5},// State {1,5} -> State 2
{1, 5},// State {1,5} -> State 3
{2, 6},// State {2,6} -> State 4
{2, 6},// State {2,6} -> State 5
{3, 7},// State {3,7} -> State 6
{3, 7},// State {3,7} -> State 7
};

static const uint8_t aTrellisTransitionOutput[8][2] =
{ 
{0, 3},// State {0,4} -> State 0 produces {"00", "11"}
{3, 0},// State {0,4} -> State 1 produces {"11", "00"}
{1, 2},// State {1,5} -> State 2 produces {"01", "10"}
{2, 1},// State {1,5} -> State 3 produces {"10", "01"}
{3, 0},// State {2,6} -> State 4 produces {"11", "00"}
{0, 3},// State {2,6} -> State 5 produces {"00", "11"}
{2, 1},// State {3,7} -> State 6 produces {"10", "01"}
{1, 2},// State {3,7} -> State 7 produces {"01", "10"}
};

static const uint8_t aTrellisTransitionInput[8] = {0,1,0,1,0,1,0,1};
/* end of trellis constants */

/* functions used by Trellis Decoder */
static uint8_t hammWeight(uint8_t a)
{
  a = ((a & 0xAA) >> 1) + (a & 0x55);
  a = ((a & 0xCC) >> 2) + (a & 0x33);
  a = ((a & 0xF0) >> 4) + (a & 0x0F);
  return a;
}

static uint8_t min(uint8_t a, uint8_t b)
{
  return (a <= b ? a : b);
}

unsigned short calcCRC(uint8_t crcData, unsigned short crcReg)
{
  uint8_t i;
  for (i = 0; i < 8; i++) {
      if (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80))
          crcReg = (crcReg << 1) ^ 0x8005;
      else
          crcReg = (crcReg << 1);
      crcData <<= 1;
  }
  return crcReg;
}
/* end of functions used by Trellis */

/* FEC Decoder function */

/**************************************************************************************************************
* @fn
fecDecode
*
* @brief
De-interleaves and decodes a given input buffer
*
* @param
pDecData - Pointer to where to put decoded data (NULL when initializing at start of packet)
*
pInData
- Pointer to received data
*
nRemBytes - of remaining (decoded) bytes to decode
*
*
* @return
Number of bytes of decoded data stored at pDecData
*/

unsigned short fecDecode(uint8_t *pDecData, uint8_t* pInData, unsigned short nRemBytes)
{
  // Two sets of buffers (last, current) for each destination state for holding:
  static uint8_t nCost[2][8];
  // Accumulated path cost
  static unsigned long aPath[2][8];
  // Encoder input data (32b window)
  // Indices of (last, current) buffer for each iteration
  static uint8_t iLastBuf;
  static uint8_t iCurrBuf;
  // Number of bits in each path buffer
  static uint8_t nPathBits;
  // Variables used to hold # Viterbi iterations to run, # bytes output,
  // minimum cost for any destination state, bit index of input symbol
  uint8_t nIterations;
  unsigned short nOutputBytes = 0;
  uint8_t nMinCost;
  signed char iBit = 8 - 2;
  // Initialize variables at start of packet (and return without doing any more)
  if (pDecData == NULL) {
      uint8_t n ;
      memset(nCost, 0, sizeof(nCost));
      for (n = 1; n < 8; n++)
      nCost[0][n] = 100;
      iLastBuf = 0;
      iCurrBuf = 1;
      nPathBits = 0;
      return 0;
  }
  
  uint8_t aDeintData[4];
  signed char iOut;
  signed char iIn;
  // De-interleave received data (and change pInData to point to de-interleaved data)
  for (iOut = 0; iOut < 4; iOut++) {
      uint8_t dataByte = 0;
      for (iIn = 3; iIn >= 0; iIn--)
          dataByte = (dataByte << 2) | ((pInData[iIn] >>( 2 * iOut)) & 0x03);
      aDeintData[iOut] = dataByte;
  }
  pInData = aDeintData;
  
  // Process up to 4 bytes of de-interleaved input data, processing one encoder symbol (2b) at a time
  for (nIterations = 16; nIterations > 0; nIterations--) {
  uint8_t iDestState;
  uint8_t symbol = ((*pInData) >> iBit) & 0x03;
  // Find minimum cost so that we can normalize costs (only last iteration used)
  nMinCost = 0xFF;
  // Get 2b input symbol (MSB first) and do one iteration of Viterbi decoding
  if ((iBit -= 2) < 0) {
  iBit = 6;
  pInData++;
  // Update pointer to the next byte of received data
  }
  // For each destination state in the trellis, calculate hamming costs for both possible paths into state and
  // select the one with lowest cost.
  for (iDestState = 0; iDestState < 8; iDestState++) {
      uint8_t nCost0;
      uint8_t nCost1;
      uint8_t iSrcState0;
      uint8_t iSrcState1;
      uint8_t nInputBit;
      nInputBit = aTrellisTransitionInput[iDestState];
      // Calculate cost of transition from each of the two source states (cost is Hamming difference between
      // received 2b symbol and expected symbol for transition)
      iSrcState0 = aTrellisSourceStateLut[iDestState][0];
      nCost0 = nCost[iLastBuf][iSrcState0];
      nCost0 += hammWeight(symbol ^ aTrellisTransitionOutput[iDestState][0]);
      iSrcState1 = aTrellisSourceStateLut[iDestState][1];
      nCost1 = nCost[iLastBuf][iSrcState1];
      nCost1 += hammWeight(symbol ^ aTrellisTransitionOutput[iDestState][1]);

      // Select transition that gives lowest cost in destination state, copy that source state's path and add
      // new decoded bit
      if (nCost0 <= nCost1) {
          nCost[iCurrBuf][iDestState] = nCost0;
          nMinCost = min(nMinCost, nCost0);
          aPath[iCurrBuf][iDestState] = (aPath[iLastBuf][iSrcState0] << 1) | nInputBit;
      } else {
          nCost[iCurrBuf][iDestState] = nCost1;
          nMinCost = min(nMinCost, nCost1);
          aPath[iCurrBuf][iDestState] = (aPath[iLastBuf][iSrcState1] << 1) | nInputBit;
      }
  }
  nPathBits++;
  // If trellis history is sufficiently long, output a byte of decoded data
  if (nPathBits == 32) {
      *pDecData++ = (aPath[iCurrBuf][0] >> 24) & 0xFF;
      nOutputBytes++;
      nPathBits -= 8;
      nRemBytes--;
  }
  // After having processed 3-symbol trellis terminator, flush out remaining data
  if ((nRemBytes <= 3) && (nPathBits == ((8 * nRemBytes) + 3))) {
      while (nPathBits >= 8) {
      *pDecData++ = (aPath[iCurrBuf][0] >> (nPathBits - 8)) & 0xFF;
      nOutputBytes++;
      nPathBits -= 8;
      }
      return nOutputBytes;
  }
  // Swap current and last buffers for next iteration
  iLastBuf = (iLastBuf + 1) % 2;
  iCurrBuf = (iCurrBuf + 1) % 2;
  }
  // Normalize costs so that minimum cost becomes 0

  uint8_t iState;
  for (iState = 0; iState < 8; iState++)
      nCost[iLastBuf][iState] -= nMinCost;

  return nOutputBytes;
}