/*
 * PC_uC_Interface.c
 *
 * Created: 09.12.2013 14:39:19
 *  Author: bernd
 */ 
#warning PC_uC_Interface

#include "PC_uC_Interface.h"
#include "uC_CPLD_Interface.h"



//Globale Variablen  //Externe Variablen
extern USB_ClassInfo_CDC_Device_t VirtualSerial1_CDC_Interface;
uint8_t init_firsttime=0;
uint8_t correct_command='r';



void PC_uC_Interface(void)
{
	Leds_init();		
	if(!init_firsttime)		
	{
		
		Leds_on();
		_delay_ms(10);
		Led2_toggle();
		_delay_ms(10);
		Led2_toggle();
		_delay_ms(10);
		Led2_toggle();
		
		CPLD_bus_init(); //Pinbelegung Bus I/O
		M_rw_on();  //dem CPLD die schreibrechte entziehen

		//CPLD_reset liefert am besten 1 zurück nach erfolgreichem reset.
		//CPLD_reset_and_stat();
		
		init_firsttime=1;
	}	
	
	int16_t ReceivedByte = CDC_Device_ReceiveByte(&VirtualSerial1_CDC_Interface);
	if (!(ReceivedByte < 0))
		{
			//CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)ReceivedByte);
			char Buffer[20];
			uint16_t data;			
			switch(	(uint8_t)ReceivedByte)
			{
				/*
				'g' (go) 	Messung starten
				's' (stop) 	Messung stoppen
				'd' (data/dump) 	Auslesen der Daten starten
				'r' (reset) 	Reset
				*/
				
				case 'g':			
				case 'G':	//CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'g');
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"g\r\n");
							CPLDWriteByte(0x04);
							
							Led2_on();		
							break;
							
				case 's':
				case 'S':	//CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'s');
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"s\r\n");
							CPLDWriteByte(0x05);
							
							Led2_off();
							break;
				
				case 'd':
				case 'D':	CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'d');
							CPLDReadyForReadData();
							break;
				case 'l':
				case 'L':   
				
							for(int i=10000;i>=0;i--)
							{	//4Mal auslesen solange i nicht erreicht ist 
								//wenn in den ausgelesenen Daten das ende noch nicht errecht ist, =Pending
								data = CPLDRead_regData(0); // 0 für Daten auslesen ; 1 für Status auslesen
								itoa(data,Buffer,16);
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface, Buffer);
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface, "\r\n");
							
								if(i % 500==0)Led2_toggle();
							}
							break;
				
				case 'r':
				case 'R':	CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'r');
							CPLDWriteByte(0x77);
							
							Led2_off();
							break;
							
				case 'i':				
				case 'I':	CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"\r\ni: 0x");
							uint16_t status=CPLDReadStatus();
							uint8_t statusl=status;
							uint8_t statush=status>>8;
							
							//char* s_statusl[] = itoa(statusl,s_statusl,16)
							char Buffer[20];
							itoa(status,Buffer,16);
							//Buffer[4]='\0';
							
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface, Buffer);
							//CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, statush);
							//CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, statusl);
							Led3_toggle();
							
							break;
				
					
				case 'h':
				case 'H':	
							//const char* infotext= "\r\nPC_uC_Interface\r\ng - go\r\ns - stop\r\nd - dump data\r\nr - reset CPLD\r\ni - information status\r\nh - help\r\n";
							//for(int i=0;i<strlen(infotext);i++ ){
								//
								//CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, infotext[i]);
							//}
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"\r\nPC_uC_Interface\r\ng - go\r\ns - stop\r\nd - dump data\r\nr - reset CPLD\r\nl - auslesen Leitungen\r\ni - information status\r\nh - help\r\n");
							Led2_off();
				break;
							
				default:	CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'E');
			}
			
			correct_command= (uint8_t) ReceivedByte;
		}
	
	
}




