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

#include <third_party/fatfs/ffcio.h>

#include <ti/drivers/GPIO.h>
#include <ti/drivers/SDFatFS.h>

/* Example/Board Header files */
#include "Board.h"

#include <util.h>

#include "audiotest.h"
#include "fatsd_test.h"

/* Buffer size used for the file copy process */
#ifndef CPY_BUFF_SIZE
#define CPY_BUFF_SIZE       2048
#endif

/* String conversion macro */
#define STR_(n)             #n
#define STR(n)              STR_(n)

/* Drive number used for FatFs */
#define DRIVE_NUM           0

const char inputfile[] = "fat:"STR(DRIVE_NUM)":input.txt";
const char outputfile[] = "fat:"STR(DRIVE_NUM)":output.txt";

const char textarray_fatfs[] = \
"***********************************************************************\n"
"0         1         2         3         4         5         6         7\n"
"01234567890123456789012345678901234567890123456789012345678901234567890\n"
"This is some text to be inserted into the inputfile if there isn't\n"
"already an existing file located on the media.\n"
"If an inputfile already exists, or if the file was already once\n"
"generated, then the inputfile will NOT be modified.\n"
"***********************************************************************\n";

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

/* Variables for the CIO functions */
FILE *src = NULL, *dst = NULL;

/* Variables to keep track of the file copy progress */
unsigned int bytesRead = 0;
unsigned int bytesWritten = 0;
unsigned int filesize;
unsigned int totalBytesCopied = 0;

/* Return variables */
int result;

const uint_least8_t SDFatFS_count = CONFIG_SD_FATFS_COUNT;

/* File name prefix for this filesystem for use with TI C RTS */
char fatfsPrefix[] = "fat";

unsigned char cpy_buff[CPY_BUFF_SIZE];

const char athanBinFile[] = "fat:"STR(DRIVE_NUM)":athanBin.bin";
//const char outputBinFile[] = "fat:"STR(DRIVE_NUM)":outputBin.txt";

int FatSD_Read(unsigned short* audioData, unsigned short lenBytes, int sectorIndex)
{
    result = SD_STATUS_SUCCESS;

    /* Try to open the source file */
    if(!src)
    {
    src = fopen(athanBinFile, "rb"); //open for reading (The file must exist)
    if (!src) {
        Log_error1("Error opening a file \"%s\"...", athanBinFile);

        //src = fopen(athanBinFile, "rb+"); //open for reading and writing (The file must exist)
        if (!src) {
            Log_error1("Error: \"%s\" could not be created.\n", athanBinFile);
            return SD_STATUS_ERROR;
        }

        Log_info0("Done opening read file\n");
    }
    else
    {
        Log_info1("Using existing copy of \"%s\"\n", inputfile);
    }
    }

    if(sectorIndex == STARTINGSECTOR)
    {
        /* Reset the internal file pointer */
        rewind(src);
    }

    /*  Read from source file */
    bytesRead = fread(audioData, 1, lenBytes, src);
    if (bytesRead == 0) {
        return SD_STATUS_ERROR; /* Error or EOF */
    }

    return result;
}

int FatSD_Write(unsigned short* audioData, unsigned short lenBytes, int sectorIndex)
{
    result = SD_STATUS_SUCCESS;

    /* Try to open the source file */
    if(!src)
    {
    src = fopen(athanBinFile, "w+"); //open for appending (creates file if it doesn't exist)
    if (!src) {
        Log_error1("Error opening a file \"%s\"...", athanBinFile);

        if (!src) {
            Log_error1("Error: \"%s\" could not be created.\n", inputfile);
            Log_error0("Aborting...\n");
            while (1);
        }

        Log_info0("done\n");
        return SD_STATUS_ERROR;
    }
    else
    {
        Log_info1("Using existing copy of \"%s\"\n", inputfile);
    }
    }

    if(sectorIndex == STARTINGSECTOR)
    {
        /* Reset the internal file pointer */
        rewind(src);
    }

    bytesWritten = fwrite(audioData, 1, lenBytes, src);
    fflush(src);

    Log_info1("Wrote %d bytes of data", bytesWritten);

    return result;
}

void FatSD_Close(void)
{
    /* Close the file */
    fclose(src);

    /* Stopping the SDCard */
    SDFatFS_close(sdfatfsHandle);
    Log_info1("Drive %u unmounted\n", DRIVE_NUM);
}

void FatSD_Init(void)
{
    /* Call driver init functions */
    SDFatFS_init();

    /* add_device() should be called once and is used for all media types */
    add_device(fatfsPrefix, _MSA, ffcio_open, ffcio_close, ffcio_read,
        ffcio_write, ffcio_lseek, ffcio_unlink, ffcio_rename);

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
    src = fopen(athanBinFile, "w+"); //open for appending (creates file if it doesn't exist)
    if (!src) {
        Log_error1("Error opening a file \"%s\"...", athanBinFile);

        if (!src) {
            Log_error1("Error: \"%s\" could not be created.\n", inputfile);
            Log_error0("Aborting...\n");
            while (1);
        }

        Log_info0("done\n");
    }
    else
    {
        Log_error0("Opening file SUCCESS!!!...\n");
    }
}

void FatSD_Test(void)
{
    SDFatFS_Handle sdfatfsHandle;

    /* Variables for the CIO functions */
    FILE *src, *dst;

    /* Variables to keep track of the file copy progress */
    unsigned int bytesRead = 0;
    unsigned int bytesWritten = 0;
    unsigned int filesize;
    unsigned int totalBytesCopied = 0;

    /* Return variables */
    int result;

    /* Call driver init functions */
    SDFatFS_init();

    /* add_device() should be called once and is used for all media types */
    add_device(fatfsPrefix, _MSA, ffcio_open, ffcio_close, ffcio_read,
        ffcio_write, ffcio_lseek, ffcio_unlink, ffcio_rename);

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
    src = fopen(inputfile, "r");
    if (!src) {
        Log_info1("Creating a new file \"%s\"...", inputfile);

        /* Open file for both reading and writing */
        src = fopen(inputfile, "w+");
        if (!src) {
            Log_error1("Error: \"%s\" could not be created.\n", inputfile);
            Log_error0("Aborting...\n");
            while (1);
        }

        fwrite(textarray_fatfs, 1, strlen(textarray_fatfs), src);
        fflush(src);

        /* Reset the internal file pointer */
        rewind(src);

        Log_info0("done\n");
    }
    else {
        Log_info1("Using existing copy of \"%s\"\n", inputfile);
    }

    /* Create a new file object for the file copy */
    dst = fopen(outputfile, "w");
    if (!dst) {
        Log_error1("Error opening \"%s\"\n", outputfile);
        Log_error0("Aborting...\n");
        while (1);
    }
    else {
        Log_info0("Starting file copy\n");
    }

    /*
     * Optional call to disable internal buffering. This allows FatFs to use
     * multi-block writes to increase throughput if applicable. Note that this
     * may come with a performance penalty for smaller writes.
     */
    result = setvbuf(dst, NULL, _IONBF, 0);
    if (result != 0) {
        Log_error0("Call to setvbuf failed!");
    }

    /*  Copy the contents from the src to the dst */
    while (true) {
        /*  Read from source file */
        bytesRead = fread(cpy_buff, 1, CPY_BUFF_SIZE, src);
        if (bytesRead == 0) {
            break; /* Error or EOF */
        }

        /*  Write to dst file */
        bytesWritten = fwrite(cpy_buff, 1, bytesRead, dst);
        if (bytesWritten < bytesRead) {
            Log_error0("Disk Full\n");
            break; /* Error or Disk Full */
        }

        /*  Update the total number of bytes copied */
        totalBytesCopied += bytesWritten;
    }

    fflush(dst);

    /* Get the filesize of the source file */
    fseek(src, 0, SEEK_END);
    filesize = ftell(src);
    rewind(src);

    /* Close both inputfile[] and outputfile[] */
    fclose(src);
    fclose(dst);

    Log_info4("File \"%s\" (%u B) copied to \"%s\" (Wrote %u B)\n", inputfile, filesize, outputfile, totalBytesCopied);

    /* Now output the outputfile[] contents onto the console */
    dst = fopen(outputfile, "r");
    if (!dst) {
        Log_error1("Error opening \"%s\"\n", outputfile);
        Log_error0("Aborting...\n");
        while (1);
    }

    /* Print file contents */
    while (true) {
        /* Read from output file */
        bytesRead = fread(cpy_buff, 1, CPY_BUFF_SIZE, dst);
        if (bytesRead == 0) {
            break; /* Error or EOF */
        }
        cpy_buff[bytesRead] = '\0';
        /* Write output */
        Log_info1("%s", cpy_buff);
    }

    /* Close the file */
    fclose(dst);

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
