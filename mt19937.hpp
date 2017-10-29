#ifndef __MT19937_HPP__
#define __MT19937_HPP__

#include <cstring>
#include <cassert>

class mt19937 {
protected:
    // coefficients for mt19937-32
    static const uint32_t w = 32;
    static const uint32_t n = 624;
    static const uint32_t m = 397;
    static const uint32_t r = 31;
    static const uint32_t a = 0x9908B0DF;
    static const uint32_t u = 11;
    static const uint32_t d = 0xFFFFFFFF;
    static const uint32_t s = 7;
    static const uint32_t b = 0x9D2C5680;
    static const uint32_t t = 15;
    static const uint32_t c = 0xEFC60000;
    static const uint32_t l = 18;
    static const uint32_t f = 1812433253;

    const uint32_t lower_mask = (1ull << r) - 1;
    const uint32_t upper_mask = ~lower_mask;

    uint32_t _mt[n];
    uint32_t _index;

public:
    static const uint32_t default_seed = 5489u;

    mt19937(uint32_t seed = default_seed) {
        seed_mt(seed);
    }

    void seed_mt(uint32_t seed) {
        _index = n;
        _mt[0] = seed;

        for (int i = 1; i < n; i += 1) {
            _mt[i] = (f * (_mt[i-1] ^ (_mt[i-1] >> (w-2))) + i);
        }
    }

    uint32_t extract_number() {
        if (_index >= n) {
            twist();
        }

        uint32_t y = _mt[_index];
        y ^= (y >> u) & d;
        y ^= (y << s) & b;
        y ^= (y << t) & c;
        y ^= (y >> l);

        _index += 1;

        return y;
    }

    void twist() {
        for (int i = 0; i < n; i += 1) {
            uint32_t x = (_mt[i] & upper_mask) + (_mt[(i+1) % n] & lower_mask);
            uint32_t xA = x >> 1;

            if (x % 2 != 0) {
                xA = xA ^ a;
            }

            _mt[i] = _mt[(i+m) % n] ^ xA;
        }
        _index = 0;
    }

};

#endif//__MT19937_HPP__

