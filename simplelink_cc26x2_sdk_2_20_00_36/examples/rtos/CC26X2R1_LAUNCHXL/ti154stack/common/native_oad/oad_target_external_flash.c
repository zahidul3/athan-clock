/******************************************************************************

 @file oad_target_flash,c

 @brief OAD Target

 Group: WCS LPC
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2016-2018, Texas Instruments Incorporated
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
 * INCLUDES
 */
#if defined(FEATURE_BLE_OAD) || defined(FEATURE_NATIVE_OAD)
#include <string.h>
#include "common/native_oad/oad_target.h"
#include "common/native_oad/ext_flash_layout.h"

#include <extflash/ExtFlash.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)


/*******************************************************************************
 * Constants and macros
 */
#define PROG_BUF_SIZE             16
#define PAGE_0                    0
#define PAGE_1                    1
#define PAGE_31                   31

#define APP_IMAGE_START           0x1000
#define BOOT_LOADER_START         0x1F000

#define MAX_BLOCKS                (EFL_SIZE_IMAGE_APP / OAD_BLOCK_SIZE)

/*******************************************************************************
 * PRIVATE VARIABLES
 */
static bool isOpen = false;
static ExtImageInfo_t imgInfo;

/*******************************************************************************
 * PRIVATE FUNCTIONS
 */

/*******************************************************************************
 * FUNCTIONS
 */

/*******************************************************************************
 * @fn      OADTarget_open
 *
 * @brief   Open an OAD target for download.
 *
 * @param   none
 *
 * @return  true if OAD target successfully opened
 */
uint8_t OADTarget_open(void)
{
    if (!isOpen)
    {
        isOpen = ExtFlash_open();
    }

    return isOpen ? true : false;
}


/*******************************************************************************
 * @fn      OADTarget_close
 *
 * @brief   Close an OAD target after a download has finished
 *
 * @param   none
 *
 * @return  none
 */
void OADTarget_close(void)
{
  if (isOpen)
  {
    isOpen = false;
    ExtFlash_close();
  }
}

/*******************************************************************************
 * @fn      OADTarget_hasExternalFlash
 *
 * @brief   Check if the target has external flash
 *
 * @param   none
 *
 * @return  always true
 */
uint8_t OADTarget_hasExternalFlash(void)
{
  return true;
}

/*******************************************************************************
 * @fn      OADTarget_getCurrentImageHeader
 *
 * @brief   Get the header of the running image.
 *
 * @param   pHdr - pointer to store running image header.
 *
 * @return  true if flash is successfully opened
 */
uint8_t OADTarget_getCurrentImageHeader(OADTarget_ImgHdr_t *pHdr)
{
  uint8_t ret;

  ret = ExtFlash_open() ? true : false;

  if (ret)
  {
    // N.B: it is assumed that OADTarget_storeImageHeader() has been called
    // prior to this function.
    uint32_t metaDataAddr;
    ExtImageInfo_t tempHdr;

    if (imgInfo.imgType == EFL_OAD_IMG_TYPE_STACK
        || imgInfo.imgType == EFL_OAD_IMG_TYPE_NP)
    {
      metaDataAddr = EFL_IMAGE_INFO_ADDR_BLE;
    }
    else // Assume imgInfo.imgType == EFL_OAD_IMG_TYPE_APP
    {
      metaDataAddr = EFL_IMAGE_INFO_ADDR_APP;
    }

    ExtFlash_read(metaDataAddr, sizeof(ExtImageInfo_t), (uint8_t*)&tempHdr);
    ExtFlash_close();

    pHdr->len = tempHdr.len;

    // In case metadata does not exist, use 0x00
    pHdr->ver = tempHdr.ver == 0xFF ? 0x00 : tempHdr.ver;
    memcpy(pHdr->uid,tempHdr.uid,sizeof(tempHdr.uid));
    pHdr->res[0] = OADTarget_HI_UINT16(tempHdr.addr);
    pHdr->res[1] = OADTarget_LO_UINT16(tempHdr.addr);
    pHdr->res[2] = tempHdr.imgType;
    pHdr->res[3] = 0xFF;
  }

  return ret;
}

/*******************************************************************************
 * @fn      OADTarget_getRemoteImageHeader
 *
 * @brief   Get the header of the remote app image.
 *
 * @param   imageType - Image type.
 * @param   pHdr      - pointer to store running image header.
 *
 * @return  true if flash is successfully opened
 */
uint16_t OADTarget_getImageHeader(uint8_t imageType, OADTarget_ImgHdr_t *pHdr)
{
  uint8_t ret;
  uint32_t metaDataAddr;

  if (imageType == EFL_OAD_IMG_TYPE_STACK
      || imageType == EFL_OAD_IMG_TYPE_NP)
  {
      metaDataAddr = EFL_IMAGE_INFO_ADDR_BLE;
  }
  else if(imageType == EFL_OAD_IMG_TYPE_REMOTE_APP)
  {
      metaDataAddr = EFL_IMAGE_INFO_ADDR_REMOTE_APP;
  }
  else // Assume imageType == EFL_OAD_IMG_TYPE_APP
  {
      metaDataAddr = EFL_IMAGE_INFO_ADDR_APP;
  }

  ret = ExtFlash_open() ? true : false;

  if (ret)
  {
    ExtImageInfo_t tempHdr;

    ExtFlash_read(metaDataAddr, sizeof(ExtImageInfo_t), (uint8_t*)&tempHdr);
    ExtFlash_close();

    pHdr->len = tempHdr.len;

    // In case metadata does not exist, use 0x00
    pHdr->ver = tempHdr.ver == 0xFF ? 0x00 : tempHdr.ver;
    memcpy(pHdr->uid,tempHdr.uid,sizeof(tempHdr.uid));
    pHdr->res[0] = OADTarget_HI_UINT16(tempHdr.addr);
    pHdr->res[1] = OADTarget_LO_UINT16(tempHdr.addr);
    pHdr->res[2] = tempHdr.imgType;
    pHdr->res[3] = 0xFF;
  }

  return ret;
}

/*******************************************************************************
 * @fn      OADTarget_validateNewImage
 *
 * @brief   Determine if a new image should be downloaded or not based on
 *          target specific criteria.
 *
 * @param   pValue - pointer to new Image header information
 * @param   pCur - pointer to contents of current image header
 * @param   blkTot - total number of blocks comprising new image.
 *
 * @return  true to begin OAD otherwise false to reject the image.
 */
uint8_t OADTarget_validateNewImage(uint8_t *pValue, OADTarget_ImgHdr_t *pCur,
                                   uint16_t blkTot)
{
  OADTarget_ImgHdr_t *pNew;
  uint32_t addr;
  uint8_t imgType;
  uint8_t valid;

  pNew = (OADTarget_ImgHdr_t *)pValue;
  addr = OADTarget_BUILD_UINT16(pNew->res[0],pNew->res[1]) * EFL_OAD_ADDR_RESOLUTION;
  imgType = pNew->res[2];
  valid = false;

  // Check if number of blocks make sense
  if (blkTot > MAX_BLOCKS || blkTot == 0)
  {
    return false;
  }

  //Note that network processor images can/will take up the entire internal
  //flash space
  if (imgType != EFL_OAD_IMG_TYPE_NP
      && (addr < APP_IMAGE_START || addr > BOOT_LOADER_START))
  {
    return false;
  }

  // Image type 'APP' must start at 0x1000
  if (addr != APP_IMAGE_START && imgType == EFL_OAD_IMG_TYPE_APP)
  {
    return false;
  }

  // Check if current header is invalid
  if (pCur->ver == 0xFFFF || pCur->ver == 0x0000)
  {
    // Accept the image.
    return true;
  }

  // By default, accept an image if version is 0.
  if (pNew->ver == 0)
  {
    valid = true;
  }

  // If not already validated, check if new image is a later version than the
  // current image.
  //TODO: Allow old images for now
  valid = 1;
  //if (!valid)
  //{
  //  valid =  pNew->ver > pCur->ver;
  //}

  return valid;
}

/*******************************************************************************
 * @fn      OADTarget_storeImageHeader
 *
 * @brief   Store the image header of the new image
 *
 * @param   pValue - pointer to the new image header
 *
 * @return  none
 */
void OADTarget_storeImageHeader(uint8_t *pValue)
{
  OADTarget_ImgHdr_t* pNew;

  pNew = (OADTarget_ImgHdr_t*)(pValue +4);

  // Storage image header (written to external flash before reboot)
  imgInfo.crc[0] = OADTarget_BUILD_UINT16(pValue[0],pValue[1]);
  imgInfo.crc[1] = OADTarget_BUILD_UINT16(pValue[2],pValue[3]);
  imgInfo.addr = OADTarget_BUILD_UINT16(pNew->res[0],pNew->res[1]);
  imgInfo.ver = pNew->ver;
  imgInfo.len = pNew->len;
  memcpy(imgInfo.uid,pNew->uid,sizeof(imgInfo.uid));
  imgInfo.imgType = pNew->res[2];
}

/*******************************************************************************
 * @fn      OADTarget_imageAddress
 *
 * @brief   Get the address to store the new image
 *
 * @param   pValue - pointer to the new image header
 *
 * @param   pHdr - pointer to the current image header
 *
 * @return  address
 */
uint32_t OADTarget_imageAddress(uint8_t *pValue)
{
  OADTarget_ImgHdr_t *pNew;
  uint32_t extAddr;

  pNew = (OADTarget_ImgHdr_t *)pValue;

  switch (pNew->res[2])
  {
    // Application start at the beginning of external flash.
    case EFL_OAD_IMG_TYPE_APP:
      extAddr = EFL_ADDR_IMAGE_APP;
      break;

    // Remote Application image.
    case EFL_OAD_IMG_TYPE_REMOTE_APP:
      extAddr = EFL_ADDR_IMAGE_REMOTE_APP;
      break;

    // All other images are placed into the next available image slot.
    default:
      extAddr = EFL_ADDR_IMAGE_BLE;
      break;
  }

  return extAddr;
}

/*******************************************************************************
 * @fn      OADTarget_getCrc
 *
 * @brief   Get the CRC array from the image that is being downloaded
 *
 * @param   pCrc - pointer to the new image header
 *
 * @return  None
 */
void OADTarget_getCrc(uint16_t *pCrc)
{
  // Copy CRC information from header information.
  pCrc[0] = imgInfo.crc[0];
  pCrc[1] = imgInfo.crc[1];
}

/*******************************************************************************
 * @fn      OADTarget_setCrc
 *
 * @brief   Set the CRC shadow of the downloaded image.
 *
 * @param   pCrc - pointer to the new image header
 *
 * @return  Non
 */
void OADTarget_setCrc(uint16_t *pCrc)
{
  // Update shadow CRC to verify
  imgInfo.crc[1] = pCrc[1];
}

/*******************************************************************************
 * @fn      OADTarget_enableCache
 *
 * @brief   Prepares system for a write to flash, if necessary.
 *
 * @param   None.
 *
 * @return  None.
 */
void OADTarget_enableCache(void)
{
  // Do nothing.
}

/*******************************************************************************
 * @fn      OADTarget_disableCache
 *
 * @brief   Resumes system after a write to flash, if necessary.
 *
 * @param   None.
 *
 * @return  None.
 */
void OADTarget_disableCache(void)
{
  // Do nothing.
}

/*******************************************************************************
 * @fn      OADTarget_readFlash
 *
 * @brief   Read data from flash.
 *
 * @param   page   - page to read from in flash
 * @param   offset - offset into flash page to begin reading
 * @param   pBuf   - pointer to buffer into which data is read.
 * @param   len    - length of data to read in bytes.
 *
 * @return  None.
 */
void OADTarget_readFlash(uint8_t page, uint32_t offset, uint8_t *pBuf,
                         uint16_t len)
{
  ExtFlash_read(FLASH_ADDRESS(page,offset), len, pBuf);
}

/*******************************************************************************
 * @fn      OADTarget_writeFlash
 *
 * @brief   Write data to flash.
 *
 * @param   page   - page to write to in flash
 * @param   offset - offset into flash page to begin writing
 * @param   pBuf   - pointer to buffer of data to write
 * @param   len    - length of data to write in bytes
 *
 * @return  None.
 */
void OADTarget_writeFlash(uint8_t page, uint32_t offset, uint8_t *pBuf,
                          uint16_t len)
{
  ExtFlash_write(FLASH_ADDRESS(page,offset), len, pBuf);
}

/*********************************************************************
 * @fn      OADTarget_eraseFlash
 *
 * @brief   Erase selected flash page.
 *
 * @param   page - the page to erase.
 *
 * @return  None.
 */
void OADTarget_eraseFlash(uint8_t page)
{
  uint32_t flashPageSize = FlashSectorSizeGet();
  ExtFlash_erase(FLASH_ADDRESS(page,0), flashPageSize);
}

/*******************************************************************************
 * @fn      saveImageInfo
 *
 * @brief   Save image information in the meta-data area
 *
 * @return  none
 */
void saveImageInfo(void)
{
  uint32_t addr;
  uint32_t flashPageSize = FlashSectorSizeGet();

  if (imgInfo.imgType == EFL_OAD_IMG_TYPE_APP)
  {
    addr = EFL_IMAGE_INFO_ADDR_APP;
  }
  else if (imgInfo.imgType == EFL_OAD_IMG_TYPE_REMOTE_APP)
  {
    addr = EFL_IMAGE_INFO_ADDR_REMOTE_APP;
  }
  else
  {
    addr = EFL_IMAGE_INFO_ADDR_BLE;
  }

  // Erase old meta data.
  ExtFlash_erase(addr, flashPageSize);

  // Set status so that bootloader pull in the new image.
  imgInfo.status = 0xFF;

  // Write new meta data.
  ExtFlash_write(addr, sizeof(ExtImageInfo_t),
                (uint8_t*)&imgInfo);
}

/*******************************************************************************
 * @fn      getImageFlag
 *
 * @brief   Get the image type flag.
 *
 * @return  Image type or 0 if unknown.
 */
uint8_t getImageFlag(void)
{

  uint8_t flag = 0;

  if (imgInfo.imgType == EFL_OAD_IMG_TYPE_APP)
  {
    flag = OAD_IMG_APP_FLAG;
  }
  else if (imgInfo.imgType == EFL_OAD_IMG_TYPE_STACK)
  {
    flag = OAD_IMG_STACK_FLAG;
  }
  else if (imgInfo.imgType == EFL_OAD_IMG_TYPE_NP)
  {
    flag = OAD_IMG_NP_FLAG;
  }

  return flag;
}

#endif //FEATURE_BLE_OAD || FEATURE_NATIVE_OAD
