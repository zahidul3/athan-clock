//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "CircularBuffer.h"
/*
This code implements a simpile circular buffer for marshaling bytes from an 
interrupt routine to main loop and vice versa.

Would be good to add an event to call main loop function on
x bytes or time.

could also add some checking on state of buffer.

RE_ENTRANT
Because the only shared variable operations on .Count are atomic (completed in one instruction)
interrupts do not need to be disabled!

*/
//-----------------------------------------------------------------------------
void IntPutCharCircBuf(TsCircularBuffer * b, uint8_t c)
{
  if(b->Count < b->BufferSize)
  {
    b->Buf[b->InIndex++] = c;
    if(b->InIndex>=b->BufferSize) b->InIndex = 0;
    b->Count++;  
  }
}  
//-----------------------------------------------------------------------------
uint16_t IntGetCharCircBuf(TsCircularBuffer * b)  // high byte is 0 if no char
{
  if(b->Count)
  {
    uint16_t c = 0x100 + b->Buf[b->OutIndex++];
    if(b->OutIndex>=b->BufferSize) b->OutIndex = 0;
    b->Count--;  
    return c;
  }
  else return 0;
}  
//-----------------------------------------------------------------------------
void PutCharCircBuf(TsCircularBuffer * b, uint8_t c)
{
  IntPutCharCircBuf(b, c);
}  
//-----------------------------------------------------------------------------
uint16_t GetCharCircBuf(TsCircularBuffer * b)
{
  return IntGetCharCircBuf(b);
}  
//-----------------------------------------------------------------------------
uint16_t CircBufFreeSpace(TsCircularBuffer * b)
{
  return b->BufferSize - b->Count;
}  
//-----------------------------------------------------------------------------
uint16_t CircBufUsedSpace(TsCircularBuffer * b)
{
  return b->Count;
}  
//-----------------------------------------------------------------------------
void InitCircBuf(TsCircularBuffer * b, uint16_t BufferSize)
{
  b->Buf = calloc(BufferSize, 1);
  b->BufferSize = BufferSize;
  b->Count = 0;
  b->InIndex = 0;
  b->OutIndex = 0;
}  
//-----------------------------------------------------------------------------
void DeleteCircBuf(TsCircularBuffer * b)
{
  free(b->Buf);
  b->Buf = 0;
  b->BufferSize = 0;
  b->Count = 0;
}  
//-----------------------------------------------------------------------------
