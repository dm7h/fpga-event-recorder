-- Projekt Logikanalysator an der FH-Augsburg
-- 5. Semester, Technische Informatik,  WS2013/2014
-- A. Gareis, S. Vockinger
-- Testbench für CPLD Komponente: send_control_controlpath
-- Datum: 19.11.2013
-- Vers.: 1.0
-- ===========================================================================


library ieee;
use ieee.std_logic_1164.all;

-------------------------------------------------------------------------------

entity tb_send_control_controlpath is
end tb_send_control_controlpath;

-------------------------------------------------------------------------------

architecture tbench of tb_send_control_controlpath is

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

  -- definition of a clock period
  constant period : time := 10 ns;

  -- definition of M_clk period
  constant M_clk_period : integer := 5;
  
  -- component ports
		signal clk					:  std_ulogic;
		signal master_reset		:  std_ulogic;
		
		signal M_clk					:  std_ulogic;
		signal M_rw					:  std_ulogic;
		signal M_nib_sel			:  std_ulogic;
		signal M_reg_sel			:  std_ulogic_vector(1 downto 0);

		signal send_data			:  std_ulogic;
		signal A_end_reached		:  std_ulogic;
		
		signal M_A					:  std_ulogic;
		signal M_B					:  std_ulogic;
		signal nib_sel_tx			:  std_ulogic;
		signal nib_sel_valid		:  std_ulogic;
		
		signal A_cnt_mega			:  std_ulogic;
		signal send_complete		:  std_ulogic;
		
		signal clken_p  : boolean := true;
      
begin  -- tbench

  -- component instantiation
  DUT : entity work.send_control_controlpath(rtl)
    port map (
		clk				=> clk,
		master_reset	=> master_reset,
		
		M_clk			=> M_clk,
		M_rw			=> M_rw,
		M_nib_sel		=> M_nib_sel,
		M_reg_sel		=> M_reg_sel,

		send_data		=> send_data,
		A_end_reached 	=> A_end_reached,
		
		M_A				=> M_A,
		M_B				=> M_B,
		nib_sel_tx		=> nib_sel_tx,
		nib_sel_valid	=> nib_sel_valid,
		
		A_cnt_mega		=> A_cnt_mega,
		send_complete	=> send_complete	
	);
      
	  
    -- clock generation
  clock_proc : process
  begin
    while clken_p loop
      clk <= '0'; wait for period/2;
      clk <= '1'; wait for period/2;
    end loop;
    wait;
  end process;

  
  
  stimuli_p : process
  begin
    -- init
  master_reset <= '0';  
  M_clk <= '0';
  M_rw  <= '1';
  M_nib_sel <= '0';
  M_reg_sel <= "00";
  send_data <= '0';
  A_end_reached <= '0';                                     
  wait for M_clk_period * period; 
                       
    -- =========== one send iteration of 16bit (RAM) ============
    -- idle	
  
 	send_data <= '1';
	M_rw <= '0';  
  wait for M_clk_period * period;   
  M_clk <= '1';
	
	-- nib_valid nibbel 0
	
  -- wait1
    wait for M_clk_period * period;
	M_clk <= '0';
	M_nib_sel <= '1';
	--wait2
	wait for M_clk_period * period;
	M_clk <= '1';
	
	-- nib_valid nibbel 1
	
  -- wait1
  wait for M_clk_period * period;
	M_clk <= '0';
	M_nib_sel <= '0';
	M_reg_sel <= "01";
	--wait2
	wait for M_clk_period * period;
	M_clk <= '1';
	
	-- nib_valid nibbel 2
	
    -- wait1
   wait for M_clk_period * period;
	M_clk <= '0';
	M_nib_sel <= '1';
	M_reg_sel <= "01";
	--wait2
	wait for M_clk_period * period;
	M_clk <= '1';
	
	-- nib_valid nibbel 3
	
	-- inc_A_cnt
	
	-- check_A_cnt_end
	A_end_reached <= '1';
	
	-- send_compl
	
	--wait0 
	wait for M_clk_period * period;
	M_clk <= '0';
    --=========================================================  
    wait;
  end process;

end tbench;
