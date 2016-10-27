#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "hex.hpp"
#include "xor.hpp"

using namespace std::string_literals;

int main() {

    auto str = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal"s;
    auto key = "ICE"s;

    auto str_bytes = str_to_bytes(str);
    auto key_bytes = str_to_bytes(key);

    auto xor_str_bytes = str_bytes ^ cycle_xor_key(key_bytes);
    auto xor_str_hex = bytes_to_hex(xor_str_bytes);

    std::cout << xor_str_hex << std::endl;

    return 0;
}

