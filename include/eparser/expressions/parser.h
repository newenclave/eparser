#pragma once
#include "eparser/common/exceptions.h"
#include "eparser/common/parser_base.h"
#include "eparser/expressions/ast.h"
#include "eparser/expressions/lexer.h"

namespace eparser { namespace expressions {

    template <typename CharT, typename KeyT, typename LessT = std::less<CharT>>
    class parser {
    public:
        using char_type = CharT;
        using key_type = KeyT;
        using less_type = LessT;
        using node_type = typename ast::node<char_type, key_type>::uptr;
        using base_parser
            = common::parser_base<node_type, char_type, key_type, less_type>;
        using string_type = std::basic_string<char_type>;
        using lexer_type = lexer<char_type, key_type, less_type>;
        using error_type = common::parser_error<char_type, key_type>;

        using ast_node = ast::node<char_type, key_type>;
        using node_uptr = typename ast_node::uptr;
        using ast_value = ast::value<char_type, key_type>;
        using ast_ident = ast::ident<char_type, key_type>;
        using ast_bin_operation = ast::binary_operation<char_type, key_type>;
        using ast_prefix_operation = ast::ptefix_operation<char_type, key_type>;
        using ast_postfix_operation
            = ast::postfix_operation<char_type, key_type>;

        parser()
        {
            parser_.set_default_nud([](auto ptr) -> node_uptr {
                std::stringstream ss;
                ss << "No NUD function defined for the value.";
                throw error_type(ss.str(), ptr->current());
                return {};
            });
            parser_.set_default_led([](auto ptr, auto state) -> node_uptr {
                std::stringstream ss;
                ss << "No LED function defined for the value.";
                throw error_type(ss.str(), ptr->current());
                return {};
            });
        }

        void set_ident_key(key_type key)
        {
            parser_.set_nud(key, parse_ident);
            lexer_.set_ident_key(std::move(key));
        }
        void set_float_key(key_type key)
        {
            parser_.set_nud(key, parse_value);
            lexer_.set_float_key(std::move(key));
        }
        void set_number_key(key_type key)
        {
            parser_.set_nud(key, parse_value);
            lexer_.set_number_key(std::move(key));
        }
        void set_string_key(key_type key)
        {
            parser_.set_nud(key, parse_value);
            lexer_.set_string_key(std::move(key));
        }
        void set_key(key_type key, const string_type& value,
                     bool force_ident = false)
        {
            lexer_.set_key(key, value, force_ident);
            parser_.set_nud(key, parse_value);
        }

        void add_string_key(key_type key, const string_type& begin,
                            const string_type& end, bool is_ident = false)
        {
            parser_.set_nud(key, is_ident ? parse_ident : parse_value);
            lexer_.add_string_key(std::move(key), begin, end);
        }

        void set_paren_pair(key_type left_key, const string_type& left_val,
                            key_type right_key, const string_type& right_val)
        {
            parser_.set_nud(left_key, [this, right_key](auto ptr) {
                ptr->advance();
                auto expr = ptr->parse_expression(-1);
                ptr->expect(right_key);
                return expr;
            });
            lexer_.set_key(std::move(left_key), left_val);
            lexer_.set_key(std::move(right_key), right_val);
        }

        void add_binary_operation(key_type key, const string_type& value,
                                  int precedence = 0)
        {
            parser_.set_precedense(key, precedence);
            lexer_.set_key(key, value);
            parser_.set_led(key, parse_binary_operation);
        }

        void add_prefix_operation(key_type key, const string_type& value,
                                  int precedence = 0)
        {
            lexer_.set_key(key, value);
            parser_.set_nud(key, [precedence](auto ptr) {
                return parse_prefix_operation(ptr, precedence);
            });
        }

        void add_postfix_operation(key_type key, const string_type& value,
                                   int precedence = 0)
        {
            parser_.set_precedense(key, precedence);
            lexer_.set_key(key, value);
            parser_.set_led(key, parse_postfix_operation);
        }

        node_uptr run(string_type input)
        {
            parser_.reset(lexer_.run(std::move(input)));
            return parser_.parse_expression(-1);
        }

    private:
        static node_uptr parse_value(base_parser* ptr)
        {
            auto value = ptr->current();
            return std::make_unique<ast_value>(std::move(value));
        }

        static node_uptr parse_ident(base_parser* ptr)
        {
            auto value = ptr->current();
            return std::make_unique<ast_ident>(std::move(value));
        }

        static node_uptr parse_prefix_operation(base_parser* ptr,
                                                int precedence)
        {
            auto operation = ptr->current();
            ptr->advance();
            return std::make_unique<ast_prefix_operation>(
                operation, ptr->parse_expression(precedence));
        }

        static node_uptr parse_binary_operation(base_parser* ptr,
                                                node_uptr left)
        {
            auto current = ptr->current();
            auto pp = ptr->current_precednse();
            ptr->advance();
            auto right = ptr->parse_expression(pp);
            return std::make_unique<ast_bin_operation>(
                std::move(current), std::move(left), std::move(right));
        }

        static node_uptr parse_postfix_operation(base_parser* ptr,
                                                 node_uptr left)
        {
            auto value = ptr->current();
            return std::make_unique<ast_postfix_operation>(std::move(value),
                                                           std::move(left));
        }

        base_parser parser_;
        lexer_type lexer_;
    };
}}
