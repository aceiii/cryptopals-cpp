#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "base64.hpp"

int main() {

    auto s = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";

    auto b = hex_to_bytes(s);

    std::cout << bytes_to_base64(b) << std::endl;

    return 0;
}

