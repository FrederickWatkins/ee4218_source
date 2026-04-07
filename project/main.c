#include "xil_cache.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xaxidma.h"         // Replaced xllfifo.h
#include "xparameters.h"
#include "xtmrctr.h"
#include "xstatus.h"
#include "xuartps.h"
#include "myaccel.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "soft_impl.h"

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ROWS 64 // Parametrizable
#define COLS 8

#define TIMER_COUNTER_0  0

#define WORD_SIZE 1 // Size of words in bytes

void init();
void uart_receive_matrix(uint8_t* matrix, size_t len);
void uart_send_matrix(uint8_t* matrix, size_t len);
void dma_send_matrix(uint8_t* matrix, size_t len);
void dma_receive_matrix(uint8_t* matrix, size_t len);

uint8_t w_hid[8 * 2] = {
    // Column 0
    26,
    25,
    31,
    29,
    22,
    1,
    11,
    26,
    // Column 1
    6,
    18,
    6,
    26,
    1,
    28,
    9,
    45,
};

uint8_t w_out[3 * 1] = {
    80,
    50,
    200,
};
    
uint8_t X_data[64 * 8] = {
    44, 90, 0, 0, 24, 81, 22, 0,
    159, 250, 140, 176, 121, 183, 138, 0,
    167, 158, 172, 134, 172, 161, 118, 0,
    130, 178, 136, 120, 135, 121, 86, 0,
    34, 112, 142, 112, 163, 159, 43, 0,
    63, 28, 145, 126, 190, 165, 66, 0,
    88, 214, 157, 140, 205, 174, 128, 0,
    185, 203, 170, 110, 211, 138, 126, 0,
    140, 255, 110, 136, 133, 156, 131, 0,
    73, 90, 164, 118, 75, 121, 111, 0,
    16, 93, 187, 88, 162, 101, 27, 0,
    31, 110, 120, 77, 166, 101, 45, 0,
    183, 181, 155, 175, 135, 211, 185, 0,
    129, 193, 104, 159, 184, 183, 161, 0,
    190, 183, 143, 124, 139, 138, 145, 0,
    79, 113, 115, 111, 164, 101, 88, 0,
    213, 213, 149, 159, 133, 199, 183, 0,
    13, 94, 127, 146, 132, 150, 89, 0,
    103, 175, 164, 139, 187, 147, 128, 0,
    42, 82, 104, 83, 167, 50, 71, 0,
    26, 90, 109, 143, 135, 220, 106, 0,
    69, 90, 101, 180, 125, 222, 108, 0,
    143, 225, 125, 147, 196, 197, 121, 0,
    73, 87, 125, 29, 8, 87, 67, 0,
    100, 136, 254, 119, 170, 140, 77, 0,
    110, 136, 101, 137, 186, 174, 126, 0,
    165, 143, 179, 151, 167, 156, 147, 0,
    182, 207, 131, 105, 130, 101, 124, 0,
    34, 51, 194, 94, 90, 94, 58, 0,
    108, 85, 116, 25, 24, 0, 59, 0,
    18, 70, 110, 118, 179, 138, 50, 0,
    35, 165, 120, 72, 126, 72, 82, 0,
    72, 99, 85, 76, 210, 101, 56, 0,
    27, 85, 85, 109, 122, 133, 54, 0,
    149, 166, 176, 111, 135, 115, 98, 0,
    138, 180, 136, 131, 255, 139, 84, 0,
    120, 97, 115, 96, 110, 128, 44, 0,
    183, 181, 183, 152, 119, 174, 148, 0,
    38, 113, 125, 67, 88, 26, 68, 0,
    180, 188, 169, 137, 188, 124, 145, 0,
    86, 87, 80, 72, 76, 73, 71, 0,
    219, 224, 155, 165, 197, 252, 218, 0,
    31, 65, 145, 38, 112, 32, 78, 0,
    21, 134, 48, 83, 94, 78, 111, 0,
    111, 62, 128, 89, 163, 209, 65, 0,
    224, 206, 128, 155, 167, 170, 150, 0,
    231, 225, 139, 174, 149, 202, 208, 0,
    140, 146, 106, 124, 192, 142, 104, 0,
    116, 191, 196, 136, 192, 170, 108, 0,
    43, 136, 131, 58, 44, 50, 118, 0,
    39, 76, 130, 63, 71, 62, 39, 0,
    103, 166, 170, 115, 236, 131, 75, 0,
    83, 148, 206, 120, 142, 156, 102, 0,
    73, 41, 150, 63, 123, 78, 51, 0,
    159, 136, 93, 153, 140, 149, 198, 0,
    14, 77, 160, 67, 68, 72, 56, 0,
    37, 70, 131, 53, 72, 46, 37, 0,
    225, 171, 136, 148, 136, 161, 188, 0,
    64, 177, 76, 69, 92, 92, 84, 0,
    44, 54, 166, 93, 158, 101, 59, 0,
    153, 170, 150, 125, 152, 171, 166, 0,
    86, 155, 136, 41, 36, 131, 63, 0,
    138, 160, 104, 119, 149, 124, 100, 0,
    19, 56, 140, 139, 217, 161, 51, 0,
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
    
// Aligning the receive buffer as well, preventing stack cache corruption
uint8_t mat_res[ROWS] __attribute__((aligned(64)));
uint8_t soft_res[ROWS] __attribute__((aligned(64)));
bool labels_res[ROWS];
bool soft_labels[ROWS];

XUartPs Uart_Ps;
XAxiDma DmaInstance; // Replaced XLlFifo
MyAccel MyAccelInstance;
XTmrCtr TimerInstance;

int main(void) {
    uint32_t timestamps[4];
    init();
    
    myaccel_upload_w_hid(&MyAccelInstance, w_hid);
    myaccel_upload_w_out(&MyAccelInstance, w_out);
    
    timestamps[0] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    dma_receive_matrix(mat_res, ARRAY_LENGTH(mat_res));
    dma_send_matrix(X_data, ARRAY_LENGTH(X_data));

    while(XAxiDma_Busy(&DmaInstance, XAXIDMA_DEVICE_TO_DMA));
    while(XAxiDma_Busy(&DmaInstance, XAXIDMA_DMA_TO_DEVICE));

    Xil_DCacheInvalidateRange((UINTPTR)mat_res, ARRAY_LENGTH(mat_res) * WORD_SIZE);

    timestamps[1] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);

    bool error = false;
    for(int i = 0; i < ROWS; i++) {
        labels_res[i] = mat_res[i] > 128;
        if(labels_res[i] != labels[i]) {
            xil_printf("Labels mismatch at index %i\r\n", i);
            error = true;
        }
    }

    uart_send_matrix(mat_res, ARRAY_LENGTH(mat_res));
    if(error) {
        xil_printf("Some hls values were incorrect\r\n");
    } else {
        xil_printf("ALL HLS TESTS PASSED\r\n");
    }

    timestamps[2] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    run_inference(X_data, soft_res, w_hid, w_out);
    timestamps[3] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    
    error = false;
    for(int i = 0; i < ROWS; i++) {
        soft_labels[i] = soft_res[i] > 128;
        if(soft_labels[i] != labels[i]) {
            xil_printf("Labels mismatch at index %i\r\n", i);
            error = true;
        }
    }
    
    uart_send_matrix(soft_res, ARRAY_LENGTH(soft_res));
    if(error) {
        xil_printf("Some soft values were incorrect\r\n");
    } else {
        xil_printf("ALL SOFTWARE TESTS PASSED\r\n");
    }

    xil_printf("Time for HLS implementation: %i\r\n", timestamps[1] - timestamps[0]);
    xil_printf("Time for software implementation: %i\r\n", timestamps[3] - timestamps[2]);
}

void init() {
    XUartPs_Config* uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
    if(uart_config == NULL) {
        while(1);
    }
    s32 status = XUartPs_CfgInitialize(&Uart_Ps, uart_config, uart_config->BaseAddress);
    if(status != XST_SUCCESS) {
        while(1);
    }
    
	XAxiDma_Config* dma_config = XAxiDma_LookupConfig(XPAR_XAXIDMA_0_BASEADDR);
	if (!dma_config) {
		xil_printf("No config found for %d\r\n", XPAR_XAXIDMA_0_BASEADDR);
	}

	if (XAxiDma_HasSg(&DmaInstance)) {
		xil_printf("Device configured as SG mode \r\n");
	}
    status = XAxiDma_CfgInitialize(&DmaInstance, dma_config);
    if(status != XST_SUCCESS) {
        xil_printf("Failed to initialise DMA, error code %i\r\n", status);
        while(1);
    }

	/* Disable interrupts, we use polling mode
	 */
	XAxiDma_IntrDisable(&DmaInstance, XAXIDMA_IRQ_ALL_MASK,
			    XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&DmaInstance, XAXIDMA_IRQ_ALL_MASK,
			    XAXIDMA_DMA_TO_DEVICE);

    MyAccelInstance.baseaddress = (void*)XPAR_MYACCEL_0_BASEADDR;

    XTmrCtr_Config* timer_config = XTmrCtr_LookupConfig(XPAR_XTMRCTR_0_BASEADDR);
    if(timer_config == NULL) {
        xil_printf("Failed to acquire timer config");
        while(1);
    }
    XTmrCtr_CfgInitialize(&TimerInstance, timer_config, timer_config->BaseAddress);
    XTmrCtr_Start(&TimerInstance, TIMER_COUNTER_0);

    xil_printf("Successfully initialized\r\n");
}

void uart_receive_matrix(uint8_t* matrix, size_t len) {
    for(size_t i = 0; i < len; i++) {
        char str[16];
        for(size_t j = 0; j < 4; j++) {
            int received = 0;
            while (!received){
                received = XUartPs_Recv(&Uart_Ps, (unsigned char*)&str[j], 1);
            }
            if(str[j] == '\n' || str[j] == ',') {
                str[j] = '\0';
                break;
            }
        }
        matrix[i] = atoi(str);
    }
}

void uart_send_matrix(uint8_t *matrix, size_t len) {
    for (size_t i = 0; i < len; i++) {
        xil_printf("%i\r\n", (int)matrix[i]);
    }
}

void dma_send_matrix(uint8_t *matrix, size_t len) {
    size_t bytes = len * WORD_SIZE;
    
    // Flush ONLY the specific range we are sending
    Xil_DCacheFlushRange((UINTPTR)matrix, bytes);
    
    s32 status = XAxiDma_SimpleTransfer(&DmaInstance, (UINTPTR)matrix, bytes, XAXIDMA_DMA_TO_DEVICE);
    if (status != XST_SUCCESS) return;
}

void dma_receive_matrix(uint8_t *matrix, size_t len) {
    size_t bytes = len * WORD_SIZE;
    
    // Invalidate the range BEFORE the transfer starts 
    // to ensure the CPU doesn't try to speculatively pull in old data
    Xil_DCacheInvalidateRange((UINTPTR)matrix, bytes);
    
    // FIX: Direction must be DEVICE_TO_DMA
    s32 status = XAxiDma_SimpleTransfer(&DmaInstance, (UINTPTR)matrix, bytes, XAXIDMA_DEVICE_TO_DMA);
    if (status != XST_SUCCESS) return;
}