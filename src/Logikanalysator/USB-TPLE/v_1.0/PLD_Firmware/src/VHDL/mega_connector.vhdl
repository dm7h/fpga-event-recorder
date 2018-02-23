------------------------------------------
--! @File mega_connector.vhdl
--! @Brief Connector between µC and CPLD
--! 
--! Project: 
--!
--! Author: Andreas Mueller
--! Date create: 2010-02-16
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
entity mega_connector is
	generic(
		--!Parameters
		cnt_size: positive := 19	
	);
	port (
		--!Global Signals
		clk100: 	in std_logic;
		ext_reset: 	in std_logic;
		--!Data Interface
		data_bus:	inout std_logic_vector(3 downto 0);
		clk_d:		in std_logic;
		mega_ctrl:	in std_logic;	--! if 1, the clk = clk_d
		rw:			in std_logic;	--! 1 = write, 0 = read
		nib_sel:	in std_logic;	--! 1 = upper, 0 = lower
		reg_sel:	in std_logic;
		stat_int:	out std_logic;
		--!CPLD Interface
		data_ram:	in std_logic_vector(15 downto 0);
		contr_reg:	out std_logic_vector(7 downto 0);
		stat_reg:	in std_logic_vector(3 downto 0);
		stat_wr:	in std_logic;
		clk:		out std_logic	--! Global clk		
	);
end mega_connector;

--! Architecture is RTL
architecture RTL of mega_connector is
	--! Signals
	signal s_contr_reg: std_logic_vector(7 downto 0);
	signal s_stat_reg: std_logic_vector(3 downto 0);
	signal s_fifo:	std_logic_vector(3 downto 0);

begin

MUX: process(clk_d, ext_reset)
	begin
		if ext_reset = '1' then				--! Asynchronous reset overweights
			s_stat_reg <= (others => '0');
			s_contr_reg <= (others => '0');
			s_fifo <= (others => '0');
		elsif clk_d = '1' and clk_d'event then --!Atmega takes Clock-controll
		end if;	

			
	end process MUX;
	
	--! Glue Logic
	contr_reg <= s_contr_reg;
	
end RTL;
