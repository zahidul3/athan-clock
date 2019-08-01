
#ifndef __CIRCULAR_H__
#define __CIRCULAR_H__

typedef struct
{
  uint8_t   *Buf;
  uint16_t   BufferSize;
  uint16_t   InIndex;
  uint16_t   OutIndex;
  uint16_t   Count;
} __attribute__((__packed__)) TsCircularBuffer;

void PutCharCircBuf(TsCircularBuffer * b, uint8_t c);
uint16_t GetCharCircBuf(TsCircularBuffer * b);

void IntPutCharCircBuf(TsCircularBuffer * b, uint8_t c);
uint16_t IntGetCharCircBuf(TsCircularBuffer * b);

void InitCircBuf(TsCircularBuffer * b, uint16_t BufferSize);
void DeleteCircBuf(TsCircularBuffer * b);

uint16_t CircBufFreeSpace(TsCircularBuffer * b);
uint16_t CircBufUsedSpace(TsCircularBuffer * b);

#endif

