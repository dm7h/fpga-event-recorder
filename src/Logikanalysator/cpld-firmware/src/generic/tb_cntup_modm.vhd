-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench für CPLD Komponente: cntup_modm
-- Datum: 23.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_cntup_modm is
end tb_cntup_modm;

architecture tbench of tb_cntup_modm is
  
  constant period : time := 10 ns;
  signal clken_p  : boolean := true;
  constant bit_with : natural := 18;
  constant cnt_max : natural := 262143;
  signal clk : std_ulogic;
  
  component cntup_modm
    generic (
      width   : natural := 18;
      cnt_max : natural := 262143
    );
    port (
     clk            : in std_ulogic;
     master_reset   : in std_ulogic;
     count_enable   : in std_ulogic;
     count_reset    : in std_ulogic;
     count_o        : out std_ulogic_vector(width-1 downto 0);
     carry_o        : out std_ulogic
   );
  end component;
  
  signal master_reset, count_enable, count_reset, carry_o : std_ulogic;
  signal count_o : std_ulogic_vector(bit_with-1 downto 0);
  
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
  
  
  dut : cntup_modm
  generic map (
    width => bit_with,
    cnt_max => cnt_max
  )
  port map (
    clk           => clk,
    master_reset  => master_reset,
    count_enable  => count_enable,
    count_reset   => count_reset,
    count_o       => count_o,
    carry_o       => carry_o
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
    wait for period*10;
    count_reset <= '1';
    wait for period*10;
    count_reset <= '0';
    
    wait for period*cnt_max + 10 ns;
    wait;
  end process;


end architecture;