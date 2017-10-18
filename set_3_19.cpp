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
    std::vector<std::string> random_strings = {
        "SSBoYXZlIG1ldCB0aGVtIGF0IGNsb3NlIG9mIGRheQ==",
        "Q29taW5nIHdpdGggdml2aWQgZmFjZXM=",
        "RnJvbSBjb3VudGVyIG9yIGRlc2sgYW1vbmcgZ3JleQ==",
        "RWlnaHRlZW50aC1jZW50dXJ5IGhvdXNlcy4=",
        "SSBoYXZlIHBhc3NlZCB3aXRoIGEgbm9kIG9mIHRoZSBoZWFk",
        "T3IgcG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
        "T3IgaGF2ZSBsaW5nZXJlZCBhd2hpbGUgYW5kIHNhaWQ=",
        "UG9saXRlIG1lYW5pbmdsZXNzIHdvcmRzLA==",
        "QW5kIHRob3VnaHQgYmVmb3JlIEkgaGFkIGRvbmU=",
        "T2YgYSBtb2NraW5nIHRhbGUgb3IgYSBnaWJl",
        "VG8gcGxlYXNlIGEgY29tcGFuaW9u",
        "QXJvdW5kIHRoZSBmaXJlIGF0IHRoZSBjbHViLA==",
        "QmVpbmcgY2VydGFpbiB0aGF0IHRoZXkgYW5kIEk=",
        "QnV0IGxpdmVkIHdoZXJlIG1vdGxleSBpcyB3b3JuOg==",
        "QWxsIGNoYW5nZWQsIGNoYW5nZWQgdXR0ZXJseTo=",
        "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4=",
        "VGhhdCB3b21hbidzIGRheXMgd2VyZSBzcGVudA==",
        "SW4gaWdub3JhbnQgZ29vZCB3aWxsLA==",
        "SGVyIG5pZ2h0cyBpbiBhcmd1bWVudA==",
        "VW50aWwgaGVyIHZvaWNlIGdyZXcgc2hyaWxsLg==",
        "V2hhdCB2b2ljZSBtb3JlIHN3ZWV0IHRoYW4gaGVycw==",
        "V2hlbiB5b3VuZyBhbmQgYmVhdXRpZnVsLA==",
        "U2hlIHJvZGUgdG8gaGFycmllcnM/",
        "VGhpcyBtYW4gaGFkIGtlcHQgYSBzY2hvb2w=",
        "QW5kIHJvZGUgb3VyIHdpbmdlZCBob3JzZS4=",
        "VGhpcyBvdGhlciBoaXMgaGVscGVyIGFuZCBmcmllbmQ=",
        "V2FzIGNvbWluZyBpbnRvIGhpcyBmb3JjZTs=",
        "SGUgbWlnaHQgaGF2ZSB3b24gZmFtZSBpbiB0aGUgZW5kLA==",
        "U28gc2Vuc2l0aXZlIGhpcyBuYXR1cmUgc2VlbWVkLA==",
        "U28gZGFyaW5nIGFuZCBzd2VldCBoaXMgdGhvdWdodC4=",
        "VGhpcyBvdGhlciBtYW4gSSBoYWQgZHJlYW1lZA==",
        "QSBkcnVua2VuLCB2YWluLWdsb3Jpb3VzIGxvdXQu",
        "SGUgaGFkIGRvbmUgbW9zdCBiaXR0ZXIgd3Jvbmc=",
        "VG8gc29tZSB3aG8gYXJlIG5lYXIgbXkgaGVhcnQs",
        "WWV0IEkgbnVtYmVyIGhpbSBpbiB0aGUgc29uZzs=",
        "SGUsIHRvbywgaGFzIHJlc2lnbmVkIGhpcyBwYXJ0",
        "SW4gdGhlIGNhc3VhbCBjb21lZHk7",
        "SGUsIHRvbywgaGFzIGJlZW4gY2hhbmdlZCBpbiBoaXMgdHVybiw=",
        "VHJhbnNmb3JtZWQgdXR0ZXJseTo=",
        "QSB0ZXJyaWJsZSBiZWF1dHkgaXMgYm9ybi4=",
    };

    const byte_vector random_key = random_bytes().next_n(aes::block_size);
    const uint64_t nonce = 0;
}

int main() {

    std::vector<byte_vector> ciphertexts;
    std::size_t max_length = 0;

    for (const std::string &str : random_strings) {
        byte_vector input_bytes = base64_to_bytes(str);
        max_length = std::max(max_length, input_bytes.size());
        byte_vector encrypted = aes_ctr_encrypt(input_bytes, random_key, nonce);

        ciphertexts.push_back(encrypted);

        //std::cout << input_bytes.size() << std::endl;
    }

    //return 0;

    // sort random_strings
    std::vector<int> ciphertext_order(ciphertexts.size());
    std::iota(ciphertext_order.begin(), ciphertext_order.end(), 0);

    std::sort(ciphertext_order.begin(), ciphertext_order.end(), [&ciphertexts] (const int &b1, const int &b2) {
        return ciphertexts[b1].size() > ciphertexts[b2].size();
    });

    std::vector<byte_vector> original_ciphertexts(ciphertexts.begin(), ciphertexts.end());
    for (int i = 0; i < ciphertext_order.size(); i += 1) {
        ciphertexts[i] = original_ciphertexts[ciphertext_order[i]];
    }

    std::cout << "max_length: " << max_length << std::endl;

    // CIPHERTEXT-BYTE XOR PLAINTEXT-BYTE = KEYSTREAM-BYTE
    // thereforce:
    // CIPHERTEXT-BYTE XOR KEYSTREAM-BYTE = PLAINTEXT-BYTE

    std::vector<byte_vector> dec_strings(random_strings.size());

    //byte_vector first_string;

    // Manually tweak incorrect bytes to solve rest
    byte_vector first_string = str_to_bytes("He, too, has been changed in his turn.");

    for (int i = 0; i < max_length; i += 1) {
        bool match = false;

        if (i < first_string.size()) {
            byte b = first_string[i];
            byte key = ciphertexts[0][i] ^ b;

            byte_vector other_bytes;
            for (int k = 1; k < ciphertexts.size(); k += 1) {
                if (i < ciphertexts[k].size()) {
                    other_bytes.push_back(ciphertexts[k][i]);
                }
            }

            byte_vector dec_bytes = other_bytes ^ key;

            dec_strings[0].push_back(b);
            for (int m = 0; m < dec_bytes.size(); m += 1) {
                dec_strings[1 + m].push_back(dec_bytes[m]);
            }

            continue;
        }

        for (int j = 0; j < 256; j += 1) {

            byte b = (byte) j;

            byte key = ciphertexts[0][i] ^ b; // key-stream byte = ciphertext-byte ^ plaintext-byte
            byte_vector other_bytes;
            for (int k = 1; k < ciphertexts.size(); k += 1) {
                if (i < ciphertexts[k].size()) {
                    other_bytes.push_back(ciphertexts[k][i]);
                }
            }

            byte_vector dec_bytes = other_bytes ^ key;
            if (all_of(dec_bytes.begin(), dec_bytes.end(), [] (const byte &db) {
                return isdigit(db) || isalpha(db) || db == ' ' || db == '.' || db == '?' || db == ',' || db == '-' || db == '=' || db == '!' || db == ';' || db == ':' || db == '_' || db == '\'';
                //return db > 32 && db <= 127;
            })) {
                //std::cout << b << bytes_to_str(dec_bytes) <<  std::endl;
                dec_strings[0].push_back(b);
                for (int m = 0; m < dec_bytes.size(); m += 1) {
                    dec_strings[1 + m].push_back(dec_bytes[m]);
                }

                break;
            }
        }
    }

    std::vector<byte_vector> reodered(ciphertext_order.size());
    for (int i = 0; i < ciphertext_order.size(); i += 1) {
        for (int j = 0; j < ciphertext_order.size(); j += 1) {
            if (i == ciphertext_order[j]) {
                reodered[i] = dec_strings[j];
                break;
            }
        }
    }

    for (auto &ds : reodered) {
        std::cout << ds.size() << ": " << bytes_to_str(ds) << std::endl;
    }

    /*
    for (const auto &ds : dec_strings) {
        std::cout << ds.size() << ": " << bytes_to_str(ds) << std::endl;
    }
    */

    return 0;
}
