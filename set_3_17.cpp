#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "aes.hpp"
#include "random_bytes.hpp"
#include "padding.hpp"
#include "base64.hpp"

namespace {
    std::vector<std::string> rand_strings = {
		"MDAwMDAwTm93IHRoYXQgdGhlIHBhcnR5IGlzIGp1bXBpbmc="s,
		"MDAwMDAxV2l0aCB0aGUgYmFzcyBraWNrZWQgaW4gYW5kIHRoZSBWZWdhJ3MgYXJlIHB1bXBpbic="s,
		"MDAwMDAyUXVpY2sgdG8gdGhlIHBvaW50LCB0byB0aGUgcG9pbnQsIG5vIGZha2luZw=="s,
		"sMDAwMDAzQ29va2luZyBNQydzIGxpa2UgYSBwb3VuZCBvZiBiYWNvbg=="s,
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

void test1() {
    auto enc = random_encrypt();

    std::cout << "ciphertext: " << bytes_to_hex(enc.first) << std::endl;
    std::cout << "iv: " << bytes_to_hex(enc.second) << std::endl;

    std::cout << "valid: " << std::boolalpha << decryption_oracle(enc) << std::endl;
}

int main() {
    test1();
    return 0;
}

