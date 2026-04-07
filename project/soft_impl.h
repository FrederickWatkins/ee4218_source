#include <stddef.h>
#include <stdint.h>

void run_inference(uint8_t* mat_X, uint8_t* mat_res, uint8_t* w_hid, uint8_t* w_out);

void bias_matrix_multiply(uint8_t* mat_a, uint8_t* w_in, uint8_t* mat_res, size_t a_rows, size_t a_cols, size_t b_rows, size_t b_cols, size_t a_stride);

void matrix_activate(uint8_t* mat_a, uint8_t* mat_res, size_t a_cols, size_t a_rows);
