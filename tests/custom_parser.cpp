
#include <iostream>
#include <sstream>

#include "ast_to_string.h"
#include "eparser/all.h"

namespace eparser { namespace tests { namespace custom_parser {

    using namespace eparser::common;
    using namespace eparser::expressions;
    using namespace eparser::expressions::objects;

    using parser_type = parser<char, std::string>;

    class custom_node : public ast::node<char, std::string> {
    public:
        using super_type = ast::node<char, std::string>;
        using node_ptr = typename super_type::uptr;
        using token_info = typename super_type::token_info;

        custom_node(token_info inf)
            : super_type(base::info::create<custom_node>(), inf,
                         "ast::custom_node")
        {
        }

        custom_node()
            : super_type(base::info::create<custom_node>(), {},
                         "ast::custom_node")
        {
        }

        std::vector<base::uptr>& values()
        {
            return values_;
        }

        objects::base::uptr clone() const override
        {
            std::vector<base::uptr> val;
            for (auto& v : values_) {
                val.emplace_back(v->clone());
            }
            auto result = std::make_unique<custom_node>();
            result->values().swap(val);
            return result;
        }

    private:
        std::vector<base::uptr> values_;
    };

    void run()
    {
        parser_type parser;

        // let lexer know we want to read idents.
        parser.set_ident_key("ident");

        // let lexer know we want to read numbers.
        parser.set_number_key("num");
        parser.set_float_key("num");

        // adding a couple of tokens without any parsing function
        parser.set_key(",", ",");
        parser.set_key("]", "]");

        // adding a couple of operators
        parser.add_binary_operation("+", "+");
        parser.add_binary_operation("-", "-");
        parser.add_binary_operation("*", "*", 1);
        parser.add_binary_operation("/", "/", 1);
        parser.set_paren_pair("(", "(", ")", ")");

        // custom parser operation for sequence [a, b, c, d, ...]
        parser.add_nud_operation("[", "[", [](auto ptr) {
            std::vector<base::uptr> values;
            // we start parsing the expression with the token
            // we defined for the expression. '['
            // so in 'current' we have this token and have to advance the
            // parser's state
            ptr->advance();

            // the expecting ending is ']'
            while (!ptr->is_current("]")) {
                values.emplace_back(ptr->parse_expression());
                ptr->expect(",");
                ptr->advance();
            }
            auto result = std::make_unique<custom_node>();
            result->values().swap(values);
            return result;
        });

        auto op = ast_to_string<char, std::string>("(", ")");

        // to string transformer for our custom type
        op->set<custom_node>([&](auto custom) {
            std::stringstream ss;
            ss << "[";
            bool empty = true;
            for (auto& v : custom->values()) {
                if (!empty) {
                    ss << ", ";
                }
                empty = false;
                ss << op->apply(v.get());
            }
            ss << "]";
            return ss.str();
        });

        std::cout << "In expressions you can use idents, numbers and []\n";
        while (true) {
            std::string value(1024, '\0');
            std::cout << "Input an expression: ";
            std::cin.getline(&value[0], value.size());
            try {
                auto res = parser.run(value.c_str());
                std::cout << "\t" << op->apply(res.get()) << "\n";
            } catch (const std::exception& ex) {
                std::cerr << "\tFaild to evaluate string '" << value.c_str()
                          << "'. "
                          << "Error: " << ex.what() << "\n";
            }
        }
    }

}}}
