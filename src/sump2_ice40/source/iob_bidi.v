/* ****************************************************************************
-- Source file: iob_bidi               
-- Description: Inferrable bidi
-- Language:    Verilog-2001 
-- Synthesis:   Lattice LSE 
-- ***************************************************************************/
module iob_bidi
(
   inout  wire IO,
   input  wire T,
   input  wire I,
   output wire O 
);

  assign O  = IO;
  assign IO = ( T == 0 ) ? I : 1'bz;

endmodule // iob_bidi
