-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench fuer CPLD Komponenten: falling_edge_detector und rising_edge_detector
-- Datum: 05.12.2013
-- Vers.: 1.0
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_edge_detector is
END tb_edge_detector;

-------------------------------------------------------------------------------

architecture tbench of tb_edge_detector is
  
    -- definition of a clock period
  constant period : time := 10 ns;
  
  signal clken_p  : boolean := true;
  signal clk	: std_ulogic;
  signal rst_i	: std_ulogic;
  signal data	: std_ulogic;
  signal rise_o	: std_ulogic;
  signal fall_o	: std_ulogic;
  
  component falling_edge_detector
    port (
      clk_i : in std_ulogic;
      rst_i : in std_ulogic;
      d_i : in std_ulogic;
      fall_o : out std_ulogic
    );
  end component;
  
  component rising_edge_detector
    port (
      clk_i : in std_ulogic;
      rst_i : in std_ulogic;
      d_i : in std_ulogic;
      rise_o : out std_ulogic
    );
  end component;
  
begin
  
  -- clock generation
  clock_proc : process
  begin
    while clken_p loop
      clk <= '0'; wait for period/2;
      clk <= '1'; wait for period/2;
    end loop;
    wait;
  end process;
  
  dut1: falling_edge_detector
    port map (
      clk_i => clk,
      rst_i => rst_i,
      d_i => data,
      fall_o => fall_o
    );
    
  dut2: rising_edge_detector
    port map (
      clk_i => clk,
      rst_i => rst_i,
      d_i => data,
      rise_o => rise_o
    );
    
  stimuli_p : process
  begin
    
    rst_i <= '0';
    data <= '0';
    
    wait for period * 3;
    
    data <= '1';
    wait for period * 3;
    data <= '0';
    
    wait for period * 3;
    data <= '1';
    wait for period * 3;
    rst_i <= '1';
    wait for period * 3;
    
    wait;
    
  end process;
  
  
end architecture;
