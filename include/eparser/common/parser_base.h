#pragma once
#include <functional>
#include <map>
#include <vector>

#include "eparser/common/lexer_base.h"

namespace eparser { namespace common {
    template <typename NodeT, typename CharT, typename KeyT,
              typename LessT = std::less<CharT>>
    class parser_base {
    public:
        using char_type = CharT;
        using key_type = KeyT;
        using node_type = NodeT;
        using less_type = LessT;

        using lexer_type = lexer_base<char_type, key_type, less_type>;
        using token_type = typename lexer_type::token_info;
        using string_type = typename lexer_type::string_type;

        using this_type
            = parser_base<node_type, char_type, key_type, less_type>;

        class state {
            friend class parser_base;
            state(typename std::vector<token_type>::const_iterator current,
                  typename std::vector<token_type>::const_iterator next)
            {
                current_ = current;
                next_ = next;
            }
            typename std::vector<token_type>::const_iterator current_;
            typename std::vector<token_type>::const_iterator next_;
        };

        using led_call_type = std::function<node_type(this_type*, node_type)>;
        using nud_call_type = std::function<node_type(this_type*)>;

        virtual ~parser_base() = default;

        parser_base()
            : current_(tokens_.begin())
            , next_(tokens_.begin())
        {
        }

        parser_base(std::vector<token_type> lexem)
            : tokens_(std::move(lexem))
            , current_(tokens_.begin())
            , next_(tokens_.begin())
        {
            advance();
        }

        void reset(std::vector<token_type> lexem)
        {
            tokens_ = std::move(lexem);
            current_ = tokens_.begin();
            next_ = tokens_.begin();
            advance();
        }

        void set_led(key_type id, led_call_type call)
        {
            leds_[id] = std::move(call);
        }

        void set_nud(key_type id, nud_call_type call)
        {
            nuds_[id] = std::move(call);
        }

        void set_default_nud(nud_call_type call)
        {
            default_nud_ = std::move(call);
        }

        void set_default_led(led_call_type call)
        {
            default_led_ = std::move(call);
        }

        void set_precedense(key_type id, int value)
        {
            precedenses_[id] = value;
        }

        node_type parse_expression()
        {
            return parse_expression(-1);
        }

        node_type parse_expression(int p)
        {
            auto nud = nuds_.find(current().key());
            node_type left;
            if (nud == nuds_.end()) {
                left = default_nud();
            } else {
                left = nud->second(this);
            }

            if (!left) {
                return nullptr;
            }

            int pp = next_precednse();
            token_type pt = next();
            while (p < pp) {
                led_call_type led_call = [this](auto, auto left) {
                    return default_led(std::move(left));
                };

                auto led = leds_.find(pt.key());
                if (led != leds_.end()) {
                    led_call = led->second;
                }

                advance();
                left = led_call(this, std::move(left));
                if (!left) {
                    return left;
                }

                pp = next_precednse();
                pt = next();
            }
            return left;
        }

        void advance()
        {
            current_ = next_;
            if (!next_eof()) {
                ++next_;
            }
        }

        bool expect(key_type id)
        {
            if (!next_eof() && (next().key() == id)) {
                advance();
                return true;
            }
            return false;
        }

        bool eof() const
        {
            return current_ == tokens_.cend();
        }

        bool next_eof() const
        {
            return next_ == tokens_.cend();
        }

        token_type current() const
        {
            return eof() ? token_type {} : *current_;
        }

        bool is_current(key_type id) const
        {
            return current_->key() == id;
        }

        token_type next() const
        {
            return next_eof() ? token_type {} : *next_;
        }

        int current_precednse()
        {
            return itr_precednse(current_);
        }

        int next_precednse()
        {
            return itr_precednse(next_);
        }

        node_type default_nud()
        {
            return default_nud_ ? default_nud_(this) : node_type {};
        }

        node_type default_led(node_type left)
        {
            return default_led_ ? default_led_(this, std::move(left))
                                : node_type {};
        }

        state store() const
        {
            return { current_, next_ };
        }

        void restore(state state)
        {
            current_ = state.current_;
            next_ = state.next_;
        }

    private:
        int itr_precednse(typename std::vector<token_type>::const_iterator itr)
        {
            if (itr == tokens_.cend()) {
                return -1;
            }
            auto found = precedenses_.find(itr->key());
            return found == precedenses_.end() ? -1 : found->second;
        }

        std::map<key_type, nud_call_type> nuds_;
        std::map<key_type, led_call_type> leds_;

        nud_call_type default_nud_;
        led_call_type default_led_;

        std::map<key_type, int> precedenses_;
        std::vector<token_type> tokens_;
        typename std::vector<token_type>::const_iterator current_;
        typename std::vector<token_type>::const_iterator next_;
    };

}}