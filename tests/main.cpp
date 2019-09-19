#include "eparser/expressions/ast.h"
#include "eparser/expressions/objects/operations.h"
#include "eparser/expressions/parser.h"

#include <chrono>
#include <iostream>

using namespace eparser::common;
using namespace eparser::expressions;

struct node {
    using uptr = std::unique_ptr<node>;
};

using parser_type = parser<char, std::string>;
using lexer_type = typename parser_type::lexer_type;
using operations = objects::oprerations::all<std::string, std::string>;
using transform = typename operations::transfrom_type;

using ident_type = typename parser_type::ast_ident;
using value_type = typename parser_type::ast_value;
using binop_type = typename parser_type::ast_bin_operation;
using prefix_type = typename parser_type::ast_prefix_operation;
using postfix_type = typename parser_type::ast_postfix_operation;

int main()
{
    transform op;

    op.set<ident_type>([&](auto value) { return value->info().raw_value(); });
    op.set<value_type>([&](auto value) { return value->info().raw_value(); });
    op.set<prefix_type>([&](auto value) {
        return "(" + value->info().value() + " " + op.call(value->value().get())
            + ")";
    });
    op.set<postfix_type>([&](auto value) {
        return "(" + op.call(value->value().get()) + " " + value->info().value()
            + ")";
    });
    op.set<binop_type>([&](auto value) {
        return "(" + op.call(value->left().get()) + " " + value->info().value()
            + " " + op.call(value->right().get()) + ")";
    });

    parser_type par;
    std::string test1 = "(ident1-- + 17-- * 89 + [AND] eq 13 eq [eq]) as "
                        "boolean in (1, 2, 3, 4, 5, 6)";
    par.set_ident_key("IDENT");
    par.set_string_key("STRING");
    par.set_number_key("NUMBER");
    par.set_float_key("FLOAT");
    par.set_paren_pair("(", "(", ")", ")");
    par.add_binary_operation("eq", "eq", 1);
    par.add_binary_operation("in", "in", 1);
    par.add_binary_operation("and", "and", 1);
    par.add_binary_operation("as", "as", 1);
    par.add_binary_operation("+", "+");
    par.add_binary_operation("-", "-");
    par.add_binary_operation("*", "*", 2);
    par.add_binary_operation(",", ",", 0);
    par.add_prefix_operation("--", "--", 5);
    par.add_postfix_operation("--", "--", 5);
    par.add_string_key("IDENT", "[", "]", true);

    auto start = std::chrono::high_resolution_clock::now();

    auto vals = par.run(test1);
    for (int i = 0; i < 1; ++i) {
        vals = par.run(test1);
        /// std::cout << op.call(vals.get()) << "\n";
    }

    std::cout << op.call(vals.get()) << "\n";
    auto stop = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop
                                                                       - start)
                     .count()
              << "\n";

    return 0;
}
