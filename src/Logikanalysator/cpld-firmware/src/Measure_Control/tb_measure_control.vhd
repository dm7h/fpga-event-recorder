-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench fuer CPLD Komponente: measure_control
-- Datum: 15.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_measure_control is
end tb_measure_control;

architecture tbench of tb_measure_control is
  
  constant period : time := 10 ns;
  signal clken_p  : boolean := true;
  signal clk : std_ulogic;
  
  signal master_reset, store_data, measure_start, measure_stop, ram_write_ready : std_ulogic;
  signal measure_complete : std_ulogic;
  signal inp_vec : std_ulogic_vector (7 downto 0);
  signal measured_data : std_ulogic_vector (31 downto 0);
  
  component measure_control
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
  
  dut: measure_control
    port map (
		clk				=> clk,
		master_reset	=> master_reset,		
		inp_vec		    => inp_vec,
		measure_start   => measure_start,
		measure_stop    => measure_stop,
		ram_write_ready => ram_write_ready,
		store_data      => store_data,
		measured_data32 => measured_data,
		measure_complete=> measure_complete
		
      );
  
  
  stimuli_p : process
  begin
  --init
  inp_vec <= "00000000";
	measure_start <= '0';
	measure_stop <= '0';
	ram_write_ready <= '0';
	master_reset <='1';
	wait for period;
	master_reset <='0';
	wait for period;
	
	--=========test=============
	
	
	
	ram_write_ready <= '1';
	wait for period;
	measure_start <= '1';
	wait for period * 30;
	measure_stop <= '1';
	wait for period * 30;
	measure_stop <= '0';
	wait for period * 30; 
	-- first trigger
	inp_vec <= "00001011";
	wait until store_data = '1';
	wait for period * 2;
	ram_write_ready <= '0';	
	wait for period * 10;
	ram_write_ready <= '1';
	-- ram ready
		
	wait for period * 20;
	--next trigger
	inp_vec <= "01100101";
	wait until store_data = '1';
	wait for period * 2;
	ram_write_ready <= '0';	
	wait for period * 10;
	ram_write_ready <= '1';
	
	
	
   
    wait;
end process;
  
end architecture;