#include <cstdlib>
#include <exception>
#include <memory>

#include "FocusStacking.h"

int main(int argc, char** argv) {
    std::stringstream fileName;
    if (argc < 3) {
        throw std::invalid_argument("Bad number of files");
    }

    auto focus = std::make_unique<FocusStacking>(std::string(argv[1]), std::string(argv[2]));
    focus->generateResult();
    focus->writeResult();
    return 0;
}

