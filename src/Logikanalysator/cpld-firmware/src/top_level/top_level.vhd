-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: top_level
-- Datum: 06.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity top_level is
	port (
		clk : in std_ulogic;
		ext_reset : in std_ulogic;
		
		-- mega Interface
		M_clk	: in std_ulogic;
		M_rw : in std_ulogic;
		M_nib_sel : in std_ulogic;
		M_reg_sel : in std_ulogic_vector(1 downto 0);
		M_data : inout std_logic_vector(3 downto 0);
		M_int : out std_ulogic;
		
		--RAM Interface
		ram_data : inout std_logic_vector(15 downto 0);
		ram_adr : out std_ulogic_vector(17 downto 0);
		lb : out std_ulogic;
		ub : out std_ulogic;
		we : out std_ulogic;
		ce1 : out std_ulogic;
		ce2 : out std_ulogic;
		oe : out std_ulogic;
		
		-- Measure Interface
		io_dir : out std_ulogic;
		io_data : in std_ulogic_vector(7 downto 0)	--TODO: Change to : inout std_logic_vector(7 downto 0)
	);
end entity;

architecture structure of top_level is

	component mega_control
		port (
			clk : in std_ulogic;
			master_reset : in std_ulogic;
			
			M_clk	: in std_ulogic;
			M_rw : in std_ulogic;
			M_nib_sel : in std_ulogic;
			M_reg_sel : in std_ulogic_vector(1 downto 0);
			
			M_data : inout std_logic_vector(3 downto 0);

			ram_data_in : in std_ulogic_vector(15 downto 0);
			status_register : in std_ulogic_vector(15 downto 0);
			
			send_data : in std_ulogic;
			A_end_reached : in std_ulogic;
			
			start_measure_cmd : out std_ulogic;
			stop_measure_cmd : out std_ulogic;
			reset_cmd : out std_ulogic;
			send_cmd : out std_ulogic;
			error_contr_reg : out std_ulogic;
			
			A_cnt_mega : out std_ulogic;
			send_complete : out std_ulogic
			);
	end component;
	
	component manager
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
	end component;
	
	component ram_control
		port (
			clk : in std_ulogic;
			master_reset : in std_ulogic;
			
			--RAM Interface
			ram_data : inout std_logic_vector(15 downto 0);
			ram_adr : out std_ulogic_vector(17 downto 0);
			lb : out std_ulogic;
			ub : out std_ulogic;
			we : out std_ulogic;
			ce1 : out std_ulogic;
			ce2 : out std_ulogic;
			oe : out std_ulogic;
			
			--write_control Interface
			store_data : in std_ulogic;
			ram_write_ready : out std_ulogic;
			
			ram_read  : in std_ulogic;
			
			--Acnt Interface
			A_cnt_mega : in std_ulogic;
			A_rst : in std_ulogic;
			limit_reached : out std_ulogic;
			
			--Data Interface
			ram_write_data : in std_ulogic_vector(31 downto 0);
			ram_data_in : out std_ulogic_vector(15 downto 0)
		);
	end component;
	
	component status_register
		port (
			clk : in std_ulogic;
			master_reset : in std_ulogic;
			
			measure_start : in std_ulogic;
			
			status_register_data : out std_ulogic_vector(15 downto 0)
		);
	end component;
	
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
	
	signal master_reset : std_ulogic;
	signal start_measure_cmd, stop_measure_cmd, reset_cmd, send_cmd, error_contr_reg : std_ulogic;
	signal send_data, A_end_reached, A_limit_reached : std_ulogic;
	signal A_cnt_mega, send_complete : std_ulogic;
	signal ram_data_in, status_register_data : std_ulogic_vector(15 downto 0);
	signal ram_write_data : std_ulogic_vector(31 downto 0);
	
	signal A_rst, A_cnt : std_ulogic;
	signal measure_start, measure_stop, measure_complete : std_ulogic;
	
	signal store_data, ram_read, ram_write_ready : std_ulogic;
	
begin

	--Set Direction of IO-Pins
	io_dir <= '1'; -- Set Direction A ==> B in Hardware Version 1.1

	I1: mega_control
		port map (
			clk => clk,
			master_reset => master_reset,
			M_clk	=> M_clk,
			M_rw => M_rw,
			M_nib_sel => M_nib_sel,
			M_reg_sel => M_reg_sel,
			M_data => M_data,
			ram_data_in => ram_data_in,
			status_register => status_register_data,
			send_data => send_data,
			A_end_reached => '0',   --TODO: Bessere Lösung finden
			start_measure_cmd => start_measure_cmd,
			stop_measure_cmd => stop_measure_cmd,
			reset_cmd => reset_cmd,
			send_cmd => send_cmd,
			error_contr_reg => error_contr_reg,
			A_cnt_mega => A_cnt_mega,
			send_complete => send_complete
		);
		
	I2: manager
		port map (
			clk => clk,
			ext_reset => ext_reset,
			start_measure_cmd => start_measure_cmd,
			stop_measure_cmd => stop_measure_cmd,
			reset_cmd => reset_cmd,
			send_cmd => send_cmd,
			error_contr_reg => error_contr_reg,
			send_complete => send_complete,
			measure_complete => measure_complete,
			A_limit_reached => A_limit_reached,
			master_reset => master_reset,
			A_rst => A_rst,
			send_data => send_data,
			measure_start => measure_start,
			measure_stop => measure_stop,
			ram_read => ram_read,
			M_int => M_int
		);
	
	I3: ram_control
		port map (
			clk => clk,
			master_reset => master_reset,
			ram_data => ram_data,
			ram_adr => ram_adr,
			lb => lb,
			ub => ub,
			we => we,
			ce1 => ce1,
			ce2 => ce2,
			oe => oe,
			store_data => store_data,
			ram_write_ready => ram_write_ready,
			ram_read  => ram_read,
			A_cnt_mega => A_cnt_mega,
			A_rst => A_rst,
			limit_reached => A_limit_reached,
			ram_write_data => ram_write_data,
			ram_data_in => ram_data_in
		);
		
	I4: status_register
		port map (
			clk => clk,
			master_reset => master_reset,
			measure_start => measure_start,
			status_register_data => status_register_data
		);
		
	I5: measure_control
		port map (
			clk => clk,
			master_reset => master_reset,
			inp_vec => io_data,
			measure_start => measure_start,
			measure_stop => measure_stop,
			ram_write_ready => ram_write_ready,
			store_data => store_data,
			measured_data32 => ram_write_data,
			measure_complete => measure_complete
		);

end architecture;