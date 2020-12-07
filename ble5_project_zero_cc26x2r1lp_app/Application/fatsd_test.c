/*
 * fatsd_test.c
 *
 *  Created on: Nov 21, 2020
 *      Author: zahidhaq
 */

#include <file.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDFatFS.h>

/* Example/Board Header files */
#include "Board.h"

#include <util.h>

#include "audiotest.h"
#include "fatsd_test.h"

/* Buffer size used for the file copy process */
#ifndef CPY_BUFF_SIZE
#define CPY_BUFF_SIZE       5
#endif

/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)

/* Drive number used for FatFs */
#define DRIVE_NUM           0

const char inputfileBin[] = "inputBin.bin";
const char outputfileBin[] = "outputBin.bin";

const char textarray_fatfs[] = \
"***********************************************************************\n"
"0         1         2         3         4         5         6         7\n"
"01234567890123456789012345678901234567890123456789012345678901234567890\n"
"This is some text to be inserted into the inputfile if there isn't\n"
"already an existing file located on the media.\n"
"If an inputfile already exists, or if the file was already once\n"
"generated, then the inputfile will NOT be modified.\n"
"***********************************************************************\n";

const uint8_t binarray_fatfs[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
const uint8_t binarray2_fatfs[5] = {0x06, 0x07, 0x08, 0x09, 0x0A};

#define CONFIG_SD_COUNT 1
#define CONFIG_SD_FATFS_COUNT 1

SDFatFS_Object sdfatfsObjects[CONFIG_SD_FATFS_COUNT];

const SDFatFS_Config SDFatFS_config[CONFIG_SD_FATFS_COUNT] = {
    /* CONFIG_SDFatFS_0 */
    {
        .object = &sdfatfsObjects[CONFIG_SDFatFS_0]
    },
};

SDFatFS_Handle sdfatfsHandle;

/* Variables to keep track of the file copy progress */
unsigned int bytesRead = 0;
unsigned int bytesWritten = 0;
unsigned int filesize;
unsigned int totalBytesCopied = 0;

/* Return variables */
int result;

const uint_least8_t SDFatFS_count = CONFIG_SD_FATFS_COUNT;

uint8_t cpy_buff[CPY_BUFF_SIZE];

const char athanBinFile[] = "athanBin.bin";
const char PreFajrBinFile[] = "PreFajr.bin";

// FatFs Static Variables
FIL fil, fil2, filAthan;        /* File object */
char filename[31] = "input.bin";
char filename2[31] = "output.bin";
FRESULT rc;
unsigned int filesize;
unsigned int fileOpen = false;

int FatSD_Read(unsigned short* audioData, unsigned short lenBytes, int sectorIndex)
{
    result = SD_STATUS_SUCCESS;

    /* Try to open the source file */
    if(filAthan.obj.fs == NULL)
    {
    Log_error0("Opening file...\n");
    rc = f_open(&filAthan, athanBinFile, FA_READ); //open for reading (The file must exist)
    if (rc != FR_OK)
    {
        Log_error2("Error opening a file \"%s\" error: %d...", athanBinFile, rc);

        //src = fopen(athanBinFile, "rb+"); //open for reading and writing (The file must exist)
        if (fileOpen == false) {
            Log_error1("Error: \"%s\" could not be created.\n", athanBinFile);
            return SD_STATUS_ERROR;
        }

        Log_info0("Done opening read file\n");
    }
    else
    {
        Log_info1("Using existing copy of \"%s\"\n", athanBinFile);
    }
    }

    if(sectorIndex == STARTINGSECTOR)
    {
        /* Reset the internal file pointer */
        f_lseek(&filAthan, 0);
    }

    /*  Read from source file */
    rc = f_read(&filAthan, audioData, lenBytes, &bytesRead);
    if(rc != FR_OK || bytesRead == 0)
    {
        result = SD_STATUS_ERROR; /* Error or EOF */
    }

    if(bytesRead == 0)
        Log_warning1("Read %d bytes of data", bytesRead);
    else
        Log_info1("Read %d bytes of data", bytesRead);

    return result;
}

int FatSD_Write(unsigned short* audioData, unsigned short lenBytes, int sectorIndex)
{
    result = SD_STATUS_SUCCESS;

    /* Try to open the source file */
    if(filAthan.obj.fs == NULL)
    {
    Log_error0("Opening file...\n");
    rc = f_open(&filAthan, athanBinFile, FA_READ | FA_WRITE | FA_CREATE_ALWAYS); //open for appending (creates file if it doesn't exist)
    if (rc != FR_OK) {
        Log_error2("Error opening a file \"%s\" error: %d...", athanBinFile, rc);

        if (fileOpen == false) {
            Log_error1("Error: \"%s\" could not be created.\n", athanBinFile);
            Log_error0("Aborting...\n");
            while (1);
        }

        Log_info0("done\n");
        return SD_STATUS_ERROR;
    }
    else
    {
        Log_info1("Using existing copy of \"%s\"\n", athanBinFile);
    }
    }

    if(sectorIndex == STARTINGSECTOR)
    {
        /* Reset the internal file pointer */
        f_lseek(&filAthan, 0);
    }

    rc = f_write(&filAthan, audioData, lenBytes, &bytesWritten);
    if (rc != FR_OK) {
        Log_error1("Error: \"%s\" could not be written.\n", filename);
        Log_error0("Aborting...\n");
        while (1);
    }
    f_sync(&filAthan);

    Log_info1("Wrote %d bytes of data", bytesWritten);

    return result;
}

void FatSD_Close(void)
{
    /* Close the file */
    f_close(&filAthan);
    Log_info1("Closed \"%s\" file", athanBinFile);

    fileOpen = false;

    /* Stopping the SDCard */
    //SDFatFS_close(sdfatfsHandle);
    //Log_info1("Drive %u unmounted\n", DRIVE_NUM);
}

void FatSD_Init(void)
{
    /* Call driver init functions */
    SDFatFS_init();

    Log_info0("Starting the fatsd example\n");
    Log_info0("This example requires a FAT filesystem on the SD card.\n");
    Log_info0("You will get errors if your SD card is not formatted with a filesystem.\n");

    /* Mount and register the SD Card */
    sdfatfsHandle = SDFatFS_open(Board_SD0, DRIVE_NUM);
    if (sdfatfsHandle == NULL) {
        Log_error0("Error starting the SD card\n");
        while (1);
    }
    else {
        Log_info1("Drive %u is mounted\n", DRIVE_NUM);
    }

    Log_error0("Opening file...\n");
    uint32_t key;
    rc = f_open(&filAthan, athanBinFile, FA_READ | FA_WRITE | FA_OPEN_APPEND);

    filesize = f_size(&filAthan);
    Log_info1("File size:  %u\n", filesize);
    if ((rc != FR_OK))// || (filesize==0))
    {
        Log_error2("Error opening a file \"%s\" %d...", athanBinFile, rc);

        if (rc != FR_OK) {
            Log_error1("Error: \"%s\" could not be created.\n", athanBinFile);
            Log_error0("Aborting...\n");
            while (1);
        }

        Log_info0("done\n");
    }
    else
    {
        fileOpen = true;
        Log_info0("Opening file SUCCESS!!!...\n");
    }

}

void FatSD_Test(void)
{
    SDFatFS_Handle sdfatfsHandle;

    /* Variables to keep track of the file copy progress */
    unsigned int bytesRead = 0;
    unsigned int bytesWritten = 0;
    unsigned int filesize;
    unsigned int totalBytesCopied = 0;

    /* Return variables */
    int result;

    /* Call driver init functions */
    SDFatFS_init();

    Log_info0("Starting the fatsd example\n");
    Log_info0("This example requires a FAT filesystem on the SD card.\n");
    Log_info0("You will get errors if your SD card is not formatted with a filesystem.\n");

    /* Mount and register the SD Card */
    sdfatfsHandle = SDFatFS_open(Board_SD0, DRIVE_NUM);
    if (sdfatfsHandle == NULL) {
        Log_error0("Error starting the SD card\n");
        while (1);
    }
    else {
        Log_info1("Drive %u is mounted\n", DRIVE_NUM);
    }

    /* Try to open the source file */
    rc = f_open(&fil, filename, FA_READ);

    if ((rc == FR_NO_FILE) || (f_size(&fil)==0))
    {
        Log_info1("Creating a new file \"%s\"...", filename);

        /* Open file for both reading and writing */
        rc = f_open(&fil, filename, FA_READ | FA_WRITE| FA_CREATE_NEW);
        if (rc != FR_OK) {
            Log_error1("Error: \"%s\" could not be created.\n", filename);
            Log_error0("Aborting...\n");
            while (1);
        }

        rc = f_write(&fil, binarray_fatfs, sizeof(binarray_fatfs), &bytesWritten);
        if (rc != FR_OK) {
            Log_error1("Error: \"%s\" could not be written.\n", filename);
            Log_error0("Aborting...\n");
            while (1);
        }
        f_sync(&fil);

        /* Reset the internal file pointer */
        f_lseek(&fil, 0);

        Log_info0("done\n");
    }
    else {
        Log_info1("Using existing copy of \"%s\"\n", inputfileBin);
    }

    /* Create a new file object for the file copy */
    rc = f_open(&fil2, filename2, FA_READ | FA_WRITE | FA_OPEN_APPEND);
    if (rc != FR_OK) {
        Log_error1("Error opening \"%s\"\n", filename2);
        Log_error0("Aborting...\n");
        while (1);
    }
    else {
        Log_info0("Starting file copy\n");
    }

    /*  Copy the contents from the src to the dst */
    while (true) {
        /*  Read from source file */
        rc = f_read(&fil, cpy_buff, CPY_BUFF_SIZE, &bytesRead);
        if (bytesRead == 0) {
            break; /* Error or EOF */
        }

        /*  Write to dst file */
        rc = f_write(&fil2, cpy_buff, sizeof(cpy_buff), &bytesWritten);
        if (bytesWritten < bytesRead) {
            Log_error0("Disk Full\n");
            break; /* Error or Disk Full */
        }

        /*  Write to dst file */
        rc = f_write(&fil2, binarray2_fatfs, sizeof(binarray2_fatfs), &bytesWritten);
        if (bytesWritten < bytesRead) {
            Log_error0("Disk Full\n");
            break; /* Error or Disk Full */
        }

        /*  Update the total number of bytes copied */
        totalBytesCopied += bytesWritten;
    }

    f_sync(&fil);

    /* Get the filesize of the source file */
    filesize = f_size(&fil);
    f_lseek(&fil, 0);

    /* Close both inputfile[] and outputfile[] */
    f_close(&fil);
    f_close(&fil2);

    Log_info4("File \"%s\" (%u B) copied to \"%s\" (Wrote %u B)\n", filename, filesize, filename2, totalBytesCopied);

    /* Now output the outputfile[] contents onto the console */
    //dst = fopen(outputfileBin, "rb");
    rc = f_open(&fil2, filename2, FA_READ);

    if (rc != FR_OK)
    {
        Log_error1("Error opening \"%s\"\n", filename2);
        Log_error0("Aborting...\n");
        while (1);
    }

    /* Print file contents */
    while (true) {
        /* Read from output file */
        rc = f_read(&fil2, cpy_buff, CPY_BUFF_SIZE, &bytesRead);

        if (bytesRead == 0) {
            break; /* Error or EOF */
        }

        Log_info1("Read %d bytes:", bytesRead);

        /* Write output */
        for(int x = 0; x < bytesRead; x++)
            Log_info1("%d", cpy_buff[x]);
    }

    /* Close the file */
    f_close(&fil2);

    /* Stopping the SDCard */
    SDFatFS_close(sdfatfsHandle);
    Log_info1("Drive %u unmounted\n", DRIVE_NUM);
}

/*
 *  ======== fatfs_getFatTime ========
 */
int32_t fatfs_getFatTime(void)
{
    time_t seconds;
    uint32_t fatTime;
    struct tm *pTime;

    /*
     *  TI time() returns seconds elapsed since 1900, while other tools
     *  return seconds from 1970.  However, both TI and GNU localtime()
     *  sets tm tm_year to number of years since 1900.
     */
    seconds = time(NULL);

    pTime = localtime(&seconds);

    /*
     *  localtime() sets pTime->tm_year to number of years
     *  since 1900, so subtract 80 from tm_year to get FAT time
     *  offset from 1980.
     */
    fatTime = ((uint32_t)(pTime->tm_year - 80) << 25) |
        ((uint32_t)(pTime->tm_mon) << 21) |
        ((uint32_t)(pTime->tm_mday) << 16) |
        ((uint32_t)(pTime->tm_hour) << 11) |
        ((uint32_t)(pTime->tm_min) << 5) |
        ((uint32_t)(pTime->tm_sec) >> 1);

    return ((int32_t)fatTime);
}
