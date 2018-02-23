-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench fuer CPLD Komponente: send_control_datapath
-- Datum: 16.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity tb_send_control_datapath is
end tb_send_control_datapath;

architecture tbench of tb_send_control_datapath is
  
  constant period : time := 10 ns;
  signal clken_p  : boolean := true;
  signal clk : std_ulogic;
  
  signal reset, M_A, M_B, nib_sel_tx, nib_sel_valid : std_ulogic;
  signal RAM_data_in, status_register : std_ulogic_vector(15 downto 0);
  signal M_data_out : std_ulogic_vector(3 downto 0);
  
  component send_control_datapath
  	port (
   	clk				        	: in std_ulogic;
  	 reset			      		: in std_ulogic;
  	
   	M_A		        			: in std_ulogic;
	  M_B		        			: in std_ulogic;
	  nib_sel_tx			   : in std_ulogic;
	  nib_sel_valid		 : in std_ulogic;
	  
	  RAM_data_in  			: in std_ulogic_vector(15 downto 0);
  	 status_register	: in std_ulogic_vector(15 downto 0);
	
   	M_data_out			   : out std_ulogic_vector(3 downto 0)
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
  
  dut: send_control_datapath
    port map (
      clk             => clk,
      reset           => reset,
      M_A             => M_A,
      M_B             => M_B,
      nib_sel_tx      => nib_sel_tx,
      nib_sel_valid   => nib_sel_valid,
      RAM_data_in     => RAM_data_in,
      status_register => status_register,
      M_data_out      => M_data_out
    );
  
  stimuli_p : process
  begin
    reset <= '0';
    wait for period*2;
    RAM_data_in <= x"abcd";
    status_register <= x"1234";
    nib_sel_valid <= '0';
    nib_sel_tx <= '0';
    M_A <= '0';
    M_B <= '0';
    reset <= '1';
    wait for period*2;
    reset <= '0';
    
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2;
    nib_sel_valid <= '0';
    nib_sel_tx <= '1';
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2;
    nib_sel_valid <= '0';
    nib_sel_tx <= '0';
    M_B <= '1';
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2;
    nib_sel_valid <= '0';
    nib_sel_tx <= '1';
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2; 
    
    M_B <= '0';
    M_A <= '1';
    nib_sel_tx <= '0';
    
   -- reset <= '1';
   -- wait for period*2;
   -- reset <= '0';
    
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2;
    nib_sel_valid <= '0';
    nib_sel_tx <= '1';
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2;
    nib_sel_valid <= '0';
    nib_sel_tx <= '0';
    M_B <= '1';
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2;
    nib_sel_valid <= '0';
    nib_sel_tx <= '1';
    wait for period*2;
    nib_sel_valid <= '1';
    wait for period*2; 
    
    reset <= '1';
    wait for period*2;
    reset <= '0';
    wait for period*2;
    nib_sel_valid <= '0';
    reset <= '1';
    wait for period*2;
    reset <= '0';
    
    wait;
  end process;
  
end architecture;