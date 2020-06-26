/******************************************************************************

 @file  oad_image_header.h

 @brief This file contains the image header definition and structures.

 Group: CMCU, LPRF
 Target Device: CC2652

 ******************************************************************************
 
 Copyright (c) 2017-2018, Texas Instruments Incorporated
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

/** ============================================================================
 *  @defgroup OAD_IMAGE_HEADER OAD Image Header
 *  @brief Structure definition for image header and segments, these are
 *         prepended to the over the air images.
 *
 *  @{
 *
 *  # Header Include #
 *  The OAD Image header file should be included in an OAD enabled application
 *  as follows:
 *  @code
 *  #include <common/cc26xx/oad/oad_image_header.h>
 *  @endcode
 *
 *  # Overview #
 *
 *  The OAD image header is a combined structure that is intended to be
 *  prepdended to a binary image before sending over the air.
 *  This file defines the structures that are used to build up an image header.
 *  Additionally, this header file can be used by the application to read fields
 *  within an image header.
 *
 * ## General Behavior #
 * For code examples, see the use cases below.
 *
 *  ### Creating a header structure
 *  - In a separate file, define header structure members like so:
 *  @code
 *  const imgHdr_t _imgHdr =
 *  {
 *    // First define core header (imgFixedHdr_t)
 *    {
 *      .imgID = OAD_IMG_ID_VAL,
 *      .crc32 = DEFAULT_CRC,
 *      .bimVer = BIM_VER,
 *      .metaVer = META_VER,
 *      // Define rest of core header below ...
 *    },
 *    // Image payload segment initialization
 *    {
 *      .segTypeImg = IMG_PAYLOAD_SEG_ID,
 *      .wirelessTech = OAD_WIRELESS_TECH_BLE,
 *      .rfu = DEFAULT_STATE,
 *      .startAddr = (uint32_t)&(_imgHdr.fixedHdr.imgID),
 *    }
 *  };
 *  @endcode
 *
 *  - Note: A core image header and contiguous image header are required
 *  - When building the stack as a separate image, a boundary segment is
 *    required (boundarySeg_t)
 *
 *  ### Placing/Using a header structure
 *  - The OAD image header must precede an executable OAD image
 *  - The OAD image header must be aligned on page boundary
 *  - The linker file is responsible for the two requirements above
 *  - A sample placement of the header in CCS
 *    @code
 *    #pragma DATA_SECTION(_imgHdr, ".image_header")
 *    #pragma RETAIN(_imgHdr)
 *    const imgHdr_t _imgHdr =
 *    @endcode
 *  - A sample placement for IAR
 *    @code
 *    #pragma location=".img_hdr"
 *    const imgHdr_t _imgHdr @ ".img_hdr" =
 *    @endcode
 *
 *
 *  @file  oad_image_header.h
 *
 *  @brief Structure definition for image header and segments, these are
 *         prepended to the over the air images.
 */

#ifndef OAD_IMAGE_HEADER_H
#define OAD_IMAGE_HEADER_H

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


#ifdef OAD_ONCHIP
  /*!
   * BIM Variable Address
   *
   * The bim_var is a variable stored in RAM that is used to control switching
   * between persistent and user application in the BIM
   * On CC26xx platforms RAM is retained between resets and this is possible
   *
   * Warning! placement of the BIM variable must respect the following
   * reserved RAM regions ( Valid for CC2640R2):
   *    - TI-RTOS in ROM reserved region: 0x20000100-0x20000280
   *    - BLE Stack memory: < ICALL_RAM0_START
   *    - Vector Table in RAM: 0x20000000-0x200000C8
   * Failing to respect either of the above boundaries will result in a
   * linker error
   */
  #define BIM_VAR_ADDR  0x20000400

  /*!
   * BIM Argument variable
   *
   * The application is responsible for defining this variable for on-chip OAD
   */
  extern uint32_t _bim_var;
#endif // OAD_ONCHIP

/**
 * @defgroup OAD_IMG_TYPE OAD image types
 * @{
 */

/*!
 * OAD on-chip persistent application image
 *
 * The persistent app is the permanently resident app used in on-chip OAD
 * The persistent app is responsible for updating the user application
 */
#define OAD_IMG_TYPE_PERSISTENT_APP       0

/*!
 * OAD user application image
 *
 * A user application of split app/stack type
 */
#define OAD_IMG_TYPE_APP                  1

/*!
 * OAD stack image
 *
 * An OAD enabled stack image of split image type
 */
#define OAD_IMG_TYPE_STACK                2

/*!
 * OAD app + stack merge
 *
 * An OAD enabled image that is a merge of both app and stack that are of split
 * image type.
 */
#define OAD_IMG_TYPE_APP_STACK            3

/*!
 * OAD network processor image
 *
 * A network processor image. This image is intended to run an external
 * processor
 */
#define OAD_IMG_TYPE_NP                   4

/*!
 * OAD network processor image
 *
 * A network processor image. This image is intended to run an external
 * processor
 */
#define OAD_IMG_TYPE_FACTORY              5

/*!
 * OAD Boot Image Manager (BIM) image
 *
 * A BIM image
 * \note BIM upgrade is not currently supported
 */
#define OAD_IMG_TYPE_BIM                  6

/*!
 * OAD app + stack library image
 *
 * An OAD enabled executable where the app and stack are built and linked
 * together as one image.
 * \note This is different than @ref OAD_IMG_TYPE_APP_STACK which is a post
 *        build merge of two split images.
 */
#define OAD_IMG_TYPE_APPSTACKLIB          7

/*!
 * OAD User defined image type begin
 *
 * This is the beginning of the range of image types reserved for the user.
 * OAD applications will not attempt to use or load images with this type
 */
#define OAD_IMG_TYPE_USR_BEGIN            16

/*!
 * OAD User defined image type end
 *
 * This is the end of the range of image types reserved for the user.
 * OAD applications will not attempt to use or load images with this type
 */
#define OAD_IMG_TYPE_USR_END              31

/*!
 * OAD Host image type  begin
 *
 * An image for a host processor
 */
#define OAD_IMG_TYPE_HOST_BEGIN           32

/*!
 * OAD Host image type  end
 *
 * An image for a host processor
 */
#define OAD_IMG_TYPE_HOST_END             63

/*!
 * OAD reserved image type begin
 *
 * \warning These fields are RFU
 */
#define OAD_IMG_TYPE_RSVD_BEGIN           64

/*!
 * OAD reserved image type end
 *
 * \warning These fields are RFU
 */
#define OAD_IMG_TYPE_RSVD_END             255

/** @} End OAD_IMG_TYPE */

/**
 * @defgroup OAD_WIRELESS_TECH OAD wireless technology fields
 * @{
 */

/*!
 * Image built for BLE
 */
#define OAD_WIRELESS_TECH_BLE             0xFFFE

/*!
 * Image built for Sub1-GHz TI 15.4-Stack
 */
#define OAD_WIRELESS_TECH_TIMAC_SUBG      0xFFFD

/*!
 * Image built for 2.4-GHz TI 15.4-Stack
 */
#define OAD_WIRELESS_TECH_TIMAC_2_4G      0xFFFB

/*!
 * Image built for Zigbee
 */
#define OAD_WIRELESS_TECH_ZIGBEE          0xFFF7

/*!
 * Image built for RF4CE
 */
#define OAD_WIRELESS_TECH_RF4CE           0xFFEF

/*!
 * Image built for TI Open Thread
 */
#define OAD_WIRELESS_TECH_THREAD          0xFFDF

/*!
 * Image built for EasyLink
 */
#define OAD_WIRELESS_TECH_EASYLINK        0xFFBF

/** @} End OAD_WIRELESS_TECH */

#ifdef DeviceFamily_CC26X2
  #define OAD_IMG_ID_VAL                    {'C', 'C', '2', '6', 'x', '2', 'R', '1'}
#elif defined (DeviceFamily_CC13X2) ||                                        \
      defined (DeviceFamily_CC13X2_V1) ||                                     \
      defined (DeviceFamily_CC13X2_V2)
  #define OAD_IMG_ID_VAL                    {'C', 'C', '1', '3', 'x', '2', 'R', '1'}
#elif defined (DeviceFamily_CC26X0R2)
  #define OAD_IMG_ID_VAL                    {'O', 'A', 'D', ' ', 'I', 'M', 'G', ' '}
#elif defined (DOXYGEN)
  /*!
   * Magic number to identify OAD image header. It is recommended that the
   * customer adjust these to uniquely identify their device
   *
   * This define will be filled in conditionally based on DeviceFamily_*
   *   - DeviceFamily_CC26X2:  @code {'C', 'C', '2', '6', 'x', '2', 'R', '1'} @endcode
   *   - DeviceFamily_CC13X2:  @code {'C', 'C', '1', '3', 'x', '2', 'R', '1'} @endcode
   *   - DeviceFamily_CC26X0R2: @code {'O', 'A', 'D', ' ', 'I', 'M', 'G', ' '} @endcode
   *   - Else an error will be thrown
   */
  #define OAD_IMG_ID_VAL
#else
  #error "Unknown Device Family"
#endif //DeviceFamily_CC26X2

/*!
 * Magic number used by entries in the image header table at the beginning of
 * external flash. Note this is different from @ref OAD_IMG_ID_VAL so that
 * the application can determine the difference between an entry in the header
 * table and the start of an OAD image
 *
 * \note This is only used by off-chip OAD
 */
#define OAD_EXTFL_ID_VAL                  {'O', 'A', 'D', ' ', 'N', 'V', 'M', '1'}


/**
 * @defgroup OAD_OFFSETS OAD Image Header offsets
 * @{
 */

/*!
 * Offset in bytes of crc32 into the structure @ref imgFixedHdr_t
 */
#define CRC_OFFSET                   offsetof(imgHdr_t, fixedHdr.crc32)

/*!
 * Offset in bytes of bimVer into the structure @ref imgFixedHdr_t
 */
#define BIM_VER_OFFSET               offsetof(imgHdr_t, fixedHdr.bimVer)

/*!
 * Offset in bytes of bimVer into the structure
 * @ref imgFixedHdr_t. This is the start of data after the CRC bytes
 */
#define IMG_DATA_OFFSET              BIM_VER_OFFSET

/*!
 * Offset in bytes of imgCpStat into the structure @ref imgFixedHdr_t
 */
#define IMG_INFO_OFFSET              offsetof(imgHdr_t, fixedHdr.imgCpStat)

/*!
 * Offset in bytes of @ref IMG_INFO_OFFSET into the structure @ref imgFixedHdr_t
 */
#define IMG_COPY_STAT_OFFSET         IMG_INFO_OFFSET

/*!
 * Offset in bytes of CRC status into the structure @ref imgFixedHdr_t
 */
#define CRC_STAT_OFFSET              offsetof(imgHdr_t, fixedHdr.crcStat)

/*!
 * Offset in bytes of imgType into the structure @ref imgFixedHdr_t
 */
#define IMG_TYPE_OFFSET              offsetof(imgHdr_t, fixedHdr.imgType)

/*!
 * Offset in bytes of the boundary segment into the structure @ref imgHdr_t
 */
#define BOUNDARY_SEG_OFFSET          offsetof(imgHdr_t, fixedHdr.segTypeBd)


/*!
 * Offset in bytes of the length field into the structure @ref boundarySeg_t
 */
#define SEG_LEN_OFFSET               4

/** @} End OAD_OFFSETS */

/**
 * @defgroup OAD_LENGTHS OAD image lengths
 * @{
 */

/*!
 * Length of @ref boundarySeg_t in bytes
 */
#define SEG_HDR_LEN                  12

/*!
 * Length of OAD image ID field, this is the length of @ref OAD_IMG_ID_VAL and
 * @ref OAD_EXTFL_ID_VAL
 */
#define OAD_IMG_ID_LEN               8

/*!
 * Length of software version in bytes
 */
#define OAD_SW_VER_LEN               4

/*!
 * Length of @ref imgFixedHdr_t, this is the length of the core header
 */
#define OAD_IMG_HDR_LEN              sizeof(imgFixedHdr_t)

/** @} End OAD_LENGTHS */


/**
 * @defgroup OAD_SEG_CODES OAD segment op codes
 * @{
 */

/*!
 * Op code used to identify a boundary segment @ref boundarySeg_t
 */
#define IMG_BOUNDARY_SEG_ID          0x00

/*!
 * Op code used to identify a contiguous image segment  @ref imgPayloadSeg_t
 */
#define IMG_PAYLOAD_SEG_ID           0x01


/*!
 * Op code used to identify a non-contiguous image segment.
 */
#define IMG_NONCOUNT_SEG_ID          0x02

/*!
 * Op code used to identify a security segment.
 */
#define IMG_SECURITY_SEG_ID          0x03

/** @} End OAD_SEG_CODES */

/*!
 * Version of BIM this image is intended to work with
 *
 * \warning This is not intended to be changed
 */
#define BIM_VER                      0x3

/*!
 * Version of metadata that defined this image header
 *
 * \warning This is not intended to be changed
 */
#define META_VER                     0x1

/*!
 * Default state of unprogrammed flash in an image header field
 */
#define DEFAULT_STATE                0xFF

/*!
 * Flag for crcStat indicating the CRC of the image is
 * valid
 */
#define CRC_VALID                    0xFE

/*!
 * Flag for crcStat indicating the CRC of the image is
 * invalid
 */
#define CRC_INVALID                  0xFC

/*!
 * FlagimgCpStat indicating the image should be copied
 */
#define NEED_COPY                    0xFE

/*!
 * FlagimgCpStat indicating the image is already copied
 */
#define COPY_DONE                    0xFC

/*!
 * Length of image external flash image header
 */
#define EFL_MATADATA_HDR_LEN         OAD_IMG_HDR_LEN + 8

/*!
 * An invalid address as shown by unprogrammed flash
 */
#define INVALID_ADDR                 0xFFFFFFFF

/*!
 * An invalid length as shown by unprogrammed flash.
 */
#define INVALID_LEN                  INVALID_ADDR

/*!
 * An invalid address as shown by unprogrammed flash.
 */
#define DEFAULT_CRC                  0xFFFFFFFF

/*******************************************************************************
 * Typedefs
 */

/// @cond NODOC
#if defined (__IAR_SYSTEMS_ICC__)
  #define TYPEDEF_STRUCT_PACKED        __packed typedef struct
#elif defined __TI_COMPILER_VERSION || defined __TI_COMPILER_VERSION__
  #define TYPEDEF_STRUCT_PACKED        typedef struct __attribute__((packed))
#elif defined (__GNUC__)
  #define TYPEDEF_STRUCT_PACKED        typedef struct __attribute__((__packed__))
#endif
/// @endcond // NODOC

/*!
 * OAD Core image header structure definition
 * \note This header is required to be part of all OAD images
 */
TYPEDEF_STRUCT_PACKED
{
  uint8_t   imgID[8];       //!< User-defined Image Identification bytes
  uint32_t  crc32;          //!< Image's 32-bit CRC value
  uint8_t   bimVer;         //!< BIM version
  uint8_t   metaVer;        //!< Metadata version
  uint16_t  techType;       //!< Wireless protocol type BLE/TI-MAC/ZIGBEE etc.
  uint8_t   imgCpStat;      //!< Image copy status
  uint8_t   crcStat;        //!< CRC status
  uint8_t   imgType;        //!< Image Type
  uint8_t   imgNo;          //!< Image number of 'image type'
  uint32_t  imgVld;         //!< This field is RFU
  uint32_t  len;            //!< Image length in bytes.
  uint32_t  prgEntry;       //!< Program entry address
  uint8_t   softVer[4];     //!< Software version of the image
  uint32_t  imgEndAddr;     //!< Address of the last byte of a contiguous image
  uint16_t  hdrLen;         //!< Total length of the image header
  uint16_t  rfu;            //!< Reserved bytes
}imgFixedHdr_t;

/*!
 * OAD continuous image payload segment
 *
 * This segment is used to describe a contiguous region of an image
 * The code data in this blob will be flashed to sequential addresses
 */
TYPEDEF_STRUCT_PACKED
{
  uint8_t   segTypeImg;     //!< Segment type - for Contiguous image payload
  uint16_t  wirelessTech;   //!< Wireless technology type
  uint8_t   rfu;            //!< Reserved byte
  uint32_t  imgSegLen;      //!< Payload segment length
  uint32_t  startAddr;      //!< Start address of image on internal flash
}imgPayloadSeg_t;

/*!
 * OAD boundary segment
 *
 * This segment is used to describe the RAM and Flash boundaries in a split
 * image system.
 *
 * \note This segment is optional and not required if app and stack are linked
 *       together as one image.
 */
TYPEDEF_STRUCT_PACKED
{
  uint8_t   segTypeBd;      //!< Segment type - for boundary segment
  uint16_t  wirelessTech;   //!< Wireless technology type
  uint8_t   rfu;            //!< Reserved byte
  uint32_t  boundarySegLen; //!< Boundary segment length
  uint32_t  stackStartAddr; //!< Start address of stack image on internal flash
  uint32_t  stackEntryAddr; //!< Stack start address
  uint32_t  ram0StartAddr;  //!< RAM entry start address
  uint32_t  ram0EndAddr;    //!< RAM entry end address
}boundarySeg_t;

/*!
 * OAD Image Header
 *
 * This structure represents the complete header to be prepended to an OAD image
 *
 * It is an amalgamation of the structures defined above, and must contain a
 * core header
 */
TYPEDEF_STRUCT_PACKED
{
   imgFixedHdr_t         fixedHdr;    //!< Required core image header
#if (!defined(STACK_LIBRARY) && (defined(SPLIT_APP_STACK_IMAGE)))
   boundarySeg_t         boundarySeg; //!< Optional boundary segment
#endif
   imgPayloadSeg_t       imgPayload;  //!< Required contiguous image segment
} imgHdr_t;

/** @} End OAD_IMAGE_HEADER */

#ifdef __cplusplus
}
#endif

#endif /* OAD_IMAGE_HEADER_H */
