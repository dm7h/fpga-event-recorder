/* ****************************************************************************
-- (C) Copyright 2016 Kevin M. Hubbard - All rights reserved.
-- Source file: core.v                
-- Date:        December 17, 2016
-- Author:      khubbard @ Black Mesa Labs
-- Description: Core wrapper for the logic. Minimized for fit to HX1K fabric.
-- Language:    Verilog-2001 and VHDL-1993
-- Simulation:  Mentor-Modelsim 
-- Synthesis:   Xilinst-XST 
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

module core 
(
  input  wire         reset,
  input  wire         clk_lb,   
  input  wire         clk_cap,  
  input  wire         lb_wr,
  input  wire         lb_rd,
  input  wire [31:0]  lb_addr,
  input  wire [31:0]  lb_wr_d,
  output reg  [31:0]  lb_rd_d,
  output reg          lb_rd_rdy,
  input  wire [31:0]  dbg_pin,
  output wire [3:0]   led_bus,
  inout  wire [31:0]  gpio_pin
);// module core


  wire [31:0]   u0_lb_rd_d;
  wire          u0_lb_rd_rdy;
  wire [31:0]   u1_lb_rd_d;
  wire          u1_lb_rd_rdy;
  wire [31:0]   gpio_pin_q;
  reg  [31:0]   events_din;
  wire [31:0]   user_ctrl;
  wire          lb_cs_sump2_ctrl;
  wire          lb_cs_sump2_data;
  wire [31:0]   time_stamp_d;
  reg           lb_wr_p1;
  reg           lb_rd_p1;
  reg  [31:0]   lb_addr_p1;
  reg  [31:0]   lb_wr_d_p1;
  reg           lb_cs_sump2_ctrl_p1;
  reg           lb_cs_sump2_data_p1;


// ----------------------------------------------------------------------------
// Flop SUMP2 inputs for best possible timing
// ----------------------------------------------------------------------------
always @ ( posedge clk_cap ) begin : proc_sump_in
  events_din <= gpio_pin_q[31:0];
end // proc_sump_in


// ----------------------------------------------------------------------------
// Decode the SUMP access registers
// ----------------------------------------------------------------------------
  assign lb_cs_sump2_ctrl = ( lb_addr[7:0] == 8'H10 ) ? 1 : 0;
  assign lb_cs_sump2_data = ( lb_addr[7:0] == 8'H14 ) ? 1 : 0;


// ----------------------------------------------------------------------------
// LocalBus Readback
// ----------------------------------------------------------------------------
always @ ( posedge clk_lb ) begin : proc_lb_rd
  lb_rd_d    <= 32'd0;
  lb_rd_rdy  <= 0;
  lb_wr_p1            <= lb_wr;
  lb_rd_p1            <= lb_rd;
  lb_addr_p1          <= lb_addr[31:0];
  lb_wr_d_p1          <= lb_wr_d[31:0];
  lb_cs_sump2_ctrl_p1 <= lb_cs_sump2_ctrl;
  lb_cs_sump2_data_p1 <= lb_cs_sump2_data;

  if ( lb_rd == 1 ) begin
    lb_rd_rdy <= 1;
    if          ( lb_addr[7:0] == 8'H00 ) begin
      lb_rd_d   <= 32'H11223344;
    end else if ( lb_addr[7:0] == 8'H04 ) begin
      lb_rd_d   <= 32'H00000100;
    end else if ( lb_addr[7:0] == 8'H08 ) begin
      lb_rd_d   <= time_stamp_d[31:0];
    end else begin
      lb_rd_rdy <= 0;
    end
  end // if ( lb_rd == 1 ) begin

  if ( u0_lb_rd_rdy == 1 ) begin
    lb_rd_d   <= u0_lb_rd_d[31:0];
    lb_rd_rdy <= 1;
  end 
  if ( u1_lb_rd_rdy == 1 ) begin
    lb_rd_d   <= u1_lb_rd_d[31:0];
    lb_rd_rdy <= 1;
  end 
end // proc_lb_rd


//-----------------------------------------------------------------------------
// 32bit UNIX TimeStamp of when the design was synthesized
//-----------------------------------------------------------------------------
time_stamp u_time_stamp
(
  .time_dout                        ( time_stamp_d                   )
);


//-----------------------------------------------------------------------------
// GPIO Module 
//-----------------------------------------------------------------------------
gpio_core u_gpio_core
(
  .reset         ( reset                    ),
  .clk_lb        ( clk_lb                   ),
  .clk_cap       ( clk_cap                  ),
  .lb_wr         ( lb_wr                    ),
  .lb_rd         ( lb_rd                    ),
  .lb_addr       ( lb_addr[31:0]            ),
  .lb_wr_d       ( lb_wr_d[31:0]            ),
  .lb_rd_d       ( u0_lb_rd_d               ),
  .lb_rd_rdy     ( u0_lb_rd_rdy             ),
  .dbg_pin       ( dbg_pin[31:0]            ),
  .gpio_pin      ( gpio_pin[31:0]           ),
  .gpio_pin_q    ( gpio_pin_q[31:0]         )
); // u_gpio_core


//-----------------------------------------------------------------------------
// SUMP2 Logic Analyzer
//-----------------------------------------------------------------------------
sump2
#
(
  .depth_len      (  1024                   ),
  .depth_bits     (  10                     ),
  .event_bytes    (  4                      ),
//.event_bytes    (  2                      ),
  .data_dwords    (  0                      ),
  .nonrle_en      (  0                      ),
  .rle_en         (  1                      ),
  .pattern_en     (  0                      ),
  .trigger_nth_en (  1                      ),
  .trigger_dly_en (  1                      ),
  .trigger_wd_en  (  0                      ),
  .freq_mhz       (  16'd100                ),
  .freq_fracts    (  16'h0000               )
)
u_sump2
(
  .reset         ( reset                    ),
  .clk_lb        ( clk_lb                   ),
  .clk_cap       ( clk_cap                  ),
  .lb_cs_ctrl    ( lb_cs_sump2_ctrl_p1      ),
  .lb_cs_data    ( lb_cs_sump2_data_p1      ),
  .lb_wr         ( lb_wr_p1                 ),
  .lb_rd         ( lb_rd_p1                 ),
  .lb_wr_d       ( lb_wr_d_p1[31:0]         ),
  .lb_rd_d       ( u1_lb_rd_d               ),
  .lb_rd_rdy     ( u1_lb_rd_rdy             ),
  .active        (                          ),
  .trigger_in    ( 1'b0                     ),
  .trigger_out   (                          ),
  .events_din    ( events_din[31:0]         ),
  .dwords_3_0    ( 128'd0                   ),
  .dwords_7_4    ( 128'd0                   ),
  .dwords_11_8   ( 128'd0                   ),
  .dwords_15_12  ( 128'd0                   ),
  .led_bus       ( led_bus[3:0]             ),
  .user_ctrl     ( user_ctrl[31:0]          ),
  .user_pat0     (                          ),
  .user_pat1     (                          )
); // u_sump2


endmodule // core
