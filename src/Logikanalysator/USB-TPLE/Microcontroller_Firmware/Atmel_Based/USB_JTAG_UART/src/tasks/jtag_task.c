
//! \file *********************************************************************
//!
//! \brief This file manages the JTAG Task
//!
//! - Supported devices:  ATmega32U4
//!
//! \author               Andreas Mueller, HSA
//!                       Date Modified: 2010-03-24
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

//_____  I N C L U D E S ___________________________________________________

#include "../conf/config.h"
#include "../conf/conf_usb.h"
#include "jtag_task.h"
#include "../lib_mcu/usb/usb_drv.h"
#include "../usb/usb_descriptors.h"
#include "../usb/usb_standard_request.h"
#include "../usb/usb_specific_request.h"
#include "../lib_mcu/uart/uart_lib.h"
#include "../usb/uart_usb_lib.h"
#include "../lib_mcu/util/start_boot.h"
#include "../usb/jtag_usb_lib.h"		// API Functions for USB-JTAG
#include <stdio.h>



//_____ M A C R O S ________________________________________________________



//_____ D E F I N I T I O N S ______________________________________________



//_____ D E C L A R A T I O N S ____________________________________________

/*
volatile U8 cpt_sof;
extern U8    jtag_rx_counter;
extern U8    jtag_tx_counter;
extern volatile U8 usb_request_break_generation;

S_line_coding line_coding;
S_line_status line_status;      // for detection of serial state input lines
S_serial_state serial_state;   // for serial state output lines

volatile U8 rs2usb[10];
*/

//! @brief This function initializes the hardware ressources required for CDC demo.
//!
//!
//! @param none
//!
//! @return none
//!
//!/
void jtag_task_init(void)
{
	jtag_init();
	jtag_usb_init();

//#ifdef __GNUC__
//  fdevopen((int (*)(char, FILE*))(uart_usb_putchar),(int (*)(FILE*))uart_usb_getchar); //for printf redirection
//#endif
}



//! @brief Entry point of the uart cdc management
//!
//! This function links the uart and the USB bus.
//!
//! @param none
//!
//! @return none
void jtag_task(void)
{     
//	char jtag_rx_buffer[JTAG_RX_EP_SIZE];
//	char jtag_tx_buffer[JTAG_TX_EP_SIZE];
//	char* z_jtag_rx_buffer;
//	char* z_jtag_tx_buffer;
	
	//Zeiger referenzieren

//	z_jtag_rx_buffer = &jtag_rx_buffer[0];
//	z_jtag_tx_buffer = &jtag_tx_buffer[0];	


	
	if(Is_device_enumerated())
	{
	//Select RX EP:
// 	Usb_select_endpoint(JTAG_RX_EP);				
	//Fill RX Buffer:
//	if(jtag_usb_test_hit())
//	{
//		*z_jtag_rx_buffer = jtag_usb_getchar();
//		z_jtag_rx_buffer++;
    Led2_toggle();
//	}
//	z_jtag_rx_buffer = &jtag_rx_buffer[0]; //Wenn Puffer befüllt, setze Zeiger zurück

	}
}

//! @brief sof_action
//!
//! This function increments the cpt_sof counter each times
//! the USB Start Of Frame interrupt subroutine is executed (1ms)
//! Usefull to manage time delays
//!
//! @warning Code:?? bytes (function code length)
//!
//! @param none
//!
//! @return none
/*
void sof_action()
{
   cpt_sof++;
}


//! @brief Uart Receive interrupt subroutine
//!
//! @param none
//!
//! @return none
#ifdef __GNUC__
 ISR(USART1_RX_vect)
#else
#pragma vector = USART1_RX_vect
__interrupt void usart_receive_interrupt()
#endif
{
   U8 i=0;
   U8 save_ep;
   
   if(Is_device_enumerated()) 
   {
      save_ep=Usb_get_selected_endpoint();   
      Usb_select_endpoint(TX_EP);
      do 
      {
         if(Uart_rx_ready())
         {
            rs2usb[i]=Uart_get_byte();
            i++;
         }
      }while(Is_usb_write_enabled()==FALSE );
      uart_usb_send_buffer((U8*)&rs2usb,i);
      Usb_select_endpoint(save_ep);
   }
}
*/
