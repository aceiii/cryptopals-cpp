#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "aes.hpp"
#include "random_int.hpp"
#include "random_bytes.hpp"

using namespace std::string_literals;

enum class aes_mode {
    ECB,
    CBC,
};

namespace {
    std::string unknown_string = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkg"
        "aGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBq"
        "dXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg"
        "YnkK"s;
    byte_vector encryption_key = random_bytes().next_n(aes::block_size);
    byte_vector unknown_bytes = base64_to_bytes(unknown_string);
}

byte_vector encryption_oracle(const byte_vector& bytes) {
    constexpr size_t block_size = aes::block_size;

    int new_size = bytes.size() + unknown_bytes.size();
    if (new_size % block_size != 0) {
        new_size += block_size - (new_size % block_size);
    }

    byte_vector new_bytes(new_size, 0);
    std::copy(begin(bytes), end(bytes), begin(new_bytes));
    std::copy(begin(unknown_bytes), end(unknown_bytes), next(begin(new_bytes), bytes.size()));

    return aes_ecb_encrypt(new_bytes, encryption_key);
}

int main() {

    auto s = "hello world!!12";
    auto b = encryption_oracle(str_to_bytes(s));

    std::cout << "plaintext:  " << s << std::endl;
    std::cout << "ciphertext: " << b << std::endl;

    return 0;
}

