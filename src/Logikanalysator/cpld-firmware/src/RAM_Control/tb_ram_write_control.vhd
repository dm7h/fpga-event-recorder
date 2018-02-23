-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench für CPLD Komponente: ram_write_control
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_ram_write_control is
end tb_ram_write_control;

-------------------------------------------------------------------------------

architecture tbench of tb_ram_write_control is

  -- definition of a clock period
  constant period : time := 10 ns;
  signal clken_p  : boolean := true;
  constant cnt_delay_width : natural := 1;
  constant cnt_delay_count : natural := 1;
  
  signal clk, master_reset, store_data, ram_write_ready, ram_rw, write_enable, A_cnt_ram, we : std_ulogic;
  signal ram_write_data : std_ulogic_vector(31 downto 0);
  signal ram_data_out : std_ulogic_vector(15 downto 0);
  
  component ram_write_control
    generic (
      cnt_delay_width : natural := 1;
      cnt_delay_count : natural := 1  -- (Anzahl der zu Wartenden Takte) - 1 
                                      -- durch moore-Automaten im steuerwerk ergibt sich
                                      -- bereits bei der ueberpruefung des carry-Ausgangs
                                      -- eine verzoegerung von einem Takt 
    );
    port (
      clk : in std_ulogic;
      master_reset : in std_ulogic;
      
      ram_write_data : in std_ulogic_vector(31 downto 0);
      ram_data_out : out std_ulogic_vector(15 downto 0);
      
      store_data : in std_ulogic;
      ram_write_ready : out std_ulogic;
      
      ram_rw : out std_ulogic;
      write_enable : out std_ulogic;
      A_cnt_ram : out std_ulogic;
      
      we : out std_ulogic
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
  
  dut : ram_write_control
    generic map (
      cnt_delay_width => cnt_delay_width,
      cnt_delay_count => cnt_delay_count
    )
    port map (
      clk => clk,
      master_reset => master_reset,
      ram_write_data => ram_write_data,
      ram_data_out => ram_data_out,
      store_data => store_data,
      ram_write_ready => ram_write_ready,
      ram_rw => ram_rw,
      write_enable => write_enable,
      A_cnt_ram => A_cnt_ram,
      we => we
    );
    
  stimuli_p : process
  begin
    master_reset <= '0';
    ram_write_data <= x"deadbeef";
    store_data <= '0';
    wait for period*2;
    master_reset <= '1';
    wait for period*2;
    master_reset <= '0';
    wait for period*2;
    
    store_data <= '1';
    wait for period;
    store_data <= '0';
    
    wait for period*50;
    
    wait;
  end process;
  
end architecture;
