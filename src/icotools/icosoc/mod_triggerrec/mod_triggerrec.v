module icosoc_mod_triggerrec #(
	parameter integer CLOCK_FREQ_HZ = 0, // unused
	parameter integer MAX_EVENTS = 16,
	parameter integer IO_LENGTH = 16
) (
	input clk,
	input clk_fast,
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
	// gpio input
	wire [IO_LENGTH-1:0] io_in;
	reg [IO_LENGTH-1:0] io_buf1, io_buf2;
	
	// state register for switching uppper/lower bytes 
	// of 64 bit registers (for bus transfer) 
	reg [2:0] ctrl_state;

	// status register (running, stopped etc.)
	reg [31:0] status;

	// register array holding the event trigger configurations
	reg [31:0] triggers [0:(MAX_EVENTS << 1) - 1];
	reg [31:0] triggers_in, triggers_out;
	reg triggers_wr, triggers_rd;
	reg [4:0] triggers_addr;
	integer i, ii, iii;

	reg [63:0] counter, counter_in;
	reg [31:0] counter_buf;
	reg counter_wr;

	// registers for the event fifo
	reg [63:0] data_in, data_in_fast; 
	wire [63:0] data_out;
	reg shift_in, shift_in_fast, pop_out;
	wire nempty_in, nempty_out;
	wire full_in, full_out;

	wire shift_sel = shift_in_fast || shift_in;
	wire [63:0] data_sel = (shift_in)? data_in : data_in_fast;

	reg [2:0] shift_reg;
	wire shift_rise = shift_reg[2:1] == 'b01;
	

	SB_IO #(
		.PIN_TYPE(6'b 0000_01),
		.PULLUP(1'b 1)
	) ios [IO_LENGTH-1:0] (
		.PACKAGE_PIN(IO),
		.D_IN_0(io_in)
	);

	initial $readmemh("../../mod_triggerrec/bram_triggers_template.hex", triggers);

	always @(posedge clk) begin
		if (!resetn)
			triggers_out <= 0;
		else if (triggers_rd)
			triggers_out <= triggers[triggers_addr];
	end

	always @(posedge clk) begin
		if (triggers_wr) 
			triggers[triggers_addr] <= triggers_in;
	end


	always @(posedge clk) begin
		
		// reset
		if (!resetn) begin
		       counter_in <= 0;
		       status <= 0;
		       ctrl_state <= 0;
		       data_in <= 0;
		       triggers_in <= 0;
		       // initialize trigger array with 0xFF's
		       //for (i = 0; i < (MAX_EVENTS << 1) ; i = i + 1)
			 //      triggers[i] <= ~32'h0;
	       	end else;	
		
		
		// bus communication
		ctrl_rdat <= 'bx;
		ctrl_done <= 0;
		pop_out <= 0;
		shift_in <= 0;

		triggers_rd <= 0;
		triggers_wr <= 0;

		counter_wr <= 0;

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
						counter_in[63:32] <= ctrl_wdat;
					else
						counter_in[31:0] <= ctrl_wdat;
					ctrl_state <= ~ctrl_state;
					counter_wr <= ctrl_state;
				end
		
				// write fifo  
				if (ctrl_addr == 'hc) begin
					if (ctrl_state == 0 ) begin
						data_in[63:32] <= ctrl_wdat;
					end else begin
						data_in[31:0] <= ctrl_wdat;
						shift_in <= 1;
					end
					
					ctrl_state <= ~ctrl_state;
				end
							
				// write triggers  
				if (ctrl_addr >= 'h100) begin
					triggers_addr <= (ctrl_addr[5:0] >> 1) | ctrl_state[0];
				       	triggers_in <= ctrl_wdat;
					triggers_wr <= 1;
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
				
				// read fifo
				if (ctrl_addr == 'hc) begin
					ctrl_rdat <= (ctrl_state)? data_out[31:0] : data_out[63:32];
					pop_out <= ctrl_state;
					ctrl_state <= ~ctrl_state;
				end
				// read triggers
				if (ctrl_addr >= 'h100) begin
					ctrl_done <= 0;
					if (ctrl_state[1:0] == 'b00) begin
						triggers_addr <= (ctrl_addr[5:0] >> 1) | ctrl_state[2];
						triggers_rd <= 1;						
					end else if (ctrl_state[1:0] == 'b01) begin
						// wait for triggers_out
						// register
					end else if (ctrl_state[1:0] == 'b11) begin
						ctrl_done <= 1;
						ctrl_rdat <= triggers_out;
					end

					ctrl_state <= ctrl_state + 1;
				end

			end
		end
		
	end	
	
	// trigger logic
	always @(posedge clk) begin
		// defaults:
		shift_in_fast <= 0; //shift_rise; // generate fast pulse for slow shift_in	
		
		// slow shift_in rising detection
		shift_reg <= {shift_reg[1:0], shift_in};
		
		// TODO: signals to synchronize:
		// status
		// counter_wr

		// stop counter when paused
		if (status[0]) begin 
			counter <= counter + 1;

			// trigger on all input changes
			if ((io_buf1 != io_in) && (io_in != 0)) begin
				data_in_fast <= { ~io_in, counter[47:0] };
				shift_in_fast <= 1;
			end	
		end
		if (counter_wr)
			counter <= counter_in;
		
		io_buf1 <= io_in;
	       	io_buf2 <= io_buf1;	
	end

	// fifo
	icosoc_crossclkfifo #(
		.WIDTH(64),
		.DEPTH(128)
	) events_fifo (
		.in_clk(clk),
		.in_shift(shift_sel),
		.in_data(data_sel),
		.in_nempty(nempty_in),

		.out_clk(clk),
		.out_pop(pop_out),
		.out_data(data_out),
		.out_nempty(nempty_out)
	);
endmodule

