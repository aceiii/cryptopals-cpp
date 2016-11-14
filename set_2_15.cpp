#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "byte_vector.hpp"
#include "padding.hpp"

using namespace std::string_literals;

bool has_valid_pkcs7_padding(const byte_vector& bytes) {
    if (bytes.size() == 0) {
        return false;
    }

    const int size = bytes.size();
    byte last_byte = bytes[size - 1];

    bool is_valid = std::all_of(next(begin(bytes), size - last_byte), end(bytes),
        [&] (const byte& b) { return b == last_byte; });

    return is_valid;
}

bool strip_pkcs7_padding(const std::string& in, std::string& out) {
}

void test1() {
    auto s1 = "ICE ICE BABY\x04\x04\x04\x04"s;
    auto s2 = "ICE ICE BABY\x05\x05\x05\x05"s;
    auto s3 = "ICE ICE BABY\x01\x02\x03\x04"s;

    assert(has_valid_pkcs7_padding(str_to_bytes(s1)) == true);
    assert(has_valid_pkcs7_padding(str_to_bytes(s2)) == false);
    assert(has_valid_pkcs7_padding(str_to_bytes(s3)) == false);

    std::cout << "Success!" << std::endl;
}

int main() {
    test1();
    return 0;
}

