#include <stdio.h>
#include "disk_parser.h"

int main(int argc, char *argv[]) {

    int result = 1;
    Disk diskImg;
    FAT fatImg;
    ClustEntry clEntry;

    if (argc < 2)
    {
        printf("you should pass disk name as parameter!\n");
        return result;
    }

    const char * diskName = argv[1];

    if (loadDisk(&diskImg, diskName))
    {
        if (initFAT(&fatImg, &diskImg))
        {
            //initialize first entry to start read data from root directory
            initNewClustEntry(&clEntry, &fatImg, fatImg.bpb.BPB_RootClus);
            listDirectories(&clEntry);
            freeResources(&diskImg);
            result = 0;
        }
        else
        {
            printf("could not read the disk.\nEXIT\n");
        }
    }
    else
    {
        printf("error occurred while processing disk data\nEXIT\n");
    }

    return result;
}



