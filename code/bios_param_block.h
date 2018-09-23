#ifndef MY_PARSEFAT_BIOS_PARAM_BLOCK_H
#define MY_PARSEFAT_BIOS_PARAM_BLOCK_H

#include <stdint.h>

// BIOS Parameter Block structure
typedef struct BPB_structure
{
    uint16_t BPB_BytsPerSec;
    uint8_t  BPB_SecPerClus;
    uint16_t BPB_RsvdSecCnt;
    uint8_t  BPB_NumFATs;
    uint32_t BPB_TotSec32;
    uint32_t BPB_FATSz32;
    uint32_t BPB_RootClus;
    //uint16_t Signature; // should be always 0xAA55
}BPB;

void parseBPB(BPB *bpb, const uint8_t *bootBuf);
void printBPB(const BPB *bpb);


#endif //MY_PARSEFAT_BIOS_PARAM_BLOCK_H
