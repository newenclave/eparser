#include <cmath>
#include <iostream>
#include <numeric>

#include "ast_to_string.h"
#include "eparser/all.h"

using namespace eparser;
using namespace eparser::common;
using namespace eparser::expressions;

namespace eparser { namespace tests { namespace calc2 {

    using parser_type = parser<char, std::string>;
    using ident_type = typename parser_type::ast_ident;
    using value_type = typename parser_type::ast_value;
    using binop_type = typename parser_type::ast_bin_operation;
    using prefix_type = typename parser_type::ast_prefix_operation;
    using postfix_type = typename parser_type::ast_postfix_operation;

    using operations = objects::oprerations::all<std::string, double>;
    using evaluator = typename operations::transfrom_type;
    using bin_evaluator = typename operations::binary_type;
    using un_evaluator = typename operations::unary_type;

    void run()
    {
        std::map<std::string, objects::base::uptr> env;
        std::map<std::string, double> constants;

        auto op = tests::ast_to_string<char, std::string>("(", ")");
        evaluator ev;
        bin_evaluator bev;
        un_evaluator uev;

        ev.set<ident_type>([&](auto value) {
            auto itr = env.find(value->token().value());
            auto itrc = constants.find(value->token().value());
            if (itr != env.end()) {
                return ev.apply(itr->second.get());
            } else if (itrc != constants.end()) {
                return itrc->second;
            }
            throw std::runtime_error("No value for '" + value->token().value()
                                     + "'");
        });

        ev.set<value_type>([&](auto value) {
            return std::atof(value->token().value().c_str());
        });

        uev.set<objects::base>(
            "-", [&](auto value) { return -1 * ev.apply(value); });

        uev.set<objects::base>("+",
                               [&](auto value) { return ev.apply(value); });

        ev.set<prefix_type>([&](auto value) {
            auto oper = value->token().value();
            if (auto call = uev.get(oper, value->value().get())) {
                return call(value->value().get());
            }
            throw std::runtime_error("prefix operator '" + oper
                                     + "' not found for '"
                                     + value->value()->type_name() + "'");
        });

        bev.set<objects::base, objects::base>("+", [&](auto lft, auto rght) {
            return ev.apply(lft) + ev.apply(rght);
        });
        bev.set<objects::base, objects::base>("-", [&](auto lft, auto rght) {
            return ev.apply(lft) - ev.apply(rght);
        });
        bev.set<objects::base, objects::base>("*", [&](auto lft, auto rght) {
            return ev.apply(lft) * ev.apply(rght);
        });
        bev.set<objects::base, objects::base>("/", [&](auto lft, auto rght) {
            auto delimeter = ev.apply(rght);
            return delimeter ? ev.apply(lft) / delimeter
                             : std::numeric_limits<double>::infinity();
        });
        bev.set<objects::base, objects::base>("%", [&](auto lft, auto rght) {
            auto delimeter = ev.apply(rght);
            return delimeter ? static_cast<std::int64_t>(ev.apply(lft))
                    % static_cast<std::int64_t>(delimeter)
                             : std::numeric_limits<double>::infinity();
        });

        bev.set<ident_type, objects::base>("=", [&](auto id, auto value) {
            return constants[id->token().value()] = ev.apply(value);
        });
        bev.set<ident_type, objects::base>(":=", [&](auto id, auto value) {
            env[id->token().value()] = value->clone();
            return ev.apply(value);
        });

        ev.set<binop_type>([&](auto value) {
            auto left = value->left().get();
            auto right = value->right().get();
            auto oper = value->token().key();
            if (auto call = bev.get(oper, left, right)) {
                return call(left, right);
            }
            throw std::runtime_error("binary operator '" + oper
                                     + "' not found for '" + left->type_name()
                                     + "' and '" + right->type_name() + "'");
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
                std::cout << ev.apply(val.get()) << "\n";
            } catch (const std::exception& ex) {
                std::cerr << "\n\tFailed to evaluate string '" << value.c_str()
                          << "'. "
                          << "Error: " << ex.what() << "\n";
            }
        }
    }

}}}
