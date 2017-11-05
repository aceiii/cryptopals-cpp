#include <iostream>
#include <vector>
#include <chrono>

#include "xor.hpp"
#include "aes.hpp"
#include "byte.hpp"
#include "byte_vector.hpp"
#include "byte_util.hpp"
#include "file.hpp"
#include "random_bytes.hpp"
#include "base64.hpp"

using namespace std::string_literals;

namespace {
    const byte_vector random_key = random_bytes().next_n(aes::block_size);
}

byte_vector decrypt_aes_ecb_file(const std::string &filename, const byte_vector &key) {
    auto lines = read_lines_from_file(filename);

    std::string combined_lines;
    for (const auto &line : lines) {
        std::copy(line.begin(), line.end(), std::back_inserter(combined_lines));
    }

    auto byte_lines = base64_to_bytes(combined_lines);
    auto decrypted_bytes = aes_ecb_decrypt(byte_lines, key);

    return decrypted_bytes;
}

void edit_aes_ctr(byte_vector &edit_bytes, const byte_vector &key, size_t offset, const std::string &new_text, uint64_t nonce = 0) {
    auto block_size = aes::block_size;
    auto start_index = offset / block_size;
    auto end_index = ((offset + new_text.size() + block_size - 1) / block_size);

    auto cipher = aes(key);

    //std::cout << "block_size: " << block_size << std::endl;
    //std::cout << "start_index: " << start_index << std::endl;
    //std::cout << "end_index: " << end_index << std::endl;

    uint64_t counter = (uint64_t)start_index;
    byte_vector nonce_block(block_size);
    std::memcpy(nonce_block.data(), &nonce, sizeof(nonce));

    std::string reversed_new_text(new_text);
    std::reverse(reversed_new_text.begin(), reversed_new_text.end());

    for (int i = start_index; i < end_index; i += 1) {
        std::memcpy(nonce_block.data() + sizeof(nonce), &counter, sizeof(counter));

        byte_vector encrypted_block(block_size);
        std::copy(edit_bytes.begin() + (i * block_size), edit_bytes.begin() + ((i + 1) * block_size), std::begin(encrypted_block));


        byte_vector block1 = cipher.encrypt(nonce_block);
        byte_vector block2 = block1 ^ encrypted_block;

        for (int j = 0; j < block_size; j += 1) {
            int k = (i * block_size) + j;
            if (k >= offset && k < offset + new_text.size()) {
                block2[j] = reversed_new_text.back();
                reversed_new_text.pop_back();
            }
        }

        byte_vector block3 = block1 ^ block2;

        std::copy(block3.begin(), block3.end(), edit_bytes.begin() + (i * block_size));

        counter += 1;
    }
}


int main() {
    byte_vector file_key = str_to_bytes("YELLOW SUBMARINE"s);
    byte_vector dec = decrypt_aes_ecb_file("25.txt"s, file_key);
    //std::cout << bytes_to_str(dec) << std::endl;

    byte_vector reencrypted = aes_ctr_encrypt(dec, random_key);
    //std::cout << bytes_to_base64(reencrypted) << std::endl;

    //byte_vector dec2 = aes_ctr_decrypt(reencrypted, random_key);
    //std::cout << bytes_to_str(dec2) << std::endl;

    //edit_aes_ctr(reencrypted, random_key, 0, "ZZZZZZZ"s);

    size_t total_size = reencrypted.size();

    for (int i = 0; i < (int)total_size; i += 1) {
        byte orig_byte = reencrypted[i];

        edit_aes_ctr(reencrypted, random_key, i, "A"s);

        byte xor_byte = 'A' ^ reencrypted[i];

        byte dec_byte = xor_byte ^ orig_byte;

        std::cout << dec_byte;
    }

    return 0;
}

