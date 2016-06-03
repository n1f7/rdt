/*
* Copyright © 2016 Elisey Shemyakin <elisey@shemyakin.me>
* This work is free. You can redistribute it and/or modify it under the
* terms of the Do What The Fuck You Want To Public License, Version 2,
* as published by Sam Hocevar. See the COPYING file for more details.
*/

#ifndef BO_H__
#define BO_H__

#include <climits>
#include <type_traits>

/* Original idea is taken from an excellent Rob Pike's blog post
 * https://commandcenter.blogspot.ru/2012/04/byte-order-fallacy.html */

namespace rdt
{
    namespace impl__
    {

        namespace byte_order
        {
            struct le {
            };
            struct be {
            };
        }

        template <class T, std::size_t N, class ByteOrder, class = void>
        struct shift {
        };

        template <class T, std::size_t N, class ByteOrder>
        struct shift<
            T,
            N,
            ByteOrder,
            std::enable_if_t<std::is_same<ByteOrder, byte_order::le>::value>> {
            inline constexpr std::size_t operator()() const noexcept
            {
                return ((N - 1) * CHAR_BIT);
            }
        };

        template <class T, std::size_t N, class ByteOrder>
        struct shift<
            T,
            N,
            ByteOrder,
            std::enable_if_t<std::is_same<ByteOrder, byte_order::be>::value>> {
            inline constexpr std::size_t operator()() const noexcept
            {
                return (sizeof(T) * CHAR_BIT - (N * CHAR_BIT));
            }
        };

        template <class ByteOrder, class T, std::size_t N = sizeof(T)>
        struct bo_decoder {
            static_assert(std::is_pod<T>::value,
                          "Byte shifting only works on POD types");
            template <class Char>
            inline T operator()(const Char *str) const noexcept
            {
                return (static_cast<T>(str[N - 1])
                        << shift<T, N, ByteOrder>{}()) |
                       bo_decoder<ByteOrder, T, N - 1>{}(str);
            }
        };

        template <class ByteOrder, class T>
        struct bo_decoder<ByteOrder, T, 0> {
            static_assert(std::is_pod<T>::value,
                          "Byte shifting only works on POD types");
            template <class Char>
            inline T operator()(const Char *) const noexcept
            {
                return 0;
            }
        };

        template <class T,
                  class Src,
                  class Dest,
                  std::size_t N = sizeof(T),
                  class = void>
        struct bo_converter {
            static_assert(std::is_pod<T>::value,
                          "Byte shifting only works on POD types");
            inline T operator()(const T &src) const noexcept
            {
                constexpr auto shift = (sizeof(T) - N) * CHAR_BIT;
                return (((src & (static_cast<T>(0xff) << shift)) >> shift)
                        << ((N - 1) * CHAR_BIT)) |
                       bo_converter<T, Src, Dest, N - 1>{}(src);
            }
        };

        template <class T, class Src, class Dest, std::size_t N>
        struct bo_converter<T,
                            Src,
                            Dest,
                            N,
                            std::enable_if_t<std::is_same<Src, Dest>::value>> {
            static_assert(std::is_pod<T>::value,
                          "Byte shifting only works on POD types");
            inline T operator()(const T &src) const noexcept { return src; }
        };

        template <class T, class Src, class Dest>
        struct bo_converter<T, Src, Dest, 0, void> {
            static_assert(std::is_pod<T>::value,
                          "Byte shifting only works on POD types");
            inline T operator()(const T &) const noexcept { return 0; }
        };

        /* Decode a stream of bytes encoded in ByteOrder into type-T-sized
         * object of host byte order */
        template <class ByteOrder, class T, class Char>
        inline T decode(const Char *src) noexcept
        {
            return bo_decoder<ByteOrder, T>{}(src);
        }
    }
}

#endif
