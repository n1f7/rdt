/*
* Copyright © 2016 Elisey Shemyakin <elisey@shemyakin.me>
* This work is free. You can redistribute it and/or modify it under the
* terms of the Do What The Fuck You Want To Public License, Version 2,
* as published by Sam Hocevar. See the COPYING file for more details.
*/

#ifndef RDT_H__
#define RDT_H__

#include <array>
#include <chrono>
#include <cstdint>
#include <exception>
#include <iomanip>
#include <istream>
#include <sstream>
#include <vector>

#include "bo.h"
#include "error.h"

namespace rdt
{

    struct hit {
        std::chrono::nanoseconds delta;
        std::uint8_t channel;
    };

    struct device {
        std::vector<hit> hits;
        std::uint32_t id;
    };

    using time_frame = std::vector<device>;

    namespace impl__
    {
        template <class T>
        T read(std::istream &str)
        {
            std::array<std::uint8_t, sizeof(T)> buf = {};
            str.read(reinterpret_cast<char *>(buf.data()), sizeof(T));
            return impl__::decode<byte_order::le, T>(buf.data());
        }
    }

    class bad_file : public error
    {
        std::istream::pos_type pos__;

    public:
        template <class T>
        inline bad_file(AT_ARGS_, std::istream &str, const T &expected)
            : error{AT_VARS_},
              pos__{str.tellg() - std::istream::pos_type{sizeof(T)}}
        {
            std::stringstream pos_str, got_str, exp_str;
            pos_str << "0x" << std::hex << pos__;
            exp_str << "0x" << std::hex << expected;
            got_str << "0x" << std::hex << impl__::read<T>(str);
            msg__ += "bad file at: " + pos_str.str() + "\texpected: " +
                     exp_str.str() + "\t got: " + got_str.str();
        }
    };

    template <class Iter>
    Iter read_file(std::istream &str, Iter d_first)
    {
        constexpr std::uint32_t checkword = 0xffffffff;
        constexpr std::uint32_t delta_mask = 0xffffff;
        constexpr std::uint32_t hits_mask = 0x7ff;

        while (str) {
            /* Skip redundant frame counter word */
            (void)impl__::read<std::uint32_t>(str);
            auto num_of_devices = impl__::read<std::uint32_t>(str);
            std::vector<device> devices(num_of_devices);

            for (auto &dev : devices) {
                dev.id = impl__::read<std::uint32_t>(str);
                if (impl__::read<std::uint32_t>(str) != checkword)
                    throw bad_file{AT_, str, checkword};
                auto word = impl__::read<std::uint32_t>(str);
                auto num_of_hits = hits_mask & word;
                dev.hits.resize(num_of_hits);

                for (auto &sig : dev.hits) {
                    auto word = impl__::read<std::uint32_t>(str);
                    sig.channel = (word & ~delta_mask) >> 24;
                    sig.delta = decltype(sig.delta){word & delta_mask};
                }
            }

            if (str.eof()) break;
            *d_first++ = std::move(devices);
        }
        return d_first;
    }
}

std::ostream &operator<<(std::ostream &str, const rdt::time_frame &x)
{
    for (const auto &dev : x) {
        str << "device #" << dev.id << " caught " << dev.hits.size()
            << " hits\n";
        for (const auto &sig : dev.hits) {
            str << "channel #" << static_cast<int>(sig.channel);
            str << ":\tdelta: " << sig.delta.count() << "\tnsec\n";
        }
        str << '\n';
    }
    return str;
}

#endif
