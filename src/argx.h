/*
* Header-only posix arguments handling library
*
* Copyright © 2016 Elisey Shemyakin <elisey@shemyakin.me>
* This work is free. You can redistribute it and/or modify it under the
* terms of the Do What The Fuck You Want To Public License, Version 2,
* as published by Sam Hocevar. See the COPYING file for more details.
*/

#ifndef ARGX_H__
#define ARGX_H__

#include <algorithm>
#include <cstddef>
#include <iterator>

namespace argx
{
    /* Simple posix command line arguments wrapper. */
    class arguments;

    namespace impl__
    {

        template <class T>
        class basic_iterator__
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T *;
            using reference = T &;

            basic_iterator__() = default;

            basic_iterator__(const basic_iterator__ &) = default;
            basic_iterator__(basic_iterator__ &&) = default;

            basic_iterator__ &operator=(const basic_iterator__ &) = default;
            basic_iterator__ &operator=(basic_iterator__ &&) = default;

            reference operator*() const { return *it__; }

            basic_iterator__ &operator++()
            {
                ++it__;
                return *this;
            }

            basic_iterator__ operator++(int)
            {
                auto tmp = *this;
                ++(*this);
                return tmp;
            }

            basic_iterator__ &operator--()
            {
                --it__;
                return *this;
            }

            basic_iterator__ operator--(int)
            {
                auto tmp = *this;
                --(*this);
                return tmp;
            }

            basic_iterator__ &operator+=(const difference_type i)
            {
                it__ += i;
                return *this;
            }

            basic_iterator__ &operator-=(const difference_type i)
            {
                it__ -= i;
                return *this;
            }

            friend bool operator==(const basic_iterator__ &l,
                                   const basic_iterator__ &r)
            {
                return l.it__ == r.it__;
            }

            friend bool operator<(const basic_iterator__ &l,
                                  const basic_iterator__ &r)
            {
                return l.it__ < r.it__;
            }

            friend difference_type operator-(const basic_iterator__ &l,
                                             const basic_iterator__ &r)
            {
                return l.it__ - r.it__;
            }

        private:
            friend class argx::arguments;

            pointer it__ = nullptr;
            basic_iterator__(pointer it) : it__{it} {}
        };

        template <class T>
        bool operator!=(const basic_iterator__<T> &l,
                        const basic_iterator__<T> &r)
        {
            return !(l == r);
        }

        template <class T>
        bool operator>(const basic_iterator__<T> &l,
                       const basic_iterator__<T> &r)
        {
            return r < l;
        }

        template <class T>
        bool operator<=(const basic_iterator__<T> &l,
                        const basic_iterator__<T> &r)
        {
            return !(l > r);
        }

        template <class T>
        bool operator>=(const basic_iterator__<T> &l,
                        const basic_iterator__<T> &r)
        {
            return !(l < r);
        }
    }

    class arguments
    {
    public:
        using size_type = std::size_t;
        using value_type = char *;
        using pointer = value_type *;
        using const_pointer = const pointer;
        using reference = value_type &;
        using const_reference = const value_type &;
        using iterator = impl__::basic_iterator__<value_type>;
        using const_iterator = impl__::basic_iterator__<const value_type>;
        using reverse_iterator = std::reverse_iterator<iterator>;

        arguments(int argc, char **argv)
            : size__{static_cast<std::size_t>(--argc)}, begin__{std::next(argv)}
        {
        }
        arguments() = delete;
        ~arguments() = default;

        arguments(const arguments &) = delete;
        arguments(arguments &&) = default;

        arguments &operator=(const arguments &) = delete;
        arguments &operator=(arguments &&) = default;

        reference program_name() const { return *(std::prev(begin__)); }

        size_type size() const { return size__; }
        bool empty() const { return !size__; }
        const_reference operator[](const size_type i) const
        {
            return *std::next(begin(), i);
        }

        reference front() const { return *(begin__); }
        reference back() const { return *(std::prev(end__())); }

        iterator begin() const { return {begin__}; }
        iterator end() const { return {end__()}; }

        const_iterator cbegin() const { return {begin__}; }
        const_iterator cend() const { return {end__()}; }

        bool switch_set(const std::string &sw) const
        {
            return std::find_if(cbegin(), cend(), [&sw](const char *x) {
                       return x == sw;
                   }) != cend();
        }

    private:
        size_type size__ = 0;
        pointer begin__ = nullptr;

        pointer end__() const { return begin__ + size__; }
    };
}

#endif
