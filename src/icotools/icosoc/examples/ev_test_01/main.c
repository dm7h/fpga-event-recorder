#include <stdio.h>
#include <stdint.h>
#include "icosoc.h"

#define MAX_EVENTS 8

/* FIFO buffer, see: https://www.mikrocontroller.net/articles/FIFO */
#define BUFFER_FAIL     0
#define BUFFER_SUCCESS  1
 
#define BUFFER_SIZE 2048 // max at current config: 15997 // must be 2^n  (8, 16, 32, 64 ...)
#define BUFFER_MASK (BUFFER_SIZE-1) 

__attribute__ ((section (".text.buffer"))) struct Buffer {
  uint64_t data[BUFFER_SIZE];
  uint64_t read; // always points to the oldest entry
  uint64_t write; // always point to a empty entry
} buffer = {{}, 0, 0};


/*
 * write entry to ring buffer
 *
 * returns:
 *    BUFFER_FAIL       buffer is full
 *    BUFFER_SUCCESS    saved successfully
 */

__attribute__ ((section (".text.sram"))) uint8_t BufferIn(uint64_t byte)
{
  uint64_t next = ((buffer.write + 1) & BUFFER_MASK);

  if (buffer.read == next)
    return BUFFER_FAIL; // voll

  buffer.data[buffer.write] = byte;
  // buffer.data[buffer.write & BUFFER_MASK] = byte; // absolutely save
  buffer.write = next;

  return BUFFER_SUCCESS;
}

/*
 * get entry from ring buffer, if there is at least one
 *
 * returns:
 *     BUFFER_FAIL       buffer is empty
 *     BUFFER_SUCCESS    returned one entry
 */

__attribute__ ((section (".text.sram")))  uint8_t BufferOut(uint64_t *pByte)
{
  if (buffer.read == buffer.write)
    return BUFFER_FAIL;

  *pByte = buffer.data[buffer.read];

  buffer.read = (buffer.read+1) & BUFFER_MASK;

  return BUFFER_SUCCESS;
}

/* wait */
void waitn(int n) 
{
	for (int i = 0; i < n; i++)
		asm volatile("");
}

/* main loop: make sure we put this into sram for faster execution */
__attribute__ ((section (".text.sram"))) int main(void) 
{
	
	printf("EvR 0.1\r\n");
	icosoc_tr0_set_status(3);

	// test fifo
	uint64_t fifo0 = ~0;
	icosoc_tr0_set_fifo(fifo0);
	fifo0 = icosoc_tr0_get_fifo();
	if (fifo0 == ~0)
		printf("fifo0: ready\r\n");	
	

	
	// reset counter after boot
	uint64_t counter;
	icosoc_tr0_set_counter(0);
	counter = icosoc_tr0_get_counter();
	printf("counter @ 0x%016llx\r\n", counter);



	// setup debug and general purpose gpios as output
	//icosoc_gpio_dir(0xffff);

	// leds register
	uint8_t leds_reg = 0;

	// setup events array 
	uint64_t events[MAX_EVENTS] = { ~0 }; // note: fpga registers are already initialized with 0xFFs

	// main loop	
	uint32_t cmd, event[2], event_count = 0;	
	uint64_t buffer0;
	uint8_t stat_buffer = 0, stat_fifo = 0, stat_rec = 0;
	
	for (uint8_t i = 0; ; i++) {
		// set leds
		icosoc_leds(((stat_buffer > 0)? 4: 0) + ((stat_fifo > 0)? 2 : 0) + ((stat_rec > 0)? 1 : 0));
		
		// push events from BRAM fifo to SRAM buffer 
		fifo0 = icosoc_tr0_get_fifo(0);
		if (fifo0 != 0) {
			//printf("fifo0: 0x%016llx\r\n", fifo0);
			stat_buffer += BufferIn(fifo0)? 0 : 1;; // add error to status 
			if (stat_buffer) printf("sram buffer overlow!\r\b");
			continue; // continue until we got all events	
		}
		cmd = icosoc_spi0_xfer(0);
		
		//printf("spi0: command: %08lx\r\n", cmd);
		
		switch (cmd) 
		{
			case 1:
				
				// receive new event configuration
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
					printf("spi0: amount of event configurations exceeded!\r\n");
				}
				
				break;

			case 2:
				// reset counter
				icosoc_tr0_set_counter(0);
				// clear fifo
				while (icosoc_tr0_get_fifo() != 0)
					icosoc_tr0_get_fifo();
				// clear buffer
				while (BufferOut(&buffer0));
				
				// update stat & leds
				icosoc_tr0_set_status(1);
			        stat_rec = 1;	

				// send some xFF's to signal that we're done
				//for (uint8_t i = 0; i < 4; i++)
					icosoc_spi0_xfer(~0);
				printf("spi0: got start command: counter reset and fifo cleared\r\n");
				break;

			case 3:
				// stop recording
				icosoc_tr0_set_status(0);
				stat_rec = 0;
				printf("spi0: got stop command");
				break;

			default:
				// send data
				if (BufferOut(&buffer0)) {
					printf("buffer0: 0x%016llx\r\n", buffer0);
					icosoc_spi0_xfer(buffer0 >> 32);		
					icosoc_spi0_xfer(buffer0);
				}		
				break;
		}
	
	}
		
}


