#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <set>

#include "byte_vector.hpp"
#include "aes.hpp"
#include "random_bytes.hpp"
#include "padding.hpp"
#include "base64.hpp"
#include "blocks.hpp"

namespace {
    std::vector<std::string> rand_strings = {
		"MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc="s,
		"MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic="s,
		"MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw=="s,
		"MDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg=="s,
		"MDAwMDA0QnVybmluZyAnZW0sIGlmIHlvdSBhaW4ndCBxdWljayBhbmQgbmltYmxl"s,
		"MDAwMDA1SSBnbyBjcmF6eSB3aGVuIEkgaGVhciBhIGN5bWJhbA=="s,
		"MDAwMDA2QW5kIGEgaGlnaCBoYXQgd2l0aCBhIHNvdXBlZCB1cCB0ZW1wbw=="s,
		"MDAwMDA3SSdtIG9uIGEgcm9sbCwgaXQncyB0aW1lIHRvIGdvIHNvbG8="s,
		"MDAwMDA4b2xsaW4nIGluIG15IGZpdmUgcG9pbnQgb2g="s,
		"MDAwMDA5aXRoIG15IHJhZy10b3AgZG93biBzbyBteSBoYWlyIGNhbiBibG93"s,
    };

	random_int<int> rand_int(0, rand_strings.size() - 1);
	random_bytes rand_bytes;

    const byte_vector rand_key = rand_bytes.next_n(aes::block_size);
}

byte_vector select_random_string() {
    return base64_to_bytes(rand_strings[rand_int.next()]);
}

std::pair<byte_vector, byte_vector> random_encrypt() {
    constexpr size_t block_size = aes::block_size;

    byte_vector buffer = select_random_string();
    size_t buffer_size = ((buffer.size() / block_size) + 1) * block_size;
    pad_pkcs7(buffer, buffer_size);

    byte_vector iv = rand_bytes.next_n(block_size);
    byte_vector ciphertext = aes_cbc_encrypt(buffer, rand_key, iv);

    return std::make_pair(ciphertext, iv);
}

bool decryption_oracle(const std::pair<byte_vector, byte_vector>& ciphertext_iv_pair) {
    const byte_vector& ciphertext = ciphertext_iv_pair.first;
    const byte_vector& iv = ciphertext_iv_pair.second;

    byte_vector dec = aes_cbc_decrypt(ciphertext, rand_key, iv);

    return has_valid_pkcs7_padding(dec);
}

byte_vector solve_ciphertext(const byte_vector& block, const byte_vector& iv, const byte_vector& partial_solve = byte_vector(aes::block_size, 0)) {
    constexpr size_t block_size = aes::block_size;

    byte_vector current_block(block_size, 0);

    for (int pos = block_size - 1; pos >= 0; pos -= 1) {
        int i = 1;
        int b = block_size - pos;

        for (; i < 256; i += 1) {

            byte_vector new_iv = iv;
            for (int j = block_size - 1; j > pos; j -= 1) {
                new_iv[j] = new_iv[j] xor (b xor current_block[j]);
            }
            new_iv[pos] = new_iv[pos] xor i;

            auto pair = std::make_pair(block, new_iv);

            bool is_valid = decryption_oracle(pair);

            if (is_valid) {
                current_block[pos] = b ^ i;
                break;
            }
        }

        if (i > 255) {
            current_block[pos] = b;
        }
    }

    return current_block;
}

void test1() {
    auto enc = random_encrypt();

    std::cout << "ciphertext: " << bytes_to_hex(enc.first) << std::endl;
    std::cout << "iv: " << bytes_to_hex(enc.second) << std::endl;

    std::cout << "valid: " << std::boolalpha << decryption_oracle(enc) << std::endl;
}

int main() {

    constexpr size_t block_size = aes::block_size;

    auto enc = random_encrypt();
    const byte_vector& ciphertext = enc.first;
    const byte_vector& iv = enc.second;

    std::vector<byte_vector> blocks = split_blocks(ciphertext, block_size);
    std::vector<byte_vector> decrypted_blocks;

    for (int i = 0; i < blocks.size(); i += 1) {
        if (i == 0) {
            decrypted_blocks.push_back(solve_ciphertext(blocks[0], iv));
        } else {
            decrypted_blocks.push_back(solve_ciphertext(blocks[i], blocks[i-1]));
        }
    }

    byte_vector decrypted = combine_blocks(decrypted_blocks);
    byte_vector plaintext;

    strip_pkcs7_padding(decrypted, plaintext);

    std::cout << "plaintext: " << bytes_to_str(plaintext) << std::endl;
    std::cout << "bytes (hex): " << std::endl;
    for (auto it = begin(decrypted_blocks); it != end(decrypted_blocks); it++) {
        std::cout << "    " << bytes_to_hex(*it) << std::endl;
    }

    return 0;
}

