-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: control_register
-- Datum: 01.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity control_register is
  port (
    clk : in std_ulogic;
    master_reset : in std_ulogic;
    reg_data_valid : in std_ulogic;
	 start_measure_cmd : out std_ulogic;
	 stop_measure_cmd : out std_ulogic;
	 reset_cmd : out std_ulogic;
	 send_cmd : out std_ulogic;
	 error_contr_reg : out std_ulogic;
    reg_data : in std_ulogic_vector(7 downto 0)
  );
end control_register;

architecture rtl of control_register is
  
  signal internal_register_data : std_ulogic_vector(7 downto 0);
  
  
begin
  
  data_register: internal_register_data <= (others => '0') when master_reset = '1' else
                                    reg_data when rising_edge(clk) and reg_data_valid = '1';
												
												
												
  process(internal_register_data)
  begin
	 start_measure_cmd <= '0';
	 stop_measure_cmd <= '0';
	 reset_cmd <= '0';
	 send_cmd <= '0';
	 error_contr_reg <= '0';
	  
    case internal_register_data(3 downto 0) is
	 when "0000" =>
		--DUMMY
    when "0100" =>
      start_measure_cmd <= '1'; -- Messung starten
    when "0101" =>
      stop_measure_cmd <= '1'; -- Messung stoppen
    when "0110" =>
      send_cmd <= '1'; -- Auslesen der Daten starten
    when "0111" =>
      reset_cmd <= '1'; -- Reset
    when others =>
      error_contr_reg <= '1'; 
    end case;
end process;
  
  
  
end architecture;