#include <stdio.h>
#include <stdint.h>
#include "icosoc.h"


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



int main()
{
	
	// main loop
	for (uint8_t i = 1;; i++)
	{
		icosoc_leds(i);
		// debug print
		
		
		//spiflash_end();
		//spiflash_begin();
		
		char c = console_getc(); //spiflash_xfer(i);
		//spiflash_end();
		printf("got: %c", (c != 0)? c : '.'); 
		//spiflash_begin();
		//spiflash_xfer(c);
		//spiflash_end();
		// sleep for a bit ..
		for (int i = 0; i < 1000000; i++)
			asm volatile ("");

	}
}

