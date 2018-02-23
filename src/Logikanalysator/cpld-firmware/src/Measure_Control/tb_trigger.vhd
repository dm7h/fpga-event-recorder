-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench für CPLD Komponente: input_change_detect
-- Datum: 10.12.2013
-- Vers.: 1.0
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_trigger is

end tb_trigger;

-------------------------------------------------------------------------------

architecture tbench of tb_trigger is
 component trigger
  port (
    clk                 : in std_ulogic;
	master_reset        : in std_ulogic;
	input               : in std_ulogic_vector( 7 downto 0);
	freqdiv             : in std_ulogic;
	timestamp_overflow  : in std_ulogic;
	measure_init        : in std_ulogic; 
	trigger_out         : out std_ulogic
	);
	end component;
	
	 -- definition of a clock period
  constant period : time := 10 ns;

  -- definition of M_clk period
  constant M_clk_period : integer := 5;
  
  signal clken_p 		      : boolean := true;
  signal clk			      : std_ulogic;
  
  signal input                : std_ulogic_vector( 7 downto 0);
  signal measure_init         : std_ulogic;
  signal freqdiv          : std_ulogic;
  signal timestamp_overflow   : std_ulogic;
  signal master_reset         : std_ulogic;
  signal trigger_out          : std_ulogic;
  
  begin
    DUT: entity work.trigger(rtl)
    port map (
      clk => clk,
      master_reset => master_reset,
	    input => input,
      measure_init  => measure_init,
      freqdiv => freqdiv,
	    timestamp_overflow => timestamp_overflow,
	    trigger_out => trigger_out
        );
  
    -- clock generation
  clock_proc : process
  begin
    while clken_p loop
      
      clk <= '0'; wait for period/2;
      clk <= '1'; wait for period/2;
      end loop;
    wait;
  end process;
  
  freqdiv_p1: process
  begin
    while clken_p loop
	  freqdiv <= '1'; wait for period ;
	  freqdiv <= '0'; wait for period * 15;
	  end loop;
	wait;
  end process;
  
  
  stimuli_p : process
  begin
  --init
  master_reset <= '1';
  input <= "00000000";
  timestamp_overflow <= '0';
  measure_init <= '0';
  wait for period ;
  master_reset <= '0';
  wait for period;
  measure_init <= '1';
  wait for period; 
  measure_init <= '0';
  wait for period/2;
  --test
  input <= "00000001";
  wait for period * 34;
  input <= "00000011";
  wait for period * 34;
  input <= "00000111";
  wait for period * 30;
  input <= "00001111";
  wait for period *4;
  master_reset <= '1';
  wait for period * 8;
  master_reset <= '0';
  measure_init <= '1';
  wait for period ;
  measure_init <= '0';
  
  wait for period * 34;
  input <= "00011111";
  wait for period * 34;
  input <= "00111111";
  wait for period * 34;
  input <= "01111111";
  wait for period * 34;
  input <= "11111111";
  wait for period * 34;
  input <= "11111110";
  wait for period * 34;
  input <= "11111100";
  wait for period * 34;
  input <= "11111000";
  wait for period * 34;
  input <= "11110000";
  wait for period * 34;
  input <= "11100000";
  wait for period * 34;
  input <= "11000000";
  wait for period * 34;
  input <= "10000000";
  wait for period * 34;
  input <= "00000000";
  wait for period * 34;
  input <= "11111111";
  wait for period * 34;
  input <= "00000000";
  
  
  
  
  
  
  
  
  
  wait;
  end process;
  
end tbench;
   