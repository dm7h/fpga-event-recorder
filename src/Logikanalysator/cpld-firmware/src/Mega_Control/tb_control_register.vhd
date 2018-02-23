-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench fuer CPLD Komponente: control_register
-- Datum: 01.12.2013
-- Vers.: 1.0
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_control_register is
END tb_control_register;

-------------------------------------------------------------------------------

architecture tbench of tb_control_register is

  constant period : time := 10 ns;
  signal clken_p  : boolean := true;
  signal clk : std_ulogic;
  
  signal master_reset, reg_data_valid : std_ulogic;
  signal reg_data : std_ulogic_vector(7 downto 0);
  
  component control_register
    port (
      clk : in std_ulogic;
      master_reset : in std_ulogic;
      reg_data_valid : in std_ulogic;
      reg_data : in std_ulogic_vector(7 downto 0)
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
  
  dut: control_register
    port map (
      clk => clk,
      master_reset => master_reset,
      reg_data_valid => reg_data_valid,
      reg_data => reg_data
    );
    
  stimuli_p : process
  begin
    master_reset <= '0';
    reg_data_valid <= '0';
    reg_data <= x"12";
    wait for period*2;
    master_reset <= '1';
    wait for period*2;
    master_reset <= '0';
    wait for period*2;
    reg_data_valid <= '1';
    wait for period;
    reg_data_valid <= '0';
    
    wait for period*2;
    master_reset <= '1';
    wait for period*2;
    master_reset <= '0';
    wait for period*2;
    
    reg_data <= x"34";
    wait for period*2;
    reg_data_valid <= '1';
    reg_data <= x"56";
    wait for period*2;
    
    master_reset <= '1';
    wait for period*2;
    master_reset <= '0';
    wait for period*2;
    
    
    wait;
  end process;
  
  
end architecture;