// ******************************************************************************

// iCEcube Netlister

// Version:            2015.04.27409

// Build Date:         May 27 2015 16:13:33

// File Generated:     Nov 20 2016 12:56:16

// Purpose:            Post-Route Verilog/VHDL netlist for timing simulation

// Copyright (C) 2006-2010 by Lattice Semiconductor Corp. All rights reserved.

// ******************************************************************************

// Verilog file for cell "top_pll" view "INTERFACE"

module top_pll (
    RESET,
    REFERENCECLK,
    PLLOUTGLOBAL,
    PLLOUTCORE,
    LOCK);

    input RESET;
    input REFERENCECLK;
    output PLLOUTGLOBAL;
    output PLLOUTCORE;
    output LOCK;

    wire N__262;
    wire N__261;
    wire N__260;
    wire N__253;
    wire N__252;
    wire N__251;
    wire N__244;
    wire N__243;
    wire N__242;
    wire N__235;
    wire N__234;
    wire N__233;
    wire N__226;
    wire N__225;
    wire N__224;
    wire N__207;
    wire N__204;
    wire N__201;
    wire N__198;
    wire N__195;
    wire N__192;
    wire N__189;
    wire N__186;
    wire N__183;
    wire N__180;
    wire N__177;
    wire N__174;
    wire N__171;
    wire N__168;
    wire N__165;
    wire N__162;
    wire N__159;
    wire N__156;
    wire N__153;
    wire N__150;
    wire N__147;
    wire N__144;
    wire N__141;
    wire N__138;
    wire N__135;
    wire N__132;
    wire N__129;
    wire N__126;
    wire N__123;
    wire N__120;
    wire N__117;
    wire N__114;
    wire N__111;
    wire RESET_c;
    wire REFERENCECLK_c;
    wire PLLOUTCORE_c;
    wire VCCG0;
    wire GNDG0;
    wire PLL_BUFFER_top_pll_inst_LOCK_THRU_CO;
    wire LOCK_c;
    wire PLLOUTGLOBAL_c;
    wire GB_BUFFER_PLLOUTGLOBAL_c_THRU_CO;
    wire _gnd_net_;

    defparam top_pll_inst.DELAY_ADJUSTMENT_MODE_FEEDBACK="FIXED";
    defparam top_pll_inst.TEST_MODE=1'b0;
    defparam top_pll_inst.SHIFTREG_DIV_MODE=2'b00;
    defparam top_pll_inst.PLLOUT_SELECT="GENCLK";
    defparam top_pll_inst.FILTER_RANGE=3'b010;
    defparam top_pll_inst.FEEDBACK_PATH="SIMPLE";
    defparam top_pll_inst.FDA_RELATIVE=4'b0000;
    defparam top_pll_inst.FDA_FEEDBACK=4'b0000;
    defparam top_pll_inst.ENABLE_ICEGATE=1'b0;
    defparam top_pll_inst.DIVR=4'b0000;
    defparam top_pll_inst.DIVQ=3'b011;
    defparam top_pll_inst.DIVF=7'b0011111;
    defparam top_pll_inst.DELAY_ADJUSTMENT_MODE_RELATIVE="FIXED";
    SB_PLL40_CORE top_pll_inst (
            .EXTFEEDBACK(),
            .LATCHINPUTVALUE(),
            .SCLK(),
            .SDO(),
            .LOCK(PLL_BUFFER_top_pll_inst_LOCK_THRU_CO),
            .PLLOUTCORE(PLLOUTCORE_c),
            .REFERENCECLK(N__183),
            .RESETB(N__207),
            .BYPASS(GNDG0),
            .SDI(),
            .DYNAMICDELAY({GNDG0,GNDG0,GNDG0,GNDG0,GNDG0,GNDG0,GNDG0,GNDG0}),
            .PLLOUTGLOBAL(PLLOUTGLOBAL_c));
    IO_PAD PLLOUTGLOBAL_obuf_iopad (
            .OE(N__262),
            .DIN(N__261),
            .DOUT(N__260),
            .PACKAGEPIN(PLLOUTGLOBAL));
    defparam PLLOUTGLOBAL_obuf_preio.NEG_TRIGGER=1'b0;
    defparam PLLOUTGLOBAL_obuf_preio.PIN_TYPE=6'b011001;
    PRE_IO PLLOUTGLOBAL_obuf_preio (
            .PADOEN(N__262),
            .PADOUT(N__261),
            .PADIN(N__260),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(),
            .DOUT0(N__114),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD LOCK_obuf_iopad (
            .OE(N__253),
            .DIN(N__252),
            .DOUT(N__251),
            .PACKAGEPIN(LOCK));
    defparam LOCK_obuf_preio.NEG_TRIGGER=1'b0;
    defparam LOCK_obuf_preio.PIN_TYPE=6'b011001;
    PRE_IO LOCK_obuf_preio (
            .PADOEN(N__253),
            .PADOUT(N__252),
            .PADIN(N__251),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(),
            .DOUT0(N__138),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD PLLOUTCORE_obuf_iopad (
            .OE(N__244),
            .DIN(N__243),
            .DOUT(N__242),
            .PACKAGEPIN(PLLOUTCORE));
    defparam PLLOUTCORE_obuf_preio.NEG_TRIGGER=1'b0;
    defparam PLLOUTCORE_obuf_preio.PIN_TYPE=6'b011001;
    PRE_IO PLLOUTCORE_obuf_preio (
            .PADOEN(N__244),
            .PADOUT(N__243),
            .PADIN(N__242),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(),
            .DOUT0(N__165),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD REFERENCECLK_ibuf_iopad (
            .OE(N__235),
            .DIN(N__234),
            .DOUT(N__233),
            .PACKAGEPIN(REFERENCECLK));
    defparam REFERENCECLK_ibuf_preio.NEG_TRIGGER=1'b0;
    defparam REFERENCECLK_ibuf_preio.PIN_TYPE=6'b000001;
    PRE_IO REFERENCECLK_ibuf_preio (
            .PADOEN(N__235),
            .PADOUT(N__234),
            .PADIN(N__233),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(REFERENCECLK_c),
            .DOUT0(),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IO_PAD RESET_ibuf_iopad (
            .OE(N__226),
            .DIN(N__225),
            .DOUT(N__224),
            .PACKAGEPIN(RESET));
    defparam RESET_ibuf_preio.NEG_TRIGGER=1'b0;
    defparam RESET_ibuf_preio.PIN_TYPE=6'b000001;
    PRE_IO RESET_ibuf_preio (
            .PADOEN(N__226),
            .PADOUT(N__225),
            .PADIN(N__224),
            .CLOCKENABLE(),
            .DOUT1(),
            .OUTPUTENABLE(),
            .DIN0(RESET_c),
            .DOUT0(),
            .INPUTCLK(),
            .LATCHINPUTVALUE(),
            .DIN1(),
            .OUTPUTCLK());
    IoInMux I__43 (
            .O(N__207),
            .I(N__204));
    LocalMux I__42 (
            .O(N__204),
            .I(N__201));
    IoSpan4Mux I__41 (
            .O(N__201),
            .I(N__198));
    Span4Mux_s1_v I__40 (
            .O(N__198),
            .I(N__195));
    Span4Mux_v I__39 (
            .O(N__195),
            .I(N__192));
    Sp12to4 I__38 (
            .O(N__192),
            .I(N__189));
    Span12Mux_h I__37 (
            .O(N__189),
            .I(N__186));
    Odrv12 I__36 (
            .O(N__186),
            .I(RESET_c));
    IoInMux I__35 (
            .O(N__183),
            .I(N__180));
    LocalMux I__34 (
            .O(N__180),
            .I(N__177));
    Span4Mux_s1_v I__33 (
            .O(N__177),
            .I(N__174));
    Span4Mux_v I__32 (
            .O(N__174),
            .I(N__171));
    Sp12to4 I__31 (
            .O(N__171),
            .I(N__168));
    Odrv12 I__30 (
            .O(N__168),
            .I(REFERENCECLK_c));
    IoInMux I__29 (
            .O(N__165),
            .I(N__162));
    LocalMux I__28 (
            .O(N__162),
            .I(N__159));
    Span4Mux_s3_h I__27 (
            .O(N__159),
            .I(N__156));
    Span4Mux_h I__26 (
            .O(N__156),
            .I(N__153));
    Span4Mux_h I__25 (
            .O(N__153),
            .I(N__150));
    Span4Mux_h I__24 (
            .O(N__150),
            .I(N__147));
    Odrv4 I__23 (
            .O(N__147),
            .I(PLLOUTCORE_c));
    InMux I__22 (
            .O(N__144),
            .I(N__141));
    LocalMux I__21 (
            .O(N__141),
            .I(PLL_BUFFER_top_pll_inst_LOCK_THRU_CO));
    IoInMux I__20 (
            .O(N__138),
            .I(N__135));
    LocalMux I__19 (
            .O(N__135),
            .I(N__132));
    Span4Mux_s0_h I__18 (
            .O(N__132),
            .I(N__129));
    Odrv4 I__17 (
            .O(N__129),
            .I(LOCK_c));
    InMux I__16 (
            .O(N__126),
            .I(N__123));
    LocalMux I__15 (
            .O(N__123),
            .I(N__120));
    Glb2LocalMux I__14 (
            .O(N__120),
            .I(N__117));
    GlobalMux I__13 (
            .O(N__117),
            .I(PLLOUTGLOBAL_c));
    IoInMux I__12 (
            .O(N__114),
            .I(N__111));
    LocalMux I__11 (
            .O(N__111),
            .I(GB_BUFFER_PLLOUTGLOBAL_c_THRU_CO));
    VCC VCC (
            .Y(VCCG0));
    GND GND (
            .Y(GNDG0));
    GND GND_Inst (
            .Y(_gnd_net_));
    defparam PLL_BUFFER_top_pll_inst_LOCK_THRU_LUT4_0_LC_1_1_0.C_ON=1'b0;
    defparam PLL_BUFFER_top_pll_inst_LOCK_THRU_LUT4_0_LC_1_1_0.SEQ_MODE=4'b0000;
    defparam PLL_BUFFER_top_pll_inst_LOCK_THRU_LUT4_0_LC_1_1_0.LUT_INIT=16'b1111111100000000;
    LogicCell40 PLL_BUFFER_top_pll_inst_LOCK_THRU_LUT4_0_LC_1_1_0 (
            .in0(_gnd_net_),
            .in1(_gnd_net_),
            .in2(_gnd_net_),
            .in3(N__144),
            .lcout(LOCK_c),
            .ltout(),
            .carryin(_gnd_net_),
            .carryout(),
            .clk(_gnd_net_),
            .ce(),
            .sr(_gnd_net_));
    defparam GB_BUFFER_PLLOUTGLOBAL_c_THRU_LUT4_0_LC_1_5_0.C_ON=1'b0;
    defparam GB_BUFFER_PLLOUTGLOBAL_c_THRU_LUT4_0_LC_1_5_0.SEQ_MODE=4'b0000;
    defparam GB_BUFFER_PLLOUTGLOBAL_c_THRU_LUT4_0_LC_1_5_0.LUT_INIT=16'b1010101010101010;
    LogicCell40 GB_BUFFER_PLLOUTGLOBAL_c_THRU_LUT4_0_LC_1_5_0 (
            .in0(N__126),
            .in1(_gnd_net_),
            .in2(_gnd_net_),
            .in3(_gnd_net_),
            .lcout(GB_BUFFER_PLLOUTGLOBAL_c_THRU_CO),
            .ltout(),
            .carryin(_gnd_net_),
            .carryout(),
            .clk(_gnd_net_),
            .ce(),
            .sr(_gnd_net_));
endmodule // top_pll
