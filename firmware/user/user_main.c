//Copyright 2015 <>< Charles Lohr, see LICENSE file.

#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "ets_sys.h"
#include <uart.h>
#include "osapi.h"
#include "espconn.h"
#include "i2sduplex.h"
#include <commonservices.h>
#include "lighthouse_decode.h"
#include <esp82xxutil.h>
#include <mdns.h>

#define PORT 7777

#define procTaskPrio        0
#define procTaskQueueLen    1

static volatile os_timer_t some_timer;
static struct espconn *pUdpServer;


//int ICACHE_FLASH_ATTR StartMDNS();

void user_rf_pre_init(void)
{
	//nothing.
}


//Tasks that happen all the time.

os_event_t    procTaskQueue[procTaskQueueLen];


static void ICACHE_FLASH_ATTR procTask(os_event_t *events)
{
	CSTick( 0 );
	system_os_post(procTaskPrio, 0, 0 );
}

remot_info *premot_udp = NULL;

int SendPacket( uint32_t * data, int size )
{
	if( premot_udp )
	{
		pUdpServer->proto.udp->remote_port = premot_udp->remote_port;
		pUdpServer->proto.udp->remote_ip[0] = premot_udp->remote_ip[0];
		pUdpServer->proto.udp->remote_ip[1] = premot_udp->remote_ip[1];
		pUdpServer->proto.udp->remote_ip[2] = premot_udp->remote_ip[2];
		pUdpServer->proto.udp->remote_ip[3] = premot_udp->remote_ip[3];
		data[0]--;
		espconn_sendto(pUdpServer, (char*)data, size*4 );
		data[0]++;
		return 0;
	}
	else
	{
		return 1;
	}
}

//Timer event.
static void ICACHE_FLASH_ATTR myTimer(void *arg)
{
	//printf( "%d %d %d %08x %08x\n", fxcycle, etx, erx, i2sBDRX[0], i2sBDRX[I2SDMABUFLEN-1] );
	//uart0_sendStr("X");


	CSTick( 1 );
}


//Called when new packet comes in.
static void ICACHE_FLASH_ATTR
udpserver_recv(void *arg, char *pusrdata, unsigned short len)
{
	struct espconn *pespconn = (struct espconn *)arg;
	espconn_get_connection_info(pespconn,&premot_udp,0);
	lighthousebufferflag = 0;
	uart0_sendStr("X");
}

void ICACHE_FLASH_ATTR charrx( uint8_t c )
{
	//Called from UART.
}

void ICACHE_FLASH_ATTR user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
//	ets_delay_us(200000 );
	uart0_sendStr("\r\nesp8266 ws2812 driver\r\n");

//	int opm = wifi_get_opmode();
//	if( opm == 1 ) need_to_switch_opmode = 120;
//	wifi_set_opmode_current(2);
//Uncomment this to force a system restore.
//	system_restore();

	CSSettingsLoad( 0 );
	CSPreInit();

    pUdpServer = (struct espconn *)os_zalloc(sizeof(struct espconn));
	ets_memset( pUdpServer, 0, sizeof( struct espconn ) );
	espconn_create( pUdpServer );
	pUdpServer->type = ESPCONN_UDP;
	pUdpServer->proto.udp = (esp_udp *)os_zalloc(sizeof(esp_udp));
	pUdpServer->proto.udp->local_port = 7777;
	espconn_regist_recvcb(pUdpServer, udpserver_recv);
	espconn_set_opt(pUdpServer, 0x04); // enable write buffer

	if( espconn_create( pUdpServer ) )
	{
		while(1) { uart0_sendStr( "\r\nFAULT\r\n" ); }
	}

	CSInit();

	SetServiceName( "lighthouse" );
	AddMDNSName( "cn8266" );
	AddMDNSName( "ws2812" );
	AddMDNSService( "_http._tcp", "An ESP8266 Webserver", 80 );
	AddMDNSService( "_cn8266._udp", "ESP8266 Backend", 7878 );

	//Add a process
	system_os_task(procTask, procTaskPrio, procTaskQueue, procTaskQueueLen);

	//Timer example
	os_timer_disarm(&some_timer);
	os_timer_setfn(&some_timer, (os_timer_func_t *)myTimer, NULL);
	os_timer_arm(&some_timer, 100, 1);

	//Configure 
	testi2s_init();

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);

	PIN_DIR_OUTPUT = _BV(13) | _BV(14);
	PIN_OUT_SET = _BV(13) | _BV(14);

	//Configure the charge pump pins.

	system_update_cpu_freq( SYS_CPU_160MHZ );

	system_os_post(procTaskPrio, 0, 0 );
}


//There is no code in this project that will cause reboots if interrupts are disabled.
void EnterCritical()
{
}

void ExitCritical()
{
}


