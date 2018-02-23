
-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: cntup_n_maxm_no_overflow
-- Datum: 10.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cntup_n_maxm_no_overflow_limit is
  generic (
    cnt_width : natural := 18; -- Bitbreite Zaehler
    cnt_max : natural := 262135; -- maximaler wert; bei erreichen wird nicht weiter gezaehlt, um overflow zu vermeiden
	 cnt_limit : natural := 262131 -- bei erreichen dieses Wertes wird limit_reached aktiv
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
end cntup_n_maxm_no_overflow_limit;

architecture rtl of cntup_n_maxm_no_overflow_limit is
  
  signal next_state, current_state : unsigned(cnt_width-1 downto 0);
  
begin
  
  adder : next_state <= current_state when current_state >= to_unsigned(cnt_max, cnt_width) else
                        current_state + 1;
                        
  count_register : process(master_reset, clk, count_enable, count_reset)
  begin
    if master_reset = '1' then
      current_state <= (OTHERS => '0');
  elsif rising_edge(clk) then
    if count_reset = '1' then
      current_state <= (OTHERS => '0');
    elsif count_enable = '1' then
      current_state <= next_state;
    end if;
  end if;
  end process;
  
  output : count_o <= std_ulogic_vector(current_state);
  
  carry : max_reached_o <= '1' when current_state >= to_unsigned(cnt_max, cnt_width) else '0';
  
  limit_reached_o <= '1' when current_state >= to_unsigned(cnt_limit, cnt_width) else '0';
   
end architecture;
