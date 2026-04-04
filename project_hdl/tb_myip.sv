`timescale 1ns/1ps

module tb_myip();

    // ----------------------------------------------------
    // Clock and Reset Generation
    // ----------------------------------------------------
    logic clk;
    logic resetn;

    initial begin
        clk = 0;
        forever #5 clk = ~clk; // 100MHz clock
    end

    // ----------------------------------------------------
    // DUT Signals
    // ----------------------------------------------------
    logic start;
    logic done;

    // Input memory interface signals (matrix_RAM <-> myip)
    logic [5:0]        in_mem_addr;
    logic              in_mem_re;
    logic [7:0][7:0]   in_mem_data;

    // Output memory interface signals (myip <-> output_mem)
    logic [5:0]        out_mem_addr;
    logic              out_mem_we;
    logic [7:0]        out_mem_data;

    // Weight matrices
    logic [1:0][7:0][7:0] w_hid;
    logic [2:0][7:0]      w_out;

    // ----------------------------------------------------
    // Testbench Memory Control Signals
    // ----------------------------------------------------
    // Signals to write to matrix_RAM from TB
    logic              in_mem_we_tb;
    logic [5:0]        in_mem_addr_tb;
    logic [7:0][7:0]   in_mem_write_data_tb;

    // Signals to read from output_mem to TB
    logic              out_mem_re_tb;
    logic [2:0]        out_mem_addr_tb;
    logic [63:0]       out_mem_read_data_tb;

    // Local arrays to hold data from .mem files
    // 64 rows of 64 bits (8 cols * 8 bits) for input matrix
    logic [63:0]       tb_input_data [0:63]; 
    
    // 8 rows of 64 bits for expected output
    logic [63:0]       tb_expected_data [0:7];

    // ----------------------------------------------------
    // Module Instantiations
    // ----------------------------------------------------

    // Input Memory (matrix_RAM)
    matrix_RAM #(
        .width(8),
        .depth_rows(6), // 6 bits = 64 locations
        .num_cols(8)
    ) input_ram_inst (
        .clk(clk),
        .write_en(in_mem_we_tb),               // TB drives write
        .write_address(in_mem_addr_tb),        // TB drives write address
        .write_data_in(in_mem_write_data_tb),  // TB drives write data
        .read_en(in_mem_re),                   // DUT drives read
        .read_address(in_mem_addr),            // DUT drives read address
        .read_data_out(in_mem_data)            // DUT receives data
    );

    // Output Memory (output_mem)
    output_mem output_ram_inst (
        .clk(clk),
        .write_en(out_mem_we),                 // DUT drives write
        .write_address(out_mem_addr),          // DUT drives write address
        .write_data_in(out_mem_data),          // DUT drives write data
        .read_en(out_mem_re_tb),               // TB drives read
        .read_address(out_mem_addr_tb),        // TB drives read address
        .read_data_out(out_mem_read_data_tb)   // TB receives data to check
    );

    // DUT (myip)
    myip dut (
        .clk(clk),
        .resetn(resetn),
        .start(start),
        .done(done),
        .input_mem_addr(in_mem_addr),
        .input_mem_re(in_mem_re),
        .input_mem_data(in_mem_data),
        .output_mem_addr(out_mem_addr),
        .output_mem_we(out_mem_we),
        .output_mem_data(out_mem_data),
        .w_hid(w_hid),
        .w_out(w_out)
    );

    // ----------------------------------------------------
    // Constant Weight Initialization
    // ----------------------------------------------------
    initial begin
        // Initialize w_hid with some dummy constant values
        w_hid[0][0] = 26;	w_hid[1][0] = 6;
        w_hid[0][1] = 25;	w_hid[1][1] = 18;
        w_hid[0][2] = 31;	w_hid[1][2] = 6;
        w_hid[0][3] = 29;	w_hid[1][3] = 26;
        w_hid[0][4] = 22;	w_hid[1][4] = 1;
        w_hid[0][5] = 1;	w_hid[1][5] = 28;
        w_hid[0][6] = 11;	w_hid[1][6] = 9;
        w_hid[0][7] = 26;	w_hid[1][7] = 45;

        // Initialize w_out with some dummy constant values
        w_out[0] = 80;
        w_out[1] = 50;
        w_out[2] = 200;
    end

    // ----------------------------------------------------
    // Main Test Sequence
    // ----------------------------------------------------
    int errors;

    initial begin
        // 1. Initialize variables and files
        $display("Starting Testbench...");
        $readmemh("input_matrix.mem", tb_input_data);
        $readmemh("expected_output.mem", tb_expected_data);
        
        errors = 0;
        resetn = 0;
        start = 0;
        in_mem_we_tb = 0;
        out_mem_re_tb = 0;

        // Apply reset
        repeat(5) @(posedge clk);
        resetn = 1;
        repeat(2) @(posedge clk);

        // 2. Load the matrix_RAM from the file arrays
        $display("Loading input RAM...");
        in_mem_we_tb = 1;
        for (int i = 0; i < 64; i++) begin
            in_mem_addr_tb <= i[5:0];
            // SystemVerilog transparently maps [63:0] flat array to [7:0][7:0]
            in_mem_write_data_tb <= tb_input_data[i]; 
            @(posedge clk);
        end
        @(posedge clk);
        in_mem_we_tb = 0;

        // 3. Start the IP block
        $display("Starting the IP...");
        @(posedge clk);
        start = 1;
        @(posedge clk);
        start = 0;

        // 4. Wait for the 'done' signal
        wait(done == 1'b1);
        $display("IP processing complete. Validating results...");
        @(posedge clk);

        // 5. Read output_mem and compare with expected data
        out_mem_re_tb = 1;
        for (int i = 0; i < 8; i++) begin
            out_mem_addr_tb = i[2:0];
            @(posedge clk);
            #1; // Wait a moment for read data to settle
            
            if (out_mem_read_data_tb !== tb_expected_data[i]) begin
                $display("FAIL @ Address %0d: Expected %16h, Got %16h", i, tb_expected_data[i], out_mem_read_data_tb);
                errors++;
            end else begin
                $display("PASS @ Address %0d: Data = %16h", i, out_mem_read_data_tb);
            end
        end
        out_mem_re_tb = 0;

        // 6. Test Conclusion
        $display("--------------------------------------------------");
        if (errors == 0)
            $display("TEST PASSED! All outputs match expected values.");
        else
            $display("TEST FAILED! Total errors: %0d", errors);
        $display("--------------------------------------------------");
        
        $finish;
    end

endmodule
