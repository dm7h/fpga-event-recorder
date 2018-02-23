-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: send_control
-- Datum: 15.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity send_control is
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
end send_control;

architecture structure of send_control is

	component send_control_datapath
		port (
			clk					: in std_ulogic;
			reset					: in std_ulogic;
			
			M_A					: in std_ulogic;
			M_B					: in std_ulogic;
			nib_sel_tx			: in std_ulogic;
			nib_sel_valid		: in std_ulogic;
			
			RAM_data_in			: in std_ulogic_vector(15 downto 0);
			status_register	: in std_ulogic_vector(15 downto 0);
			
			M_data_out			: out std_ulogic_vector(3 downto 0)
		);
	end component;
	
	component send_control_controlpath 
		port (
			clk					: in std_ulogic;
			master_reset		: in std_ulogic;
			
			M_clk					: in std_ulogic;
			M_rw					: in std_ulogic;
			M_nib_sel			: in std_ulogic;
			M_reg_sel			: in std_ulogic_vector(1 downto 0);
			
			send_data			: in std_ulogic;
			A_end_reached		: in std_ulogic;
			
			M_A					: out std_ulogic;
			M_B					: out std_ulogic;
			nib_sel_tx			: out std_ulogic;
			nib_sel_valid		: out std_ulogic;
			
			A_cnt_mega			: out std_ulogic;
			send_complete		: out std_ulogic
		);
	end component;
	
	signal M_A : std_ulogic;
	signal M_B : std_ulogic;
	signal nib_sel_tx : std_ulogic;
	signal nib_sel_valid : std_ulogic;

begin

send_control_datapath_i0: send_control_datapath
	port map (
		clk					=> clk,
		reset					=> master_reset,		
		M_A					=> M_A,
		M_B					=> M_B,
		nib_sel_tx			=> nib_sel_tx,
		nib_sel_valid		=> nib_sel_valid,	
		RAM_data_in			=> RAM_data_in,
		status_register	=> status_register,	
		M_data_out			=> M_data_out
	);

send_control_controlpath_i0: send_control_controlpath
	port map (
		clk					=> clk,
		master_reset		=> master_reset,
		M_clk					=> M_clk,
		M_rw					=> M_rw,
		M_nib_sel			=> M_nib_sel,
		M_reg_sel			=> M_reg_sel,
		send_data			=> send_data,
		A_end_reached		=> A_end_reached,
		M_A					=> M_A,
		M_B					=> M_B,
		nib_sel_tx			=> nib_sel_tx,
		nib_sel_valid		=> nib_sel_valid,
		A_cnt_mega			=> A_cnt_mega,
		send_complete		=> send_complete
	);
end architecture;