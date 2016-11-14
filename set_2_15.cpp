#include <iostream>
#include <string>
#include <vector>
#include <cassert>

#include "byte_vector.hpp"
#include "padding.hpp"

using namespace std::string_literals;

int get_pkcs7_padding(const byte_vector& bytes) {
    if (bytes.size() == 0) {
        return false;
    }

    const int size = bytes.size();
    byte last_byte = bytes[size - 1];

    bool is_valid = std::all_of(next(begin(bytes), size - last_byte), end(bytes),
        [&] (const byte& b) { return b == last_byte; });

    return is_valid ? last_byte : -1;
}

bool has_valid_pkcs7_padding(const byte_vector& bytes) {
    return get_pkcs7_padding(bytes) != -1;
}

bool strip_pkcs7_padding(const byte_vector& in, byte_vector& out) {
    int padding_size = get_pkcs7_padding(in);
    if (padding_size == -1) {
        return false;
    }

    out = byte_vector(begin(in), next(begin(in), in.size() - padding_size));
    return true;
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
    test2();
    return 0;
}

