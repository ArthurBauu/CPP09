#include "RPN.hpp"
#include <iostream>
#include <string>


// int main(int argc, char* argv[]) {
//     if (argc != 2) {
//         std::cerr << "Usage: " << argv[0] << " \"<expression>\"" << std::endl;
//         return 1;
//     }

//     RPN rpn;
//     try {
//         std::cout << rpn.evaluate(argv[1]) << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }




static void runTest(const std::string& expr) {
    RPN rpn;
    std::cout << "[\"" << expr << "\"] -> ";
    try {
        std::cout << rpn.evaluate(expr) << std::endl;
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc == 2) {
        RPN rpn;
        try {
            std::cout << rpn.evaluate(argv[1]) << std::endl;
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return 1;
        }
        return 0;
    }

    const std::string tests[] = {
        "+",
        "3 4 + +",
        "1 2",
        "3 0 /",
        "12 3",
        "-2 3 +",
        "2 3 &",
        "3     5    +",
        "7 3 /",
        "7 -3 /",
    };

    const int n = sizeof(tests) / sizeof(tests[0]);
    for (int i = 0; i < n; i++)
        runTest(tests[i]);

    return 0;
}




