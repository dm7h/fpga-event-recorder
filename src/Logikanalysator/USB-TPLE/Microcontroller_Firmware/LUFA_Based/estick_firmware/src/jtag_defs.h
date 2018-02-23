#ifndef __JTAG_DEFS_H__
#define __JTAG_DEFS_H__


//LEDs

	#define  LED_PORT       PORTF
	#define  LED_DDR        DDRF
	#define  LED_PIN        PINF
                    
	#define  LED1_BIT       PINF1
	#define  LED2_BIT       PINF0
        
	#define  Leds_init()    (LED_DDR  |= (1<<LED1_BIT) | (1<<LED2_BIT))
	#define  Leds_on()      (LED_PORT |= (1<<LED1_BIT) | (1<<LED2_BIT))
	#define  Leds_off()     (LED_PORT &= ~((1<<LED1_BIT) | (1<<LED2_BIT)))
	#define  Leds_set_val(val) (Leds_off(),LED_PORT |= (val<<6)&((1<<LED1_BIT) | (1<<LED2_BIT)))
	#define  Leds_get_val()    (LED_PORT>>6)

	#define  Led1_on()      (LED_PORT |= (1<<LED1_BIT))
	#define  Led2_on()      (LED_PORT |= (1<<LED2_BIT))


	#define  Led1_off()     (LED_PORT &= ~(1<<LED1_BIT))
	#define  Led2_off()     (LED_PORT &= ~(1<<LED2_BIT))
        

	#define  Led1_toggle()  (LED_PIN |= (1<<LED1_BIT))
	#define  Led2_toggle()  (LED_PIN |= (1<<LED2_BIT))
        

	#define  Is_led1_on()   (LED_PIN & (1<<LED1_BIT) ? TRUE : FALSE)
	#define  Is_led2_on()   (LED_PIN & (1<<LED2_BIT) ? TRUE : FALSE)



	//jtag i/o pins
	#define JTAG_OUT PORTB
	#define JTAG_IN  PINB
	#define JTAG_DIR DDRB

	//output pins
	#define JTAG_PIN_TDI  2
	#define JTAG_PIN_TMS  7
	#define JTAG_PIN_TRST 0
	#define JTAG_PIN_SRST 4
	#define JTAG_PIN_TCK  1
	//input pins
	#define JTAG_PIN_TDO  3
	#define JTAG_PIN_EMU  5
	#define JTAG_PIN_RTCK 6

	//JTAG usb commands
	//TODO: ADD commands to deal with RTCK ?
	//TODO: maybe add commands to query some firmware info...
	#define JTAG_CMD_TAP_OUTPUT     0x0
	#define JTAG_CMD_SET_TRST       0x1
	#define JTAG_CMD_SET_SRST       0x2
	#define JTAG_CMD_READ_INPUT     0x3
	#define JTAG_CMD_TAP_OUTPUT_EMU 0x4
	#define JTAG_CMD_SET_DELAY      0x5
	#define JTAG_CMD_SET_SRST_TRST  0x6

	//JTAG usb command mask
	#define JTAG_CMD_MASK       0x0f
	#define JTAG_DATA_MASK      0xf0

	//JTAG pins masks
	#define JTAG_OUTPUT_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS)|(1<<JTAG_PIN_TRST)|(1<<JTAG_PIN_SRST)|(1<<JTAG_PIN_TCK))
	#define JTAG_INPUT_MASK  ((1<<JTAG_PIN_TDO)|(1<<JTAG_PIN_EMU)|(1<<JTAG_PIN_RTCK))
	#define JTAG_SIGNAL_MASK ((1<<JTAG_PIN_TDI)|(1<<JTAG_PIN_TMS))

	#define JTAG_CLK_LO  ~(1<<JTAG_PIN_TCK)
	#define JTAG_CLK_HI   (1<<JTAG_PIN_TCK)

	//additional delay to make clk hi and lo approximately the same length, not sure if this is really needed
	#define JTAG_DELAY2 20

#endif //__JTAG_DEFS_H__
