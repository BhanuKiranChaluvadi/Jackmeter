#include "SimpleFrequencyProcessor.hpp"
#include <fftw3.h>
#include <math.h>
#include <string_view>

namespace jackmeter {
SimpleFrequencyProcessor::SimpleFrequencyProcessor(std::string_view name)
    : m_name(std::string(name))
{
}

void SimpleFrequencyProcessor::Process(float* samples, uint32_t nSamples, uint32_t sampleRate)
{

    // printf all the input arguments
    // printf("sample 0: %f\n", samples[0]);
    // printf("nSamples: %d\n", nSamples);
    // printf("sampleRate: %d\n", sampleRate);

    if (!spectroData) {
        spectroData = std::make_unique<StreamData>();
    }

    // Allocate memory for in and out buffers
    spectroData->in = std::make_unique<double[]>(nSamples);
    spectroData->out = std::make_unique<double[]>(nSamples);

    // Setup FFT plan
    spectroData->p = fftw_plan_r2r_1d(
        nSamples, spectroData->in.get(), spectroData->out.get(),
        FFTW_R2HC, FFTW_ESTIMATE);

    double sampleRatio = static_cast<double>(nSamples) / sampleRate; // 640 / 48000 = 0.0133333333333333
    spectroData->startIndex = static_cast<int>(std::ceil(sampleRatio * SPECTRO_FREQ_START)); // 0.0133333333333333 * 20 = 0.2666666666666666 -> 1
    spectroData->spectroSize = static_cast<int>(std::min(
                                                    std::ceil(sampleRatio * SPECTRO_FREQ_END),
                                                    static_cast<double>(nSamples) / 2.0) // 0.0133333333333333 * 20000 = 266.6666666666666 -> 267, 640 / 2 = 320 -> 267
        - spectroData->startIndex); // 267 - 1 = 266

    // Copy samples into input buffer
    for (uint32_t i = 0; i < nSamples; ++i) {
        spectroData->in[i] = static_cast<double>(samples[i]);
    }

    // Execute FFT
    fftw_execute(spectroData->p);

    /*
    // Set our spectrogram size in the terminal to 100 characters, and move the
    // cursor to the beginning of the line
    int dispSize = 100;
    printf("\r");

    // Draw the spectrogram
    for (int i = 0; i < dispSize; i++) {
        // Sample frequency data logarithmically
        double proportion = std::pow(i / (double)dispSize, 4);
        double freq = spectroData->out[(int)(spectroData->startIndex
            + proportion * spectroData->spectroSize)];

        freq = std::abs(freq)/100.0;

        // print the magnitude of the frequency
        // printf("magnitude: %f ", freq);

        if (freq < 0.125) {
            printf("▁");
        } else if (freq < 0.25) {
            printf("▂");
        } else if (freq < 0.375) {
            printf("▃");
        } else if (freq < 0.5) {
            printf("▄");
        } else if (freq < 0.625) {
            printf("▅");
        } else if (freq < 0.75) {
            printf("▆");
        } else if (freq < 0.875) {
            printf("▇");
        } else {
            printf("█");
        }
    }
    */
}

const StreamData& SimpleFrequencyProcessor::GetSpectroData() const
{
    if (!spectroData) {
        throw std::runtime_error("SpectroData has not been initialized.");
    }
    return *spectroData;
}

bool SimpleFrequencyProcessor::SignalDetected() const
{
    return m_signalDetected;
}

std::string_view SimpleFrequencyProcessor::GetName()
{
    return m_name;
}
} // namespace jackmeter