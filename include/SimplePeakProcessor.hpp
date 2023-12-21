#pragma once

#include <atomic>
#include <string>

#include "IProcessor.hpp"

namespace jackmeter {
class SimplePeakProcessor : public IProcessor {
public:
    explicit SimplePeakProcessor(std::string_view name);
    virtual ~SimplePeakProcessor() = default;

    virtual void Process(float* samples, uint32_t nSamples) final;

    bool SignalDetected() const;

    float GetLatestPeak();
    float GetLatestPeakDb();

    float GetMinPeak();
    float GetMinPeakDb();

    float GetMaxPeak();
    float GetMaxPeakDb();

    std::string_view GetName();

private:
    std::string m_name;
    std::atomic_bool m_signalDetected = false;
    std::atomic<float> m_latestPeak = 0.0;
    std::atomic<float> m_minPeak = 0.0;
    std::atomic<float> m_maxPeak = 0.0;
};
} // namespace jackmeter