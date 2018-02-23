/*
 * uC_CPLD_Interface.h
 *
 * Created: 10.12.2013 21:09:47
 *  Author: bernd
 */ 


#ifndef UC_CPLD_INTERFACE_H_
#define UC_CPLD_INTERFACE_H_

/*
Funktion				Mikrocontroller		Richtung			CPLD

M_data[0]				PD0					<-->				51
M_data[1]				PD1					<-->				50
M_data[2]				PD2					<-->				49
M_data[3]				PD3					<-->				48
M_rw					PD5					--->				47
M_clk					PC7					--->				14
M_nib_sel				PD4					--->				42
M_reg_sel[0]			PD6					--->				41
M_reg_sel[1]			PD7					--->				40
M_int					PB4					<---				39

*/

#define DELAY_US  1 //5.0 //1.0?

#define  DATA_PORT      PORTD
#define  DATA_DDR       DDRD
#define  DATA_PIN       PIND

#define M_DATA_MASK		0x0F
#define M_data0			PD0
#define M_data1			PD1			
#define M_data2			PD2			
#define M_data3			PD3			
#define M_rw			PD5	
#define M_nib_sel		PD4
#define M_reg_sel0		PD6
#define M_reg_sel1		PD7

		
#define  CLK_PORT      PORTC
#define  CLK_DDR       DDRC
#define  CLK_PIN       PINC

#define M_clk			PC7	
		
		
#define  INT_PORT      PORTB
#define  INT_DDR       DDRB
#define  INT_PIN       PINB

#define M_int			PB4			

/* */
#define  DATA_init_all_output()		(DATA_DDR  |= (1<<M_data0) | (1<<M_data1)  | (1<<M_data2)  | (1<<M_data3) | (1<<M_rw) | (1<<M_nib_sel)  | (1<<M_reg_sel0)  | (1<<M_reg_sel1) )											 
#define  DATA_init_output()			(DATA_DDR  |= (1<<M_data0) | (1<<M_data1)  | (1<<M_data2)  | (1<<M_data3)  )
#define  DATA_init_input()			(DATA_DDR  &= ~((1<<M_data0) | (1<<M_data1)  | (1<<M_data2)  | (1<<M_data3)) )
#define  CLK_init()    (CLK_DDR  |= (1<<M_clk) )
#define  INT_init()    (INT_DDR  &= ~(1<<M_int) )


#define	 M_clk_on()		(CLK_PORT |= (1<<M_clk))
#define	 M_clk_off()	(CLK_PORT &= ~(1<<M_clk))

#define  M_nib_sel_on()		(DATA_PORT |= (1<<M_nib_sel))
#define  M_nib_sel_off()	(DATA_PORT &= ~(1<<M_nib_sel))

#define  M_rw_on()		(DATA_PORT |= (1<<M_rw))
#define  M_rw_off()		(DATA_PORT &= ~(1<<M_rw))



#define Is_int_on()	(INT_PIN & (1<<M_clk) ? TRUE : FLALSE )

/* Bit setzen */ 
//#define set_bit(var, bit) ((var) |= (1 << (bit))) //unschöne hilfsfunktion laut mc.net

/* Bit löschen */
//#define clear_bit(var, bit) ((var) &= (unsigned)~(1 << (bit))) //unschöne hilfsfunktion laut mc.net



//-----------




//Prototypen
void CPLD_bus_init(void);
uint16_t CPLDReadStatus();
void M_reg_sel_11();
void M_reg_sel_10();
void M_reg_sel_01();
void M_reg_sel_00();
void M_data(uint8_t data);
void CPLDWriteByte(uint8_t data);
void CPLDReadyForReadData();
uint16_t CPLDRead_regData(uint8_t reg1);



#endif /* UC_CPLD_INTERFACE_H_ */

