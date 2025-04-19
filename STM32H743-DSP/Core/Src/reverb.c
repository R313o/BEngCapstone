/*
 * reverb.c
 *
 *  Created on: Mar 4, 2025
 *      Author: spencer
 */


#include "reverb.h"
#include <string.h>

// In this implementation we use an FFT size of 2*BUFFER_SIZE.
// The valid output from the inverse FFT is the last BUFFER_SIZE samples.
void reverbInit(reverb *self, float32_t mix, const float32_t *ir, uint32_t irLength) {
    // Limit the IR length to MAX_IR_LENGTH.
    if (irLength > MAX_IR_LENGTH) {
        irLength = MAX_IR_LENGTH;
    }
    self->mix = mix;
    self->ir = ir;
    self->irLength = irLength;

    // Initialize the FFT instance.
    arm_rfft_fast_init_f32(&self->fftInstance, FFT_SIZE);

    // Prepare a temporary buffer for IR processing.
    float32_t temp[FFT_SIZE] = {0};
    // Copy the IR into temp; if irLength < FFT_SIZE, the rest is zero.
    memcpy(temp, self->ir, irLength * sizeof(float32_t));
    if (irLength < FFT_SIZE) {
        memset(temp + irLength, 0, (FFT_SIZE - irLength) * sizeof(float32_t));
    }
    // Compute and store the FFT of the IR.
    arm_rfft_fast_f32(&self->fftInstance, temp, self->irFFT, 0);

    // Initialize the overlap buffer to zero.
    memset(self->overlap, 0, sizeof(self->overlap));
}

void reverbApply(reverb *self, pipe *p) {
    // dry: current input block of length BUFFER_SIZE.
    float32_t dry[BUFFER_SIZE];
    memcpy(dry, p->processBuffer, BUFFER_SIZE * sizeof(float32_t));

    // Form the input window X by concatenating the saved overlap (length FFT_SIZE - BUFFER_SIZE)
    // with the current input block (length BUFFER_SIZE) to create an array X of length FFT_SIZE.
    float32_t X[FFT_SIZE];
    memcpy(X, self->overlap, (FFT_SIZE - BUFFER_SIZE) * sizeof(float32_t));
    memcpy(X + (FFT_SIZE - BUFFER_SIZE), dry, BUFFER_SIZE * sizeof(float32_t));

    // Compute FFT of X.
    float32_t X_fft[FFT_SIZE];
    arm_rfft_fast_f32(&self->fftInstance, X, X_fft, 0);

    // Multiply X_fft by the precomputed IR FFT (frequency–domain multiplication).
    // The FFT outputs are in packed format:
    // X_fft[0] is DC, X_fft[1] is Nyquist, then pairs: real and imaginary.
    float32_t Y_fft[FFT_SIZE];
    // DC component:
    Y_fft[0] = X_fft[0] * self->irFFT[0];
    // Nyquist:
    Y_fft[1] = X_fft[1] * self->irFFT[1];
    uint32_t half = FFT_SIZE / 2;
    for (uint32_t k = 1; k < half; k++) {
        uint32_t idx = 2 * k;
        float32_t a = X_fft[idx];
        float32_t b = X_fft[idx + 1];
        float32_t c = self->irFFT[idx];
        float32_t d = self->irFFT[idx + 1];
        // Complex multiplication: (a+jb)*(c+jd) = (ac - bd) + j(ad + bc)
        Y_fft[idx]     = a * c - b * d;
        Y_fft[idx + 1] = a * d + b * c;
    }

    // Compute inverse FFT to get the time-domain convolution result.
    float32_t Y[FFT_SIZE];
    arm_rfft_fast_f32(&self->fftInstance, Y_fft, Y, 1);
    // Scale the inverse FFT output (CMSIS does not auto-scale).
    for (uint32_t n = 0; n < FFT_SIZE; n++) {
        Y[n] /= FFT_SIZE;
    }

    // In overlap–save, the first (FFT_SIZE - BUFFER_SIZE) samples of Y are discarded.
    // The valid output is the last BUFFER_SIZE samples.
    float32_t wet[BUFFER_SIZE];
    memcpy(wet, Y + (FFT_SIZE - BUFFER_SIZE), BUFFER_SIZE * sizeof(float32_t));

    // Update the overlap buffer with the current block's first (FFT_SIZE - BUFFER_SIZE) samples
    // from X (the input window). This is needed for the next block.
    memcpy(self->overlap, X, (FFT_SIZE - BUFFER_SIZE) * sizeof(float32_t));

    // Mix dry and wet signals.
    for (uint32_t n = 0; n < BUFFER_SIZE; n++) {
        p->processBuffer[n] = (1.0f - self->mix) * dry[n] + self->mix * wet[n];
    }
}
