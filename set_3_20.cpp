#include <iostream>
#include <vector>
#include <cassert>
#include <fstream>
#include <algorithm>
#include <utility>

#include "file.hpp"
#include "xor.hpp"
#include "base64.hpp"
#include "aes.hpp"
#include "random_bytes.hpp"
#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "byte_util.hpp"

using namespace std::string_literals;

namespace {
    const byte_vector random_key = random_bytes().next_n(aes::block_size);
    const uint64_t nonce = 0;
}

auto main() -> int {
    auto corpus_file = "miss_jemima.txt"s;
    auto corpus_freq_map = map_byte_frequency_from_file(corpus_file);
    auto corpus_freq_list = freq_map_to_list(corpus_freq_map);

    int min_length = std::numeric_limits<int>::max();
    int max_length = std::numeric_limits<int>::min();

    std::vector<byte_vector> lines = lines_to_bytes(read_lines_from_file("20.txt"));
    std::vector<byte_vector> ciphertexts;

    for (const auto &input_bytes : lines) {
        byte_vector ciphertext = aes_ctr_encrypt(input_bytes, random_key, nonce);
        ciphertexts.push_back(ciphertext);

        min_length = std::min(min_length, (int)ciphertext.size());
        max_length = std::max(max_length, (int)ciphertext.size());
    }

    const int keysize = min_length;

    // transpose blocks
    auto block_length = ciphertexts.size();
    auto blocks = transpose_blocks(ciphertexts, keysize); // truncates to common length

    std::vector<byte_vector> decoded_blocks(keysize);

    for (int i = 0; i < blocks.size(); i += 1) {
        double selected_decoded_score = std::numeric_limits<double>::infinity();
        byte_vector selected_decoded_block;
        const auto &block = blocks[i];

        for (int k = 0; k < 256; k += 1) {
            auto decoded = block ^ k;
            auto freq = map_byte_frequency(decoded);
            auto freq_list = freq_map_to_list(freq);
            double score = compare_freq_list(corpus_freq_list, freq_list);

            if (score < selected_decoded_score) {
                selected_decoded_score = score;
                selected_decoded_block = decoded;
            }
        }

        decoded_blocks[i] = selected_decoded_block;
    }

    auto final_blocks = transpose_blocks(decoded_blocks, block_length);

    for (const auto &b : final_blocks) {
        std::cout << bytes_to_str(b) << std::endl;
    }

    return 0;
}

