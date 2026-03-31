module myip (
    input clk,
    input resetn,

    input start, // start will be raised for a single cycle when start is requested
    output done, // done should be raised for a single cycle when accel is done

    output [5:0] input_mem_addr, // 6 bits for 64 rows
    output input_mem_re, // Input memory read enable
    input [7:0][7:0] input_mem_data, // 8 8 bit values

    output [5:0] output_mem_addr, // 6 bits for 64 rows
    output output_mem_we, // Output memory write enable
    output [7:0] output_mem_data, // Single 8 bit value

    input [1:0][7:0][7:0] w_hid, // 8x2 matrix of 8 bit values. Currently column major, you may want to swap
    input [2:0][7:0] w_out // 3x1 matrix
);

    reg [6:0] addr; // 7 bits so it can count up to 64 and idle
    reg [5:0] out_addr;
    reg out_we;

    reg [15:0] result;
    reg [15:0] hidden[1:0];
    integer i;
    
    always @(posedge clk) begin
        if (!resetn) begin // I assume it's active low
            addr     <= 64; // idle at 64
            out_addr <= 0;
            out_we   <= 0;
        end else if (start) begin
            addr     <= 0;
            out_we   <= 0;
        end else if (addr < 64) begin
            addr <= addr + 1;
            out_addr <= addr[5:0];
            out_we   <= 1;
        end else begin
            out_we   <= 0; // Stop writing once we finish row 63
        end
    end

    assign input_mem_addr  = addr[5:0];
    assign input_mem_re    = (addr < 64);
    
    assign output_mem_addr = out_addr;
    assign output_mem_we   = out_we;

    // combinational math
    always @* begin
        hidden[0] = 0;
        hidden[1] = 0;
        
        for (i = 1; i < 8; i = i + 1) begin
            hidden[0] = hidden[0] + input_mem_data[i-1] * w_hid[0][i];
            hidden[1] = hidden[1] + input_mem_data[i-1] * w_hid[1][i];
        end
        
        hidden[0] = (hidden[0] >> 8) + w_hid[0][0];
        hidden[1] = (hidden[1] >> 8) + w_hid[0][0];

        //TODO: Apply sigmoid function

        // Output layer
        result = w_out[0] + ((hidden[0] * w_out[1]) + (hidden[1] * w_out[2]) >> 8);
    end

    assign output_mem_data = result[7:0];
    
    // pulses high for 1 cycle when the final row is written
    assign done = (out_addr == 63 && out_we == 1);
endmodule