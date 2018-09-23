#ifndef MY_PARSEFAT_FATTYPES_H
#define MY_PARSEFAT_FATTYPES_H

#include <stdint.h>
#include <stddef.h>
#include "bios_param_block.h"

#define BYTES_PER_SECTOR 512
#define DIR_Name_LENGTH 11
#define BYTES_PER_DIR_ENT 32
#define UNUSED_ENTRY 0xE5
#define END_OF_DIR 0x00
#define LAST_LONG_ENTRY 0x40  // All valid sets of long dir entries must begin with an entry having this mask.


typedef struct DIR_Entry_struct
{
    char DIR_Name[DIR_Name_LENGTH+1];
    uint8_t  DIR_Attr;
    uint16_t DIR_FstClus;
    uint32_t DIR_FileSize;
    char LongFileName[256];
}DIR;


typedef enum DIR_ATTR  // Directory Entry Attribute: verifies type of entry
{
    READ_ONLY = 0x01,
    HIDDEN    = 0x02,
    SYSTEM    = 0x04,
    VOLUME_ID = 0x08,
    DIRECTORY = 0x10,
    ARCHIVE   = 0x20,
    LFN       = READ_ONLY|HIDDEN|SYSTEM|VOLUME_ID
} Attr;


struct DiskInfo
{
    void * disk_entry; // used as pointer to current position in memory of opened file (disk)
    size_t size;       // size of FAT disk, determined by seeking to the end of file
}typedef Disk;


struct FAT_struct
{
    BPB bpb;
    Disk *disk;
}typedef FAT;

// structure used to remember position of directory entry while dive recursively into directories
// to read all the information on the current cluster
struct FAT_ClusterEntry
{
    uint32_t firstCluster;
    uint32_t currCluster;
    uint32_t currSector;
    uint32_t currOffset;
    FAT * fatImg;
}typedef ClustEntry;

#endif //MY_PARSEFAT_FATTYPES_H
