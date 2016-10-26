#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "byte_vector.hpp"
#include "hex.hpp"
#include "xor.hpp"
#include "base64.hpp"
#include "byte_freqency.hpp"

using namespace std::string_literals;

struct is_ascii_predicate {
    bool operator () (const byte& b) const {
        return (b >= 32 && b < 127) || b == 0;
    }
};

int main() {
    auto filename = "4.txt"s;
    std::vector<std::string> lines;

    {
        std::ifstream file(filename);
        while (!file.fail()) {
            std::string line;
            file >> line;

            if (file.fail()) {
                break;
            }

            //std::cout << line << std::endl;
            lines.push_back(line);
        }
        file.close();
    }

    for (auto line: lines) {
        auto bytes = hex_to_bytes(line);
        auto freq = map_byte_frequency(bytes);
        auto freq_vec = map_to_vector(freq);

        sort_freq_vec_desc(freq_vec);

        auto decipher = bytes ^ freq_vec[0].first;
        auto str = bytes_to_str(decipher);

        if (std::all_of(begin(str), end(str), is_ascii_predicate())) {
            std::cout << decipher << std::endl;
        }
    }

    return 0;
}

