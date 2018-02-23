-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: manager
-- Datum: 06.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity manager is
	port (
		clk : in std_ulogic;
		ext_reset : in std_ulogic;
		
		start_measure_cmd : in std_ulogic;
		stop_measure_cmd : in std_ulogic;
		reset_cmd : in std_ulogic;
		send_cmd : in std_ulogic;
		error_contr_reg : in std_ulogic;
		
		send_complete : in std_ulogic;
		
		measure_complete : in std_ulogic;
		A_limit_reached : in std_ulogic;
		
		master_reset : out std_ulogic;
		
		A_rst : out std_ulogic;
		
		send_data : out std_ulogic;
		measure_start : out std_ulogic;
		measure_stop : out std_ulogic;
		
		ram_read : out std_ulogic;
		
		M_int : out std_ulogic
	);
end manager;

architecture rtl of manager is

	signal temp_ext_reset1, temp_ext_reset2 : std_ulogic;
	signal internal_master_reset : std_ulogic;
	
	signal internal_send_data : std_ulogic;
	signal send_cmd_rising_edge : std_ulogic;
	
	signal internal_measure_start : std_ulogic;
	signal internal_measure_stop : std_ulogic;
	signal internal_ram_read, A_limit_reached_rising_edge : std_ulogic;
	signal start_measure_cmd_rising_edge : std_ulogic;
	signal stop_measure_cmd_rising_edge : std_ulogic;
	
	signal internal_measure_start_rising_edge, internal_send_data_rising_edge : std_ulogic;
	
	component rising_edge_detector
	  port (
		 clk_i : in std_ulogic;
		 rst_i : in std_ulogic;
		 d_i : in std_ulogic;
		 rise_o : out std_ulogic
	  );
	end component;
	
begin
	-- externen Reset einsynchronisieren
	resetSync1 : temp_ext_reset1 <= ext_reset when rising_edge(clk);
	resetSync2 : temp_ext_reset2 <= temp_ext_reset1 when rising_edge(clk);
	internal_master_reset <= reset_cmd or not temp_ext_reset2;
	master_reset <= internal_master_reset;
	
	-- Erzeugung von send_data (Start-Signal für Sendeautomat)
	send_cmd_rising_edge_detector: rising_edge_detector
		port map (
			 clk_i => clk,
			 rst_i => internal_master_reset,
			 d_i => send_cmd,
			 rise_o => send_cmd_rising_edge
		);
		
	send_data_logic: internal_send_data <= '0' when internal_master_reset = '1' else
														'1' when send_cmd_rising_edge = '1' and rising_edge(clk) else
														'0' when (send_complete = '1' or send_cmd = '0') and rising_edge(clk);
											
	send_data <= internal_send_data;
											
	--Erzeugung von measure_start
	start_measure_cmd_rising_edge_detector: rising_edge_detector
		port map (
			 clk_i => clk,
			 rst_i => internal_master_reset,
			 d_i => start_measure_cmd,
			 rise_o => start_measure_cmd_rising_edge
		);
		
	measure_start_logic: internal_measure_start <= 	'0' when internal_master_reset = '1' else
																	'1' when start_measure_cmd_rising_edge = '1' and rising_edge(clk) else
																	'0' when measure_complete = '1'  and rising_edge(clk); --gibts measure_complete; rising edge end_cmd????
		
	measure_start <= internal_measure_start;
	
	--Erzeugung von measure_stop
	stop_measure_cmd_rising_edge_detector: rising_edge_detector
		port map (
			 clk_i => clk,
			 rst_i => internal_master_reset,
			 d_i => stop_measure_cmd,
			 rise_o => stop_measure_cmd_rising_edge
		);
	
	measure_stop_logic: internal_measure_stop <= '0' when internal_master_reset = '1' else
																'1' when (stop_measure_cmd_rising_edge = '1' or A_limit_reached = '1') and rising_edge(clk) else
																'0' when start_measure_cmd_rising_edge = '1' and rising_edge(clk);
		
	measure_stop <= internal_measure_stop;
	
	--Erzeugung von M_int
	A_limit_reached_rising_edge_detector: rising_edge_detector
		port map (
			 clk_i => clk,
			 rst_i => internal_master_reset,
			 d_i => A_limit_reached,
			 rise_o => A_limit_reached_rising_edge
		);	
	
	M_int <= '0' when internal_master_reset = '1' else
				'1' when A_limit_reached_rising_edge = '1' and internal_ram_read = '0' and rising_edge(clk) else
				'0' when send_cmd_rising_edge = '1' and rising_edge(clk);
											
	
	--Erzeugung von A_rst
	internal_measure_start_rising_edge_detector: rising_edge_detector
		port map (
			 clk_i => clk,
			 rst_i => internal_master_reset,
			 d_i => internal_measure_start,
			 rise_o => internal_measure_start_rising_edge
		);
		
	internal_send_data_rising_edge_detector: rising_edge_detector
		port map (
			 clk_i => clk,
			 rst_i => internal_master_reset,
			 d_i => internal_send_data,
			 rise_o => internal_send_data_rising_edge
		);
			
	A_rst <= (internal_send_data_rising_edge and not internal_measure_start) or internal_measure_start_rising_edge;
	
	ram_read <= internal_ram_read;
	internal_ram_read <= internal_send_data and not internal_measure_start;

end architecture;