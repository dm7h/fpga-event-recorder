library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity de1_freqdiv is
	port (
		clk50_i : in std_ulogic;
		clk50_o : out std_ulogic;
		div_o : out std_ulogic
	);
end entity;

architecture structure of de1_freqdiv is

	component freqdiv
	  generic (
		 cnt_width : natural := 5;  -- Bitbreite Zaehler
		 cnt_max : natural := 20  
		 
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
	
	signal count_o : std_ulogic_vector(3 downto 0);

begin

	clk50_o <= clk50_i;
	
	div : freqdiv
		generic map (
			cnt_width => 4,
			cnt_max => 15
		)
		port map (
		 clk => clk50_i,
		 master_reset => '0',
		 count_enable => '1',
		 count_reset => '0',
		 count_o => count_o,
		 freqdiv_out => OPEN
		);

		div_o <= '1' when count_o(3) = '1' else '0';

end architecture;