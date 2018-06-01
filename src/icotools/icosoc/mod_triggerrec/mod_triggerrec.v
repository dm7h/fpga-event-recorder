module icosoc_mod_triggerrec #(
	parameter integer CLOCK_FREQ_HZ = 0, // unused
	parameter integer MAX_EVENTS = 16,
	parameter integer IO_LENGTH = 16
) (
	input clk,
	input resetn,

	// bus
	input [3:0] ctrl_wr,
	input ctrl_rd,
	input [15:0] ctrl_addr,
	input [31:0] ctrl_wdat,
	output reg [31:0] ctrl_rdat,
	output reg ctrl_done,

	// gpio pins
	input [IO_LENGTH-1:0] IO
);
	// gpio inputs
	wire [IO_LENGTH-1:0] io_in;
	reg [IO_LENGTH-1:0] io_in_buf;
	
	// state register for switching up/lower half of 
	reg ctrl_state;

	// status register (running, stopped etc.)
	reg status;

	// register array holding the event trigger configurations
	reg [31:0] triggers [0:MAX_EVENTS-1];
	integer i, ii, iii;

	reg [63:0] counter;
	reg [31:0] counter_buf;

	// registers for the event fifo
	reg [63:0] data_in, data_out;
	wire shift_in, pop_out;
	wire nempty_in, nempty_out;
	wire full_in, full_out;


	SB_IO #(
		.PIN_TYPE(6'b 0000_01),
		.PULLUP(1'b 0)
	) ios [IO_LENGTH-1:0] (
		.PACKAGE_PIN(IO),
		.D_IN_0(io_in)
	);

	always @(posedge clk) begin
		
		// reset
		if (!resetn) begin
		       counter <= 0;
		       status <= 0;
		       ctrl_state <= 0;
		       // initialize trigger array with 0xFF's
		       for (i = 0; i < (MAX_EVENTS - 1) << 1; i = i + 1)
			       triggers[i] <= ~32'h0;
	       	end else;	
		
		
		
		// bus communication
		ctrl_rdat <= 'bx;
		ctrl_done <= 0;
		pop_out <= 0;
		shift_in <= 0;

		if (!ctrl_done) begin
			// write from bus to local registers
			if (|ctrl_wr) begin
				ctrl_done <= 1;
				
				// write status register
				if (ctrl_addr == 4)
					status <= ctrl_wdat;

				// (re)set counter
				if (ctrl_addr == 8) begin
					if (ctrl_state == 0) 
						counter[63:32] <= ctrl_wdat;
					else
						counter[31:0] <= ctrl_wdat;
					ctrl_state <= ~ctrl_state;
				end
				
				// write triggers  
				if (ctrl_addr >= 'h100) begin
					triggers[(ctrl_addr[4:0] >> 1) | ctrl_state ] <= ctrl_wdat;
					ctrl_state <= ~ctrl_state;
				end
				
				// write fifo  
				if (ctrl_addr == 'h12) begin
					data_in <= 64'h0102030405060708;
					shift_in <= 1;
					/*
					if (ctrl_state == 0 ) begin
						data_in[63:32] <= ctrl_wdat;
					else begin
						data_in[31:0] <= ctrl_wdat;
						shift_in <= 1;
					end
					*/
					ctrl_state <= ~ctrl_state;
				end
							
			end
			
			// read from local registers to bus
			if (ctrl_rd) begin
				ctrl_done <= 1;
				
				// read gpio input state
				if (ctrl_addr == 0) ctrl_rdat <= io_in;
				
				// read status register
				if (ctrl_addr == 4) ctrl_rdat <= status;

				// read counter
				if (ctrl_addr == 8) begin
					ctrl_rdat <= (ctrl_state)? counter[31:0] : counter[63:32];
					ctrl_state <= ~ctrl_state;
				end

				// read triggers
				if (ctrl_addr >= 'h100) begin
					ctrl_rdat <= triggers[(ctrl_addr[4:0] >> 1) | ctrl_state];
					ctrl_state <= ~ctrl_state;
				end
				
				// read fifo
				if (ctrl_addr >= 'h12) begin
					ctrl_rdat <= (ctrl_state)? data_out[63:32] : data_out[31:0];
					pop_out <= 1;
					ctrl_state <= ~ctrl_state;
				end
			end
		end
		
		
		// trigger logic
		
		// stop counter when paused
		if (!status[0]) 
			counter <= counter + 1;

		// trigger on all input changes
		if (io_in != io_in_buf) begin
			if (ctrl_state == 0) begin
				// save counter for second "ctrl_state"
			       	counter_buf <= counter[31:0];
		       		data_in[63:32] <= { io_in, counter[47:32] };
			end else begin
       				data_in[31:0] <= counter_buf;
				shift_in <= 1;
			end
			ctrl_state = ~ctrl_state;	
		end	
		io_in_buf <= io_in;
	end

	// fifo
	icosoc_crossclkfifo #(
		.WIDTH(64),
		.DEPTH(128)
	) events_fifo (
		.in_clk(clk),
		.in_shift(shift_in),
		.in_data(data_in),
		.in_nempty(nempty_in),

		.out_clk(clk),
		.out_pop(pop_out),
		.out_data(data_out),
		.out_nempty(nempty_out)
	);
endmodule

