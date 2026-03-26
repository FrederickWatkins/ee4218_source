module output_mem (
    input clk,

    input write_en,
    input [5:0] write_address,
    input [7:0] write_data_in,
    input read_en,
    input [2:0] read_address,
    output logic [63:0] read_data_out
);

    logic [7:0][63:0] ram;
    logic [2:0] write_index;
    logic [2:0] write_offset;

    always @(posedge clk) begin
        if(write_en) begin
            write_index = write_address[5:3];
            write_offset = write_address[2:0];
            ram[write_index][(write_offset * 8) +: 8] <= write_data_in;
        end
        if(read_en) begin
            read_data_out <= ram[read_address];
        end
    end

endmodule
