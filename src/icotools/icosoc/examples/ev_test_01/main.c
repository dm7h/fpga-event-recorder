#include <stdio.h>
#include <stdint.h>
#include "icosoc.h"

/*
static int console_getc()
{
	while (1) {
		int c = *(volatile uint32_t*)0x30000000;
		if (c >= 0) return c;
	}
}


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

int main()
{
	
	printf("reading conf data .. \r\n");
	uint8_t conf_buffer[4];
	
	icosoc_leds(0xaa);
	icosoc_ser0_write("ELO\r\n", 5);
	//for (unit8_t i = 0; i < 4; i++)
	//{
	//	icosoc_ser0_read(&conf_buffer, 4);
	//}

	printf("got %x \r\n", *(uint32_t *) conf_buffer);
	
	icosoc_tr0_set_config(*(uint32_t *) conf_buffer);
	
	uint32_t conf = icosoc_tr0_get_config();

	printf("got %x back from trigger module \r\n", conf);

	uint32_t trig, trig_old, cnt1, cnt2;
	//uint64_t counter;

	// main loop
	for (uint8_t i = 1; ; i++)
	{
		icosoc_leds(i);
		// debug print
		
		
		//spiflash_end();
		//spiflash_begin();
		
		char buffer[100];
		int buffer_len;

		//uint8_t r_in = 2;

		//read from uart
		//icosoc_ser0_read(&r_in, 1);
		//buffer_len = snprintf(buffer, 100, "[%02x]\r\n", i);

		trig_old = trig;
		trig = icosoc_tr0_get_trig();	
		
		if (trig != trig_old)
		{
			cnt1 = icosoc_tr0_get_cnt1();
			cnt2 = icosoc_tr0_get_cnt2();
			buffer_len = snprintf(buffer, 100, "[%x %x] %x\r\n", cnt1, cnt2, trig);
			icosoc_ser0_write(buffer, buffer_len);
			printf(buffer);
		}
		
		
		//icosoc_ser0_write(buffer, buffer_len);
		

		//char c = console_getc(); //spiflash_xfer(i);
		//spiflash_end();
		//char buffer = '0';
		//icosoc_ser0_read(&buffer, 1);
		//spiflash_begin();
		//spiflash_xfer(c);
		//spiflash_end();
		// sleep for a bit ..
		//for (int i = 0; i < 1000000; i++)
		//	asm volatile ("");

	}
}

