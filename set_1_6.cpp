#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <bitset>

#include "file.hpp"
#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"

using namespace std::string_literals;

size_t hamming_weight(const byte& b) {
    std::bitset<8> bits(b);
    return bits.count();
}

template <typename T>
size_t hamming_distance(const T& t1, const T& t2) {
    auto len1 = t1.size();
    auto len2 = t2.size();
    auto min_len = std::min(len1, len2);
    auto max_len = std::max(len1, len2);
    auto dist = (max_len - min_len) * sizeof(typename T::value_type) * 8;

    for (auto i = 0; i < min_len; i += 1) {
        auto b = t1[i] ^ t2[i];
        dist += hamming_weight(b);
    }

    return dist;
}

void test1() {
    auto str1 = "this is a test"s;
    auto str2 = "wokka wokka!!!"s;
    auto ham = hamming_distance(str1, str2);

    std::cout << ham << std::endl;
}


int main() {
    std::vector<std::string> corpus = read_lines_from_file("miss_jemima.txt");
    byte_vector corpus_bytes;
    for (const auto &line : corpus) {
        auto byte_line = str_to_bytes(line);
        std::copy(byte_line.begin(), byte_line.end(), std::back_inserter(corpus_bytes));
    }
    auto corpus_freq_map = map_byte_frequency(corpus_bytes);
    auto corpus_freq_list = freq_map_to_list(corpus_freq_map);

    byte_vector lines;
    auto str_lines = read_lines_from_file("6.txt");
    for (const auto &line : str_lines) {
        auto b64_line = base64_to_bytes(line);
        copy(begin(b64_line), end(b64_line), std::back_inserter(lines));
    }

    const int max_keysize = 40;
    float min_ham = std::numeric_limits<float>::max();
    int actual_keysize = 0;
    for (int keysize = 2; keysize <= max_keysize; keysize += 1) {
        byte_vector block1;
        byte_vector block2;
        byte_vector block3;
        byte_vector block4;

        copy(begin(lines), begin(lines) + keysize, back_inserter(block1));
        copy(begin(lines) + keysize, begin(lines) + (2*keysize), back_inserter(block2));
        copy(begin(lines) + (2*keysize), begin(lines) + (3*keysize), back_inserter(block3));
        copy(begin(lines) + (3*keysize), begin(lines) + (4*keysize), back_inserter(block4));

        auto ham1 = hamming_distance(block1, block2);
        auto ham2 = hamming_distance(block2, block3);
        auto ham3 = hamming_distance(block3, block4);
        auto ham4 = hamming_distance(block1, block3);
        auto ham5 = hamming_distance(block1, block4);
        auto ham6 = hamming_distance(block2, block4);
        auto ham = (ham1 + ham2 + ham3 + ham4 + ham5 + ham6) / 6.0f;

        float normalized_ham = (float)ham / (float)keysize;

        if (normalized_ham < min_ham) {
            min_ham = normalized_ham;
            actual_keysize = keysize;
        }
    }

    std::cout << "Keysize: " << actual_keysize << std::endl;

    std::vector<byte_vector> blocks (actual_keysize);
    for (int i = 0; i < lines.size(); i += 1) {
        blocks[i % actual_keysize].push_back(lines[i]);
    }

    byte_vector key(actual_keysize);
    std::vector<byte_vector> decoded_blocks(actual_keysize);

    for (int i = 0; i < actual_keysize; i += 1) {
        double selected_decoded_score = std::numeric_limits<double>::infinity();
        byte_vector selected_decoded_block;
        const auto &block = blocks[i];

        for (int k = 0; k < 256; k += 1) {
            auto decoded = block ^ k;
            auto decoded_freq_map = map_byte_frequency(decoded);
            auto decoded_freq_list = freq_map_to_list(decoded_freq_map);
            double score = compare_freq_list(corpus_freq_list, decoded_freq_list);

            if (score < selected_decoded_score) {
                selected_decoded_score = score;
                selected_decoded_block = decoded;
            }
        }

        decoded_blocks[i] = selected_decoded_block;
    }

    byte_vector decoded_lines(lines.size());
    for (int i = 0; i < decoded_lines.size(); i += 1) {
        decoded_lines[i] = decoded_blocks[i % actual_keysize][i / actual_keysize];
    }

    std::cout << "Key: " << bytes_to_str(key) << std::endl;
    std::cout << bytes_to_str(decoded_lines) << std::endl;

    return 0;
}


