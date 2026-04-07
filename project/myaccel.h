#include <stdint.h>

typedef struct {
    void* baseaddress;
} MyAccel;

// Writes hidden weights to accelerator
// w_hid is a pointer to a 8x2 weight matrix in COLUMN MAJOR order
void myaccel_upload_w_hid(MyAccel* instance, uint8_t* w_hid);

// Write output weights to accelerator
//
// w_out is a pointer to a 3x1 weight matrix in COLUMN MAJOR order
void myaccel_upload_w_out(MyAccel* instance, uint8_t* w_out);
