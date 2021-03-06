/*
             LUFA Library
     Copyright (C) Dean Camera, 2013.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2013  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the DualVirtualSerial demo. This file contains the main tasks of
 *  the demo and is responsible for the initial application hardware configuration.
 */

#include "DualVirtualSerial.h"
#include "PC_uC_Interface.h"





/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the first CDC interface,
 *  which sends strings to the host for each joystick movement.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial1_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = 0,
				.DataINEndpoint           =
					{
						.Address          = CDC1_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC1_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC1_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the second CDC interface,
 *  which echos back all received data from the host.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial2_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = 2,
				.DataINEndpoint           =
					{
						.Address          = CDC2_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC2_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC2_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},

			},
	};

/*
USB_ClassInfo_CDC_Device_t VirtualSerial3_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber   = 4,
				.DataINEndpoint           =
					{
						.Address          = CDC3_TX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.DataOUTEndpoint =
					{
						.Address          = CDC3_RX_EPADDR,
						.Size             = CDC_TXRX_EPSIZE,
						.Banks            = 1,
					},
				.NotificationEndpoint =
					{
						.Address          = CDC3_NOTIFICATION_EPADDR,
						.Size             = CDC_NOTIFICATION_EPSIZE,
						.Banks            = 1,
					},
			},
	};
*/
//static FILE USBSerialStream;

int doJTAG=0; //CPLD-JTAG

/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
int main(void)
{
	//int16_t c_in; //CPLD-JTAG
	//uint8_t c_out;//CPLD-JTAG
	
	/* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
	//CDC_Device_CreateStream(&VirtualSerial3_CDC_Interface, &USBSerialStream);
	 
	SetupHardware();

	GlobalInterruptEnable();
	
	if(DEBUG_ENABLED){	
	//PE6 Test USB-Stack Overhead
	DDRE |= (1<<PE6);
	PORTE &= ~(1<<PE6);
	}
	
	for (;;)
	{
		
		/* Discard all received data on the second CDC interface */
		CDC_Device_BytesReceived(&VirtualSerial2_CDC_Interface);	
		
		/* Discard all received data on the second CDC interface */
		//CDC_Device_BytesReceived(&VirtualSerial3_CDC_Interface);
			
		

		//TASKs--------------------------------------------
		CDC_Device_USBTask(&VirtualSerial1_CDC_Interface);
		CDC_Device_USBTask(&VirtualSerial2_CDC_Interface);	
		//CDC_Device_USBTask(&VirtualSerial3_CDC_Interface);
		
		/*
		while (CDC_Device_BytesReceived(&VirtualSerial3_CDC_Interface))
		CDC_Device_SendByte(&VirtualSerial3_CDC_Interface, CDC_Device_ReceiveByte(&VirtualSerial3_CDC_Interface));
				
		while (CDC_Device_BytesReceived(&VirtualSerial1_CDC_Interface))
		CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, CDC_Device_ReceiveByte(&VirtualSerial1_CDC_Interface));
		
		while (CDC_Device_BytesReceived(&VirtualSerial2_CDC_Interface))
		//CDC_Device_SendByte(&VirtualSerial2_CDC_Interface, CDC_Device_ReceiveByte(&VirtualSerial2_CDC_Interface));
		*/			
			
		USB_USBTask();
		
		if(DEBUG_ENABLED) PORTE |= (1<<PE6); //DEBUG Test wie lange die USB-Tasks brauchen
	
		PC_uC_Interface();
		
		/* //Vorbereitung f�r 3.CDC JTAG
		if (doJTAG==0) 
		{
			
		}
		else 
		{
						
			//initJTAG PORT	
			if(doJTAG <2 )
			{	// Set Data Direction Register for JTAG Port
				JTAG_DDR |=  ( (1 << JTAG_TCK) | (1 << JTAG_TDI) | (1 << JTAG_TMS) | (1 << JTAG_TDO_RDIND) ); // outputs
				JTAG_DDR &= ~(1 << JTAG_TDO); // inputs

				// Initialize Port with zeros (all outputs low/ off)
				JTAG_PORT = 0x00;
				
				doJTAG=2;
			}
			
			// Must throw away unused bytes from the host, or it will lock up while waiting for the device 
			c_in = CDC_Device_ReceiveByte(&VirtualSerial3_CDC_Interface);
			if(c_in > 0)
			{
			
				// write TCK, TDI, TMS
				JTAG_PORT = (uint8_t)c_in;

				// read TDO
				c_out = JTAG_PIN | (uint8_t)0x01; // will serial interface send if JTAG_PIN == 0x00?
						
				
				CDC_Device_SendByte(&VirtualSerial3_CDC_Interface, c_out );
				
				//ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial1_CDC_Interface);
				//ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial2_CDC_Interface);
			}

			//CDC_Device_USBTask(&VirtualSerial3_CDC_Interface);
			
		}*/
		
		//if(DEBUG_ENABLED) PORTE &= ~(1<<PE6);	//DEBUG
		
		//TASKs------------------- end --------------------
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	//Joystick_Init();
	//LEDs_Init();
	USB_Init();
}

/** Checks for changes in the position of the board joystick, sending strings to the host upon each change
 *  through the first of the CDC interfaces.
 */
void CheckJoystickMovement(void)
{
	uint8_t     JoyStatus_LCL = Joystick_GetStatus();
	char*       ReportString  = NULL;
	static bool ActionSent = false;

	if (JoyStatus_LCL & JOY_UP)
	  ReportString = "Joystick Up\r\n";
	else if (JoyStatus_LCL & JOY_DOWN)
	  ReportString = "Joystick Down\r\n";
	else if (JoyStatus_LCL & JOY_LEFT)
	  ReportString = "Joystick Left\r\n";
	else if (JoyStatus_LCL & JOY_RIGHT)
	  ReportString = "Joystick Right\r\n";
	else if (JoyStatus_LCL & JOY_PRESS)
	  ReportString = "Joystick Pressed\r\n";
	else
	  ActionSent = false;

	if ((ReportString != NULL) && (ActionSent == false))
	{
		ActionSent = true;

		CDC_Device_SendString(&VirtualSerial1_CDC_Interface, ReportString);
	}
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial1_CDC_Interface);
	ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial2_CDC_Interface);
	//ConfigSuccess &= CDC_Device_ConfigureEndpoints(&VirtualSerial3_CDC_Interface);

	LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial1_CDC_Interface);
	CDC_Device_ProcessControlRequest(&VirtualSerial2_CDC_Interface);
	//CDC_Device_ProcessControlRequest(&VirtualSerial3_CDC_Interface);
}

