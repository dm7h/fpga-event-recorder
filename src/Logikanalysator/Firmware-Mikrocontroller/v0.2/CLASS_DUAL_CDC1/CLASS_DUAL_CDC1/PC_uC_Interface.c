/*
 * PC_uC_Interface.c
 *
 * Created: 09.12.2013 14:39:19
 *  Author: bernd
 */ 


#include "PC_uC_Interface.h"
#include "uC_CPLD_Interface.h"



//Globale Variablen  //Externe Variablen
extern USB_ClassInfo_CDC_Device_t VirtualSerial1_CDC_Interface;
uint8_t init_firsttime=0;
//uint8_t correct_command='r';
uint8_t pending_data=0;



void PC_uC_Interface(void)
{
	Leds_init();		
	
	
	//char Buffer[20];
	
	if(!init_firsttime)		
	{
		Leds_on();
		if(DEBUG_ENABLED)
		{
			int i;
			for(i=0;i<10;i++)
			{
				Led2_toggle();
				_delay_ms(10);
			}	
		}
		
		CPLD_bus_init(); //Pinbelegung Bus I/O
		M_rw_on();  //dem CPLD die schreibrechte entziehen
		CPLDWriteByte(CMD_DUMMY_BYTE);
		CPLDWriteByte(CMD_RESET_BYTE);
		
		//TODO: use M_int as Interrupt / not polling
		
		
		init_firsttime=1;
	}	
	
	
	
	if(!pending_data)
	{
			

		//itoa(pending_data,Buffer,10);
		//CDC_Device_SendString(&VirtualSerial1_CDC_Interface,Buffer);
		
		if(Is_M_int_on()) 
		{
			CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"f");
			CPLDReadStatus();	
		}
		
		
		int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial1_CDC_Interface);
		if (!(ReceivedByte < 0))
		{	
			switch(	(uint8_t)ReceivedByte)
			{
				/*
				"\r\nPC_uC_Interface
				\r\ng - (go) Messung starten
				\r\ns - (stop) Messung stoppen
				\r\nd - (dump) Daten auslesen
				\r\nr - (reset) Reset von CPLD
				\r\ni - (identify) wird fuer sigrok benoetigt
				\r\nh - help
				\r\nS - Status von CPLD
				\r\nB - Springe in uC-Bootloader
				\r\n\n - Rueckmeldungen: 'r' run ; 's' stop ; 'f' RAM Speicher voll\r\n"				
				*/
				
				case 'g':	// go->
							CPLDWriteByte(CMD_DUMMY_BYTE);
							CPLDWriteByte(CMD_RESET_BYTE);
							CPLDWriteByte(CMD_DUMMY_BYTE);
							CPLDWriteByte(CMD_START_MEASUREMENT_BYTE);
						
							//Statusbit 0 wird überprüft - 1=run; 0=stop
							if( CheckStatusBit(0,CPLDReadStatus() ) )
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"r");
							else
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"s");
							
							
							Led3_toggle();		
							break;
							
				case 's':	// stop->
							CPLDWriteByte(CMD_DUMMY_BYTE);
							CPLDWriteByte(CMD_STOP_MEASUREMENT_BYTE);
							
							//Statusbit 0 wird überprüft  - 1=run; 0=stop
							if( CheckStatusBit(0,CPLDReadStatus() ) )
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"r");
							else
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"s");
							
							Led3_toggle();	
							break;
							
				case 'r':	// reset->
							//CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'r');
							CPLDWriteByte(CMD_DUMMY_BYTE);
							CPLDWriteByte(CMD_RESET_BYTE);
				
							Led3_toggle();
							break;
				
				case 'd':	// dump->
							//Statusbit 0 wird überprüft - 1=run; 0=stop
							if( CheckStatusBit(0,CPLDReadStatus() ) )
							{
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"r");
									
							}else
							{
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"s");
								//CPLDReadyForReadData(); TODO: CPLDReadyForReadData() wird wohl nicht mehr benötigt
								CPLDWriteByte(CMD_DUMMY_BYTE);
								CPLDWriteByte(CMD_STATUS_BYTE);
								pending_data=1;	
															
							}
							
							Led3_toggle();
							Led2_toggle();
							break;
			

							
				case 'i':	// identify->
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"HSA-TPLE");
							break;
									
									
				case 'S'://-Status->
							//Statusbit 0 wird überprüft  - 1=run; 0=stop
							if( CheckStatusBit(0,CPLDReadStatus() ) )
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"r");
							else
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"s");
					
							Led3_toggle();	
							break;
									
				case 'B':  //bootloader
							Led3_off();
							Led2_off();
							bootloader();
							break;
											
				//case 'j':				
				//case 'J':	CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"\r\ni: 0x");
							//uint16_t status=CPLDReadStatus();
							//uint8_t statusl=status;
							//uint8_t statush=status>>8;
							//
							////char* s_statusl[] = itoa(statusl,s_statusl,16)
							//char Buffer[20];
							//itoa(status,Buffer,16);
							////Buffer[4]='\0';
							//
							//CDC_Device_SendString(&VirtualSerial1_CDC_Interface, Buffer);
							////CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, statush);
							////CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, statusl);
							//Led3_toggle();
							//
							//break;
				
					
				case 'h':
				case 'H':	
							
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface, HELP_STRING);
							Led3_toggle();
				break;
				
				

							
				default:	CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'E');
			}
			
			//correct_command= (uint8_t) ReceivedByte;
		}
		
	}else
	{ // pending_data
		if(DEBUG_ENABLED) CDC_Device_SendString(&VirtualSerial1_CDC_Interface, "\r\nIsPendingData");
		

		
		int i;
		for(i=10000;i>=0 && pending_data;i--) //Diese for-schleife ist wichtig für den USB-Lufa-Stack
		{				
			//itoa(data,Buffer,16);
			//CDC_Device_SendString(&VirtualSerial1_CDC_Interface, Buffer);
			//CDC_Device_SendString(&VirtualSerial1_CDC_Interface, "\r\n");
			
			uint8_t messdaten,status_byte;
			uint16_t zeitstempel; 
			
			//uint8_t islast = CPLDRead_MeasurementData(&messdaten, &zeitstempel, &status_byte);
			
			
			
			if(CPLDRead_MeasurementData(&messdaten, &zeitstempel, &status_byte) )
			{//is last data
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, messdaten);
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)((zeitstempel & 0xff00) >> 8));
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)(zeitstempel & 0xff));
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, status_byte);
				pending_data=0;
				//TODO: Auswertung des Zeitstempels in uC?
				
				
			}else
			{//not last data
				
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, messdaten);
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)((zeitstempel & 0xff00) >> 8));
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)(zeitstempel & 0xff));
				CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, status_byte);
				
				//if(DEBUG_ENABLED && i>9000) status_byte=0xff;
				
				//itoa(messdaten,Buffer,16);
				//CDC_Device_SendString(&VirtualSerial1_CDC_Interface, Buffer);
				//CDC_Device_SendString(&VirtualSerial1_CDC_Interface, "\r\n");
			}

			if(DEBUG_ENABLED)
			{
				 //CDC_Device_SendString(&VirtualSerial1_CDC_Interface, "\r\ni:");
				 //itoa(i,Buffer,10);
				 //CDC_Device_SendString(&VirtualSerial1_CDC_Interface, Buffer );
				 
				 
			}
			if(i % 200==0)Led3_toggle();
			if(i % 100==0)Led2_toggle();
		}
		
	}
	
	
}




