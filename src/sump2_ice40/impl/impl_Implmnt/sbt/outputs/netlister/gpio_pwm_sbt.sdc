create_clock -period 1000.00 -name {gpio_pwm|clk} -waveform [list 0.00 500.00] [get_ports clk]
