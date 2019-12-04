#include <chrono>
#include <iostream>

#include "eparser/common/scanner.h"
#include "eparser/expressions/objects/operations.h"

namespace eparser { namespace tests {
    namespace calc2 {
        void run();
    }
    namespace custom_parser {
        void run();
    }
    namespace plot {
        void run();
    }
}}

using namespace eparser::expressions::objects;

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
struct C : public eparser::expressions::objects::base {
    C()
        : base(base::info::create<C>())
    {
    }
    const char* type_name() const override
    {
        return "C";
    }
    uptr clone() const override
    {
        return std::make_unique<C>();
    }
};

int main(int argc, char* argv[])
{
    std::string name;
    if (argc < 2) {
        std::cout << "usage: test <name>;\n\tdefault is calc\n";
        name = "calc";
    } else {
        name = argv[1];
    }

    if (name == "calc") {
        eparser::tests::calc2::run();
    } else if (name == "custom") {
        eparser::tests::custom_parser::run();
    } else if (name == "plot") {
        eparser::tests::plot::run();
    }

    return 0;
}
