create_clock -period 10.000 -name clk_100m      [get_nets clk_100m]
create_clock -period 10.000 -name clk_100m_tree [get_nets clk_100m_tree]
create_clock -period 30.000 -name pi_spi_sck    [get_nets pi_spi_sck]
