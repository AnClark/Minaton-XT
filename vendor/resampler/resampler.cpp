/*
 * Simple Resampler
 * https://github.com/cpuimage/resampler.git
 *
 * License: MIT
 */

#include "resampler.hpp"

#include <stdio.h>

uint64_t Resample_f32(const float* input, float* output, int inSampleRate, int outSampleRate, uint64_t inputSize,
    uint32_t channels)
{
    if (input == NULL)
        return 0;
    uint64_t outputSize = (uint64_t)(inputSize * (double)outSampleRate / (double)inSampleRate);
    outputSize -= outputSize % channels;
    if (output == NULL)
        return outputSize;
    double stepDist = ((double)inSampleRate / (double)outSampleRate);
    const uint64_t fixedFraction = (1LL << 32);
    const double normFixed = (1.0 / (1LL << 32));
    uint64_t step = ((uint64_t)(stepDist * fixedFraction + 0.5));
    uint64_t curOffset = 0;
    for (uint32_t i = 0; i < outputSize; i += 1) {
        for (uint32_t c = 0; c < channels; c += 1) {
            *output++ = (float)(input[c] + (input[c + channels] - input[c]) * ((double)(curOffset >> 32) + ((curOffset & (fixedFraction - 1)) * normFixed)));
        }
        curOffset += step;
        input += (curOffset >> 32) * channels;
        curOffset &= (fixedFraction - 1);
    }
    return outputSize;
}

uint64_t Resample_f32(const float* input, float* output, int inSampleRate, int outSampleRate, double inputSize,
    uint32_t channels, uint32_t maxOutputSize)
{
    if (input == NULL)
        return 0;

    /**
     * POLYFILL:
     * C/C++ has float decision issues.
     * For example, (44100.0f / 96000.0f * 512) should equal 235.2, but in C/C++ it equals 235.199997.
     *
     * Since audio sample rates are canonical values, calculating input/output buffer size will get
     * a number with only 1 decimal place. So simply round it.
     *
     * NOTICE: inputSize should always be a value calculated by getExpectedInputSize(),
     *         and make sure you have enough space for output buffer,
     *         otherwise you may encounter unexpected behaviors (out-of-range, stitches, etc.)!
     */
    uint64_t outputSize = (uint64_t)round(inputSize * (double)outSampleRate / (double)inSampleRate);
    outputSize -= outputSize % channels;
    if (output == NULL)
        return outputSize;

    // Prevent out-of-range exceptions
    outputSize = (outputSize > maxOutputSize) ? maxOutputSize : outputSize;

    double stepDist = ((double)inSampleRate / (double)outSampleRate);
    const uint64_t fixedFraction = (1LL << 32);
    const double normFixed = (1.0 / (1LL << 32));
    uint64_t step = ((uint64_t)(stepDist * fixedFraction + 0.5));
    uint64_t curOffset = 0;
    for (uint32_t i = 0; i < outputSize; i += 1) {
        for (uint32_t c = 0; c < channels; c += 1) {
            *output++ = (float)(input[c] + (input[c + channels] - input[c]) * ((double)(curOffset >> 32) + ((curOffset & (fixedFraction - 1)) * normFixed)));
        }
        curOffset += step;
        input += (curOffset >> 32) * channels;
        curOffset &= (fixedFraction - 1);
    }
    return outputSize;
}

uint64_t Resample_s16(const int16_t* input, int16_t* output, int inSampleRate, int outSampleRate, uint64_t inputSize,
    uint32_t channels)
{
    if (input == NULL)
        return 0;
    uint64_t outputSize = (uint64_t)(inputSize * (double)outSampleRate / (double)inSampleRate);
    outputSize -= outputSize % channels;
    if (output == NULL)
        return outputSize;
    double stepDist = ((double)inSampleRate / (double)outSampleRate);
    const uint64_t fixedFraction = (1LL << 32);
    const double normFixed = (1.0 / (1LL << 32));
    uint64_t step = ((uint64_t)(stepDist * fixedFraction + 0.5));
    uint64_t curOffset = 0;
    for (uint32_t i = 0; i < outputSize; i += 1) {
        for (uint32_t c = 0; c < channels; c += 1) {
            *output++ = (int16_t)(input[c] + (input[c + channels] - input[c]) * ((double)(curOffset >> 32) + ((curOffset & (fixedFraction - 1)) * normFixed)));
        }
        curOffset += step;
        input += (curOffset >> 32) * channels;
        curOffset &= (fixedFraction - 1);
    }
    return outputSize;
}
