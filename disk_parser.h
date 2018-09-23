#ifndef MY_PARSEFAT_DISK_PARSER_H
#define MY_PARSEFAT_DISK_PARSER_H

#include <stddef.h>
#include <stdbool.h>
#include "fat_structs.h"

bool loadDisk(Disk * disk, const char * diskName);
bool initFAT (FAT *fat, Disk *disk);
void initNewClustEntry(ClustEntry *clEntry, FAT * fatImg, uint32_t startCluster);
void listDirectories(ClustEntry *clEntry);
void freeResources(Disk *disk);

#endif //MY_PARSEFAT_DISK_PARSER_H
