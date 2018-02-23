-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench für CPLD Komponente: receive_control
-- Datum: 29.11.2013
-- Vers.: 1.0
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_receive_control is
end tb_receive_control;

-------------------------------------------------------------------------------

architecture tbench of tb_receive_control is
  
	component receive_control
		port  (
			clk				: in std_ulogic;
			master_reset	: in std_ulogic;
			
			M_data_in		: in std_ulogic_vector (3 downto 0);
			M_clk				: in std_ulogic;
			M_rw				: in std_ulogic;
			M_nib_sel		: in std_ulogic;
			
			reg_data			: out std_ulogic_vector (7 downto 0);
			reg_data_valid	: out std_ulogic
		);
	end component;
	
	  signal clken_p  : boolean := true;
  
  		signal clk					:  std_ulogic;
		signal master_reset		:  std_ulogic;
		signal M_clk					:  std_ulogic;
		signal M_rw					:  std_ulogic;
		signal M_nib_sel			:  std_ulogic;
		signal M_data_in : std_ulogic_vector(3 downto 0);
    signal reg_data : std_ulogic_vector(7 downto 0);
    signal reg_data_valid	:  std_ulogic;
  
  
  -- definition of a clock period
  constant period : time := 10 ns;

  -- definition of M_clk period
  constant M_clk_period : integer := 5;
  
begin  -- tbench
  
    -- clock generation
  clock_proc : process
  begin
    while clken_p loop
      clk <= '0'; wait for period/2;
      clk <= '1'; wait for period/2;
    end loop;
    wait;
  end process;
  
  DUT: receive_control
    port map (
      clk => clk,
      master_reset => master_reset,
      M_data_in => M_data_in,
      M_clk => M_clk,
      M_rw => M_rw,
      M_nib_sel => M_nib_sel,
      reg_data => reg_data,
      reg_data_valid => reg_data_valid
    );
    
    
  stimuli_p : process
  begin
    master_reset <= '0';
    M_clk <= '0';
    M_nib_sel <= '0';
    M_rw <= '0';
    M_data_in <= x"1";
    wait for period;
    
    master_reset <= '1';
    wait for period;
    master_reset <= '0';
    
    wait for period * M_clk_period;
    M_clk <= '1';
    wait for period * M_clk_period;
    M_clk <= '0';
    M_rw <= '1';
    wait for period * M_clk_period;
    
    
    M_clk <= '1';
    wait for period * M_clk_period;
    M_clk <= '0';
    M_data_in <= x"2";
    wait for period * M_clk_period;
    
    M_clk <= '1';
    wait for period * M_clk_period;
    M_clk <= '0';
    M_nib_sel <= '1'; 
    M_data_in <= x"3";
    wait for period * M_clk_period;
    
    M_clk <= '1';
    wait for period * M_clk_period;
    M_clk <= '0';
    M_data_in <= x"4";
    wait for period * M_clk_period; 
    
    M_clk <= '1';
    wait for period * M_clk_period;
    M_clk <= '0';
    M_data_in <= x"5";
    wait for period * M_clk_period;
    
    M_clk <= '1';
    wait for period;
    M_rw <= '0';
    wait for period * M_clk_period;
    M_clk <= '0';
    M_data_in <= x"6";
    wait for period * M_clk_period;
    
    wait; 
    
  end process;
  

end tbench;