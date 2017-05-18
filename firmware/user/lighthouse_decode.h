#ifndef _LIGHTHOUSE_DECODE_H
#define _LIGHTHOUSE_DECODE_H

#include <stdint.h>

//FI2S = 40,000,000 Hz

#define MAX_LHBUFF 256
#define MAX_EDGES 512

//XXX Tricky: On Xtensa, by keeping everything in one global structure,
// it is actually much faster because its location need take only one
// register and you can offset from that register quickly.  Individual
// global registers are slow! because they require a LR.
//  P.S. Be sure to make this word-aligned.
struct LHSM_type
{
	const unsigned short * carrierlut;  //32-bit boundary

	uint16_t defaultstate;
	uint16_t statemachinestate;         //32-bit boundary

	uint32_t timebase; 					 //Current tick time (in FI2S)

	uint32_t * edgetimesbase;
	uint32_t * edgetimeshead;

	uint16_t edgecount; 
	uint8_t  debugmonitoring;
	uint8_t  debugbufferflag;           //32-bit boundary

	uint32_t debugbufferlen;			//Only updated at end of packet.

	uint32_t * debugbufferbase;
	uint32_t * debugbufferhead;
};

extern struct LHSM_type LHSM;


//For decidng "optical data" from the lighthouses. 
// In particular, modulated 1.8 MHz data as it comes in off
// of the I2S bus.

void lighthouse_decode( uint32_t * data, int size_words );
void lighthouse_setup(); //Call this first.
int SendPacket( uint32_t * data, int size );

#endif
