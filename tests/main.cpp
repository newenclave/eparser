#include <chrono>
#include <iostream>

namespace eparser { namespace tests { namespace calc {
    void run();
}}}

int main(int argc, char *argv[])
{
	std::string name;
	if(argc < 2) {
		std::cout << "write test <name>;\ndefault is calc\n";
		name = "calc"
		
	} else {
		name = argv[1];
	}

	if(name == "calc") {
		tests::calc::run();
	}

    return 0;
}
