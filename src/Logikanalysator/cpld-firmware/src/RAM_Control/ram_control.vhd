-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: ram_control
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity ram_control is
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
end ram_control;

architecture structure of ram_control is

	component cntup_n_maxm_no_overflow_limit
	  generic (
		 cnt_width : natural := 18; -- Bitbreite Zaehler
		 cnt_max : natural := 262135; -- maximaler wert; bei erreichen wird nicht weiter gezaehlt, um overflow zu vermeiden
		 cnt_limit : natural := 262131 -- bei erreichen dieses Wertes wird limit_reached aktiv
	  );
	  port (
		 clk : in std_ulogic;
		 master_reset : in std_ulogic;
		 count_enable : in std_ulogic;
		 count_reset : in std_ulogic;
		 count_o : out std_ulogic_vector(cnt_width-1 downto 0);
		 max_reached_o : out std_ulogic;
		 limit_reached_o : out std_ulogic
	  );
	end component;
	
	component ram_write_control
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
	
	
	signal A_cnt, A_cnt_msb, A_cnt_ram : std_ulogic;
	signal write_enable, ram_rw : std_ulogic;
	signal read_enable, ce_enable : std_ulogic;
	signal A_count_o : std_ulogic_vector(18 downto 0);
	signal ram_data_out : std_ulogic_vector(15 downto 0);

begin

	A_counter : cntup_n_maxm_no_overflow_limit
	  generic map (
		 cnt_width => 19,  -- Bitbreite Zaehler
		 cnt_max => 524287,  -- maximaler wert; bei erreichen wird nicht weiter gezaehlt, um overflow zu vermeiden
		 cnt_limit => 524285  -- bei erreichen dieses Wertes wird limit_reached aktiv
	  )
	  port map (
		 clk => clk,
		 master_reset => master_reset,
		 count_enable => A_cnt,
		 count_reset => A_rst,
		 count_o => A_count_o,
		 max_reached_o => OPEN,
		 limit_reached_o => limit_reached
	  );
	 
	write_control : ram_write_control
	  generic map (
		 cnt_delay_width => 1,
		 cnt_delay_count => 1
	  )
	  port map ( 
		 clk => clk,
		 master_reset => master_reset,
		 ram_write_data => ram_write_data,
		 ram_data_out => ram_data_out,
		 store_data => store_data,
		 ram_write_ready => ram_write_ready,
		 ram_rw => ram_rw,
		 write_enable => write_enable,
		 A_cnt_ram => A_cnt_ram,
		 we => we
	  );
	
	ram_tristate_driver : generic_tristate_driver
	  generic map ( 
		 width => 16
		)
	  port map (
		 bidir_data => ram_data,
		 dir => ram_rw,
		 data_out => ram_data_out,
		 data_in => ram_data_in
		);

	A_cnt <= A_cnt_ram when write_enable = '1' else A_cnt_mega;
	ram_adr <= A_count_o(17 downto 0);
	A_cnt_msb <= A_count_o(18);
	
	read_enable <= ram_read and not write_enable;
	oe <= not read_enable;
	
	ce_enable <= read_enable or write_enable;
	ce1 <= not (ce_enable and not A_cnt_msb);
	ce2 <= not (ce_enable and A_cnt_msb);
	
	lb <= '0';
	ub <= '0';

end architecture;
