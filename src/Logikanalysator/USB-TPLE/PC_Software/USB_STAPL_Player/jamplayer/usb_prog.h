/*
 This file implements the USB specific part of the stapl player

 */

#define  GET_INFO (0x01)
#define  CONFIG_CHAIN (0x02)
#define  SINGLE_DATA_WITH_READ (0xac)
#define  BLOCK_DATA (0xb0)
#define  BLOCK_DATA_WITH_READ (0xc0)
#define  SET_PINS (0xd0)
#define  SELECT_CHAIN (0xe0)
