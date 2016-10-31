#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "hex.hpp"
#include "padding.hpp"

using namespace std::string_literals;

int main() {
    auto str = "YELLOW SUBMARINE"s;
    auto str_20 = padded_pkcs7(str, 20);
    assert(str_20 == "YELLOW SUBMARINE\04\04\04\04"s);
    std::cout << str_to_bytes(str_20) << std::endl;

    auto s1 = "Hello World?!"s;
    pad_pkcs7(s1, 16);
    assert(s1 == "Hello World?!\03\03\03"s);
    std::cout << str_to_bytes(s1) << std::endl;

    auto s2 = "Welcome to the jungle!"s;
    auto s3 = padded_pkcs7(s2, 24);
    auto s4 = "Welcome to the jungle!\02\02"s;
    assert(s3 == s4);
    std::cout << str_to_bytes(s3) << std::endl;

    auto s5 = "The END is nigh"s;
    assert(padded_pkcs7(s5, 16) == "The END is nigh\01"s);
    std::cout << str_to_bytes(padded_pkcs7(s5, 16)) << std::endl;

    auto s6 = "Wtf! What are you doing?"s;
    auto s6p(s6);
    pad_pkcs7(s6p, 24);
    assert(s6p == s6);
    std::cout << str_to_bytes(s6p) << std::endl;


    return 0;
}

