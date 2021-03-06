#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "aes.hpp"
#include "random_bytes.hpp"

enum class aes_mode {
    ECB,
    CBC,
};

std::ostream& operator << (std::ostream& os, const aes_mode& mode ) {
    if (mode == aes_mode::ECB) {
        return (os << "ECB");
    }
    return (os << "CBC");
}

struct oracle_data {
    aes_mode mode;
    byte_vector plaintext;
    byte_vector prependtext;
    byte_vector appendtext;
    byte_vector ciphertext;
};

oracle_data encryption_oracle(const byte_vector& bytes) {
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

    aes_mode mode;

    if (rand_coin.next() == 0) {
        mode = aes_mode::ECB;
        output = aes_ecb_encrypt(new_bytes, rand_key);
    } else {
        mode = aes_mode::CBC;
        output = aes_cbc_encrypt(new_bytes, rand_key, rand_iv);
    }

    return oracle_data { mode, bytes, rand_preprend, rand_append, output };
}

oracle_data encryption_oracle(const std::string& str) {
    return encryption_oracle(str_to_bytes(str));
}

std::pair<aes_mode, oracle_data> detect_aes_mode(std::function<oracle_data(const byte_vector&)> oracle_func) {
    constexpr size_t block_size = aes::block_size;

    constexpr size_t bufsize = 128;
    byte_vector buffer(bufsize, '0');
    std::map<byte_vector, int> count;
    oracle_data enc = oracle_func(buffer);

    const byte_vector& ciphertext = enc.ciphertext;
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

    return std::make_pair(mode, enc);
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
    using namespace std::placeholders;

    constexpr int N = 100;

    int matches = 0;
    for (int i = 0; i < N; i += 1) {
        auto d = detect_aes_mode(std::bind<oracle_data(const byte_vector&)>(encryption_oracle, _1));
        const aes_mode& mode = d.first;
        const oracle_data& enc = d.second;

        std::cout << mode << " == " << enc.mode << std::endl;

        if (mode == enc.mode) {
            matches += 1;
        }
    }

    assert(matches == N);
    std::cout << "Success!" << std::endl;

    return 0;
}

