-- ******************************************************************************

-- iCEcube Netlister

-- Version:            2015.04.27409

-- Build Date:         May 27 2015 16:13:32

-- File Generated:     Dec 17 2016 11:35:34

-- Purpose:            Post-Route Verilog/VHDL netlist for timing simulation

-- Copyright (C) 2006-2010 by Lattice Semiconductor Corp. All rights reserved.

-- ******************************************************************************

-- VHDL file for cell "buf_io" view "INTERFACE"

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

library ice;
use ice.vcomponent_vital.all;

-- Entity of buf_io
entity buf_io is
port (
    T : in std_logic;
    IO : inout std_logic;
    CLK : in std_logic;
    O : out std_logic;
    I : in std_logic);
end buf_io;

-- Architecture of buf_io
-- View name is \INTERFACE\
architecture \INTERFACE\ of buf_io is

signal \N__163\ : std_logic;
signal \N__162\ : std_logic;
signal \N__161\ : std_logic;
signal \N__152\ : std_logic;
signal \N__151\ : std_logic;
signal \N__150\ : std_logic;
signal \N__143\ : std_logic;
signal \N__142\ : std_logic;
signal \N__141\ : std_logic;
signal \N__134\ : std_logic;
signal \N__133\ : std_logic;
signal \N__132\ : std_logic;
signal \N__125\ : std_logic;
signal \N__124\ : std_logic;
signal \N__123\ : std_logic;
signal \N__106\ : std_logic;
signal \N__103\ : std_logic;
signal \N__100\ : std_logic;
signal \N__97\ : std_logic;
signal \N__94\ : std_logic;
signal \N__91\ : std_logic;
signal \N__88\ : std_logic;
signal \N__85\ : std_logic;
signal \N__82\ : std_logic;
signal \N__79\ : std_logic;
signal \O_c\ : std_logic;
signal \I_c\ : std_logic;
signal \VCCG0\ : std_logic;
signal \GNDG0\ : std_logic;
signal \T_c\ : std_logic;
signal \T_c_i\ : std_logic;
signal \_gnd_net_\ : std_logic;

signal \CLK_wire\ : std_logic;
signal \T_wire\ : std_logic;
signal \I_wire\ : std_logic;
signal \O_wire\ : std_logic;

begin
    \CLK_wire\ <= CLK;
    \T_wire\ <= T;
    \I_wire\ <= I;
    O <= \O_wire\;

    \CLK_ibuf_gb_io_preiogbuf\ : PRE_IO_GBUF
    port map (
            PADSIGNALTOGLOBALBUFFER => \N__161\,
            GLOBALBUFFEROUTPUT => OPEN
        );

    \CLK_ibuf_gb_io_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__163\,
            DIN => \N__162\,
            DOUT => \N__161\,
            PACKAGEPIN => \CLK_wire\
        );

    \CLK_ibuf_gb_io_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "000001"
        )
    port map (
            PADOEN => \N__163\,
            PADOUT => \N__162\,
            PADIN => \N__161\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => OPEN,
            DOUT0 => '0',
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \T_ibuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__152\,
            DIN => \N__151\,
            DOUT => \N__150\,
            PACKAGEPIN => \T_wire\
        );

    \T_ibuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "000001"
        )
    port map (
            PADOEN => \N__152\,
            PADOUT => \N__151\,
            PADIN => \N__150\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => \T_c\,
            DOUT0 => '0',
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \u_SB_IO_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__143\,
            DIN => \N__142\,
            DOUT => \N__141\,
            PACKAGEPIN => IO
        );

    \u_SB_IO_preio\ : PRE_IO
    generic map (
            PIN_TYPE => "101001",
            NEG_TRIGGER => '0'
        )
    port map (
            PADOEN => \N__143\,
            PADOUT => \N__142\,
            PADIN => \N__141\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => \N__82\,
            DIN0 => \O_c\,
            DOUT0 => \N__97\,
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \I_ibuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__134\,
            DIN => \N__133\,
            DOUT => \N__132\,
            PACKAGEPIN => \I_wire\
        );

    \I_ibuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "000001"
        )
    port map (
            PADOEN => \N__134\,
            PADOUT => \N__133\,
            PADIN => \N__132\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => \I_c\,
            DOUT0 => '0',
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \O_obuf_iopad\ : IO_PAD
    generic map (
            IO_STANDARD => "SB_LVCMOS",
            PULLUP => '0'
        )
    port map (
            OE => \N__125\,
            DIN => \N__124\,
            DOUT => \N__123\,
            PACKAGEPIN => \O_wire\
        );

    \O_obuf_preio\ : PRE_IO
    generic map (
            NEG_TRIGGER => '0',
            PIN_TYPE => "011001"
        )
    port map (
            PADOEN => \N__125\,
            PADOUT => \N__124\,
            PADIN => \N__123\,
            CLOCKENABLE => 'H',
            DOUT1 => '0',
            OUTPUTENABLE => '0',
            DIN0 => OPEN,
            DOUT0 => \N__106\,
            INPUTCLK => '0',
            LATCHINPUTVALUE => '0',
            DIN1 => OPEN,
            OUTPUTCLK => '0'
        );

    \I__18\ : IoInMux
    port map (
            O => \N__106\,
            I => \N__103\
        );

    \I__17\ : LocalMux
    port map (
            O => \N__103\,
            I => \N__100\
        );

    \I__16\ : Odrv4
    port map (
            O => \N__100\,
            I => \O_c\
        );

    \I__15\ : IoInMux
    port map (
            O => \N__97\,
            I => \N__94\
        );

    \I__14\ : LocalMux
    port map (
            O => \N__94\,
            I => \N__91\
        );

    \I__13\ : Odrv4
    port map (
            O => \N__91\,
            I => \I_c\
        );

    \I__12\ : InMux
    port map (
            O => \N__88\,
            I => \N__85\
        );

    \I__11\ : LocalMux
    port map (
            O => \N__85\,
            I => \T_c\
        );

    \I__10\ : IoInMux
    port map (
            O => \N__82\,
            I => \N__79\
        );

    \I__9\ : LocalMux
    port map (
            O => \N__79\,
            I => \T_c_i\
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

    \u_SB_IO_RNO_LC_1_4_0\ : LogicCell40
    generic map (
            C_ON => '0',
            SEQ_MODE => "0000",
            LUT_INIT => "0101010101010101"
        )
    port map (
            in0 => \N__88\,
            in1 => \_gnd_net_\,
            in2 => \_gnd_net_\,
            in3 => \_gnd_net_\,
            lcout => \T_c_i\,
            ltout => OPEN,
            carryin => \_gnd_net_\,
            carryout => OPEN,
            clk => \_gnd_net_\,
            ce => 'H',
            sr => \_gnd_net_\
        );
end \INTERFACE\;
