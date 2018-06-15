/* ****************************************************************************
-- Source file: buf_io_sdr.                
-- Description: Wrapper around Lattice ICE40 IOB Primitive
-- Language:    Verilog-2001 
-- Synthesis:   Lattice LSE 
-- ***************************************************************************/
module buf_io_sdr
(
   input  wire CLK,
   inout  wire IO,
   input  wire I,
   input  wire T,
   output wire O 
);


//-----------------------------------------------------------------------------
// Pin_Type
//  [1:0] Input
//     01 = Simple
//     00 = Registered SDR or DDR
//  [5:2] Output
//   0000 = Disabled
//   0110 = Simple
//   1010 = Tristatable
//   1101 = SDR Registered Output and Tristate
//   1100 = DDR Registered Output and Tristate
//
// Note that explicitly connecting a logic '1' value to port CLOCK_ENABLE will
// result in a non-optimal implementation, since an extra LUT will be used to
// generate the Logic '1'. If the user's intention is to keep the Input and
// Output registers always enabled, it is recommended that port CLOCK_ENABLE be
// left unconnected.
//-----------------------------------------------------------------------------
SB_IO
#
(
 .PIN_TYPE           ( 6'b110100 ) 
)
 u_SB_IO
(
 .PACKAGE_PIN        ( IO       ), // Pin signal name
 .LATCH_INPUT_VALUE  ( 1'b0     ), // Latches/holds the Input value
 .CLOCK_ENABLE       (          ), // Clock Enable common to input and output
 .INPUT_CLK          ( CLK      ), // Clock for the input registers
 .OUTPUT_CLK         ( CLK      ), // Clock for the output registers
 .OUTPUT_ENABLE      ( T        ), // Output Pin Tristate/Enable control
 .D_OUT_0            ( I        ), // Data 0 – out to Pin/Rising clk edge
 .D_OUT_1            (          ), // Data 1 - out to Pin/Falling clk edge
 .D_IN_0             ( O        ), // Data 0 - Pin input/Rising clk edge
 .D_IN_1             (          )  // Data 1 – Pin input/Falling clk edge
);


endmodule
