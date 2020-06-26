/******************************************************************************

 @file  bim_main.c

 @brief This module contains the definitions for the main functionality of a
        Boot  Image Manager.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2012-2018, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 Release Name: simplelink_cc26x2_sdk_2_20_00_36
 Release Date: 2018-06-27 10:07:01
 *****************************************************************************/

/*******************************************************************************
 *                                          Includes
 */
#include <stdint.h>
#include <string.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/watchdog.h)
#include DeviceFamily_constructPath(inc/hw_prcm.h)

#include "common/cc26xx/oad/ext_flash_layout.h"
#include "common/cc26xx/crc/crc32.h"
#include "common/cc26xx/board_support_pkg/CC26X2R1_CC13X2R1_LAUNCHXL/ext_flash.h"
#include "common/cc26xx/flash_interface/flash_interface.h"
#include "common/cc26xx/bim/bim_util.h"
#include "common/cc26xx/oad/oad_image_header.h"
#include "common/cc26xx/oad/oad_image_header.h"
#ifdef __IAR_SYSTEMS_ICC__
#include <intrinsics.h>
#endif

#ifdef LED_DEBUG
#include DeviceFamily_constructPath(driverlib/gpio.h)
#include "common/cc26xx/board_support_pkg/CC26X2R1_CC13X2R1_LAUNCHXL/bsp.h"
#include "common/cc26xx/debug/led_debug.h"
#endif

/*******************************************************************************
 *                                          Constants
 */

#define IMG_HDR_FOUND                  -1
#define EMPTY_METADATA                 -2

#define SUCCESS                         0
#define FAIL                           -1

#define ONCHIP_COPY_CHUNK_SIZE          256            /* Max number of bytes to copy on on-chip flash */

#ifdef CREATE_FACT_IMG_INT_TO_EXT_FLSH
#define INTFLASH_APP_ADDR               0              /* Start address of application on on-chip flash */
#endif

/*******************************************************************************
 * LOCAL VARIABLES
 */
static uint32_t intFlashPageSize;                   /* Size of internal flash page */

/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static uint32_t Bim_findImageStartAddr(uint32_t efImgAddr, uint32_t imgLen);
static uint8_t Bim_EraseOnchipFlashPages(uint8_t startAddr, uint32_t imgLen, uint32_t pageSize);

/*******************************************************************************
 * @fn     Bim_copyImage
 *
 * @brief  Copies firmware image into the executable flash area.
 *
 * @param  imgStart - starting address of image in external flash.
 * @param  imgLen   - size of image in 4 byte blocks.
 * @param  dstAddr  - destination address within internal flash.
 *
 * @return Zero/SUCCESS when successful. FAIL, otherwise.
 */
static int8_t Bim_copyImage(uint32_t imgStart, uint32_t imgLen, uint32_t dstAddr)
{
    uint_fast16_t page = dstAddr/intFlashPageSize;
    uint_fast16_t lastPage;

    lastPage = (uint_fast16_t) ((dstAddr + imgLen - 1) / intFlashPageSize);

    if(dstAddr & 3)
    {
        /* Not an aligned address */
        return(FAIL);
    }

    if(page > lastPage || lastPage > MAX_ONCHIP_FLASH_PAGES)
    {
        return(FAIL);
    }

    /* Erase the pages needed to be copied */
    if(Bim_EraseOnchipFlashPages(dstAddr, imgLen, intFlashPageSize) != SUCCESS)
    {
        return(FAIL);
    }

    uint8_t buf[ONCHIP_COPY_CHUNK_SIZE];
    uint16_t byteCnt = ONCHIP_COPY_CHUNK_SIZE;

    while (imgLen > 0)
    {
        if(imgLen < byteCnt)
        {
            byteCnt = imgLen;
        }

        /* Read word from external flash */
        if(!extFlashRead(imgStart, byteCnt, (uint8_t *)&buf))
        {
            /* read failed */
            return(FAIL);
        }

        /* Write word to internal flash */
        if(writeFlash(dstAddr, buf, byteCnt) != FLASH_SUCCESS)
        {
            /* Program failed */
            return(FAIL);
        }

        imgStart += byteCnt;
        dstAddr += byteCnt;
        imgLen -= byteCnt;
    }
    /* Do not close external flash driver here just return */
    return(SUCCESS);
}

#ifdef CREATE_FACT_IMG_INT_TO_EXT_FLSH
/*******************************************************************************
 * @fn     Bim_copyFactoryImage
 *
 * @brief  Copies the internal application + stack image to the external flash
 *         to serve as the permanant resident factory image in external flash.
 *
 * @param  imgStart - starting address of image in external flash.
 * @param  imgLen   - size of image in 4 byte blocks
 * @param  dstAddr  - destination address within internal flash.
 *
 * @return true     - on success else
 *         false
 */
static bool Bim_copyFactoryImage(uint32_t imgStart, uint32_t imgLen, uint32_t dstAddr)
{
    uint32_t *srcAddr;
    uint32_t writeBuf;
    uint32_t tempReadBuf;

    bool rtn = true;
    uint32_t dstAddrStart = dstAddr;

    /* initialize external flash driver */
    if(extFlashOpen() != 0)
    {
        /* Erase - external portion to be written*/
        if(extFlashErase(dstAddr, imgLen))
        {
            /* COPY - image from internal to external */
            if(extFlashWrite(dstAddr, imgLen, (uint8_t *)imgStart))
            {
                /* VERIFY - if image copied correctly */
                for(srcAddr = (uint32_t *)imgStart, dstAddr = dstAddrStart;
                        ((uint32_t)srcAddr) < imgStart + imgLen;
                         srcAddr++, dstAddr+=4)
                {
                    /* Read internal flash */
                    writeBuf = *((uint32_t*)srcAddr);

                    /* clear read buffer */
                    tempReadBuf = 0;

                    /* read word from external flash */
                    if(extFlashRead(dstAddr, 4, (uint8_t *)&tempReadBuf) == false)
                    {
                        /* Program failed */
                        rtn = false;
                        break;
                    }

                    /* verify word in external flash */
                    if(writeBuf != tempReadBuf)
                    {
                        /* Program failed */
                        rtn = false;
                        break;
                    }

                }
                if(rtn != false)
                {
                    ExtImageInfo_t metadataHdr = { .fixedHdr.imgID = OAD_EXTFL_ID_VAL }; /* Write OAD flash metadata identification */

                    /* Copy Image header from internal flash image, skip magic */
                    CRC32_memCpy((uint8_t *)&metadataHdr.fixedHdr.crc32, ((uint8_t *)imgStart + 8) , (EFL_METADATA_LEN - CRC_OFFSET));

                    /*
                     * Calculate the CRC32 value and update that in image header as CRC32
                     * wouldn't be available for running image.
                     */
                    uint32_t startAddr = 0; /* For App, app +stack library, App+stack merge */
                    if(metadataHdr.fixedHdr.crc32 == INVALID_ADDR)
                    {
                        metadataHdr.fixedHdr.crc32 = CRC32_calc(FLASH_PAGE(startAddr), intFlashPageSize, 0, imgLen, false);
                    }

                    /* Update CRC status */
                    metadataHdr.fixedHdr.crcStat = CRC_VALID;

                    /* update external flash storage address */
                    metadataHdr.extFlAddr = dstAddrStart;

                    /* Allow application or some other place in BIM to mark factory image as
                    pending copy (OAD_IMG_COPY_PEND). Should not be done here, as
                    what is in flash at this time will already be the factory
                    image. */
                    metadataHdr.fixedHdr.imgCpStat = COPY_DONE;

                    /* Update length as we might have word aligned */
                    metadataHdr.fixedHdr.len = imgLen;

                    /* WRITE METADATA */
                    /* Erase - external portion to be written*/
                    if(extFlashErase(EFL_ADDR_META_FACT_IMG, sizeof(ExtImageInfo_t)))
                    {
                        extFlashWrite(EFL_ADDR_META_FACT_IMG, sizeof(ExtImageInfo_t),
                                      (uint8_t *)&metadataHdr);

                    }
                    else
                    {
                        rtn = false;
                    }
                }
            }
            else
            {
                rtn = false;
            }
        }
        else
        {
            rtn = false;
        }

        /* close driver */
        extFlashClose();
    }
    else
    {
        rtn = false;
    }

#ifdef LED_DEBUG
    /* For LED feedback */
    powerUpGpio();

    /* If the copy of the internal image to external flash failed,
       turn on RED LED indefinitely */
    if(rtn != true)
    {
        /* Turn on RED LED */
        GPIO_setDio(IOID_6);
    }
    else
    {
        /* No Error, Blink Green Led */
        blinkLed(IOID_5, 10, 50);

        /* Turn on GREEN LED */
        GPIO_setDio(IOID_5);
    }
#endif /* #ifdef LED_DEBUG */
    return(rtn);
}

/*******************************************************************************
 * @fn     Bim_createFactoryImageBackup
 *
 * @brief  Copies the internal application + stack image to the external flash
 *         to serve as the permanant resident factory image in external flash.
 *
 * @param  imgStart - starting address of image in external flash.
 *
 * @return true     - on successful creation else
 *         false
 */
static bool Bim_createFactoryImageBackup(uint32_t imgStart)
{
    imgFixedHdr_t imgHdr;
    uint32_t imgLen = 0;
    uint32_t dstAddr = INVALID_ADDR;

    extFlashOpen();
    const ExtFlashInfo_t *pExtFlashInfo = extFlashInfo();
    extFlashClose();

    /* Read on-chip flash to read the current application image header to the image end address
     * to calculate the length of current application.
     */
    readFlash(0, (uint8_t *)&imgHdr, OAD_IMG_HDR_LEN);

    imgLen = imgHdr.imgEndAddr - imgStart + 1;

    /* Change image size into word unit */
    if(imgLen & 0x3)
    {
        imgLen = (imgLen - (imgLen & 0x3)) + 4;
    }

    if(imgLen & EXTFLASH_PAGE_MASK)
    {
        dstAddr = pExtFlashInfo->deviceSize - (EXT_FLASH_PAGE(imgLen) + 1)*EFL_PAGE_SIZE;
    }
    else
    {
        dstAddr = pExtFlashInfo->deviceSize - EXT_FLASH_PAGE(imgLen)*EFL_PAGE_SIZE;
    }

    /* Flash write work around
     *
     */
    return Bim_copyFactoryImage(imgStart, imgLen, dstAddr);
}
#endif /* CREATE_FACT_IMG_INT_TO_EXT_FLSH */

#ifdef TEST_IMG_COPY
/*******************************************************************************
 * @fn     testImgCopy
 *
 * @brief  This is a debug function to test the image copy from external flash
 *         to onchip flash and CRC32 validation and execution of copied image.
 *
 * @return status - True -if test pass otherwise
 *                  False - if test fail
 */
static bool testImgCopy(void)
{
    ExtImageInfo_t metadataHdr;

    /* read metadata a mark it to be copied and copy image and jump to copied
     application */
    bool rtn = true;

    /* initialize external flash driver */
    if(!extFlashOpen())
    {
        return(false);
    }

    extFlashRead(EFL_ADDR_META_FACT_IMG_BLE,  EFL_METADATA_LEN, (uint8_t *)&metadataHdr);

    /* mark header to copy needed */
    metadataHdr.fixedHdr.imgCpStat = NEED_COPY;

    /* Update METADATA */

    /* Erase - external portion to be written*/
    if(extFlashErase(EFL_ADDR_META_FACT_IMG_BLE, EFL_METADATA_LEN))
    {
        rtn = extFlashWrite(EFL_ADDR_META_FACT_IMG_BLE, EFL_METADATA_LEN, (uint8_t *)&metadataHdr);
    }

    extFlashClose();
    return(rtn);
}
#endif /* TEST_IMG_COPY */

/*******************************************************************************
 * @fn     isLastMetaData
 *
 * @brief  Copies the internal application + stack image to the external flash
 *         to serve as the permanent resident image in external flash.
 *
 * @param  flashPageNum - Flash page number to start search for external flash
 *         metadata
 *
 * @return flashPageNum - Valid flash page number if metadata is found.
 *         IMG_HDR_FOUND - if metadat starting form specified flash page not found
 *         EMPTY_METADATA - if it is empty flashif metadata not found.
 */
static int8_t isLastMetaData(uint8_t flashPageNum)
{
    /* Read flash to find OAD image identification value */
    imgFixedHdr_t imgHdr;
    uint8_t readNext = true;
    do
    {
        extFlashRead(EXT_FLASH_ADDRESS(flashPageNum, 0),  OAD_IMG_ID_LEN, (uint8_t *)&imgHdr.imgID[0]);

        /* Check imageID bytes */
        if(metadataIDCheck(&imgHdr) == true)  /* External flash metadata found */
        {
            return(flashPageNum);
        }
        else if(imgIDCheck(&imgHdr) == true)   /* Image metadata found which indicate end of external flash metadata pages*/
        {
           return(IMG_HDR_FOUND);
        }
        flashPageNum +=1;
        if(flashPageNum >= MAX_OFFCHIP_METADATA_PAGES) /* End of flash reached, Note:practically it would never go till end if there a factory image exits */
        {
            readNext = false;
        }

    }while(readNext);

    return(EMPTY_METADATA);
}

/*******************************************************************************
 * @fn     checkImagesExtFlash
 *
 * @brief  Checks for stored images on external flash. If valid image is found
 * to be copied, it copies the image and if the image is executable it will jump
 * to execute.
 *
 * @param  None
 *
 * @return No return if image is found to be copied and copied successfully else
 *         FAIL - If flash open fails or no image found on external flash
 *             number if metadata is found else
 *         SUCCESS - if no valid external flash metadata not found indicating the image to be copied.
 */
static int8_t checkImagesExtFlash(void)
{
    ExtImageInfo_t metadataHdr;

    int8_t flashPageNum = 0;

    /* Initialize external flash driver. */
    if(!extFlashOpen())
    {
        return(FAIL);
    }

    /* Read flash to find OAD external flash metadata identification value  and check for external
      flash bytes */
    while((flashPageNum = isLastMetaData(flashPageNum)) > -1)
    {
        /* Read whole metadata header */
        extFlashRead(EXT_FLASH_ADDRESS(flashPageNum, 0), EFL_METADATA_LEN, (uint8_t *)&metadataHdr);

        /* check BIM and Metadata version */
        if((metadataHdr.fixedHdr.imgCpStat != NEED_COPY) ||
           (metadataHdr.fixedHdr.bimVer != BIM_VER  || metadataHdr.fixedHdr.metaVer != META_VER) ||
           (metadataHdr.fixedHdr.crcStat == CRC_INVALID))  /* Invalid CRC */
        {
            flashPageNum += 1; /* increment flash page number */
            continue;          /* Continue search on next flash page */
        }

        /* check image CRC status */
        if(metadataHdr.fixedHdr.crcStat == 0xFF) /* CRC not calculated */
        {
            /*
             * Calculate the CRC over the data buffer and update status
             * @T.B.D. Skipping this section, BIM shouldn't calculate the CRC,
             * the application should calculate and update it.
             */
#ifdef BIM_CALC_CRC
            uint32_t crc32 = 0;
            uint8_t  crcstat = CRC_VALID;
            uint32_t startAddr = Bim_findImageStartAddr(EXT_FLASH_ADDRESS(flashPageNum, 0), (uint32_t)metadataHdr.fixedHdr.len);
            crc32 = CRC32_calc(startAddr, 0, false);

            /* Check if calculated CRC matched with the image header */
            if(crc32 != metadataHdr.fixedHdr.crc32)
            {
                /* Update CRC status */
                crcstat = CRC_INVALID;
                extFlashWrite(EXT_FLASH_ADDRESS(flashPageNum, CRC_STAT_OFFSET), 1, (uint8_t *)&crcstat);

                /* Continue looking for next metadata */
                flashPageNum += 1; /* increment flash page number */
                continue;          /* Continue serach on next flash page */
            }

            /*
             * Image don't need to be copied, and CRC matched else this would upset the
             * crc calculation of the image to be copied
             */
            if(metadataHdr.fixedHdr.imgCpStat != NEED_COPY)
            {
                /* Update CRC status */
                extFlashWrite(EXT_FLASH_ADDRESS(flashPageNum, CRC_STAT_OFFSET), 1, (uint8_t *)&crcstat);
            }
#endif /* BIM_CALC_CRC */
        } /* if(metadataHdr.fixedHdr.crcStat == 0xFF) */

        /*
         * We get here, we must have valid CRC
         * Check image copy status,  On CC26XXR2 platform only stack application
         * needes to be copied.
         */
        if(metadataHdr.fixedHdr.imgCpStat == NEED_COPY)
        {
            /* On CC26XXR2 platform only stack application needes to be copied
               Do the image copy */
            uint32_t eFlStrAddr = metadataHdr.extFlAddr;
            uint8_t status = COPY_DONE;

           /* Now read image's start address from image stored on external flash */
           imgFixedHdr_t imgHdr;

           /* Read whole image header, to find the image start address */
           extFlashRead(eFlStrAddr, OAD_IMG_HDR_LEN, (uint8_t *)&imgHdr);

           /* Copy image on internal flash */

           /* Get image start address from image */
           uint32_t startAddr = INVALID_ADDR;
           startAddr = Bim_findImageStartAddr(eFlStrAddr, (uint32_t)imgHdr.len);
           if(startAddr == INVALID_ADDR)
           {
               return FAIL;
           }
           /*
            * NOTE:@TBD during debugging image length wouldn't available, as it is updated
            * by python script during post build process, for now this implementation
            * catering only for contiguous image's, so image length will be calculate
            * by subtracting the image end adress by image start address.
            */
#ifdef JTAG_DEBUG
            imgHdr.len = imgHdr.imgEndAddr - startAddr + 1;
#endif

            uint8_t retVal = Bim_copyImage(eFlStrAddr, imgHdr.len, startAddr);
            extFlashWrite(EXT_FLASH_ADDRESS(flashPageNum, IMG_COPY_STAT_OFFSET), 1, (uint8_t *)&status);

            /* If image copy is successful */
            if(retVal == SUCCESS)
            {
                /* update image copy status and calculate the
                the CRC of the copied
                and update it's CRC status. CRC_STAT_OFFSET
                */

                uint32_t crc32 = CRC32_calc(FLASH_PAGE(startAddr), intFlashPageSize, 0, imgHdr.len, false);
                status = CRC_VALID;
                if(crc32 == imgHdr.crc32) // if crc matched then update its status in the copied image
                {
                    extFlashWrite(FLASH_ADDRESS(flashPageNum, CRC_STAT_OFFSET),
                                    sizeof(status), &status);

                    writeFlashPg(FLASH_PAGE(startAddr), CRC_STAT_OFFSET,
                                    &status, sizeof(status));

                    /* If it is executable Image jump to execute it */
                    if((imgHdr.imgType == OAD_IMG_TYPE_APP) ||
                        (imgHdr.imgType == OAD_IMG_TYPE_PERSISTENT_APP) ||
                        (imgHdr.imgType == OAD_IMG_TYPE_APP_STACK) ||
                        (imgHdr.imgType == OAD_IMG_TYPE_APPSTACKLIB))
                    {
                        jumpToPrgEntry(imgHdr.prgEntry);
                    }
                }
                else
                {
                    status = CRC_INVALID;
                    extFlashWrite((startAddr + CRC_STAT_OFFSET), 1, &status);
                }
                status = COPY_DONE;
                extFlashWrite((startAddr + IMG_COPY_STAT_OFFSET), 1, (uint8_t *)&status);
            } /* if(retVal == SUCCESS) */
        }

        /* Unable to find valid executable image, try to find valid user application */
        flashPageNum += 1;

    }  /* end of  while((retVal = isLastMetaData(flashPageNum)) > -1) */

    extFlashClose();
    return(SUCCESS);
}

/*******************************************************************************
 * @fn     Bim_findImageStartAddr
 *
 * @brief  Checks for stored images on the on-chip flash. If valid image is
 * found to be copied, it executable it.
 *
 * @param  efImgAddr - image start address on external flash
 * @param  imgLen    - length of the image
 *
 * @return - start address of the image if image payload segment is found else
 *          INVALID_ADDR
 */
static uint32_t Bim_findImageStartAddr(uint32_t efImgAddr, uint32_t imgLen)
{
    uint8_t buff[12];
    uint32_t startAddr = INVALID_ADDR;
    uint32_t offset = OAD_IMG_HDR_LEN;
    uint32_t len = SEG_HDR_LEN;
    uint32_t flashAddrBase = efImgAddr;
    do
    {
        /* Read flash to find segment header of the first segment */
        if(extFlashRead((flashAddrBase + offset), len, &buff[0]))
        {
            /* Check imageID bytes */
            if(buff[0] == IMG_PAYLOAD_SEG_ID)
            {
                startAddr = *((uint32_t *)(&buff[8]));
                break;
            }
            else /* some segment other than image payload */
            {
                /* get segment payload length and keep iterating */
                uint32_t payloadlen = *((uint32_t *)&buff[4]);
                if(payloadlen == 0) /* We have problem here break to avoid spinning in loop */
                {
                    break;
                }
                else
                {
                    offset += payloadlen;
                }
            }
        }
        else
        {
            break;
        }
    }while(offset < imgLen);

    return startAddr;
}

/*******************************************************************************
 * @fn     checkImagesIntFlash
 *
 * @brief  Checks for stored images on the on-chip flash. If valid image is
 * found to be copied, it executable it.
 *
 * @param  flashPageNum - Flash page number to start searching for imageType.
 * @param  imgType - Image type to look for.
 *
 * @return - No return if image is found else
 *           0 - If indended image is not found.
 */
static uint8_t checkImagesIntFlash(uint8_t flashPageNum)
{
    imgFixedHdr_t imgHdr;

    do
    {
        /* Read flash to find OAD image identification value */
        readFlash((uint32_t)FLASH_ADDRESS(flashPageNum, 0), &imgHdr.imgID[0], OAD_IMG_ID_LEN);

        /* Check imageID bytes */
        if(imgIDCheck(&imgHdr) == true)
        {
            /* Read whole image header */
            readFlash((uint32_t)FLASH_ADDRESS(flashPageNum, 0), (uint8_t *)&imgHdr, OAD_IMG_HDR_LEN);

            /* If application is neither executable user application or merged app_stack */
            if(!(OAD_IMG_TYPE_APP == imgHdr.imgType ||
                 OAD_IMG_TYPE_APP_STACK == imgHdr.imgType  ||
                 OAD_IMG_TYPE_APPSTACKLIB == imgHdr.imgType))
            {
                continue;
            }
            /* Invalid metadata version */
            if((imgHdr.bimVer != BIM_VER  || imgHdr.metaVer != META_VER) ||
               /* Invalid CRC */
               (imgHdr.crcStat == CRC_INVALID))
            {
                continue;
            }
            else if(imgHdr.crcStat == DEFAULT_STATE) /* CRC not calculated */
            {
#ifndef JTAG_DEBUG  /* If JTAG debug, skip the crc checking and updating the crc
                     as crc wouldn't have been calculated status */
                uint8_t  crcstat = CRC_VALID;

                /* Calculate the CRC over the data buffer and update status */
                uint32_t crc32 = CRC32_calc(flashPageNum, intFlashPageSize, 0, imgHdr.len, false);

                /* Check if calculated CRC matched with the image header */
                if(crc32 != imgHdr.crc32)
                {
                    /* Update CRC status */
                    crcstat = CRC_INVALID;
                }

                writeFlash((uint32_t)FLASH_ADDRESS(flashPageNum, CRC_STAT_OFFSET), (uint8_t *)&crcstat, 1);
                if(crc32 == imgHdr.crc32)
                {
#endif
                    jumpToPrgEntry(imgHdr.prgEntry);  /* No return from here */
#ifndef JTAG_DEBUG
                }
#endif
                /* If it reached here nothing can be done other than try to find
                   good another image which wouldn't be found since it external
                   flash OAD */
            }
            else if(imgHdr.crcStat == CRC_VALID)
            {
                jumpToPrgEntry(imgHdr.prgEntry);  /* No return from here */
            }

        } /* if (imgIDCheck(&imgHdr) == true) */

    } while(flashPageNum++ < (MAX_ONCHIP_FLASH_PAGES -1));  /* last flash page contains CCFG */
    return(0);
}

/*******************************************************************************
 * @fn     Bim_revertFactoryImage
 *
 * @brief  It copies the factory image from external flash to on-chip and executes.
 *
 * @param  None.
 *
 * @return - No return if image is copied succsfully else
 *           false
 */
static bool Bim_revertFactoryImage(void)
{
    ExtImageInfo_t metadataHdr;

    /* Initialize external flash driver. */
    if(!extFlashOpen())
    {
        return(false);
    }
    // Read First metadata page for getting factory image information
    extFlashRead(EFL_ADDR_META_FACT_IMG, EFL_METADATA_LEN, (uint8_t *)&metadataHdr);

    /* check BIM and Metadata version */
    if(metadataHdr.fixedHdr.crcStat != CRC_VALID)  /* Invalid CRC */
    {
        return false;
    }

    uint32_t eFlStrAddr = metadataHdr.extFlAddr;
    uint32_t startAddr = Bim_findImageStartAddr(eFlStrAddr, metadataHdr.fixedHdr.len);

    if(Bim_copyImage(eFlStrAddr, metadataHdr.fixedHdr.len, startAddr) == SUCCESS)
    {

        // Calculate the CRC of the copied on-chip image to make sure copy is successful
        uint32_t crc32 = CRC32_calc(FLASH_PAGE(startAddr), intFlashPageSize, 0, metadataHdr.fixedHdr.len, false);
        uint8_t status = CRC_INVALID;
        if(crc32 == metadataHdr.fixedHdr.crc32) // if crc matched then update its status in the copied image
        {
            status = CRC_VALID;

            /* Update the CRC status of the copied image at CRC_STAT_OFFSET */
            writeFlashPg(FLASH_PAGE(startAddr), CRC_STAT_OFFSET, &status, sizeof(status));

            /* Jump to program entry to execute it */
            jumpToPrgEntry(metadataHdr.fixedHdr.prgEntry);
        }
    }
    extFlashClose();
    return false;
}

/*******************************************************************************
 * @fn     Bim_EraseOnchipFlashPages
 *
 * @brief  It Erases the onchip flash pages.
 *
 * @param  startAddr - Image start address on on-chip flash
 * @param  imgLen    - image length
 * @param  pageSize  - flash page size
 *
 * @return - SUCCESS on successful erasure else
 *           FAIL
 */
static uint8_t Bim_EraseOnchipFlashPages(uint8_t startAddr, uint32_t imgLen, uint32_t pageSize)
{
    int8_t status = SUCCESS;

    uint8_t startPage = startAddr/pageSize;
    uint8_t numFlashPages = imgLen/pageSize;
    if(0 != (imgLen % pageSize))
    {
        numFlashPages += 1;
    }

    // Erase the correct amount of pages
    for(uint8_t page=startPage; page<(startPage + numFlashPages); ++page)
    {
        uint8_t flashStat = eraseFlashPg(page);
        if(flashStat == FLASH_FAILURE)
        {
            // If we fail to pre-erase, then halt the OAD process
            status = FAIL;
            break;
        }
    }
    return status;
}

/*******************************************************************************
 * @fn     Bim_checkImages
 *
 * @brief  Check for stored images on external flash needed to be copied and
 *         execute. If there is no image to be copied, execute on-chip image.
 *
 * @param  none
 *
 * @return none
 */
static void Bim_checkImages(void)
{
#ifndef DEBUG
    /* In case an imaged is flashed via debugger and no external image check should
     * be made, define NO_COPY.
     */
    #ifndef NO_COPY
        checkImagesExtFlash();
    #endif

    /* Find executable on onchip flash and execute */
    checkImagesIntFlash(0);

    /* BIM is not able find any valid application, try to revert to Factory image
     *
     */
    Bim_revertFactoryImage();

#else /* ifdef DEBUG */
    int8_t retVal = 0;

    if( !(retVal = checkImagesExtFlash() ) )/* if it traversed through the metadata */
    {
       checkImagesIntFlash(0); /* Find application image on internal flash page and jump */
    }
    if( retVal == -1)
    {
#ifdef LED_DEBUG
        // Lit LED
#endif
        while(1);
    }
#endif
    /* if it reached there is a problem */
    /* TBD put device to sleep and wait for button press interrupt */
}

/*******************************************************************************
 * @fn          main
 *
 * @brief       C-code main function.
 *
 * input parameters
 *
 * None.
 *
 * output parameters
 *
 * None.
 *
 * @return      None.
 */
int main(void)
{
#ifdef __IAR_SYSTEMS_ICC__
  __set_CONTROL(0);
#endif

#ifdef LED_DEBUG
    /* For LED feedback */
    powerUpGpio();

    /* If the copy of the internal image to external flash failed,
    No Error, Blink Green Led */
    blinkLed(BSP_IOID_LED_2, 10, 50);

    powerDownGpio();
#endif /* #ifdef LED_DEBUG */


    intFlashPageSize = FlashSectorSizeGet();

#ifdef CREATE_FACT_IMG_INT_TO_EXT_FLSH
    Bim_createFactoryImageBackup(INTFLASH_APP_ADDR);
#endif /* CREATE_FACT_IMG_INT_TO_EXT_FLSH */

#ifdef TEST_IMG_COPY
    uint8_t rtn = testImgCopy();
    if(rtn)
    {
        Bim_checkImages();
    }
#endif /* TEST_IMG_COPY */

    Bim_checkImages();
    return(0);
}

/**************************************************************************************************
*/
