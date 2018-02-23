-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: generic_tristate_driver
-- Datum: 23.11.2013
-- Vers.: 1.1
-- ===========================================================================

LIBRARY IEEE;
USE IEEE.STD_LOGIC_1164.ALL;
USE IEEE.NUMERIC_STD.ALL;

ENTITY generic_tristate_driver IS
  GENERIC(
    width : natural := 4);
  PORT(
    bidir_data    : INOUT STD_LOGIC_VECTOR (width-1 DOWNTO 0);
    dir           : IN STD_ULOGIC;	-- 0: CPLD write; 1: CPLD read
    data_out    : IN STD_ULOGIC_VECTOR (width-1 DOWNTO 0);
    data_in     : OUT STD_ULOGIC_VECTOR (width-1 DOWNTO 0));
END generic_tristate_driver;

ARCHITECTURE rtl OF generic_tristate_driver IS

BEGIN 
    PROCESS (dir, bidir_data, data_out) 
        BEGIN 
        IF( dir = '0') THEN
          bidir_data <= To_StdLogicVector(data_out);  
          data_in <= data_out;
        ELSE
          bidir_data <= (OTHERS => 'Z');
          data_in <= To_StdULogicVector(bidir_data);  
        END IF;
    END PROCESS;
END rtl;
