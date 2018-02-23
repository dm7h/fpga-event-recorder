-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: ram_write_control_datapath
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ram_write_control_datapath is
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
end ram_write_control_datapath;

architecture rtl of ram_write_control_datapath is
  
  component cntup_modm
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
  end component;
  
  signal cnt2_o : std_ulogic_vector(0 downto 0);  
  
begin
  
  cnt_delay : cntup_modm
    generic map (
     cnt_width => cnt_delay_width,
     cnt_max => cnt_delay_count
    )
    port map (
     clk => clk,
     master_reset => master_reset,
     count_enable => '1',
     count_reset => cnt_delay_rst,
     count_o => OPEN,
     carry_o => cnt_delay_car
   );      

  cnt2 : cntup_modm
    generic map (
     cnt_width => 1,
     cnt_max => 1 -- 2 Durchlauefe;
    )
    port map (
     clk => clk,
     master_reset => master_reset,
     count_enable => cnt2_en,
     count_reset => cnt2_rst,
     count_o => cnt2_o,
     carry_o => cnt2_car
   );   
  
  data_mux : process(cnt2_o, ram_write_data)
  begin
    case cnt2_o is
    when "0" => 
      ram_data_out <= ram_write_data(15 downto 0);
    when "1" => 
      ram_data_out <= ram_write_data(31 downto 16);
    when others => 
      ram_data_out <= (others => '0');
    end case;
  end process;
  
  
end architecture;