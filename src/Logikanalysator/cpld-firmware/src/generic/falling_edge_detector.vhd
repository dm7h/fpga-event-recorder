-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: falling_edge_detector
-- Datum: 05.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity falling_edge_detector is
  port (
    clk_i : in std_ulogic;
    rst_i : in std_ulogic;
    d_i : in std_ulogic;
    fall_o : out std_ulogic
  );
end falling_edge_detector;

architecture rtl of falling_edge_detector is
  
  signal q1, q2 : std_ulogic;
  
begin
  
  ff1: q1 <= '0' when rst_i = '1' else d_i when rising_edge(clk_i);
  
  ff2: q2 <= '0' when rst_i = '1' else q1 when rising_edge(clk_i);
  
  logic: fall_o <= not q1 and q2;
  
end architecture;

