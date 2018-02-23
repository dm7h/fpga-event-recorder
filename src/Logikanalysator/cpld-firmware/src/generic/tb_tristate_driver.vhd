-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench für CPLD Komponente: tristate_driver
-- Datum: 15.11.2013
-- Vers.: 1.0
-- ===========================================================================

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;

ENTITY tb_tristate_driver IS
END tb_tristate_driver;

ARCHITECTURE tbench OF tb_tristate_driver IS
  
  CONSTANT data_width : positive := 4;
  
  COMPONENT generic_tristate_driver
    GENERIC(
      width : natural := 4);
    PORT(
      bidir_data    : INOUT STD_LOGIC_VECTOR (width-1 DOWNTO 0);
      dir           : IN STD_ULOGIC;
      data_out      : IN STD_ULOGIC_VECTOR (width-1 DOWNTO 0);
      data_in       : OUT STD_ULOGIC_VECTOR (width-1 DOWNTO 0));
  END COMPONENT;
  
  SIGNAL bidir_data     : STD_LOGIC_VECTOR(data_width-1 DOWNTO 0);
  SIGNAL dir       : STD_ULOGIC;
  SIGNAL data_out : STD_ULOGIC_VECTOR (data_width-1 DOWNTO 0);
  SIGNAL data_in  : STD_ULOGIC_VECTOR (data_width-1 DOWNTO 0);
  
  SIGNAL mega_data  : STD_LOGIC_VECTOR (data_width-1 DOWNTO 0);  -- Signal auf Datenleitungen, das der atmega aktiv auf die Leitungen legt
  
BEGIN  -- tbench
  
  DUT : generic_tristate_driver
    GENERIC MAP (
      width => data_width)
    PORT MAP (
      bidir_data      => bidir_data,
      dir        => dir,
      data_out  => data_out,
      data_in   => data_in);
  
  stimuli_p : PROCESS
  BEGIN

  data_out <= "1001";
  bidir_data <= "1010";
  mega_data <= "1010";
  dir <= '1';
  WAIT FOR 10 ns;
  
  dir <= '0';      --Kurzschluss auf Datenleitung, da uC nicht hochohmig
  WAIT FOR 10 ns;

  bidir_data <= "ZZZZ";
  mega_data <= "ZZZZ";
  dir <= '0';
  WAIT FOR 10 ns;
  
  data_out <= "0110";
  WAIT FOR 10 ns;
  
  dir <= '1';
  WAIT FOR 10 ns;
  
  bidir_data <= "0101";
  mega_data <= "0101";
  WAIT FOR 10 ns;
  
  WAIT;
  END PROCESS;

END tbench;








