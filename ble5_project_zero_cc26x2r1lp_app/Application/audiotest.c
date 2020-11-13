/*
 *  ======== audiotest.c ========
 */
#include <stdint.h>

/* For usleep() */
//#include <unistd.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Driver Header files */
//#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/SD.h>

#include <ti/drivers/ADC.h>

#include <ti/drivers/timer/GPTimerCC26XX.h>
#include <ti/sysbios/BIOS.h>
#include <icall.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/common/cc26xx/uartlog/UartLog.h>  // Comment out if using xdc Log

#include <ti/display/AnsiColor.h>

/* Driver Header files */
#include <ti/drivers/PWM.h>

#include <project_zero.h>

/* Example/Board Header files */
#include "Board.h"
#include "audiotest.h"
//#include "uartecho.h"
#include <util.h>

/*********************************************************************
 * CONSTANTS
 */
// Task configuration
#define PA_TASK_PRIORITY                     3

#ifndef PA_TASK_STACK_SIZE
#define PA_TASK_STACK_SIZE                   1024
#endif


#define enqueue(x) Log_info0(x)

// Internal Events for RTOS application
#define SP_ADC_EVT                          Event_Id_31 // Event_Id_31
#define SP_QUEUE_EVT                        Event_Id_30 // Event_Id_30
#define SP_SPI_EVT                          Event_Id_29 // Event_Id_30
#define SP_SD_WRITE_EVT                     Event_Id_28 // Event_Id_30
#define SP_SD_READ_EVT                      Event_Id_27 // Event_Id_30

#define SP_BUTTON0_DEBOUNCED_EVT            Event_Id_26 // Event_Id_30
#define SP_BUTTON1_DEBOUNCED_EVT            Event_Id_25

// Bitwise OR of all RTOS events to pend on
#define SP_ALL_EVENTS                        (SP_ADC_EVT             | \
                                              SP_SPI_EVT             | \
                                              SP_SD_WRITE_EVT        | \
                                              SP_SD_READ_EVT         | \
                                              SP_BUTTON0_DEBOUNCED_EVT | \
                                              SP_BUTTON1_DEBOUNCED_EVT | \
                                              SP_QUEUE_EVT)

#define SPI_MSG_LENGTH  (8)
#define MASTER_MSG      ("Hello from master, msg#: ")

#define MAX_LOOP        (10)
#define SAMPLE_SIZE 2
#define SIZE_OF_SECTOR  (512)
#define NUM_SECTOR      (20)
#define ADCBUFFERSIZE    (SIZE_OF_SECTOR*NUM_SECTOR/SAMPLE_SIZE)
#define UARTBUFFERSIZE   (100)

uint16_t sampleBufferHeader[SIZE_OF_SECTOR/SAMPLE_SIZE] = {0};

/* Buffer size used for the file copy process */
#define BUFFSIZE 1024
 /* Starting sector to write/read to */
#define STARTINGSECTOR 0
#define STARTINGSECTOR2 (NUM_SECTOR)

#define BYTESPERKILOBYTE 1024

#define SAMPLE_RATE 8000 //8kHz timer

#define WRITEENABLE 1

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Task configuration
Task_Struct paTask;
#if defined __TI_COMPILER_VERSION__
#pragma DATA_ALIGN(appAzanTaskStack, 8)
#else
#pragma data_alignment=8
#endif
uint8_t appAzanTaskStack[PA_TASK_STACK_SIZE];


unsigned char textarray[BUFFSIZE];

unsigned char cpy_buff[BUFFSIZE];

unsigned char masterRxBuffer[SPI_MSG_LENGTH];
unsigned char masterTxBuffer[SPI_MSG_LENGTH];

// Event globally used to post local events and pend on system and
// local events.
static SyncHandle syncEventAzan = NULL;

static uint32_t counter = 0;
static uint32_t DACoutCnt = 0;

/* Period and duty in microseconds */
uint16_t   pwmPeriod = 3000;
uint16_t   duty = 0;
uint16_t   dutyInc = 100;
uint8_t   ADCinProgress = 0;
uint8_t   OutputDAC = 0;
uint8_t readFromSD = 0;

/* Sleep time in microseconds */
//uint32_t   time = 50000;
PWM_Handle pwm1 = NULL;
PWM_Handle pwm2 = NULL;
PWM_Params params;


GPTimerCC26XX_Handle hTimer;

ADC_Handle   adc;
ADC_Params   paramsADC;
int_fast16_t res;

uint16_t sampleBufferOne[ADCBUFFERSIZE] = {0};
uint16_t sampleBuffer2[ADCBUFFERSIZE] = {0};

uint16_t sampleBufferTwo[10] = {0};
//uint32_t microVoltBuffer[ADCBUFFERSIZE];
uint32_t buffersCompletedCounter = 0;
char uartTxBuffer[UARTBUFFERSIZE];

SPI_Handle      masterSpi;
SPI_Params      spiParams;
SPI_Transaction transaction;
bool            transferOK;
int32_t         status;

uint16_t        DACvalue = 0x0001;
char testString2[] = "\r\nRaw Buffer: ";

int_fast8_t   result;
uint_fast32_t cardCapacity;
uint_fast32_t totalSectors;
uint_fast32_t sectorSize;
uint_fast32_t sectors;
int           i;
SD_Handle     sdHandle = NULL;

uint8_t bufActive = 1;
uint8_t bufActiveWrite = 1;
uint8_t stopRecording = 0;
uint8_t writeDone = 0;
uint8_t eventPosted = 0;

int_fast32_t sdStartWriteSector = 0;
int_fast32_t sdStartReadSector = 0;

#define SD_DATA_TYPE_AZAN 1
#define STRUCTURE_VERSION 1

typedef struct SD_HEADER{
    uint16 structureVersion;
    uint16 type;
    uint16 startingSector;
    uint16 size;
}SD_HEADER;

SD_HEADER sdHeader;

// State of the buttons
//static uint8_t button0State = 0;
//static uint8_t button1State = 0;

PIN_Config savedButtonPinConfig;

static void ProjectAzan_taskFxn(UArg a0,
                                UArg a1);
void closeReadingFromSD(void)
{
    readFromSD = 0;
    OutputDAC = 0;
    PIN_setOutputValue(ledPinHandle, PZ_RLED_PIN, 0);
    PIN_setOutputValue(spiPinHandle, PA_DAC_AMP_PIN, 1);
}

void readFromSDCard(void)
{
    uint32_t keyReadSD;

    if(sdHandle == NULL)
    {
        Log_error0("sdHandle is 0");
        return;
        //Task_exit();
    }

    if(sdStartReadSector == 0)
    {
        Log_error0("sdStartReadSector is 0");
        return;
        //Task_exit();
    }

    if(bufActive == 1)
    {
        /* Calculate number of sectors taken up by the array by rounding up */
        sectors = (sizeof(sampleBufferOne) + sectorSize - 1) / sectorSize;

        if(sectors > 0)
        {
            keyReadSD = HwiP_disable();
            memset(sampleBufferOne, 0, sizeof(sampleBufferOne));
            result = SD_read(sdHandle, sampleBufferOne, sdStartReadSector, sectors);
            HwiP_restore(keyReadSD);
            if (result > SD_STATUS_SUCCESS && sampleBufferOne[0] == 0)
            {
                Log_error1("Error reading from the SD card: %d", result);
                closeReadingFromSD();
                //while (1);
            }
            else
            {
                Log_info2("Read array sector:%d size: %d", sdStartReadSector, sectors);
                sdStartReadSector += sectors;
                DACoutCnt = 0;
                if((sdStartReadSector < sdHeader.size))
                {
                    readFromSD = 1;
                }
                else
                {
                    Log_info0("Done reading from SD!");
                    closeReadingFromSD();
                }
            }
        }
    }
    else if(bufActive == 2)
    {
        /* Calculate number of sectors taken up by the array by rounding up */
        sectors = (sizeof(sampleBuffer2) + sectorSize - 1) / sectorSize;

        if(sectors > 0) //&& sampleBuffer2[0]==0)
        {
            keyReadSD = HwiP_disable();
            memset(sampleBuffer2, 0, sizeof(sampleBuffer2));
            result = SD_read(sdHandle, sampleBuffer2, sdStartReadSector, sectors);
            HwiP_restore(keyReadSD);
            if (result > SD_STATUS_SUCCESS && sampleBuffer2[0] == 0)
            {
                Log_error1("Error reading from the SD card: %d", result);
                closeReadingFromSD();
                //while (1);
            }
            else
            {
                Log_info2("Read array sector:%d size: %d", sdStartReadSector, sectors);
                sdStartReadSector += sectors;
                DACoutCnt = 0;
                if((sdStartReadSector < sdHeader.size))
                {
                    readFromSD = 2;
                }
                else
                {
                    Log_info0("Done reading from SD!\n");
                    closeReadingFromSD();
                }
            }
        }
    }
}

void writeToSDDone(void)
{
    readFromSD = 0;
    stopRecording = 0;
    writeDone = 0;
    bufActiveWrite = 1;
    PIN_setOutputValue(ledPinHandle, Board_PIN_GLED, 0);
}

int_fast8_t writetoSDCard(void)
{
    if(sdHandle==NULL)
    {
        Log_error0("sdHandle is 0");
        return;
    }
    /* Calculate number of sectors taken up by the array by rounding up */
    sectors = (sizeof(sampleBufferOne) + sectorSize - 1) / sectorSize;

#if (WRITEENABLE)

    if(sdStartWriteSector == 0)
    {
        Log_error0("sdStartWriteSector is 0");
        return;
    }

    if(bufActiveWrite == 1)
        result = SD_write(sdHandle, sampleBuffer2, sdStartWriteSector, sectors);
    else if(bufActiveWrite == 2)
        result = SD_write(sdHandle, sampleBufferOne, sdStartWriteSector, sectors);

    if (result != SD_STATUS_SUCCESS)
    {
        Log_error0("Error writing to the SD card");
        writeToSDDone();
        //while (1);
    }
    else
    {
        Log_info2("Wrote array sector:%d size: %d", sdStartWriteSector, sectors);
        sdStartWriteSector += sectors;

        if(stopRecording==1 && writeDone==0)
        {
            ADCinProgress = 0;
            GPTimerCC26XX_stop(hTimer);

            sdHeader.structureVersion = STRUCTURE_VERSION;
            sdHeader.type = SD_DATA_TYPE_AZAN;
            sdHeader.startingSector = 0x01;
            sdHeader.size = sdStartWriteSector - sdHeader.startingSector;

            memset(sampleBufferHeader, 0, sizeof(sampleBufferHeader));
            memcpy(sampleBufferHeader, &sdHeader.structureVersion, sizeof(sdHeader));
            result = SD_write(sdHandle, sampleBufferHeader, STARTINGSECTOR, 1);
            if (result == SD_STATUS_SUCCESS)
            {
                Log_info2("Azan type wrote:%d size:%d", sdHeader.startingSector, sdHeader.size);
            }
            else
            {
                Log_error0("Error writing to the SD card");
                writeToSDDone();
            }

            PIN_setOutputValue(ledPinHandle, Board_PIN_GLED, 0);
        }

        if(stopRecording==0)
        {
            enqueue("write done!");
            writeDone = 1;
        }
        readFromSD = 0;
    }
#endif

    return result;
}

void TurnOffAthanAlarm(void)
{
    OutputDAC = 0;
}

void timerCallback(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask interruptMask) {
    // interrupt callback code goes here. Minimize processing in interrupt.
    //GPIO_toggle(CC26X2R1_LAUNCHXL_GPIO_LED_RED);
    if(ADCinProgress == 1)
    {
        if(counter >= ADCBUFFERSIZE)
        {
            if(bufActiveWrite == 1 && (writeDone==1 || eventPosted==0))
            {
                writeDone = 0;
                bufActiveWrite = 2;
                eventPosted = 0;
                counter = 0;
                PIN_setOutputValue(ledPinHandle, PZ_RLED_PIN, 1);
            }
            else if(bufActiveWrite == 2 && writeDone==1)
            {
                writeDone = 0;
                bufActiveWrite = 1;
                eventPosted = 0;
                counter = 0;
                PIN_setOutputValue(ledPinHandle, PZ_RLED_PIN, 1);
            }
            else
            {
                PIN_setOutputValue(ledPinHandle, PZ_RLED_PIN, 0);
            }
        }
        else if(bufActiveWrite == 1)
        {
            if(sampleBuffer2[0] && eventPosted==0)
            {
                eventPosted = 1;
                Event_post(syncEventAzan, SP_SD_WRITE_EVT);
            }
            ADC_convert(adc, &sampleBufferOne[counter++]);
        }
        else if(bufActiveWrite == 2)
        {
            if(sampleBufferOne[0] && eventPosted==0)
            {
                eventPosted = 1;
                Event_post(syncEventAzan, SP_SD_WRITE_EVT);
            }
            ADC_convert(adc, &sampleBuffer2[counter++]);
        }

    }
    else if (OutputDAC == 1 && readFromSD)
    {
        if(readFromSD == 1)
        {
            DACvalue = sampleBufferOne[DACoutCnt++];
        }
        else if(readFromSD == 2)
        {
            DACvalue = sampleBuffer2[DACoutCnt++];
        }
        else
        {
            DACvalue = 0;
            readFromSD = 3;
        }

        if(DACoutCnt >= ADCBUFFERSIZE)
        {
            DACoutCnt = 0;

            if(bufActive==1)
            {
                bufActive = 2;
                Event_post(syncEventAzan, SP_SD_READ_EVT);
            }
            else if(bufActive==2)
            {
                bufActive = 1;
                Event_post(syncEventAzan, SP_SD_READ_EVT);
            }
            else
            {
                if(readFromSD == 2 && sampleBufferOne[0])
                    readFromSD = 1;
                else
                    readFromSD = 2;
            }
        }

        if(DACvalue > 0)
        {
            masterTxBuffer[0] = (DACvalue & 0xff);
            masterTxBuffer[1] = ((DACvalue >> 8) & 0x3F);
            PIN_setOutputValue(spiPinHandle, PA_DAC_SYNC_PIN, 0);
            transferOK = SPI_transfer(masterSpi, &transaction);
            if(transferOK == false)
                Log_error0("SPI_transfer failed!");
            PIN_setOutputValue(spiPinHandle, PA_DAC_SYNC_PIN, 1);
        }
    }
}

void initTimer(void)
{
    GPTimerCC26XX_Params params;
    GPTimerCC26XX_Params_init(&params);
    params.width          = GPT_CONFIG_16BIT;
    params.mode           = GPT_MODE_PERIODIC_UP;
    params.debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF;
    hTimer = GPTimerCC26XX_open(CC26X2R1_LAUNCHXL_GPTIMER2, &params);
    if(hTimer == NULL) {
        Log_error0("Failed to open GPTimer");
        return;
        //Task_exit();
    }

    Types_FreqHz  freq;
    BIOS_getCpuFreq(&freq);

    Log_info1("CPU freq: %d", freq.lo);

    GPTimerCC26XX_Value loadVal = freq.lo/SAMPLE_RATE - 1;//8kHz;
    GPTimerCC26XX_setLoadValue(hTimer, loadVal);
    GPTimerCC26XX_registerInterrupt(hTimer, timerCallback, GPT_INT_TIMEOUT);
}

void initPWM(void)
{
    /* Call driver init functions. */
    PWM_init();

    PWM_Params_init(&params);
    params.dutyUnits = PWM_DUTY_US;
    params.dutyValue = 0;
    params.periodUnits = PWM_PERIOD_US;
    params.periodValue = pwmPeriod;
    pwm1 = PWM_open(Board_PWM1, &params);
    if (pwm1 == NULL) {
        /* Board_PWM0 did not open */
        while (1);
    }

    PWM_start(pwm1);
}

inline void clearBuf(char* buf, int size)
{
    int i = 0;
    for(i=0; i < size; i++)
        buf[i] = 0;
}

void initADC(void)
{
    ADC_init();
    ADC_Params_init(&paramsADC);
    adc = ADC_open(Board_ADC1, &paramsADC);
    if (adc == NULL) {
        Log_error0("Error initializing ADC channel 1");
        return;
        //Task_exit();
    }else
    {
        enqueue("ADC 1 initialized");
    }
}

void initDACSPI(void)
{
    SPI_init();

    /* Open SPI as master (default) */
    SPI_Params_init(&spiParams);
    spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.transferTimeout = SPI_WAIT_FOREVER;
    spiParams.transferCallbackFxn = NULL;
    spiParams.mode = SPI_MASTER;
    spiParams.bitRate = 2000000; //2Mhz
    spiParams.dataSize = 16;
    spiParams.frameFormat = SPI_POL0_PHA1;

    //SPI1 DAC
    masterSpi = SPI_open(Board_SPI_MASTER, &spiParams);
    if (masterSpi == NULL) {
        Log_error0("SPI not initialized");
        return;
        //Task_exit();
    }
    else
    {
        enqueue("SPI 1 DAC initialized");
    }

    /* Initialize master SPI transaction structure */
    //masterTxBuffer[sizeof(MASTER_MSG) - 1] = (i % 10) + '0';
    memset((void *) masterRxBuffer, 0, SPI_MSG_LENGTH);
    masterTxBuffer[0] = (DACvalue & 0xff);
    masterTxBuffer[1] = ((DACvalue >> 8) & 0x3F);
    masterTxBuffer[2] = 0x00;
    masterTxBuffer[3] = 0x00;
    transaction.count = 1;
    transaction.txBuf = (void *) masterTxBuffer;
    transaction.rxBuf = 0; //(void *) masterRxBuffer;
}

void initSD(void)
{
    SD_init();

    /* Initialize the array to write to the SD card */
    for (i = 0; i < BUFFSIZE; i++) {
        textarray[i] = i & 0xFF;
    }

    /* Mount and register the SD Card */
    sdHandle = SD_open(Board_SD0, NULL);
    if (sdHandle == NULL) {
        Log_error0("Error starting the SD card");
        return;
    }

    result = SD_initialize(sdHandle);
    if (result != SD_STATUS_SUCCESS) {
        Log_error0("Error initializing the SD card");
        return;
    }

    Log_info0("SD initialized!");

    totalSectors = SD_getNumSectors(sdHandle);
    sectorSize = SD_getSectorSize(sdHandle);
    cardCapacity = (totalSectors / BYTESPERKILOBYTE) * sectorSize;

    /* Calculate number of sectors taken up by the array by rounding up */
    sectors = (sizeof(sampleBufferHeader) + sectorSize - 1) / sectorSize;

//#define RESERVED_INFO_SECTOR_SIZE 1
    if(sdStartReadSector != 0)
        Log_error0("sdStartReadSector not zero");

    result = SD_read(sdHandle, sampleBufferHeader, sdStartReadSector, sectors);
    if (result != SD_STATUS_SUCCESS) {
        Log_error0("Error reading from header in SD card");
        return;
        //Task_exit();
    }
    else
    {
        Log_info2("Read header array sector:%d size: %d", sdStartReadSector, sectors);
        memcpy(&sdHeader.structureVersion, sampleBufferHeader, sizeof(sdHeader));
        sdStartReadSector += sectors;
        sdStartWriteSector += sectors;
        if(sdHeader.type == SD_DATA_TYPE_AZAN && sdHeader.structureVersion == STRUCTURE_VERSION && sdHeader.startingSector > 0)
            Log_info2("Azan type found start:%d size:%d", sdHeader.startingSector, sdHeader.size);
        else
            Log_error0("Invalid reading from header in SD card");
    }
}

/*********************************************************************
 * @fn      ProjectZero_createTask
 *
 * @brief   Task creation function for the Project Zero.
 */
void ProjectAzan_Audio_createTask(void)
{
    Task_Params taskParams;

    // Configure task
    Task_Params_init(&taskParams);
    taskParams.stack = appAzanTaskStack;
    taskParams.stackSize = PA_TASK_STACK_SIZE;
    taskParams.priority = PA_TASK_PRIORITY;

    Task_construct(&paTask, ProjectAzan_taskFxn, &taskParams, NULL);
}

void SetRecordAzanEvent(void)
{
    if(syncEventAzan != NULL)
        Event_post(syncEventAzan, SP_BUTTON1_DEBOUNCED_EVT);
}

void SetPlaybackAzanEvent(void)
{
    if(syncEventAzan != NULL)
        Event_post(syncEventAzan, SP_BUTTON0_DEBOUNCED_EVT);
}

/*
 *  ======== mainThread ========
 */
static void ProjectAzan_taskFxn(UArg a0, UArg a1)
{
    syncEventAzan = Event_create(NULL, NULL);

    //initPWM();
    enqueue("GPIO initialized to 0");
    initADC();
    initDACSPI();
    initTimer();
    initSD();
    //initTimerADC();

    /* Loop forever echoing */
    while (1) {

        uint32_t events;
       // int_fast8_t result;

        events = Event_pend(syncEventAzan, Event_Id_NONE, SP_ALL_EVENTS, ICALL_TIMEOUT_FOREVER);
        if (events)
        {
            if (events & SP_BUTTON0_DEBOUNCED_EVT)
            {
                enqueue("SP_BUTTON0_DEBOUNCED_EVT");

                if(OutputDAC == 0)
                {
                    OutputDAC = 1;
                    sdStartReadSector = sdHeader.startingSector;
                    bufActive = 1;
                    Event_post(syncEventAzan, SP_SD_READ_EVT);
                    PIN_setOutputValue(ledPinHandle, PZ_RLED_PIN, 1);
                    PIN_setOutputValue(spiPinHandle, PA_DAC_AMP_PIN, 0);
                    //savedButtonPinConfig = PIN_getConfig(Board_PIN_BUTTON1);
                    //PIN_setConfig(buttonPinHandle, PIN_BM_IRQ, Board_PIN_BUTTON1 | PIN_IRQ_DIS);
                    //PIN_setConfig(buttonPinHandle, PIN_BM_OUTPUT_MODE|PIN_BM_IRQ, Board_PIN_BUTTON1|PIN_GPIO_OUTPUT_EN|PIN_GPIO_LOW|PIN_PUSHPULL|PIN_IRQ_DIS|PIN_DRVSTR_MAX);
                    Log_info1("starting playback at sector:%d", sdStartReadSector);
                    GPTimerCC26XX_start(hTimer);
                }
                else
                {
                    OutputDAC = 0;
                    GPTimerCC26XX_stop(hTimer);
                    PIN_setOutputValue(ledPinHandle, PZ_RLED_PIN, 0);
                    PIN_setOutputValue(spiPinHandle, PA_DAC_AMP_PIN, 1);
                    //PIN_setConfig(buttonPinHandle, PIN_BM_IRQ, buttonId | PIN_IRQ_NEGEDGE);
                    //PIN_setConfig(buttonPinHandle, savedButtonPinConfig, Board_PIN_BUTTON1| savedButtonPinConfig);
                    Log_info1("stopped playback at sector:%d", sdStartReadSector);
                }
            }
            if (events & SP_BUTTON1_DEBOUNCED_EVT)
            {
                enqueue("SP_BUTTON1_DEBOUNCED_EVT");

                if(ADCinProgress == 0)
                {
                    ADCinProgress = 1;
                    stopRecording = 0;
                    sdStartWriteSector = sdHeader.startingSector;
                    PIN_setOutputValue(ledPinHandle, Board_PIN_GLED, 1);
                    Log_info1("starting record at sector:%d", sdStartWriteSector);
                    //savedButtonPinConfig = PIN_getConfig(Board_PIN_BUTTON1);
                    //PIN_setConfig(buttonPinHandle, PIN_BM_OUTPUT_MODE, Board_PIN_BUTTON1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX);
                    GPTimerCC26XX_start(hTimer);
                }
                else
                {
                    Log_info1("stopping record at sector:%d", sdStartWriteSector);
                    //PIN_setConfig(buttonPinHandle, savedButtonPinConfig, Board_PIN_BUTTON1| savedButtonPinConfig);
                    stopRecording = 1;
                }
            }
            // If RTOS queue is not empty, process app message.
            if (events & SP_SD_WRITE_EVT)
            {
                result = writetoSDCard();
            }
            // If RTOS queue is not empty, process app message.
            if (events & SP_SD_READ_EVT)
            {
                readFromSDCard();
            }
            // If RTOS queue is not empty, process app message.
            if (events & SP_SPI_EVT)
            {
                enqueue("Starting SPI transfer");

                transferOK = SPI_transfer(masterSpi, &transaction);
                if (transferOK) {
                    //Display_printf(display, 0, 0, "Master received: %s", masterRxBuffer);
                }
                else {
                    //Display_printf(display, 0, 0, "Unsuccessful master SPI transfer");
                }

                enqueue("End SPI transfer");

                PIN_setOutputValue(spiPinHandle, PA_DAC_SYNC_PIN, 0);
            }

            if (events & SP_ADC_EVT)
            {
                /* Blocking mode conversion */
                /* Start converting. */
                if (ADC_convert(adc, &sampleBufferOne[counter++]) != ADC_STATUS_SUCCESS)
                {
                    /* Did not start conversion process correctly. */
                    while(1);
                }
                else
                {
                    ADCinProgress = 0;
                    PIN_setOutputValue(ledPinHandle, Board_PIN_GLED, 0);
                }
            }

        }

        //sleep(1);
        /* Turn on user LED */
        //GPIO_toggle(CC26X2R1_LAUNCHXL_GPIO_LED_GREEN);
    }
}
