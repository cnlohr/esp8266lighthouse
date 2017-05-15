/*
	Data processor for raw pulse data.

	Run this program by piping in esp8266-formatted UDP messages.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

uint32_t Read32()
{
	uint32_t ret = 0;
	ret = getchar();
	ret |= getchar()<<8;
	ret |= getchar()<<16;
	int gc = getchar();
	if( gc == EOF ) { fprintf( stderr, "Broken pipe.\n" ); exit( -1 ); }
	ret |= gc<<24;
	return ret;
}

int main()
{
	while(1)
	{
		int size = Read32();
		int i;
		printf( "Size: %d\n", size );
		for( i = 0; i < size; i++ )
		{
			printf( "%08x ", Read32() );
			//Read32();
		}
		printf("\n" );
	}
}
