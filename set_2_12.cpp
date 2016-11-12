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

aes_mode detect_aes_mode(std::function<byte_vector(const byte_vector&)> oracle_func) {
    constexpr size_t block_size = aes::block_size;

    constexpr size_t bufsize = 128;
    byte_vector buffer(bufsize, '0');
    std::map<byte_vector, int> count;
    auto ciphertext = oracle_func(buffer);

    aes_mode mode = aes_mode::CBC;

    for (int i = 0; i < ciphertext.size(); i += block_size) {
        auto first = next(begin(ciphertext), i);
        auto last = next(first, block_size);
        byte_vector block(first, last);
        count[block] += 1;
    }

    if (std::any_of(begin(count), end(count), [] (const auto& pair) { return pair.second > 1; })) {
        mode = aes_mode::ECB;
    }

    return mode;
}

size_t detect_block_size(std::function<byte_vector(const byte_vector&)> oracle_func) {
    constexpr byte b = 'A';
    size_t block_size = 0;
    while (true) {
        block_size += 1;
        byte_vector buffer(block_size * 2, b);
        byte_vector enc = oracle_func(buffer);

        byte_vector block1(block_size);
        std::copy(begin(enc), next(begin(enc), block_size), begin(block1));

        byte_vector block2(block_size);
        std::copy(next(begin(enc), block_size), next(begin(enc), block_size + block_size), begin(block2));

        if (block1 == block2) {
            return block_size;
        }
    }
}

void test1() {

    auto s = "hello world!!12";
    auto b = encryption_oracle(str_to_bytes(s));

    std::cout << "plaintext:  " << s << std::endl;
    std::cout << "ciphertext: " << b << std::endl;
}

void test2() {
    int block_size = detect_block_size(encryption_oracle);
    assert(block_size == aes::block_size);
    std::cout << "Success!" << std::endl;
}

int main() {
    test2();

    return 0;
}

