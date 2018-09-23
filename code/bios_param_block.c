
#include <stdio.h>
#include "bios_param_block.h"
#include "macros.h"

//Count of bytes per sector. This value may take on
//only the following values: 512, 1024, 2048 or 4096.
static inline uint16_t parseBytsPerSec(const uint8_t *bpbBuf)
{
    return TO_16((bpbBuf + 11));
}

//Number of sectors per allocation unit. This value
//must be a power of 2 that is greater than 0. The
//legal values are 1, 2, 4, 8, 16, 32, 64, and 128.
static inline uint8_t parseSecPerClus(const uint8_t *bpbBuf)
{
    return bpbBuf[13];
}

//Number of reserved sectors in the reserved region
//of the volume starting at the first sector of the
//volume. This field is used to align the start of the
//data area to integral multiples of the cluster size
//with respect to the start of the partition/media.
//This field must not be 0 and can be any non-zero
//value.
//This field should typically be used to align the start
//of the data area (cluster #2) to the desired
//alignment unit, typically cluster size.
static inline uint16_t parseRsvdSecCnt(const uint8_t *bpbBuf)
{
    return TO_16((bpbBuf + 14));
}

//The count of file allocation tables (FATs) on the
//volume. A value of 2 is recommended although a
//value of 1 is acceptable.
static inline uint8_t parseNumFATs(const uint8_t *bpbBuf)
{
    return bpbBuf[16];
}

//This field is the new 32-bit total count of sectors on
//the volume. This count includes the count of all
//sectors in all four regions of the volume.
//This field can be 0; if it is 0, then BPB_TotSec16
//must be non-zero. For FAT12/FAT16 volumes, this
//field contains the sector count if BPB_TotSec16 is
//0 (count is greater than or equal to 0x10000).
//For FAT32 volumes, this field must be non-zero.
static inline uint32_t parseTotSec32(const uint8_t *bpbBuf)
{
    return TO_32((bpbBuf+ 32));
}

//This field is the FAT32 32-bit count of sectors occupied
//by one FAT.
//Note that BPB_FATSz16 must be 0 for media formatted
//FAT32.
static inline uint32_t parseFATSz32(const uint8_t *bpbBuf)
{
    return TO_32((bpbBuf+ 36));
}

//This is set to the cluster number of the first cluster of
//the root directory,
//This value should be 2 or the first usable (not bad)
//cluster available thereafter.
static inline uint32_t parseRootClus(const uint8_t *bpbBuf)
{
    return (uint32_t)TO_32((bpbBuf+ 44));
}


void printBPB(const BPB *bpb)
{
    printf("____BIOS Parameter Block data____\n");
    printf("BPB_BytsPerSec: %u\n",bpb->BPB_BytsPerSec);
    printf("BPB_FATSz32: %u\n",   bpb->BPB_FATSz32);
    printf("BPB_NumFATs: %u\n",   bpb->BPB_NumFATs);
    printf("BPB_RootClus: %u\n",  bpb->BPB_RootClus);
    printf("BPB_RsvdSecCnt: %u\n",bpb->BPB_RsvdSecCnt);
    printf("BPB_SecPerClus: %u\n",bpb->BPB_SecPerClus);
    printf("BPB_TotSec32: %u\n",  bpb->BPB_TotSec32);
    // printf("Signature: %u\n",bpb->Signature);
}


void parseBPB(BPB *bpb, const uint8_t *bootBuf)
{
    bpb->BPB_BytsPerSec = parseBytsPerSec(bootBuf);
    bpb->BPB_FATSz32    = parseFATSz32(bootBuf);
    bpb->BPB_NumFATs    = parseNumFATs(bootBuf);
    bpb->BPB_RootClus   = parseRootClus(bootBuf);
    bpb->BPB_RsvdSecCnt = parseRsvdSecCnt(bootBuf);
    bpb->BPB_SecPerClus = parseSecPerClus(bootBuf);
    bpb->BPB_TotSec32   = parseTotSec32(bootBuf);
    //bpb->Signature      = parseSignature(bootBuf);

    //printBPB_fields(bpb);
}