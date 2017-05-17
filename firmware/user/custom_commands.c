//Copyright 2015 <>< Charles Lohr, see LICENSE file.

#include <commonservices.h>
#include <esp82xxutil.h>
#include "lighthouse_decode.h"
#include <string.h>

extern remot_info *premot_udp;

void ICACHE_FLASH_ATTR ReinitSettings()
{
}

void ICACHE_FLASH_ATTR SettingsLoaded()
{
}


int ICACHE_FLASH_ATTR CustomCommand(char * buffer, int retsize, char *pusrdata, unsigned short len)
{
	char * buffend = buffer;

	switch( pusrdata[1] )
	{
	case 'C': case 'c': //Custom command test
	{
		buffend += ets_sprintf( buffend, "CC" );
		return buffend-buffer;
	}
	case 'P': case 'p': //Get last packet
	{
		buffend += ets_sprintf( buffend, "CP\t%d\t%d", lighthousebufferflag,lighthousebufferlen-1 );
		premot_udp = 0;
		if( lighthousebufferflag == 2 )
		{
			int k;
			buffend+= ets_sprintf( buffend, "\t" );
			int tsend = lighthousebufferlen-1;
			if( tsend > 300 ) tsend = 300;
			//ets_memcpy( buffend, lighthousebuffer, tsend );
			for( k = 1; k <= tsend; k++ )
			{
				uint32_t r = lighthousebuffer[k];
				*(buffend++) = r>>24;
				*(buffend++) = r>>16;
				*(buffend++) = r>>8;
				*(buffend++) = r;
			}
			lighthousebufferflag = 0;
		}
		return buffend-buffer;
	}
	}
	return -1;
}
