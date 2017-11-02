#include <iostream>
#include <cassert>
#include <random>
#include <ctime>
#include <thread>

#include "xor.hpp"
#include "byte.hpp"
#include "byte_util.hpp"
#include "byte_vector.hpp"
#include "mt19937.hpp"
#include "mt19937_stream.hpp"

using namespace std::literals::string_literals;

namespace {
    constexpr uint32_t min_random_prefix_length = 16;
    constexpr uint32_t max_random_prefix_length = 1024;
}

byte_vector mt19937_stream_encrypt(uint32_t key, const byte_vector &bytes) {
    mt19937_stream mts(key);
    std::uniform_int_distribution<uint32_t> dis(min_random_prefix_length, max_random_prefix_length);

    auto rand_prefix_size = dis(mts);
    auto total_size = rand_prefix_size + bytes.size();

    auto rand_prefix = mts.next_bytes(rand_prefix_size);

    std::copy(bytes.begin(), bytes.end(), std::back_inserter(rand_prefix));

    return rand_prefix ^ mts.next_bytes(total_size);
}

byte_vector mt19937_stream_decrypt(uint32_t key, const byte_vector &bytes) {
    byte_vector recovered_plaintext;

    mt19937_stream mts(key);
    std::uniform_int_distribution<uint32_t> dis(min_random_prefix_length, max_random_prefix_length);

    auto rand_prefix_size = dis(mts);
    auto total_size = bytes.size();
    auto random_prefix = mts.next_bytes(rand_prefix_size);
    auto decrypted = bytes ^ mts.next_bytes(total_size);

    std::copy(decrypted.begin() + rand_prefix_size, decrypted.end(), std::back_inserter(recovered_plaintext));

    return recovered_plaintext;
}

uint32_t discover_mt19937_stream_key(const byte_vector &ciphertext, const byte_vector &plaintext, uint32_t real_key) {
    uint32_t key = 0;
    int total_size = ciphertext.size();
    int random_length = ciphertext.size() - plaintext.size();
    mt19937_stream mts;

    for (int i = 0; i < 65536; i += 1) {
        key = (uint32_t) i;
        mts.seed_mt(key);
        mts.extract_number();

        auto random_prefix = mts.next_bytes(random_length);
        std::copy(plaintext.begin(), plaintext.end(), std::back_inserter(random_prefix));

        auto decrypted = ciphertext ^ mts.next_bytes(total_size);

        if (decrypted == random_prefix) {
            break;
        }
    }

    return key;
}

byte_vector generate_password_token(uint32_t key) {
    const size_t random_password_length = 24;
    mt19937_stream mts(key);

    auto random_password = mts.next_bytes(random_password_length);

    std::uniform_int_distribution<uint32_t> dis(1, 1024);
    auto random_prefix = mts.next_bytes(dis(mts));

    std::copy(random_password.begin(), random_password.end(), std::back_inserter(random_prefix));

    return random_prefix ^ mts.next_bytes(random_prefix.size());
}

byte_vector generate_random_password_token() {
    auto current_timestamp = time(nullptr);
    return generate_password_token(current_timestamp);
}

bool validate_password_token(const byte_vector &token) {
    const auto expiration_seconds = 2880;
    auto current_time = time(nullptr);

    for (int i = 0; i < expiration_seconds; i += 1) {
        auto t = current_time - i;
        auto new_token = generate_password_token(t);

        if (new_token == token) {
            return true;
        }
    }

    return false;
}

int main() {
    std::random_device rd;
    mt19937_stream mts(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 65535);

    auto random_key = dis(mts);
    auto known_string = "Hello world, this is a test!"s;
    auto known_string_bytes = str_to_bytes(known_string);
    auto encrypted_bytes = mt19937_stream_encrypt(random_key, known_string_bytes);
    auto decrypted_bytes = mt19937_stream_decrypt(random_key, encrypted_bytes);

    std::cout << "random_key: " << random_key << std::endl;
    std::cout << "known_string: " << known_string << std::endl;
    std::cout << "decrypted string: " << bytes_to_str(decrypted_bytes) << std::endl;

    auto recovered_key = discover_mt19937_stream_key(encrypted_bytes, known_string_bytes, random_key);
    std::cout << "recovered_key: " << recovered_key << std::endl;

    auto token = generate_random_password_token();
    auto random_token = mts.next_bytes(1024);

    std::uniform_int_distribution<> dis2(2, 20);
    auto random_sleep_time = std::chrono::seconds(dis2(mts));
    std::this_thread::sleep_for(random_sleep_time);

    auto is_valid = validate_password_token(token);
    auto is_valid2 = validate_password_token(random_token);

    std::cout << "random password token: " << bytes_to_base64(token) << std::endl;
    std::cout << "is valid: " << std::boolalpha << is_valid << std::endl;
    std::cout << "is random valid: " << std::boolalpha << is_valid2 << std::endl;

    return 0;
}

