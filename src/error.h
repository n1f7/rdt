/*
* Copyright © 2016 Elisey Shemyakin <elisey@shemyakin.me>
* This work is free. You can redistribute it and/or modify it under the
* terms of the Do What The Fuck You Want To Public License, Version 2,
* as published by Sam Hocevar. See the COPYING file for more details.
*/

#ifndef ERROR_H__
#define ERROR_H__

#include <exception>
#include <string>

#define AT_ __FILE__, __LINE__, __func__
#define AT_ARGS_                                                               \
    const char *file__, decltype(__LINE__) line__, const char *function__
#define AT_VARS_ file__, line__, function__

namespace rdt
{

    inline auto info_at_str(AT_ARGS_) noexcept
    {
        return std::string{file__} + ":" + std::string{function__} + "():" +
               std::to_string(line__);
    }

    class error : virtual public std::exception
    {
    protected:
        std::string at__;
        std::string msg__;
        std::string caught__;
        mutable std::string print__;

    public:
        explicit error(AT_ARGS_) noexcept : at__{info_at_str(AT_VARS_)} {}
        virtual ~error() override = default;

        virtual const char *what() const noexcept override
        {
            if (print__.empty()) {
                print__ =
                    std::string{"exception thrown at: "} + at__ +
                    std::string{"\n============================================"
                                "====================================\n"} +
                    msg__ +
                    std::string{"\n============================================"
                                "====================================\n"} +
                    caught__;
            }
            return print__.data();
        }

        void caught(AT_ARGS_) noexcept
        {
            caught__ = "caught at: " + info_at_str(AT_VARS_);
        }
    };
}

#endif
