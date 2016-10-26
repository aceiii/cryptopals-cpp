#include <cassert>
#include <iostream>
#include <vector>
#include <string>

using namespace std::string_literals;

#include "base64.h"
#include "xor.hpp"

int main() {
    auto hex_str = "1c0111001f010100061a024b53535009181c"s;
    auto xor_hex_str = "686974207468652062756c6c277320657965"s;

    auto b1 = hex_to_bytes(hex_str);
    auto b2 = hex_to_bytes(xor_hex_str);
    auto b3 = b1 xor b2;

    std::cout << b3 << std::endl;
}

