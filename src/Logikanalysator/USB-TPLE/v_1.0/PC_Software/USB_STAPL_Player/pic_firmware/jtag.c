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
#include "common_types.h"
#include "jtag.h"
#include <pic18fregs.h>
#include <delay.h>



/* The variable dummy_var_for_bug_protection has been introduced to
  workaround the bug in the SDCC: 
  http://sourceforge.net/tracker/index.php?func=detail&aid=1818857&group_id=599&atid=100599
  When the bug gets fixed, you can get rid of this variable */
uchar dummy_var_for_bug_protection=0;
uchar current_chain=0;
/* The variable pin_invert informs which lines are inverted.
   You can get the proper value by or-ing the necessary masks from:
   TMS_mask, TDI_mask, TCK_mask and TDO_mask.
   In my design all the lines are inverted, so I simply set all the values to 0xff.
*/
uchar code pin_invert[8]={
  0xff, // chain 0
  0xff, // chain 1
  0xff, // chain 2
  0xff, // chain 3
  0xff, // chain 4
  0xff, // chain 5
  0xff, // chain 6
  0xff, // chain 7
};
uint delay_num[8];
uchar delay_exp[8];

void jtag_set_chain(uchar chain)
{
  current_chain = chain & 0x7;
}


void jtag_config_chain(uchar * datain)
{
  delay_num[current_chain]=((uint) *datain++) | (((uint) *datain++) << 8);
}

void init_jtag()
{
  TRISD=0xc0;
  PORTD=0x0;
}

/* The procedure below is coded with the speed in mind. That's why it is coded
   in "copy&paste" way */
void jtag_block_xmit(uchar *datain, short len)
{
  uchar *indata;
  uchar dta=0;
  uchar invert=0;
  short i=0;
  uint jtag_del,d;
  jtag_del=delay_num[current_chain];
  invert=pin_invert[current_chain];
  indata=datain;
  for(i=0;i<len;i++) {
    //We extract the two bits of data
    if((i & 3)==0) {
      dta = *indata;
      indata++;
    }
    //Now we transfer the data
    if ((dta ^ invert)& TDI_mask) {
      // Set TDI to 1
      switch(current_chain) {
      case 0: TDI0 = 1; break;
      case 1: TDI1 = 1;	break;
      case 2: TDI2 = 1;	break;
      case 3: TDI3 = 1; break;
      case 4: TDI4 = 1;	break;
      case 5: TDI5 = 1; break;
      case 6: TDI6 = 1;	break;
      default: TDI7 = 1; break;
      }
    } else {
      // Set TDI to 0
      switch(current_chain) {
      case 0: TDI0 = 0;	break;
      case 1: TDI1 = 0; break;
      case 2: TDI2 = 0; break;
      case 3: TDI3 = 0;	break;
      case 4: TDI4 = 0;	break;
      case 5: TDI5 = 0;	break;
      case 6: TDI6 = 0;	break;
      default: TDI7 = 0; break;
      }
    }
    if ((dta^invert) & TMS_mask) {
      // Set TMS to 1
      switch(current_chain) {
      case 0: TMS0 = 1; break;
      case 1: TMS1 = 1;	break;
      case 2: TMS2 = 1;	break;
      case 3: TMS3 = 1;	break;
      case 4: TMS4 = 1;	break;
      case 5: TMS5 = 1;	break;
      case 6: TMS6 = 1;	break;
      default: TMS7 = 1; break;
      }
    } else {
      // Set TMS to 0
      switch(current_chain) {
      case 0: TMS0 = 0; break;
      case 1: TMS1 = 0;	break;
      case 2: TMS2 = 0;	break;
      case 3: TMS3 = 0;	break;
      case 4: TMS4 = 0;	break;
      case 5: TMS5 = 0;	break;
      case 6: TMS6 = 0;	break;
      default: TMS7 = 0; break;
      }
    }
    //delay
    for(d=jtag_del;d>0;d--) {};
    //Set TCK to 1
    if (invert & TCK_mask) {
      // Set pin to 0 - it will be inverted
      switch(current_chain) {
      case 0: TCK0 = 0; break;
      case 1: TCK1 = 0;	break;
      case 2: TCK2 = 0;	break;
      case 3: TCK3 = 0;	break;
      case 4: TCK4 = 0;	break;
      case 5: TCK5 = 0;	break;
      case 6: TCK6 = 0;	break;
      default: TCK7 = 0; break;
      }
    } else {
      // Set pin to 1
      switch(current_chain) {
      case 0: TCK0 = 1; break;
      case 1: TCK1 = 1;	break;
      case 2: TCK2 = 1;	break;
      case 3: TCK3 = 1;	break;
      case 4: TCK4 = 1;	break;
      case 5: TCK5 = 1;	break;
      case 6: TCK6 = 1;	break;
      default: TCK7 = 1; break;
      }
    }
    //delay
    for(d=jtag_del;d>0;d--) {};
    if (invert & TCK_mask) {
      // Set pin to 1 - it will be inverted
      switch(current_chain) {
      case 0: TCK0 = 1; break;
      case 1: TCK1 = 1;	break;
      case 2: TCK2 = 1;	break;
      case 3: TCK3 = 1;	break;
      case 4: TCK4 = 1;	break;
      case 5: TCK5 = 1;	break;
      case 6: TCK6 = 1;	break;
      default: TCK7 = 1; break;
      }
    } else {
      // Set pin to 0
      switch(current_chain) {
      case 0: TCK0 = 0; break;
      case 1: TCK1 = 0;	break;
      case 2: TCK2 = 0;	break;
      case 3: TCK3 = 0;	break;
      case 4: TCK4 = 0;	break;
      case 5: TCK5 = 0;	break;
      case 6: TCK6 = 0;	break;
      default: TCK7 = 0; break;
      }
    }
    dta >>= 2;
  }
}

uchar jtag_single(uchar datain)
{
  uchar res;
  uchar invert;
  uint jtag_del;
  uint d;
  //Here we need to check the current JTAG chain as well
  //Maybe we need to have another function for setting (resetting) of the pins?
  //The two least significant bits encode TMS and TDI
  //We return the state of the TDO
  //First we set or clear TMS and TDI
  jtag_del=delay_num[current_chain];
  invert=pin_invert[current_chain];
  for(d=jtag_del;d>0;d--) {};
  // Read the TDO
  switch(current_chain) {
  case 0: res = (invert & TDO_mask) ? !TDO0 : TDO0; break;
  case 1: res = (invert & TDO_mask) ? !TDO1 : TDO1; break;
  case 2: res = (invert & TDO_mask) ? !TDO2 : TDO2; break;
  case 3: res = (invert & TDO_mask) ? !TDO3 : TDO3; break;
  case 4: res = (invert & TDO_mask) ? !TDO4 : TDO4; break;
  case 5: res = (invert & TDO_mask) ? !TDO5 : TDO5; break;
  case 6: res = (invert & TDO_mask) ? !TDO6 : TDO6; break;
  case 7: res = (invert & TDO_mask) ? !TDO6 : TDO7; break;
  }
  // Set the TDI
    if ((datain ^ invert)& TDI_mask) {
      // Set TDI to 1
      switch(current_chain) {
      case 0: TDI0 = 1; break;
      case 1: TDI1 = 1;	break;
      case 2: TDI2 = 1;	break;
      case 3: TDI3 = 1; break;
      case 4: TDI4 = 1;	break;
      case 5: TDI5 = 1; break;
      case 6: TDI6 = 1;	break;
      default: TDI7 = 1; break;
      }
    } else {
      // Set TDI to 0
      switch(current_chain) {
      case 0: TDI0 = 0;	break;
      case 1: TDI1 = 0; break;
      case 2: TDI2 = 0; break;
      case 3: TDI3 = 0;	break;
      case 4: TDI4 = 0;	break;
      case 5: TDI5 = 0;	break;
      case 6: TDI6 = 0;	break;
      default: TDI7 = 0; break;
      }
    }
    if ((datain^invert) & TMS_mask) {
      // Set TMS to 1
      switch(current_chain) {
      case 0: TMS0 = 1; break;
      case 1: TMS1 = 1;	break;
      case 2: TMS2 = 1;	break;
      case 3: TMS3 = 1;	break;
      case 4: TMS4 = 1;	break;
      case 5: TMS5 = 1;	break;
      case 6: TMS6 = 1;	break;
      default: TMS7 = 1; break;
      }
    } else {
      // Set TMS to 0
      switch(current_chain) {
      case 0: TMS0 = 0; break;
      case 1: TMS1 = 0;	break;
      case 2: TMS2 = 0;	break;
      case 3: TMS3 = 0;	break;
      case 4: TMS4 = 0;	break;
      case 5: TMS5 = 0;	break;
      case 6: TMS6 = 0;	break;
      default: TMS7 = 0; break;
      }
    }
    //Set TCK to 1
    if (invert & TCK_mask) {
      // Set pin to 0 - it will be inverted
      switch(current_chain) {
      case 0: TCK0 = 0; break;
      case 1: TCK1 = 0;	break;
      case 2: TCK2 = 0;	break;
      case 3: TCK3 = 0;	break;
      case 4: TCK4 = 0;	break;
      case 5: TCK5 = 0;	break;
      case 6: TCK6 = 0;	break;
      default: TCK7 = 0; break;
      }
    } else {
      // Set pin to 1
      switch(current_chain) {
      case 0: TCK0 = 1; break;
      case 1: TCK1 = 1;	break;
      case 2: TCK2 = 1;	break;
      case 3: TCK3 = 1;	break;
      case 4: TCK4 = 1;	break;
      case 5: TCK5 = 1;	break;
      case 6: TCK6 = 1;	break;
      default: TCK7 = 1; break;
      }
    }
    //delay
    for(d=jtag_del;d>0;d--) {};
    if (invert & TCK_mask) {
      // Set pin to 1 - it will be inverted
      switch(current_chain) {
      case 0: TCK0 = 1; break;
      case 1: TCK1 = 1;	break;
      case 2: TCK2 = 1;	break;
      case 3: TCK3 = 1;	break;
      case 4: TCK4 = 1;	break;
      case 5: TCK5 = 1;	break;
      case 6: TCK6 = 1;	break;
      default: TCK7 = 1; break;
      }
    } else {
      // Set pin to 0
      switch(current_chain) {
      case 0: TCK0 = 0; break;
      case 1: TCK1 = 0;	break;
      case 2: TCK2 = 0;	break;
      case 3: TCK3 = 0;	break;
      case 4: TCK4 = 0;	break;
      case 5: TCK5 = 0;	break;
      case 6: TCK6 = 0;	break;
      default: TCK7 = 0; break;
      }
    }
  return res;
}

/* This procedure is not used in the STAPL Player!!!
   it is not optimized, and therefore it is relatively slow */
void jtag_block_xmit_with_read(uchar * datain,
			       uchar * dataout, short len)
{
  uchar dta=0;
  uchar dout=0;
  uchar mask=0;
  short i=0;
  mask = 1;
  for(i=0;i<len;i++) {
    if((i & 3) == 0)  dta = *(datain++);
    dout |= jtag_single(dta) ? mask : 0;
    mask <<= 1;
    if((i & 7) == 7)  {
      mask = 0;
      *(dataout++) = dout;
    }
  }
  //We check if any unsaved bits remain
  if(len & 7)  *(dataout++) = dout;
}

/* The set_xxx procedures have bin written for testing
   they are very slow, and should not be used for any repeating
   activities */
void set_tdi(uchar d)
{
  if (pin_invert[current_chain] & TDI_mask) d = !d;
  switch (current_chain) {
  case 0:
    if(d) TDI0 = 1;
    else TDI0 = 0;
    break;
  case 1:
    if(d) TDI1 = 1;
    else  TDI1 = 0;
    break;
  }
}

void set_tms(uchar d)
{
  if (pin_invert[current_chain] & TMS_mask) d = !d;
  switch (current_chain) {
  case 0:
    if(d) TMS0=1;
    else TMS0=0;
    break;
  case 1:
    if(d) TMS1=1;
    else TMS1=0;
    break;
  }
}
void set_tck(uchar d)
{
  if (pin_invert[current_chain] & TCK_mask) d = !d;
  switch (current_chain) {
  case 0:
    if(d) TCK0=1;
    else TCK0=0;
    break;
  case 1:
    if(d) TCK1=1;
    else TCK1=0;
    break;
  }
}

uchar get_tdo(void)
{
  uchar d=0;
  switch(current_chain) {
  case 0:
    d = TDO0;
    break;
  case 1:
    d = TDO1;
    break;
  }
  if (pin_invert[current_chain] & TDO_mask) d = !d;
  return d ? 1 : 0;
}

uchar jtag_set_pins(uchar d)
{
  uchar res;
  res=get_tdo();
  set_tdi(d & TDI_mask);
  set_tms(d & TMS_mask);
  set_tck(d & TCK_mask);
  //set_trst(d & TRST_mask);
  return res;
}
