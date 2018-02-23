/*
JTAG Header
Original Project: USB Stapl Player ()
Modified by Andreas Mueller for USB-TPLE Project
Date: 2010-03-24
*/

//#define jtag_addr (0xcaffffe0)
#define  GET_INFO (0x01)
#define  CONFIG_CHAIN (0x02)
#define  SINGLE_DATA_WITH_READ (0xac)
#define  BLOCK_DATA (0xb0)
#define  BLOCK_DATA_WITH_READ (0xc0)
#define  SET_PINS (0xd0)
#define  SELECT_CHAIN (0xe0)


/* Encoding of bits */
#define TDI_bit (0) /* TDI & TMS must(!) be on the two lowest bits */
#define TMS_bit (1) /* it is used when passing args to jtag_single! */
#define TCK_bit (2)
#define TRST_bit (3) /* TDI, TMS, TCK & TRST must be on bits 0...3 ! */
#define TDO_bit (4)

#define TDI_mask (1<<TDI_bit)
#define TMS_mask (1<<TMS_bit)
#define TCK_mask (1<<TCK_bit)
//#define TRST_mask (1<<TRST_bit)
#define TDO_mask (1<<TDO_bit)


//Original IDs
//#define CJ_USB_ID_VENDOR (0xa5a5)
//#define CJ_USB_ID_PRODUCT (0x313)

//Atmel IDs
//#define CJ_USB_ID_VENDOR (0x03eb)
//#define CJ_USB_ID_PRODUCT (0x2018)

//HSA IDs
#define CJ_USB_ID_VENDOR (0x1781)
#define CJ_USB_ID_PRODUCT (0x0C66)


#define  EP_TX 0x85
#define  EP_RX 0x04