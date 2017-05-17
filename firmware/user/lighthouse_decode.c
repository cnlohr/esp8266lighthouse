#include "lighthouse_decode.h"
#include <esp82xxutil.h>

uint32_t lighthousebuffer[MAX_LHBUFF+1];
uint32_t lighthousebufferlen;
uint8_t  lighthousebufferflag;



void lighthouse_decode( uint32_t * data, int size_words )
{
	int i;
	for( i = 0; i < size_words; i++ )
	{
		uint32_t r = data[i];
		if( data[i] != 0 && data[i] != 0xffffffff )
		{
			if( lighthousebufferflag == 0 )
			{
				lighthousebufferflag = 1;
				lighthousebufferlen = 1;
			}
			if( lighthousebufferflag == 1 )
			{
				if( lighthousebufferlen < MAX_LHBUFF )
					lighthousebuffer[lighthousebufferlen++] = r;		
			}
		}
		else
		{
			if( lighthousebufferflag == 1 )
			{
				if( lighthousebufferlen < 5 )
				{
					lighthousebufferflag = 0;
					lighthousebufferlen = 1;
				}
				else
				{
					lighthousebuffer[0] = lighthousebufferlen;
					if( SendPacket( lighthousebuffer, lighthousebufferlen ) == 0 )
					{
						lighthousebufferflag = 0;
					}
					else
					{
						lighthousebufferflag = 2;
					}
				}
			}
		}
	}
}

