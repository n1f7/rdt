/*
 * Simple .rdt reader
 *
* Copyright © 2016 Elisey Shemyakin <elisey@shemyakin.me>
* This work is free. You can redistribute it and/or modify it under the
* terms of the Do What The Fuck You Want To Public License, Version 2,
* as published by Sam Hocevar. See the COPYING file for more details.
*/

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "argx.h"
#include "rdt.h"

constexpr auto usage = u8R"(Usage: read [OPTION] [FILE]
Read .rdt FILE and print out its contents to standard output if requested.

  -p,	print out lots of badly formatted data
  -h,	show this help and exit)";

class arguments : public argx::arguments
{
public:
    using argx::arguments::arguments;
    auto help() const { return switch_set("-h"); }
    auto printout() const { return switch_set("-p"); }
};

int main(int argc, char **argv)
{
    try {
        arguments args{argc, argv};
        std::vector<rdt::time_frame> frames;

        if (args.help()) {
            std::cout << usage << '\n';
            return 0;
        }

        if (args.empty() || args.back()[0] == '-') {
            rdt::read_file(std::cin, std::back_inserter(frames));
        } else {
            auto str = std::ifstream{args.back(), std::ios::binary};
            rdt::read_file(str, std::back_inserter(frames));
        }

        std::cout << "read " << frames.size() << " time frames\n";

        if (args.printout()) {
            std::cout << '\n';
            for (auto i = 0; i < frames.size(); ++i)
                std::cout << "frame #" << i << '\n' << frames[i] << '\n';
        }
    } catch (rdt::bad_file &e) {
        e.caught(AT_);
        std::cerr << e.what() << '\n';
        return 2;
    } catch (const std::exception &e) {
        std::cerr << "caught: " << e.what() << '\n';
        return 1;
    }
}
