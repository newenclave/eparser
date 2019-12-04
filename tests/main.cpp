#include <chrono>
#include <iostream>

#include "eparser/common/scanner.h"
#include "eparser/expressions/objects/operations.h"

namespace eparser { namespace tests {
    namespace calc {
        void run();
    }
    namespace custom_parser {
        void run();
    }
    namespace plot {
        void run();
    }
}}


struct A : public eparser::expressions::objects::base {
    A()
        : base(base::info::create<A>())
    {
    }
    const char* type_name() const override
    {
        return "A";
    }
    uptr clone() const override
    {
        return std::make_unique<A>();
    }
};

struct B : public eparser::expressions::objects::base {
    B()
        : base(base::info::create<B>())
    {
    }
    const char* type_name() const override
    {
        return "B";
    }
    uptr clone() const override
    {
        return std::make_unique<B>();
    }
};

int main(int argc, char* argv[])
{
    eparser::expressions::objects::oprerations::all<std::string> oper;
    oper.get_binary().set<A, B>("+", [](auto a, auto b) {
        std::cout << "A B\n";
        return nullptr;
    });

    std::string name;
    if (argc < 2) {
        std::cout << "usage: test <name>;\n\tdefault is calc\n";
        name = "calc";
    } else {
        name = argv[1];
    }

    if (name == "calc") {
        eparser::tests::calc::run();
    } else if (name == "custom") {
        eparser::tests::custom_parser::run();
    } else if (name == "plot") {
        eparser::tests::plot::run();
    }

    return 0;
}
