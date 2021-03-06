/* (c) 2010 Andreas Mueller, HSA <andz@rz.fh-augsburg.de>
   Date: 2010-03-26


/* JTAG Part: */
/*
  (c) 2006,2007 Wojciech M. Zabolotny <wzab@ise.pw.edu.pl>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/* USB Part: */

/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief Main for USB application.
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  ATmega32U4
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************
//!
//! @mainpage ATmega32U4 CDC demonstration
//!
//! @section intro License
//! Use of this program is subject to Atmel's End User License Agreement.
//!
//! Please read file  \ref lic_page for copyright notice.
//!
//! @section install Description
//! This embedded application source code illustrates how to implement a CDC application
//! with the ATmega32U4 controller.
//!
//! @section sample About the sample application
//! The sample code is delivered for EVK527.
//!
//! This application will enumerates as a CDC (communication device class) virtual COM port. The application
//! can be used as a USB to serial converter. 
//!
//! @section src_code About the source code
//! This source code is usable with the following compilers:
//! - IAR Embedded Workbench (5.11A and higher)
//! - AVRGCC (WinAVR 20080610 and higher).
//!
//! Support for other compilers may required modifications or attention for:
//! - compiler.h file 
//! - special registers declaration file
//! - interrupt subroutines declarations
//!
//! @section chg Changes
//! 2.0.1: Now does not allow to send data before end enumeration AND open port detection.
//!
//! @section arch Architecture
//! As illustrated in the figure bellow, the application entry point is located is the main.c file.
//! The main function first performs the initialization of a scheduler module and then runs it in an infinite loop.
//! The scheduler is a simple infinite loop calling all its tasks defined in the conf_scheduler.h file.
//! No real time schedule is performed, when a task ends, the scheduler calls the next task defined in
//! the configuration file (conf_scheduler.h).
//!
//! The sample dual role application is based on two different tasks:
//! - The usb_task  (usb_task.c associated source file), is the task performing the USB low level
//! enumeration process in device mode.
//! - The cdc task performs the loop back application between USB and USART interfaces.
//!
//! \image html arch_full.gif
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

#include "conf/config.h"
#include "modules/scheduler/scheduler.h"
#include "lib_mcu/wdt/wdt_drv.h"
#include "lib_mcu/power/power_drv.h"
#include "lib_mcu/usb/usb_drv.h"
#include "lib_mcu/util/start_boot.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

int main(void)
{
   Usb_enable_regulator();
#ifndef  __GNUC__
   Wdt_off();
#else
   wdt_reset();
   Wdt_clear_flag();
   Wdt_change_enable();
   Wdt_stop();
#endif
   start_boot_if_required();
   //Clear_prescaler();
   scheduler();
   return 0;
}

//! \name Procedure to speed up the startup code
//! This one increment the CPU clock before RAM initialisation
//! @{
#ifdef  __GNUC__
// Locate low level init function before RAM init (init3 section)
// and remove std prologue/epilogue
char __low_level_init(void) __attribute__ ((section (".init3"),naked));
#endif

#ifdef __cplusplus
extern "C" {
#endif
char __low_level_init()
{
  //Clear_prescaler();
  return 1;
}
#ifdef __cplusplus
}
#endif
//! @}
