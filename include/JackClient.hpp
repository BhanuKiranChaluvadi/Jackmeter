#pragma once

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>


#include <jack/jack.h>

#include "IProcessor.hpp"

namespace jackmeter {
class JackClient {
private:
    struct NaturalStringLess;

public:
    explicit JackClient(std::string_view name);
    virtual ~JackClient();

    void Activate();

    void Deactivate();

    void Connect();

    void AddProbe(std::string_view peer, std::shared_ptr<IProcessor> processor);

    using PortSet = std::set<std::string, NaturalStringLess>;
    PortSet GetOutPorts(const std::vector<std::string>& filter, bool all);

private:
    std::string m_clientName;
    using Link = std::pair<jack_port_t*, jack_port_t*>;
    using ProcessingPair = std::pair<Link, std::shared_ptr<IProcessor>>;
    using ProcessingPairs = std::vector<ProcessingPair>;
    ProcessingPairs m_pairs;

    void NaturalSort(std::vector<std::string>& data);

private:
    jack_client_t* m_client;
    jack_nframes_t m_bufSize;
    jack_nframes_t m_sampleRate;
    static int OnJackProcess(jack_nframes_t nFrames, void* arg);
    // natural compare with numbers in strings
    struct NaturalStringLess {
        bool operator()(std::string a, std::string b) const
        {
            a += '$'; // to avoid special case at end
            b += '$';
            const auto size = std::min(a.length(), b.length());
            for (size_t i = 0; i < size; i++) {
                if (isdigit(a[i]) && isdigit(b[i])) {
                    for (size_t n = i; n < size; n++) {
                        if (isdigit(a[n]) != isdigit(b[n])) {
                            if (isdigit(b[n]))
                                return true; // a has less digits
                            return false; // b has less digits
                        }
                        if (!isdigit(a[n]))
                            break; // same number of digits
                    }
                }
                if (a[i] != b[i]) {
                    if (a[i] == '-' && isdigit(b[i]))
                        return true; // negative less than implicit positive
                    return a[i] < b[i];
                }
            }
            return a.length() < b.length();
        }
    };
};
} // namespace jackmeter
