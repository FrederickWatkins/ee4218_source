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

    input w_hid [1:0][7:0][7:0], // 8x2 matrix of 8 bit values. Currently column major, you may want to swap
    input w_out [3:0][7:0] // 3x1 matrix
);

    // Something along the lines of:
    // mat_mul(input_mem, w_hid) -> intermediate memory 1
    // activate(intermediate memory 1) -> intermediate memory 2
    // mat_mul(intermediate memory 2, w_out) -> output_mem

    // I'd expect mat_mul and activate to have start and done signals like myip, that way they can
    // just be daisychained together, ie mat_mul_1.start = myip.start,
    // activate.start = mat_mul_1.done, mat_mul_2.start = activate.done, myip.done = mat_mul_2.done

    // Also note that w_hid and w_out aren't stored in memories, all of the values are available
    // all of the time and they can just be indexed into

endmodule
