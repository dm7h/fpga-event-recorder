/* ****************************************************************************
-- (C) Copyright 2016 Kevin M. Hubbard - All rights reserved.
-- Source file: gpio_pwm.v                
-- Date:        December 17, 2016 
-- Author:      khubbard @ Black Mesa Labs
-- Description: Pulse Width Modulator for GPIO design.
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

module gpio_pwm 
(
  input  wire         reset,
  input  wire         clk,   
  input  wire [11:0]  pwm_hout,
  input  wire [11:0]  pwm_lout,
  input  wire [1:0]   pwm_ctrl,
  input  wire [2:0]   pwm_prescale,
  output reg          pwm_pin
);

  reg  [19:0]   prescale_stop;
  reg  [19:0]   prescale_stop_p1;
  reg  [19:0]   prescale_cnt;
  reg  [19:0]   prescale_cnt_p1;
  reg           prescale_tick;
  reg           prescale_tick_p1;
  reg  [11:0]   pwm_cnt;
  reg  [11:0]   pwm_hout_p1;
  reg  [11:0]   pwm_lout_p1;
  reg           pwm_togl;
  reg           pwm_loc;
  reg  [1:0]    pwm_ctrl_p1;
  reg  [2:0]    pwm_prescale_p1;


// ----------------------------------------------------------------------------
//  Prescaler
// ----------------------------------------------------------------------------
always @ ( pwm_prescale_p1[2:0] ) begin : proc_prescale_mux
 case( pwm_prescale_p1[2:0] )
   3'd0  : prescale_stop <= 20'd0;
   3'd1  : prescale_stop <= 20'd1;         //  10 ns
   3'd2  : prescale_stop <= 20'd10-1;      // 100 ns
   3'd3  : prescale_stop <= 20'd100-1;     //   1 us
   3'd4  : prescale_stop <= 20'd1000-1;    //  10 us
   3'd5  : prescale_stop <= 20'd10000-1;   // 100 us
   3'd6  : prescale_stop <= 20'd100000-1;  //   1 ms
   3'd7  : prescale_stop <= 20'd1000000-1; //  10 ms
 endcase
end


// ----------------------------------------------------------------------------
//  0x0     : Input
//  0x1     : Output totem_pole ( Drive 0 or 1    )
//  0x2     : Output open-drain ( Drive 0 or Hi-Z )
//  0x3     : Output wired-or   ( Drive 1 or Hi-Z )
//  0x4-0x7 : Reserved
//  0x8-0xF : Output PWM0 - PWM7
// ----------------------------------------------------------------------------
always @ ( posedge reset or posedge clk ) begin : proc_pwm_cnt
 if ( reset == 1 ) begin
   prescale_cnt     <= 20'd0;
   prescale_tick    <= 0;
   prescale_tick_p1 <= 0;
   prescale_stop_p1 <= 20'd0;
   prescale_cnt_p1  <= 20'd0;
 end else begin
   prescale_stop_p1 <= prescale_stop[19:0];
   prescale_cnt_p1  <= prescale_cnt[19:0];
   prescale_tick    <= 0;
   prescale_tick_p1 <= prescale_tick;
   if ( prescale_stop_p1 != 20'd0 ) begin
//   if ( prescale_cnt == prescale_stop_p1[19:0] ) begin
     if ( prescale_cnt_p1 == prescale_stop_p1[19:0] ) begin
       prescale_cnt  <= 20'd1;
       prescale_tick <= 1;
     end else begin
       prescale_cnt  <= prescale_cnt + 1;
     end 
   end else begin
     prescale_cnt <= 20'd0;
   end 
   if ( pwm_prescale_p1[2:0] == 3'd1 ) begin
     prescale_tick <= 1;// No Prescale
     prescale_cnt  <= 20'd0;
   end
   prescale_cnt[19:17] <= 3'd0;// Reduce to 1ms resolution for timing closure
 end 
end // proc_pin_ctrl


// ----------------------------------------------------------------------------
// Actual PWM
// ----------------------------------------------------------------------------
always @ ( posedge reset or posedge clk ) begin : proc_pwm_fsm
 if ( reset == 1 ) begin
   pwm_cnt         <= 12'd0;
   pwm_hout_p1     <= 12'd0;
   pwm_lout_p1     <= 12'd0;
   pwm_togl        <= 0;
   pwm_loc         <= 0;
   pwm_pin         <= 0;
   pwm_ctrl_p1     <= 2'd0;
   pwm_prescale_p1 <= 3'd0;
 end else begin
   pwm_pin         <= pwm_loc;
   pwm_ctrl_p1     <= pwm_ctrl[1:0];
   pwm_prescale_p1 <= pwm_prescale[2:0];
   pwm_hout_p1     <= pwm_hout[11:0];
   pwm_lout_p1     <= pwm_lout[11:0];

   if ( pwm_ctrl_p1==2'd1 || pwm_ctrl_p1==2'd2 || pwm_ctrl_p1==2'd3 ) begin
     if ( prescale_tick == 1 ) begin
       if ( pwm_togl == 0 ) begin
         if ( pwm_cnt == pwm_hout_p1 ) begin
           pwm_loc  <= 0;
           pwm_cnt  <= 12'd1;
           pwm_togl <= ~ pwm_togl;
         end else begin
           pwm_loc  <= 1;
           pwm_cnt  <= pwm_cnt + 1;
         end
       end else begin
         if ( pwm_cnt == pwm_lout_p1 ) begin
           if ( pwm_ctrl_p1 == 2'd2 || pwm_ctrl_p1 == 2'd1 ) begin
             pwm_cnt  <= pwm_cnt;// Halt as one-shot mode
           end else begin
             pwm_loc  <= 1; // Continuous, so loop
             pwm_cnt  <= 12'd1;
             pwm_togl <= ~ pwm_togl;
           end
         end else begin
           pwm_loc  <= 0;
           pwm_cnt  <= pwm_cnt + 1;
         end
       end
     end
   end else begin
     pwm_cnt  <= 12'd1;
     pwm_togl <= 0;
   end
 end 
end // proc_pin_ctrl


endmodule // gpio_pwm
