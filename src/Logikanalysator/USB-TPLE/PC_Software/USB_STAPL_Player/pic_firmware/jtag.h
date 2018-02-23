/*
 (c) 2006,2007 Wojciech M. Zabolotny <wzab@ise.pw.edu.pl>

   This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
void jtag_block_xmit(uchar * datain, short len);
void jtag_block_xmit_with_read(uchar * datain,
			  uchar * dataout, short len);
uchar jtag_single(uchar datain);
uchar jtag_set_pins(uchar datain);
void jtag_set_chain(uchar chain);
void jtag_config_chain(uchar * datain);

void tck_delay();
void init_jtag();

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

/* Allocation of pins into JTAG chains */
#define TDO0 PORTDbits.RD7
#define TCK0 PORTDbits.RD5
#define TMS0 PORTDbits.RD3
#define TDI0 PORTDbits.RD1
//#define TRST0 PORTDbits.RD3

#define TDO1 PORTDbits.RD6
#define TCK1 PORTDbits.RD4
#define TMS1 PORTDbits.RD2
#define TDI1 PORTDbits.RD0
//#define TRST1 PORTEbits.RE1


#define TDO2 PORTDbits.RD6
#define TCK2 PORTDbits.RD4
#define TMS2 PORTDbits.RD2
#define TDI2 PORTDbits.RD0


#define TDO3 PORTDbits.RD6
#define TCK3 PORTDbits.RD4
#define TMS3 PORTDbits.RD2
#define TDI3 PORTDbits.RD0


#define TDO4 PORTDbits.RD6
#define TCK4 PORTDbits.RD4
#define TMS4 PORTDbits.RD2
#define TDI4 PORTDbits.RD0


#define TDO5 PORTDbits.RD6
#define TCK5 PORTDbits.RD4
#define TMS5 PORTDbits.RD2
#define TDI5 PORTDbits.RD0

#define TDO6 PORTDbits.RD6
#define TCK6 PORTDbits.RD4
#define TMS6 PORTDbits.RD2
#define TDI6 PORTDbits.RD0

#define TDO7 PORTDbits.RD6
#define TCK7 PORTDbits.RD4
#define TMS7 PORTDbits.RD2
#define TDI7 PORTDbits.RD0
