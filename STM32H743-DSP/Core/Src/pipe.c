/*
 * pipe.c
 *
 *  Created on: Feb 19, 2025
 *      Author: spencer
 */

 #include "pipe.h"

static void pipe_ADC_HalfComplete(pipe *self, const volatile uint16_t *adcInput)
{
    uint16_t i;
    //load values from adc dma buffer
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        self->inBuffer1[i] = (float32_t)adcInput[i];
    }
    //convert values from quantized value to voltage range float
    arm_scale_f32(self->inBuffer1, ADC_BITS2VOLTS, self->inBuffer1, BUFFER_SIZE);
    //remove DC offset
    arm_offset_f32(self->inBuffer1, -1.0f, self->inBuffer1, BUFFER_SIZE);
    self->inBuffer  = self->inBuffer1;
    self->outBuffer = self->outBuffer1;
    self->ppState   = 1;
    self->bufferReady = true;
}

static void pipe_ADC_Complete(pipe *self, const volatile uint16_t *adcInput)
{
    uint16_t i;
    //load values from adc dma buffer
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        self->inBuffer2[i] = (float32_t)adcInput[BUFFER_SIZE + i];
    }
    //convert values from quantized value to voltage range float
    arm_scale_f32(self->inBuffer2, ADC_BITS2VOLTS, self->inBuffer2, BUFFER_SIZE);
    //remove DC offset
    arm_offset_f32(self->inBuffer2, -1.0f, self->inBuffer2, BUFFER_SIZE);
    self->inBuffer  = self->inBuffer2;
    self->outBuffer = self->outBuffer2;
    self->ppState   = 0;
    self->bufferReady = true;
}

static void pipe_updateDelayBuffer(pipe *self)
{

    arm_copy_f32(self->inBuffer, &self->delayBuffer[self->delayIndex], BUFFER_SIZE);
    self->delayIndex += BUFFER_SIZE;
    if (self->delayIndex >= DELAY_BUFFER_SIZE)
    {
        self->delayIndex = 0;
    }
}

float32_t *pipe_getDelayBuffer(pipe *self, uint16_t n)
{
    uint32_t offset = n * BUFFER_SIZE;

    uint32_t index = (self->delayIndex + DELAY_BUFFER_SIZE - offset) % DELAY_BUFFER_SIZE;

    return &self->delayBuffer[index];
}

static void pipe_updateDACOutput(pipe *self, volatile uint16_t *dacBuffer)
{
    uint16_t i;

    uint8_t half = (self->ppState == 0) ? 1 : 0;
    //add dc offset
    arm_offset_f32(self->outBuffer, 1.0f, self->outBuffer, BUFFER_SIZE);
    //convert from voltage range to quantized dac value
    arm_scale_f32(self->outBuffer, DAC_VOLTS2BITS, self->outBuffer, BUFFER_SIZE);
    //load dac dma buffer from output buffer
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        dacBuffer[BUFFER_SIZE * half + i] = (uint16_t)self->outBuffer[i];
    }
}

static void pipe_loadProcess(pipe *self)
{
	arm_copy_f32(self->inBuffer, self->processBuffer, BUFFER_SIZE);
}

void pipeInit(pipe *self)
{
    self->inBuffer  = self->inBuffer2;
    self->outBuffer = self->outBuffer2;
    self->ppState   = 0;
    self->delayIndex = 0;
    self->bufferReady = false;

    self->adcHalfComplete = pipe_ADC_HalfComplete;
    self->adcComplete     = pipe_ADC_Complete;
    self->updateDelayBuffer = pipe_updateDelayBuffer;
    self->updateDACOutput = pipe_updateDACOutput;
    self->getDelayBuffer = pipe_getDelayBuffer;
    self->loadProcess = pipe_loadProcess;

    for(int i = 0; i < BUFFER_SIZE ; i++) {

    	self->processBuffer[i] = 0;

    }

}


