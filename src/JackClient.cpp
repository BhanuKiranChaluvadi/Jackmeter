#include <algorithm>
#include <memory>
#include <regex>
#include <string>

#include <fmt/format.h>
#include <jack/jack.h>

#include "JackClient.hpp"


namespace jackmeter {
JackClient::JackClient(std::string_view name)
    : m_clientName(std::string(name))
{
    jack_options_t options = JackNullOption;
    jack_status_t status;
    m_client = jack_client_open(m_clientName.c_str(), options, &status);

    if (!m_client)
        throw std::runtime_error(fmt::format("Could not create jack client '{}', reason:{}", m_clientName, status));
    if (status & JackServerStarted)
        fmt::print(stderr, "jack_client_open started jack server, is there a bug?");
    if (status & JackNameNotUnique) {
        m_clientName = jack_get_client_name(m_client);
        fmt::print(stderr, "Name not unique.");
    }

    jack_set_process_callback(m_client, JackClient::OnJackProcess, this);

    m_bufSize = jack_get_buffer_size(m_client);
    m_sampleRate = jack_get_sample_rate(m_client);
}

JackClient::~JackClient()
{
    for (auto pair : m_pairs) {
        auto srcPort = pair.first.first;
        auto sinkPort = pair.first.second;
        jack_disconnect(m_client, jack_port_name(srcPort), jack_port_name(sinkPort));
    }

    jack_deactivate(m_client);
    jack_set_process_callback(m_client, nullptr, nullptr);
    for (auto pair : m_pairs) {
        auto sinkPort = pair.first.second;
        jack_port_unregister(m_client, sinkPort);
    }

    jack_client_close(m_client);
}

void JackClient::Activate()
{
    auto res = jack_activate(m_client);
    if (res)
        throw std::runtime_error(fmt::format( //
            "Activating client '{}' failed with {}", m_clientName, res));
}

void JackClient::Deactivate()
{
    jack_deactivate(m_client);
}

void JackClient::Connect()
{
    for (auto pair : m_pairs) {
        auto srcPort = pair.first.first;
        auto sinkPort = pair.first.second;
        if (jack_connect(m_client, jack_port_name(srcPort), jack_port_name(sinkPort))) {
            fmt::print("Can't connect ports {} -> {}\n", jack_port_name(srcPort), jack_port_name(sinkPort));
        }
    }
}

void JackClient::AddProbe(std::string_view peer, std::shared_ptr<IProcessor> processor)
{
    auto peerPort = jack_port_by_name(m_client, std::string(peer).c_str());
    if (!peerPort)
        throw std::runtime_error(fmt::format("Can't find port {}", peer));

    auto newPortName = fmt::format("in_{}", m_pairs.size());
    auto newPort = jack_port_register(m_client, newPortName.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    m_pairs.push_back(std::make_pair(std::make_pair(peerPort, newPort), processor));
}

JackClient::PortSet JackClient::GetOutPorts(const std::vector<std::string>& filter, bool connectedOnly)
{
    PortSet result;
    auto try_insert = [this, &result, connectedOnly](auto ports) {
        for (unsigned i = 0; ports && ports[i]; ++i) {
            if (connectedOnly) {
                auto portObj = jack_port_by_name(m_client, ports[i]);
                if (!portObj)
                    continue;
                auto connections = jack_port_get_all_connections(m_client, portObj);
                if (!connections)
                    continue;
                jack_free(connections);
            }
            result.insert(ports[i]);
        }
        jack_free(ports);
    };
    if (filter.empty()) {
        auto ports = jack_get_ports(m_client, NULL, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput);
        try_insert(ports);
    } else {
        for (const auto& pattern : filter) {
            auto ports = jack_get_ports(m_client, pattern.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput);
            try_insert(ports);
        }
    }
    return result;
}

int JackClient::OnJackProcess(jack_nframes_t nFrames, void* arg)
{
    JackClient* self = static_cast<JackClient*>(arg);
    try {
        for (auto pair : self->m_pairs) {
            auto probePort = pair.first.second;
            auto probeProcessor = pair.second;
            probeProcessor->Process(static_cast<float*>(jack_port_get_buffer(probePort, nFrames)), nFrames);
        }
    } catch (const std::exception& ex) {
        fmt::print(stderr, "Caught exception when running audio callback {}", ex.what());
        return -1;
    }
    return 0;
}
} // namespace jackmeter
