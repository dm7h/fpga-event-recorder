-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: status_register
-- Datum: 22.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity status_register is
	port (
		clk : in std_ulogic;
		master_reset : in std_ulogic;
		
		measure_start : in std_ulogic;
		
		status_register_data : out std_ulogic_vector(15 downto 0)
	);
end status_register;

architecture rtl of status_register is

	signal reg_measure_start : std_ulogic;

begin
	
	reg_measure_start <=	'0' when master_reset = '1' else
								measure_start when rising_edge(clk);
										
	status_register_data <= "000000000000000" & reg_measure_start;

end architecture;