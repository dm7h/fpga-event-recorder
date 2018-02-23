-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: send_control_datapath
-- Datum: 16.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity send_control_datapath is
	port (
		clk					: in std_ulogic;
		reset					: in std_ulogic;
		
		M_A					: in std_ulogic;
		M_B					: in std_ulogic;
		nib_sel_tx			: in std_ulogic;
		nib_sel_valid		: in std_ulogic;
		
		RAM_data_in			: in std_ulogic_vector(15 downto 0);
		status_register	: in std_ulogic_vector(15 downto 0);
		
		M_data_out			: out std_ulogic_vector(3 downto 0)
	);
end send_control_datapath;

architecture rtl of send_control_datapath is

signal current_byte : std_ulogic_vector(7 downto 0);
signal current_byte_ram : std_ulogic_vector(7 downto 0);
signal current_byte_status : std_ulogic_vector(7 downto 0);
signal current_nibble : std_ulogic_vector(3 downto 0);

begin

current_byte_ram <=	RAM_data_in(7 downto 0) when M_B = '0' else
							RAM_data_in(15 downto 8);
							
current_byte_status <=	status_register(7 downto 0) when M_B = '0' else
								status_register(15 downto 8);
								
current_byte <=	current_byte_ram when M_A = '0' else
						current_byte_status;
						
current_nibble <= 	current_byte(3 downto 0) when nib_sel_tx = '0' else
							current_byte(7 downto 4);
							
M_data_out <=	(others => '0') when reset = '1' else
					current_nibble when rising_edge(clk) and nib_sel_valid = '1';
						
end architecture;