#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>

#include "byte_vector.hpp"

using namespace std::string_literals;

size_t hamming_weight(const byte& b) {
    std::bitset<8> bits(b);
    return bits.count();
}

template <typename T>
size_t hamming_distance(const T& t1, const T& t2) {
    auto len1 = t1.size();
    auto len2 = t2.size();
    auto min_len = std::min(len1, len2);
    auto max_len = std::max(len1, len2);
    auto dist = (max_len - min_len) * sizeof(typename T::value_type) * 8;

    for (auto i = 0; i < min_len; i += 1) {
        auto b = t1[i] ^ t2[i];
        dist += hamming_weight(b);
    }

    return dist;
}

int main() {
    auto keysize = 2;
    auto str1 = "this is a test"s;
    auto str2 = "wokka wokka!!!"s;

    auto ham = hamming_distance(str1, str2);

    std::cout << ham << std::endl;

    return 0;
}

