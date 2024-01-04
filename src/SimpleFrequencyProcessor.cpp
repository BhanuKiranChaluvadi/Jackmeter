#include "SimpleFrequencyProcessor.hpp"
#include <fftw3.h>
#include <math.h>
#include <string_view>

namespace jackmeter {
SimpleFrequencyProcessor::SimpleFrequencyProcessor(std::string_view name)
    : m_name(std::string(name))
    , m_latestFrequency(0.0f)
{
}

void SimpleFrequencyProcessor::Process(float* samples, uint32_t nSamples, uint32_t sampleRate)
{
    // Perform Fourier transform
    fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * nSamples);
    fftw_plan p = fftw_plan_dft_r2c_1d(nSamples, reinterpret_cast<double*>(samples), out, FFTW_ESTIMATE);
    fftw_execute(p);

    // Find frequency with highest magnitude
    
    double maxMagnitude = 0.0;
    double maxFrequency = 0.0;
    for (uint32_t i = 0; i < nSamples / 2 + 1; i++) {
        double magnitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        if (magnitude > maxMagnitude) {
            maxMagnitude = magnitude;
            maxFrequency = (double)i * sampleRate / nSamples; // Calculate frequency
        }
    }

    // Update min, max, and latest frequency
    m_latestFrequency = maxFrequency;
    if (maxFrequency < m_minFrequency) {
        m_minFrequency = maxFrequency;
    }
    if (maxFrequency > m_maxFrequency) {
        m_maxFrequency = maxFrequency;
    }
    
    // Clean up
    fftw_destroy_plan(p);
    fftw_free(out);
}

bool SimpleFrequencyProcessor::SignalDetected() const
{
    return m_signalDetected;
}

float SimpleFrequencyProcessor::GetLatestPeak()
{
    return m_latestFrequency.load();
}

float SimpleFrequencyProcessor::GetLatestPeakDb()
{
    return 20.0f * log10f(m_latestFrequency.load());
}

float SimpleFrequencyProcessor::GetMinPeak()
{
    return m_minFrequency.load();
}

float SimpleFrequencyProcessor::GetMinPeakDb()
{
    return 20.0f * log10f(m_minFrequency.load());
}

float SimpleFrequencyProcessor::GetMaxPeak()
{
    return m_maxFrequency.load();
}

float SimpleFrequencyProcessor::GetMaxPeakDb()
{
    return 20.0f * log10f(m_maxFrequency.load());
}

std::string_view SimpleFrequencyProcessor::GetName()
{
    return m_name;
}
} // namespace jackmeter