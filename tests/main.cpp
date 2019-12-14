#include <chrono>
#include <iostream>

#include "eparser/common/helpers.h"
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

struct number_value {
    number_value(std::int64_t v)
    {
        type = INTEGER;
        value.integer = v;
    }
    number_value(double v)
    {
        type = FLOATING;
        value.floating = v;
    }
    union {
        double floating;
        std::int64_t integer;
    } value = {};
    enum { NONE, FLOATING, INTEGER } type = NONE;
};

using namespace eparser::common;

template <typename ItrT1>
number_value scan_number(scanner<ItrT1>& scan)
{

    auto eol = [&scan]() { return scan.eol(); };
    auto is_digit = [&eol, &scan]() {
        return !eol() && helpers::reader::is_digit_(*scan);
    };

    std::int64_t d = 0;
    std::int64_t e = 0;
    double a = 0.0;

    while (is_digit()) {
        if (helpers::reader::is_gap(*scan)) {
            ++scan;
            continue;
        }
        auto value = helpers::reader::char2int(*scan);

        d *= 10;
        d += value;

        a *= 10.0;
        a += value;
        ++scan;
    }
    bool found = false;
    auto scan_backup = scan;

    if (!eol() && *scan == '.') {
        ++scan;
        while (is_digit()) {
            if (helpers::reader::is_gap(*scan)) {
                ++scan;
                continue;
            }
            found = true;
            a *= 10.0;
            a += helpers::reader::char2int(*scan);
            e -= 1;
            ++scan;
        }
    }

    if (!eol() && (*scan == 'e' || *scan == 'E')) {
        int sign = 1;
        int i = 0;
        ++scan;
        if (!eol()) {
            switch (*scan) {
            case '-':
                ++scan;
                sign = -1;
                break;
            case '+':
                ++scan;
                break;
            }
        }
        while (is_digit()) {
            if (helpers::reader::is_gap(*scan)) {
                ++scan;
                continue;
            }
            found = true;
            i *= 10;
            i += helpers::reader::char2int(*scan);
            ++scan;
        }
        e += (i * sign);
    }

    while (e > 0) {
        a *= 10.0;
        e -= 1;
    }

    while (e < 0) {
        a *= 0.1;
        e += 1;
    }

    if (!found) {
        scan = scan_backup;
        return number_value { d };
    } else {
        return number_value { a };
    }
}

int main(int argc, char* argv[])
{
    std::string test = "0.3e+3-!@#$";
    auto scan = make_scanner(test.begin(), test.end());
    auto val = scan_number(scan);

    std::cout << val.type << " i" << val.value.integer << " f"
              << val.value.floating << "\n";
    std::cout << &(*scan.begin()) << "\n";

    return 0;

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
