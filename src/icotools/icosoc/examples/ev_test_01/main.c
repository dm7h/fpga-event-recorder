#include <stdio.h>
#include <stdint.h>
#include "icosoc.h"

#define  MAX_EVENT_TRIGGERS 16


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
	
	// general purpose gpios ;)
	icosoc_gpio_dir(0xffff);

	// leds: status: up
	icosoc_leds(0x01);
	
	// setup uart buffers
	uint8_t r_buffer[8] = { 0 }; // 8 byte 
	uint8_t t_buffer[8] = { 0 };
	uint64_t rbuf2[16] = { 0 };
	
	uint8_t sram[100] = "HALLO SRAM, WO BIST DU?!";

	uint64_t events[16] = {
		0x0001000002ffffff, 
		0x0102000003ffffff, 
		0x020020ffffffffff, 
		0x03045fffffffffff, 
		0x040a7fffffffffff, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF, 
		0xFFFFFFFFFFFFFFFF 
	};

	// event trigger configuration 
	uint64_t event_trigger_config[MAX_EVENT_TRIGGERS];

	//console_getc();

	// wait for new configuration (with timeout)
	
	uint32_t stat = 0xAA, ch, ch_last = 0x01020304;

	/* gpio toggle "benchmark"	
	for (uint8_t i = 0; ; i++) {
		stat = ~stat;
		icosoc_gpio_set(stat);
		printf("stat: %08lx\r\n", stat);
		//waitn(100000);
	}
	*/
	
	
	for (uint8_t i = 0; ; i++) {
		
		icosoc_spi0_xfer(0x010203+i);
		//stat = ~stat;
		//icosoc_gpio_set(stat);
		//if (ch != ch_last)
			//printf("spi0: got %08lx | status: %lx \r\n", ch, stat);
		//ch_last = ch;
	
	}
		
	 /*	
	uint32_t conf = icosoc_tr0_get_config();

	printf("got %lx back from trigger0 module \r\n", conf);

	uint32_t trig, trig_old, cnt1, cnt2;
	//uint64_t counter;
	
	uint8_t stat = 0;
	// main loop
	for (uint8_t i = 1; ; i++)
	{
		icosoc_leds(i);
		
		char buffer[100];
		int buffer_len;

		trig_old = trig;
		trig = icosoc_tr0_get_trig();	
		
		if (trig != trig_old)
		{
			cnt1 = icosoc_tr0_get_cnt1();
			cnt2 = icosoc_tr0_get_cnt2();
			buffer_len = snprintf(buffer, 100, "[%032lx] %lx\r\n", cnt1, trig);
			printf(buffer);
	
			for (int j = 0; j < 16; j++) {
				if (~events[j] == 0) continue;
				uint8_t id = events[j] >> 56;
				uint8_t flag = events[j] >> 48;
				uint8_t trigger[16] = { 0 };
				uint8_t match[16];
				for (int k = 0; k < 16; k++) {
					trigger[k] = events[j] >> ((15 - k) * 3) & 7;
					uint8_t bit = (trig >> (15 - k)) & 1;
					uint8_t bit_old = (trig_old >> (15 - k)) & 1;
					match[k] = 0;
					if (trigger[k] == 7) match[k] = 1;
					if (trigger[k] == 0 && ~bit && ~bit_old) match[k] = 1; //0
					if (trigger[k] == 3 && bit && bit_old) match[k] = 1; //1
					if (trigger[k] == 2 && bit && ~bit_old) match[k] = 1; //down
					if (trigger[k] == 1 && ~bit && bit_old) match[k] = 1; //up
				}
				for (int i = 0; i < 16; i++) {
					buffer_len = snprintf(buffer, 100, "[%d:%d],", trigger[i], match[i]);
					printf(buffer);
						
				}
				buffer_len = snprintf(buffer, 100, "\r\n");
				printf(buffer);

				//icosoc_ser0_write(buffer, buffer_len);
			}
		}
		
	}
	*/
}

/*
int main(void) {
	return run();
};
*/
