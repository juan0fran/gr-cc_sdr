#include <stdio.h>
#include <stdint.h>
#define UINT8   uint8_t
#define UINT16  uint16_t
#define UINT32  uint32_t
#define BYTE    unsigned char
//-------------------------------------------------------------------------------- 
UINT16 culCalcCRC(BYTE crcData, UINT16 crcReg) { 
    UINT8 i; 
for
 (i = 0; i < 8; i++) { 
if
 (((crcReg & 0x8000) >> 8) ^ (crcData & 0x80)) 
            crcReg = (crcReg << 1) ^ 0x8005; 
else 
            crcReg = (crcReg << 1); 
        crcData <<= 1; 
    } 
return
 crcReg; 
}
// culCalcCRC 
//-------------------------------------------------------------------------------- 
// Variables
int main(void){ 
    UINT16 fecEncodeTable[] = { 
        0, 3, 1, 2, 
        3, 0, 2, 1, 
        3, 0, 2, 1, 
        0, 3, 1, 2    
    }; 
    UINT16 input[260];  
    UINT16 i, j, val, fecReg, fecOutput; 
    UINT32 intOutput; 
    UINT16 fec[520]; 
    UINT16 interleaved[520]; 
    UINT16 inputNum = 0, fecNum; 
    UINT16 checksum; 
    UINT16 length; 
    //-------------------------------------------------------------------------------- 
    //Example code 
    length = 20; 
    input[0] = 'H'; 
    input[1] = 'e'; 
    input[2] = 'l'; 
    input[3] = 'l'; 
    input[4] = 'o';
    input[5] = ',';
    input[6] = ' ';
    input[7] = 'W';
    input[8] = 'o';
    input[9] = 'r';
    input[10] = 'l';
    input[11] = 'd';
    input[12] = '!';
    input[13] = ' ';
    input[14] = ' ';
    input[15] = ' ';
    input[16] = ' ';
    input[17] = ' ';
    input[18] = ' ';
    input[19] = ' ';
    inputNum = length;
    printf("Input: [%5d bytes]\n", length); 
    for(i = 0; i < length; i++) 
        printf("%02X%s", input[i], (i % 8 == 7) ? "\n" : (i % 2 == 1) ? "  " : " "); 
    printf("\n\n"); 
    // Generate CRC 
    checksum = 0xFFFF;  
    //Init value for CRC calculation
    for(i = 0; i < length; i++) 
        checksum = culCalcCRC(input[i], checksum); 
    input[inputNum++] = checksum >> 8;     
    // CRC1 
    input[inputNum++] = checksum & 0x00FF; 
    // CRC0 
    printf("Appended CRC: [%5d bytes]\n", inputNum); 
    for (i = 0; i < length + 2; i++) 
      printf("%02X%s", input[i], (i % 8 == 7) ? "\n" : (i % 2 == 1) ? "  " : " "); 
    printf("\n\n"); 
    // Append Trellis Terminator 
    input[inputNum] = 0x0B; 
    input[inputNum + 1] = 0x0B; 
    fecNum = 2*(((length+2) / 2) + 1); 
    printf("Appended Trellis terminator: [%5d bytes]\n", fecNum); 
    for (i = 0; i < fecNum; i++) 
           printf("%02X%s", input[i], (i % 8 == 7) ? "\n" : (i % 2 == 1) ? "  " : " "); 
    printf("\n\n"); 
    // FEC encode 
    fecReg = 0;  
    for (i = 0; i < fecNum; i++) { 
        fecReg = (fecReg & 0x700) | (input[i] & 0xFF); 
        fecOutput = 0; 
    for (j = 0; j < 8; j++) { 
            fecOutput = (fecOutput << 2) | fecEncodeTable[fecReg >> 7]; 
            fecReg = (fecReg << 1) & 0x7FF; 
        } 
        fec[i * 2] = fecOutput >> 8;  
        fec[i * 2 + 1] = fecOutput & 0xFF; 
    }   
    printf("FEC encoder output: [%5d bytes]\n", fecNum * 2); 
    for (i = 0; i < fecNum * 2; i++) 
        printf("%02X%s", fec[i],  (i % 16 == 15) ? "\n" : (i % 4 == 3) ? "  " : " ");     
    printf("\n\n"); 
    // Perform interleaving 
    for
     (i = 0; i < fecNum * 2; i += 4) { 
        intOutput = 0; 
    for (j = 0; j < 4*4; j++) 
            intOutput =  
    (intOutput << 2) | ((fec[i +(~j & 0x03)] >> (2 * ((j & 0x0C) >> 2))) & 0x03);       
        interleaved[i]      = (intOutput >> 24) & 0xFF; 
        interleaved[i + 1]  = (intOutput >> 16) & 0xFF; 
        interleaved[i + 2]  = (intOutput >> 8)  & 0xFF; 
        interleaved[i + 3]  = (intOutput >> 0)  & 0xFF; 
    } 
    printf("Interleaver output: [%5d bytes]\n", fecNum * 2); 
    for
     (i = 0; i < fecNum * 2; i++) 
        printf("%02X%s", interleaved[i],  (i % 16 == 15) ? "\n" : (i % 4 == 3) ? "  " : " ");   
    printf("\n\n");
}
