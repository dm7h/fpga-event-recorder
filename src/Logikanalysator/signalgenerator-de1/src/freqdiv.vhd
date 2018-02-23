-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: freqdiv
-- Datum: 20.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity freqdiv is
  generic (
    cnt_width : natural := 5;  -- Bitbreite Zaehler
	 cnt_car : natural := 42;
    cnt_max : natural := 20  
    
  );
  port (
    clk : in std_ulogic;
    master_reset       : in std_ulogic;
    count_enable       : in std_ulogic;
    count_reset        : in std_ulogic;
    count_o            : out std_ulogic_vector(cnt_width-1 downto 0);
    freqdiv_out        : out std_ulogic
   
  );
end freqdiv;


architecture structure of freqdiv is


component cntup_modm
  generic (
    cnt_width : natural := 5;
    cnt_max : natural := 20
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

signal count : std_ulogic_vector(cnt_width-1 downto 0);
  
begin
  
  counter: cntup_modm
    generic map (
        cnt_width => cnt_width,
        cnt_max => cnt_max
      )
      port map (
       clk => clk,
       master_reset => master_reset,
       count_enable => count_enable,
       count_reset => count_reset,
       count_o => count,
       carry_o => OPEN
    );
	 
	 count_o <= count;
	 freqdiv_out <= '1' when unsigned(count) > to_unsigned(cnt_car, cnt_width) else '0';
   
end architecture;
