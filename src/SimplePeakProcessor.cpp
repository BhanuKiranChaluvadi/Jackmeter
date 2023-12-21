#include "SimplePeakProcessor.hpp"
#include <math.h>
#include <string_view>

namespace jackmeter {
SimplePeakProcessor::SimplePeakProcessor(std::string_view name)
    : m_name(std::string(name))
    , m_latestPeak(0.0f)
{
}

void SimplePeakProcessor::Process(float* samples, uint32_t nSamples)
{
    // find peak and push to queue
    float bufferPeak = 0.0f;
    for (uint32_t i = 0; i < nSamples; i++) {
        const float s = fabs(samples[i]);
        if (s > bufferPeak) {
            bufferPeak = s;
        }
    }
    m_latestPeak.store(bufferPeak);
    if (bufferPeak > m_maxPeak.load()) {
        m_maxPeak.store(bufferPeak);
        if (!m_signalDetected) {
            m_minPeak.store(bufferPeak);
            m_signalDetected = true;
        }
    }
    if (bufferPeak < m_minPeak.load())
        m_minPeak.store(bufferPeak);
}

bool SimplePeakProcessor::SignalDetected() const
{
    return m_signalDetected;
}

float SimplePeakProcessor::GetLatestPeak()
{
    return m_latestPeak.load();
}

float SimplePeakProcessor::GetLatestPeakDb()
{
    return 20.0f * log10f(m_latestPeak.load());
}

float SimplePeakProcessor::GetMinPeak()
{
    return m_minPeak.load();
}

float SimplePeakProcessor::GetMinPeakDb()
{
    return 20.0f * log10f(m_minPeak.load());
}

float SimplePeakProcessor::GetMaxPeak()
{
    return m_maxPeak.load();
}

float SimplePeakProcessor::GetMaxPeakDb()
{
    return 20.0f * log10f(m_maxPeak.load());
}

std::string_view SimplePeakProcessor::GetName()
{
    return m_name;
}
} // namespace jackmeter