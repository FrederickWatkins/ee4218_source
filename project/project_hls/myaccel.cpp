#include "myaccel.hpp"

void myaccel(hls::stream<AXIS>& S_AXIS, hls::stream<AXIS>& M_AXIS, ap_uint<64> w_hid[2], ap_uint<64> w_out[1]) {
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS INTERFACE axis port=S_AXIS
#pragma HLS INTERFACE axis port=M_AXIS
#pragma HLS INTERFACE s_axilite port=w_hid
#pragma HLS INTERFACE s_axilite port=w_out

#pragma HLS DATAFLOW

    ap_uint<8> mat_receive[64][7];
    ap_uint<8> mat_hidden[64][2];
    ap_uint<8> mat_activated[64][2];
    ap_uint<8> mat_out[64][1];

#pragma HLS STABLE variable=w_hid
#pragma HLS STABLE variable=w_out

    receive_matrix<64, 7>(S_AXIS, mat_receive);
    bias_matrix_multiply<64, 7, 8, 2>(mat_receive, w_hid, mat_hidden);
    matrix_activate<64, 2>(mat_hidden, mat_activated);
    bias_matrix_multiply<64, 2, 3, 1>(mat_activated, w_out, mat_out);
    send_matrix<64>(M_AXIS, mat_out);
    
#ifndef __SYNTHESIS__
    printf("Received matrix:\n");
    matrix_print<64, 7>(mat_receive);
    printf("Hidden matrix:\n");
    matrix_print<64, 2>(mat_hidden);
    printf("Activated matrix:\n");
    matrix_print<64, 2>(mat_activated);
    printf("Output matrix:\n");
    matrix_print<64, 1>(mat_out);
#endif
}