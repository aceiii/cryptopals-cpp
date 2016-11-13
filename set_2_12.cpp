#include <iostream>
#include <string>
#include <vector>
#include <map>

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

std::ostream& operator << (std::ostream& os, const aes_mode& mode) {
    return os << (mode == aes_mode::ECB ? "ECB" : "CBC");
}

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

size_t detect_oracle_secret_string_size(const int block_size, std::function<byte_vector(const byte_vector&)> oracle_func) {
    int prev_size = oracle_func(str_to_bytes("A")).size();
    int padding_size = 1;

    for (padding_size = 2; padding_size <= block_size; padding_size += 1) {
        byte_vector block(padding_size, 'A');
        byte_vector enc = oracle_func(block);

        if (enc.size() > prev_size) {
            return prev_size - padding_size + 1;
        }
    }

    return prev_size - padding_size + 1;
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
    const int block_size = detect_block_size(encryption_oracle);
    std::cout << "block_size: " << block_size << std::endl;

    const aes_mode mode = detect_aes_mode(encryption_oracle);
    std::cout << "aes_mode: " << mode << std::endl;

    const int unknown_size = detect_oracle_secret_string_size(block_size, encryption_oracle);
    std::cout << "unknown_size: " << unknown_size << std::endl;

    byte_vector dec;
    for (int j = 0; j < unknown_size; j += 1) {

        std::map<byte_vector, byte> block_map;
        int block_check_size = (std::floor(j / block_size) + 1) * block_size;
        int filler_size = block_size - (dec.size() % block_size) - 1;

        for (int i = 0; i < 256; i += 1) {
            byte b = i;

            byte_vector block(filler_size, 'A');
            std::copy(begin(dec), end(dec), std::back_inserter(block));
            block.push_back(b);

            byte_vector enc = encryption_oracle(block);

            byte_vector enc_block(block_check_size);
            std::copy(begin(enc), next(begin(enc), block_check_size), begin(enc_block));

            block_map[enc_block] = b;
        }

        byte_vector block(filler_size, 'A');
        byte_vector enc = encryption_oracle(block);

        byte_vector enc_block(block_check_size);
        std::copy(begin(enc), next(begin(enc), block_check_size), begin(enc_block));

        byte b = block_map[enc_block];

        dec.push_back(b);
    }

    std::cout << std::endl << "Decrypted: " << std::endl;
    std::cout << bytes_to_str(dec) << std::endl;

    return 0;
}

