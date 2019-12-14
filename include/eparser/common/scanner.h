#pragma once
#include <cstdint>

namespace eparser { namespace common {
    template <typename ItrT>
    class scanner {
    public:
        using iterator_type = ItrT;

        scanner(ItrT b, ItrT e)
            : current_(b)
            , end_(e)
        {
        }

        scanner() = default;
        scanner(scanner&&) = default;
        scanner& operator=(scanner&&) = default;
        scanner(const scanner&) = default;
        scanner& operator=(const scanner&) = default;

        std::size_t line() const
        {
            return line_;
        }

        std::size_t pos() const
        {
            return pos_;
        }

        bool eol() const
        {
            return current_ == end_;
        }

        ItrT begin() const
        {
            return current_;
        }

        ItrT end() const
        {
            return end_;
        }

        auto operator*()
        {
            return *current_;
        }

        auto operator*() const
        {
            return *current_;
        }

        scanner<ItrT>& operator++() // prefix
        {
            if (*current_ == '\n') {
                ++line_;
                pos_ = 1;
            } else {
                ++pos_;
            }
            ++current_;
            return *this;
        }

        scanner<ItrT> operator++(int) // postfix
        {
            auto old = *this;
            this->operator++();
            return old;
        }

    private:
        iterator_type current_ = {};
        iterator_type end_ = {};

        std::size_t line_ = 1;
        std::size_t pos_ = 1;
    };

    template <typename IT1, typename IT2>
    inline bool operator==(const scanner<IT1> begin, IT2 end)
    {
        return begin.begin() == end;
    }
    template <typename IT1, typename IT2>
    inline bool operator!=(const scanner<IT1> begin, IT2 end)
    {
        return begin.begin() != end;
    }

    template <typename IT1, typename IT2>
    inline bool operator==(IT1 begin, const scanner<IT2>& end)
    {
        return begin == end.begin();
    }
    template <typename IT1, typename IT2>
    inline bool operator!=(IT1 begin, const scanner<IT2>& end)
    {
        return begin != end.begin();
    }

    template <typename IT1, typename IT2>
    inline bool operator==(const scanner<IT2>& begin, const scanner<IT2>& end)
    {
        return begin.begin() == end.begin();
    }
    template <typename IT1, typename IT2>
    inline bool operator!=(const scanner<IT2>& begin, const scanner<IT2>& end)
    {
        return begin.begin() != end.begin();
    }

    template <typename ItrT>
    inline auto make_scanner(ItrT b, ItrT e)
    {
        return scanner<ItrT>(b, e);
    }
}}
