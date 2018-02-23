/* ****************************************************************************
-- (C) Copyright 2015 Black Mesa Labs
-- Source file: mesa_pi_spi.v                
-- Date:        August 2, 2015  
-- Author:      khubbard
-- Description: Convert SPI MOSI stream from a RaspPi to Mesa binary nibbles
-- Language:    Verilog-2001 
--
-- 2 MHz Capture
-- pi_spi_cs_l    \________________________________________________________/
-- pi_spi_sck     ___________/  \__/  \__/  \__/  \______/ \_/ \_/ \________ 
-- pi_spi_mosi    ---------< 7  ><  6 >< .. ><  0 >----< 7 ><..><0 >--------
-- pi_spi_miso
--                |<-10us->|                                       |<-2us->|
--
-- http://www.raspberry-projects.com/pi/programming-in-c/spi/
--       using-the-spi-interface
--
-- Revision History:
-- Ver#  When      Who      What
-- ----  --------  -------- ---------------------------------------------------
-- 0.1   08.02.15  khubbard Creation
-- ***************************************************************************/
//`default_nettype none // Strictly enforce all nets to be declared
                                                                                
module mesa_pi_spi
(
  input  wire         pi_spi_sck,
  input  wire         pi_spi_cs_l,
  input  wire         pi_spi_mosi,
  output wire         pi_spi_miso,

  input  wire         clk,
  input  wire         lb_rd,
  input  wire [31:0]  lb_rd_d,
  input  wire         lb_rd_rdy,

  output reg  [3:0]   mosi_nib_d,
  output reg          mosi_nib_rdy
); // module mesa_pi_spi


  reg [1:0]     bit_cnt;
  reg [3:0]     nib_sr;
  reg [7:0]     miso_sr;
  reg           rdy_loc;
  reg           rdy_meta;
  reg           rdy_p1;
  reg           rdy_p2;
  reg [31:0]    rd_d_sr;
  reg           rd_d_jk;
  reg [3:0]     rd_cnt;
  reg           mosi_not_f;
  reg           cs_l_meta;
  reg           cs_l_p1;
  reg           cs_l_p2;




//-----------------------------------------------------------------------------
// Xfer clock domains
//-----------------------------------------------------------------------------
always @ ( posedge clk ) begin : proc_xfer
 begin
   rdy_meta <= rdy_loc;
   rdy_p1   <= rdy_meta;
   rdy_p2   <= rdy_p1;
   mosi_nib_rdy <= rdy_p1 & ~ rdy_p2;
 end
end


//-----------------------------------------------------------------------------
// Convert MOSI to 2 nibbles to insert in regular UART nibble stream
//  input  pi_spi_sck;
//  input  pi_spi_cs_l;
//  input  pi_spi_mosi;
//  output pi_spi_miso;
//-----------------------------------------------------------------------------
always @ ( posedge pi_spi_sck or posedge pi_spi_cs_l ) begin : proc_tx
 begin
   if ( pi_spi_cs_l == 1 ) begin
     bit_cnt    <= 2'd0;
     rdy_loc    <= 0;
     mosi_not_f <= 0;
   end else begin
     rdy_loc <= 0;
     nib_sr  <= { nib_sr[2:0], pi_spi_mosi };
     bit_cnt <= bit_cnt + 1;
     if ( bit_cnt == 2'd3 ) begin
       mosi_nib_d <= { nib_sr[2:0], pi_spi_mosi };
       rdy_loc    <= 1;
       mosi_not_f <= ~ pi_spi_mosi;
     end
   end
 end
end // proc_tx 


//-----------------------------------------------------------------------------
// Handle the Readback. When 32bit read comes in, latch and then decrement a
// count after each SPI cycle ( where each cycle reads 1 of 8 bytes ).
//-----------------------------------------------------------------------------
always @ ( posedge clk ) begin : proc_lb_sr
 begin
   cs_l_meta <= pi_spi_cs_l;
   cs_l_p1   <= cs_l_meta;
   cs_l_p2   <= cs_l_p1;

   // Decrement the byte counter after each SPI cycle
   if ( cs_l_p1 == 1 && cs_l_p2 == 0 ) begin
     if ( rd_cnt != 4'd0 ) begin
       rd_cnt <= rd_cnt[3:0] - 1;
     end
   end

   if ( lb_rd_rdy == 1 ) begin
     rd_d_jk <= 1;
     rd_d_sr <= lb_rd_d[31:0];
//   rd_cnt  <= 4'd8;
     rd_cnt  <= 4'd9;
   end

   if ( lb_rd == 1 ) begin
     rd_d_jk <= 0;
     rd_cnt  <= 4'd0;
   end

 end
end


//-----------------------------------------------------------------------------
// Reads are handled by MOSI shifting single bytes at a time, looking for 0xF0.
// When 0xF0 is received, it then reads 3+4 bytes. 
// Burst reads are NOT supported.
//
// Ro : 0xF0FE3404 :                                                            
//      0xFF = Bus Idle ( NULLs )                                               
//  B0  0xF0 = New Bus Cycle to begin ( Nibble and bit orientation )            
//  B1  0xFE = Slot Number - Default is 0xFE for Ro traffic                     
//  B2  0x3  = Sub-Slot within the chip (0-0xF)                                 
//      0x4  = Command Nibble for Sub-Slot                                      
//  B3  0x04 = Number of Payload Bytes (0-255)     
//
// Shift 0xFF until rd_rdy asserts, then shift 0xF0,0xFE,0x00,0x04
//-----------------------------------------------------------------------------
always @ ( negedge pi_spi_sck or posedge pi_spi_cs_l ) begin : proc_miso
 begin
   if ( pi_spi_cs_l == 1 ) begin
//   miso_sr <= 8'hF5;
     if          ( rd_cnt == 4'd9 ) begin
       miso_sr <= 8'hFF;
     end else if ( rd_cnt == 4'd8 ) begin
       miso_sr <= 8'hF0;
     end else if ( rd_cnt == 4'd7 ) begin
       miso_sr <= 8'hFE;
     end else if ( rd_cnt == 4'd6 ) begin
       miso_sr <= 8'h00;
     end else if ( rd_cnt == 4'd5 ) begin
       miso_sr <= 8'h04;
     end else if ( rd_cnt == 4'd4 ) begin
       miso_sr <= rd_d_sr[31:24];
     end else if ( rd_cnt == 4'd3 ) begin
       miso_sr <= rd_d_sr[23:16];
     end else if ( rd_cnt == 4'd2 ) begin
       miso_sr <= rd_d_sr[15:8];
     end else if ( rd_cnt == 4'd1 ) begin
       miso_sr <= rd_d_sr[7:0];
     end else begin
       miso_sr <= 8'hFF;
     end
   end else begin
     miso_sr <= { miso_sr[6:0], 1'b0 };
   end
 end
end // proc_tx 
  assign pi_spi_miso = miso_sr[7];


endmodule // mesa_pi_spi
