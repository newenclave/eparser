#include "ast_to_string.h"
#include "eparser/expressions/ast.h"
#include "eparser/expressions/objects/operations.h"
#include "eparser/expressions/parser.h"
#include <cmath>
#include <iostream>
#include <numeric>

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
    using binary = typename operations::binary_type;

    void run()
    {
        std::map<std::string, double> env;
        auto op = tests::ast_to_string<char, std::string>("(", ")");
        transfrom calculum;
        binary bin;

        calculum.set<ident_type>([&](auto value) {
            auto itr = env.find(value->info().value());
            if (itr != env.end()) {
                return itr->second;
            }
            throw std::runtime_error("No value for '" + value->info().value()
                                     + "'");
        });

        calculum.set<value_type>([&](auto value) {
            return std::atof(value->info().value().c_str());
        });

        calculum.set<prefix_type>([&](auto value) {
            if (value->info().value() == "-") {
                return -1 * calculum.call(value->value().get());
            }
            return calculum.call(value->value().get());
        });

        bin.set<ident_type, value_type>("=", [&](auto ident, auto value) {
            auto right_expr = calculum.call(value);
            return env[ident->info().value()] = right_expr;
        });

        calculum.set<binop_type>([&](auto value) {
            if (auto call = bin.wrap(value->info().value(), value->left().get(),
                                     value->right().get())) {
                return call();
            } else {
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
        parser.set_ident_key("iden");
        parser.set_number_key("num");
        parser.set_float_key("num");
        parser.set_paren_pair("(", "(", ")", ")");

        parser.add_binary_operation("+", "+");
        parser.add_binary_operation("-", "-");
        parser.add_binary_operation("*", "*", 1);
        parser.add_binary_operation("/", "/", 1);
        parser.add_binary_operation("%", "%", 1);
        parser.add_binary_operation("=", "=", 3);

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
                std::cout << "\t" << op->call(val.get()) << " = ";
                std::cout << calculum.call(val.get()) << "\n";
            }
            catch (const std::exception& ex) {
                std::cerr << "\tFaild to evaluate string '" << value.c_str()
                          << "'. "
                          << "Error: " << ex.what() << "\n";
            }
        }
    }

}}}
