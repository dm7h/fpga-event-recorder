-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: timestamp_cnt
-- Datum: 20.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity timestamp_cnt is
  generic (
    cnt_width : natural := 16;  -- Bitbreite Zaehler
    cnt_max : natural := 65535 
    
  );
  port (
    clk : in std_ulogic;
    master_reset       : in std_ulogic;
    count_enable       : in std_ulogic;
    count_reset        : in std_ulogic;
    timestamp_out      : out std_ulogic_vector(cnt_width-1 downto 0);
    timestamp_overflow : out std_ulogic
   
  );
end timestamp_cnt;


architecture structure of timestamp_cnt is
  

component cntup_modm_pre_overflow
  generic (
    cnt_width : natural := 16;
    cnt_max : natural := 65535
  );
  port (
    clk : in std_ulogic;
    master_reset : in std_ulogic;
    count_enable : in std_ulogic;
    count_reset : in std_ulogic;
    count_o : out std_ulogic_vector(cnt_width-1 downto 0);
    carry_o : out std_ulogic
  );
end component;
  
begin
  
  counter: cntup_modm_pre_overflow
    generic map (
        cnt_width => cnt_width,
        cnt_max => cnt_max
      )
      port map (
       clk => clk,
       master_reset => master_reset,
       count_enable => count_enable,
       count_reset => count_reset,
       count_o => timestamp_out,
       carry_o => timestamp_overflow
    );
   
end architecture;
