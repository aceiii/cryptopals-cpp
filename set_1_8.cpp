#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "hex.hpp"
#include "aes.hpp"

using namespace std::string_literals;

bool detect_ecb(const byte_vector& bytes) {
    const int block_size = 16;

    assert(bytes.size() % block_size == 0);

    std::map<std::string, int> block_count;

    for (int pos = 0; pos < bytes.size(); pos += block_size) {
        byte_vector block(block_size, 0);

        auto start = next(begin(bytes), pos);
        auto stop = next(start, block_size);

        std::copy(start, stop, begin(block));

        std::string str = bytes_to_str(block);

        block_count[str] += 1;
    }

    return std::any_of(begin(block_count), end(block_count),
        [] (const auto& pair) { return pair.second > 1; });
}

int main() {
    auto filename = "8.txt"s;

    int i = 0;
    std::ifstream file(filename);
    while (!file.fail()) {
        i += 1;

        std::string line;
        file >> line;

        if (file.eof()) {
            break;
        }

        if (detect_ecb(hex_to_bytes(line))) {
            std::cout << "Found at line " << i << std::endl;
            std::cout << line << std::endl << std::endl;
        }
    }

    return 0;
}

