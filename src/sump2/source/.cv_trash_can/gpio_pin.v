/* ****************************************************************************
-- (C) Copyright 2016 Kevin M. Hubbard - All rights reserved.
-- Source file: gpio_pin.v                
-- Date:        December 17, 2016 
-- Author:      khubbard @ Black Mesa Labs
-- Description: Pin Control for GPIO design.
-- Language:    Verilog-2001 and VHDL-1993
-- Simulation:  Mentor-Modelsim 
-- Synthesis:   Xilinst-XST,Lattice
-- License:     This project is licensed with the CERN Open Hardware Licence
--              v1.2.  You may redistribute and modify this project under the
--              terms of the CERN OHL v.1.2. (http://ohwr.org/cernohl).
--              This project is distributed WITHOUT ANY EXPRESS OR IMPLIED
--              WARRANTY, INCLUDING OF MERCHANTABILITY, SATISFACTORY QUALITY
--              AND FITNESS FOR A PARTICULAR PURPOSE. Please see the CERN OHL
--              v.1.2 for applicable Conditions.
--
-- Revision History:
-- Ver#  When      Who      What
-- ----  --------  -------- ---------------------------------------------------
-- 0.1   12.17.16  khubbard Creation
-- ***************************************************************************/
`default_nettype none // Strictly enforce all nets to be declared

module gpio_pin 
(
  input  wire         reset,
  input  wire         clk,   
  inout  wire         gpio_pin,
  input  wire         pin_ctrl,
  output wire         pin_status,
  input  wire [3:0]   pin_cfg,
  input  wire [7:0]   pwm_pin,
  input  wire         dbg_pin
);

  reg           pin_oe_l;
  wire          pin_in;
  reg           pin_out;
  reg [3:0]     pin_cfg_p1;
  reg           pin_ctrl_p1;


// ----------------------------------------------------------------------------
// The Bidi pin primitive
// ----------------------------------------------------------------------------
  iob_bidi u1_out ( .IO( gpio_pin      ), .T( pin_oe_l       ),
                    .I ( pin_out       ), .O( pin_in         ));

  assign pin_status = pin_in;


// ----------------------------------------------------------------------------
//  0x0     : Input
//  0x1     : Output totem_pole ( Drive 0 or 1    )
//  0x2     : Output open-drain ( Drive 0 or Hi-Z )
//  0x3     : Output wired-or   ( Drive 1 or Hi-Z )
//  0x4-0x7 : Reserved
//  0x8-0xF : Output PWM0 - PWM7
// ----------------------------------------------------------------------------
always @ ( posedge reset or posedge clk ) begin : proc_pin_ctrl
 if ( reset == 1 ) begin
   pin_cfg_p1  <= 4'd0;
   pin_ctrl_p1 <= 0;
   pin_oe_l    <= 1;
   pin_out     <= 0;
 end else begin
   pin_cfg_p1  <= pin_cfg[3:0];
   pin_ctrl_p1 <= pin_ctrl;
   case( pin_cfg_p1[3:0] )
     4'd0  : pin_oe_l <= 1;
     4'd1  : pin_oe_l <= 0;
     4'd2  : pin_oe_l <=  pin_ctrl_p1;
     4'd3  : pin_oe_l <= ~pin_ctrl_p1;
     4'd4  : pin_oe_l <= 1;
     4'd5  : pin_oe_l <= 1;
     4'd6  : pin_oe_l <= 1;
     4'd7  : pin_oe_l <= 0;
     4'd8  : pin_oe_l <= 0;
     4'd9  : pin_oe_l <= 0;
     4'd10 : pin_oe_l <= 0;
     4'd11 : pin_oe_l <= 0;
     4'd12 : pin_oe_l <= 0;
     4'd13 : pin_oe_l <= 0;
     4'd14 : pin_oe_l <= 0;
     4'd15 : pin_oe_l <= 0;
   endcase

   case( pin_cfg_p1[3:0] )
     4'd0  : pin_out <= 0;
     4'd1  : pin_out <=  pin_ctrl_p1;
     4'd2  : pin_out <= 0;
     4'd3  : pin_out <= 1;
     4'd4  : pin_out <= 1;
     4'd5  : pin_out <= 1;
     4'd6  : pin_out <= 1;
     4'd7  : pin_out <= dbg_pin;
     4'd8  : pin_out <= pwm_pin[0];
     4'd9  : pin_out <= pwm_pin[1];
     4'd10 : pin_out <= pwm_pin[2];
     4'd11 : pin_out <= pwm_pin[3];
     4'd12 : pin_out <= pwm_pin[4];
     4'd13 : pin_out <= pwm_pin[5];
     4'd14 : pin_out <= pwm_pin[6];
     4'd15 : pin_out <= pwm_pin[7];
   endcase
 end 
end // proc_pin_ctrl


endmodule // gpio_pin
