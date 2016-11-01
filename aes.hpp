#ifndef __AES_HPP__
#define __AES_HPP__

#include <cassert>
#include <vector>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "xor.hpp"

class aes {
	const byte sbox[256] = {
		0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
	};

	const byte rsbox[256] = {
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
	};

	const byte rcon[255] = {
		0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
		0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
		0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
		0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
		0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
		0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
		0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
		0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
		0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
		0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
		0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
		0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
		0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
		0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
		0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
		0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb
	};

	static constexpr int num_cols = 4;
	static constexpr int num_rounds = 10;
	static constexpr int key_num_words = 4;
	static constexpr int key_length = 16;

    const byte_vector key;

    typedef byte state_t[4][4];
    state_t* state;
    byte round_key[176];

public:
	aes(const byte_vector& key):key(key) {
        assert(key.size() == key_length);
    }

public:
    static constexpr size_t block_size = key_length;

    byte_vector encrypt(const byte_vector& input) {
        assert(input.size() == key_length);

        key_expansion();

        byte output[key_length];
        std::copy(begin(input), end(input), std::begin(output));

        cipher(output);

        byte_vector res(key_length);
        std::copy(std::begin(output), std::end(output), std::begin(res));

        return res;
    }

    byte_vector decrypt(const byte_vector& input) {
        assert(input.size() == key_length);

        key_expansion();

        byte output[key_length];
        std::copy(begin(input), end(input), std::begin(output));

        inv_cipher(output);

        byte_vector res(input.size());
        std::copy(std::begin(output), std::end(output), begin(res));

        return res;
    }

    inline void encrypt(const byte_vector& input, byte_vector& output) {
        output = encrypt(input);
    }

    inline void decrypt(const byte_vector& input, byte_vector& output) {
        output = decrypt(input);
    }

    template <typename Mode>
    byte_vector encrypt_buffer(Mode mode, const byte_vector& input) {
        assert(input.size() % key_length == 0);

        key_expansion();

        byte_vector res(input.size());

        mode.encrypt_buffer(*this, begin(input), end(input), begin(res), end(res));

        return res;
    }

    template <typename Mode>
    byte_vector decrypt_buffer(Mode mode, const byte_vector& input) {
        assert(input.size() % key_length == 0);

        key_expansion();

        byte_vector res(input.size());

        mode.decrypt_buffer(*this, begin(input), end(input), begin(res), end(res));

        return res;
    }

private:
    void print_round_key() const {
        std::cout << "round key: ";
        for (auto it = std::begin(round_key); it != std::end(round_key); it++) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)*it;
        }
        std::cout << std::endl;
    }

    void print_state(int round) const {
        byte* bstate = (byte*)state;
        std::cout << std::dec << "round " << round << ": ";
        for (int i = 0; i < key_length; i += 1) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)bstate[i];
        }
        std::cout << std::endl;
    }

    void key_expansion() {
        byte tempa[4];
        int i = 0;

        for (; i < key_num_words; i += 1) {
            round_key[(i * 4) + 0] = key[(i * 4) + 0];
            round_key[(i * 4) + 1] = key[(i * 4) + 1];
            round_key[(i * 4) + 2] = key[(i * 4) + 2];
            round_key[(i * 4) + 3] = key[(i * 4) + 3];
        }

        for (; i < (num_cols * (num_rounds + 1)); i += 1) {
            for (int j = 0; j < 4; j += 1) {
                tempa[j] = round_key[(i - 1) * 4 + j];
            }

            if (i % key_num_words == 0) {
                std::rotate(std::begin(tempa), std::next(std::begin(tempa)), std::end(tempa));

                tempa[0] = sbox[tempa[0]];
                tempa[1] = sbox[tempa[1]];
                tempa[2] = sbox[tempa[2]];
                tempa[3] = sbox[tempa[3]];

                tempa[0] = tempa[0] ^ rcon[i / key_num_words];
            } else  if (key_num_words > 6 && i % key_num_words == 4) {
                tempa[0] = sbox[tempa[0]];
                tempa[1] = sbox[tempa[1]];
                tempa[2] = sbox[tempa[2]];
                tempa[3] = sbox[tempa[3]];
            }

            round_key[i * 4 + 0] = round_key[(i - key_num_words) * 4 + 0] ^ tempa[0];
            round_key[i * 4 + 1] = round_key[(i - key_num_words) * 4 + 1] ^ tempa[1];
            round_key[i * 4 + 2] = round_key[(i - key_num_words) * 4 + 2] ^ tempa[2];
            round_key[i * 4 + 3] = round_key[(i - key_num_words) * 4 + 3] ^ tempa[3];
        }
    }

    void add_round_key(byte round) {
        for (int i = 0; i < 4; i += 1) {
            for (int j = 0; j < 4; j += 1) {
                (*state)[i][j] ^= round_key[round * num_cols * 4 + i * num_cols + j];
            }
        }
    }

    void sub_bytes() {
        for (int i = 0; i < 4; i += 1) {
            for (int j = 0; j < 4; j += 1) {
                (*state)[j][i] = sbox[(*state)[j][i]];
            }
        }
    }

    void shift_rows() {
        byte temp;

        temp = (*state)[0][1];
        (*state)[0][1] = (*state)[1][1];
        (*state)[1][1] = (*state)[2][1];
        (*state)[2][1] = (*state)[3][1];
        (*state)[3][1] = temp;

        temp = (*state)[0][2];
        (*state)[0][2] = (*state)[2][2];
        (*state)[2][2] = temp;

        temp = (*state)[1][2];
        (*state)[1][2] = (*state)[3][2];
        (*state)[3][2] = temp;

        temp = (*state)[0][3];
        (*state)[0][3] = (*state)[3][3];
        (*state)[3][3] = (*state)[2][3];
        (*state)[2][3] = (*state)[1][3];
        (*state)[1][3] = temp;
    }

    byte xtime(byte x) {
        return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
    }

    void mix_columns() {
        byte tmp, tm, t;
        for (int i = 0; i < 4; i += 1) {
            t = (*state)[i][0];
            tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];

            tm = (*state)[i][0] ^ (*state)[i][1];
            tm = xtime(tm);
            (*state)[i][0] ^= tm ^ tmp;

            tm = (*state)[i][1] ^ (*state)[i][2];
            tm = xtime(tm);
            (*state)[i][1] ^= tm ^ tmp;

            tm = (*state)[i][2] ^ (*state)[i][3];
            tm = xtime(tm);
            (*state)[i][2] ^= tm ^ tmp;

            tm = (*state)[i][3] ^ t;
            tm = xtime(tm);
            (*state)[i][3] ^= tm ^ tmp;
        }
    }

    byte multiply(byte x, byte y) {
        return (((y & 1) * x) ^
            ((y >> 1 & 1) * xtime(x)) ^
            ((y >> 2 & 1) * xtime(xtime(x))) ^
            ((y >> 3 & 1) * xtime(xtime(xtime(x)))) ^
            ((y >> 4 & 1) * xtime(xtime(xtime(xtime(x))))));
    }

    void inv_mix_columns() {
        byte a, b, c, d;
        for (int i = 0; i < 4; i += 1) {
            a = (*state)[i][0];
            b = (*state)[i][1];
            c = (*state)[i][2];
            d = (*state)[i][3];

            (*state)[i][0] = multiply(a, 0x0e) ^ multiply(b, 0x0b) ^ multiply(c, 0x0d) ^ multiply(d, 0x09);
            (*state)[i][1] = multiply(a, 0x09) ^ multiply(b, 0x0e) ^ multiply(c, 0x0b) ^ multiply(d, 0x0d);
            (*state)[i][2] = multiply(a, 0x0d) ^ multiply(b, 0x09) ^ multiply(c, 0x0e) ^ multiply(d, 0x0b);
            (*state)[i][3] = multiply(a, 0x0b) ^ multiply(b, 0x0d) ^ multiply(c, 0x09) ^ multiply(d, 0x0e);
        }
    }

    void inv_sub_bytes() {
        for (int i = 0; i < 4; i += 1) {
            for (int j = 0; j < 4; j += 1) {
                (*state)[j][i] = rsbox[(*state)[j][i]];
            }
        }
    }

    void inv_shift_rows() {
        byte temp;

        temp = (*state)[3][1];
        (*state)[3][1] = (*state)[2][1];
        (*state)[2][1] = (*state)[1][1];
        (*state)[1][1] = (*state)[0][1];
        (*state)[0][1] = temp;

        temp = (*state)[0][2];
        (*state)[0][2] = (*state)[2][2];
        (*state)[2][2] = temp;

        temp = (*state)[1][2];
        (*state)[1][2] = (*state)[3][2];
        (*state)[3][2] = temp;

        temp = (*state)[0][3];
        (*state)[0][3] = (*state)[1][3];
        (*state)[1][3] = (*state)[2][3];
        (*state)[2][3] = (*state)[3][3];
        (*state)[3][3] = temp;
    }

    void cipher(byte output[]) {
        state = reinterpret_cast<state_t*>(output);

        add_round_key(0);

        for (byte round = 1; round < num_rounds; round += 1) {
            sub_bytes();
            shift_rows();
            mix_columns();
            add_round_key(round);
        }

        sub_bytes();
        shift_rows();
        add_round_key(num_rounds);
    }

    void inv_cipher(byte output[]) {
        state = reinterpret_cast<state_t*>(output);

        add_round_key(num_rounds);

        for (byte round = num_rounds - 1; round > 0; round -= 1) {
            inv_shift_rows();
            inv_sub_bytes();
            add_round_key(round);
            inv_mix_columns();
        }

        inv_shift_rows();
        inv_sub_bytes();
        add_round_key(0);
    }
};

template <size_t BlockSize>
struct ecb_mode {

    template <typename Cipher, typename It, typename It2>
    void encrypt_buffer(Cipher& cipher, It in_first, It in_last, It2 out_first, It2 out_last) const {

        size_t input_size = std::distance(in_first, in_last);
        size_t output_size = std::distance(out_first, out_last);

        assert(input_size == output_size);
        assert(output_size % BlockSize == 0);

        while (in_first != in_last) {

            byte_vector block_in(in_first, next(in_first, BlockSize));
            byte_vector block_out(BlockSize);

            cipher.encrypt(block_in, block_out);

            std::copy(begin(block_out), end(block_out), out_first);

            std::advance(in_first, BlockSize);
            std::advance(out_first, BlockSize);
        }
    }

    template <typename Cipher, typename It, typename It2>
    void decrypt_buffer(Cipher& cipher, It in_first, It in_last, It2 out_first, It2 out_last) const {
        size_t input_size = std::distance(in_first, in_last);
        size_t output_size = std::distance(out_first, out_last);

        assert(input_size == output_size);
        assert(output_size % BlockSize == 0);

        while (in_first != in_last) {

            byte_vector block_in(BlockSize);
            std::copy(in_first, next(in_first, BlockSize), begin(block_in));

            byte_vector block_out(BlockSize);

            cipher.decrypt(block_in, block_out);

            std::copy(begin(block_out), end(block_out), out_first);

            std::advance(in_first, BlockSize);
            std::advance(out_first, BlockSize);
        }
    }
};

template <size_t BlockSize>
struct cbc_mode {
    const byte_vector iv;

    cbc_mode(const byte_vector& iv_):iv(iv_) {}

    template <typename Cipher, typename It, typename It2>
    void encrypt_buffer(Cipher& cipher, It in_first, It in_last, It2 out_first, It2 out_last) const {
        size_t input_size = std::distance(in_first, in_last);
        size_t output_size = std::distance(out_first, out_last);

        assert(input_size == output_size);
        assert(output_size % BlockSize == 0);

        byte_vector current_iv = iv;

        while (in_first != in_last) {

            byte_vector block_in(BlockSize);
            std::copy(in_first, next(in_first, BlockSize), begin(block_in));

            block_in = block_in ^ current_iv;

            byte_vector block_out(BlockSize);

            cipher.encrypt(block_in, block_out);
            std::cout << block_out << std::endl;

            std::copy(begin(block_out), end(block_out), out_first);

            current_iv = block_out;
            std::cout << "current_iv: " << current_iv << std::endl;

            std::advance(in_first, BlockSize);
            std::advance(out_first, BlockSize);
        }
    }

    template <typename Cipher, typename It, typename It2>
    void decrypt_buffer(Cipher& cipher, It in_first, It in_last, It2 out_first, It2 out_last) const {
        size_t input_size = std::distance(in_first, in_last);
        size_t output_size = std::distance(out_first, out_last);

        assert(input_size == output_size);
        assert(output_size % BlockSize == 0);

        byte_vector current_iv = iv;

        while (in_first != in_last) {

            byte_vector block_in(BlockSize);
            std::copy(in_first, next(in_first, BlockSize), begin(block_in));

            byte_vector block_out(BlockSize);

            cipher.decrypt(block_in, block_out);

            block_out = block_out ^ current_iv;
            current_iv = block_in;

            std::copy(begin(block_out), end(block_out), out_first);

            std::advance(in_first, BlockSize);
            std::advance(out_first, BlockSize);
        }
    }
};

byte_vector zero_iv(size_t block_size = aes::block_size) {
    return byte_vector(block_size, 0);
}

byte_vector aes_ecb_encrypt(const byte_vector& input, const byte_vector& key) {
    return aes(key).encrypt_buffer(ecb_mode<aes::block_size>(), input);
}

byte_vector aes_ecb_decrypt(const byte_vector& input, const byte_vector& key) {
    return aes(key).decrypt_buffer(ecb_mode<aes::block_size>(), input);
}

byte_vector aes_cbc_encrypt(const byte_vector& input, const byte_vector& key, const byte_vector& iv = zero_iv()) {
    return aes(key).encrypt_buffer(cbc_mode<aes::block_size>(iv), input);
}

byte_vector aes_cbc_decrypt(const byte_vector& input, const byte_vector& key, const byte_vector& iv = zero_iv()) {
    return aes(key).decrypt_buffer(cbc_mode<aes::block_size>(iv), input);
}

#endif//__AES_HPP__
