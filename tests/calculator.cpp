#include "ast_to_string.h"
#include "eparser/expressions/ast.h"
#include "eparser/expressions/objects/operations.h"
#include "eparser/expressions/parser.h"
#include <iostream>

using namespace eparser;
using namespace eparser::common;
using namespace eparser::expressions;

namespace eparser { namespace tests { namespace calc {

    using parser_type = parser<char, std::string>;
    using ident_type = typename parser_type::ast_ident;
    using value_type = typename parser_type::ast_value;
    using binop_type = typename parser_type::ast_bin_operation;
    using prefix_type = typename parser_type::ast_prefix_operation;
    using postfix_type = typename parser_type::ast_postfix_operation;

    using operations = objects::oprerations::all<std::string, double>;
    using transfrom = typename operations::transfrom_type;

    void run()
    {
        auto op = tests::ast_to_string<char, std::string>("(", ")");
        transfrom calculum;

        calculum.set<value_type>([&](auto value) {
            return std::atof(value->info().value().c_str());
        });
        calculum.set<prefix_type>([&](auto value) {
            if (value->info().value() == "-") {
                return -1 * calculum.call(value->value().get());
            }
            return calculum.call(value->value().get());
        });
        calculum.set<binop_type>([&](auto value) {
            auto left_expr = calculum.call(value->left().get());
            auto right_expr = calculum.call(value->right().get());
            switch (value->info().value()[0]) {
            case '-':
                return left_expr - right_expr;
            case '+':
                return left_expr + right_expr;
            case '*':
                return left_expr * right_expr;
            case '/':
                return left_expr / right_expr;
            }
            return std::nan("");
        });

        parser_type parser;
        parser.set_number_key("num");
        parser.set_float_key("float");
        parser.set_paren_pair("(", "(", ")", ")");

        parser.add_binary_operation("+", "+");
        parser.add_binary_operation("-", "-");
        parser.add_binary_operation("*", "*", 1);
        parser.add_binary_operation("/", "/", 1);
        parser.add_prefix_operation("-", "-", 2);
        parser.add_prefix_operation("+", "+", 2);

        while (true) {
            std::string value(1024, '\0');
            std::cout << "Enter: ";
            std::cin.getline(&value[0], 1024);
            if (value[0] == 'q' || value[0] == 'Q') {
                break;
            }
			auto val = parser.run(value.c_str());
			std::cout << "\t" << op->call(val.get()) << " = ";
            std::cout << calculum.call(val.get()) << "\n";
        }
    }

}}}