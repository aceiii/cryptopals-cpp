#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "byte_vector.hpp"
#include "byte_util.hpp"
#include "padding.hpp"

using namespace std::string_literals;

void test1() {
    auto s1 = "ICE ICE BABY\x04\x04\x04\x04"s;
    auto s2 = "ICE ICE BABY\x05\x05\x05\x05"s;
    auto s3 = "ICE ICE BABY\x01\x02\x03\x04"s;

    assert(has_valid_pkcs7_padding(str_to_bytes(s1)) == true);
    assert(has_valid_pkcs7_padding(str_to_bytes(s2)) == false);
    assert(has_valid_pkcs7_padding(str_to_bytes(s3)) == false);

    std::cout << "Success!" << std::endl;
}

void test2() {
    auto b1_in = str_to_bytes("ICE ICE BABY\x04\x04\x04\x04"s);
    auto b2_in = str_to_bytes("ICE ICE BABY\x05\x05\x05\x05"s);
    auto b3_in = str_to_bytes("ICE ICE BABY\x01\x02\x03\x04"s);

    auto b1_res = str_to_bytes("ICE ICE BABY"s);

    byte_vector b1_out, b2_out, b3_out;

    assert(strip_pkcs7_padding(b1_in, b1_out) && (b1_out == b1_res));
    assert(strip_pkcs7_padding(b2_in, b2_out) == false);
    assert(strip_pkcs7_padding(b3_in, b3_out) == false);

    std::cout << "Success!" << std::endl;
}

int main() {
    //test1();
    test2();
    return 0;
}

