#include <cmath>
#include <iostream>
#include <numeric>

#include "ast_to_string.h"
#include "eparser/all.h"

using namespace eparser;
using namespace eparser::common;
using namespace eparser::expressions;

namespace eparser { namespace tests { namespace plot {

    using parser_type = parser<char, std::string>;
    using ident_type = typename parser_type::ast_ident;
    using value_type = typename parser_type::ast_value;
    using binop_type = typename parser_type::ast_bin_operation;
    using prefix_type = typename parser_type::ast_prefix_operation;
    using postfix_type = typename parser_type::ast_postfix_operation;

    using operations = objects::oprerations::all<std::string, double>;

    using transfrom = typename operations::transfrom_type;
    using bibary_type = typename operations::binary_type;
    using transfrom = typename operations::transfrom_type;

    using line_type = std::vector<std::uint8_t>;
    using canvas_type = std::vector<line_type>;

    void point(canvas_type& canvas, std::size_t x, std::size_t y)
    {
        if (x >= 0 && y >= 0 && y < canvas.size() && x < canvas[y].size()) {
            canvas[canvas.size() - y - 1][x] = '*';
        }
    }

    void line(canvas_type& canvas, int ax, int ay, int bx, int by)
    {
        float inc = 1.0f / std::max(by - ay, bx - ax);
        for (float i = 0.0f; i <= 1.0f; i += inc) {
            auto sx = static_cast<std::size_t>(
                std::round((1.0f - i) * ax + (i * bx)));
            auto sy = static_cast<std::size_t>(
                std::round((1.0f - i) * ay + (i * by)));
            point(canvas, sx, sy);
        }
    }

    canvas_type create_canvas(std::size_t x, std::size_t y)
    {
        return canvas_type(y, line_type(x + 1, 0));
    }

    void show(canvas_type& canvas)
    {
        for (auto& line : canvas) {
            for (auto& v : line) {
                std::cout.put(v);
            }
            std::cout.put('\n');
        }
    }

    void run()
    {
        std::map<std::string, objects::base::uptr> env;
        std::map<std::string, double> constants;

        auto op = tests::ast_to_string<char, std::string>("(", ")");
        bibary_type bin_calc;
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
            } else if (value->token().value() == "abs") {
                return std::abs(calc.apply(value->value().get()));
            } else if (value->token().value() == "sin") {
                return std::sin(calc.apply(value->value().get()));
            } else if (value->token().value() == "cos") {
                return std::cos(calc.apply(value->value().get()));
            } else if (value->token().value() == "tan") {
                return std::tan(calc.apply(value->value().get()));
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
        parser.add_prefix_operation("abs", "abs", 2);
        parser.add_prefix_operation("sin", "sin", 2);
        parser.add_prefix_operation("cos", "cos", 2);
        parser.add_prefix_operation("tan", "tan", 2);

        std::cout << "Write an expression like 'x + 2'\n";
        constexpr std::size_t X = 120;
        constexpr std::size_t Y = 25;
        auto canvas = create_canvas(X, Y);
        while (true) {
            std::string value;
            std::cout << "Enter: ";
            std::getline(std::cin, value);
            if (value[0] == 'q' || value[0] == 'Q') {
                break;
            } else if (value.size() == 1
                       && (value[0] == 'c' || value[0] == 'C')) {
                canvas = create_canvas(X, Y);
                continue;
            }
            try {
                auto val = parser.run(value);
                int last_x = 0;
                int last_y = 0;
                for (std::size_t x = 0; x < X; ++x) {
                    constants["x"] = static_cast<double>(x);
                    auto y = calc.apply(val.get());
                    if (x == 0) {
                        last_x = x;
                        last_y = static_cast<int>(y);
                        point(canvas, x, last_y);
                    } else {
                        line(canvas, last_x, last_y, x, static_cast<int>(y));
                        last_x = x;
                        last_y = static_cast<int>(y);
                    }
                }
                show(canvas);
                std::cout << "\n";
            } catch (const std::exception& ex) {
                std::cerr << "\tFaild to evaluate string '" << value.c_str()
                          << "'. "
                          << "Error: " << ex.what() << "\n";
            }
        }
    }

}}}
