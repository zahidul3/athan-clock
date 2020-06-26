/******************************************************************************

 @file oad_storage.h

 @brief OAD Storage Header

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
#ifndef OADStorage_H
#define OADStorage_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include <stdbool.h>

/*********************************************************************
 * CONSTANTS
 */
 
 /// OADStorage_Status_t status codes
typedef enum {
    OADStorage_Status_Success, ///< Success
    OADStorage_Failed,         ///< Fail
    OADStorage_CrcError,       ///< Acknowledgment or Response Timed out
    OADStorage_FlashError,     ///< flash access error
    OADStorage_Aborted,        ///< Canceled by application
    OADStorage_Rejected,       ///< OAD request rejected by application
} OADStorage_Status_t;

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERNAL VARIABLES
 */


/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * @fn      OADStorage_init
 *
 * @brief   Initialise the OAD Target Profile.
 *
 * @param   None.
 *
 * @return  None.
 */
extern void OADStorage_init(void);

/*********************************************************************
 * @fn      OADStorage_imgIdentifyRead
 *
 * @brief   Read Image header and return number of blocks.
 *
 * @param   imageType   - image type indicating which image to read
 * @param   pImgHdr     - pointer to image header data
 *
 * @return  Total Blocks if image accepted, 0 if Image invalid
 */
uint16_t OADStorage_imgIdentifyRead(uint8_t imageType, OADTarget_ImgHdr_t *pImgHdr);

/*********************************************************************
 * @fn      OADStorage_imgIdentifyWrite
 *
 * @brief   Process the Image Identify Write.  Determine from the received OAD
 *          Image Header if the Downloaded Image should be acquired.
 *
 * @param   pValue     - pointer to image header data
 * @param   erase      - set true to erase image area in flash
 *
 * @return  Total Blocks if image accepted, 0 if Image rejected
 */
uint16_t OADStorage_imgIdentifyWrite(uint8_t *pValue, bool erase);

/*********************************************************************
 * @fn      OADStorage_imgBlockRead
 *
 * @brief   Read Image Block.
 *
 * @param   blockNum   - block number to be written
 * @param   pBlockData - pointer for data to be read
 *
 * @return  none
 */
extern void OADStorage_imgBlockRead(uint16_t blockNum, uint8_t *pBlockData);

/*********************************************************************
 * @fn      OADStorage_imgInfoRead
 *
 * @brief   Read an Image info.
 *
 * @param   pimgInfo - pointer for data to be read
 *
 * @return  none
 */
extern void OADStorage_imgInfoRead(uint8_t *pimgInfo);

/*********************************************************************
 * @fn      OADStorage_imgBlockWrite
 *
 * @brief   Write Image Block.
 *
 * @param   blockNum   - block number to be written
 * @param   pBlockData - pointer to data to be written
 *
 * @return  status
 */
extern void OADStorage_imgBlockWrite(uint16_t blockNum, uint8_t *pBlockData);

/*********************************************************************
 * @fn      OADStorage_imgFinalise
 *
 * @brief   Process the Image Block Write.
 *
 * @param  none
 *
 * @return  status
 */
extern OADStorage_Status_t OADStorage_imgFinalise(void);

/*********************************************************************
 * @fn      OADStorage_eraseImgPage
 *
 * @brief   Erases an Image page. Note this is only needed if an image
 *          page has been corrupted typically OADStorage_imgBlockWrite
 *          pre-erase all pages
 *
 * @param  none
 *
 * @return  OADStorage_Status_t
 */
extern OADStorage_Status_t OADStorage_eraseImgPage(uint32_t page);

/*********************************************************************
 * @fn      OADStorage_close
 *
 * @brief   Releases the resource required for OAD stoarage.
 *
 * @param  none
 *
 * @return none
 */
extern void OADStorage_close(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OADStorage_H */
