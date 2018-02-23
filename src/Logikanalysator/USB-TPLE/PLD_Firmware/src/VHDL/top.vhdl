------------------------------------------
--! @File top.vhdl
--! @Brief Topmodule for Timing Analyzor
--! 
--! Project: 
--!
--! Author: Andreas Mueller
--! Date create: 2010-01-26
--! Date changed:
------------------------------------------

--! Use standard library
library ieee;
--! Use Logic Elements
use IEEE.STD_LOGIC_1164.ALL;
--! Use Logic Artihm
use IEEE.STD_LOGIC_ARITH.ALL;
--! Use Logic Unsigned
use IEEE.STD_LOGIC_UNSIGNED.ALL;
--! Use Numeric
use IEEE.numeric_std.all;
--! Use Config File
--use work.cfg.all;

--! Main Entity
entity top is
	port (
		--! External Global Signals
		clk100: in std_logic;
		ext_reset: in std_logic;
		--! Memory Interface
		mem_adr: out std_logic_vector (17 downto 0);
		mem_data: inout std_logic_vector (15 downto 0);
		mem_oe: out std_logic;
		mem_we: out std_logic;		
		mem_ce1: out std_logic;
		mem_ce2: out std_logic;
		mem_ub: out std_logic;
		mem_lb: out std_logic;
		--! ATMEGA Interface
		mega_clk: in std_logic;
		mega_int: out std_logic;
		mega_nib_sel: in std_logic;
		mega_rw: in std_logic;
		mega_reg_sel: in std_logic_vector (1 downto 0);
		mega_data: inout std_logic_vector (3 downto 0);
		--! IO-Ports
		io_dir: out std_logic;
		io_data: inout std_logic_vector (23 downto 0)	
	);
end top;

--! Architecture is RTL
architecture RTL of top is
--! Component Time_Counter





begin


end RTL;