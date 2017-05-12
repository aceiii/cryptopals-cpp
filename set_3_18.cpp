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
    const std::string ciphertext = "L77na/nrFsKvynd6HzOoG7GHTLXsTVu9qvY/2syLXzhPweyyMTJULu/6/kXX0KSvoOLSFQ==";
    const std::string key = "YELLOW SUBMARINE";
    const int nonce = 0;
}

int main() {

    byte_vector input = base64_to_bytes(ciphertext);
    byte_vector key_bytes = str_to_bytes(key);
    byte_vector decrypted = aes_ctr_decrypt(input, key_bytes, nonce);

    std::string text = bytes_to_str(decrypted);

    std::cout << "Decrypted: " << text << std::endl;

    byte_vector enc_bytes = aes(key_bytes).encrypt_buffer(ctr_mode<aes::block_size>(nonce), decrypted);

    std::cout << "Encrypted: " << bytes_to_base64(enc_bytes) << std::endl;

    std::cout << "Match: " << std::boolalpha << (enc_bytes == input) << std::endl;

    return 0;
}

