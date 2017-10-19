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

using namespace std::string_literals;

namespace {
    const byte_vector random_key = random_bytes().next_n(aes::block_size);
    const uint64_t nonce = 0;
}

auto main() -> int {
    std::vector<std::string> corpus = read_lines_from_file("miss_jemima.txt");
    size_t corpus_total_chars = 0;
    std::map<byte, size_t> corpus_char_counts;
    std::map<byte, double> corpus_char_frequency;
    for (const auto &line : corpus) {
        for (const auto &c : line) {
            corpus_total_chars += 1;
            corpus_char_counts[c] += 1;
            corpus_char_frequency[c] = double(corpus_char_counts[c]) / double(corpus_total_chars);
        }
    }

    std::vector<std::pair<byte, double>> corpus_frequency_list;
    std::copy(corpus_char_frequency.begin(), corpus_char_frequency.end(), std::back_inserter(corpus_frequency_list));

    std::sort(corpus_frequency_list.begin(), corpus_frequency_list.end(), [] (const auto &p1, const auto &p2) {
        return p1.second > p2.second;
    });

    std::vector<std::string> lines = read_lines_from_file("20.txt");

    std::vector<byte_vector> ciphertexts;

    int min_length = std::numeric_limits<int>::max();
    int max_length = std::numeric_limits<int>::min();
for (const auto &line : lines) {
        byte_vector input_bytes = base64_to_bytes(line);
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
        const byte_vector &block = blocks[i];

        auto freq = map_byte_frequency(block);
        std::vector<std::pair<byte, double>> freq_list;
        for (const auto &p : freq) {
            freq_list.push_back(std::make_pair(p.first, double(p.second) / double(block.size())));
        }

        sort(freq_list.begin(), freq_list.end(), [] (const auto &p1, const auto &p2) {
            return p1.second > p2.second;
        });

        bool stop = false;
        int k = 0;
        do {
            decoded_blocks[i] = block ^ freq_list[0].first ^ corpus_frequency_list[k].first;
            if (k >= corpus_frequency_list.size() || std::all_of(decoded_blocks[i].begin(), decoded_blocks[i].end(),
                [] (const auto &b) {
                    return std::isdigit(b) || std::isalpha(b) || b == ' ' || b == '.' || b == '-' || b == '?' || b == ';' || b == '(' || b == ')';
                }))
            {
                stop = true;
            }
            k += 1;
        } while (!stop);
    }

    auto final_blocks = transpose_blocks(decoded_blocks, block_length);

    for (const auto &b : final_blocks) {
        std::cout << bytes_to_str(b) << std::endl;
    }

    return 0;
}

