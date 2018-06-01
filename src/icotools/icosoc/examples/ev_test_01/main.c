#include <stdio.h>
#include <stdint.h>
#include "icosoc.h"

#define MAX_EVENTS 16

int console_getc(void)
{
	while (1) {
		int c = *(volatile uint32_t*)0x30000000;
		if (c >= 0) return c;
	}
}

/*
static void spiflash_begin()
{
	*(volatile uint32_t*)0x20000004 &= ~8;
}

static void spiflash_end()
{
	*(volatile uint32_t*)0x20000004 |= 8;
}

static uint8_t spiflash_xfer(uint8_t value)
{
	*(volatile uint32_t*)0x20000008 = value;
	return *(volatile uint32_t*)0x20000008;
}

*/

void waitn(int n) 
{
	for (int i = 0; i < n; i++)
		asm volatile("");
}





__attribute__ ((section (".text.sram"))) int main(void) 
{
	
	printf("EvR 0.1\r\n");

	// test fifo
	uint64_t fifo0 = ~0;
	icosoc_tr0_set_fifo(fifo0);
	fifo0 = 0x0102030405060708;
	icosoc_tr0_set_fifo(fifo0);
	
	fifo0 = icosoc_tr0_get_fifo();
	printf("fifo0: 0x%016llx\r\n", fifo0);
	
	fifo0 = icosoc_tr0_get_fifo();
	printf("fifo0: 0x%016llx\r\n", fifo0);	
	
	fifo0 = icosoc_tr0_get_fifo();
	printf("fifo0: 0x%016llx\r\n", fifo0);
	
	// reset counter after boot
	uint64_t counter;
	icosoc_tr0_set_counter(0);
	counter = icosoc_tr0_get_counter();
	printf("counter @ 0x%016llx\r\n", counter);


	// setup debug and general purpose gpios as output
	icosoc_gpio_dir(0xffff);

	// leds: status: up
	icosoc_leds(0x01);
	
	
	// setup events array 
	uint64_t events[MAX_EVENTS] = { ~0 };

	// main loop	
	uint32_t cmd, event[2], event_count = 0;	
	
	for (uint8_t i = 0; ; i++) {
		
		cmd = icosoc_spi0_xfer(0);
		
		//printf("spi0: command: %08lx\r\n", cmd);
		
		switch (cmd) 
		{
			case 1:
				
				// receive new event
				event[0] = icosoc_spi0_xfer(cmd);
				event[1] = icosoc_spi0_xfer(event[0]);
				icosoc_spi0_xfer(event[1]);
			
				uint32_t event_id = (event[0] >> 24) & 0xFF;	
				
				if (event_id < MAX_EVENTS) 
				{
					uint64_t event64 = event[0]; 
					events[event_id] = (event64 << 32 ) | event[1];
				       	icosoc_tr0_set_trigger((uint8_t) event_id, event64);
					event64 = icosoc_tr0_get_trigger((uint8_t) event_id);	
					printf("spi0: got new event configuration %d: %016llx\r\n", event_id, events[event_id]);
				} 
				else
				{
					printf("spi0: got invalid event id for tirgger configuration!\r\n");
				}
				
				break;

			default:
				fifo0 = icosoc_tr0_get_fifo();
				if (fifo0  != 0 )
					printf("fifo0: 0x%016llx\r\n", fifo0);		
				break;
		}
	
	}
		
}


