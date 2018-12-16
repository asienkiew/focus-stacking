#include <iostream>
#include <memory>
#include <exception>

#include "FocusStacking.h"

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cout << "Usage: converter <input Dir> <output Dir>\n";
        return 0;
    }
    try {
        auto focus = std::make_unique<FocusStacking>(std::string(argv[1]), std::string(argv[2]));
        focus->generateResult();
        focus->writeResult();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}

