#include <stdint.h>
#include <stdbool.h>

#define MIC_SIGNAL_ADC_PIN 26
#define MIC_BITRATE_KHZ 192
#define MIC_BUFSIZE_FLOATS 1000

//
// clk div
// 0    = 500 kHz
// 960  = 50 kHz
// 9600 = 5 kHz
#define MIC_CLOCK_DIV 960 // 50kHz
#define MIC_FSAMP 50000
#define MIC_NSAMP 2000

void Mic_Init(void);
uint16_t Mic_TestPoll(void);
void Mic_StartDMASampling(void);
float *Mic_GetBufPtr(void);
void Mic_TakeUsrCommand(uint8_t argc, char **argv);
float Mic_DoSingleSample(void);
void Mic_ResetBuffer(void);
void Mic_DoFFT(void);
