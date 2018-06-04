module icosoc_mod_triggerrec #(
	parameter integer CLOCK_FREQ_HZ = 0, // unused
	parameter integer MAX_EVENTS = 8,
	parameter integer IO_LENGTH = 16
) (
	input clk,
	input clk_fast, // this should be at least twice as fast as clk
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
	reg [31:0] status, status_in;
	reg status_wr;

	// register array holding the event trigger configurations
	reg [31:0] triggers [0:(MAX_EVENTS << 1) - 1];
	reg [31:0] triggers_in, triggers_out;
	reg triggers_wr, triggers_rd;
	reg [4:0] triggers_addr;
	integer i;
	
	/* example trigger trig1
	wire [15:0] io1_xmask = io_buf1 | triggers[0][15:0];
	wire [15:0] io_mask = triggers[0][15:0];
	wire [15:0] io2_xmask = io_buf2 | triggers[0][15:0];
	wire [15:0] io1_match = io1_xmask ~^ triggers[1][15:0];
	wire [15:0] io2_match = io2_xmask ~^ triggers[1][31:16];
	wire trig1 = &{io1_match, io2_match};
	*/

	
	wire trig [0:MAX_EVENTS-1];
	genvar ii;
	generate 
	for (ii = 0; ii < (MAX_EVENTS-1); ii=ii+1) begin 
		assign trig[ii] = &triggers[(ii << 1)][15:0]? 0 :  // all 0xFF's? -> trigger not active 
					//   AND REDUCTION( xmask (don't care) XNOR  match )
					&{ (io_buf2 | triggers[(ii << 1)][15:0]) ~^ triggers[(ii << 1)+1][31:16], // match buf2 (last input)
	       			    	(io_buf1 | triggers[(ii << 1)][15:0]) ~^ triggers[(ii << 1)+1][15:0]}; // match buf1 (current input)

	end
	endgenerate 
	

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
		.PULLUP(1'b0) // b1 = pullup; b0 = floating
	) ios [IO_LENGTH-1:0] (
		.PACKAGE_PIN(IO),
		.D_IN_0(io_in)
	);

	// initialize triggers array
	initial $readmemh("../../mod_triggerrec/bram_triggers_template.hex", triggers);
	
	/* 
	 * keep the read and write logic of the triggers array as 
	 * simple as possible to ensure it gets synthesized as BRAM 
	 * and not as FFs 
	 * (that makes the bus logic slightly more complicated)
	 */

	// read trigger
	always @(posedge clk) begin
		if (!resetn)
			triggers_out <= 0;
		else if (triggers_rd)
			triggers_out <= triggers[triggers_addr];
	end

	// write trigger
	always @(posedge clk) begin
		if (triggers_wr) 
			triggers[triggers_addr] <= triggers_in;
	end


	// bus communication
	always @(posedge clk) begin
		
		// reset
		if (!resetn) begin
		       counter_in <= 0;
		       ctrl_state <= 0;
		       data_in <= 0;
		       triggers_in <= 0;

		       /* WARNING: don't try to initialize the triggers array
		        * with a for-loop: the array will be synthesized as FFs 
			* instead of BRAM!
	       		* .. use $readmem instead (see above) */ 	
		       
		       // initialize trigger array with 0xFF's
		       //for (i = 0; i < (MAX_EVENTS << 1) ; i = i + 1)
			 //      triggers[i] <= ~32'h0;
	       	end else;	
		
		
		// defaults
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
				if (ctrl_addr == 4) begin
					status_in <= ctrl_wdat;
					status_wr <= 1;
				end

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
	always @(posedge clk_fast) begin
		
		if (!resetn) begin
			status <= 0;
			counter <= 0;
		end

		// defaults:
		shift_in_fast <= shift_rise; // generate fast pulse for slow shift_in	
		
		// slow shift_in rising detection
		shift_reg <= {shift_reg[1:0], shift_in};
		
		// TODO: signals to synchronize:
		// counter_wr
		// status_wr

		// if running ..
		if (status[0]) begin 
			counter <= counter + 1;
			
			// if "dump" mode ..
			if (status[1]) begin 
				// trigger on all input changes
				if ((io_buf2 != io_buf1)) begin
					data_in_fast <= { io_buf1, 1'b0, counter[46:0] };
					shift_in_fast <= 1;
				end	
			// else event detection ..
			end else begin
				for (i = 0; i < (MAX_EVENTS-1); i=i+1) begin
					if (trig[i]) begin
						data_in_fast <= { io_buf1, 1'b1, counter[46:0] };
						shift_in_fast <= 1;
						// apply trigger commands
						/**/
						if (triggers[i << 1][16])
							status[0] <= 1;
						else if (triggers[i << 1][17])
							status[0] <= 0;
						if (triggers[i << 1][18])
							status[1] <= 1;
						else if (triggers[i << 1][19])
							status[1] = 0;
						/**/
					end

				end
			end
		end
		if (counter_wr)
			counter <= counter_in;
		if (status_wr)
			status <= status_in;
		
		io_buf1 <= io_in;
	       	io_buf2 <= io_buf1;	
	end

	// fifo
	icosoc_crossclkfifo #(
		.WIDTH(64),
		.DEPTH(128)
	) events_fifo (
		.in_clk(clk_fast),
		.in_shift(shift_in_fast),
		.in_data(data_sel),
		.in_nempty(nempty_in),

		.out_clk(clk),
		.out_pop(pop_out),
		.out_data(data_out),
		.out_nempty(nempty_out)
	);
endmodule

