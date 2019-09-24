#include "ast_to_string.h"
#include "eparser/expressions/ast.h"
#include "eparser/expressions/parser.h"
#include <iostream>
#include <sstream>

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

        parser.set_ident_key("ident");
        parser.set_number_key("num");
        parser.set_float_key("num");

        parser.set_key(",", ",");
        parser.set_key("]", "]");

        /// custom parser operation for sequence [a, b, c, d, ...]
        parser.add_nud_operation("[", "[", [](auto ptr) {
            std::vector<base::uptr> values;
            ptr->advance();
            while (!ptr->is_current("]")) {
                values.emplace_back(ptr->parse_expression());
                ptr->expect(",");
                ptr->advance();
            }
            auto result = std::make_unique<custom_node>();
            result->values().swap(values);
            return result;
        });

        std::string test = "[a, b, c, d, e, 19, 54.90, A]";
        auto res = parser.run(test);
        auto op = ast_to_string<char, std::string>("(", ")");

        /// to string transformer for our custom type
        op->set<custom_node>([&](auto custom) {
            std::stringstream ss;
            ss << "[";
            bool empty = true;
            for (auto& v : custom->values()) {
                if (!empty) {
                    ss << ", ";
                }
                empty = false;
                ss << op->call(v.get());
            }
            ss << "]";
            return ss.str();
        });

        std::cout << "Test sequence is: \"" << test << "\".\n";

        std::cout << "Result is:        \"" << op->call(res.get()) << "\"\n";
    }

}}}
