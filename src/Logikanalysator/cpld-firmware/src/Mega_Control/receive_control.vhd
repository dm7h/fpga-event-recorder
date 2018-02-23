-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: receive_control
-- Datum: 22.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity receive_control is
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
end receive_control;

architecture stucture of receive_control is

component receive_control_datapath
	port (
		clk				: in std_ulogic;
		reset				: in std_ulogic;
	
		M_data_in		: in std_ulogic_vector (3 downto 0);
		nib_sel_rx		: in std_ulogic;
		nib_sel_valid	: in std_ulogic;
		
		reg_data			: out std_ulogic_vector (7 downto 0)
	);
end component;

component receive_control_controlpath
	port (
		clk				: in std_ulogic;
		master_reset	: in std_ulogic;
		
		M_clk				: in std_ulogic;
		M_rw				: in std_ulogic;
		M_nib_sel		: in std_ulogic;
		
		nib_sel_valid	: out std_ulogic;
		reg_data_valid	: out std_ulogic
	);
end component;

signal nib_sel_valid	: std_ulogic;

begin

receive_control_datapath_i0: receive_control_datapath
port map (
	clk				=> clk,
	reset				=> master_reset,
	M_data_in		=> M_data_in,
	nib_sel_rx		=> M_nib_sel,
	nib_sel_valid	=>	nib_sel_valid,
	reg_data			=> reg_data
	);

receive_control_controlpath_i0: receive_control_controlpath
port map (
	clk				=> clk,
	master_reset	=> master_reset,
	M_clk				=> M_clk,
	M_rw				=> M_rw,
	M_nib_sel		=> M_nib_sel,
	nib_sel_valid	=> nib_sel_valid,
	reg_data_valid	=> reg_data_valid
	);
	
end architecture;
