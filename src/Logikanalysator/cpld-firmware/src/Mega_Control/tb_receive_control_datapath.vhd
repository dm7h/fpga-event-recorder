-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench fuer CPLD Komponente: receive_control_datapath
-- Datum: 15.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity tb_receive_control_datapath is
end tb_receive_control_datapath;

architecture tbench of tb_receive_control_datapath is
  
  constant period : time := 10 ns;
  signal clken_p  : boolean := true;
  signal clk : std_ulogic;
  
  signal reset, nib_sel_rx, nib_sel_valid : std_ulogic;
  signal M_data_in : std_ulogic_vector (3 downto 0);
  signal reg_data : std_ulogic_vector (7 downto 0);
  
  component receive_control_datapath
	port (
		clk				: in std_ulogic;
		reset				: in std_ulogic;
	
		M_data_in		: in std_ulogic_vector (3 downto 0);
		nib_sel_rx		: in std_ulogic;
		nib_sel_valid	: in std_ulogic;
		
		reg_data			: out std_ulogic_vector (7 downto 0)
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
  
  dut: receive_control_datapath
    port map (
      clk           => clk,
      reset         => reset,     
      M_data_in     => M_data_in,
      nib_sel_rx    => nib_sel_rx,
      nib_sel_valid => nib_sel_valid,
      reg_data      => reg_data
      );
  
  
  stimuli_p : process
  begin
    reset <= '0';
    wait for period*2;
    M_data_in <= "1110";
    wait for period*2;
    reset <= '1';
    wait for period*2;
    reset <= '0';
    wait for period*2;
    nib_sel_valid <= '1';
    nib_sel_rx <= '0';
    wait for period*2;
    M_data_in <= "1010";
    wait for period*2;
    nib_sel_valid <= '0';
    wait for period*2;
    nib_sel_rx <= '1';
    M_data_in <= "1100";
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2;
    nib_sel_valid <= '0';
    wait for period*2;
    M_data_in <= "0000";
    wait for period*2;
    reset <= '1';
    wait for period*2;  
  
    wait;
end process;
  
end architecture;