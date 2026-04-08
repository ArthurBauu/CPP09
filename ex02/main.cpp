#include "PmergeMe.hpp"

int main(int argc, char** argv) {
    try {
        PmergeMe pm;
        pm.parseArgs(argc, argv);
        pm.sort();
        pm.printResults();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
