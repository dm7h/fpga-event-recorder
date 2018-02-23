-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: measure_control_controlpath
-- Datum: 20.12.2013
-- Vers.: 1.1
-- ===========================================================================

library ieee;
use ieee.std_logic_1164.all;

entity measure_control_controlpath is
	port (
		clk	            : in std_ulogic;
		master_reset    : in std_ulogic;
		
		measure_start   : in std_ulogic;
		measure_stop	: in std_ulogic;
		ram_write_ready : in std_ulogic;
		trigger         : in std_ulogic;
		
		measure_complete: out std_ulogic;
		store_data      : out std_ulogic;
		write_end_data  : out std_ulogic;
		measure_init    : out std_ulogic;
		enable_timestamp: out std_ulogic
	);
end measure_control_controlpath;

architecture rtl of measure_control_controlpath is

type state_t is (init, idle, trigger_init, wait_first_trigger, wait_trigger, store,
                 wait_ram_is_running1, wait_ram_is_running2,
				 stop, wait_ram_rdy, complete);
signal current_state, next_state : state_t;
signal transition_vector : std_ulogic_vector(12 downto 0);

begin

--Zustandsuebergangstabelle.
--Fuer jeden Zustandsuebergang im Automaten wird jeweils ein Bit im transition_vector verwendet
transition_vector(0) <= '1';                                                                --st_init_idle
transition_vector(1) <= measure_start and not measure_stop and ram_write_ready;		        --st_idle_waittrigger
transition_vector(2) <= '1';                                                                --st_triggerinit_waitfirsttrigger
transition_vector(3) <= measure_start and not measure_stop and ram_write_ready and trigger; --st_waitfirsttrigger_store
transition_vector(4) <= '1';                                                                --st_store_waitramisrunning1
transition_vector(5) <= measure_start and not measure_stop and not ram_write_ready;			--st_waitramisrunning1_waittrigger
transition_vector(6) <= measure_start and not measure_stop and ram_write_ready and trigger; --st_waittrigger_store
transition_vector(7) <= measure_stop and ram_write_ready;                                   --st_waittrigger_stop
transition_vector(8) <= '1';                                                                --st_stop_waitramisrunning2
transition_vector(9) <= not ram_write_ready;                                                --st_waitramisrunning2_waitramrdy
transition_vector(10) <= ram_write_ready;                                                   --st_waitramrdy_complete
transition_vector(11) <= '1';                                                               --st_complete_idle
transition_vector(12) <= measure_stop;                                                      --st_waitfirsttrigger_complete   

statemachine : process(current_state, transition_vector)
begin
	next_state <= current_state;
	measure_complete <= '0';
	store_data <= '0';
	write_end_data <= '0';
	measure_init <= '0';
	enable_timestamp <= '0';
	case current_state is
	when init =>
		if transition_vector(0) = '1' then next_state <= idle; end if;
	when idle =>
		if transition_vector(1) = '1' then next_state <= trigger_init; end if;
	when trigger_init =>
	    measure_init <= '1';
		if transition_vector(2) = '1' then next_state <= wait_first_trigger; end if; 
  when wait_first_trigger =>
    if transition_vector(3) = '1' then next_state <= store; elsif
       transition_vector(12) = '1' then next_state <= complete; end if; 	
	when wait_trigger =>
	    enable_timestamp <= '1';
		if transition_vector(6) = '1' then next_state <= store; elsif
			transition_vector(7) = '1' then next_state <= stop; end if;
	when store =>
		store_data <= '1';
		enable_timestamp <= '1';
		if transition_vector(4) = '1' then next_state <= wait_ram_is_running1; end if;			
	when wait_ram_is_running1 =>
	    enable_timestamp <= '1';
		if transition_vector(5) = '1' then next_state <= wait_trigger; end if;
	when stop =>
		store_data <= '1';
		write_end_data <= '1';
		enable_timestamp <= '1';
		if transition_vector(8) = '1' then next_state <= wait_ram_is_running2; end if;
	when wait_ram_is_running2 =>
		write_end_data <= '1';
		if transition_vector(9) = '1' then next_state <= wait_ram_rdy; end if;
	when wait_ram_rdy =>
		write_end_data <= '1';
		if transition_vector(10) = '1' then next_state <= complete; end if;
	when complete =>
		measure_complete <= '1';
		if transition_vector(11) = '1' then next_state <= idle; end if;
		
	when others => next_state <= init;			
	end case;

end process statemachine;

current_state <= init when master_reset = '1' else next_state when rising_edge(clk);

end architecture;
