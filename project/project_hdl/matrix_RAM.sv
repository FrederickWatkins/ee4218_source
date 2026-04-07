module matrix_RAM #(
    parameter width = 8,
    parameter depth_rows = 6,
    parameter num_cols = 8
) (
    input clk,
	input write_en,
	input [depth_rows-1:0] write_address,
	input [num_cols-1:0][width-1:0] write_data_in,
	input read_en,    
	input [depth_rows-1:0] read_address,
	output [num_cols-1:0][width-1:0] read_data_out
);
    generate 
        for(genvar i = 0; i < num_cols; i = i + 1) begin : RAM_BANKS
            memory_RAM #(
                .width(width),
                .depth_bits(depth_rows)
            ) ram_inst (
                .clk(clk),
                .write_en(write_en),
                .write_address(write_address),
                .write_data_in(write_data_in[i]),
                .read_en(read_en),
                .read_address(read_address),
                .read_data_out(read_data_out[i])
            );
        end
    endgenerate
            
endmodule
