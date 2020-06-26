/******************************************************************************

 @file  oad_image_header.h

 @brief This file contains the image preable information.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2001-2018, Texas Instruments Incorporated
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
 ******************************************************************************/

#ifndef _OAD_IMAGE_PREAMBLE_H
#define _OAD_IMAGE_PREAMBLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/*
 * BIM Varible Address
 * The bim_var is a variable stored in RAM that is used to control switching
 * between persistent and user application in the BIM
 * On 26xx platforms RAM is retained between resets and this is possible
 *
 * Warning! placement of the BIM variable must respect the following
 * reserved RAM regions ( Valid for CC2640R2):
 *    - TI-RTOS in ROM reserved region: 0x20000100-0x20000280
 *    - BLE Stack memory: < ICALL_RAM0_START
 *    - Vector Table in RAM: 0x20000000-0x200000C8
 * Failing to respect either of the above boundaries will result in a
 * linker error
 */
#ifdef OAD_ONCHIP
  #define BIM_VAR_ADDR  0x20000400
  extern uint32_t _bim_var;
#endif // OAD_ONCHIP

/*
 * NV Page Setting:
 * This define is used ensure the stack is built with a compatible NV setting
 * Note: this restriction does not apply to the stack library configuration
 * for off-chip OAD
 */
#define OAD_NUM_NV_PGS  1

#ifdef OSAL_SNV
  #if OSAL_SNV != OAD_NUM_NV_PGS
    #error "Incompatible NV settings detected. OAD supports 1 NV page"
  #endif //OSAL_SNV != OAD_NUM_NV_PGS
#endif //OSAL_SNV

// Image types
#define OAD_IMG_TYPE_PERSISTENT_APP  0
#define OAD_IMG_TYPE_APP             1
#define OAD_IMG_TYPE_STACK           2
#define OAD_IMG_TYPE_APP_STACK       3
#define OAD_IMG_TYPE_NP              4
#define OAD_IMG_TYPE_FACTORY         5
#define OAD_IMG_TYPE_BIM                        6
#define OAD_IMG_TYPE_USR_BEGIN                  16
#define OAD_IMG_TYPE_USR_END                    31
#define OAD_IMG_TYPE_HOST_BEGIN                 32
#define OAD_IMG_TYPE_HOST_END                   63
#define OAD_IMG_TYPE_RSVD_BEGIN                 64
#define OAD_IMG_TYPE_RSVD_END                   255

//!< Segment types
#define OAD_SEG_BOUNDARY                        0x00
#define OAD_SEG_CONTIGUOUS                      0x01
#define OAD_SEG_NON_CONTIGUOUS                  0x02
#define OAD_SEG_RSVD                            0xFF

//!< Wireless Technology Types
#define OAD_WIRELESS_TECH_BLE                   0xFFFE
#define OAD_WIRELESS_TECH_TIMAC_SUBG            0xFFFD
#define OAD_WIRELESS_TECH_TIMAC_2_4G            0xFFFB
#define OAD_WIRELESS_TECH_ZIGBEE                0xFFF7
#define OAD_WIRELESS_TECH_RF4CE                 0xFFEF
#define OAD_WIRELESS_TECH_THREAD                0xFFDF
#define OAD_WIRELESS_TECH_EASYLINK              0xFFBF

// Magic numbers used in preamble to indicate valid OAD image
#define OAD_IMG_ID_VAL               {'O', 'A', 'D', ' ', 'I', 'M', 'G', ' '}

// External flash metadata identification value
#define OAD_EXTFL_ID_VAL             {'O', 'A', 'D', ' ', 'N', 'V', 'M', '1'}
#define SOFTWARE_VER                 {'0', '0', '0', '1'}

#define MAX_ONCHIP_FLASH_PAGES       32
#define IMG_DATA_OFFSET              12                                   /* Start of data after CRC bytes */
#define OAD_IMG_HDR_LEN              44                                   /* The length of just the OAD Header */
#define OAD_IMG_FULL_HDR_LEN         sizeof(imgHdr_t)                     /* The length of the OAD Header + Segments */
#define OAD_IMG_ID_LEN               8                                    /* OAD image identification bytes length */
#define OAD_SW_VER_LEN               4                                    /* OAD software version length in bytes */


#define CRC_OFFSET                   offsetof(imgHdr_t, crc32)            /* Supported metadata version */
#define BIM_VER_OFFSET               offsetof(imgHdr_t, bimVer)           /* BIM metadata version offset */
#define IMG_INFO_OFFSET              offsetof(imgHdr_t, bimVer)           /* ImageInfo Bytes offset */
#define IMG_COPY_STAT_OFFSET         IMG_INFO_OFFSET                      /* Image copy status */
#define CRC_STAT_OFFSET              offsetof(imgHdr_t, crcStat)          /* Offset to CRC status byte */
#define IMG_TYPE_OFFSET              offsetof(imgHdr_t, imgType)          /* Offset to CRC status byte */
#define BOUNDARY_SEG_OFFSET          offsetof(imgHdr_t, segTypeBd)        /* Offset into image header for boundary */

#define SEG_LEN_OFFSET               4                                    /* Offset from seg header to seg len */

#define SECURITY_SEG_OFFSET          offsetof(imgHdr_t, segTypeSec)        /* Offset into image header for security segment */
#define SECURITY_SIG_OFFSET          offsetof(securitySeg, secSignature)  /* Offset from the start of security seg to sig */
#define SECURITY_TIMESTAMP           offsetof(securitySeg, secTimestamp)  /* Offset from the start of security seg to the time stamp */
#define SECURITY_SIG_TYPE            offsetof(securitySeg, secVer)        /* Offset from the start of security seg to sig type */
#define SECURITY_RFU_OFFSET          offsetof(securitySeg, rfu5)        /* Offset from the start of security seg to rfy byte */

#define SECURITY_TYPE_NONE           0x00
#define SECURITY_TYPE_ECC            0x01
#define SECURITY_TYPE_AES            0x02

#define BIM_VER                      0x2      /* Supported BIM version */
#define META_VER                     0x1      /* Supported metadata version */

#define DEFAULT_STATE                0xFF    /* default status of Status/Flags in the HDR */
#define CRC_VALID                    0xFE
#define CRC_INVALID                  0xFC
#define NEED_COPY                    0xFE    /* Image to be copied on on-chip flash at location indicated in the image */
#define COPY_DONE                    0xFC    /* Image already copied */

/*******************************************************************************
 * Typedefs
 */

/* Image Preamble */
typedef struct __attribute__((packed))
{
  uint8_t   imgID[8];       //!< User-defined Image Identification bytes. */
  uint32_t  crc32;          //!< Image's crc32 value */
  uint8_t   bimVer;         //!< BIM version */
  uint8_t   metaVer;        //!< Metadata version */
  uint16_t  techType;       //!< Wireless protocol type BLE/TI-MAC/ZIGBEE etc. */
  uint8_t   imgCpStat;      //!< Image copy status */
  uint8_t   crcStat;        //!< CRC status */
  uint8_t   imgType;        //!< Image Type */
  uint8_t   imgNo;          //!< Image number of 'image type' */
  uint32_t  imgVld;         //!< In indicates if the current image in valid 0xff - valid, 0x00 invalid image */
  uint32_t  len;            //!< Image length in bytes. */
  uint32_t  prgEntry;       //!< Program entry address */
  uint8_t   softVer[4];     //!< Software version of the image */
  uint32_t  imgEndAddr;     //!< Address of the last byte of a contiguous image */
  uint16_t  hdrLen;         //!< Total length of the image header */
  uint16_t  rfu2;           //!< Reserved bytes */

  uint8_t   segTypeSec;     //!< Segment type - for Security info payload */
  uint16_t  wirelessTech3;  //!< Wireless technology type */
  uint8_t   rfu5;           //!< Reserved byte */
  uint32_t  secSegLen;      //!< Security segment length */
  uint8_t   secVer;         //!< Security version */
  uint32_t  secTimestamp;   //!< Security timestamp */
  uint64_t  secSignerInfo;  //!< Security signer info */
  uint64_t  secSignature[8];//!< Security signature */

  uint8_t   segTypeBd;      //!< Segment type - for boundary segment */
  uint16_t  wirelessTech1;  //!< Wireless technology type */
  uint8_t   rfu3;           //!< Reserved byte */
  uint32_t  boundarySegLen; //!< Boundary segment length */
  uint32_t  stackStartAddr; //!< Start address of stack image on internal flash */
  uint32_t  stackEntryAddr; //!< Stack start address */
  uint32_t  ram0StartAddr;  //!< RAM entry start address */
  uint32_t  ram0EndAddr;    //!< RAM entry end address */
  uint8_t   segTypeImg;     //!< Segment type - for Contiguous image payload */
  uint16_t  wirelessTech2;  //!< Wireless technology type */
  uint8_t   rfu4;           //!< Reserved byte */
  uint32_t  imgSegLen;      //!< Payload sengment length */
  uint32_t  startAddr;      //!< Start address of image on internal flash */
} imgHdr_t;

/* Image boundary segment */
typedef struct __attribute__((packed))
{
  uint8_t   segTypeBd;        //!< Segment type - for boundary segment */
  uint16_t  wirelessTech1;    //!< Wireless technology type */
  uint8_t   rfu3;
  uint32_t  segTypeNLen;      //!< Start address of stack image on internal flash */
  uint32_t  stackStartAddr;   //!< Start address of stack image on internal flash */
  uint32_t  stackEntryAddr;   //!< Stack start adddress */
  uint32_t  ram0StartAddr;    //!< RAM entry start address */
  uint32_t  ram0EndAddr;      //!< End of Image address */
} boundarySeg;

/* Security segment */
typedef struct __attribute__((packed))
{
  uint8_t   segTypeSec;     //!< Segment type - for Secuirty info payload */
  uint16_t  wirelessTech3;  //!< Wireless technology type */
  uint8_t   rfu5;           //!< Reserved byte */
  uint32_t  secSegLen;      //!< Security sengment length */
  uint8_t   secVer;         //!< Security version */
  uint32_t  secTimestamp;   //!< Security timestamp */
  uint64_t  secSignerInfo;  //!< Security signer info */
  uint64_t  secSignature[8];//!< Security signature */
} securitySeg;
#ifdef __cplusplus
}
#endif

#ifndef FEATURE_OAD_SERVER_ONLY
/* Make the header visible to other components */
extern const imgHdr_t _imgHdr;
#endif //FEATURE_OAD_SERVER_ONLY

#endif /* _OAD_IMAGE_PREAMBLE_H */
