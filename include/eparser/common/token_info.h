#pragma once
#include <string>

namespace eparser { namespace common {

    template <typename CharT, typename KeyT>
    class token_info {
    public:
        using char_type = CharT;
        using key_type = KeyT;
        using string_type = std::basic_string<char_type>;

        struct position {
            std::size_t line;
            std::size_t pos;
        };

        token_info(position p)
            : pos_(std::move(p))
        {
        }

        token_info() = default;
        token_info(token_info&&) = default;
        token_info& operator=(token_info&&) = default;
        token_info(const token_info&) = default;
        token_info& operator=(const token_info&) = default;

        position pos() const
        {
            return pos_;
        }

        void set_pos(position val)
        {
            pos_ = std::move(val);
        }

        const key_type& key() const
        {
            return key_;
        }

        void set_key(key_type val)
        {
            key_ = std::move(val);
        }

        void set_value(string_type val)
        {
            value_ = std::move(val);
        }

        const string_type& value() const
        {
            return value_.empty() ? raw_value_ : value_;
        }

        void set_raw_value(string_type val)
        {
            raw_value_ = std::move(val);
        }
        const string_type& raw_value() const
        {
            return raw_value_;
        }

    private:
        string_type raw_value_;
        string_type value_;
        position pos_ = { 0, 0 };
        key_type key_ = {};
    };

}}
