#pragma once

#include <atomic>
#include <fftw3.h>
#include <memory>
#include <string>


#include "IProcessor.hpp"

namespace jackmeter {

struct StreamData {
    std::unique_ptr<double[]> in;
    std::unique_ptr<double[]> out;
    fftw_plan p;
    int startIndex;
    int spectroSize;
};


class SimpleFrequencyProcessor : public IProcessor {
public:
    explicit SimpleFrequencyProcessor(std::string_view name);
    virtual ~SimpleFrequencyProcessor() = default;

    virtual void Process(float* samples, uint32_t nSamples, uint32_t sampleRate) final;

    bool SignalDetected() const;

    const StreamData& GetSpectroData() const;

    std::string_view GetName();

private:
    std::string m_name;
    std::atomic_bool m_signalDetected = false;

    // define a variable for specrogram data
    // streamData* spectroData;
    std::unique_ptr<StreamData> spectroData;
    static constexpr int SPECTRO_FREQ_START = 20; // Lower bound of the displayed spectrogram (Hz)
    static constexpr int SPECTRO_FREQ_END = 20000; // Upper bound of the displayed spectrogram (Hz)
};
} // namespace jackmeter