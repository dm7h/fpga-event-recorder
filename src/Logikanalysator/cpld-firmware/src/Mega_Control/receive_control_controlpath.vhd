-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: receive_control_controlpath
-- Datum: 22.11.2013
-- Vers.: 1.0
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity receive_control_controlpath is
	port (
		clk				: in std_ulogic;
		master_reset	: in std_ulogic;
		
		M_clk				: in std_ulogic;
		M_rw				: in std_ulogic;
		M_nib_sel		: in std_ulogic;
		
		nib_sel_valid	: out std_ulogic;
		reg_data_valid	: out std_ulogic
	);
end receive_control_controlpath;

architecture rtl of receive_control_controlpath is

type state_t is (init, idle, wait_rising_edge, nib_valid, wait_falling_edge, receive_compl);
signal current_state, next_state : state_t;
signal transition_vector : std_ulogic_vector(7 downto 0);

begin

--Zustandsübergangstabelle.
--Für jeden Zustandsübergang im Automaten wird jeweils ein Bit im transition_vector verwendet
transition_vector(0) <= '1';							--st_init_idle
transition_vector(1) <= not M_clk and M_rw;		--st_idle_waitRisingEdge
transition_vector(2) <= M_clk and M_rw;			--st_waitRisingEdge_nibValid
transition_vector(3) <= M_rw and not M_nib_sel;	--st_nibValid_waitFallingEdge
transition_vector(4) <= not M_clk and M_rw;		--st_waitFallingEdge_waitRisingEdge
transition_vector(5) <= M_rw and M_nib_sel;		--st_nibValid_receiveCompl
transition_vector(6) <= '1';							--st_sendCompl_idle
transition_vector(7) <= not M_rw;					--st_to_idle

statemachine : process(current_state, transition_vector)
begin
	next_state <= current_state;
	nib_sel_valid <= '0';
	reg_data_valid <= '0';
	
	case current_state is
	when init =>
		if transition_vector(0) = '1' then next_state <= idle; end if;
	when idle =>
		if transition_vector(1) = '1' then next_state <= wait_rising_edge; end if;
	when wait_rising_edge =>
		if transition_vector(7) = '1' then next_state <= idle; elsif
			transition_vector(2) = '1' then next_state <= nib_valid; end if;
	when nib_valid =>
		nib_sel_valid <= '1';
		if transition_vector(7) = '1' then next_state <= idle; elsif
			transition_vector(5) = '1' then next_state <= receive_compl; elsif
			transition_vector(3) = '1' then next_state <= wait_falling_edge; end if;
	when wait_falling_edge =>
		if transition_vector(7) = '1' then next_state <= idle; elsif
			transition_vector(4) = '1' then next_state <= wait_rising_edge; end if;
	when receive_compl =>
		reg_data_valid <= '1';
		if transition_vector(6) = '1' then next_state <= idle; end if;
		
	when others => next_state <= init;			
	end case;

end process statemachine;

current_state <= init when master_reset = '1' else next_state when rising_edge(clk);

end architecture;
