/*
----------------------------------------------------------------------------------
--	(c) Rajesh C Panicker, NUS,
--  Description : AXI Stream Coprocessor (HLS), implementing the sum of 4 numbers
--	License terms :
--	You are free to use this code as long as you
--		(i) DO NOT post a modified version of this on any public repository;
--		(ii) use it only for educational purposes;
--		(iii) accept the responsibility to ensure that your implementation does not violate any intellectual property of any entity.
--		(iv) accept that the program is provided "as is" without warranty of any kind or assurance regarding its suitability for any particular purpose;
--		(v) send an email to rajesh.panicker@ieee.org briefly mentioning its use (except when used for the course EE4218/CEG5203 at the National University of Singapore);
--		(vi) retain this notice in this file or any files derived from this.
----------------------------------------------------------------------------------
*/

#include "hls_stream.h"
#include "ap_int.h"
#include "ap_axi_sdata.h"

#define A_ROWS 64
#define A_COLS 8

#define VALUE_WIDTH 8
#define WORD_WIDTH 128
#define PACK_FACTOR (WORD_WIDTH / VALUE_WIDTH)

#define A_LENGTH (A_ROWS * A_COLS)
#define B_LENGTH A_COLS
#define RES_LENGTH A_ROWS

#define NUMBER_OF_INPUT_WORDS ((A_LENGTH + B_LENGTH + PACK_FACTOR - 1) / PACK_FACTOR)  // length of an input vector
#define NUMBER_OF_OUTPUT_WORDS RES_LENGTH / PACK_FACTOR  // length of an input vector

// ACLK, ARESETN, TREADY, TDATA, TVALID are essential signals for AXIS. New version of AXI DMA seems to expect TSTRB and/or TKEEP as well.

typedef ap_axis<WORD_WIDTH,0,0,0> AXIS;  //data, user, id, dest

void myip_v1_0_HLS(hls::stream<AXIS>& S_AXIS, hls::stream<AXIS>& M_AXIS){
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis port=S_AXIS
#pragma HLS INTERFACE axis port=M_AXIS
    ap_uint<8> mat_A[A_LENGTH];
    ap_uint<8> mat_B[B_LENGTH];
    ap_uint<8> res_data[RES_LENGTH];
#pragma HLS array_partition variable=mat_A cyclic factor=64
#pragma HLS array_partition variable=mat_B cyclic factor=8
#pragma HLS dataflow
	int word_cnt;
	ap_uint<8> sum = 0; // using arbitrary precision
	//int sum = 0;		 // using 32 bit precision
	AXIS read_input, write_output;

    myip_v1_0_HLS_for1:for(word_cnt = 0; word_cnt < NUMBER_OF_INPUT_WORDS; word_cnt++){
        read_input = S_AXIS.read();
        // read_input is the element (data + other signals) received by our ip through S_AXIS in one clock cycle (which contains one word).
        // read() extracts it from the stream. Overloaded operator >> can also be used.
        myip_v1_0_HLS_for5:for(int i = 0; i < PACK_FACTOR; i++) {
            if(word_cnt * PACK_FACTOR + i >= A_LENGTH + B_LENGTH) {
                break;
            }
            if(word_cnt * PACK_FACTOR + i < A_LENGTH) {
                mat_A[word_cnt * PACK_FACTOR + i] = read_input.data >> i * VALUE_WIDTH;
            } else {
                mat_B[(word_cnt * PACK_FACTOR + i)-A_LENGTH] = read_input.data >> i * VALUE_WIDTH;
            }
        }
        
        // We are not making using of S_AXIS_TLAST in this example.
        // S_AXIS_TLAST is required only when we are receiving an unknown number of words.
    }

    myip_v1_0_HLS_for2:for(word_cnt = 0; word_cnt < RES_LENGTH; word_cnt++){
#pragma HLS pipeline
#pragma HLS unroll factor=8
        int sum = 0;
        myip_v1_0_HLS_for3:for(int col_cnt = 0; col_cnt < A_COLS; col_cnt++) {
            printf("mat_A value: %i, mat_B value: %i\n", mat_A[word_cnt * A_COLS + col_cnt], mat_B[col_cnt]);
            sum += mat_A[word_cnt * A_COLS + col_cnt] * mat_B[col_cnt];
        }
        printf("sum: %i\n", sum);
        sum /= 256;
        printf("sum: %i\n", sum);
        res_data[word_cnt] = sum;
    }

    myip_v1_0_HLS_for4:for(word_cnt = 0; word_cnt < NUMBER_OF_OUTPUT_WORDS; word_cnt++){
        //write_output.data = sum.to_int() + word_cnt;	// using arbitrary precision internally but int for interfacing
        write_output.data = 0;
        myip_v1_0_HLS_for6:for(int i = 0; i < PACK_FACTOR; i++) {
            // Explicitly cast to 128-bit before shifting
            write_output.data |= ((ap_uint<128>)res_data[word_cnt * PACK_FACTOR + i]) << (i * 8);
        }
        // write_output is the element sent by our ip through M_AXIS in one clock cycle.
        write_output.last = 0;
        write_output.keep = 0xFFFFU;
        write_output.strb = 0xFFFFU;
        if(word_cnt==NUMBER_OF_OUTPUT_WORDS-1)
        {
            write_output.last = 1;
            // M_AXIS_TLAST is required to be asserted for the last word.
            // Else, the AXI Stream FIFO / AXI DMA will not know if all the words have been received from the co-processor.
        }
        M_AXIS.write(write_output);
        // write() inserts it into the stream. Overloaded operator << can also be used.
    }
}

