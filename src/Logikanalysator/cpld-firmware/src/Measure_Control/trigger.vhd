-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- CPLD Komponente: trigger
-- Datum: 08.01.2014
-- Vers.: 1.1
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity trigger is
port (
	clk                 : in std_ulogic;
	master_reset        : in std_ulogic;
	input               : in std_ulogic_vector(7 downto 0);
	freqdiv             : in std_ulogic;
	timestamp_overflow  : in std_ulogic;
	measure_init        : in std_ulogic; -- bei init = '1' wird der aktuelle Wert des Eingangs 
	                                     -- in die flipflops geladen. Damit wird verhindert dass value_changed 
										 -- auslï¿½st falls bereits eine '1' an input anliegt. 
										 -- Achtung: Nach einem master_reset muss ebenfalls init auf '1' gesetzt werden
	trigger_out       : out std_ulogic
	);
end trigger;

-------------------------------------------------------------------------------

architecture rtl of trigger is

signal value_changed, q2 : std_ulogic;
signal q0, q1 : std_ulogic_vector(7 downto 0);

begin

-- ============= input_change_detection ===========================
ff_0: q0 <= "00000000" when master_reset = '1' else input when rising_edge(clk);
ff_1: q1 <= "00000000" when master_reset = '1' else input when measure_init = '1' and rising_edge(clk) 
                     else q0 when rising_edge(clk);

input_change: process (clk) begin
 if rising_edge (clk) then
   value_changed <= '0';
   --if (q0 < q1) or (q0 > q1)then  -- benötigt 16 logikelemente mehr
	if (q0(0) = '1' and q1(0) = '0') or (q0(0) = '0' and q1(0) = '1') or
	   (q0(1) = '1' and q1(1) = '0') or (q0(1) = '0' and q1(1) = '1') or
		(q0(2) = '1' and q1(2) = '0') or (q0(2) = '0' and q1(2) = '1') or
		(q0(3) = '1' and q1(3) = '0') or (q0(3) = '0' and q1(3) = '1') or
		(q0(4) = '1' and q1(4) = '0') or (q0(4) = '0' and q1(4) = '1') or
		(q0(5) = '1' and q1(5) = '0') or (q0(5) = '0' and q1(5) = '1') or
		(q0(6) = '1' and q1(6) = '0') or (q0(6) = '0' and q1(6) = '1') or
		(q0(7) = '1' and q1(7) = '0') or (q0(7) = '0' and q1(7) = '1') then
     value_changed <= '1';
   end if;
  end if;
end process;


-- ===============store value_changed ============================
reg1: process (clk, master_reset)
 begin
 if master_reset = '1' then
   q2 <= '0';
 elsif rising_edge(clk) then
   if value_changed = '1' then
     q2 <= '1';
   elsif freqdiv = '1' then
     q2 <= '0';
   end if;
 end if;
 end process;
 
 -- ===============output logic ====================================
 outlogic: trigger_out <= freqdiv and (q2 or timestamp_overflow);
 


end rtl;