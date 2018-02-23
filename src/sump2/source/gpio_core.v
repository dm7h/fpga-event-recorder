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

module gpio_core
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
  inout  wire [23:0]  gpio_pin,
  output reg  [31:0]  gpio_pin_q
);

  reg           lb_rd_p1;
  reg           lb_wr_p1;
  reg  [31:0]   lb_wr_d_p1;
  reg  [31:0]   lb_addr_p1;

  reg  [31:0]   lb_0020_reg;
  reg  [31:0]   lb_0024_reg;
  reg  [31:0]   lb_0028_reg;
  reg  [31:0]   lb_002c_reg;
  reg  [31:0]   lb_0030_reg;
  reg  [31:0]   lb_0034_reg;
  reg  [31:0]   lb_0038_reg;
  reg  [31:0]   lb_003c_reg;

  reg  [31:0]   lb_0080_reg;
  reg  [31:0]   lb_0084_reg;
  reg  [31:0]   lb_0088_reg;
  reg  [31:0]   lb_008c_reg;
  reg  [31:0]   lb_0090_reg;
  reg  [31:0]   lb_0094_reg;
  reg  [31:0]   lb_0098_reg;
  reg  [31:0]   lb_009c_reg;

  wire [31:0]   pin_status;
  wire [31:0]   pin_ctrl;
  wire [127:0]  pin_cfg;
  wire [255:0]  pwm_lb_reg;
  wire [7:0]    pwm_pin;

//assign pin_cfg[ 31:0 ] = 32'h77777777;// Force Debug SPI
  assign pin_cfg[ 31:0 ] = lb_0020_reg[31:0];// pmod_p1_cfg;
  assign pin_cfg[ 63:32] = lb_0024_reg[31:0];// pmod_p2_cfg;
  assign pin_cfg[ 95:64] = lb_0028_reg[31:0];// pmod_p3_cfg;
  assign pin_cfg[127:96] = lb_002c_reg[31:0];// pmod_p4_cfg;

  assign pin_ctrl[ 7:0 ] = lb_0030_reg[31:0];// pmod_p1_ctrl;
  assign pin_ctrl[15:8 ] = lb_0034_reg[31:0];// pmod_p2_ctrl;
  assign pin_ctrl[23:16] = lb_0038_reg[31:0];// pmod_p3_ctrl;
  assign pin_ctrl[31:24] = lb_003c_reg[31:0];// pmod_p4_ctrl;

  assign pwm_lb_reg[0*32+31:0*32+0] = lb_0080_reg[31:0];// pwm0_cfg;
  assign pwm_lb_reg[1*32+31:1*32+0] = lb_0084_reg[31:0];// pwm1_cfg;
  assign pwm_lb_reg[2*32+31:2*32+0] = lb_0088_reg[31:0];// pwm2_cfg;
  assign pwm_lb_reg[3*32+31:3*32+0] = lb_008c_reg[31:0];// pwm3_cfg;
  assign pwm_lb_reg[4*32+31:4*32+0] = lb_0090_reg[31:0];// pwm4_cfg;
  assign pwm_lb_reg[5*32+31:5*32+0] = lb_0094_reg[31:0];// pwm5_cfg;
  assign pwm_lb_reg[6*32+31:6*32+0] = lb_0098_reg[31:0];// pwm6_cfg;
  assign pwm_lb_reg[7*32+31:7*32+0] = lb_009c_reg[31:0];// pwm7_cfg;


// ----------------------------------------------------------------------------
//  Pipe the GPIO input pin status before handing off to SUMP2
// ----------------------------------------------------------------------------
always @ ( posedge clk_cap ) begin : proc_gpio_status
  gpio_pin_q <= pin_status[31:0];
end // proc_gpio_status


// ----------------------------------------------------------------------------
//  Pipe Local Bus input for timing
//  WARNING: Only bottom 8 Address bits used, 24 MSBs are optimized out here.
// ----------------------------------------------------------------------------
always @ ( posedge clk_lb ) begin : proc_lb_pipe
  lb_rd_p1   <= lb_rd;
  lb_wr_p1   <= lb_wr;
  lb_addr_p1 <= { 24'h00, lb_addr[7:0] };// Note Top 8 bits optimized out
  lb_wr_d_p1 <= lb_wr_d[31:0];
end // proc_lb_pipe


// ----------------------------------------------------------------------------
//  Local Bus Write Registers
// ----------------------------------------------------------------------------
always @ ( posedge reset or posedge clk_lb ) begin : proc_lb_wr
 if ( reset == 1 ) begin
   lb_0020_reg <= 32'd0;
   lb_0024_reg <= 32'd0;
   lb_0028_reg <= 32'd0;
   lb_002c_reg <= 32'd0;
   lb_0030_reg <= 32'd0;
   lb_0034_reg <= 32'd0;
   lb_0038_reg <= 32'd0;
   lb_003c_reg <= 32'd0;
   lb_0080_reg <= 32'd0;
   lb_0084_reg <= 32'd0;
   lb_0088_reg <= 32'd0;
   lb_008c_reg <= 32'd0;
   lb_0090_reg <= 32'd0;
   lb_0094_reg <= 32'd0;
   lb_0098_reg <= 32'd0;
   lb_009c_reg <= 32'd0;
 end else begin
  if ( lb_wr_p1 == 1 ) begin
   case( lb_addr_p1[15:0] )
     16'h0020 : lb_0020_reg <= lb_wr_d_p1[31:0];
     16'h0024 : lb_0024_reg <= lb_wr_d_p1[31:0];
     16'h0028 : lb_0028_reg <= lb_wr_d_p1[31:0];
     16'h002c : lb_002c_reg <= lb_wr_d_p1[31:0];
     16'h0030 : lb_0030_reg <= lb_wr_d_p1[31:0];
     16'h0034 : lb_0034_reg <= lb_wr_d_p1[31:0];
     16'h0038 : lb_0038_reg <= lb_wr_d_p1[31:0];
     16'h003c : lb_003c_reg <= lb_wr_d_p1[31:0];
     16'h0080 : lb_0080_reg <= lb_wr_d_p1[31:0];
     16'h0084 : lb_0084_reg <= lb_wr_d_p1[31:0];
     16'h0088 : lb_0088_reg <= lb_wr_d_p1[31:0];
     16'h008c : lb_008c_reg <= lb_wr_d_p1[31:0];
     16'h0090 : lb_0090_reg <= lb_wr_d_p1[31:0];
     16'h0094 : lb_0094_reg <= lb_wr_d_p1[31:0];
     16'h0098 : lb_0098_reg <= lb_wr_d_p1[31:0];
     16'h009c : lb_009c_reg <= lb_wr_d_p1[31:0];
   endcase // case( lb_addr_p1[15:0] )
  end // if ( lb_wr_p1 == 1 ) begin
  lb_0030_reg[16:8] <= pin_status[7:0];   // pmod_p1_ctrl
  lb_0034_reg[16:8] <= pin_status[15:8];  // pmod_p2_ctrl
  lb_0038_reg[16:8] <= pin_status[23:16]; // pmod_p3_ctrl
  lb_003c_reg[16:8] <= pin_status[31:24]; // pmod_p4_ctrl

 end // if ( reset == 1 ) begin 
end // proc_pin_ctrl


// ----------------------------------------------------------------------------
//  Local Bus Readback 
// ----------------------------------------------------------------------------
always @ ( posedge reset or posedge clk_lb ) begin : proc_lb_rd
 if ( reset == 1 ) begin
   lb_rd_d   <= 32'd0;
   lb_rd_rdy <= 0;
 end else begin
  lb_rd_d   <= 32'd0;
  lb_rd_rdy <= 0;
  if ( lb_rd_p1==1 && lb_addr_p1 >= 16'h0020 && lb_addr_p1 <= 16'h009c ) begin
    lb_rd_rdy <= 1;
  end
  if ( lb_rd_p1 == 1 ) begin
   case( lb_addr_p1[15:0] )
     16'h0020 : lb_rd_d <= lb_0020_reg[31:0];
     16'h0024 : lb_rd_d <= lb_0024_reg[31:0];
     16'h0028 : lb_rd_d <= lb_0028_reg[31:0];
     16'h002c : lb_rd_d <= lb_002c_reg[31:0];
     16'h0030 : lb_rd_d <= lb_0030_reg[31:0];
     16'h0034 : lb_rd_d <= lb_0034_reg[31:0];
     16'h0038 : lb_rd_d <= lb_0038_reg[31:0];
     16'h003c : lb_rd_d <= lb_003c_reg[31:0];
     16'h0080 : lb_rd_d <= lb_0080_reg[31:0];
     16'h0084 : lb_rd_d <= lb_0084_reg[31:0];
     16'h0088 : lb_rd_d <= lb_0088_reg[31:0];
     16'h008c : lb_rd_d <= lb_008c_reg[31:0];
     16'h0090 : lb_rd_d <= lb_0090_reg[31:0];
     16'h0094 : lb_rd_d <= lb_0094_reg[31:0];
     16'h0098 : lb_rd_d <= lb_0098_reg[31:0];
     16'h009c : lb_rd_d <= lb_009c_reg[31:0];
     default  : lb_rd_d <= 32'hDEADBEEF;
   endcase // case( lb_addr_p1[15:0] )
  end // if ( lb_rd_p1 == 1 ) begin
 end
end


//-----------------------------------------------------------------------------
// Pin Control
// WARNING: Cross Clock domain between ck_lb and ck_cap isn't handled yet.
// This iteration uses same physical clock for both, so okay for now.
//-----------------------------------------------------------------------------
genvar i1;
generate
for ( i1=0; i1<=23; i1=i1+1 ) begin: gen_i1
 gpio_pin u_gpio_pin
 (
  .reset          ( reset                  ),
  .clk            ( clk_cap                ),
  .gpio_pin       ( gpio_pin[i1]           ),
  .pin_ctrl       ( pin_ctrl[i1]           ),
  .pin_status     ( pin_status[i1]         ),
  .pin_cfg        ( pin_cfg[i1*4+3:i1*4+0] ),
  .dbg_pin        ( dbg_pin[i1]            ),
  .pwm_pin        ( pwm_pin[7:0]           )
 );
 end
endgenerate


//-----------------------------------------------------------------------------
// PWM Controllers
//   D(31:20) : PWM '1' Time in 0-4095 prescaler units
//   D(19:8)  : PWM '0' Time in 0-4095 prescaler units
//   D(7:4)   : Counter Prescaler
//   D(3:0)   : Control Mode
// WARNING: Cross Clock domain between ck_lb and ck_cap isn't handled yet.
// This iteration uses same physical clock for both, so okay for now.
//-----------------------------------------------------------------------------
genvar i2;
generate
//for ( i2=0; i2<=7; i2=i2+1 ) begin: gen_i2
//for ( i2=0; i2<=3; i2=i2+1 ) begin: gen_i2
for ( i2=0; i2<=0; i2=i2+1 ) begin: gen_i2
 gpio_pwm u_gpio_pwm
 (
  .reset          ( reset                             ),
  .clk            ( clk_cap                           ),
  .pwm_hout       ( pwm_lb_reg[ i2*32+31 : i2*32+20 ] ),
  .pwm_lout       ( pwm_lb_reg[ i2*32+19 : i2*32+8  ] ),
  .pwm_prescale   ( pwm_lb_reg[ i2*32+6  : i2*32+4  ] ),
  .pwm_ctrl       ( pwm_lb_reg[ i2*32+1  : i2*32+0  ] ),
  .pwm_pin        ( pwm_pin[ i2 ]                     )
 );
 end
endgenerate
  

endmodule // gpio_core
