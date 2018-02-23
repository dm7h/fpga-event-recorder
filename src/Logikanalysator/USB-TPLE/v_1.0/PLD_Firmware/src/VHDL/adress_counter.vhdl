------------------------------------------
--! @File adress_counter.vhdl
--! @Brief Counter Module Memory Adress
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
entity adress_counter is
	generic(
		--!Parameters
		cnt_size: positive := 19	
	);
	port (
		--!Global Signals
		clk100: in std_logic;
		ext_reset: in std_logic;
		--! Counter Interface
		a_cnt: in std_logic;
		a_reset: in std_logic;
		a_carry: out std_logic;
		a_out: out std_logic_vector(cnt_size-2 downto 0);
		a_ce1: out std_logic;
		a_ce2: out std_logic
	);
end adress_counter;

--! Architecture is RTL
architecture RTL of adress_counter is
	--! Signals
	signal s_counter: std_logic_vector(cnt_size downto 0);

begin

COUNT: process(clk100, ext_reset)
	begin
		if ext_reset = '1' then				--! Asynchronous reset overweights
			s_counter <= (others => '0');
		elsif clk100 = '1' and clk100'event then
			if a_reset = '1' then			--! Synchronous reset
				s_counter <= (others => '0');
			elsif a_cnt = '1' then
				s_counter <= s_counter + 1;
			end if;
			
			if s_counter(cnt_size) = '1' then
				a_carry <= '1';
				s_counter <= (others => '0');		
			end if;
			--! Chipselect on Adresswire 18
			if s_counter(cnt_size-1) = '0' then
				a_ce1 <= '1';
				a_ce2 <= '0';
			else
				a_ce1 <= '0';
				a_ce2 <= '1';
			end if;				
		end if;
	end process COUNT;
	
	--! Glue Logic
	a_out <= s_counter(cnt_size-1 downto 0);
	
end RTL;
