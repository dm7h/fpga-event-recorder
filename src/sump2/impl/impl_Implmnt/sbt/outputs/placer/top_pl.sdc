create_clock -period 10.00 -name {clk_100m_tree} -waveform [list 0.00 5.00] [get_pins u0_sb_gb/GLOBALBUFFEROUTPUT]
create_clock -period 10.00 -name {clk_100m} -waveform [list 0.00 5.00] [get_pins clk_100m_ibuf/DIN0]
create_clock -period 30.00 -name {pi_spi_sck} -waveform [list 0.00 15.00] [get_pins pi_spi_sck_ibuf_gb_io_gb/GLOBALBUFFEROUTPUT]
