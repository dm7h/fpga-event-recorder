-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: mega_control
-- Datum: 05.12.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity mega_control is
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
end entity;

architecture structure of mega_control is

	signal M_data_in, M_data_out : std_ulogic_vector(3 downto 0);
	signal reg_data_valid : std_ulogic;
	signal reg_data : std_ulogic_vector(7 downto 0);
	
	component control_register
	  port (
		 clk : in std_ulogic;
		 master_reset : in std_ulogic;
		 reg_data_valid : in std_ulogic;
		 start_measure_cmd : out std_ulogic;
		 stop_measure_cmd : out std_ulogic;
		 reset_cmd : out std_ulogic;
		 send_cmd : out std_ulogic;
		 error_contr_reg : out std_ulogic;
		 reg_data : in std_ulogic_vector(7 downto 0)
	  );
	end component;
	
	component send_control
		port (
			clk					: in std_ulogic;
			master_reset		: in std_ulogic;
			
			M_clk					: in std_ulogic;
			M_rw					: in std_ulogic;
			M_nib_sel			: in std_ulogic;
			M_reg_sel			: in std_ulogic_vector(1 downto 0);
			
			RAM_data_in			: in std_ulogic_vector(15 downto 0);
			status_register	: in std_ulogic_vector(15 downto 0);
			
			send_data			: in std_ulogic;
			A_end_reached		: in std_ulogic;
			
			M_data_out			: out std_ulogic_vector(3 downto 0);
			
			A_cnt_mega			: out std_ulogic;
			send_complete		: out std_ulogic
		);
	end component;
	
	component receive_control
		port  (
			clk				: in std_ulogic;
			master_reset	: in std_ulogic;
			
			M_data_in		: in std_ulogic_vector (3 downto 0);
			M_clk				: in std_ulogic;
			M_rw				: in std_ulogic;
			M_nib_sel		: in std_ulogic;
			
			reg_data			: out std_ulogic_vector (7 downto 0);
			reg_data_valid	: out std_ulogic
		);
	end component;
	
	component generic_tristate_driver IS
	  GENERIC(
		 width : natural := 4);
	  PORT(
		 bidir_data    : INOUT STD_LOGIC_VECTOR (width-1 DOWNTO 0);
		 dir           : IN STD_ULOGIC;	-- 0: CPLD write; 1: CPLD read
		 data_out    : IN STD_ULOGIC_VECTOR (width-1 DOWNTO 0);
		 data_in     : OUT STD_ULOGIC_VECTOR (width-1 DOWNTO 0));
	END component;
	
	signal M_clk1, M_clk_sync, M_nib_sel1, M_nib_sel_sync, M_rw1, M_rw_sync : std_ulogic; 
	signal M_reg_sel1, M_reg_sel_sync :std_ulogic_vector(1 downto 0);

begin

	M_clk1 <= M_clk when rising_edge(clk);
	M_clk_sync <= M_clk1 when rising_edge(clk);
	
	M_rw1 <= M_rw when rising_edge(clk);
	M_rw_sync <= M_rw1 when rising_edge(clk);	
	
	M_nib_sel1 <= M_nib_sel when rising_edge(clk);
	M_nib_sel_sync <= M_nib_sel1 when rising_edge(clk);
	
	M_reg_sel1 <= M_reg_sel when rising_edge(clk);
	M_reg_sel_sync <= M_reg_sel1 when rising_edge(clk);

	I1: control_register
		port map (
			clk => clk,
			master_reset => master_reset,
			reg_data_valid => reg_data_valid,
			start_measure_cmd => start_measure_cmd,
			stop_measure_cmd => stop_measure_cmd,
			reset_cmd => reset_cmd,
			send_cmd => send_cmd,
			error_contr_reg => error_contr_reg,
			reg_data => reg_data
		 );
	
	I2: send_control
		port map(
			clk => clk,
			master_reset => master_reset,
			M_clk => M_clk_sync,
			M_rw => M_rw_sync,
			M_nib_sel => M_nib_sel_sync,
			M_reg_sel => M_reg_sel_sync,
			RAM_data_in => ram_data_in,
			status_register => status_register,
			send_data => send_data,
			A_end_reached => A_end_reached,
			M_data_out => M_data_out,
			A_cnt_mega => A_cnt_mega,
			send_complete => send_complete
		);
		
	I3: receive_control
		port map (
			clk => clk,
			master_reset => master_reset,
			M_data_in => M_data_in,
			M_clk => M_clk_sync,
			M_rw => M_rw_sync,
			M_nib_sel => M_nib_sel_sync,
			reg_data => reg_data,
			reg_data_valid => reg_data_valid
		);
		
	I4: generic_tristate_driver
		generic map (
			width => 4
			)
		port map (
		 bidir_data => M_data,
		 dir => M_rw,
		 data_out => M_data_out,
		 data_in	=> M_data_in
		);


end architecture;