#include <iostream>
#include <random>

#include "mt19937.hpp"

auto main() -> int {
    const uint32_t seed = 5489u;
    int N = 10240;

    // The 10000th consecutive invocation of a default-contructed std::mt19937 is required to produce the value 4123659995.

    std::mt19937 std_mt(seed);
    mt19937 mt(seed);

    for (int i = 0; i < N; i += 1) {
        uint32_t a = std_mt();
        uint32_t b = mt.extract_number();

        if (i < 10 || i == 9999 || a != b) {
            std::cout << "(" << i << ") a: " << a << std::endl;
            std::cout << "(" << i << ") b: " << b << std::endl;
        }

        if (a != b) {
            std::cout << "ERROR: a != b" << std::endl;
            break;
        }
    }

    return 0;
}

