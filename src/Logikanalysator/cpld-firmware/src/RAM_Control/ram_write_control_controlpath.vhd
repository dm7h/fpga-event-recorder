-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: ram_write_control_controlpath
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity ram_write_control_controlpath is 
  port (
    clk : in std_ulogic;
    master_reset : in std_ulogic;
    
    store_data : in std_ulogic;
    ram_write_ready : out std_ulogic;
    
    ram_rw : out std_ulogic;
    write_enable : out std_ulogic;
    A_cnt_ram : out std_ulogic;

    cnt2_car : in std_ulogic;
    cnt_delay_car : in std_ulogic;    
    cnt2_en : out std_ulogic;
    cnt2_rst : out std_ulogic;
    cnt_delay_rst : out std_ulogic;
    
    we : out std_ulogic
  );
end ram_write_control_controlpath;

architecture rtl of ram_write_control_controlpath is
  
  type state_t is (init, idle, start, store1, store2, store3, store4, complete);
  signal current_state, next_state : state_t;
  
begin
  
  process(current_state, store_data, cnt2_car, cnt_delay_car)
  begin
    next_state <= current_state;
    --Default Werte (Ausgang)
    ram_write_ready <= '0';
    ram_rw <= '0';
    write_enable <= '0';
    A_cnt_ram <= '0';
    cnt2_en <= '0';
    cnt2_rst <= '0';
    cnt_delay_rst <= '0';
    we <= '0';
    
    case current_state is
    when init =>
      ram_rw <= '1';
      we <= '1';
      next_state <= idle;
    when idle =>
      ram_rw <= '1';
      ram_write_ready <= '1';
      we <= '1';
      if store_data = '1' then next_state <= start; end if;
    when start =>
      write_enable <= '1';
      cnt2_rst <= '1';
      we <= '1';
      next_state <= store1;
    when store1 =>
      write_enable <= '1';
      cnt_delay_rst <= '1';
      next_state <= store2;
    when store2 =>
      write_enable <= '1';
      if cnt_delay_car = '1' then next_state <= store3; end if;
    when store3 =>
      write_enable <= '1';
      A_cnt_ram <= '1';
      we <= '1';
      next_state <= store4;
    when store4 =>
      write_enable <= '1';
      cnt2_en <= '1';
      we <= '1';
      if cnt2_car = '0' then  next_state <= store1; else
                              next_state <= complete; end if;
    when complete =>
      ram_write_ready <= '1';
      ram_rw <= '1';
      we <= '1';
      next_state <= idle;
    
    when others => next_state <= init;
    end case;
  
  end process;
  
  current_state <= init when master_reset = '1' else next_state when rising_edge(clk);
   
end architecture;
