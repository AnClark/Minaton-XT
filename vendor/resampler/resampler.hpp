/*
 * Simple Resampler
 * https://github.com/cpuimage/resampler.git
 *
 * License: MIT
 */

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

constexpr double getExpectedInputSize(float inSampleRate, float outSampleRate, uint32_t targetOutSize)
{
    return (inSampleRate / outSampleRate) * targetOutSize;
}

uint64_t Resample_f32(const float* input, float* output, int inSampleRate, int outSampleRate, uint64_t inputSize,
    uint32_t channels);

uint64_t Resample_f32(const float* input, float* output, int inSampleRate, int outSampleRate, double inputSize,
    uint32_t channels);

uint64_t Resample_s16(const int16_t* input, int16_t* output, int inSampleRate, int outSampleRate, uint64_t inputSize,
    uint32_t channels);
