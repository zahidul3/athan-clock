/******************************************************************************

 @file oad_protocol.h

 @brief OAD Protocol Header

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
/*!*****************************************************************************
 *  @file  oad_protocol.h
 *
 *  @brief      Wireless Sensor Network Protocol
 *
 *  The OADProtocol interface provides device independent APIs, data types,
 *  and macros.
 *
 *  # Overview #
 *
 *  The Over the Air Download Protocol module provides protocol
 *  functionality on top of the RF protocol. 
 *
 *  The APIs in this module serve as an interface to a TIRTOS
 *  application and offers functionality for a OAD messaging protocol between
 *  OAD Sever and the OAD Client. The module  handles formating / parsing of
 *  messages.
 *
 *  # Usage #
 *
 *  To use the OADProtocol module to format/parse OAD messages, the application
 *  calls the following APIs:
 *    - OADProtocol_init(): Initialize the OADProtocol module/task.
 *    - OADProtocol_Params_init():  Initialize a OADProtocol_Params structure
 *      with default values.  Then change the parameters from non-default
 *      values as needed.
 *    - OADProtocol_open():  Open an instance of the OADProtocol module,
 *      passing the initialized parameters.
 *    - OADProtocol_sendFwRequest():  This is an example of an OAD message that
 *      is formated and sent.
 *
 *  The following code example opens OADProtocol, sends a FW version request
 *  and processes the response.
 *
 *  @code
 *
 *  OADProtocol_packetCBs_t OADProtocolCbs = {
 *    NULL,              //Incoming FW Req
 *    fwVersionReqCb,    //Incoming FW Version Rsp
 *    NULL,              //Incoming Image Identify Req
 *    NULL,              //Incoming Image Identify Rsp
 *    NULL,              //Incoming OAD Block Req
 *    NULL,              //Incoming OAD Block Rsp
 *  };
 *
 * static void fwVersionRspCb(void* pSrcAddr, char *fwVersionStr)
 * {
 *   //Do something with srcAddr and fwVersionStr
 * }
 *
 * void someTaskInit(void)
 * {
 *   OADProtocol_init();
 * }
 *
 * void someTaskFxn(void)
 * {
 *   // Set Default parameters structure
 *   static OADProtocol_Params_t OADProtocol_params;
 *
 *   // Initialize and open the Wsn Protocol Task
 *   OADProtocol_Params_init(&OADProtocol_params);
 *   OADProtocol_params.pCallbacks = &OADProtocolCbs;
 *   OADProtocol_open(&OADProtocol_params);
 *
 *   OADProtocol_sendFwVersionReq(nodeAddress);
 *
 *  }
 *  @endcode
 *
 *
 *  ## OADProtocol Configuration ##
 *
 *  In order to use the OADProtocol APIs, the application is required
 *  to provide application specific configuration and callbacks.
 *
 *  @code
 *  OADProtocol_Params_t OADProtocol_Params = {
 *    pCallbacks;          // Application Callbacks for pressing packets
 *  };
 *  @endcode
 *
 *
 *  ## Initializing the OADProtocol Module ##
 *
 *  OADProtocol_init() must be called before any other OADProtocol APIs.
 *  This function
 *  iterates through the elements of the SPI_config[] array, calling
 *  the element's device implementation SPI initialization function.
 *
 *
 *  ## Over the Air Download packets ##
 *
 *  The OADProtocol supports OAD messages used to update the FW of a sensor
 *  node. The OAD is instigated by the concentrator sending
 *  OADProtocol_PACKET_TYPE_OAD_IMG_IDENTIFY_REQ packet containing the image
 *  header. The sensor node checks the image header and sends a
 *  OADProtocol_PACKET_TYPE_OAD_IMG_IDENTIFY_RSP containing the status, which
 *  is true f the age header is accepted or false if it is rejected. If the
 *  image header is accepted the sensor node requests the FW image blocks with
 *  the OADProtocol_PACKET_TYPE_OAD_BLOCK_REQ and the concentrator sends the
 *  image blocks with OADProtocol_PACKET_TYPE_OAD_BLOCK_RSP.
 *
 *  The message flow is:
 *
 *  Server                           Client
 *
 * (Optional FW version Req)
 *   FW_VERSION_REQ ----------------------->
 *       <-------------------------- FW_VERSION_RSP("rfWsnNode v03.01.00")
 *
 *   OAD_IMG_IDENTIFY_REQ ----------------->
 *       <-------------------------- OAD_IMG_IDENTIFY_RSP(status=1)
 *       <-------------------------- OAD_BLOCK_REQ(block=0)
 *   OAD_BLOCK_RSP(Block 0) --------------->
 *       <-------------------------- OAD_BLOCK_REQ(block=1)
 *   OAD_BLOCK_RSP(Block 1) --------------->
 *         ...
 *       <-------------------------- OAD_BLOCK_REQ(block=n)
 *   OAD_BLOCK_RSP(Block n) --------------->
 *
 *
 *******************************************************************************
 */
#ifndef OADProtocol_H_
#define OADProtocol_H_

#include "stdint.h"
#include "common/native_oad/oad_target.h"

/**
 *  @defgroup OADProtocol_PACKET packet defines
 *  @{
 */

/*!
 * retry timeouts and max number or retries
 */
#define OADProtocol_DEFAULT_MAX_RETRIES 2 ///< Max number of retries for timed out packets
/**
 *  @defgroup Other OADProtocol defines
 *  @{
 */
#define OADProtocol_FW_VERSION_STR_LEN                 32 ///< Max Length of the FW version string

#define OADProtocol_PKT_CMDID_OFFSET   0   ///< Offset to packet type files in packet header

#define OADProtocol_IMAGE_ID_LEN 16

/*!
 * Req/Rsp Packet Types
 */
#define OADProtocol_PACKET_TYPE_FW_VERSION_REQ          0x00 ///< Firmware version request
#define OADProtocol_PACKET_TYPE_FW_VERSION_REQ_LEN      1 ///< Firmware version request size

#define OADProtocol_PACKET_TYPE_FW_VERSION_RSP          0x01 ///< Firmware version response
#define OADProtocol_PACKET_TYPE_FW_VERSION_RSP_LEN      1 + OADProtocol_FW_VERSION_STR_LEN ///< Firmware version response
#define OADProtocol_VER_RSP_VERSIONSTRING_OFFSET    1   ///< Offset to version string in FW Ver Response

#define OADProtocol_PACKET_TYPE_OAD_IMG_IDENTIFY_REQ                0x02 ///< OAD update image identify request
#define OADProtocol_PACKET_TYPE_OAD_IMG_IDENTIFY_REQ_LEN            1 + 1 + 16 ///< OAD update image identify request
#define OADProtocol_IMG_IDENTIFY_REQ_IMG_ID_OFFSET    1   ///< Offset to status in Image Identify Response
#define OADProtocol_IMG_IDENTIFY_REQ_IMG_HDR_OFFSET   2   ///< Offset to status in Image Identify Response

#define OADProtocol_PACKET_TYPE_OAD_IMG_IDENTIFY_RSP                0x03 ///< OAD update image identify response
#define OADProtocol_PACKET_TYPE_OAD_IMG_IDENTIFY_RSP_LEN            1 + 1 ///< OAD update image identify response
#define OADProtocol_IMG_IDENTIFY_RSP_STATUS_OFFSET   1   ///< Offset to status in Image Identify Response

#define OADProtocol_PACKET_TYPE_OAD_BLOCK_REQ         0x04 ///< OAD update image block request
#define OADProtocol_PACKET_TYPE_OAD_BLOCK_REQ_LEN     1 + 1 + 2 + 2 ///< OAD update image block request
#define OADProtocol_BLOCK_REQ_IMG_ID_OFFSET           1   ///< Offset to 16B block num in Block Request
#define OADProtocol_BLOCK_REQ_BLOCK_NUM_OFFSET        2   ///< Offset to 16B block num in Block Request
#define OADProtocol_BLOCK_REQ_MULTI_BLOCK_SIZE_OFFSET 4   ///< Offset to 16B block num in Block Request

#define OADProtocol_PACKET_TYPE_OAD_BLOCK_RSP         0x05 ///< OAD update image block request
#define OADProtocol_PACKET_TYPE_OAD_BLOCK_RSP_LEN     1 + 1 + 2 + OAD_BLOCK_SIZE ///< OAD update image block request
#define OADProtocol_BLOCK_RSP_IMG_ID_OFFSET           1   ///< Offset to 16B block num in Block Request
#define OADProtocol_BLOCK_RSP_BLOCK_NUM_OFFSET        2   ///< Offset to 16B block num in Block Request
#define OADProtocol_BLOCK_RSP_BLOCK_DATA_OFFSET       4   ///< Offset to block data in Block Response


/** @}*/

/// OADProtocol status codes
typedef enum {
    OADProtocol_Status_Success, ///< Success
    OADProtocol_Failed, ///< Fail
    OADProtocol_FailedTimeout, ///< Acknowledgment or Response Timed out
    OADProtocol_FailedCanceled, ///< Canceled by application
} OADProtocol_Status_t;

/** @brief firmware version request packet callback function type
 *
 */
typedef void (*fwVersionReqCb_t)(void* pSrcAddr);

/** @brief firmware version response packet callback function type
 *
 */
typedef void (*fwVersionRspCb_t)(void* pSrcAddr, char *fwVersionStr);

/** @brief OAD image identify request packet callback function type
 *
 */
typedef void (*oadImgIdentifyReqCb_t)(void* pSrcAddr, uint8_t imgId, uint8_t *imgMetaData);

/** @brief OAD image identify response packet callback function type
 *
 */
typedef void (*oadImgIdentifyRspCb_t)(void* pSrcAddr, uint8_t status);

/** @brief OAD image block request packet callback function type
 *
 */
typedef void (*oadBlockReqCb_t)(void* pSrcAddr, uint8_t imgId, uint16_t blockNum, uint16_t multiBlockSize);

/** @brief OAD image block response packet callback function type
 *
 */
typedef void (*oadBlockRspCb_t)(void* pSrcAddr, uint8_t imgId, uint16_t blockNum, uint8_t *blkData);

/** @brief OADProtocol callback table
 *
 */
typedef struct
{
    fwVersionReqCb_t      pfnFwVersionReqCb; ///< Incoming FW Req
    fwVersionRspCb_t      pfnFwVersionRspCb; ///< Incoming FW Version Rsp
    oadImgIdentifyReqCb_t pfnOadImgIdentifyReqCb; ///< Incoming Image Identify Req
    oadImgIdentifyRspCb_t pfnOadImgIdentifyRspCb; ///< Incoming Image Identify Rsp
    oadBlockReqCb_t       pfnOadBlockReqCb; ///< Incoming OAD Block Req
    oadBlockRspCb_t       pfnOadBlockRspCb; ///< Incoming OAD Block Rsp
} OADProtocol_MsgCBs_t;

/** @brief function definition for sending message over the radio
 *
 */
typedef void* (*radioAccessAllocMsg_t)(uint32_t size);

/** @brief function definition for sending message over the radio
 *
 */
typedef OADProtocol_Status_t (*radioAccessPacketSend_t)(void* pDstAddress, uint8_t *payload, uint32_t msgSize);

/** @brief OADProtocol radio access functions
 *
 */
typedef struct
{
    radioAccessAllocMsg_t        pfnRadioAccessAllocMsg;   ///< Function for allocating a message buffer
    radioAccessPacketSend_t      pfnRadioAccessPacketSend; ///< Function for sending message over the radio
} OADProtocol_RadioAccessFxns_t;

/** @brief RF parameter struct
 *  RF parameters are used with the OADProtocol_open() and OADProtocol_Params_init() call.
 */
typedef struct {
    OADProtocol_RadioAccessFxns_t  *pRadioAccessFxns;    ///< Radio access function table
    OADProtocol_MsgCBs_t           *pProtocolMsgCallbacks;          ///< Application Callbacks for pressing packets
} OADProtocol_Params_t;

/** @brief  Function to initialize the OADProtocol_Params struct to its defaults
 *
 *  @param  params      An pointer to RF_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *     pRadioAccessFxns     = {0}
 *      pCallbacks          = {0}
 */
extern void OADProtocol_Params_init(OADProtocol_Params_t *params);

/** @brief  Function that initializes the Wsn Protocol Task and creates all TI-RTOS objects
 *
 */
extern void OADProtocol_init(void);

/** @brief  Function to open the OADProtocol module
 *
 *  @param  params      An pointer to RF_Params structure for initialization
 */
extern void OADProtocol_open(OADProtocol_Params_t *params);

/** @brief  Function to parse OADProtocol packets
 *
 *  @param  srcAddr             address of the device that sent the message
 *  @param  incomingPacket      pointer to packet to be parsed
 *  @param  packetLen           length of the message
 */
extern OADProtocol_Status_t OADProtocol_ParseIncoming(void* pSrcAddr, uint8_t* incomingPacket);


/** @brief  Function to send a FW version request packet
 *
 *  @param  dstAddress          Address to send the request to
 *
 *  @return                     Status
 */
extern OADProtocol_Status_t OADProtocol_sendFwVersionReq(void* pDstAddress);

/** @brief  Function to send a FW version response packet
 *
 *  @param  dstAddress          Address to send the response to
 *  @param  fwVersion           Firmware version string to send
 *
 *  @return                     Status
 */
extern OADProtocol_Status_t OADProtocol_sendFwVersionRsp(void* pDstAddress, char *fwVersion);

/** @brief  Function to send an OAD image identify request packet
 *
 *  @param  dstAddress          Address to send the request to
 *  @param  imgId               image ID used for requesting image blocks
 *  @param  pImgInfoData        Image header
 *
 *  @return                     Status
 */
extern OADProtocol_Status_t OADProtocol_sendImgIdentifyReq(void* pDstAddress, uint8_t imgId, uint8_t *pImgInfoData);

/** @brief  Function to send an OAD image identify request packet
 *
 *  @param  dstAddress          Address to send the response to
 *  @param  status              status to send
 *
 *  @return                     Status
 */
extern OADProtocol_Status_t OADProtocol_sendOadIdentifyImgRsp(void* pDstAddress, uint8_t status);

/** @brief  Function to send an OAD block request packet
 *
 *  @param  dstAddress          Address to send the request to
 *  @param  imgId               image ID of image blocks
 *  @param  blockNum            block Number to request
 *  @param  multiBlockSize      Numer of blocks in the multi Block transfer (0 or 1 for none-multiblock)
 *
 *  @return                     Status
 *
 */
extern OADProtocol_Status_t OADProtocol_sendOadImgBlockReq(void* pDstAddress, uint8_t imgId, uint16_t blockNum, uint16_t multiBlockSize);

/** @brief  Function to send an OAD block response packet
 *
 *  @param  dstAddress          Address to send the response to
 *  @param  imgId               image ID of image blocks
 *  @param  blockNum            Block number
 *  @param  block               pointer to image block
 *
 *  @return                     Status
 *
 */
extern OADProtocol_Status_t OADProtocol_sendOadImgBlockRsp(void* pDstAddress, uint8_t imgId, uint16_t blockNum, uint8_t *block);

#endif /* OADProtocol_H_ */
