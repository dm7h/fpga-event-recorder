#include <stdio.h>
#include <stdint.h>
#include "icosoc.h"

int main()
{
	
	// main loop
	for (uint8_t i = 0;; i++)
	{
		icosoc_leds(i);
		
		// debug print
		printf("[%02x] 'elo there!\n", i);

		// sleep for a bit ..
		for (int i = 0; i < 100000; i++)
			asm volatile ("");

	}
}

