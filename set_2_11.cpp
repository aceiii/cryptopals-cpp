#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "aes.hpp"
#include "random_bytes.hpp"

byte_vector encryption_oracle(const byte_vector& bytes) {
    random_int<int> rand_count(5, 10);
    random_bytes rand_bytes;
    random_int<int> rand_coin(0, 1);

    byte_vector rand_preprend = rand_bytes.next_n(rand_count.next());
    byte_vector rand_append = rand_bytes.next_n(rand_count.next());

    int remainder = (aes::block_size - ((bytes.size() + rand_preprend.size() + rand_append.size()) % aes::block_size)) % aes::block_size;

    byte_vector new_bytes(bytes.size() + rand_preprend.size() + rand_append.size() + remainder);

    auto start = begin(new_bytes);
    std::copy(begin(rand_preprend), end(rand_preprend), next(start, 0));
    std::copy(begin(bytes), end(bytes), next(start, rand_preprend.size()));
    std::copy(begin(rand_append), end(rand_append), next(start, rand_preprend.size() + bytes.size()));

    byte_vector rand_key = rand_bytes.next_n(aes::block_size);
    byte_vector rand_iv = rand_bytes.next_n(aes::block_size);
    byte_vector output;

    if (rand_coin.next() == 0) {
        output = aes_ecb_encrypt(new_bytes, rand_key);
    } else {
        output = aes_cbc_encrypt(new_bytes, rand_key, rand_iv);
    }

    return output;
}

byte_vector encryption_oracle(const std::string& str) {
    return encryption_oracle(str_to_bytes(str));
}

void test1() {
    constexpr int LO = 5;
    constexpr int HI = 10;

    random_int<int> rand_count(LO, HI);
    int upper = std::numeric_limits<int>::min();
    int lower = std::numeric_limits<int>::max();

    constexpr size_t N = 10000;
    for (int i = 0; i < N; i += 1) {
        int rand_val = rand_count.next();
        upper = std::max(upper, rand_val);
        lower = std::min(lower, rand_val);
    }

    assert(lower == LO);
    assert(upper == HI);

    std::cout << "Succcess" << std::endl;
}

int main() {
    byte_vector enc = encryption_oracle("You're weakenin' fast, YO! and I can tell it!wtf");
    std::cout << enc << std::endl;

    return 0;
}

