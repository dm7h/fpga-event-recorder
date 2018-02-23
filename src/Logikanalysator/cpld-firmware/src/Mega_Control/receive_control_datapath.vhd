-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: receive_control_datapath
-- Datum: 15.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity receive_control_datapath is
	port (
		clk				: in std_ulogic;
		reset				: in std_ulogic;
	
		M_data_in		: in std_ulogic_vector (3 downto 0);
		nib_sel_rx		: in std_ulogic;
		nib_sel_valid	: in std_ulogic;
		
		reg_data			: out std_ulogic_vector (7 downto 0)
	);
end receive_control_datapath;

architecture rtl of  receive_control_datapath is

signal low_nibble : std_ulogic_vector (3 downto 0);
signal high_nibble : std_ulogic_vector (3 downto 0);

begin

flipflop_low_nibble: low_nibble <= (others => '0') when reset = '1' else
												M_data_in when rising_edge(clk) and nib_sel_valid = '1' and nib_sel_rx = '0';
												
flipflop_high_nibble: high_nibble <= (others => '0') when reset = '1' else
												 M_data_in when rising_edge(clk) and nib_sel_valid = '1' and nib_sel_rx = '1';
												
output_logic: reg_data <= high_nibble & low_nibble;

end rtl;
