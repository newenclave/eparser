#pragma once
#include "eparser/expressions/ast.h"
#include "eparser/expressions/objects/operations.h"

namespace eparser { namespace tests {

    template <typename CharT, typename KeyT>
    inline auto ast_to_string(std::basic_string<CharT> lparen,
                              std::basic_string<CharT> rparen)
    {
        using namespace expressions;
        using string_type = std::basic_string<CharT>;
        using operations = objects::oprerations::all<KeyT, string_type>;
        using transfrom = typename operations::transfrom_type;

        using ident_type = ast::ident<CharT, KeyT>;
        using value_type = ast::value<CharT, KeyT>;
        using binop_type = ast::binary_operation<CharT, KeyT>;
        using prefix_type = ast::ptefix_operation<CharT, KeyT>;
        using postfix_type = ast::postfix_operation<CharT, KeyT>;

        auto op = std::make_unique<transfrom>();
        auto optr = op.get();

        op->template set<ident_type>(
            [optr](auto value) { return value->token().raw_value(); });
        op->template set<value_type>(
            [optr](auto value) { return value->token().raw_value(); });

        op->template set<prefix_type>([optr, lparen, rparen](auto value) {
            return lparen + value->token().value() + " "
                + optr->apply(value->value().get()) + rparen;
        });

        op->template set<postfix_type>([optr, lparen, rparen](auto value) {
            return lparen + optr->apply(value->value().get()) + " "
                + value->token().value() + rparen;
        });

        op->template set<binop_type>([optr, lparen, rparen](auto value) {
            return lparen + optr->apply(value->left().get()) + " "
                + value->token().value() + " "
                + optr->apply(value->right().get()) + rparen;
        });
        return op;
    }

}}
