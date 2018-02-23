library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity de1_funcgen is
	port (
		clk50_i : in std_ulogic;
		button : in std_ulogic_vector(3 downto 0);
		switch : in std_ulogic_vector(9 downto 0);
		
		ledg : out std_ulogic_vector(7 downto 0);
		ledr : out std_ulogic_vector(9 downto 0);
		hex3 : out std_ulogic_vector(6 downto 0);
		hex0 : out std_ulogic_vector(6 downto 0);
		data_out : out std_ulogic_vector(7 downto 0);
		clk50_o : out std_ulogic
	);
end entity;

architecture structure of de1_funcgen is

	component freqdiv
	  generic (
		 cnt_width : natural := 4;  -- Bitbreite Zaehler
		 cnt_car : natural := 42;
		 cnt_max : natural := 15  
	  );
	  port (
		 clk : in std_ulogic;
		 master_reset       : in std_ulogic;
		 count_enable       : in std_ulogic;
		 count_reset        : in std_ulogic;
		 count_o            : out std_ulogic_vector(cnt_width-1 downto 0);
		 freqdiv_out        : out std_ulogic
		
	  );
	end component;
	
	component cntup_modm
	  generic (
		 cnt_width   : natural := 18;
		 cnt_max : natural := 262143
	  );
	  port (
		 clk            : in std_ulogic;
		 master_reset   : in std_ulogic;
		 count_enable   : in std_ulogic;
		 count_reset    : in std_ulogic;
		 count_o        : out std_ulogic_vector(cnt_width-1 downto 0);
		 carry_o        : out std_ulogic
	  );
	end component;
	
	component rising_edge_detector
	  port (
		 clk_i : in std_ulogic;
		 rst_i : in std_ulogic;
		 d_i : in std_ulogic;
		 rise_o : out std_ulogic
	  );
	end component;
	
	component tple_wave
	  PORT (clk: IN std_ulogic;
			  rst_p: IN std_ulogic;
			  start_i: IN std_ulogic;
			  out1: OUT std_ulogic;
			  out2: OUT std_ulogic;
			  out3: OUT std_ulogic;
			  out4: OUT std_ulogic;
			  out5: OUT std_ulogic;
			  trigger: OUT std_ulogic);
	END component;
	
	signal sys_clk, sys_clk_fast, sys_clk_slow, master_reset : std_ulogic;
	signal countup_enable, countup_car, start_button_rising_edge, tplewave_enable, tplewave_trigger_o : std_ulogic;
	signal modesel_i : std_ulogic_vector(2 downto 0);
	signal button_i : std_ulogic_vector(3 downto 0);
	signal internal_data_out, countup_data_out, tplewave_data_out : std_ulogic_vector(7 downto 0);
	signal switch1, switch_s : std_ulogic_vector(9 downto 0);
	signal button1, button_s : std_ulogic_vector(3 downto 0);

begin
	clk50_o <= clk50_i;
	master_reset <= '0';
	modesel_i <= switch_s(9 downto 7);
	data_out <= internal_data_out;
	button_i <= not button;
	ledr(7 downto 0) <= internal_data_out;
	ledg <= switch_s(3) & button_s(3) & switch_s(2) & button_s(2) & switch_s(1) & button_s(1) & switch_s(0) & button_s(0);
	
	-- ==================================
	-- Input sync
	-- ==================================
	
	switch1 <= switch when rising_edge(sys_clk);
	switch_s <= switch1 when rising_edge(sys_clk);
	
	button1 <= button_i when rising_edge(sys_clk);
	button_s <= button1 when rising_edge(sys_clk);

	-- ==================================
	-- sysclock
	-- ==================================
	
	sysclock_gen_fast : freqdiv
		generic map(
			cnt_width => 4,
			cnt_car => 8,
			cnt_max => 15
		)
		port map (
		 clk => clk50_i,
		 master_reset => master_reset,
		 count_enable => '1',
		 count_reset => '0',
		 count_o => OPEN,
		 freqdiv_out => sys_clk_fast
		);
		
	sysclock_gen_slow : freqdiv
		generic map(
			cnt_width => 22,
			cnt_car => 2097151,
			cnt_max => 4194303
		)
		port map (
		 clk => clk50_i,
		 master_reset => master_reset,
		 count_enable => '1',
		 count_reset => '0',
		 count_o => OPEN,
		 freqdiv_out => sys_clk_slow
		);
		
	sysclock_mux : process(switch_s(6), sys_clk_fast, sys_clk_slow)
	begin
		case switch_s(6) is
			when '0' =>
				sys_clk <= sys_clk_fast;
				hex0 <= "0001110";
			when '1' =>
				sys_clk <= sys_clk_slow;
				hex0 <= "0010010";
			when others =>
				sys_clk <= '0';
				hex0 <= "0000110";
		end case;
	end process;
	
	-- ==================================
	-- start button rising edge
	-- ==================================
	
	start_rising_edge_detector: rising_edge_detector
		port map(
		 clk_i => sys_clk,
		 rst_i => master_reset,
		 d_i => button_s(0),
		 rise_o => start_button_rising_edge
		);
		
	-- ==================================
	-- countup
	-- ==================================
	
	countup : cntup_modm
		generic map (
		 cnt_width => 8,
		 cnt_max => 255	
		)
		port map (
		 clk => sys_clk, 
		 master_reset => master_reset,
		 count_enable => countup_enable,
		 count_reset => '0',
		 count_o => countup_data_out,
		 carry_o => countup_car
		);
		
	countup_enable_generator: countup_enable <= 	'1' when start_button_rising_edge = '1' or switch_s(0) = '1' else
																'0' when countup_car = '1';


	-- ==================================
	-- tple_wave
	-- ==================================
	
	tple_machine : tple_wave
	  port map (
		clk => sys_clk,
		rst_p => master_reset,
		start_i => tplewave_enable,
		out1 => tplewave_data_out(2),
		out2 => tplewave_data_out(3),
		out3 => tplewave_data_out(4),
		out4 => tplewave_data_out(5),
		out5 => tplewave_data_out(6),
		trigger => tplewave_trigger_o
	  );
	
	tplewave_data_out(0) <= tplewave_trigger_o and sys_clk;
	tplewave_data_out(1) <= '0';
	tplewave_data_out(7) <= tplewave_trigger_o;
	
	tplewave_enable_generator : tplewave_enable <= '1' when start_button_rising_edge = '1' or switch_s(0) = '1' else '0';
																
	-- ==================================
	-- Mode MUX
	-- ==================================
	
	modesel_mux: process(modesel_i, button_s, switch_s)
	begin
	
		case modesel_i is
			when "000" =>		-- sysclock
				hex3 <= "1000000";
				internal_data_out <= "0000000" & sys_clk;
			when "001" =>		-- countup
				hex3 <= "1111001";
				internal_data_out <= countup_data_out;
			when "010" =>		-- tple wave
				hex3 <= "0100100";
				internal_data_out <= tplewave_data_out;
			when "011" =>		-- userbutton
				hex3 <= "0110000";
				internal_data_out <= "0000" & (button_s or switch_s(3 downto 0));
			when others =>
				hex3 <= "0000110";
				internal_data_out <= "00000000";	
		end case;
	end process;
		
	--data_out(0) <= sys_clk;

end architecture;