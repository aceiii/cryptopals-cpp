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

template <size_t BlockSize>
struct ctr_mode {

    const uint64_t nonce;

    ctr_mode(const uint64_t nonce_):nonce(nonce_) {}

    template <typename Cipher, typename In_it, typename Out_it>
    void encrypt_buffer(Cipher &cipher, In_it in_first, In_it in_last, Out_it out_first, Out_it out_last) const {

        size_t input_size = std::distance(in_first, in_last);
        size_t output_size = std::distance(out_first, out_last);

        assert(input_size == output_size);

        size_t num_blocks = output_size / BlockSize;

        uint64_t counter = 0;

        for (int i = 0; i <= num_blocks; i += 1) {
            byte_vector block_in(BlockSize);
            std::memcpy(block_in.data(), &nonce, sizeof(nonce));
            std::memcpy(block_in.data() + sizeof(nonce), &counter, sizeof(counter));

            byte_vector block_out(BlockSize);

            cipher.encrypt(block_in, block_out);

            for (int j = 0; j < BlockSize; j += 1) {
                if (out_first + j == out_last) {
                    break;
                }

                *(out_first + j) = *(in_first + j) ^ block_out[j];
            }

            std::advance(in_first, BlockSize);
            std::advance(out_first, BlockSize);

            counter += 1;
        }
    }

    template <typename Cipher, typename In_it, typename Out_it>
    void decrypt_buffer(Cipher &cipher, In_it in_first, In_it in_last, Out_it out_first, Out_it out_last) const {
        encrypt_buffer(cipher, in_first, in_last, out_first, out_last);
    }
};

int main() {

    byte_vector input = base64_to_bytes(ciphertext);
    byte_vector key_bytes = str_to_bytes(key);
    byte_vector decrypted = aes(key_bytes).decrypt_buffer(ctr_mode<aes::block_size>(nonce), input);

    std::string text = bytes_to_str(decrypted);

    std::cout << "Decrypted: " << text << std::endl;

    byte_vector enc_bytes = aes(key_bytes).encrypt_buffer(ctr_mode<aes::block_size>(nonce), decrypted);

    std::cout << "Encrypted: " << bytes_to_base64(enc_bytes) << std::endl;

    std::cout << "Match: " << std::boolalpha << (enc_bytes == input) << std::endl;

    return 0;
}

