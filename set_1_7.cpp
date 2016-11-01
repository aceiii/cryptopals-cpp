#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "hex.hpp"
#include "xor.hpp"
#include "aes.hpp"

using namespace std::string_literals;

void test1() {
    std::vector<std::string> lines = {
        "6bc1bee22e409f96e93d7e117393172a"s,
        "ae2d8a571e03ac9c9eb76fac45af8e51"s,
        "30c81c46a35ce411e5fbc1191a0a52ef"s,
        "f69f2445df4f9b17ad2b417be66c3710"s
    };

    auto key = "2b7e151628aed2a6abf7158809cf4f3c"s;
    auto key_bytes = hex_to_bytes(key);

    std::for_each(begin(lines), end(lines), [&key_bytes] (const std::string& line) {
        auto bytes = hex_to_bytes(line);

        byte_vector enc = aes_ecb_encrypt(bytes, key_bytes);
        byte_vector dec = aes_ecb_decrypt(enc, key_bytes);

        std::cout << bytes_to_hex(enc) << " => " << bytes_to_hex(dec) << std::endl;
    });
}

void test2() {
    auto txt = "wTfiSgOinGoO0on!"s;
    auto key = "YELLOW SUBMARINE"s;

    auto txt_bytes = str_to_bytes(txt);
    auto key_bytes = str_to_bytes(key);

    auto enc = aes_ecb_encrypt(txt_bytes, key_bytes);

    std::cout << bytes_to_hex(enc) << std::endl;
    std::cout << bytes_to_base64(enc) << std::endl;

    auto dec = aes_ecb_decrypt(enc, key_bytes);
    auto str = bytes_to_str(dec);

    std::cout << bytes_to_hex(dec) << std::endl;
    std::cout << bytes_to_base64(dec) << std::endl;
    std::cout << str << std::endl;
}

int main() {
    std::string lines;

    auto filename = "7.txt"s;
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

    const int block_size = 16;

    assert(byte_lines.size() % block_size == 0);

    byte_vector dec_lines = aes_ecb_decrypt(byte_lines, key_bytes);

    std::string dec_str = bytes_to_str(dec_lines);
    std::cout << dec_str << std::endl;

    return 0;
}

