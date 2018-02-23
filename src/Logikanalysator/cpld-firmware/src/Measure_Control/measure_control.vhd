-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: measure_control
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity measure_control is
	port  (
		clk				: in std_ulogic;
		master_reset	: in std_ulogic;
		
		inp_vec		    : in std_ulogic_vector (7 downto 0);
		measure_start   : in std_ulogic;
		measure_stop    : in std_ulogic;
		ram_write_ready : in std_ulogic;
		
		
		store_data      : out std_ulogic;
		measured_data32 : out std_ulogic_vector (31 downto 0);
		measure_complete: out std_ulogic		
	);
end measure_control;

architecture stucture of measure_control is

component measure_control_datapath
	port (
		clk             : in std_ulogic;
		master_reset    : in std_ulogic;
	
		inp_vec         : in std_ulogic_vector (7 downto 0);
		store_data		: in std_ulogic;
				
		measured_data   : out std_ulogic_vector (7 downto 0)
	);
end component;

component measure_control_controlpath
	port (
		clk	            : in std_ulogic;
		master_reset    : in std_ulogic;
		
		measure_start   : in std_ulogic;
		measure_stop	 : in std_ulogic;
		ram_write_ready : in std_ulogic;
		trigger         : in std_ulogic;
		
		measure_complete: out std_ulogic;
		store_data      : out std_ulogic;
		write_end_data  : out std_ulogic;
		measure_init    : out std_ulogic;
		enable_timestamp: out std_ulogic
	);
end component;

component trigger
port (
	clk                 : in std_ulogic;
	master_reset        : in std_ulogic;
	input               : in std_ulogic_vector(7 downto 0);
	freqdiv             : in std_ulogic;
	timestamp_overflow  : in std_ulogic;
	measure_init        : in std_ulogic; 
	trigger_out         : out std_ulogic
	);
end component;

component timestamp_cnt
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
 end component;

component freqdiv
  generic (
    cnt_width : natural := 4;  -- Bitbreite Zaehler
    cnt_max : natural := 15  
  );
  port (
    clk : in std_ulogic;
    master_reset       : in std_ulogic;
    count_enable       : in std_ulogic;
    count_reset        : in std_ulogic;
    count_o            : out std_ulogic_vector(cnt_width-1 downto 0);
    freqdiv_out        : out std_ulogic
   
  );

end component;


signal internal_store_data	: std_ulogic;
signal internal_freqdiv_out : std_ulogic;
signal internal_timestamp_overflow: std_ulogic;
signal internal_timestamp_out: std_ulogic_vector(15 downto 0);
signal internal_enable_timestamp : std_ulogic;
signal internal_count_enable : std_ulogic;
signal internal_measure_init: std_ulogic;
signal internal_trigger: std_ulogic;
signal internal_measured_data : std_ulogic_vector(7 downto 0);
signal internal_write_end_data :std_ulogic;
signal internal_end_byte: std_ulogic_vector(7 downto 0);
begin

measure_control_datapath_i0: measure_control_datapath
port map (
	clk             => clk,
	master_reset    => master_reset,
	inp_vec         => inp_vec,
	store_data  	=> internal_store_data,
	measured_data    => internal_measured_data
	);

measure_control_controlpath_i0: measure_control_controlpath
port map (
	clk				=> clk,
	master_reset	=> master_reset,
	measure_start   => measure_start,
	measure_stop    => measure_stop,
	ram_write_ready => ram_write_ready,
	trigger         => internal_trigger,
	measure_complete=> measure_complete,
	store_data      => internal_store_data,
	write_end_data  => internal_write_end_data,
	measure_init    => internal_measure_init,
	enable_timestamp=> internal_enable_timestamp
	);
	
trigger_i0: trigger
port map(
	clk                 => clk,
	master_reset        => master_reset,
	input               => inp_vec,
	freqdiv             => internal_freqdiv_out,
	timestamp_overflow  => internal_timestamp_overflow,
	measure_init        => internal_measure_init,
	trigger_out         => internal_trigger
	);
	
timestamp_cnt_i0: timestamp_cnt
generic map(
    cnt_width => 16,  -- Bitbreite Zaehler
    cnt_max => 65535    
  )
port map(
    clk                => clk,
    master_reset       => master_reset,
    count_enable       => internal_count_enable,
    count_reset        => '0',
    timestamp_out      => internal_timestamp_out,
    timestamp_overflow => internal_timestamp_overflow   
  );

freqdiv_i0: freqdiv
generic map(
    cnt_width => 4,
    cnt_max   => 15    
  )
port map(
    clk           => clk,
    master_reset  => master_reset,
    count_enable  => '1',
    count_reset   => '0',
    count_o       => open,
    freqdiv_out   => internal_freqdiv_out   
  );
internal_count_enable <= internal_enable_timestamp and internal_freqdiv_out;
store_data <= internal_store_data;
internal_end_byte <= "11111111" when internal_write_end_data = '1' else "00000000";
measured_data32 <= internal_measured_data & internal_timestamp_out & internal_end_byte;

	
end architecture;