#include "myaccel.h"
#include "xmyaccel_hw.h"
#include <xil_io.h>

void myaccel_upload_w_hid(MyAccel* instance, uint8_t* w_hid) {
    uint32_t* w_hid_base = instance->baseaddress + XMYACCEL_CONTROL_ADDR_W_HID_BASE;
    uint32_t* w_hid_32 = (uint32_t*)w_hid;
    for(int i = 0; i < 4; i++) {
        Xil_Out32((UINTPTR)&w_hid_base[i], w_hid_32[i]);
    }
}

void myaccel_upload_w_out(MyAccel* instance, uint8_t* w_out) {
    uint32_t* w_out_base = instance->baseaddress + XMYACCEL_CONTROL_ADDR_W_OUT_DATA;
    uint32_t* w_out_32 = (uint32_t*)w_out;
    Xil_Out32((UINTPTR)w_out_base, *w_out_32);
}