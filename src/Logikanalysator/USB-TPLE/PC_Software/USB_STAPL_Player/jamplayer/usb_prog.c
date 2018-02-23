/*
  This file implements the USB specific part of the stapl player
Modified by Andreas Mueller for USB-TPLE Project
Date: 2010-03-24
*/

#include <stdlib.h>
#include <errno.h>
#include <time.h>
//#include "ports.h"
#include "jtag.h"
#include <stdio.h>
#include <sched.h>
#include <usb.h>

extern int jtag_hardware_initialized; // Was type BOOL in jamstub.c!!!
extern int jtag_delay;
extern int jtag_output_chain;
int jam_jtag_io(int tms, int tdi, int read_tdo);
int low_jtag_io(int tms, int tdi, int read_tdo);
void write_usb_cache(int tdi_tms);
void flush_usb_cache(void);
void waitTime(long microsec);
void waitNano(long nanosec);

struct usb_bus *busses=NULL;
struct usb_bus *bus=NULL;
usb_dev_handle *hdev=NULL;

unsigned char recbuf[64];

unsigned char config_chain[4];
void sp_usb_config_chain(int delay_val)
{
  int res;
  config_chain[0]=CONFIG_CHAIN;
  config_chain[1]=delay_val & 0xff;
  config_chain[2]=(delay_val >> 8) & 0xff;
  res=usb_bulk_write(hdev, EP_TX, config_chain, 4, 500);
  if (res != 4) {
    fprintf(stderr,"Can't send CONFIG_CHAIN command! got %d \n",res);
    exit(1);	   
  }
  res=usb_bulk_read(hdev, EP_RX, recbuf, sizeof(recbuf)-1, 1500);
  if(res<0) {
    fprintf(stderr,"Can't receive CONFIG_CHAIN response!\n");
    exit(1);	   
  }
  return;
}

unsigned char select_chain[1];
void sp_usb_select_chain(int num)
{
  int res;
  select_chain[0]=SELECT_CHAIN | (num & 0x0f);
  res=usb_bulk_write(hdev, EP_TX, select_chain, 1, 500);
  if (res != 1) {
    fprintf(stderr,"Can't send SELECT_CHAIN command! got %d \n",res);
    exit(1);	   
  }
  res=usb_bulk_read(hdev, EP_RX, recbuf, sizeof(recbuf)-1, 500);
  if(res<0) {
    fprintf(stderr,"Can't receive SELECT_CHAIN response!\n");
    exit(1);	   
  }
    if(recbuf[0] != SELECT_CHAIN) {
      fprintf(stderr,"Incorrect SELECT_CHAIN response!\n");
      exit(1);	   
    }  
  fprintf(stderr,"Selected chain: %d\n",num);
  return;
}

void sp_usb_init(void)
{

  int c, i, a;
  int res;
  unsigned char get_info[1];
  get_info[0]=0x01;
    
  /* ... */
  usb_init();
  //usb_set_debug(255);
  usb_find_busses();                                                            
  usb_find_devices();                                                                            
  busses=usb_get_busses();
  for (bus = busses; bus; bus = bus->next) {
    struct usb_device *dev;    
    for (dev = bus->devices; dev; dev = dev->next) {
      /* Check if this is our device */
      if ((dev->descriptor.idVendor == CJ_USB_ID_VENDOR) &&
	  (dev->descriptor.idProduct == CJ_USB_ID_PRODUCT)) {
	fprintf(stderr,"Probably found the USB TPLE JTAG Device\n");
	/* Open the device, claim the interface and do your processing */
	if(dev->descriptor.bNumConfigurations<1) {
	  fprintf(stderr,"Too few configurations!\n");
	  exit(1);	   
	}
	printf("Vorher: %d \n", hdev);
	hdev=usb_open(dev);

	if(hdev==NULL) {
	  fprintf(stderr,"Can't open device!");
	  exit(1);	   
	}
	printf("Nacher: %d \n", hdev);
//	res=usb_close(hdev);
//	printf("Schliesse Device: %d \n ",res);


	//Reset Device:
	res=usb_reset(hdev);
	if(res<0) {
	  fprintf(stderr,"Can't reset device! Error: %d\n",res);
	  exit(1);	   
	}

	if(!hdev) {
	  fprintf(stderr,"Can't open device!\n");
	  exit(1);	   
	}
	/* Warning!!! the number of configuration is increased by 1 comparing 
	   to the number in Python */
	if(usb_set_configuration(hdev, 1)) {
	  fprintf(stderr,"Can't set configuration 1!\n");
	  exit(1);	   
	}
	if(usb_claim_interface(hdev, 2)) {
	  fprintf(stderr,"Can't claim interface 2!\n");
	  exit(1);	   
	}
	if(usb_set_altinterface(hdev, 2)) {
	  fprintf(stderr,"Can't set alt interface 2!\n");
	  exit(1);	   
	}
	/* Now let's check the GET_INFO function */
	//sleep(1);
	res=usb_bulk_write(hdev, EP_TX, get_info, 1, 500);
	if (res != 1) {
	  fprintf(stderr,"Can't send GET_INFO command! got %d \n",res);
	  exit(1);	   
	}
	res=usb_bulk_read(hdev, EP_RX, recbuf, sizeof(recbuf)-1, 500);
	if(res<0) {
	  fprintf(stderr,"Can't receive GET_INFO response!\n");
	  exit(1);	   
	}
	fprintf(stderr,"%s\n",recbuf);
	// Select the chain
	sp_usb_select_chain(jtag_output_chain);
	// Config the chain
	sp_usb_config_chain(jtag_delay);
	// Reset the  TAP controller
	for(res=0;res<10;res++) write_usb_cache(3);
	flush_usb_cache();
	return;
      }
      else {
	fprintf(stderr,"found other device: Vid:%x Pid:%x\n",
		dev->descriptor.idVendor,dev->descriptor.idProduct);
      }
    }
  }
}


void sp_usb_release(void)
{
  usb_release_interface(hdev,0);
  usb_close(hdev);
  hdev=NULL;
}

void initialize_jtag_hardware()
{
  sp_usb_init();
}
void close_jtag_hardware()
{
  //First we flush the VME cache
  flush_usb_cache();
  //Then we wait for a few miliseconds
  waitTime(1000);
  //And finally we release the caen
  sp_usb_release();
}


//Buffers used for caching of write accesses
#define USB_CACHE_LEN (60*4)
unsigned char usb_cache[(USB_CACHE_LEN+3)/4+2];

//Pointer used to cache 
unsigned int usb_cache_cnt = 0;

void flush_usb_cache()
{
  if(usb_cache_cnt) {
    //Update the cache header
    usb_cache[0] = BLOCK_DATA | ((usb_cache_cnt >> 8) & 0xf);
    usb_cache[1] = usb_cache_cnt & 0xff;
    //Send the cache via the USB
    //fprintf(stderr,"u");
    if(usb_bulk_write(hdev, EP_TX, usb_cache, 2+(usb_cache_cnt+3)/4, 500) < 0) {
      fprintf(stderr,"Sending BLOCK_DATA error in usb_bulk_write!\n");
      exit(1);
    }
    //Receive the answer from the programmer
    if(usb_bulk_read(hdev, EP_RX, recbuf, sizeof(recbuf)-1, 500)<0) {
      fprintf(stderr,"Can't receive BLOCK_DATA response!\n");
      exit(1);	   
    }
    if(recbuf[0] != BLOCK_DATA) {
      fprintf(stderr,"Incorrect BLOCK_DATA response!\n");
      exit(1);	   
    }  
    usb_cache_cnt = 0;
  }
}

void write_usb_cache(int tdi_tms)
{
  if(usb_cache_cnt>=USB_CACHE_LEN) flush_usb_cache();
  int byte_pos = usb_cache_cnt/4 + 2;
  int byte_shift = (usb_cache_cnt & 3) * 2;
  if (byte_shift==0) usb_cache[byte_pos] = 0;
  usb_cache[byte_pos] |= ((tdi_tms & 3) << byte_shift);
  usb_cache_cnt++;
}

unsigned char single_with_read[1];
int single_data_with_read(tms_tdi){
  int res;
  single_with_read[0]= SINGLE_DATA_WITH_READ | (tms_tdi & 3);
  res=usb_bulk_write(hdev, EP_TX, single_with_read, 1, 500);
  if (res != 1) {
    fprintf(stderr,"Can't send SINGLE_DATA_WITH_READ command! got %d \n",res);
    exit(1);	   
  }
  res=usb_bulk_read(hdev, EP_RX, recbuf, sizeof(recbuf)-1, 2500);
  if(res<0) {
    fprintf(stderr,"Can't receive SINGLE_DATA_WITH_READ response!\n");
    exit(1);	   
  }
  //fprintf(stderr,"%x",(int)recbuf[0]);
  if((recbuf[0] & 0x0fc) != SINGLE_DATA_WITH_READ) {
    fprintf(stderr,"Incorrect SINGLE_DATA_WITH_READ response! got:%x instead of %x\n",
	    (int) (recbuf[0] & 0xf0), (int) SINGLE_DATA_WITH_READ);
    exit(1);	   
  }  
  return recbuf[0] & 1;
}

int low_jtag_io_(int tms, int tdi, int read_tdo)
  {
  int tdo=0;
  int tms_tdi=0;
  if(tdi) tms_tdi |= 1;
  if(tms) tms_tdi |= 2;
  tdo=single_data_with_read(tms_tdi);
  return tdo;
  }


int low_jtag_io(int tms, int tdi, int read_tdo)
{
  int tdo=0;
  int tms_tdi=0;
  if(tdi) tms_tdi |= 1;
  if(tms) tms_tdi |= 2;
  if(read_tdo) {
    // read is requested
    // flush the FIFO
    flush_usb_cache();
    // perform the single data transfer
    tdo=single_data_with_read(tms_tdi);
    //fprintf(stderr,"%c",tdo ? '1' : '0'); 
  } else {
    // queue the request
    write_usb_cache(tms_tdi);
  }
  return tdo;
}

//More advanced version of the low_jtag_io - all write requests are cached, until read
//request is issued,
//the cache is emptied when 1) read of TDO is requested, 2) jam_delay is called
//3) the cache is full

int jam_jtag_io(int tms, int tdi, int read_tdo)
{
  int tdo=0;
  if (!jtag_hardware_initialized)
    {
      fprintf(stderr,"JTAG not initialized yet...\n");
      initialize_jtag_hardware();
      jtag_hardware_initialized = 1; //we must set it in advance
    }
  return low_jtag_io(tms, tdi, read_tdo); 
}

int jam_vector_io
(
 int signal_count,
 long *dir_vect,
 long *data_vect,
 long *capture_vect
 )
{
  fprintf(stderr,"VECTOR functions not implemented!!!\n");
  exit(1);
}

void waitTime(long microsec)
{
  waitNano(microsec*1000);
}

void waitNano(long nanosec)
{
  struct timespec rec;
  int res;
  rec.tv_sec=0;
  rec.tv_nsec=nanosec;
  for(;;) {
    res=nanosleep(&rec, &rec);
    if (res==0) break;
    else {
      if (errno==EINVAL){
	fprintf(stderr,"nanosleep error\n",res);
	exit(1);
      }
    }
  }
}

void jam_delay(long microseconds)
{
  if(jtag_hardware_initialized) flush_usb_cache();
  //usleep(microseconds);
  waitNano(1000*microseconds);
}
