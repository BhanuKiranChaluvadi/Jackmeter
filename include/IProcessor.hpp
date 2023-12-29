#pragma once

namespace jackmeter {
class IProcessor {
public:
    virtual ~IProcessor() = default;
    virtual void Process(float* samples, uint32_t nSamples) = 0;
};
} // namespace jackmeter