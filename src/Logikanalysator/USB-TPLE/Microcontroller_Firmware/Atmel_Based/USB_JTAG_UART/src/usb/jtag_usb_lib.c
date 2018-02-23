/* (c) 2010 Andreas Mueller, HSA <andz@rz.fh-augsburg.de>
   Date: 2010-03-26 */


/* JTAG Part:

  Some Parts used from: (c) 2006,2007 Wojciech M. Zabolotny <wzab@ise.pw.edu.pl>
*/

/* USB Part: */

/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief @brief This file controls the UART USB functions.
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  ATmega32U4
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************

/* Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*_____ I N C L U D E S ____________________________________________________*/

#include "../conf/config.h"
#include "../lib_mcu/usb/usb_drv.h"
#include "usb_descriptors.h"

#include "jtag_usb_lib.h"

/*_____ M A C R O S ________________________________________________________*/

/*_____ D E F I N I T I O N ________________________________________________*/


U8 jtag_rx_counter;

/*_____ D E C L A R A T I O N ______________________________________________*/

/** 
  * @brief Initializes the uart_usb library
  */
void jtag_usb_init(void)
{
  jtag_rx_counter = 0;
  //evtl noch Port (rück)setzen
}


/* The variable dummy_var_for_bug_protection has been introduced to
  workaround the bug in the SDCC: 
  http://sourceforge.net/tracker/index.php?func=detail&aid=1818857&group_id=599&atid=100599
  When the bug gets fixed, you can get rid of this variable */
U8 dummy_var_for_bug_protection=0;
U8 current_chain=0;
/* The variable pin_invert informs which lines are inverted.
   You can get the proper value by or-ing the necessary masks from:
   TMS_mask, TDI_mask, TCK_mask and TDO_mask.
   In my design all the lines are inverted, so I simply set all the values to 0xff.
*/
U8 code pin_invert[8]={
  0xff, // chain 0
  0xff, // chain 1
  0xff, // chain 2
  0xff, // chain 3
  0xff, // chain 4
  0xff, // chain 5
  0xff, // chain 6
  0xff, // chain 7
};
Uint16 delay_num[8];
U8 delay_exp[8];

void jtag_set_chain(U8 chain)
{
  current_chain = 0; //Only One Chain
}


void jtag_config_chain(U8 * datain)
{
  delay_num[current_chain]=((Uint16) *datain++) | (((Uint16) *datain++) << 8);
}

//void init_jtag()
//{
//  TRISD=0xc0;
//  PORTD=0x0;
//}

/** 
  * @brief This function checks if a character has been received on the USB bus.
  * 
  * @return bit (true if a byte is ready to be read)
  */
bit jtag_usb_test_hit(void)
{
  if (!jtag_rx_counter)
  {
    Usb_select_endpoint(JTAG_RX_EP);
    if (Is_usb_receive_out())
    {
      jtag_rx_counter = Usb_byte_counter();
      if (!jtag_rx_counter)
      {
        Usb_ack_receive_out();
      }
    }
  }
  return (jtag_rx_counter!=0);
}

/** 
  * @brief This function reads one byte from the USB bus
  *
  * If one byte is present in the USB fifo, this byte is returned. If no data
  * is present in the USB fifo, this function waits for USB data.
  * 
  * @return U8 byte received
  */
char jtag_usb_getchar(void)
{
  register Uchar data_rx;

  Usb_select_endpoint(JTAG_RX_EP);
  if (!jtag_rx_counter) while (!jtag_usb_test_hit());
  data_rx=Usb_read_byte();
  jtag_rx_counter--;
  if (!jtag_rx_counter) Usb_ack_receive_out();
  return data_rx;
}

/** 
  * @brief This function checks if the USB emission buffer is ready to accept at
  * at least 1 byte
  * 
  * @return Boolean. TRUE if the firmware can write a new byte to transmit.
  */
bit jtag_usb_tx_ready(void)
{
  if (!Is_usb_write_enabled())
  {
    return FALSE;
  }
  return TRUE;
}

/** 
  * @brief This function fills the USB transmit buffer with the new data. This buffer
  * is sent if complete. To flush this buffer before waiting full, launch
  * the uart_usb_flush() function.
  * 
  * @param data_to_send 
  * 
  * @return 
  */
int jtag_usb_putchar(int data_to_send)
{
   jtag_usb_send_buffer((U8*)&data_to_send, 1);
   return data_to_send;
}



/** 
  * @brief This function transmits a ram buffer content to the USB.
  * This function is mode efficient in term of USB bandwith transfer.
  * 
  * @param U8 *buffer : the pointer to the RAM buffer to be sent 
  * @param data_to_send : the number of data to be sent
  */
void jtag_usb_send_buffer(U8 *buffer, U8 nb_data)
{
   U8 zlp;
   
   // Compute if zlp required
   if(nb_data%JTAG_TX_EP_SIZE) 
   { zlp=FALSE;} 
   else { zlp=TRUE; }
   
   Usb_select_endpoint(JTAG_TX_EP);
   while (nb_data)
   {
      while(Is_usb_write_enabled()==FALSE); // Wait Endpoint ready
      while(Is_usb_write_enabled() && nb_data)
      {
         Usb_write_byte(*buffer);
         buffer++;
         nb_data--;
   }
      Usb_ack_in_ready();
   }
   if(zlp)
   {
      while(Is_usb_write_enabled()==FALSE); // Wait Endpoint ready 
      Usb_ack_in_ready();
}
}


/* The procedure below is coded with the speed in mind. That's why it is coded
   in "copy&paste" way */

void jtag_block_xmit(U8 *datain, short len)
{
  U8 *indata;
  U8 dta=0;
  U8 invert=0;
  short i=0;
  Uint16 jtag_del,d;
  jtag_del=delay_num[current_chain];
  invert=pin_invert[current_chain];
  indata=datain;
  for(i=0;i<len;i++) {
    //We extract the two bits of data
    if((i & 3)==0) {
      dta = *indata;
      indata++;
    }
    //Now we transfer the data
    if ((dta ^ invert)& TDI_mask) {
      // Set TDI to 1
		Jtag_TDI_1();
    } 
	else {
      // Set TDI to 0
		Jtag_TDI_0();
    }
    if ((dta ^ invert) & TMS_mask) {
      // Set TMS to 1
		Jtag_TMS_1();
    } 
	else {
      // Set TMS to 0
		Jtag_TMS_0();
    }

    //delay
    for(d=jtag_del;d>0;d--) {};

    //Set TCK to 1
    if (invert & TCK_mask) {
      // Set pin to 0 - it will be inverted
		Jtag_TCK_0();

    } 
	else {
      // Set pin to 1
		Jtag_TCK_1();
    }

    //delay
    for(d=jtag_del;d>0;d--) {};

    if (invert & TCK_mask) {
      // Set pin to 1 - it will be inverted
 		Jtag_TCK_0();
    } 
	else {
		Jtag_TCK_1();
    }
    dta >>= 2;
  }
}

U8 jtag_single(U8 datain)
{
  U8 res;
  U8 invert;
  Uint16 jtag_del;
  Uint16 d;
  //Here we need to check the current JTAG chain as well
  //Maybe we need to have another function for setting (resetting) of the pins?
  //The two least significant bits encode TMS and TDI
  //We return the state of the TDO
  //First we set or clear TMS and TDI

  jtag_del=delay_num[current_chain];
  invert=pin_invert[current_chain];

  for(d=jtag_del;d>0;d--) {};
  // Read the TDO
  res = Get_TDO();
  // Set the TDI
    if ((datain ^ invert)& TDI_mask) {
      // Set TDI to 1
 		Jtag_TDI_1();
    } 
	else {
		Jtag_TDI_0();
    }
    if ((datain^invert) & TMS_mask) {
      // Set TMS to 1
 		Jtag_TMS_1();
    } 
	else {
  		Jtag_TMS_0();
    }
    //Set TCK to 1
    if (invert & TCK_mask) {
      // Set pin to 0 - it will be inverted
 		Jtag_TCK_0();
    } 
	else {
      // Set pin to 1
 		Jtag_TCK_1();
    }
    //delay
    for(d=jtag_del;d>0;d--) {};
    if (invert & TCK_mask) {
   		Jtag_TCK_1();

    } 
	else {
      // Set pin to 0
 		Jtag_TCK_0(); 

    }
  return res;
}

/* This procedure is not used in the STAPL Player!!!
   it is not optimized, and therefore it is relatively slow */
/*
void jtag_block_xmit_with_read(U8 * datain,
			       U8 * dataout, short len)
{
  U8 dta=0;
  U8 dout=0;
  U8 mask=0;
  short i=0;
  mask = 1;
  for(i=0;i<len;i++) {
    if((i & 3) == 0)  dta = *(datain++);
    dout |= jtag_single(dta) ? mask : 0;
    mask <<= 1;
    if((i & 7) == 7)  {
      mask = 0;
      *(dataout++) = dout;
    }
  }
  //We check if any unsaved bits remain
  if(len & 7)  *(dataout++) = dout;
}
*/
/* The set_xxx procedures have bin written for testing
   they are very slow, and should not be used for any repeating
   activities */
/*
void set_tdi(U8 d)
{
  if (pin_invert[current_chain] & TDI_mask) d = !d;
  switch (current_chain) {
  case 0:
    if(d) TDI0 = 1;
    else TDI0 = 0;
    break;
  case 1:
    if(d) TDI1 = 1;
    else  TDI1 = 0;
    break;
  }
}

void set_tms(U8 d)
{
  if (pin_invert[current_chain] & TMS_mask) d = !d;
  switch (current_chain) {
  case 0:
    if(d) TMS0=1;
    else TMS0=0;
    break;
  case 1:
    if(d) TMS1=1;
    else TMS1=0;
    break;
  }
}
void set_tck(U8 d)
{
  if (pin_invert[current_chain] & TCK_mask) d = !d;
  switch (current_chain) {
  case 0:
    if(d) TCK0=1;
    else TCK0=0;
    break;
  case 1:
    if(d) TCK1=1;
    else TCK1=0;
    break;
  }
}

U8 get_tdo(void)
{
  U8 d=0;
  switch(current_chain) {
  case 0:
    d = TDO0;
    break;
  case 1:
    d = TDO1;
    break;
  }
  if (pin_invert[current_chain] & TDO_mask) d = !d;
  return d ? 1 : 0;
}

U8 jtag_set_pins(U8 d)
{
  U8 res;
  res=get_tdo();
  set_tdi(d & TDI_mask);
  set_tms(d & TMS_mask);
  set_tck(d & TCK_mask);
  //set_trst(d & TRST_mask);
  return res;
}

*/
