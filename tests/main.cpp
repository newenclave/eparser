#include <chrono>
#include <iostream>

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
        eparser::tests::calc::run();
    } else if (name == "custom") {
        eparser::tests::custom_parser::run();
    } else if (name == "plot") {
		eparser::tests::plot::run();
	}

    return 0;
}
