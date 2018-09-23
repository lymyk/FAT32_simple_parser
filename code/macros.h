#ifndef MY_PARSEFAT_MACROS_H
#define MY_PARSEFAT_MACROS_H

//#define FILE(disk)     ((FILE *) disk)
// all numbers in FAT32 are in the little-endian format!!
// therefore we need to revert data read from it
#define TO_16(x) ((x[0]) | ((x[1])<<8))
#define TO_32(x) ((x[0]) | ((x[1])<<8) | ((x[2])<<16) | ((x[3])<<24))

#endif //MY_PARSEFAT_MACROS_H
