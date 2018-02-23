-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench fuer CPLD Komponente: cntup_n_maxm_no_overflow_limit
-- Datum: 21.12.2013
-- Vers.: 1.0
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_cntup_n_maxm_no_overflow_limit is
end tb_cntup_n_maxm_no_overflow_limit;

-------------------------------------------------------------------------------

architecture tbench of tb_cntup_n_maxm_no_overflow_limit is
  
  component cntup_n_maxm_no_overflow_limit
    generic (
      cnt_width : natural := 18;
      cnt_max : natural := 262135;
  	   cnt_limit : natural := 262131
    );
    port (
      clk : in std_ulogic;
      master_reset : in std_ulogic;
      count_enable : in std_ulogic;
      count_reset : in std_ulogic;
      count_o : out std_ulogic_vector(cnt_width-1 downto 0);
      max_reached_o : out std_ulogic;
    	 limit_reached_o : out std_ulogic
    );
  end component;
  
  
  -- definition of a clock period
  constant period : time := 10 ns;
  
  signal clken_p  : boolean := true;
  
  constant cnt_width : natural := 18;
  constant cnt_max : natural := 262139;
  constant cnt_limit : natural := 262135;
  
	signal clk					:  std_ulogic;
	signal master_reset		:  std_ulogic;
	
	signal count_enable, count_reset, limit_reached : std_ulogic;
	signal count_o : std_ulogic_vector(cnt_width-1 downto 0);
  
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
  
  dut : cntup_n_maxm_no_overflow_limit
  generic map (
     cnt_width => cnt_width,
     cnt_max => cnt_max,
     cnt_limit => cnt_limit
   )
   port map (
      clk => clk,
      master_reset => master_reset,
      count_enable => count_enable,
      count_reset => count_reset,
      count_o => count_o,
      limit_reached_o => limit_reached
   );
  
  stimuli_p : process
  begin
    count_enable <= '0';
    count_reset <= '0';
    master_reset <= '0';
    wait for period*2;
    master_reset <= '1';
    wait for period*2;
    master_reset <= '0';
    wait for period*2;
    
    count_enable <= '1';
    wait for period*20;
    count_reset <= '1';
    wait for period*10;
    count_reset <= '0';   
    
    
    wait for period*cnt_max + 10 ns;
    
    wait for period*10;
    count_reset <= '1';
    wait for period*10;
    count_reset <= '0';
    
    wait for period*10;
    
    wait;
  end process;
  
end architecture;
