#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "byte_vector.hpp"
#include "random_int.hpp"
#include "random_bytes.hpp"
#include "aes.hpp"

using namespace std::string_literals;

enum class aes_mode {
    ECB,
    CBC,
};

std::ostream& operator << (std::ostream& os, const aes_mode& mode) {
    return os << (mode == aes_mode::ECB ? "ECB" : "CBC");
}

namespace {
    random_int<int> rand_int(4, aes::block_size);
    random_bytes rand_bytes;

    std::string unknown_string = "Um9sbGluJyBpbiBteSA1LjAKV2l0aCBteSByYWctdG9wIGRvd24gc28gbXkg"
        "aGFpciBjYW4gYmxvdwpUaGUgZ2lybGllcyBvbiBzdGFuZGJ5IHdhdmluZyBq"
        "dXN0IHRvIHNheSBoaQpEaWQgeW91IHN0b3A/IE5vLCBJIGp1c3QgZHJvdmUg"
        "YnkK"s;

    byte_vector encryption_key = rand_bytes.next_n(aes::block_size);
    byte_vector unknown_bytes = base64_to_bytes(unknown_string);
    byte_vector random_prefix = rand_bytes.next_n(rand_int.next());

}

byte_vector encryption_oracle(const byte_vector& bytes) {
    constexpr size_t block_size = aes::block_size;

    int new_size = bytes.size() + unknown_bytes.size() + random_prefix.size();
    if (new_size % block_size != 0) {
        new_size += block_size - (new_size % block_size);
    }

    byte_vector new_bytes(new_size, 0);
    std::copy(begin(random_prefix), end(random_prefix), begin(new_bytes));
    std::copy(begin(bytes), end(bytes), next(begin(new_bytes), random_prefix.size()));
    std::copy(begin(unknown_bytes), end(unknown_bytes), next(begin(new_bytes), random_prefix.size() + bytes.size()));

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

template <typename T>
std::vector<T> split_blocks(const T& buf, const size_t& block_size) {
    const int num_blocks = buf.size() / block_size;
    const int remainder = buf.size() % block_size;

    std::vector<T> blocks;
    for (int i = 0; i < num_blocks; i += 1) {
        auto start = next(begin(buf), i * block_size);
        auto stop = next(start, block_size);
        T block(start, stop);
        blocks.push_back(block);
    }
    if (remainder > 0) {
        auto start = next(begin(buf), num_blocks * block_size);
        auto stop = end(buf);
        T block(start, stop);
        blocks.push_back(block);
    }

    return blocks;
}

size_t detect_block_size(std::function<byte_vector(const byte_vector&)> oracle_func) {
    constexpr byte b = 'A';
    constexpr size_t buffer_size = 1024;
    byte_vector buffer(buffer_size, b);
    byte_vector enc = oracle_func(buffer);

    for (int i = 2; i < buffer_size / 4; i += 1) {
        std::vector<byte_vector> blocks = split_blocks(enc, i);
        std::map<byte_vector, int> block_count;

        if (blocks[2] == blocks[3]) {
            return i;
        }
    }

    return -1;
}

size_t detect_oracle_secret_string_size(const size_t& block_size, const size_t& prefix_size, std::function<byte_vector(const byte_vector&)> oracle_func) {

    const size_t prefix_padding_size = (block_size - prefix_size) % block_size;

    byte_vector buffer(prefix_padding_size, '0');
    buffer.push_back('A');

    int prev_size = oracle_func(buffer).size();
    int padding_size = 1;

    for (padding_size = 2; padding_size <= block_size; padding_size += 1) {
        byte_vector block(prefix_padding_size + padding_size, 'A');
        std::fill(begin(block), next(begin(block), prefix_padding_size), '0');

        byte_vector enc = oracle_func(block);

        if (enc.size() > prev_size) {
            return prev_size - padding_size + 1 - prefix_padding_size - prefix_size;
        }
    }

    return prev_size - padding_size + 1 - prefix_padding_size - prefix_size;
}

size_t detect_oracle_prefix_size(const size_t& block_size, std::function<byte_vector(const byte_vector&)> oracle_func) {

    size_t prefix_size = 1;
    while (true) {
        int padding_offset = (block_size - prefix_size) % block_size;

        byte_vector buffer(padding_offset + (block_size * 2), 'a');
        std::fill(begin(buffer), next(begin(buffer), padding_offset), '0');
        byte_vector enc = oracle_func(buffer);

        std::vector<byte_vector> blocks = split_blocks(enc, block_size);
        if (blocks[1] == blocks[2]) {
            break;
        }
        prefix_size += 1;
    }

    return prefix_size;
}

void test1() {
    constexpr size_t block_size = aes::block_size;
    size_t size = detect_oracle_prefix_size(block_size, encryption_oracle);
    std::cout << size << " == " << random_prefix.size() << std::endl;
    assert(size == random_prefix.size());
    std::cout << "Success!" << std::endl;
}

int main() {

    const int block_size = detect_block_size(encryption_oracle);
    std::cout << "block_size: " << block_size << std::endl;

    const int prefix_size = detect_oracle_prefix_size(block_size, encryption_oracle);
    std::cout << "prefix_size: " << prefix_size << std::endl;

    const int unknown_size = detect_oracle_secret_string_size(block_size, prefix_size, encryption_oracle);
    std::cout << "unknown_size: " << unknown_size << std::endl;

    const int prefix_padding_size = (block_size - prefix_size) % block_size;

    byte_vector dec;
    for (int j = 0; j < unknown_size; j += 1) {

        std::map<byte_vector, byte> block_map;
        int block_check_size = (std::floor(j / block_size) + 1) * block_size;
        int filler_size = block_size - (dec.size() % block_size) - 1;

        for (int i = 0; i < 256; i += 1) {
            byte b = i;

            byte_vector block(filler_size + prefix_padding_size, 'A');
            std::fill(begin(block), next(begin(block), prefix_padding_size), '0');
            std::copy(begin(dec), end(dec), std::back_inserter(block));
            block.push_back(b);

            byte_vector enc = encryption_oracle(block);

            byte_vector enc_block(block_check_size);
            std::copy(next(begin(enc), block_size), next(begin(enc), block_size + block_check_size), begin(enc_block));

            block_map[enc_block] = b;
        }

        byte_vector block(filler_size + prefix_padding_size, 'A');
        std::fill(begin(block), next(begin(block), prefix_padding_size), '0');

        byte_vector enc = encryption_oracle(block);

        byte_vector enc_block(block_check_size);
        std::copy(next(begin(enc), block_size), next(begin(enc), block_size + block_check_size), begin(enc_block));

        assert(block_map.find(enc_block) != end(block_map));
        byte b = block_map[enc_block];

        dec.push_back(b);
    }

    std::cout << std::endl << "Decrypted: " << std::endl;
    std::cout << bytes_to_str(dec) << std::endl;

    return 0;
}

