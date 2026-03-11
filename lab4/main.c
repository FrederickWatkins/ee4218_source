#include "xil_cache.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xllfifo.h"
#include "xparameters.h"
#include "xstatus.h"
#include "xstreamer.h"
#include "xuartps.h"
#include "xtmrctr.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ROWS 64 // Parametrizable
#define COLS 8

#define TIMER_COUNTER_0	 0

#define WORD_SIZE 4 // Size of words in bytes

void init();
void uart_receive_matrix(uint32_t* matrix, size_t len);
void uart_send_matrix(uint32_t* matrix, size_t len);
void fifo_send_matrix(uint32_t* matrix, size_t len);
void fifo_receive_matrix(uint32_t* matrix, size_t len);

uint32_t mat_send[ROWS * COLS + COLS] = {
    160,	129,	104,	71,	184,	193,	55,	86,
39,	31,	145,	150,	32,	65,	90,	45,
153,	159,	93,	24,	149,	136,	108,	61,
133,	73,	183,	124,	151,	39,	68,	67,
91,	24,	115,	144,	125,	0,	255,	42,
148,	92,	126,	71,	175,	204,	100,	156,
170,	149,	168,	58,	198,	192,	48,	86,
91,	79,	47,	91,	104,	71,	146,	75,
64,	73,	151,	145,	78,	41,	78,	39,
96,	12,	126,	98,	177,	40,	118,	78,
26,	108,	116,	129,	0,	85,	54,	92,
135,	167,	172,	73,	161,	159,	58,	64,
116,	104,	171,	124,	132,	166,	108,	72,
57,	64,	157,	177,	83,	71,	156,	22,
146,	14,	127,	144,	151,	94,	0,	45,
138,	111,	101,	84,	175,	136,	48,	103,
137,	140,	111,	45,	157,	255,	98,	75,
161,	156,	172,	124,	170,	189,	110,	125,
69,	64,	77,	98,	92,	178,	136,	67,
151,	181,	181,	82,	175,	152,	95,	89,
104,	90,	177,	106,	121,	128,	85,	89,
61,	25,	71,	117,	129,	53,	63,	45,
255,	34,	255,	236,	192,	36,	60,	136,
95,	2,	118,	98,	124,	85,	115,	50,
166,	219,	156,	82,	253,	225,	43,	120,
92,	184,	107,	84,	120,	153,	83,	67,
43,	41,	119,	98,	57,	55,	68,	117,
137,	117,	144,	131,	147,	165,	88,	83,
117,	73,	127,	91,	142,	149,	33,	92,
89,	111,	129,	137,	210,	63,	75,	86,
83,	60,	214,	124,	60,	49,	136,	92,
140,	19,	141,	137,	162,	57,	15,	33,
86,	12,	114,	157,	83,	94,	93,	106,
156,	225,	129,	98,	170,	207,	141,	97,
118,	18,	111,	137,	138,	70,	43,	42,
100,	138,	116,	71,	143,	201,	88,	72,
148,	41,	47,	58,	173,	140,	108,	22,
177,	159,	141,	11,	184,	251,	141,	58,
96,	0,	88,	111,	122,	65,	173,	45,
149,	226,	137,	112,	162,	172,	100,	100,
160,	214,	149,	61,	200,	214,	90,	117,
136,	116,	197,	116,	170,	191,	141,	103,
124,	141,	107,	8,	143,	146,	90,	83,
73,	86,	81,	55,	74,	88,	93,	184,
64,	40,	130,	161,	63,	76,	161,	56,
58,	43,	131,	157,	51,	136,	38,	42,
180,	167,	145,	45,	255,	188,	80,	114,
117,	135,	101,	65,	120,	137,	123,	78,
83,	6,	79,	111,	116,	80,	43,	61,
137,	23,	116,	98,	161,	85,	183,	50,
152,	184,	183,	157,	175,	182,	125,	86,
120,	110,	203,	65,	138,	152,	100,	111,
191,	255,	153,	78,	203,	213,	136,	106,
0,	44,	0,	0,	81,	90,	90,	50,
83,	21,	48,	71,	78,	135,	125,	45,
118,	73,	164,	99,	122,	90,	125,	78,
123,	175,	183,	117,	170,	184,	141,	173,
111,	185,	149,	53,	129,	226,	83,	72,
176,	184,	156,	80,	212,	181,	50,	106,
146,	183,	186,	124,	157,	220,	95,	139,
96,	120,	115,	111,	129,	97,	118,	255,
126,	200,	181,	86,	175,	185,	110,	125,
111,	80,	115,	108,	101,	114,	118,	45,
141,	144,	146,	66,	157,	216,	88,	159,
75,
66,
57,
0,
1,
46,
14,
37
};

XUartPs Uart_Ps;
XLlFifo FifoInstance;
XTmrCtr TimerInstance;

int main(void) {
    uint32_t timestamps[6];
    uint32_t mat_res[ROWS];
    init();
    timestamps[0] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    timestamps[1] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    fifo_send_matrix(mat_send, ARRAY_LENGTH(mat_send));
    timestamps[2] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    fifo_receive_matrix(mat_res, ARRAY_LENGTH(mat_res));
    timestamps[3] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    timestamps[4] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    uart_send_matrix(mat_res, ARRAY_LENGTH(mat_res));
    timestamps[5] = XTmrCtr_GetValue(&TimerInstance, TIMER_COUNTER_0);
    xil_printf("Cycles taken to read matrices from uart: %i\r\n", timestamps[1] - timestamps[0]);
    xil_printf("Cycles taken to write matrices to fifo: %i\r\n", timestamps[2] - timestamps[1]);
    xil_printf("Cycles taken to read matrices from fifo: %i\r\n", timestamps[3] - timestamps[2]);
    xil_printf("Cycles taken to calculate matrix multiplication: %i\r\n", timestamps[4] - timestamps[3]);
    xil_printf("Cycles taken to write matrices to uart: %i\r\n", timestamps[5] - timestamps[4]);
}

void init() {
    XUartPs_Config* uart_config = XUartPs_LookupConfig(XPAR_XUARTPS_0_BASEADDR);
    if(uart_config == NULL) {
        while(1);
    }
    s32 status = XUartPs_CfgInitialize(&Uart_Ps, uart_config, uart_config->BaseAddress);
    if(status != XST_SUCCESS) {
        while(1); // We didn't successfully init uart, nowhere to print error message to
    }
    XLlFifo_Config* fifo_config = XLlFfio_LookupConfig(XPAR_XLLFIFO_0_BASEADDR);
    if(fifo_config == NULL) {
        xil_printf("Failed to acquire fifo config");
        while(1);
    }
    status = XLlFifo_CfgInitialize(&FifoInstance, fifo_config, fifo_config->BaseAddress);
    if(status != XST_SUCCESS) {
        xil_printf("Failed to initialise fifo, error code %i\r\n", status);
        while(1);
    }
    XTmrCtr_Config* timer_config = XTmrCtr_LookupConfig(XPAR_XTMRCTR_0_BASEADDR);
    if(timer_config == NULL) {
        xil_printf("Failed to acquire timer config");
        while(1);
    }
    XTmrCtr_CfgInitialize(&TimerInstance, timer_config, timer_config->BaseAddress);
	XTmrCtr_Start(&TimerInstance, TIMER_COUNTER_0);
    xil_printf("Successfully initialized\r\n");
}

void uart_receive_matrix(uint32_t* matrix, size_t len) {
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

void uart_send_matrix(uint32_t *matrix, size_t len) {
    for (size_t i = 0; i < len; i++) {
        xil_printf("%i\r\n", matrix[i]);
    }
}

void fifo_send_matrix(uint32_t *matrix, size_t len) {
    for(size_t i = 0; i < len; i++) {
        XLlFifo_TxPutWord(&FifoInstance, matrix[i]);
    }
    XLlFifo_iTxSetLen(&FifoInstance, (len * WORD_SIZE));
    while(!(XLlFifo_IsTxDone(&FifoInstance)));
}

void fifo_receive_matrix(uint32_t *matrix, size_t len) {
    while(XLlFifo_iRxOccupancy(&FifoInstance) == 0);
    xil_printf("%i bytes received\r\n", XLlFifo_iRxGetLen(&FifoInstance));
    for(size_t i = 0; i < len; i++) {
        matrix[i] = XLlFifo_RxGetWord(&FifoInstance);
    }
}
