#pragma once

#include <functional>

#include "eparser/common/exceptions.h"
#include "eparser/common/token_info.h"
#include "eparser/common/trie.h"

namespace eparser { namespace common {

    template <typename CharT, typename KeyT, typename LessT = std::less<CharT>>
    class lexer_base {
    public:
        using char_type = CharT;
        using key_type = KeyT;
        using string_type = std::basic_string<char_type>;
        using iterator = typename string_type::iterator;
        using const_iterator = typename string_type::const_iterator;
        using less_type = LessT;

        using token_info = common::token_info<char_type, key_type>;

        class internal_state {
        public:
            internal_state() = default;
            internal_state(internal_state&&) = default;
            internal_state& operator=(internal_state&&) = default;
            internal_state(const internal_state&) = default;
            internal_state& operator=(const internal_state&) = default;

            internal_state(const_iterator b, const_iterator e)
                : _begin(b)
                , _end(e)
            {
            }
            const_iterator begin() const
            {
                return _begin;
            }
            const_iterator end() const
            {
                return _end;
            }

        private:
            const_iterator _begin;
            const_iterator _end;
        };

        using create_state_factory = std::function<token_info()>;
        using token_state_factory
            = std::function<token_info(token_info, internal_state)>;
        using token_trie = trie<char_type, token_state_factory, less_type>;

        ~lexer_base() = default;

        lexer_base() = default;

        lexer_base(create_state_factory create_factory,
                   token_state_factory default_factory = nullptr)
            : create_factory_(std::move(create_factory))
            , default_factory_(get_default_factory(default_factory))
        {
        }

        lexer_base(lexer_base&&) = default;
        lexer_base& operator=(lexer_base&&) = default;
        lexer_base(const lexer_base&) = default;
        lexer_base& operator=(const lexer_base&) = default;

        void add_factory(const string_type& input, token_state_factory factory)
        {
            trie_.set(std::begin(input), std::end(input), std::move(factory));
        }

        void set_default_factory(token_state_factory factory)
        {
            default_factory_ = std::move(factory);
        }

        void set_create_factory(create_state_factory factory)
        {
            create_factory_ = std::move(factory);
        }

        token_info next(const_iterator begin, const_iterator end)
        {
            auto state = create_factory_();
            auto next_getter = trie_.get(begin, end, true);
            if (next_getter) {
                return (*next_getter)(
                    std::move(state),
                    { next_getter.begin(), next_getter.end() });
            }
            return default_factory_(std::move(state),
                                    { next_getter.begin(), next_getter.end() });
        }

    private:
        static token_state_factory get_default_factory(token_state_factory val)
        {
            return val ? val
                       : [](token_info state, internal_state) { return state; };
        }

        token_trie trie_;
        create_state_factory create_factory_ = []() { return token_info {}; };
        token_state_factory default_factory_
            = [](token_info state, internal_state) { return state; };
    };

}}
