/*
 *
 *  icozctl -- programming and configuration tool for the icezero event recorder project
 *
 *  Copyright (C) 2018  Domenik Müller
 *  
 *  ---
 *
 *  Heavily based on the icoprog (https://github.com/cliffordwolf/icotools/tree/master/icoprog)
 *
 *  Copyright (C) 2016  Clifford Wolf <clifford@clifford.at>
 *  
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <assert.h>
#include <vector>

// SPI (spidev)
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

// YAML parsing
#include <yaml-cpp/yaml.h>
#include <bitset>
#include <iostream>

// wiringPi for GPIO control
#include <wiringPi.h>


bool verbose = false;
bool got_error = false;

bool enable_prog_port = false;
bool enable_data_port = false;


#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#  define RPI_ICE_CLK     27 
#  define RPI_ICE_CDONE   21 
#  define RPI_ICE_MOSI    23 
#  define RPI_ICE_MISO    22 
#  define RPI_ICE_CRESET  25 
#  define RPI_ICE_CS      26 


#  define RASPI_D8   8 
#  define RASPI_D7   9 
#  define RASPI_D6  30 
#  define RASPI_D5  31 
#  define RASPI_D4  12 
#  define RASPI_D3  13 
#  define RASPI_D2  11 
#  define RASPI_D1  10 
#  define RASPI_D0   6 
#  define RASPI_DIR  5 
#  define RASPI_CLK  3 

void digitalSync(int usec_delay)
{
	usleep(usec_delay);
}


bool send_zero = false;
bool recv_zero = false;
char current_send_recv_mode = 0;
int current_recv_ep = -1;

int last_recv_v = -1;
int last_recv_rep = 0;

void fpga_reset()
{
	pinMode(RPI_ICE_CRESET,  OUTPUT);
	digitalWrite(RPI_ICE_CRESET, LOW);
	digitalSync(2000);
	digitalWrite(RPI_ICE_CRESET, HIGH);
	digitalSync(500000);
	if (digitalRead(RPI_ICE_CDONE) != HIGH) {
		fprintf(stderr, "Warning: cdone is low\n");
		got_error = true;
	}
}

int get_time_ms()
{
	static struct timespec spec_start;
	static bool spec_start_initialized = false;

	struct timespec spec_now;
	clock_gettime(CLOCK_REALTIME, &spec_now);
	if (!spec_start_initialized) {
		spec_start = spec_now;
		spec_start_initialized = true;
	}

	int s = spec_now.tv_sec - spec_start.tv_sec;
	int ns = spec_now.tv_nsec - spec_start.tv_nsec;

	return s*1000 + ns/1000000;
}

void prog_bitstream(bool reset_only = false)
{
	assert(enable_prog_port);

	pinMode(RPI_ICE_CLK,     OUTPUT);
	pinMode(RPI_ICE_MOSI,    OUTPUT);
	pinMode(RPI_ICE_CRESET,  OUTPUT);
	pinMode(RPI_ICE_CS,      OUTPUT);

	fprintf(stderr, "reset..\n");

	// enable reset
	digitalWrite(RPI_ICE_CRESET, LOW);

	// start clock high
	digitalWrite(RPI_ICE_CLK, HIGH);

	// select SRAM programming mode
	digitalWrite(RPI_ICE_CS, LOW);
	digitalSync(100);

	// release reset
	digitalWrite(RPI_ICE_CRESET, HIGH);
	digitalSync(2000);

	fprintf(stderr, "cdone: %s\n", digitalRead(RPI_ICE_CDONE) == HIGH ? "high" : "low");

	if (reset_only)
		return;

	fprintf(stderr, "programming..\n");

	for (int i = 0; i < 8; i++) {
		digitalWrite(RPI_ICE_CLK, LOW);
		digitalWrite(RPI_ICE_CLK, HIGH);
	}
	
	for (int k = 0;; k++)
	{
		int byte = getchar();
		if (byte < 0)
			break;
		for (int i = 7; i >= 0; i--) {
			digitalWrite(RPI_ICE_MOSI, ((byte >> i) & 1) ? HIGH : LOW);
			digitalWrite(RPI_ICE_CLK, LOW);
			digitalWrite(RPI_ICE_CLK, HIGH);
		}

		if (verbose && !(k % 1024) && k)
			printf("%3d kB written.\n", k / 1024);
	}

	for (int i = 0; i < 49; i++) {
		digitalWrite(RPI_ICE_CLK, LOW);
		digitalWrite(RPI_ICE_CLK, HIGH);
	}



	digitalSync(2000);
	bool cdone_high = digitalRead(RPI_ICE_CDONE) == HIGH;
	fprintf(stderr, "cdone: %s\n", cdone_high ? "high" : "low");
	if (!cdone_high) got_error = true;
}

void spi_begin()
{
	digitalWrite(RPI_ICE_CS, LOW);
	//fprintf(stderr, "SPI_BEGIN\n");
}

void spi_end()
{
	digitalWrite(RPI_ICE_CS, HIGH);
	//fprintf(stderr, "SPI_END\n");
}



uint32_t spi_xfer(uint32_t data, int nbits = 8)
{
	assert(enable_prog_port);

	uint32_t rdata = 0;

	for (int i = nbits-1; i >= 0; i--)
	{
		digitalWrite(RPI_ICE_MOSI, (data & (1 << i)) ? HIGH : LOW);

		digitalSync(1);
		if (digitalRead(RPI_ICE_MISO) == HIGH)
			rdata |= 1 << i;

		digitalWrite(RPI_ICE_CLK, HIGH);

		digitalWrite(RPI_ICE_CLK, LOW);
	}

	//fprintf(stderr, "SPI:%d %02x %02x\n", nbits, data, rdata);
	return rdata;
}

void flash_write_enable()
{
	spi_begin();
	spi_xfer(0x06);
	spi_end();
}

void flash_bulk_erase()
{
	spi_begin();
	spi_xfer(0xc7);
	spi_end();
}

void flash_erase_64kB(int addr)
{
	spi_begin();
	spi_xfer(0xd8);
	spi_xfer(addr >> 16);
	spi_xfer(addr >> 8);
	spi_xfer(addr);
	spi_end();
}

void flash_write(int addr, uint8_t *data, int n)
{
	spi_begin();
	spi_xfer(0x02);
	spi_xfer(addr >> 16);
	spi_xfer(addr >> 8);
	spi_xfer(addr);
	while (n--)
		spi_xfer(*(data++));
	spi_end();
}

void flash_read(int addr, uint8_t *data, int n)
{
	spi_begin();
	spi_xfer(0x03);
	spi_xfer(addr >> 16);
	spi_xfer(addr >> 8);
	spi_xfer(addr);
	while (n--)
		*(data++) = spi_xfer(0);
	spi_end();
}

int flash_wait()
{
	int ms_start = get_time_ms();

	while (1)
	{
		spi_begin();
		spi_xfer(0x05);
		int status = spi_xfer(0);
		spi_end();

		if ((status & 0x01) == 0)
			break;

		digitalSync(1000);
	}

	return get_time_ms() - ms_start;
}

void prog_flasherase()
{
	assert(enable_prog_port);

	pinMode(RPI_ICE_CLK,     OUTPUT);
	pinMode(RPI_ICE_MOSI,    OUTPUT);
	pinMode(RPI_ICE_CS,      OUTPUT);

	// connect flash to Raspi
	digitalWrite(RPI_ICE_CS, HIGH);
	digitalWrite(RPI_ICE_CLK, LOW);
	digitalSync(100);

	// power_up
	spi_begin();
	spi_xfer(0xab);
	spi_end();

	flash_write_enable();
	flash_bulk_erase();

	// power_down
	spi_begin();
	spi_xfer(0xb9);
	spi_end();
}

void prog_flashmem(int pageoffset, bool erase_first_block)
{
	assert(enable_prog_port);

	pinMode(RPI_ICE_CLK,     OUTPUT);
	pinMode(RPI_ICE_MOSI,    OUTPUT);
	pinMode(RPI_ICE_CRESET, OUTPUT);
	pinMode(RPI_ICE_CS,      OUTPUT);

	// connect flash to Raspi
	digitalWrite(RPI_ICE_CRESET, LOW);
	digitalWrite(RPI_ICE_MOSI, LOW);
	digitalWrite(RPI_ICE_CS, HIGH);
	digitalWrite(RPI_ICE_CLK, LOW);
	digitalSync(100);

	// power_up
	spi_begin();
	spi_xfer(0xab);
	spi_end();

	// read flash id
	spi_begin();
	spi_xfer(0x9f);
	fprintf(stderr, "flash id:");
	for (int i = 0; i < 20; i++)
		fprintf(stderr, " %02x", spi_xfer(0x00));
	fprintf(stderr, "\n");
	spi_end();

	// load prog data into buffer
	std::vector<uint8_t> prog_data;
	if (erase_first_block)
	{
		prog_data.push_back(0);
	}
	else
	{
		while (1) {
			int byte = getchar();
			if (byte < 0)
				break;
			prog_data.push_back(byte);
		}

		fprintf(stderr, "writing %.2fkB..", double(prog_data.size()) / 1024);
	}

	int ms_timer = 0;
	for (int addr = 0; addr < int(prog_data.size()); addr += 256)
	{
		if (addr % (64*1024) == 0)
		{
			if (erase_first_block) {
				fprintf(stderr, "erasing 64kB sector @%06x..", addr);
			} else {
				fprintf(stderr, "\n%3d%% @%06x ", 100*addr/int(prog_data.size()), addr);
				fprintf(stderr, "erasing 64kB sector..");
			}

			flash_write_enable();
			flash_erase_64kB(addr + pageoffset * 0x10000);
			ms_timer += flash_wait();

			if (erase_first_block)
				break;
		}

		if (addr % (32*256) == 0) {
			fprintf(stderr, "\n%3d%% @%06x writing: ", 100*addr/int(prog_data.size()), addr);
		}

		int n = std::min(256, int(prog_data.size()) - addr);
		uint8_t buffer[256];

		for (int retry_count = 0; retry_count < 100; retry_count++)
		{
			flash_write_enable();
			flash_write(addr + pageoffset * 0x10000, &prog_data[addr], n);
			ms_timer += flash_wait();

			// short cut (TODO: remove!)
			goto written_ok;

			flash_read(addr + pageoffset * 0x10000, buffer, n);

			if (!memcmp(buffer, &prog_data[addr], n)) {
				fprintf(stderr, "o");
				goto written_ok;
			}

			fprintf(stderr, "X");
		}

		// restart erasing and writing this 64kB sector
		addr -= addr % (64*1024);
		addr -= 256;

	written_ok:;
	}

	fprintf(stderr, "\n%stotal wait time: %d ms\n", erase_first_block ? "" : "100% ", ms_timer);

	// power_down
	spi_begin();
	spi_xfer(0xb9);
	spi_end();
}

void read_flashmem(int n)
{
	assert(enable_prog_port);

	pinMode(RPI_ICE_CLK,     OUTPUT);
	pinMode(RPI_ICE_MOSI,    OUTPUT);
	pinMode(RPI_ICE_CS,      OUTPUT);

	// connect flash to Raspi
	digitalWrite(RPI_ICE_CS, HIGH);
	digitalWrite(RPI_ICE_CLK, LOW);
	digitalSync(100);

	// power_up
	spi_begin();
	spi_xfer(0xab);
	spi_end();

	// read flash id
	spi_begin();
	spi_xfer(0x9f);
	fprintf(stderr, "flash id:");
	for (int i = 0; i < 20; i++)
		fprintf(stderr, " %02x", spi_xfer(0x00));
	fprintf(stderr, "\n");
	spi_end();

	if (n > 0)
		fprintf(stderr, "reading %.2fkB..\n", double(n) / 1024);

	for (int addr = 0; addr < n; addr += 256) {
		uint8_t buffer[256];
		flash_read(addr, buffer, std::min(256, n - addr));
		fwrite(buffer, std::min(256, n - addr), 1, stdout);
	}

	// power_down
	spi_begin();
	spi_xfer(0xb9);
	spi_end();
}

void epsilon_sleep()
{
	for (int i = 0; i < 1000; i++)
		asm volatile ("");
}

void send_word(int v)
{
}

int recv_word(int timeout = 0)
{
	return -1;
}

void link_sync(int trignum = -1)
{
}

void test_link()
{
}

void test_bw()
{
}



void reset_inout()
{
	if (enable_prog_port)
	{
		pinMode(RPI_ICE_CLK,     INPUT);
		pinMode(RPI_ICE_CDONE,   INPUT);
		pinMode(RPI_ICE_MOSI,    INPUT);
		pinMode(RPI_ICE_MISO,    INPUT);
		pinMode(RPI_ICE_CRESET,  INPUT);
		pinMode(RPI_ICE_CS,      INPUT);
	}
}





void run_config(char* config_file)
{
 
	// coded events
	std::vector<uint64_t> events; 
	
	
	// read events from config file
	YAML::Node config = YAML::LoadFile(config_file);
	if (config["version"]) {
		fprintf(stdout, "Reading config file (version: %s) ...\n", config["version"].as<std::string>().c_str());
	
	}

	if (config["events"] ) {
		YAML::const_iterator it = config["events"].begin();
		for ( uint8_t id = 0; it != config["events"].end(); it++) {
			YAML::Node ev = *it;
			if (ev.size() == 1) {
				YAML::const_iterator ev_it = ev.begin();
				// event name, must always exist
				std::string ev_name = ev_it->first.as<std::string>(); 
				if (verbose)
					fprintf(stdout, "==Event %d==\n name: %s\n", id, ev_name.c_str());
				// event trigger, must also exist
				std::string ev_trigger = ev_it->second["trigger"].as<std::string>();
				if (verbose)	
					fprintf(stdout, "trigger: %s\n", ev_trigger.c_str());
				
				uint8_t ev_flag = 0;	
				if (ev_it->second["func"]) {
					// event func, if it exists
					std::string ev_func = ev_it->second["func"].as<std::string>();
					if (verbose)	
						fprintf(stdout, "func: %s\n", ev_func.c_str());
					if (ev_func.find("start") != std::string::npos)
						ev_flag |= 1;
					else if (ev_func.find("stop") != std::string::npos)
						ev_flag |= 2;
					if (ev_func.find("dump_begin") != std::string::npos)
						ev_flag |= 4;
					else if (ev_func.find("dump_end") != std::string::npos)
						ev_flag |= 8;
				}
		
				uint64_t enc = 0;
				enc |= ((uint64_t) id ) << 56;
				enc |= ((uint64_t) ev_flag) << 48;	
				
				for (unsigned int i = 0; i < 16; i++) {
					uint8_t trigger_flag = 7; // 7 = X = don't care
					if (i <= ev_trigger.size() - 1) {
					       switch(ev_trigger[i]) {
						       case '0': trigger_flag = 0; break;
						       case '1': trigger_flag = 3; break;
						       case 'u': trigger_flag = 1; break;
						       case 'd': trigger_flag = 2; break;
					       }

					}
				       	//std::cout << ev_trigger[i] << ":" << std::bitset<64>(((uint64_t) trigger_flag) << ((15 - i) * 3)) << std::endl;	
					enc |= ((uint64_t) trigger_flag) << ((15 - i) * 3);
				}		
				
				if (verbose) {
					//std::cout << "encoded: " << std::bitset<64>(enc) << std::endl;	
					fprintf(stdout, "encoded: %016llx \n", enc);
				}
				events.push_back(enc);
				id++;
			}		

		}
	}

	// setup SPI


	int ret;
	int fd;
	uint8_t mode = 1;
	uint8_t bits = 8;
	uint16_t delay = 0;
	uint32_t speed = 3000000;

	fd = open("/dev/spidev0.0", O_RDWR);
	if (fd < 0)
		fprintf(stderr,"Can't open SPI device\n");

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		fprintf(stderr,"Can't set spi mode\n");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		fprintf(stderr,"can't get spi mode\n");

	/*
	 * bits per word
	 */
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		fprintf(stderr,"can't set bits per word\n");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		fprintf(stderr,"can't get bits per word\n");

	/*
	 * max speed hz
	 */
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		fprintf(stderr,"can't set max speed hz\n");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		fprintf(stderr,"can't get max speed hz\n");

	//ret = ioctl(fd, SPI_IOC_WR_LSB_FIRST, 0);	

	fprintf(stdout, "\n===\nSPI Configuration:\nspi mode: %d\n", mode);
	fprintf(stdout, "bits per word: %d\n", bits);
	fprintf(stdout, "max speed: %d Hz (%d KHz)\n", speed, speed/1000);


	for (int i = 0; i < events.size(); i++) {
		
		fprintf(stdout, "---\nSPI: sending event %d: %16llx\n", i, events[i]);
		
		// receive buffer
		uint8_t rx[12] = {0, };
		
		// single bytes of current event
		uint8_t *ev_ptr = (uint8_t *) &(events[i]);
		

		// setup transmit buffer with trigger config command
		uint8_t tx[4] = { 0x01, 0x00, 0x00, 0x00};
		

		struct spi_ioc_transfer tr = {
			tx_buf : (unsigned long) tx,
			rx_buf : (unsigned long) rx,
			len : ARRAY_SIZE(tx),
			speed_hz : speed,
			delay_usecs : delay,
			bits_per_word : bits
		};

		
		// send command
		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret < 1)
			fprintf(stderr, "can't send spi message");

		
		// send first 4 bytes (and change endianess) 
		tr.rx_buf = (unsigned long) &(rx[4]);
		for (int h = 0; h < 4; h++) 
			tx[h] = ev_ptr[h];
		tr.tx_buf = (unsigned long) tx;

		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
		if (ret < 1)
			fprintf(stderr, "can't send spi message");



		// send last 4 bytes
		tr.rx_buf = (unsigned long) &(rx[8]);
		for (int h = 0; h < 4; h++) 
			tx[h] = ev_ptr[4+h];
		tr.tx_buf = (unsigned long) tx;

		ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);	
		if (ret < 1)
			fprintf(stderr, "can't send spi message");



		// show what we got back
		fprintf(stdout, "SPI: returned:");

		for (ret = 0; ret < ARRAY_SIZE(rx); ret++) {
			if (!(ret % 12))
				puts("");
			printf("%.2X ", rx[ret]);
		}
		puts("");	

	}

	close(fd);
}


void help(const char *progname)
{
	fprintf(stderr, "\n");
	fprintf(stderr, "Resetting FPGA:\n");
	fprintf(stderr, "    %s -R\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Resetting FPGA (reload from flash):\n");
	fprintf(stderr, "    %s -b\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Erase first flash block:\n");
	fprintf(stderr, "    %s -e\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Bulk erase entire flash:\n");
	fprintf(stderr, "    %s -E\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Programming FPGA bit stream:\n");
	fprintf(stderr, "    %s -p < data.bin\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Programming serial flash:\n");
	fprintf(stderr, "    %s -f < data.bin\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Reading serial flash (first N bytes):\n");
	fprintf(stderr, "    %s -F N > data.bin\n", progname);
	fprintf(stderr, "\n");
	fprintf(stderr, "Additional options:\n");
	fprintf(stderr, "    -v      verbose output\n");
	fprintf(stderr, "    -O N    offset (in 64 kB pages) for -f and -e\n");
	fprintf(stderr, "\n");
	exit(1);
}


int main(int argc, char **argv)
{
	int opt, n = -1;
	int pageoffset = 0;
	char mode = 0;
	char *config_file;

	while ((opt = getopt(argc, argv, "RbEpfevF:C:O:")) != -1)
	{
		switch (opt)
		{
		case 'F':
			n = atoi(optarg);
			// fall through
		case 'C':
			config_file = optarg; 

		case 'E':
		case 'R':
		case 'b':
		case 'p':
		case 'f':
		case 'e':
			if (mode)
				help(argv[0]);
			mode = opt;
			break;
		case 'v':
			verbose = true;
			break;

		case 'O':
			pageoffset = atoi(optarg);
			break;

		default:
			help(argv[0]);
		}
	}

	if (optind != argc || !mode)
		help(argv[0]);

	if (mode == 'R') {
		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		prog_bitstream(true);
		reset_inout();
	}

	if (mode == 'b') {
		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		fpga_reset();
		reset_inout();
	}

	if (mode == 'E') {
		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		prog_flasherase();
		reset_inout();
	}

	if (mode == 'p') {
		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		prog_bitstream();
		reset_inout();
	}

	if (mode == 'f') {
		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		prog_flashmem(pageoffset, false);
		reset_inout();
	}

	if (mode == 'e') {
		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		prog_flashmem(pageoffset, true);
		reset_inout();
	}

	if (mode == 'F') {
		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		read_flashmem(n);
		reset_inout();
	}

	if (mode == 'C') {

		enable_prog_port = true;
		wiringPiSetup();
		reset_inout();
		run_config(config_file);
		reset_inout();
	}


	if (verbose)
		fprintf(stderr, "\n");

	return got_error ? 1 : 0;
}



