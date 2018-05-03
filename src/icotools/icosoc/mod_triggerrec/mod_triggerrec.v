module icosoc_mod_triggerrec #(
	parameter integer CLOCK_FREQ_HZ = 0,
	parameter integer IO_LENGTH = 32
) (
	input clk,
	input resetn,

	input [3:0] ctrl_wr,
	input ctrl_rd,
	input [15:0] ctrl_addr,
	input [31:0] ctrl_wdat,
	output reg [31:0] ctrl_rdat,
	output reg ctrl_done,

	input [IO_LENGTH-1:0] IO
);
	wire [IO_LENGTH-1:0] io_in;
	reg [31:0] io_conf;
	//reg [IO_LENGTH-1:0] io_buf1;
	//reg dirty

	reg [31:0] counter;	

	SB_IO #(
		.PIN_TYPE(6'b 0000_01),
		.PULLUP(1'b 1)
	) ios [IO_LENGTH-1:0] (
		.PACKAGE_PIN(IO),
		.D_IN_0(io_in)
	);

	always @(posedge clk) begin
		ctrl_rdat <= 'bx;
		ctrl_done <= 0;
		

		counter <= counter + 1;
		
		if (!resetn) begin
		       counter <= 0;
	       	end else;	       
			
		if (!ctrl_done) begin
			if (|ctrl_wr) begin
				ctrl_done <= 1;
			//	if (ctrl_addr == 0) io_out <= ctrl_wdat;
				if (ctrl_addr == 4) io_conf <= ctrl_wdat;
			end
			if (ctrl_rd) begin
				ctrl_done <= 1;
				if (ctrl_addr == 0) ctrl_rdat <= io_in;
				if (ctrl_addr == 4) ctrl_rdat <= io_conf;
				if (ctrl_addr == 8) ctrl_rdat <= counter;
			end
		end
	end
endmodule

