#pragma once

namespace jackmeter {
class IProcessor {
public:
    virtual ~IProcessor() = default;
    virtual void Process(float* samples, uint32_t nSamples,  uint32_t sampleRate) = 0;
};
} // namespace jackmeter