
`timescale 1 ns / 1 ps

	module myaccel_RTL #
	(
		// Users to add parameters here

		// User parameters ends
		// Do not modify the parameters beyond this line


		// Parameters of Axi Slave Bus Interface S00_AXI
		parameter integer C_S00_AXI_DATA_WIDTH	= 32,
		parameter integer C_S00_AXI_ADDR_WIDTH	= 5,

		// Parameters of Axi Slave Bus Interface S00_AXIS
		parameter integer C_S00_AXIS_TDATA_WIDTH	= 64,

		// Parameters of Axi Master Bus Interface M00_AXIS
		parameter integer C_M00_AXIS_TDATA_WIDTH	= 64,
		parameter integer C_M00_AXIS_START_COUNT	= 32
	)
	(
		// Users to add ports here

		// User ports ends
		// Do not modify the ports beyond this line


		// Ports of Axi Slave Bus Interface S00_AXI
		input wire  s00_axi_aclk,
		input wire  s00_axi_aresetn,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_awaddr,
		input wire [2 : 0] s00_axi_awprot,
		input wire  s00_axi_awvalid,
		output wire  s00_axi_awready,
		input wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_wdata,
		input wire [(C_S00_AXI_DATA_WIDTH/8)-1 : 0] s00_axi_wstrb,
		input wire  s00_axi_wvalid,
		output wire  s00_axi_wready,
		output wire [1 : 0] s00_axi_bresp,
		output wire  s00_axi_bvalid,
		input wire  s00_axi_bready,
		input wire [C_S00_AXI_ADDR_WIDTH-1 : 0] s00_axi_araddr,
		input wire [2 : 0] s00_axi_arprot,
		input wire  s00_axi_arvalid,
		output wire  s00_axi_arready,
		output wire [C_S00_AXI_DATA_WIDTH-1 : 0] s00_axi_rdata,
		output wire [1 : 0] s00_axi_rresp,
		output wire  s00_axi_rvalid,
		input wire  s00_axi_rready,

		// Ports of Axi Slave Bus Interface S00_AXIS
		input wire  s00_axis_aclk,
		input wire  s00_axis_aresetn,
		output wire  s00_axis_tready,
		input wire [C_S00_AXIS_TDATA_WIDTH-1 : 0] s00_axis_tdata,
		input wire [(C_S00_AXIS_TDATA_WIDTH/8)-1 : 0] s00_axis_tstrb,
		input wire  s00_axis_tlast,
		input wire  s00_axis_tvalid,

		// Ports of Axi Master Bus Interface M00_AXIS
		input wire  m00_axis_aclk,
		input wire  m00_axis_aresetn,
		output wire  m00_axis_tvalid,
		output wire [C_M00_AXIS_TDATA_WIDTH-1 : 0] m00_axis_tdata,
		output wire [(C_M00_AXIS_TDATA_WIDTH/8)-1 : 0] m00_axis_tstrb,
		output wire  m00_axis_tlast,
		input wire  m00_axis_tready
	);

	logic ip_ready;
	logic start, done, input_mem_re, input_mem_we, output_mem_re, output_mem_we;
	logic [5:0] input_mem_read_addr, input_mem_write_addr, output_mem_write_addr;
	logic [2:0] output_mem_read_addr;
	logic [7:0][7:0] input_mem_read_data, input_mem_write_data;
	logic [7:0] output_mem_write_data;
	logic [63:0] output_mem_read_data;
	logic [1:0][7:0][7:0] w_hid;
	logic [2:0][7:0] w_out;


// Instantiation of Axi Bus Interface S00_AXI
	myaccel_slave_lite_v1_0_S00_AXI # ( 
		.C_S_AXI_DATA_WIDTH(C_S00_AXI_DATA_WIDTH),
		.C_S_AXI_ADDR_WIDTH(C_S00_AXI_ADDR_WIDTH)
	) myaccel_slave_lite_v1_0_S00_AXI_inst (
		.w_hid(w_hid),
		.w_out(w_out),
		.S_AXI_ACLK(s00_axi_aclk),
		.S_AXI_ARESETN(s00_axi_aresetn),
		.S_AXI_AWADDR(s00_axi_awaddr),
		.S_AXI_AWPROT(s00_axi_awprot),
		.S_AXI_AWVALID(s00_axi_awvalid),
		.S_AXI_AWREADY(s00_axi_awready),
		.S_AXI_WDATA(s00_axi_wdata),
		.S_AXI_WSTRB(s00_axi_wstrb),
		.S_AXI_WVALID(s00_axi_wvalid),
		.S_AXI_WREADY(s00_axi_wready),
		.S_AXI_BRESP(s00_axi_bresp),
		.S_AXI_BVALID(s00_axi_bvalid),
		.S_AXI_BREADY(s00_axi_bready),
		.S_AXI_ARADDR(s00_axi_araddr),
		.S_AXI_ARPROT(s00_axi_arprot),
		.S_AXI_ARVALID(s00_axi_arvalid),
		.S_AXI_ARREADY(s00_axi_arready),
		.S_AXI_RDATA(s00_axi_rdata),
		.S_AXI_RRESP(s00_axi_rresp),
		.S_AXI_RVALID(s00_axi_rvalid),
		.S_AXI_RREADY(s00_axi_rready)
	);

// Instantiation of Axi Bus Interface S00_AXIS
	myaccel_slave_stream_v1_0_S00_AXIS # ( 
		.C_S_AXIS_TDATA_WIDTH(C_S00_AXIS_TDATA_WIDTH)
	) myaccel_slave_stream_v1_0_S00_AXIS_inst (
		.ip_ready(ip_ready),
		.memory_we(input_mem_we),
		.write_row(input_mem_write_addr),
		.write_data(input_mem_write_data),
		.writes_done(start),
		.S_AXIS_ACLK(s00_axis_aclk),
		.S_AXIS_ARESETN(s00_axis_aresetn),
		.S_AXIS_TREADY(s00_axis_tready),
		.S_AXIS_TDATA(s00_axis_tdata),
		.S_AXIS_TSTRB(s00_axis_tstrb),
		.S_AXIS_TLAST(s00_axis_tlast),
		.S_AXIS_TVALID(s00_axis_tvalid)
	);

// Instantiation of Axi Bus Interface M00_AXIS
	myaccel_master_stream_v1_0_M00_AXIS # ( 
		.C_M_AXIS_TDATA_WIDTH(C_M00_AXIS_TDATA_WIDTH),
		.C_M_START_COUNT(C_M00_AXIS_START_COUNT)
	) myaccel_master_stream_v1_0_M00_AXIS_inst (
		.read_pointer(output_mem_read_addr),
		.data_in(output_mem_read_data),
		.start(done),
		.M_AXIS_ACLK(m00_axis_aclk),
		.M_AXIS_ARESETN(m00_axis_aresetn),
		.M_AXIS_TVALID(m00_axis_tvalid),
		.M_AXIS_TDATA(m00_axis_tdata),
		.M_AXIS_TSTRB(m00_axis_tstrb),
		.M_AXIS_TLAST(m00_axis_tlast),
		.M_AXIS_TREADY(m00_axis_tready)
	);

	// Add user logic here
	assign output_mem_re = 1;
	always @(posedge s00_axi_aclk) begin
		if(!s00_axi_aresetn) begin
			ip_ready <= 1'b1;
		end
		else begin
			if(start) begin
				ip_ready <= 1'b0;
			end
			if(done) begin
				ip_ready <= 1'b1;
			end
		end
	end
	
	myip myip_inst (
		.clk(s00_axi_aclk),
		.resetn(s00_axi_aresetn),

		.start,
		.done,

		.input_mem_addr(input_mem_read_addr),
		.input_mem_re(input_mem_re),
		.input_mem_data(input_mem_read_data),

		.output_mem_addr(output_mem_write_addr),
		.output_mem_we(output_mem_we),
		.output_mem_data(output_mem_write_data),

		.w_hid(w_hid),
		.w_out(w_out)
	);

	matrix_RAM #(
		.width(8),
		.depth_rows(6),
		.num_cols(8)
	) input_mem (
		.clk(s00_axi_aclk),
		.write_en(input_mem_we),
		.write_address(input_mem_write_addr),
		.write_data_in(input_mem_write_data),
		.read_en(input_mem_re),
		.read_address(input_mem_read_addr),
		.read_data_out(input_mem_read_data)
	);

	output_mem output_mem_inst (
		.clk(s00_axi_aclk),
		.write_en(output_mem_we),
		.write_address(output_mem_write_addr),
		.write_data_in(output_mem_write_data),
		.read_en(output_mem_re),
		.read_address(output_mem_read_addr),
		.read_data_out(output_mem_read_data)
	);





	// User logic ends

	endmodule
