-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: send_control_controlpath
-- Datum: 17.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity send_control_controlpath is
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
end send_control_controlpath;

architecture rtl of send_control_controlpath is

type state_t is (init, wait_rising_edge, idle, nib_valid, wait_falling_edge, inc_A_cnt, check_A_cnt_end, send_compl);
signal current_state, next_state : state_t;
signal transition_vector : std_ulogic_vector(11 downto 0);

begin				

transition_vector(0) <= '1';																									--st_init_idle
transition_vector(1) <= not M_clk and not M_rw and send_data;														--st_idle_waitRisingEdge
transition_vector(2) <= M_clk and not M_rw and send_data;															--st_waitRisingEdge_nibValid
transition_vector(3) <= not M_rw and not (M_reg_sel(0) and M_nib_sel) and send_data;						--st_nibValid_waitFallingEdge
transition_vector(4) <= not M_clk and not M_rw and send_data;														--st_waitFallingEdge_waitRisingEdge
transition_vector(5) <= not M_rw and M_reg_sel(1) and M_reg_sel(0) and M_nib_sel and send_data; 		--st_nibValid_sendCompl
transition_vector(6) <= not M_rw and not M_reg_sel(1) and M_reg_sel(0) and M_nib_sel and send_data;	--st_nibValid_incAcnt
transition_vector(7) <= not M_rw and send_data;																			--st_incAcnt_checkAcntEnd
transition_vector(8) <= not M_rw and send_data and not A_end_reached;											--st_checkAcntEnd_waitFallingEdge
transition_vector(9) <= not M_rw and send_data and A_end_reached;													--st_checkAcntEnd_sendCompl
transition_vector(10) <= '1';																									--st_sendCompl_idle
transition_vector(11) <= not send_data or M_rw;																			--to_idle

M_A <= M_reg_sel(1);
M_B <= M_reg_sel(0);
nib_sel_tx <= M_nib_sel;

statemachine : process(current_state, transition_vector)
begin
	--Default-Werte (Ausgang)
	next_state <= current_state;
	nib_sel_valid <= '0';
	A_cnt_mega <= '0';
	send_complete <= '0';
	
	case current_state is
	when init =>
		if transition_vector(0) = '1' then next_state <= idle; end if;
	when idle =>
		if transition_vector(1) = '1' then next_state <= wait_rising_edge; end if;
	when wait_rising_edge =>
		if transition_vector(11) = '1' then next_state <= idle; elsif
			transition_vector(2) = '1' then next_state <= nib_valid; end if;
	when nib_valid =>
		nib_sel_valid <= '1';
		if transition_vector(11) = '1' then next_state <= idle; elsif
			transition_vector(5) = '1' then next_state <= send_compl; elsif
			transition_vector(6) = '1' then next_state <= inc_A_cnt; elsif
			transition_vector(3) = '1' then next_state <= wait_falling_edge; end if;
	when wait_falling_edge =>
		if transition_vector(11) = '1' then next_state <= idle; elsif
			transition_vector(4) = '1' then next_state <= wait_rising_edge; end if;
	when inc_A_cnt =>
		A_cnt_mega <= '1';
		if transition_vector(11) = '1' then next_state <= idle; elsif
			transition_vector(7) = '1' then next_state <= check_A_cnt_end; end if;		--hier vielleicht ohne bedingung (nur else)???
	when check_A_cnt_end =>
		if transition_vector(11) = '1' then next_state <= idle; elsif
			transition_vector(8) = '1' then next_state <= wait_falling_edge; elsif
			transition_vector(9) = '1' then next_state <= send_compl; end if;
	when send_compl =>
		send_complete <= '1';
		if transition_vector(10) = '1' then next_state <= idle; end if;
		
	when others => next_state <= init;		
	end case;
	
end process statemachine;

current_state <= init when master_reset = '1' else next_state when rising_edge(clk);

end architecture;