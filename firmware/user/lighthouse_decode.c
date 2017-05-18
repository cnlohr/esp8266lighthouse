#include "lighthouse_decode.h"
#include <esp82xxutil.h>

//From lighthouse_i2s_lut.c
extern const unsigned short lighthouse_i2s_lut[512];
uint32_t lighthouse_debugbuffer[MAX_LHBUFF]; 
uint32_t edgeinfo[MAX_EDGES];

struct LHSM_type LHSM;

void ResetStateMachine()
{
	LHSM.statemachinestate = LHSM.defaultstate;
	LHSM.debugbufferhead = LHSM.debugbufferbase;
	LHSM.edgetimeshead = LHSM.edgetimesbase;
}

void lighthouse_setup()
{
	LHSM.carrierlut = lighthouse_i2s_lut;
	LHSM.debugbufferbase = lighthouse_debugbuffer;
	LHSM.edgetimesbase = edgeinfo;

	ResetStateMachine();
}

//"Forced inline." :-p
//This function may look odd.  That's because it is.
//To see what's really going on in here, check lighthouse_clocking.c
#define ProcessMachineState( dat ) \
	{ \
		localstate = (localstate & 0x100) | dat; \
		localstate = LHSM.carrierlut[localstate]; \
		if( localstate & 0x4000 ) { \
			LHSM.timebase += localstate & 0x0f; \
			*(LHSM.edgetimeshead++) = LHSM.timebase; \
		} \
		if( localstate & 0x8000 ) { \
			LHSM.timebase += localstate >> 4 & 0x0f; \
			*(LHSM.edgetimeshead++) = LHSM.timebase; \
		} \
		LHSM.timebase += localstate >> 10 & 0x0f; \
	}


void lighthouse_decode( uint32_t * data, int size_words )
{
	int i;

	//Check to make sure we won't overflow our buffers.  Doing this here speeds us up,
	//rather than having to do it every iteration.
	uint32_t used_buffer = LHSM.debugbufferhead - LHSM.debugbufferbase; //Because of magic C pointer math, this returns elements.
	if( used_buffer >= MAX_LHBUFF - size_words )
	{
		LHSM.debugbufferflag = 3; //3 = abort. (oversized frame)
	}

	
	for( i = 0; i < size_words; i++ )
	{
		uint32_t r = data[i];
		if( data[i] != 0x00000000 && data[i] != 0xffffffff )
		{
			if( LHSM.debugbufferflag == 0 )
			{
				ResetStateMachine();
				LHSM.debugbufferflag = 1;
				LHSM.debugbufferlen = 1;
			}

			if( LHSM.debugbufferflag == 1 )
			{
				*(LHSM.debugbufferhead++) = r;

				uint32_t used_edges = LHSM.edgetimeshead - LHSM.edgetimesbase;

				//Check here instead of in each PSM. (performance speedup)
				if( used_edges >= MAX_EDGES - 9 ) //-9 = each PSM can initiate up to 2 edges.
				{
					LHSM.debugbufferflag = 3;  //Abort: Too many transitions.
					LHSM.timebase += 32;
					continue;
				}

				uint16_t localstate = LHSM.statemachinestate;

				//Actually process the data.
				ProcessMachineState( (r>>24&0xff) );
				ProcessMachineState( (r>>16&0xff) );
				ProcessMachineState( (r>>8&0xff) );
				ProcessMachineState( (r>>0&0xff) );

				LHSM.statemachinestate = localstate;
			}
			else
			{
				LHSM.timebase += 32;
			}
		}
		else
		{
			LHSM.timebase += 32;
	
			if( LHSM.debugbufferflag == 3 )
			{
				LHSM.debugbufferflag = 0;
			}

			if( LHSM.debugbufferflag == 1 )
			{
				//This may look odd, but what's going on 
				//is we find out what the default value is
				//for the line, i.e. bias low or high.
				//Then we can use that for the defualt state.
				LHSM.defaultstate = r & 0x100; 
				LHSM.debugbufferlen = LHSM.debugbufferhead - LHSM.debugbufferbase;

				if( LHSM.debugbufferlen < 5 )
				{
					LHSM.debugbufferflag = 0;
				}
				else
				{
					//Continue the state machine.
					uint16_t localstate = LHSM.statemachinestate;
					ProcessMachineState( r>>24 );	//We don't care about storing the output.  It just needs to finish.


					//All edges are now populated.  We could operate on them.
					int edgecount = LHSM.edgetimeshead - LHSM.edgetimesbase;
					LHSM.edgecount = edgecount;

					printf( "%d\n", edgecount );

					LHSM.debugbufferbase[0] = LHSM.debugbufferlen;
					if( !LHSM.debugmonitoring || SendPacket( LHSM.debugbufferbase, LHSM.debugbufferlen ) == 0 )
					{
						LHSM.debugbufferflag = 0;
					}
					else
					{
						LHSM.debugbufferflag = 2;
					}
				}
			}
		}
	}
}

