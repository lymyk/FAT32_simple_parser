#include <stdio.h>
#include <string.h>
#include "disk_parser.h"
#include "bios_param_block.h"
#include "macros.h"

/****  private functions declarations ****/
static void parseDirEntry(DIR *dirEntry, const uint8_t *dirBuf);
static uint8_t parseLongFileName(DIR *dir_entry, const uint8_t *dirBuf);
static void listDirContent(ClustEntry *clEntry, uint32_t current_depth);
static uint8_t nextDirEntry(ClustEntry *clEntry, DIR *dirEntry);
static size_t readDisk(const Disk *disk, size_t address, void *data, size_t length);
static bool checkSignature(Disk *disk);

#define MAX_FILE_SIZE 4294967295L
#define BOOT_LENGTH 446
#define PART_LENGTH 16
#define PARTS_NUM 4
#define LAST_BYTES_SIGNATURE 0xAA55

/**____________________________________________________________________**/

// function to open file stream to read disk
// return true if success, false otherwise
bool loadDisk(Disk * disk, const char * diskName)
{
    disk->disk_entry = fopen(diskName, "r");

    if (NULL == disk->disk_entry)
    {
        printf("%s: cannot open disk \"%s\"\n", __func__, diskName);
        return false;
    }

    printf("%s: opened disk \"%s\"\n", __func__, diskName);

    // get size of disk
    fseek(disk->disk_entry, 0, SEEK_END);
    disk->size = (size_t)ftell(disk->disk_entry);

    if (MAX_FILE_SIZE > disk->size)
    {
        if (checkSignature(disk))
        {
            printf("%s:sizeof file: %ld\nSignature is ok!\n", __func__, disk->size);
            return true;
        }
    }
    else
    {
        printf("%s: too large file \"%s\"\n", __func__, diskName);
    }
    fclose(disk->disk_entry);
    return false;
}

// function to start file stream and initialize BIOS parameter block
// return true if success, false otherwise
bool initFAT (FAT *fat, Disk *disk)
{
    uint8_t bpbBuf[BYTES_PER_SECTOR];
    fat->disk = disk;

    if (!readDisk(fat->disk, 0, bpbBuf, BYTES_PER_SECTOR))
    {
        fclose(disk->disk_entry);
        return false;
    }

    parseBPB(&fat->bpb, bpbBuf);
    printBPB(&fat->bpb);
    return true;
}

// function to check if last 2 bytes in BPB equal AA55 (if not - not FAT32 disk format)
// return true if equal, false otherwise
static bool checkSignature(Disk *disk)
{
    bool result = false;
    uint16_t sig;
    if(!readDisk(disk, BOOT_LENGTH + PART_LENGTH*PARTS_NUM, &sig, sizeof(sig)))
        return result;

    if(!(result = (LAST_BYTES_SIGNATURE == sig)))
    {
        printf("Sanity check failed: %04x\nIs not FAT32\n", sig);
    }
    return result;
}


static void parseDirEntry(DIR *dirEntry, const uint8_t *dirBuf)
{
    memcpy(dirEntry->DIR_Name, dirBuf, DIR_Name_LENGTH);
    // add explicitly null terminator to name
    dirEntry->DIR_Name[11] = '\0';
    dirEntry->DIR_Attr = dirBuf[11];
    dirEntry->DIR_FstClus = (uint16_t)(((uint32_t)TO_16((dirBuf + 26))) | ((uint32_t)TO_16((dirBuf + 20)) << 16));
    //dirEntry->DIR_FstClusHI =
    dirEntry->DIR_FileSize = TO_32((dirBuf + 28));
}

void initNewClustEntry(ClustEntry *clEntry, FAT * fatImg, uint32_t startCluster)
{
    clEntry->fatImg = fatImg;
    clEntry->firstCluster = startCluster;
    clEntry->currCluster  = clEntry->firstCluster;
    clEntry->currOffset = 0;
}

static size_t readDisk(const Disk *disk, size_t address, void *data, size_t length)
{
    size_t result = 0;
    if (fseek(disk->disk_entry, address, SEEK_SET != 0))
    {
        printf("error occurred while seeking\n");
        result = 0;
    }

    if ((length != (result = fread(data, 1, length, disk->disk_entry))) && ferror(disk->disk_entry))
    {
        printf("error occurred while reading file\n");
        result = 0;
    }
    return result;
}


static uint32_t getClusterAddress(FAT *fat, uint32_t cluster)
{
    return ((cluster - 2) * fat->bpb.BPB_SecPerClus + fat->bpb.BPB_RsvdSecCnt +
        fat->bpb.BPB_NumFATs * fat->bpb.BPB_FATSz32) * fat->bpb.BPB_BytsPerSec;
}

// function to parse long file name entry
// implemented according to Microsoft Hardware White Paper
// FAT Long Directory Entries
static uint8_t parseLongFileName(DIR *dir_entry, const uint8_t *dirBuf)
{
    //LDIR_Ord - The order of this entry in the sequence of long dir entries
    //associated with the short dir entry at the end of the long dir set.

    //If masked with 0x40 (LAST_LONG_ENTRY), this indicates the entry is the last long dir entry in a set of long dir entries.
    // All valid sets of long dir entries must begin with an entry having this mask.
    uint8_t LDIR_Ord = (uint8_t)((dirBuf[0] & (~LAST_LONG_ENTRY)) - 1);


    //setting up characters 1-5 of the long-name sub-component in this dir entry
    // (LDIR_Name1 - offset: 1, size: 10 (bytes))
    for(int i = 0; i < 5; i++)
    {
        dir_entry->LongFileName[LDIR_Ord * 13 + i] = TO_16((dirBuf + 1 + i * 2));
    }
    // setting up characters 6-11 of the long-name sub-component in this dir entry
    // (LDIR_Name2 - offset: 14, size: 12 (bytes) )
    for(int i = 5; i < 11; i++)
    {
        dir_entry->LongFileName[LDIR_Ord * 13 + i] = TO_16((dirBuf + 14 + (i - 5) * 2));
    }
    // setting up characters 12-13 of the long-name sub-component in this dir entry
    // (LDIR_Name3 - offset: 28, size: 4 (bytes) )
    for(int i = 11; i < 13; i++)
    {
        dir_entry->LongFileName[LDIR_Ord * 13 + i] = TO_16((dirBuf + 28 + (i - 11) * 2));
    }


    //if the last member of the set - set null terminator for 14th character and return
    if(LAST_LONG_ENTRY == (dirBuf[0] & LAST_LONG_ENTRY))
    {
        dir_entry->LongFileName[LDIR_Ord * 13 + 13] = '\0';
    }
    return 0;
}


static uint8_t nextDirEntry(ClustEntry *clEntry, DIR *dirEntry)
{
    uint8_t buf[BYTES_PER_DIR_ENT];
    uint32_t diskOffset = getClusterAddress(clEntry->fatImg, clEntry->currCluster);
    readDisk(clEntry->fatImg->disk, diskOffset + clEntry->currOffset, buf, BYTES_PER_DIR_ENT);
    clEntry->currOffset += BYTES_PER_DIR_ENT;

    if(LFN == (buf[DIR_Name_LENGTH] & LFN))
    {
        uint8_t LDIR_Ord = (uint8_t)(buf[0] & (~LAST_LONG_ENTRY));
        for(int i = LDIR_Ord; i >= 1; i--)
        {
            parseLongFileName(dirEntry, buf);
            readDisk(clEntry->fatImg->disk, diskOffset + clEntry->currOffset, buf, BYTES_PER_DIR_ENT);
            clEntry->currOffset += BYTES_PER_DIR_ENT;
        }
    }

    parseDirEntry(dirEntry, buf);

    return ((UNUSED_ENTRY != dirEntry->DIR_Name[0])  && (END_OF_DIR != dirEntry->DIR_Name[0]));
}

// recursive function listDirContent(ClustEntry *clEntry, uint32_t current_depth)
// iterates through each directory entry
// returns when entry is unused or is end of directory
static void listDirContent(ClustEntry *clEntry, uint32_t current_depth)
{
    DIR directory;
    while (nextDirEntry(clEntry, &directory))
    {
        //check if it is not dot/dotdot entry volume id
        if (('.' == directory.DIR_Name[0]) || (VOLUME_ID == (directory.DIR_Attr & VOLUME_ID)))
            continue;

        for (int i = 0; i < current_depth; i++)
            printf("  ");
        printf("|__%s\n", directory.LongFileName);

        if (DIRECTORY == (directory.DIR_Attr & DIRECTORY))
        {
            directory.DIR_FileSize = 0;
            ClustEntry newEntry;

            initNewClustEntry(&newEntry, clEntry->fatImg,  directory.DIR_FstClus);
            listDirContent(&newEntry,  current_depth + 1);
        }
    }
}

//public function to list directories in current disk
void listDirectories(ClustEntry *clEntry)
{
    listDirContent(clEntry, 0);
}


void freeResources(Disk *disk)
{
    printf("%s: close disk (via fclose)\n", __func__);
    fclose(disk->disk_entry);
}