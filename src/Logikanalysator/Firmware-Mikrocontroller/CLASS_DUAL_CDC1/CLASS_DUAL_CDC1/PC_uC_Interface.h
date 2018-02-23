/*
 * PC_uC_Interface.h
 *
 * Created: 09.12.2013 14:38:49
 *  Author: Bernd Krafft
 */ 


#ifndef PC_UC_INTERFACE_H_
#define PC_UC_INTERFACE_H_

#define DEBUG_ENABLED 0
#define MAX_TIME_STAMP 0xFFFF

#define INIT_VCD_FILE "$date June 21, 2142 $end\r\n$version USB -TPLE 1.0 $end\r\n$comment 64 Bit Timestamp , 8Bit Data $end\r\n$timescale 160ns $end\r\n$scope module logic $end\r\n$var wire 8 % data $end\r\n$upscope $end\r\n$enddefinitions $end\r\n\r\n"

#include <avr/io.h>
//#include <avr/wdt.h>
//#include <avr/power.h>
//#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>

#include <stdint.h>
#define __STDC_FORMAT_MACROS 1 //für Typumwandlung von uint64_t in const char string
#include <inttypes.h> //für Typumwandlung von uint64_t in const char string

#include <stdio.h>

#include "Descriptors.h"


#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

//Prototypen
//uint64_t setVcdTimestampBase(uint64_t vcd_timestamp_base);
//uint64_t setVcdTimestampAkt(uint64_t vcd_timestamp_base,uint64_t zeitstempel);
void PC_uC_Interface(void);
uint64_t setVcdTimestamp(uint64_t* timestamp_base,uint64_t zeitstempel);
void writeVCDtoCDC(uint64_t zeitstempel,uint8_t messdaten,USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface);
void uint64_to_string(uint64_t zahl, char *cString);


//LEDs
#define  LED_PORT       PORTF
#define  LED_DDR        DDRF
#define  LED_PIN        PINF

#define  LED2_BIT       1
#define  LED3_BIT       0

#define  Leds_init()    (LED_DDR  |= (1<<LED2_BIT) | (1<<LED3_BIT))
#define  Leds_on()      (LED_PORT |= (1<<LED2_BIT) | (1<<LED3_BIT))
#define  Leds_off()     (LED_PORT &= ~((1<<LED2_BIT) | (1<<LED3_BIT)))
#define  Leds_set_val(val) (Leds_off(),LED_PORT |= (val<<6)&((1<<LED2_BIT) | (1<<LED3_BIT)))
#define  Leds_get_val()    (LED_PORT>>6)

#define  Led2_on()		(LED_PORT &= ~(1<<LED2_BIT))  
#define  Led3_on()		(LED_PORT &= ~(1<<LED3_BIT))  

#define  Led2_off()		(LED_PORT |= (1<<LED2_BIT))
#define  Led3_off()		(LED_PORT |= (1<<LED3_BIT))


#define  Led2_toggle()  (LED_PIN |= (1<<LED2_BIT))
#define  Led3_toggle()  (LED_PIN |= (1<<LED3_BIT))


#define  Is_led2_on()   (LED_PIN & (1<<LED2_BIT) ? true : false)
#define  Is_led3_on()   (LED_PIN & (1<<LED3_BIT) ? true : false)

#define HELP_STRING "\r\nPC_uC_Interface\r\ng - (go) Messung starten\r\ns - (stop) Messung stoppen\r\nd - (dump) Daten auslesen\r\nr - (reset) Reset von CPLD\r\ni - (identify) wird fuer sigrok benoetigt\r\nh - help\r\nS - Status von CPLD\r\nD - (dump) Daten auslesen <VCD-Format>\r\nB - Springe in uC-Bootloader\r\nJ - CPLD-JTAG-Mode\r\n\n - Rueckmeldungen: 'r' run ; 's' stop ; 'f' RAM Speicher voll\r\n"


#endif /* PC_UC_INTERFACE_H_ */