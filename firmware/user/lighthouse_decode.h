#ifndef _LIGHTHOUSE_DECODE_H
#define _LIGHTHOUSE_DECODE_H

#include <stdint.h>

#define MAX_LHBUFF 256
extern uint32_t lighthousebuffer[MAX_LHBUFF];
extern uint32_t lighthousebufferlen;
extern uint8_t  lighthousebufferflag;  //0 = ready for data, 1 = receiveing data, 2 = data waiting in packet.

//For decidng "optical data" from the lighthouses. 
// In particular, modulated 1.8 MHz data as it comes in off
// of the I2S bus.

void lighthouse_decode( uint32_t * data, int size_words );

#endif
