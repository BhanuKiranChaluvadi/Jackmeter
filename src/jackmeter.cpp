#include <filesystem>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <list>
#include <math.h>
#include <poll.h>
#include <regex>
#include <signal.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include <fmt/format.h>
#include <jack/jack.h>
#include <ncurses.h>

#include "JackClient.hpp"
#include "SimpleFrequencyProcessor.hpp"

namespace jackmeter {

constexpr char Usage[] = "Usage: jackmeter [-h] [-c] [-p] [-x] [-f <freq>] [-d <seconds>] [-n <measurements> ] [<port>, ...]";

int usage()
{
    fmt::print(stderr, "{}\n", Usage);
    return 1;
}

int help()
{
    fmt::print(R"({}

OPTIONS
-f      is how often to update the meter per second [8]
-d      duration seconds [infinite]
-n      n measurements [infinite]
-c      monitor only connected output ports [false]
-p      plain output [false]
-x      don't do pattern matching and don't sort
<port>  the port(s) to monitor, partial matching and with regex support
        if no ports are given, displays all ports
)",
        Usage);

    return 0;
}
} // namespace jackmeter

std::shared_ptr<jackmeter::JackClient> client;
std::vector<std::shared_ptr<jackmeter::SimpleFrequencyProcessor>> processors;
bool plain_output = false;

void log_status_plain()
{
    /*
    time_t rawtime;
    struct tm* ltime;

    time(&rawtime);
    ltime = localtime(&rawtime);
    printf("# jackmeter %04u-%02u-%02ut%02u:%02u:%02u\n", 1900 + ltime->tm_year, 1 + ltime->tm_mon, ltime->tm_mday, ltime->tm_hour, ltime->tm_min, ltime->tm_sec);
    for (auto itr = processors.begin(); itr != processors.end(); ++itr) {
        printf("%s : %6.1f db  (min:%6.1f, max:%6.1f)\n", std::string((*itr)->GetName()).c_str(), (*itr)->GetLatestPeakDb(), (*itr)->GetMinPeakDb(), (*itr)->GetMaxPeakDb());
    }
    fflush(stdout);
    */
}

bool stdin_available()
{
    int ret = 0;
    if (stdin) {
        struct pollfd pfd[1] = {};
        pfd[0].fd = fileno(stdin);
        pfd[0].events = POLLIN;
        ret = poll(pfd, 1, 0);
        if (ret > 0) {
            if (getc(stdin) == EOF)
                return 0;
        }
    }
    return ret > 0;
}

void signal_callback_handler(int signum)
{
    if (!plain_output) {
        endwin();
    }
    client.reset();
    // Terminate program
    exit(signum);
}

int main(int argc, char* argv[])
{
    int rate = 8;
    int opt;
    bool connected_only = false;
    int duration = 0;
    int max_count = 0;
    bool no_matching = false;

    while ((opt = getopt(argc, argv, "f:d:n:pcxh")) != -1) {
        switch (opt) {
        case 'f':
            rate = atoi(optarg);
            fprintf(stderr, "Updates per second: %d\n", rate);
            break;
        case 'd':
            duration = atoi(optarg);
            fprintf(stderr, "Duration: %d seconds\n", duration);
            break;
        case 'n':
            max_count = atoi(optarg);
            fprintf(stderr, "Max measurement count: %d\n", max_count);
            break;
        case 'c':
            connected_only = true;
            break;
        case 'p':
            plain_output = true;
            break;
        case 'x':
            no_matching = true;
            break;
        case 'h':
            return jackmeter::help();
        default:
            return jackmeter::usage();
        }
    }

    signal(SIGINT, signal_callback_handler);

    client = std::make_shared<jackmeter::JackClient>("jackmeter");
    std::vector<std::string> filter;
    for (; argc > optind; optind++)
        filter.emplace_back(std::string(argv[optind]));
    auto addPorts = [](auto& ports) {
        for (auto& portToListenTo : ports) {
            auto processor = std::make_shared<jackmeter::SimpleFrequencyProcessor>(portToListenTo);
            processors.push_back(processor);
            client->AddProbe(portToListenTo, processor);
        }
    };
    if (no_matching) {
        addPorts(filter);
    } else {
        auto ports = client->GetOutPorts(filter, connected_only);
        addPorts(ports);
    }
    if (processors.empty())
        return jackmeter::usage();

    client->Activate();
    client->Connect();

    WINDOW* window = NULL;
    auto start = std::chrono::steady_clock::now();
    auto stop = start + std::chrono::seconds(duration);
    int measurement = 0;
    while (!stdin_available() && (std::chrono::steady_clock::now() < stop || duration == 0) && (++measurement <= max_count || max_count == 0)) {
        if (plain_output) {
            log_status_plain();
        } else {
            if (measurement == 1) {
                window = initscr();
                if (window == NULL) {
                    // Handle error if initscr() fails
                    return 1;
                }
                noecho(); // Don't echo any keypresses
                curs_set(FALSE); // Don't display a cursor
                wmove(window, 0, 0);
                wrefresh(window);
            }

            // Set our spectrogram size in the terminal to 100 characters, and move the
            // cursor to the beginning of the line
            int dispSize = 100;

            for (auto itr = processors.begin(); itr != processors.end(); ++itr) {

                try {
                    const jackmeter::StreamData& data = (*itr)->GetSpectroData();

                    for (int i = 0; i < dispSize; i++) {
                        // Sample frequency data logarithmically
                        double proportion = std::pow(i / (double)dispSize, 4);
                        double freq = data.out[(int)(data.startIndex
                            + proportion * data.spectroSize)];

                        // Display full block characters with heights based on frequency intensity
                        std::string block;
                        /*
                        if (freq < 0.125) {
                            block = "▁";
                        } else if (freq < 0.25) {
                            block = "▂";
                        } else if (freq < 0.375) {
                            block = "▃";
                        } else if (freq < 0.5) {
                            block = "▄";
                        } else if (freq < 0.625) {
                            block = "▅";
                        } else if (freq < 0.75) {
                            block = "▆";
                        } else if (freq < 0.875) {
                            block = "▇";
                        } else {
                            block = "█";
                        }
                        */
                        /**/
                        if (freq < 0.125) {
                            block = "-";
                        } else if (freq < 0.25) {
                            block = "+";
                        } else if (freq < 0.375) {
                            block = "*";
                        } else if (freq < 0.5) {
                            block = "/";
                        } else if (freq < 0.625) {
                            block = "=";
                        } else if (freq < 0.75) {
                            block = "#";
                        } else if (freq < 0.875) {
                            block = "%";
                        } else {
                            block = "@";
                        }

                        mvprintw(0, i, "%s", block.c_str());
                    }
                    refresh(); // Refresh the screen to show the new characters
                } catch (const std::runtime_error& e) {
                    continue;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds { static_cast<uint32_t>(1000 / rate) });
    }
    if (!plain_output) {
        endwin();
        log_status_plain();
    }

    return EXIT_SUCCESS;
}
