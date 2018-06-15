-- ******************************************************************************

-- iCEcube Netlister

-- Version:            2015.04.27409

-- Build Date:         May 27 2015 16:13:32

-- File Generated:     Nov 20 2016 12:56:16

-- Purpose:            Post-Route Verilog/VHDL netlist for timing simulation

-- Copyright (C) 2006-2010 by Lattice Semiconductor Corp. All rights reserved.

-- ******************************************************************************

-- VHDL file for cell "top_pll" view "INTERFACE"

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library ice;
use ice.vcomponent_vital.all;

-- Entity of top_pll
entity top_pll is
port (
    RESET : in std_logic;
    REFERENCECLK : in std_logic;
    PLLOUTGLOBAL : out std_logic;
    PLLOUTCORE : out std_logic;
    LOCK : out std_logic);
end top_pll;

-- Architecture of top_pll
-- View name is \INTERFACE\
architecture \INTERFACE\ of top_pll is

signal \N__262\ : std_logic;
signal \N__261\ : std_logic;
signal \N__260\ : std_logic;
signal \N__253\ : std_logic;
signal \N__252\ : std_logic;
signal \N__251\ : std_logic;
signal \N__244\ : std_logic;
signal \N__243\ : std_logic;
signal \N__242\ : std_logic;
signal \N__235\ : std_logic;
signal \N__234\ : std_logic;
signal \N__233\ : std_logic;
signal \N__226\ : std_logic;
signal \N__225\ : std_logic;
signal \N__224\ : std_logic;
signal \N__207\ : std_logic;
signal \N__204\ : std_logic;
signal \N__201\ : std_logic;
signal \N__198\ : std_logic;
signal \N__195\ : std_logic;
signal \N__192\ : std_logic;
signal \N__189\ : std_logic;
signal \N__186\ : std_logic;
signal \N__183\ : std_logic;
signal \N__180\ : std_logic;
signal \N__177\ : std_logic;
signal \N__174\ : std_logic;
signal \N__171\ : std_logic;
signal \N__168\ : std_logic;
signal \N__165\ : std_logic;
signal \N__162\ : std_logic;
signal \N__159\ : std_logic;
signal \N__156\ : std_logic;
signal \N__153\ : std_logic;
signal \N__150\ : std_logic;
signal \N__147\ : std_logic;
signal \N__144\ : std_logic;
signal \N__141\ : std_logic;
signal \N__138\ : std_logic;
signal \N__135\ : std_logic;
signal \N__132\ : std_logic;
signal \N__129\ : std_logic;
signal \N__126\ : std_logic;
signal \N__123\ : std_logic;
signal \N__120\ : std_logic;
signal \N__117\ : std_logic;
signal \N__114\ : std_logic;
signal \N__111\ : std_logic;
signal \RESET_c\ : std_logic;
signal \REFERENCECLK_c\ : std_logic;
signal \PLLOUTCORE_c\ : std_logic;
signal \VCCG0\ : std_logic;
signal \GNDG0\ : std_logic;
signal \PLL_BUFFER_top_pll_inst_LOCK_THRU_CO\ : std_logic;
signal \LOCK_c\ : std_logic;
signal \PLLOUTGLOBAL_c\ : std_logic;
signal \GB_BUFFER_PLLOUTGLOBAL_c_THRU_CO\ : std_logic;
signal \_gnd_net_\ : std_logic;

signal \PLLOUTGLOBAL_wire\ : std_logic;
signal \LOCK_wire\ : std_logic;
signal \PLLOUTCORE_wire\ : std_logic;
signal \REFERENCECLK_wire\ : std_logic;
signal \RESET_wire\ : std_logic;
signal \top_pll_inst_DYNAMICDELAY_wire\ : std_logic_vector(7 downto 0);

begin
    PLLOUTGLOBAL <= \PLLOUTGLOBAL_wire\;
    LOCK <= \LOCK_wire\;
    PLLOUTCORE <= \PLLOUTCORE_wire\;
    \REFERENCECLK_wire\ <= REFERENCECLK;
    \RESET_wire\ <= RESET;
    \top_pll_inst_DYNAMICDELAY_wire\ <= \GNDG0\&\GNDG0\&\GNDG0\&\GNDG0\&\GNDG0\&\GNDG0\&\GNDG0\&\GNDG0\;

    \top_pll_inst\ : SB_PLL40_CORE
    generic map (
            DELAY_ADJUSTMENT_MODE_FEEDBACK => "FIXED",
            TEST_MODE => '0',
            SHIFTREG_DIV_MODE => "00",
            PLLOUT_SELECT => "GENCLK",
            FILTER_RANGE => "010",
            FEEDBACK_PATH => "SIMPLE",
            FDA_RELATIVE => "0000",
            FDA_FEEDBACK => "0000",
            ENABLE_ICEGATE => '0',
            DIVR => "0000",
            DIVQ => "011",
            DIVF => "0011111",
            DELAY_ADJUSTMENT_MODE_RELATIVE => "FIXED"
        )
    port map (
            EXTFEEDBACK => '0',
            LATCHINPUTVALUE => '0',
            SCLK => '0',
            SDO => OPEN,
            LOCK => \PLL_BUFFER_top_pll_inst_LOCK_THRU_CO\,
            PLLOUTCORE => \PLLOUTCORE_c\,
            REFERENCECLK => \N__183\,
            RESETB => \N__207\,
            BYPASS => \GNDG0\,
            SDI => '0',
            DYNAMICDELAY => \top_pll_inst_DYNAMICDELAY_wire\,
            PLLOUTGLOBAL => \PLLOUTGLOBAL_c\
        );

    \PLLOUTGLOBAL_obuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__262\,
            DIN => \N__261\,
            DOUT => \N__260\,
            PACKAGEPIN => \PLLOUTGLOBAL_wire\
        );

    \PLLOUTGLOBAL_obuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "011001"
        )
    port map (
            PADOEN => \N__262\,
            PADOUT => \N__261\,
            PADIN => \N__260\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => OPEN,
            DOUT0 => \N__114\,
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \LOCK_obuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__253\,
            DIN => \N__252\,
            DOUT => \N__251\,
            PACKAGEPIN => \LOCK_wire\
        );

    \LOCK_obuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "011001"
        )
    port map (
            PADOEN => \N__253\,
            PADOUT => \N__252\,
            PADIN => \N__251\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => OPEN,
            DOUT0 => \N__138\,
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \PLLOUTCORE_obuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__244\,
            DIN => \N__243\,
            DOUT => \N__242\,
            PACKAGEPIN => \PLLOUTCORE_wire\
        );

    \PLLOUTCORE_obuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "011001"
        )
    port map (
            PADOEN => \N__244\,
            PADOUT => \N__243\,
            PADIN => \N__242\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => OPEN,
            DOUT0 => \N__165\,
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \REFERENCECLK_ibuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__235\,
            DIN => \N__234\,
            DOUT => \N__233\,
            PACKAGEPIN => \REFERENCECLK_wire\
        );

    \REFERENCECLK_ibuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "000001"
        )
    port map (
            PADOEN => \N__235\,
            PADOUT => \N__234\,
            PADIN => \N__233\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => \REFERENCECLK_c\,
            DOUT0 => '0',
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \RESET_ibuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__226\,
            DIN => \N__225\,
            DOUT => \N__224\,
            PACKAGEPIN => \RESET_wire\
        );

    \RESET_ibuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "000001"
        )
    port map (
            PADOEN => \N__226\,
            PADOUT => \N__225\,
            PADIN => \N__224\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => \RESET_c\,
            DOUT0 => '0',
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \I__43\ : IoInMux
    port map (
            O => \N__207\,
            I => \N__204\
        );

    \I__42\ : LocalMux
    port map (
            O => \N__204\,
            I => \N__201\
        );

    \I__41\ : IoSpan4Mux
    port map (
            O => \N__201\,
            I => \N__198\
        );

    \I__40\ : Span4Mux_s1_v
    port map (
            O => \N__198\,
            I => \N__195\
        );

    \I__39\ : Span4Mux_v
    port map (
            O => \N__195\,
            I => \N__192\
        );

    \I__38\ : Sp12to4
    port map (
            O => \N__192\,
            I => \N__189\
        );

    \I__37\ : Span12Mux_h
    port map (
            O => \N__189\,
            I => \N__186\
        );

    \I__36\ : Odrv12
    port map (
            O => \N__186\,
            I => \RESET_c\
        );

    \I__35\ : IoInMux
    port map (
            O => \N__183\,
            I => \N__180\
        );

    \I__34\ : LocalMux
    port map (
            O => \N__180\,
            I => \N__177\
        );

    \I__33\ : Span4Mux_s1_v
    port map (
            O => \N__177\,
            I => \N__174\
        );

    \I__32\ : Span4Mux_v
    port map (
            O => \N__174\,
            I => \N__171\
        );

    \I__31\ : Sp12to4
    port map (
            O => \N__171\,
            I => \N__168\
        );

    \I__30\ : Odrv12
    port map (
            O => \N__168\,
            I => \REFERENCECLK_c\
        );

    \I__29\ : IoInMux
    port map (
            O => \N__165\,
            I => \N__162\
        );

    \I__28\ : LocalMux
    port map (
            O => \N__162\,
            I => \N__159\
        );

    \I__27\ : Span4Mux_s3_h
    port map (
            O => \N__159\,
            I => \N__156\
        );

    \I__26\ : Span4Mux_h
    port map (
            O => \N__156\,
            I => \N__153\
        );

    \I__25\ : Span4Mux_h
    port map (
            O => \N__153\,
            I => \N__150\
        );

    \I__24\ : Span4Mux_h
    port map (
            O => \N__150\,
            I => \N__147\
        );

    \I__23\ : Odrv4
    port map (
            O => \N__147\,
            I => \PLLOUTCORE_c\
        );

    \I__22\ : InMux
    port map (
            O => \N__144\,
            I => \N__141\
        );

    \I__21\ : LocalMux
    port map (
            O => \N__141\,
            I => \PLL_BUFFER_top_pll_inst_LOCK_THRU_CO\
        );

    \I__20\ : IoInMux
    port map (
            O => \N__138\,
            I => \N__135\
        );

    \I__19\ : LocalMux
    port map (
            O => \N__135\,
            I => \N__132\
        );

    \I__18\ : Span4Mux_s0_h
    port map (
            O => \N__132\,
            I => \N__129\
        );

    \I__17\ : Odrv4
    port map (
            O => \N__129\,
            I => \LOCK_c\
        );

    \I__16\ : InMux
    port map (
            O => \N__126\,
            I => \N__123\
        );

    \I__15\ : LocalMux
    port map (
            O => \N__123\,
            I => \N__120\
        );

    \I__14\ : Glb2LocalMux
    port map (
            O => \N__120\,
            I => \N__117\
        );

    \I__13\ : GlobalMux
    port map (
            O => \N__117\,
            I => \PLLOUTGLOBAL_c\
        );

    \I__12\ : IoInMux
    port map (
            O => \N__114\,
            I => \N__111\
        );

    \I__11\ : LocalMux
    port map (
            O => \N__111\,
            I => \GB_BUFFER_PLLOUTGLOBAL_c_THRU_CO\
        );

    \VCC\ : VCC
    port map (
            Y => \VCCG0\
        );

    \GND\ : GND
    port map (
            Y => \GNDG0\
        );

    \GND_Inst\ : GND
    port map (
            Y => \_gnd_net_\
        );

    \PLL_BUFFER_top_pll_inst_LOCK_THRU_LUT4_0_LC_1_1_0\ : LogicCell40
    generic map (
            C_ON => '0',
            SEQ_MODE => "0000",
            LUT_INIT => "1111111100000000"
        )
    port map (
            in0 => \_gnd_net_\,
            in1 => \_gnd_net_\,
            in2 => \_gnd_net_\,
            in3 => \N__144\,
            lcout => \LOCK_c\,
            ltout => OPEN,
            carryin => \_gnd_net_\,
            carryout => OPEN,
            clk => \_gnd_net_\,
            ce => 'H',
            sr => \_gnd_net_\
        );

    \GB_BUFFER_PLLOUTGLOBAL_c_THRU_LUT4_0_LC_1_5_0\ : LogicCell40
    generic map (
            C_ON => '0',
            SEQ_MODE => "0000",
            LUT_INIT => "1010101010101010"
        )
    port map (
            in0 => \N__126\,
            in1 => \_gnd_net_\,
            in2 => \_gnd_net_\,
            in3 => \_gnd_net_\,
            lcout => \GB_BUFFER_PLLOUTGLOBAL_c_THRU_CO\,
            ltout => OPEN,
            carryin => \_gnd_net_\,
            carryout => OPEN,
            clk => \_gnd_net_\,
            ce => 'H',
            sr => \_gnd_net_\
        );
end \INTERFACE\;
