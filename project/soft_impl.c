#include "soft_impl.h"

#include <stddef.h>
#include <stdint.h>
#include <xil_printf.h>

const uint8_t sigmoid[256] = {
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

void run_inference(uint8_t* mat_X, uint8_t* mat_res, uint8_t* w_hid, uint8_t* w_out) {
    uint8_t mat_hidden[64 * 2];
    uint8_t mat_activated[64 * 2];
    bias_matrix_multiply(mat_X, w_hid, mat_hidden, 64, 7, 8, 2, 8);
    matrix_activate(mat_hidden, mat_activated, 64, 2);
    bias_matrix_multiply(mat_activated, w_out, mat_res, 64, 2, 3, 1, 2);
}

void bias_matrix_multiply(uint8_t* mat_a, uint8_t* w_in, uint8_t* mat_res, size_t a_rows, size_t a_cols, size_t b_rows, size_t b_cols, size_t a_stride) {
    for(size_t row = 0; row < a_rows; row++) {
        for(size_t col = 0; col < b_cols; col++) {
            uint16_t sum = (uint16_t)w_in[col * b_rows] * 256;
            for(size_t i = 0; i < a_cols; i++) {
                sum += (uint16_t)mat_a[row * (a_stride) + i] * (uint16_t)w_in[col * b_rows + i + 1];
            }
            mat_res[row * b_cols + col] = sum / 256;
        }
    }
}

void matrix_activate(uint8_t* mat_a, uint8_t* mat_res, size_t a_rows, size_t a_cols) {
    for(size_t row = 0; row < a_rows; row++) {
        for(size_t col = 0; col < a_cols; col++) {
            mat_res[row * a_cols + col] = sigmoid[mat_a[row * a_cols + col]];
        }
    }
}

