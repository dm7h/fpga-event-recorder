/*-------------------------------------------------------------------------
  ep1.c - USB endpoint 1 callbacks

  (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 
  (c) 2006,2007 Modified by Wojciech M. Zabolotny <wzab@ise.pw.edu.pl>

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

  Seriously modified by Wojciech M. Zabolotny <wzab@ise.pw.edu.pl>
  The USB bootloader functionality removed
  Instead the JTAG controller functionality introduced
  -------------------------------------------------------------------------*/

/* $Id: ep1.c,v 1.5 2006/04/18 19:44:32 gaufille Exp $ */

#include "ep1.h"
#include "ep2.h"
#include "usb.h"
#include "debug.h"
#include "flash.h"
#include "jtag.h"
#include <pic18fregs.h>
#include <delay.h>


#pragma udata usb_buf ep1_OutBuffer
volatile uchar ep1_OutBuffer[EP1_BUFFER_SIZE];
extern volatile uchar ep2_InBuffer[EP2_BUFFER_SIZE];

uchar code dev_id_message[]="USB JTAG GPL Interface 1.0";
//uchar data result=0xa5;

void ep1_init(void)
{
  debug("ep1_init\n");
  EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
  EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
  EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
  UEP1 = EPHSHK_EN | EPOUTEN_EN | EPCONDIS_EN;       // Init EP1 as an OUT EP
}

void ep1_out(void)
{
  debug("ep1_out\n");
  if(EP_OUT_BD(1).Cnt >= 1)
    {
      short len;
      //First decode commands encoded on four most significant bits 
      switch(ep1_OutBuffer[0] & 0xf0)
        {
	case BLOCK_DATA_WITH_READ:
	  /* This command transmits the given amount of data 
	     and simultaneously reads the TDO and puts it into the buffer.
	     This command is not used in the STAPL player, but may be used
	     by the other software */
	  /* We have received a block of data to send via the JTAG chain */
	  /* First check if the buffer length is correct */
	  if(EP_OUT_BD(1).Cnt < 3) {
	    //It is not correct buffer size!!!
	    //Do something
	    goto error;
	  };
	  len=((((short)ep1_OutBuffer[0]) & 0xf) << 8) | ep1_OutBuffer[1];
	  if(EP_OUT_BD(1).Cnt != 2+((len+3)>>2)) {
	    goto error;
	  }
	  ep2_InBuffer[0]=BLOCK_DATA_WITH_READ;
	  jtag_block_xmit_with_read(&ep1_OutBuffer[2],
				    &ep2_InBuffer[1],len);
	  ep2_num_bytes_to_send = 1+((len+7) >> 3); //ID+bytes needed to send "len" bits
	  prepare_ep2_in_no_copy();
	  break;
	case BLOCK_DATA: 
	  /* We have received a block of data to send via the JTAG chain */
	  /* First check if the buffer length is correct */
	  
	  if(EP_OUT_BD(1).Cnt < 3) {
	    //It is not correct buffer size!!!
	    //Do something
	    goto error;
	    }; 
	  len=((((short)ep1_OutBuffer[0]) & 0xf) << 8) | ep1_OutBuffer[1];
	  if(EP_OUT_BD(1).Cnt != 2+((len+3)>>2)) {
	    goto error;
	    //Wrong buffer length, again
	  }; 
	  /* If it is - call the jtag_block_xmit function */
	  jtag_block_xmit(&ep1_OutBuffer[2],len);
	  /* It would be nice to work with the ping-pong buffer
	     so we could transfer one buffer while receiving the next one...
	     Unfortunately it can not be achieved with this firmware yet
	  */
	  ep2_num_bytes_to_send = 1;
	  ep2_InBuffer[0]=BLOCK_DATA;
	  prepare_ep2_in_no_copy();
	  break;
	case SELECT_CHAIN:
	  /* Select which chain should we use */
	  jtag_set_chain(ep1_OutBuffer[0]);
	  ep2_num_bytes_to_send = 1;
	  ep2_InBuffer[0]=SELECT_CHAIN;
	  prepare_ep2_in_no_copy();
	  /*result=0xd0;
	  ep2_source_data=&result;
	  prepare_ep2_in();*/
	  break;
	case SET_PINS:
	  /* Set the pins in the selected chain 
	     there are 4 pins TRST, TCK, TMS, TDI.
	     Simultaneously we read and return the TDO value.*/
	  ep2_num_bytes_to_send = 1;
	  ep2_InBuffer[0]=SET_PINS | jtag_set_pins(ep1_OutBuffer[0]);
	  prepare_ep2_in_no_copy();
	  //result=0xc0 | jtag_set_pins(ep1_OutBuffer[0]);
	  //ep2_source_data=&result;
	  //prepare_ep2_in();
	  break;
	default:
	  switch (ep1_OutBuffer[0]) {
	  case SINGLE_DATA_WITH_READ: 
	  case SINGLE_DATA_WITH_READ+1:
	  case SINGLE_DATA_WITH_READ+2:
	  case SINGLE_DATA_WITH_READ+3:
	    /* Three lower bits encode the TMS and TDI */
	    ep2_num_bytes_to_send = 1;
	    ep2_InBuffer[0]=SINGLE_DATA_WITH_READ | jtag_single(ep1_OutBuffer[0]);
	    prepare_ep2_in_no_copy();
	    break;
	  case CONFIG_CHAIN:
	    /* We have received set of parameters needed to configure 
	       the JTAG chain. It is unclear if we really need to include
	       the number of the chain into the command identifier */
	    /* What should be configured in each chain?
	       1) Which bits are inverted, and which are not
	       2) Timing of the automatically generated clock pulses
	       3) [...] TBD
	    */
	    jtag_config_chain(&ep1_OutBuffer[1]);
	    ep2_num_bytes_to_send = 1;
	    ep2_InBuffer[0]=CONFIG_CHAIN;
	    prepare_ep2_in_no_copy();
	    break;
	  case GET_INFO:
	    ep2_num_bytes_to_send = sizeof(dev_id_message);
	    ep2_source_data = dev_id_message;
	    prepare_ep2_in();
	    break;
	  default:
	    debug("unknown command\n");
	    // Raise an error
	    EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	    EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
	    EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_BSTALL;
	    break;
	  }
        }
      EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
      if(EP_OUT_BD(1).Stat.DTS == 0)
        {
	  EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
        }
      else
        {
	  EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
        }
    }
  return;
 error:
  // Raise an error
  {
    EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
    EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
    EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_BSTALL;
  }
}
