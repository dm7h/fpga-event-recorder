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

#ifndef _UART_USB_LIB_H_
#define _UART_USB_LIB_H_

/*_____ I N C L U D E S ____________________________________________________*/

#include "../conf/config.h"

/*_____ M A C R O S ________________________________________________________*/
/*#ifdef UART_USB_DEFAULT_INPUT
#define uart_usb_getchar            _getkey
#define uart_usb_test_hit           test_hit
#endif

#ifdef UART_USB_DEFAULT_OUTPUT
#define uart_usb_putchar            putchar
#endif
*/
/*_____ D E F I N I T I O N ________________________________________________*/


#define  GET_INFO (0x01)
#define  CONFIG_CHAIN (0x02)
#define  SINGLE_DATA_WITH_READ (0xac)
#define  BLOCK_DATA (0xb0)
#define  BLOCK_DATA_WITH_READ (0xc0)
#define  SET_PINS (0xd0)
#define  SELECT_CHAIN (0xe0)


/* Encoding of bits */
#define TDI_bit (0) /* TDI & TMS must(!) be on the two lowest bits */
#define TMS_bit (1) /* it is used when passing args to jtag_single! */
#define TCK_bit (2)
#define TRST_bit (3) /* TDI, TMS, TCK & TRST must be on bits 0...3 ! */
#define TDO_bit (4)

#define TDI_mask (1<<TDI_bit)
#define TMS_mask (1<<TMS_bit)
#define TCK_mask (1<<TCK_bit)
//#define TRST_mask (1<<TRST_bit)
#define TDO_mask (1<<TDO_bit)

/* Allocation of pins into JTAG chains */
#define TDO0 JTAG_TDO
#define TCK0 JTAG_TCK
#define TMS0 JTAG_TMS
#define TDI0 JTAG_TDI
//#define TRST0 PORTDbits.RD3

#define TDO1 JTAG_TDO
#define TCK1 JTAG_TCK
#define TMS1 JTAG_TMS
#define TDI1 JTAG_TDI
//#define TRST1 PORTEbits.RE1


#define TDO2 JTAG_TDO
#define TCK2 JTAG_TCK
#define TMS2 JTAG_TMS
#define TDI2 JTAG_TDI


#define TDO3 JTAG_TDO
#define TCK3 JTAG_TCK
#define TMS3 JTAG_TMS
#define TDI3 JTAG_TDI


#define TDO4 JTAG_TDO
#define TCK4 JTAG_TCK
#define TMS4 JTAG_TMS
#define TDI4 JTAG_TDI


#define TDO5 JTAG_TDO
#define TCK5 JTAG_TCK
#define TMS5 JTAG_TMS
#define TDI5 JTAG_TDI

#define TDO6 JTAG_TDO
#define TCK6 JTAG_TCK
#define TMS6 JTAG_TMS
#define TDI6 JTAG_TDI

#define TDO7 JTAG_TDO
#define TCK7 JTAG_TCK
#define TMS7 JTAG_TMS
#define TDI7 JTAG_TDI



/*_____ D E C L A R A T I O N ______________________________________________*/

void  jtag_usb_init(void);

void jtag_block_xmit(U8 * datain, short len);
void jtag_block_xmit_with_read(U8 * datain, U8 * dataout, short len);
U8 jtag_single(U8 datain);
U8 jtag_set_pins(U8 datain);
void jtag_set_chain(U8 chain);
void jtag_config_chain(U8 * datain);

void tck_delay();
void init_jtag();

//JTAG-USB Functions

bit jtag_usb_test_hit(void);
char jtag_usb_getchar(void);
bit jtag_usb_tx_ready(void);
int jtag_usb_putchar(int data_to_send);
void jtag_usb_send_buffer(U8 *buffer, U8 nb_data);

#endif /* _UART_USB_LIB_H_ */
