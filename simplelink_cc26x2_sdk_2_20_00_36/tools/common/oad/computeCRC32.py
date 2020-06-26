"""
/******************************************************************************
 @file  computeCRC32.py

 @brief This tool is used as a helper file for OAD Image Tool

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
"""
import zlib


def computeCRC32(argv=None):
    '''
    Computes CRC32 over entire file and updates the image header
    '''
    argLen = len (argv)

    if argLen != 3:
        print "'computeCRC32'::Usage: 1:Filename, 2: dataStartOffset, " \
              "3:crcOffset"
        return 1

    fileName = argv[0]
    dataStartOffset = int(argv[1])
    crcOffset = int(argv[2])

    crcbytes = crc32_withOffset(fileName, dataStartOffset)
    updateCRCbytes(fileName, crcbytes, crcOffset)

    return 0


def crc32(fileName):
    '''
    Computes CRC32 over entire file
    '''
    filePtr = open(fileName,"rb")
    content = filePtr.readlines()
    filePtr.close()
    crc = None
    for eachLine in content:
      if not crc:
          crc = zlib.crc32(eachLine)
      else:
          crc = zlib.crc32(eachLine, crc)

    # Return 8 digits crc
    return format((crc & 0xFFFFFFFF), '08x')


def crc32_withOffset(fileName, dataOffset):
    '''
    Computes CRC32 over entire file
    '''
    filePtr = open(fileName,"rb")
    filePtr.seek(dataOffset, 0)  # seek to image data offset file

    content = filePtr.readlines()
    filePtr.close()
    crc = None
    for eachLine in content:
      if not crc:
          crc = zlib.crc32(eachLine)
      else:
          crc = zlib.crc32(eachLine, crc)

    # Return 8 digits crc
    return format((crc & 0xFFFFFFFF), '08x')


def HexToByte(hexStr):
    bytes = []

    hexStr = ''.join( hexStr.split(" ") )

    for i in range(0, len(hexStr), 2):
            bytes.append( chr( int (hexStr[i:i+2], 16 ) ) )

    return ''.join(bytes)


def int2byte(val, width=32):
    return ''.join([chr((val >> 8*n) & 255) for n in reversed(range(width/8))])


def int2byte1(val, width=32):
    bytes = [0] * 4

    for n in reversed(range(width/8)):
        bytes[n] = ((val >> 8*n) & 255)

    return bytes


def updateCRCbytes(fileName, crcbytes, crcOffset):
    x = int(crcbytes, 16)

    crcByteStr = (int2byte1(x))

    # Open file in binary mode
    filePtr = open(fileName, 'r+b')

    # Rewind back to the start
    filePtr.seek(0, 0)
    filePtr.seek(crcOffset, 0)  # Seek to file start offset

    filePtr.write(chr(crcByteStr[0]))
    filePtr.write(chr(int(crcByteStr[1])))
    filePtr.write(chr(int(crcByteStr[2])))
    filePtr.write(chr(int(crcByteStr[3])))

    filePtr.close()

    return
