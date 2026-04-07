#include "ap_int.h"
#include "hls_stream.h"
#include "ap_axi_sdata.h"

const ap_uint<8> sigmoid[256] = {
    12,  12,  12,  12,  13,  13,  13,  14,  14,  14,  15,  15,
    15,  16,  16,  16,  17,  17,  18,  18,  18,  19,  19,  20,
    20,  21,  21,  21,  22,  22,  23,  23,  24,  24,  25,  26,
    26,  27,  27,  28,  28,  29,  30,  30,  31,  32,  32,  33,
    34,  34,  35,  36,  36,  37,  38,  39,  39,  40,  41,  42,
    43,  44,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,
    54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  66,
    67,  68,  69,  70,  72,  73,  74,  75,  76,  78,  79,  80,
    82,  83,  84,  86,  87,  88,  90,  91,  92,  94,  95,  97,
    98,  99,  101, 102, 104, 105, 107, 108, 110, 111, 113, 114,
    116, 117, 119, 120, 122, 123, 125, 126, 128, 129, 130, 132,
    133, 135, 136, 138, 139, 141, 142, 144, 145, 147, 148, 150,
    151, 153, 154, 156, 157, 158, 160, 161, 163, 164, 165, 167,
    168, 169, 171, 172, 173, 175, 176, 177, 179, 180, 181, 182,
    183, 185, 186, 187, 188, 189, 191, 192, 193, 194, 195, 196,
    197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208,
    209, 210, 211, 211, 212, 213, 214, 215, 216, 216, 217, 218,
    219, 219, 220, 221, 221, 222, 223, 223, 224, 225, 225, 226,
    227, 227, 228, 228, 229, 229, 230, 231, 231, 232, 232, 233,
    233, 234, 234, 234, 235, 235, 236, 236, 237, 237, 237, 238,
    238, 239, 239, 239, 240, 240, 240, 241, 241, 241, 242, 242,
    242, 243, 243, 243
};

typedef ap_axis<64,0,0,0> AXIS;  //data, user, id, dest

template<int rows, int cols>
void receive_matrix(hls::stream<AXIS>& S_AXIS, ap_uint<8> mat_receive[rows][cols]) {

    myaccel_receive_matrix_for1:for(int row = 0; row < rows; row++){
        AXIS read_input = S_AXIS.read();
        if(read_input.last == 1 && row != rows - 1 || read_input.last == 0 && row == rows - 1) {
            // TODO handle error
        }
        myaccel_receive_matrix_for2:for(int col = 0; col < cols; col++) {
            mat_receive[row][col] = read_input.data >> col * 8;
        }
    }
}

template<int cols>
inline ap_uint<8> get_weight(ap_uint<64> w_in[cols], int row, int col) {
#pragma HLS INLINE
    return (w_in[col] >> (row * 8)) & 0xFF;
}

template<int rows>
void send_matrix(hls::stream<AXIS>& M_AXIS, ap_uint<8> mat_send[rows][1]) {
    AXIS write_output;
    write_output.data = 0;
    myaccel_send_matrix_for1:for(int word = 0; word < rows / 8; word++){
        write_output.data = 0;
        myaccel_send_matrix_for2:for(int i = 0; i < 8; i++) {
            write_output.data |= (ap_uint<64>)mat_send[word * 8 + i][0] << (i * 8);
        }
        write_output.last = 0;
        write_output.keep = 0xFFU;
        write_output.strb = 0xFFU;
        if(word == (rows / 8) - 1)
        {
            write_output.last = 1;
        }
        M_AXIS.write(write_output);
    }
}

template<int a_rows, int a_cols, int b_rows, int b_cols>
void bias_matrix_multiply(ap_uint<8> mat_a[a_rows][a_cols], ap_uint<64> w_in[b_cols], ap_uint<8> mat_res[a_rows][b_cols]) {
    static_assert(a_cols + 1 == b_rows, "Common dimension mismatch");
    myaccel_bias_matrix_multiply_for1:for(int row = 0; row < a_rows; row++) {
        myaccel_bias_matrix_multiply_for2:for(int col = 0; col < b_cols; col++) {
            ap_uint<16> sum = (ap_uint<16>)get_weight<b_cols>(w_in, 0, col) * 256;
            myaccel_bias_matrix_multiply_for3:for(int i = 0; i < a_cols; i++) {
                sum += (ap_uint<16>)mat_a[row][i] * (ap_uint<16>)get_weight<b_cols>(w_in, i + 1, col);
            }
            mat_res[row][col] = sum / 256;
        }
    }
}

template<int a_rows, int a_cols>
void matrix_activate(ap_uint<8> mat_a[a_rows][a_cols], ap_uint<8> mat_res[a_rows][a_cols]) {
    myaccel_matrix_activate_for1:for(int row = 0; row < a_rows; row++) {
        myaccel_matrix_activate_for2:for(int col = 0; col < a_cols; col++) {
            mat_res[row][col] = sigmoid[mat_a[row][col]];
        }
    }
}

#ifndef __SYNTHESIS__
template <int rows, int cols>
void matrix_print(ap_uint<8> mat[rows][cols]) {
    for(int i = 0; i < rows; i++) {
        printf("Row %i: ", i);
        for(int j = 0; j < cols; j++) {
            std::cout << mat[i][j] << ",";
        }
        std::cout << std::endl;
    }
}
#endif
