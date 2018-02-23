/*
 * uC_CPLD_Interface.c
 *
 * Created: 10.12.2013 20:42:35
 *  Author: bernd
 */ 

/*
 * Test_1_1.cpp
 *
 * Created: 16.11.2013 10:56:29
 *  Author: Patrick Echter
 *
 */ 


#include <avr/io.h>
//#include <avr/interrupt.h>
#include <stdbool.h> 
#include <util/delay.h>

#include "uC_CPLD_Interface.h"

//#################### Nicht benutzen geht nicht ######################
//// set bit
//static inline void BIT_SET(volatile uint8_t *target, uint8_t bit) __attribute__((always_inline));
//static inline void BIT_SET(volatile uint8_t *target, uint8_t bit){
	//*target |= (1<<bit);
//};
//
//// set clear
//static inline void BIT_CLEAR(volatile uint8_t *target, uint8_t bit) __attribute__((always_inline));
//static inline void BIT_CLEAR(volatile uint8_t *target, uint8_t bit){
	//*target &= ~(1<<bit);
//};

void CPLD_bus_init(void){
	DATA_init_all_output();
	CLK_init();
	INT_init();
}

/*void CPLDSendData(uint8_t data)
{

	CLK_off();

}*/

/*uint16_t CPLDReadRead(void)
{
	uint16_t data= 0x0000;
	
	DATA_init_output();
	
	_delay_us(DELAY_US);
	
	return 0x0000;
}*/

uint16_t CPLDReadStatus()
{
	uint16_t status;//=0x0000; //0x4142; //AB
	
	DATA_init_output();

	CPLDWriteByte(CMD_DUMMY_BYTE); //Dummy Byte
	
	CPLDWriteByte(CMD_STATUS_BYTE); //Status Byte 0x06	
	
	status = CPLDRead_regData(1);// 1 für status Auslesen
					// Daten Empfangen --------------------
	
	/*
	M_clk_off();	
	M_nib_sel_off();
	M_reg_sel_10(); //Low Nibble von Status empfangen
	M_rw_off();
	DATA_init_input(); //M_data auf empfang configurieren
	_delay_us(DELAY_US);
	
	M_clk_on();			// 1 CLK
	_delay_us(DELAY_US);
	
	M_clk_off();    
	M_nib_sel_on();
	//Daten von M_data auslesen
	status |= ((DATA_PIN & 0x0F)<< 0 );//Low Nibble von Status0-7 empfangen
	_delay_us(DELAY_US);
	
	M_clk_on();			//2 CLK
	_delay_us(DELAY_US);
	
	M_clk_off();
	M_nib_sel_off();
	M_reg_sel_11();	
	status |= ((DATA_PIN & 0x0F)<< 4 ); //High Nibble von Status0-7 empfangen
	_delay_us(DELAY_US);
	
	M_clk_on();			//3 CLK
	_delay_us(DELAY_US);
	
	M_clk_off();
	M_nib_sel_on();
	status |= ((DATA_PIN & 0x0F)<< 8 );//Low Nibble von Status8-15 empfangen
	_delay_us(DELAY_US);
	
	M_clk_on();			//4 CLK
	_delay_us(DELAY_US);
	
	M_clk_off();
	M_nib_sel_off(); //verm. nicht nötig da letztes High Nibble
	M_reg_sel_10();	 //verm. nicht nötig da letztes High byte 
	status |= ((DATA_PIN & 0x0F)<< 12 ); //High Nibble von Status0-7 empfangen	
	_delay_us(DELAY_US);
	
	M_rw_on(); // Dem CPLD die Schreibrechte entziehen!
	DATA_init_output(); //Datenleitungen auf Senden configurieren
	
		*/
	return status;
	
}

void CPLDReadyForReadData()
{
	
	
	//CPLD_bus_init();
	
	DATA_init_output();

	CPLDWriteByte(CMD_DUMMY_BYTE); //Dummy Byte
	
	
	CPLDWriteByte(CMD_STATUS_BYTE); //Status Byte 0x06
	
	

	
}

uint16_t CPLDRead_regData(uint8_t reg1){
	//reg1 == 1 - Es wird das StatusRegister ausgelesen
	//reg1 == 0 - Es wir das DatenRegister ausgelesen
	
	uint16_t status=0x0000; //0x4142; //AB
	
	// Daten Empfangen --------------------
	M_clk_off();
	M_nib_sel_off();
	if(reg1)M_reg_sel_10(); //Low Nibble von Status empfangen
	else M_reg_sel_00();
	M_rw_off();
	DATA_init_input(); //M_data auf empfang konfigurieren
	_delay_us(DELAY_US);

	M_clk_on();			// 1 CLK
	_delay_us(DELAY_US);

	M_clk_off();
	M_nib_sel_on();
	//Daten von M_data auslesen
	status |= ((DATA_PIN & 0x0F)<< 0 );//Low Nibble von Status0-7 empfangen
	_delay_us(DELAY_US);

	M_clk_on();			//2 CLK
	_delay_us(DELAY_US);

	M_clk_off();
	M_nib_sel_off();
	if(reg1)M_reg_sel_11();
	else M_reg_sel_01();
	status |= ((DATA_PIN & 0x0F)<< 4 ); //High Nibble von Status0-7 empfangen
	_delay_us(DELAY_US);

	M_clk_on();			//3 CLK
	_delay_us(DELAY_US);

	M_clk_off();
	M_nib_sel_on();
	status |= ((DATA_PIN & 0x0F)<< 8 );//Low Nibble von Status8-15 empfangen
	_delay_us(DELAY_US);

	M_clk_on();			//4 CLK
	_delay_us(DELAY_US);

	M_clk_off();
	//M_nib_sel_off(); //verm. nicht nötig da letztes High Nibble
	//M_reg_sel_10();	 //verm. nicht nötig da letztes High byte
	status |= ((DATA_PIN & 0x0F)<< 12 ); //High Nibble von Status0-7 empfangen
	_delay_us(DELAY_US);

	//M_rw_on(); // Dem CPLD die Schreibrechte entziehen!
	//DATA_init_output(); 
	
	return status;
}

uint8_t CPLDRead_MeasurementData(uint8_t *messdaten, uint16_t *zeitstempel, uint8_t *status_byte)
{   // gibt 1 zurück wenn letzeter Datensatz
	uint16_t data_h, data_l;
	*zeitstempel = 0x0000;
	*messdaten = 0x00;
	*status_byte = 0x00;
	uint8_t end=0;
	
	data_l = CPLDRead_regData(0); // 0 für Daten auslesen ; 1 für Status auslesen
	data_h = CPLDRead_regData(0); // 0 für Daten auslesen ; 1 für Status auslesen
	
	*messdaten = ( (data_h & 0xFF00) >> 8);
	
	*zeitstempel = ( (data_h & 0x00FF) << 8);
	*zeitstempel = *zeitstempel | ( (data_l & 0xFF00) >> 8);
	
	*status_byte = (data_l & 0x00FF);
	
	if (*status_byte==IS_LAST_DATA) end=1;
	else if(*status_byte==NOT_LAST_DATA) return end;
	
	return MEMORY_FAIL;
	
}
void M_reg_sel_11()
{

	//BIT_SET(DATA_PORT,M_reg_sel0);
	//BIT_SET(DATA_PORT,M_reg_sel1);

	DATA_PORT |= (1<<M_reg_sel0);
	DATA_PORT |= (1<<M_reg_sel1);

}

void M_reg_sel_10()
{

	//BIT_CLEAR(DATA_PORT,M_reg_sel0);
	//BIT_SET(DATA_PORT,M_reg_sel1);
	
	DATA_PORT &= ~(1<<M_reg_sel0);
	DATA_PORT |= (1<<M_reg_sel1);
}

void M_reg_sel_01()
{

	//BIT_SET(DATA_PORT,M_reg_sel0);
	//BIT_CLEAR(DATA_PORT,M_reg_sel1);
	
	DATA_PORT |= (1<<M_reg_sel0);
	DATA_PORT &= ~(1<<M_reg_sel1);
	
}

void M_reg_sel_00()
{

	//BIT_CLEAR(DATA_PORT,M_reg_sel0);
	//BIT_CLEAR(DATA_PORT,M_reg_sel1);
	
	DATA_PORT &= ~(1<<M_reg_sel0);
	DATA_PORT &= ~(1<<M_reg_sel1);
}

void M_data(uint8_t data)
{
	data &= 0x0F;
	DATA_PORT &= ~(M_DATA_MASK);
	DATA_PORT |= data;	
}

void CPLDWriteByte(uint8_t data)
{
	DATA_init_output();
		
	M_clk_off();
	M_nib_sel_off();
	//M_reg_sel_00();	//ist egal da nur 8bit geschrieben werdnen uC-->CPLD		
	M_rw_on();
	M_data(0x0F & data); //Low Nibble - Dummy Byte
	_delay_us(DELAY_US);
		
	M_clk_on();
	_delay_us(DELAY_US);
		
	M_nib_sel_on();
	M_data((data>>4)&0x0F); //High Nibble - Dummy Byte
	M_clk_off();
	_delay_us(DELAY_US);
		
	M_clk_on();
	_delay_us(DELAY_US);
	
	M_clk_off();
}

uint8_t CheckStatusBit (uint8_t bit, uint16_t status)
{
	if(bit>16) return 0;	
	return (status >> bit ) & 0x1;
	
}

void bootloader (void)
{
	MCUCR |=  (1 << IVCE);  //IVCE  = 1		//register für restet
	MCUCR |=  (1 << IVSEL); //IVSEL = 1		//register für restet
	TIMSK0 = 0;             //Timer-Interrupt ausschalten
	_delay_ms(50);
	asm volatile ("jmp 0x3800"); //springe an die richtige stelle
}

/*
// Zykluszeiten für die Tasks

//#define T0 100				// Task 0 Taktgenerator alle 50 ms



// PORTB				


//#define M_DATA7   PB7    //M_data
//#define M_DATA6   PB6    //M_data
//#define M_DATA5   PB5    //M_data
//#define M_DATA4   PB4    //M_data
#define M_RW      PB3    //M_rw Bit
//#define M_TODO    PB2	   //TODO: Signal?
#define M_NIB_SEL PB1    //M_nib_sel
#define M_CLK     PB0    //M_clk

//#define PORT_CPLD PORTB


//********************************************************************************
// Masken
//********************************************************************************
// Variablen

//enum eState{send_data_l, send_data_h, set_M_rw, send_done} eState_com; //cpp
typedef enum eState{send_data_l, send_data_h, set_M_rw, send_done} eState_com; 
	
	
volatile uint8_t	time0 = T0;			//Je Task ein Zähler für die Zykluszeiten
					

//uint8_t reload = 256 - 144;				// timer 0 neu laden (10 ms)
// 8MHz / 1024 ==> 0.128ms // 0.128ms * 78 = 9.984ms
uint8_t reload = 256 - 78;				// timer 0 neu laden (10 ms)
uint8_t tsk0flag = 0;					// Nachricht von Task 0 an Task 2

//********************************************************************************
//Funktionsprototypen
enum eState SendData(enum eState akt_state, uint8_t uDataH, uint8_t uDataL);
uint8_t RecieveData();

ISR(TIMER0_OVF_vect) //9.984ms
{
	//Timer 0 neu laden für Overflow nach 10 ms
	TCNT0=reload;
	
	//Task Timer bearbeiten
	if(time0) --time0;					//Task 0 Timer dekrementieren
	
}
//********************************************************************************
// Main
int blubmain(void)
{
	bool clk = false;
	bool nib_sel = false;
	uint8_t uNextData = 0x69; 
	uint8_t uData = 0x69;				//Datenregister uC --> CPLD
	uint8_t uDataL = 0x00;
	uint8_t uDataH = 0x00;
	uint8_t uDataR = 0x00;
	//eState_com = send_data_h; //cpp
	enum eState eState_com = send_data_h;
	DDRB = 0xFF;						//Port B als Ausgang
	PORTB = 0xFF;
	
	// Timer 0 Init
	
	TCNT0 = reload;
	//TIMSK = 1<<TOV0;					// turn on timer 0 overflow ISR
	//TIMSK undeclared
	TCCR0A &= ~(1<<WGM02);				//Timer0 Normal Mode TOV Flag set on Overflow - is default
	TCCR0B &= ~( (1<<WGM00) | (1<<WGM01) );
	
	//TCCR0 = 1<<CS02;					// prescalar to 256
	//TCCR0 undeclared
	TCCR0B |= (1<<CS00) | (1<<CS00);	// clk/1024 prescaler
	
	
	// Globalen Interrupt freigeben
	sei();
		
	//Hauptschleife
    while(1)
    {
		//Task 0: clk
        if (time0 == 0)
        {
			time0 = T0;								// Zähler für Zykluszeit neu laden
			
			if (uNextData != uData)					// Prüfen ob neue Daten vorhanden
			{
				uData = uNextData;
				
				uDataH = (uData	>> 4);				// Von 8 auf 4 Bit teilen
				uDataL = (uData >> 0) & 0x0F;
			
				uDataH = uDataH << 4;				// Daten an richtige stelle schieben
				uDataL = uDataL << 4;
				
				eState_com = send_data_h;			// auf Sendemodus umstellen
				DDRB = 0xFF;						// Auf Ausgänge umschalten
				PORTB |= (1<<M_RW);					// M_rw Bit löschen
			}
				
			// Clock
			if (clk)
			{
				PORTB |= (1<<M_CLK);
				clk = false;
				
				eState_com = SendData(eState_com,uDataH,uDataL);
			}
			else
			{
				PORTB &= ~(1<<M_CLK);
				clk = true;
				if(eState_com == send_done)				// prüfen auf Empfangen
				{
					uDataR = RecieveData();				// Daten Empfangen
					
					//TODO: Datenverarbeitung
				}
				
				if (nib_sel)
				{
					PORTB |= (1<<M_NIB_SEL);
					nib_sel = false;
				}
				else
				{
					PORTB &= ~(1<<M_NIB_SEL);
					nib_sel = true;
					
				}
			}
			
			
		}
	}
}

//eState SendData(eState akt_state, uint8_t uDataH, uint8_t uDataL) //cpp
enum eState SendData(enum eState akt_state, uint8_t uDataH, uint8_t uDataL)
{
	switch(akt_state)
	{
		case send_data_l:	PORTB &= 0x0F;
							//PORTB &= ~(1<<M_RW);
							PORTB |= uDataL;
							akt_state = set_M_rw;
		break;
		
		case send_data_h:	PORTB &= 0x0F;
							//PORTB &= ~(1<<M_RW);
							PORTB |= uDataH;
							akt_state =send_data_l;
		break;
		
		case set_M_rw:		DDRB = 0x0F;			// Auf Eingänge schalten
							PORTB &= ~(1<<M_RW);		// M_rw Bit setzten 
							akt_state = send_done;	// Senden abgeschlossen
		break;
		
		default:
		break;
		
	}
	
	return akt_state;
}

uint8_t RecieveData()
{
	uint8_t uData = PINB;
	//uint8_t uData = 0x8F;
	return uData;
}
*/