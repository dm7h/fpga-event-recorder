-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: ram_write_control
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity ram_write_control is
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
end ram_write_control;

architecture structure of ram_write_control is
  
  signal cnt2_en, cnt2_rst, cnt_delay_rst, cnt2_car, cnt_delay_car : std_ulogic;
  
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
  
  component ram_write_control_controlpath
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
  end component;
  
begin
  
  I1 : ram_write_control_datapath
    generic map(
      cnt_delay_width => cnt_delay_width,
      cnt_delay_count => cnt_delay_count
    )
    port map(
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
    
  I2 : ram_write_control_controlpath
    port map(
      clk => clk,
      master_reset => master_reset,
      store_data => store_data,
      ram_write_ready => ram_write_ready,
      ram_rw => ram_rw,
      write_enable => write_enable,
      A_cnt_ram => A_cnt_ram,
      cnt2_car => cnt2_car,
      cnt_delay_car => cnt_delay_car,
      cnt2_en => cnt2_en,
      cnt2_rst => cnt2_rst,
      cnt_delay_rst => cnt_delay_rst,
      we => we
    );
  
end architecture;
