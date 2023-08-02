#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mic.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "kiss_fftr.h"
#include "usr_commands.h"

bool micInitialized = false;
uint micDmaChan;
dma_channel_config cfg;
float freqs[MIC_NSAMP];
uint8_t micBuf[MIC_NSAMP];

// KissFFT related
kiss_fft_scalar fftIn[MIC_NSAMP];
kiss_fft_cpx fftOut[MIC_NSAMP];
kiss_fftr_cfg fftCfg;

float Mic_GetPowerFromFFTValue(kiss_fft_cpx val)
{
	return val.i * val.i + val.r * val.r;
}

void Mic_Init(void)
{
	memset(freqs, 0x00, MIC_NSAMP * sizeof(float));

	// Init adc
	adc_init();
	adc_gpio_init(MIC_SIGNAL_ADC_PIN);
	adc_select_input(0);

	// Taken from the dma_capture.c example
	adc_fifo_setup(
		true,    // Write each completed conversion to the sample FIFO
		true,    // Enable DMA data request (DREQ)
		1,       // DREQ (and IRQ) asserted when at least 1 sample present
		false,   // We won't see the ERR bit because of 8 bit reads; disable.
		true     // Shift each sample to 8 bits when pushing to FIFO
	);

	// Divisor of 0 -> full speed. Free-running capture with the divider is
	// equivalent to pressing the ADC_CS_START_ONCE button once per `div + 1`
	// cycles (div not necessarily an integer). Each conversion takes 96
	// cycles, so in general you want a divider of 0 (hold down the button
	// continuously) or > 95 (take samples less frequently than 96 cycle
	// intervals). This is all timed by the 48 MHz ADC clock.
	//
	// https://www.hackster.io/AlexWulff/adc-sampling-and-fft-on-raspberry-pi-pico-f883dd
	// "clkdiv" refers to clock divide, which allows you to split the 48 MHz base clock to sample at a lower rate. 
	// Currently, one sample takes 96 cycles to collect. 
	// This yields a maximum sample rate of 48, 000, 000 cycles per second / 96 cycles per sample = 500, 000 
	// samples per second.
	//
	adc_set_clkdiv(MIC_CLOCK_DIV);
	sleep_ms(1000);

	// Set up the DMA to start transferring data as soon as it appears in FIFO
	micDmaChan = dma_claim_unused_channel(true);
	cfg = dma_channel_get_default_config(micDmaChan);

	// Reading from constant address, writing to incrementing byte addresses
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
	channel_config_set_read_increment(&cfg, false);
	channel_config_set_write_increment(&cfg, true);

	// Pace transfers based on availability of ADC samples
	channel_config_set_dreq(&cfg, DREQ_ADC);

	// Calculate frequencies of each bin
	float fMax = MIC_FSAMP;
	float fRes = fMax / MIC_NSAMP;
	for (uint16_t i = 0; i < MIC_NSAMP; i++)
	{
		freqs[i] = fRes * i;
	}

	fftCfg = kiss_fftr_alloc(MIC_NSAMP, false, 0, 0);

	micInitialized = true;
	printf("Mic Initialized!\n");
}

void Mic_DoFFT(void)
{
	// fill fft input while subtracting DC component
	uint64_t sum = 0; 
	for (uint16_t i = 0; i < MIC_NSAMP; i++)
	{
		sum += micBuf[i];
	}

	float avg = (float) sum / MIC_NSAMP;
	for (uint16_t i = 0; i < MIC_NSAMP; i++)
	{
		fftIn[i] = (float) micBuf[i] - avg;
	}

	// compute fft
	kiss_fftr(fftCfg, fftIn, fftOut);

	// compute power and calculate max freq component
	float maxPower = 0;
	uint32_t maxIdx = 0;
	for (uint32_t i = 0; i < MIC_NSAMP / 2; i++) // any frequency bin over MIC_NSAMP/2 is aliased (nyquist sampling theorem)
	{
		float power = fftOut[i].r * fftOut[i].r + fftOut[i].i * fftOut[i].i;
		if (power > maxPower)
		{
			maxPower = power;
			maxIdx = i;
		}
	}

	float maxFreq = freqs[maxIdx];
	// printf("Greatest freq component: %0.1f Hz max power: %0.1f | spec: ", maxFreq, maxPower/10000);
	for (uint32_t i = 0; i < 10; i++)
	{
		uint32_t idx = i * 25;
		printf("  %4.1lf ", Mic_GetPowerFromFFTValue(fftOut[idx]) / 100);
	}
	printf(" |             \r");
} 

void Mic_StartDMASampling(void)
{
	// get MIC_NSAMP samples at MIC_FSAMP
	adc_fifo_drain();
	adc_run(false);
	dma_channel_configure(micDmaChan, &cfg,
											 micBuf,    // dst
											 &adc_hw->fifo,  // src
											 MIC_NSAMP,          // transfer count
											 true            // start immediately
	);

	// gpio_put(LED_PIN, 1);
	adc_run(true);
	dma_channel_wait_for_finish_blocking(micDmaChan);
	// gpio_put(LED_PIN, 0);
}

float Mic_DoSingleSample(void)
{
	const float conversion_factor = 3.3f / (1 << 12);
	uint16_t raw = adc_read();
	float result = raw * conversion_factor;
	return result;
}

uint16_t Mic_TestPoll(void)
{
	uint16_t raw = adc_read();
	printf("Raw value: 0x%08x, convd: %f\n", raw);

	uint32_t sum = 0;
	uint32_t avg = 0;
	uint16_t min = 0xffff;
	uint16_t max = 0x0000;
	uint32_t peekToPeek = 0;
	uint8_t runs = 200;
	for (uint16_t i = 0; i < runs; i++)
	{
		uint16_t raw = adc_read();
		sum += adc_read();
		if (min > raw)
		{
			min = raw;
		}
		if (max < raw)
		{
			max = raw;
		}
		sleep_ms(50);
	}
	peekToPeek = max - min;
	avg = sum / runs;
	double micOutVolts = (peekToPeek * 3.3) / (0xfff);
	printf("peekToPeek: %d, micOutVolts: %f\n", peekToPeek, micOutVolts);
	return raw;
}

float *Mic_GetBufPtr(void)
{
	return (float *) &micBuf;
}

void Mic_ResetBuffer(void)
{
	memset(freqs, 0x00, MIC_NSAMP * sizeof(float));
}

void Mic_TakeUsrCommand(uint8_t argc, char **argv)
{
	if (!micInitialized)
	{
		return;
	}
	ASSERT_ARGS(2);
	if (strcmp(argv[1], "sample") == 0)
	{
		float samp = Mic_DoSingleSample();
		printf("Sampled: %f\n", samp);
	}
	else if (strcmp(argv[1], "test") == 0)
	{
		Mic_TestPoll();
	}
	else if (strcmp(argv[1], "dma") == 0)
	{
		Mic_StartDMASampling();
		uint16_t *micBufPtr = (uint16_t *) Mic_GetBufPtr();
		const float conversion_factor = 3.3f / (1 << 12);
		for (uint16_t i = 0; i < 200; i++)
		{
			printf("Sample %d : %x %f\n", i, micBufPtr[i], micBufPtr[i] * conversion_factor);
		}
	}
}

