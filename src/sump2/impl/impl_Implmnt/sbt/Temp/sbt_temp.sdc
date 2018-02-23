####---- CreateClock list ----3
create_clock  -period 10.00 -waveform {0.00 5.00} -name {clk_100m_tree} [get_pins {u0_sb_gb/GLOBALBUFFEROUTPUT}] 
create_clock  -period 10.00 -waveform {0.00 5.00} -name {clk_100m} [get_pins {clk_100m_ibuf/DIN0}] 
create_clock  -period 30.00 -waveform {0.00 15.00} -name {pi_spi_sck} [get_pins {pi_spi_sck_ibuf_gb_io/GLOBALBUFFEROUTPUT}] 

