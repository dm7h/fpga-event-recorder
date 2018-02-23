------------------------------------------
--! @File time_counter.vhdl
--! @Brief Counter Module for Timing Analisis
--! 
--! Project: 
--!
--! Author: Andreas Mueller
--! Date create: 2010-01-28
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
entity time_counter is
	generic(
		--!Parameters
		cnt_size: positive := 32	
	);
	port (
		--!Global Signals
		clk100: in std_logic;
		ext_reset: in std_logic;
		--! Counter Interface
		t_cnt: in std_logic;
		t_reset: in std_logic;
		t_mask: in std_logic_vector(4 downto 0);
		t_carry: out std_logic;
		t_out: out std_logic_vector(cnt_size-1 downto 0)
	);
end time_counter;

--! Architecture is RTL
architecture RTL of time_counter is
	--! Signals
	signal s_counter: std_logic_vector(cnt_size downto 0);

begin

COUNT: process(clk100, ext_reset)
	begin
		if ext_reset = '1' then				--! Asynchronous reset overweights
			s_counter <= (others => '0');
		elsif clk100 = '1' and clk100'event then
			if t_reset = '1' then			--! Synchronous reset
				s_counter <= (others => '0');
			elsif t_cnt = '1' then
				s_counter <= s_counter + 1;
			end if;
			
			if s_counter(CONV_INTEGER(t_mask)) = '1' then
				t_carry <= '1';
			end if;
		end if;
	end process COUNT;
	
	--! Glue Logic
	t_out <= s_counter(cnt_size-1 downto 0);
	
end RTL;
