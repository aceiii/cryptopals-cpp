#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "hex.hpp"
#include "xor.hpp"
#include "aes.hpp"

using namespace std::string_literals;

void test1() {
    auto str = "Helllo wo00rld!1"s;
    auto key = "YellOW SubmARINE"s;

    auto str_bytes = str_to_bytes(str);
    auto key_bytes = str_to_bytes(key);

    auto iv = zero_iv();

    auto enc = aes_cbc_encrypt(str_bytes, key_bytes, iv);
    auto dec = aes_cbc_decrypt(enc, key_bytes, iv);
    auto dec_str = bytes_to_str(dec);

    std::cout << dec_str << std::endl;
    assert(dec_str == str);


    auto str2 = "A much longer string than before"s;
    auto key2 = "OhWTF is goig on"s;
    auto iv2 = "YELLOW SUBMARINE"s;

    auto enc2 = aes_cbc_encrypt(str_to_bytes(str2), str_to_bytes(key2), str_to_bytes(iv2));
    auto dec2 = aes_cbc_decrypt(enc2, str_to_bytes(key2), str_to_bytes(iv2));

    auto dec2_str = bytes_to_str(dec2);
    assert(dec2_str == str2);

    std::cout << dec2_str << std::endl;
}

int main() {
    auto filename = "10.txt"s;

    std::string lines;
    std::ifstream file(filename);
    while (!file.fail()) {
        std::string line;
        file >> line;

        if (file.eof()) {
            break;
        }

        std::copy(begin(line), end(line), std::back_inserter(lines));
    }

    auto byte_lines = base64_to_bytes(lines);

    auto key = "YELLOW SUBMARINE"s;
    auto key_bytes = str_to_bytes(key);

    auto dec = aes_cbc_decrypt(byte_lines, key_bytes, zero_iv());
    auto dec_str = bytes_to_str(dec);

    std::cout << dec_str << std::endl;

    return 0;
}

