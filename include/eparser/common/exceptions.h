#pragma once
#include "eparser/common/token_info.h"
#include <stdexcept>

namespace eparser { namespace common {
    class exception : public std::runtime_error {
    public:
        exception(const std::string& message)
            : std::runtime_error(message)
        {
        }
    };

    template <typename CharT, typename KeyT>
    class lexer_error : public exception {
    public:
        using token_type = common::token_info<CharT, KeyT>;
        lexer_error(const std::string& message, const token_type& tok)
            : exception(message)
        {
        }
        const token_type& token() const
        {
            return token_;
        }

    private:
        token_type token_;
    };

    template <typename CharT, typename KeyT>
    class parser_error : public exception {
    public:
        using token_type = common::token_info<CharT, KeyT>;
        parser_error(const std::string& message, const token_type& tok)
            : exception(message)
        {
        }
        const token_type& token() const
        {
            return token_;
        }

    private:
        token_type token_;
    };
}}
