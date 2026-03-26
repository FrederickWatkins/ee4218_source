#include <iostream>
#include "hls_stream.h"
#include "ap_axi_sdata.h"

typedef ap_axis<64,0,0,0> AXIS;  //data, user, id, dest

void myaccel(hls::stream<AXIS>& S_AXIS, hls::stream<AXIS>& M_AXIS, ap_uint<64> w_hid_in[2], ap_uint<64> w_out_in[1]);

// Column-Major Packing: Each 64-bit word is one full column
ap_uint<64> w_hid[2] = {
    ((ap_uint<64>)26 << 56) | // Row 7
    ((ap_uint<64>)11 << 48) | // Row 6
    ((ap_uint<64>)1  << 40) | // Row 5
    ((ap_uint<64>)22 << 32) | // Row 4
    ((ap_uint<64>)29 << 24) | // Row 3
    ((ap_uint<64>)31 << 16) | // Row 2
    ((ap_uint<64>)25 << 8)  | // Row 1
    ((ap_uint<64>)26),        // Row 0 (LSB)
    ((ap_uint<64>)45 << 56) | // Row 7
    ((ap_uint<64>)9  << 48) | // Row 6
    ((ap_uint<64>)28 << 40) | // Row 5
    ((ap_uint<64>)1  << 32) | // Row 4
    ((ap_uint<64>)26 << 24) | // Row 3
    ((ap_uint<64>)6  << 16) | // Row 2
    ((ap_uint<64>)18 << 8)  | // Row 1
    ((ap_uint<64>)6),         // Row 0 (LSB)
};

// Column 0: {80, 50, 200}
ap_uint<64> w_out[1] = {
    ((ap_uint<64>)200 << 16) | // Row 2
    ((ap_uint<64>)50  << 8)  | // Row 1
    ((ap_uint<64>)80),         // Row 0 (LSB)
};
    
ap_uint<8> X_data[64][7] = {
    {44, 90, 0, 0, 24, 81, 22},
    {159, 250, 140, 176, 121, 183, 138},
    {167, 158, 172, 134, 172, 161, 118},
    {130, 178, 136, 120, 135, 121, 86},
    {34, 112, 142, 112, 163, 159, 43},
    {63, 28, 145, 126, 190, 165, 66},
    {88, 214, 157, 140, 205, 174, 128},
    {185, 203, 170, 110, 211, 138, 126},
    {140, 255, 110, 136, 133, 156, 131},
    {73, 90, 164, 118, 75, 121, 111},
    {16, 93, 187, 88, 162, 101, 27},
    {31, 110, 120, 77, 166, 101, 45},
    {183, 181, 155, 175, 135, 211, 185},
    {129, 193, 104, 159, 184, 183, 161},
    {190, 183, 143, 124, 139, 138, 145},
    {79, 113, 115, 111, 164, 101, 88},
    {213, 213, 149, 159, 133, 199, 183},
    {13, 94, 127, 146, 132, 150, 89},
    {103, 175, 164, 139, 187, 147, 128},
    {42, 82, 104, 83, 167, 50, 71},
    {26, 90, 109, 143, 135, 220, 106},
    {69, 90, 101, 180, 125, 222, 108},
    {143, 225, 125, 147, 196, 197, 121},
    {73, 87, 125, 29, 8, 87, 67},
    {100, 136, 254, 119, 170, 140, 77},
    {110, 136, 101, 137, 186, 174, 126},
    {165, 143, 179, 151, 167, 156, 147},
    {182, 207, 131, 105, 130, 101, 124},
    {34, 51, 194, 94, 90, 94, 58},
    {108, 85, 116, 25, 24, 0, 59},
    {18, 70, 110, 118, 179, 138, 50},
    {35, 165, 120, 72, 126, 72, 82},
    {72, 99, 85, 76, 210, 101, 56},
    {27, 85, 85, 109, 122, 133, 54},
    {149, 166, 176, 111, 135, 115, 98},
    {138, 180, 136, 131, 255, 139, 84},
    {120, 97, 115, 96, 110, 128, 44},
    {183, 181, 183, 152, 119, 174, 148},
    {38, 113, 125, 67, 88, 26, 68},
    {180, 188, 169, 137, 188, 124, 145},
    {86, 87, 80, 72, 76, 73, 71},
    {219, 224, 155, 165, 197, 252, 218},
    {31, 65, 145, 38, 112, 32, 78},
    {21, 134, 48, 83, 94, 78, 111},
    {111, 62, 128, 89, 163, 209, 65},
    {224, 206, 128, 155, 167, 170, 150},
    {231, 225, 139, 174, 149, 202, 208},
    {140, 146, 106, 124, 192, 142, 104},
    {116, 191, 196, 136, 192, 170, 108},
    {43, 136, 131, 58, 44, 50, 118},
    {39, 76, 130, 63, 71, 62, 39},
    {103, 166, 170, 115, 236, 131, 75},
    {83, 148, 206, 120, 142, 156, 102},
    {73, 41, 150, 63, 123, 78, 51},
    {159, 136, 93, 153, 140, 149, 198},
    {14, 77, 160, 67, 68, 72, 56},
    {37, 70, 131, 53, 72, 46, 37},
    {225, 171, 136, 148, 136, 161, 188},
    {64, 177, 76, 69, 92, 92, 84},
    {44, 54, 166, 93, 158, 101, 59},
    {153, 170, 150, 125, 152, 171, 166},
    {86, 155, 136, 41, 36, 131, 63},
    {138, 160, 104, 119, 149, 124, 100},
    {19, 56, 140, 139, 217, 161, 51}
};

int labels[64] = {
    0,
    1,
    1,
    1,
    0,
    0,
    1,
    1,
    1,
    0,
    0,
    0,
    1,
    1,
    1,
    0,
    1,
    0,
    1,
    0,
    0,
    1,
    1,
    0,
    1,
    1,
    1,
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    1,
    1,
    0,
    1,
    0,
    1,
    0,
    1,
    0,
    0,
    1,
    1,
    1,
    1,
    1,
    0,
    0,
    1,
    1,
    0,
    1,
    0,
    0,
    1,
    0,
    0,
    1,
    0,
    1,
    0
};

int main() {

    // 2. Define Streams
    hls::stream<AXIS> input_stream("S_AXIS_stimulus");
    hls::stream<AXIS> output_stream("M_AXIS_results");

    // 3. Populate Input Stream (Stimulus)
    // The top function expects receive_matrix<64, 7>
    // This requires 64 reads from the stream
    for (int i = 0; i < 64; i++) {
        AXIS packet;
        packet.data = 0; // Data initialized to zero
        for (int j = 0; j < 7; j++) {
            packet.data |= (ap_uint<64>)X_data[i][j] << (j * 8);
        }
        packet.keep = 0xFF;
        packet.strb = 0xFF;
        
        // TLAST must be high only on the last row of the matrix
        if (i == 63) {
            packet.last = 1;
        } else {
            packet.last = 0;
        }
        
        input_stream.write(packet);
    }

    // 4. Call the Hardware Accelerator (DUT - Device Under Test)
    std::cout << ">> Starting Simulation..." << std::endl;
    myaccel(input_stream, output_stream, w_hid, w_out);

    // 5. Check Results
    // The top function calls send_matrix<8, 8>, which sends 8 packets
    int error_count = 0;
    int threshold = 128;

    std::cout << ">> Checking Output Stream..." << std::endl;
    for (int i = 0; i < 8; i++) {
        if (output_stream.empty()) {
            std::cout << "!! ERROR: Output stream empty at index " << i << " !!" << std::endl;
            error_count++;
            break;
        }

        AXIS out_packet = output_stream.read();
        
        for (int col = 0; col < 8; col++) {
            int idx = i * 8 + col;
            ap_uint<8> raw_val = (out_packet.data >> (col * 8)) & 0xFF;
            
            int prediction = (raw_val > threshold) ? 1 : 0;
            bool is_match = (prediction == labels[idx]);

            if (!is_match){
                error_count++;
                std::cout << "Incorrect value at idx " << idx << " value: " << raw_val << std::endl;
            }
        }
        
        // Verify TLAST on the last packet
        if (i == 7 && out_packet.last != 1) {
            std::cout << "!! ERROR: TLAST not set on last packet !!" << std::endl;
            error_count++;
        }
    }

    // 6. Final Status
    if (error_count == 0) {
        std::cout << ">> TEST PASSED! <<" << std::endl;
    } else {
        std::cout << ">> TEST FAILED with " << error_count << " errors. <<" << std::endl;
    }

    return error_count;
}