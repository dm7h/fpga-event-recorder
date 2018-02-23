-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: measure_control_datapath
-- Datum: 15.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity measure_control_datapath is
	port (
		clk             : in std_ulogic;
		master_reset    : in std_ulogic;
	
		inp_vec         : in std_ulogic_vector (7 downto 0);
		store_data		: in std_ulogic;
				
		measured_data    : out std_ulogic_vector (7 downto 0)
	);
end measure_control_datapath;

architecture rtl of  measure_control_datapath is

begin

flipflop: measured_data <= (others => '0') when master_reset = '1' else
						   inp_vec when rising_edge(clk) and store_data = '1';
end rtl;
