/*
 * PC_uC_Interface.h
 *
 * Created: 09.12.2013 14:38:49
 *  Author: Bernd Krafft
 */ 


#ifndef PC_UC_INTERFACE_H_
#define PC_UC_INTERFACE_H_

#include <avr/io.h>
//#include <avr/wdt.h>
//#include <avr/power.h>
//#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include <stdlib.h>
#include "Descriptors.h"


#include <LUFA/Drivers/Board/LEDs.h>
#include <LUFA/Drivers/Board/Joystick.h>
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

void PC_uC_Interface(void);


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

#define  Led2_on()      (LED_PORT |= (1<<LED2_BIT))
#define  Led3_on()      (LED_PORT |= (1<<LED3_BIT))


#define  Led2_off()     (LED_PORT &= ~(1<<LED2_BIT))
#define  Led3_off()     (LED_PORT &= ~(1<<LED3_BIT))


#define  Led2_toggle()  (LED_PIN |= (1<<LED2_BIT))
#define  Led3_toggle()  (LED_PIN |= (1<<LED3_BIT))


#define  Is_led2_on()   (LED_PIN & (1<<LED2_BIT) ? TRUE : FALSE)
#define  Is_led3_on()   (LED_PIN & (1<<LED3_BIT) ? TRUE : FALSE)



#endif /* PC_UC_INTERFACE_H_ */