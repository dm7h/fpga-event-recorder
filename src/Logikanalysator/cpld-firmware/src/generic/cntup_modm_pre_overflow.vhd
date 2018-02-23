-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: cntup_modm_pre_overflow
-- Datum: 23.11.2013
-- Vers.: 1.0
-- ===========================================================================

-- In dieser Version wird der Overflow/carry_o einen Zählwert bevor er tatsaechlich ueberlauft erzeugt
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity cntup_modm_pre_overflow is
  generic (
    cnt_width   : natural := 18;
    cnt_max : natural := 262143
  );
  port (
    clk            : in std_ulogic;
    master_reset   : in std_ulogic;
    count_enable   : in std_ulogic;
    count_reset    : in std_ulogic;
    count_o        : out std_ulogic_vector(cnt_width-1 downto 0);
    carry_o        : out std_ulogic
  );
end cntup_modm_pre_overflow;

architecture rtl of cntup_modm_pre_overflow is
  
  signal next_state, current_state : unsigned(cnt_width-1 downto 0);
  
begin
  
  adder : next_state <= (OTHERS => '0') when current_state = cnt_max else
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
  
  carry : carry_o <= '1' when current_state = cnt_max - 1  else '0';
   
end architecture;
