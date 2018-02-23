-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench fuer CPLD Komponente: send_control_controlpath
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_ram_write_control_datapath is
end tb_ram_write_control_datapath;

-------------------------------------------------------------------------------

architecture tbench of tb_ram_write_control_datapath is

  component ram_write_control_datapath
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
      
      cnt2_en : in std_ulogic;
      cnt2_rst : in std_ulogic;
      cnt_delay_rst : in std_ulogic;
      cnt2_car : out std_ulogic;
      cnt_delay_car : out std_ulogic
    );
  end component;
  
  -- definition of a clock period
  constant period : time := 10 ns;
  signal clken_p  : boolean := true;
  
  constant cnt_delay_width : natural := 1;
  constant cnt_delay_count : natural := 1;
  
  signal clk, master_reset, cnt2_en, cnt2_rst, cnt_delay_rst, cnt2_car, cnt_delay_car : std_ulogic;
  signal ram_write_data : std_ulogic_vector(31 downto 0);
  signal ram_data_out : std_ulogic_vector(15 downto 0);

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
  
  dut : ram_write_control_datapath
    generic map (
      cnt_delay_width => cnt_delay_width,
      cnt_delay_count => cnt_delay_count
    )
    port map (
      clk => clk,
      master_reset => master_reset,
      ram_write_data => ram_write_data,
      ram_data_out => ram_data_out,
      cnt2_en => cnt2_en,
      cnt2_rst => cnt2_rst,
      cnt_delay_rst => cnt_delay_rst,
      cnt2_car => cnt2_car,
      cnt_delay_car => cnt_delay_car
    );
    
  stimuli_p : process
  begin
    master_reset <= '0';
    ram_write_data <= x"deadbeef";
    cnt2_en <= '0';
    cnt2_rst <= '0';
    cnt_delay_rst <= '0';
    wait for period*2;
    master_reset <= '1';
    wait for period*2;
    master_reset <= '0';
    wait for period*2;
    
    cnt2_rst <= '1';   
    wait for period;
    
    cnt2_rst <= '0'; 
    cnt_delay_rst <= '1';
    wait for period;
    
    cnt_delay_rst <= '0';
    wait for period*3;
    
    cnt2_en <= '1';
    wait for period;
    cnt2_en <= '0';
    wait for period;
    

    cnt2_rst <= '0'; 
     cnt_delay_rst <= '1';
    wait for period;
    
    cnt_delay_rst <= '0';
    wait for period*3;

    cnt2_en <= '1';
    wait for period;
    cnt2_en <= '0';
    wait for period;
    
    
    cnt2_rst <= '0'; 
    cnt_delay_rst <= '1';
    wait for period;
    
    cnt_delay_rst <= '0';
    wait for period*3;
    
    cnt2_en <= '1';
    wait for period;
    cnt2_en <= '0';
    wait for period;
    
    
    cnt2_rst <= '0'; 
    cnt_delay_rst <= '1';
    wait for period;
    
    cnt_delay_rst <= '0';
    wait for period*3;
    
    cnt2_en <= '1';
    wait for period;
    cnt2_en <= '0';
    wait for period;
    
    
    wait;
  end process;
  
end architecture;
