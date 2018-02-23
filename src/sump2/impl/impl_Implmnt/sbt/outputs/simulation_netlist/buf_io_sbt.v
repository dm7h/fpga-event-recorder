// ******************************************************************************

// iCEcube Netlister

// Version:            2015.04.27409

// Build Date:         May 27 2015 16:13:33

// File Generated:     Dec 17 2016 11:35:33

// Purpose:            Post-Route Verilog/VHDL netlist for timing simulation

// Copyright (C) 2006-2010 by Lattice Semiconductor Corp. All rights reserved.

// ******************************************************************************

// Verilog file for cell "buf_io" view "INTERFACE"

module buf_io (
    T,
    IO,
    CLK,
    O,
    I);

    input T;
    inout IO;
    input CLK;
    output O;
    input I;

    wire N__163;
    wire N__162;
    wire N__161;
    wire N__152;
    wire N__151;
    wire N__150;
    wire N__143;
    wire N__142;
    wire N__141;
    wire N__134;
    wire N__133;
    wire N__132;
    wire N__125;
    wire N__124;
    wire N__123;
    wire N__106;
    wire N__103;
    wire N__100;
    wire N__97;
    wire N__94;
    wire N__91;
    wire N__88;
    wire N__85;
    wire N__82;
    wire N__79;
    wire O_c;
    wire I_c;
    wire VCCG0;
    wire GNDG0;
    wire T_c;
    wire T_c_i;
    wire _gnd_net_;

    PRE_IO_GBUF CLK_ibuf_gb_io_preiogbuf (
            .PADSIGNALTOGLOBALBUFFER(N__161),
            .GLOBALBUFFEROUTPUT());
    IO_PAD CLK_ibuf_gb_io_iopad (
            .OE(N__163),
            .DIN(N__162),
            .DOUT(N__161),
            .PACKAGEPIN(CLK));
    defparam CLK_ibuf_gb_io_preio.NEG_TRIGGER=1'b0;
    defparam CLK_ibuf_gb_io_preio.PIN_TYPE=6'b000001;
    PRE_IO CLK_ibuf_gb_io_preio (
            .PADOEN(N__163),
            .PADOUT(N__162),
            .PADIN(N__161),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(),
            .DOUT0(),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD T_ibuf_iopad (
            .OE(N__152),
            .DIN(N__151),
            .DOUT(N__150),
            .PACKAGEPIN(T));
    defparam T_ibuf_preio.NEG_TRIGGER=1'b0;
    defparam T_ibuf_preio.PIN_TYPE=6'b000001;
    PRE_IO T_ibuf_preio (
            .PADOEN(N__152),
            .PADOUT(N__151),
            .PADIN(N__150),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(T_c),
            .DOUT0(),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD u_SB_IO_iopad (
            .OE(N__143),
            .DIN(N__142),
            .DOUT(N__141),
            .PACKAGEPIN(IO));
    defparam u_SB_IO_preio.PIN_TYPE=6'b101001;
    PRE_IO u_SB_IO_preio (
            .PADOEN(N__143),
            .PADOUT(N__142),
            .PADIN(N__141),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(N__82),
            .DIN0(O_c),
            .DOUT0(N__97),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD I_ibuf_iopad (
            .OE(N__134),
            .DIN(N__133),
            .DOUT(N__132),
            .PACKAGEPIN(I));
    defparam I_ibuf_preio.NEG_TRIGGER=1'b0;
    defparam I_ibuf_preio.PIN_TYPE=6'b000001;
    PRE_IO I_ibuf_preio (
            .PADOEN(N__134),
            .PADOUT(N__133),
            .PADIN(N__132),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(I_c),
            .DOUT0(),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD O_obuf_iopad (
            .OE(N__125),
            .DIN(N__124),
            .DOUT(N__123),
            .PACKAGEPIN(O));
    defparam O_obuf_preio.NEG_TRIGGER=1'b0;
    defparam O_obuf_preio.PIN_TYPE=6'b011001;
    PRE_IO O_obuf_preio (
            .PADOEN(N__125),
            .PADOUT(N__124),
            .PADIN(N__123),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(),
            .DOUT0(N__106),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IoInMux I__18 (
            .O(N__106),
            .I(N__103));
    LocalMux I__17 (
            .O(N__103),
            .I(N__100));
    Odrv4 I__16 (
            .O(N__100),
            .I(O_c));
    IoInMux I__15 (
            .O(N__97),
            .I(N__94));
    LocalMux I__14 (
            .O(N__94),
            .I(N__91));
    Odrv4 I__13 (
            .O(N__91),
            .I(I_c));
    InMux I__12 (
            .O(N__88),
            .I(N__85));
    LocalMux I__11 (
            .O(N__85),
            .I(T_c));
    IoInMux I__10 (
            .O(N__82),
            .I(N__79));
    LocalMux I__9 (
            .O(N__79),
            .I(T_c_i));
    VCC VCC (
            .Y(VCCG0));
    GND GND (
            .Y(GNDG0));
    GND GND_Inst (
            .Y(_gnd_net_));
    defparam u_SB_IO_RNO_LC_1_4_0.C_ON=1'b0;
    defparam u_SB_IO_RNO_LC_1_4_0.SEQ_MODE=4'b0000;
    defparam u_SB_IO_RNO_LC_1_4_0.LUT_INIT=16'b0101010101010101;
    LogicCell40 u_SB_IO_RNO_LC_1_4_0 (
            .in0(N__88),
            .in1(_gnd_net_),
            .in2(_gnd_net_),
            .in3(_gnd_net_),
            .lcout(T_c_i),
            .ltout(),
            .carryin(_gnd_net_),
            .carryout(),
            .clk(_gnd_net_),
            .ce(),
            .sr(_gnd_net_));
endmodule // buf_io
