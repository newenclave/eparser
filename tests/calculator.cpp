#include <cmath>
#include <iostream>
#include <numeric>

#include "ast_to_string.h"
#include "eparser/all.h"

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
        std::map<std::string, objects::base::uptr> env;
        std::map<std::string, double> constants;

        auto op = tests::ast_to_string<char, std::string>("(", ")");
        transfrom calc;

        calc.set<ident_type>([&](auto value) {
            auto itr = env.find(value->token().value());
            auto itrc = constants.find(value->token().value());
            if (itr != env.end()) {
                return calc.apply(itr->second.get());
            } else if (itrc != constants.end()) {
                return itrc->second;
            }
            throw std::runtime_error("No value for '" + value->token().value()
                                     + "'");
        });

        calc.set<value_type>([&](auto value) {
            return std::atof(value->token().value().c_str());
        });

        calc.set<prefix_type>([&](auto value) {
            if (value->token().value() == "-") {
                return -1 * calc.apply(value->value().get());
            }
            return calc.apply(value->value().get());
        });

        calc.set<binop_type>([&](auto value) {
            auto left = value->left().get();
            auto right = value->right().get();
            auto oper = value->token().key();
            if (left->token().key() == "ident"
                && (oper == "=" || oper == ":=")) {
                auto res = calc.apply(right);
                constants.erase(left->token().value());
                env.erase(left->token().value());
                if (oper == "=") {
                    constants[left->token().value()] = res;
                } else {
                    env[left->token().value()] = right->clone();
                }
                return res;
            } else {
                auto left_expr = calc.apply(left);
                auto right_expr = calc.apply(right);
                switch (oper[0]) {
                case '-':
                    return left_expr - right_expr;
                case '+':
                    return left_expr + right_expr;
                case '*':
                    return left_expr * right_expr;
                case '/':
                    return right_expr ? left_expr / right_expr
                                      : std::numeric_limits<double>::infinity();
                case '%':
                    return right_expr ? static_cast<double>(
                               static_cast<std::int64_t>(left_expr)
                               % static_cast<std::int64_t>(right_expr))
                                      : std::numeric_limits<double>::infinity();
                }
            }
            return std::nan("");
        });

        parser_type parser;
        parser.set_ident_key("ident");
        parser.set_number_key("num");
        parser.set_float_key("num");
        parser.set_paren_pair("(", "(", ")", ")");

        parser.add_binary_operation("+", "+", 1);
        parser.add_binary_operation("-", "-", 1);
        parser.add_binary_operation("*", "*", 2);
        parser.add_binary_operation("/", "/", 2);
        parser.add_binary_operation("%", "%", 2);
        parser.add_binary_operation("=", "=", 3);
        parser.add_binary_operation(":=", ":=", 3);

        parser.add_prefix_operation("-", "-", 2);
        parser.add_prefix_operation("+", "+", 2);

        std::cout << "Calculator. Write an expression like '2 + 2'\n";
        while (true) {
            std::string value(1024, '\0');
            std::cout << "Enter: ";
            std::cin.getline(&value[0], 1024);
            if (value[0] == 'q' || value[0] == 'Q') {
                break;
            }
            try {
                auto val = parser.run(value.c_str());
                std::cout << "\t" << op->apply(val.get()) << " = ";
                std::cout << calc.apply(val.get()) << "\n";
            } catch (const std::exception& ex) {
                std::cerr << "\tFaild to evaluate string '" << value.c_str()
                          << "'. "
                          << "Error: " << ex.what() << "\n";
            }
        }
    }

}}}
