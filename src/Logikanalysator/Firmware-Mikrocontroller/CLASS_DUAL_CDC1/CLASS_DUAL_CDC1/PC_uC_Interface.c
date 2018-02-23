/*
 * PC_uC_Interface.c
 *
 * Created: 09.12.2013 14:39:19
 *  Author: bernd
 */ 


#include "PC_uC_Interface.h"
#include "uC_CPLD_Interface.h"

//#warning F_CPU 16000000ul

//Globale Variablen  //Externe Variablen
extern USB_ClassInfo_CDC_Device_t VirtualSerial1_CDC_Interface;
extern USB_ClassInfo_CDC_Device_t VirtualSerial2_CDC_Interface;
extern int doJTAG;


uint8_t init_firsttime=0;
uint8_t pending_data=0;

uint8_t ascii_format=0;
uint8_t vcd_init=0;
uint64_t vcd_timestamp_base=0; //Zeitbasis- (0xFFFF) auf die der Offset, wenn nicht 0xFFFF, dazu addiert wird						


void PC_uC_Interface(void)
{
	Leds_init();		
	
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
			
		if(Is_M_int_on()) 
		{ //RAM ist voll
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
				\r\nD - (dump) Daten auslesen <VCD-Format>
				\r\nB - Springe in uC-Bootloader
				\r\nJ - CPLD mit STAPL-Player flashen (JTA
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
							//CDC_Device_Flush(&VirtualSerial1_CDC_Interface); //TODO: wird möglicherweise für sigrok benötigt
				
							Led3_toggle();
							break;
							
				case 'D': 	// dump-ASCII->
							{
								ascii_format=1;
								vcd_init=1;
								vcd_timestamp_base=0; //Zeitbasis- (0xFFFF) auf die der Offset, wenn nicht 0xFFFF, dazu addiert wird
							}
				
				case 'd':	// dump->
							//Statusbit 0 wird überprüft - 1=run; 0=stop
							if( CheckStatusBit(0,CPLDReadStatus() ) )
							{
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"r");
									
							}else
							{
								CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"s");
								CPLDReadyForReadData(); //CPLDReadyForReadData() wird benötigt!
								//CPLDWriteByte(CMD_DUMMY_BYTE);
								//CPLDWriteByte(CMD_STATUS_BYTE);
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
							
				case 'J':	//TODO: In Windows funktioniert bei der 3. CDC das echo (mit 3VirtualSerial.inf)!! in Linux wird nix zurückgegeben (Treiberproblem?)
							doJTAG=1; //Danach ist ein reset von uC nötig für weitere Befehle
							//CDC_Device_SendString(&VirtualSerial1_CDC_Interface,"J");
							break;
				
					
				case 'h':
				case 'H':	
							
							CDC_Device_SendString(&VirtualSerial1_CDC_Interface, HELP_STRING);
							Led3_toggle();
				break;
				
				

							
				default://CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)'E'); //obsolet
						break;
			}
			
			
		}
		
	}else
	{ // pending_data
		if(DEBUG_ENABLED) CDC_Device_SendString(&VirtualSerial1_CDC_Interface, "\r\nIsPendingData");
		

		
		int i;
		for(i=10000;i>=0 && pending_data;i--) //Diese for-schleife ist wichtig für den USB-Lufa-Stack
		{		
			
			if(ascii_format)
			{//-----------------------------ascii_format
			
				uint8_t messdaten,status_byte;
				uint16_t zeitstempel; 
			
				uint8_t islast = CPLDRead_MeasurementData(&messdaten, &zeitstempel, &status_byte);
				
				// Auswertung des Zeitstempels für VCD-Datei			
				uint64_t vcd_timestamp_akt = setVcdTimestamp(&vcd_timestamp_base,zeitstempel);	
			
				if(islast)
				{//is last data
					
					writeVCDtoCDC(vcd_timestamp_akt , messdaten, VirtualSerial2_CDC_Interface);
					
					pending_data=0;
					ascii_format=0;
					vcd_init=1;
					vcd_timestamp_base=0;
				
				}else//not last data
				{
					
					/*
					$date June 21, 2142 $end
					$version USB -TPLE 1.0 $end
					$comment 64 Bit Timestamp , 8Bit Data $end
					$timescale 160ns $end
					$scope module logic $end
					$var wire 8 % data $end
					$upscope $end
					$enddefinitions $end

					#0
					b00000000 %
				
					#2303
					b00000010 %
					#56843
					b10000010 %
					
					#65535
					b10000010 %
					
					#85535
					b10000011 %
					
					*/
					if(vcd_init)
					{//Header der VCD-Strings
						//TODO: Flush nötig?
						CDC_Device_Flush(&VirtualSerial2_CDC_Interface);
						
						CDC_Device_SendString(&VirtualSerial2_CDC_Interface, INIT_VCD_FILE);
						vcd_init=0;
					}
					
					//void writeVCDTimeAndMdata(vcd_timestamp_akt,messdaten);
					writeVCDtoCDC(vcd_timestamp_akt , messdaten, VirtualSerial2_CDC_Interface);
					
				}
			
				if(i % 200==0)Led3_toggle();
				if(i % 100==0)Led2_toggle();
				
			}else//----------------------------------ascii_format end --------------------
			{
				
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
		
								
				}else
				{//not last data
								
					CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, messdaten);
					CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)((zeitstempel & 0xff00) >> 8));
					CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, (uint8_t)(zeitstempel & 0xff));
					CDC_Device_SendByte(&VirtualSerial1_CDC_Interface, status_byte);
								
				}

				if(i % 200==0)Led3_toggle();
				if(i % 100==0)Led2_toggle();
			}
		}
		
	}
	
}

//uint64_t setVcdTimestampBase(uint64_t vcd_timestamp_base,uint64_t zeitstempel)
//{
	//if(zeitstempel==MAX_TIME_STAMP) vcd_timestamp_base += zeitstempel; //
	//return vcd_timestamp_base;
//}
//
//uint64_t setVcdTimestampAkt(uint64_t vcd_timestamp_base,uint64_t zeitstempel)
//{
	//if(vcd_timestamp_base)
	//return ;
//}
//
uint64_t setVcdTimestamp(uint64_t* timestamp_base,uint64_t zeitstempel)
{//returnts the aktual timestamp
	if(*timestamp_base==0)
	{//TODO: möglicherweise obsolet da erster Zeitstempel immer 0?
		 *timestamp_base=zeitstempel;
		 return zeitstempel;	 
	}else if(zeitstempel==MAX_TIME_STAMP)
	{//neue timestamp_base erst Speichern wenn MAX_TIME_STAMP erreicht
//		*vcd_timestamp_base+=zeitstempel;
		*timestamp_base= *timestamp_base + zeitstempel;
		return *timestamp_base;
	}else
	{
		return *timestamp_base + zeitstempel;
	}
}

void writeVCDtoCDC(uint64_t zeitstempel,uint8_t messdaten,USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface)
{//schreibe Zeitstempel und Messdaten in VCD-Format
	//#56843
	//b10000010 %
				
	char cBuffer[42];//64-bit Zahl max 20 stellen
	//char szeitstempel[31]= "TEST_String";
	//char *blub;
	
	//char* szeitstempel;
	//szeitstempel= "blub";
	
	//TODO: umwandlung von uint64_t in const char string
	
	//sprintf(szeitstempel,"%I64u",zeitstempel);
	//sprintf(szeitstempel,"%llu",zeitstempel); //nein 
	//sprintf(szeitstempel,"%lld",zeitstempel);   //nein
	//sprintf(szeitstempel, "#%" PRIu64 "\r\n", zeitstempel);
	//sprintf(szeitstempel,"TODOtest:%llu", 1234567890ull );
	//sprintf(szeitstempel,"%Lu", zeitstempel);
	//snprintf(szeitstempel, sizeof szeitstempel, "%"PRIu64, zeitstempel);
	//snprintf (szeitstempel, sizeof(szeitstempel)-1, "%llu", (unsigned long long) zeitstempel); //nein
	//sprintf(szeitstempel,"% PRIu64 ",zeitstempel); 
	//sprintf(szeitstempel,"%" SCNx64,zeitstempel);	
	
	/*blub = *///uint64_to_string(/*zeitstempel*/17234567842ull,cBuffer); //DEBUG
	
	//utoa(zeitstempel,cBuffer,10);
	//uint64_to_string(/*zeitstempel*/17234567842ull,cBuffer);
	uint64_to_string(zeitstempel,cBuffer);
	CDC_Device_SendString(&VirtualSerial_CDC_Interface, "#");
	//CDC_Device_SendString(&VirtualSerial_CDC_Interface, blub /*szeitstempel */); //TODO:ist noch fehlerhaft
	CDC_Device_SendString(&VirtualSerial_CDC_Interface, cBuffer /*szeitstempel */); //TODO:ist noch fehlerhaft
	//CDC_Device_SendString(&VirtualSerial_CDC_Interface, blub /*szeitstempel*/ );
	CDC_Device_SendString(&VirtualSerial_CDC_Interface, "\r\n");
	
	
	//uint64_to_string(/*zeitstempel*/18234567842ull,cBuffer); 
	//CDC_Device_SendString(&VirtualSerial_CDC_Interface, "#");
	//CDC_Device_SendString(&VirtualSerial_CDC_Interface, blub /*szeitstempel*/ );
	//CDC_Device_SendString(&VirtualSerial_CDC_Interface, cBuffer /*szeitstempel */); //TODO:ist noch fehlerhaft
	//CDC_Device_SendString(&VirtualSerial_CDC_Interface, blub /*szeitstempel*/ );
	//CDC_Device_SendString(&VirtualSerial_CDC_Interface, "\r\n");
	
	
	utoa(messdaten,cBuffer,2);
	CDC_Device_SendString(&VirtualSerial_CDC_Interface, "b");
	CDC_Device_SendString(&VirtualSerial_CDC_Interface, cBuffer);
	CDC_Device_SendString(&VirtualSerial_CDC_Interface, " %\r\n\r\n");
	
/*	if(messdaten>0){
		//TODO: 0en am anfang einfügen falls nötig
		utoa(messdaten,cBuffer,2);
		CDC_Device_SendString(&VirtualSerial_CDC_Interface, "b");
		CDC_Device_SendString(&VirtualSerial_CDC_Interface, cBuffer);
		CDC_Device_SendString(&VirtualSerial_CDC_Interface, " %\r\n\r\n");

	}else
	{

		CDC_Device_SendString(&VirtualSerial_CDC_Interface, "b0 %\r\n\r\n");

	}*/
}

void uint64_to_string(uint64_t zahl, char *cString) //sprintf und utoa funktionieren nicht
{
	uint64_t i;
	uint8_t flag=0;
	
	if(zahl>18000000000ull)
	{ 
		*cString='-';
		cString++;
		*cString='1';
		cString++;
		*cString='\0';
		return ;
	}
	
	for(i=1;i<=100000000000;i*=10)
	{
		if(zahl/(100000000000/i))
		{
			*cString=((zahl/(100000000000/i))+'0');
			cString++;
			flag=1;
		}
		else if(flag)
		{
			*cString='0';
			cString++;
		}
		zahl=zahl%(100000000000/i);
	}
	
	*cString='\0';
	if(flag==0)
	{
		*cString='0';
		cString++;
		*cString='\0';
	}
	
	//return cString;
}