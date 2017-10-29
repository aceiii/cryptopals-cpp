#include <iostream>
#include <random>

#include "mt19937.hpp"

// https://gist.github.com/46bit/8346702
class mt19937_cloner : public mt19937 {
public:
    void set_index(uint32_t index) {
        assert(index <= n);

        _index = index;
    }

    void set_state(uint32_t index, uint32_t state) {
        assert(index < n);

        auto s = invert_result(state);
        _mt[index] = s;
    }

protected:
    uint32_t invert_result(uint32_t number) {
        // https://www.randombit.net/bitbashing/2009/07/21/inverting_mt19937_tempering.html
        auto y = number;
        y = y ^ (y >> 18);
        y = y ^ ((y << 15) & 0xEFC60000);
        y = y ^ ((y << 7) & 0x1680);
        y = y ^ ((y << 7) & 0xC4000);
        y = y ^ ((y << 7) & 0xD200000);
        y = y ^ ((y << 7) & 0x90000000);
        y = y ^ ((y >> 11) & 0xFFC00000);
        y = y ^ ((y >> 11) & 0x3FF800);
        y = y ^ ((y >> 11) & 0x7FF);

        return y;
    }

    uint32_t invert_result2(uint32_t number) {
        auto y = number;
        y = revertRightShift(y, l);
        y = revertLeftShift(y, t, c);
        y = revertLeftShift(y, s, b);
        y = revertRightShift(y, u, d);
        return y;
    }

private:
    uint32_t revertRightShift(uint32_t number, uint32_t right_shift_amount, uint32_t mask = ~0) {
        //return number ^ (number >> right_shift_amount);
        auto y = number;

        for (int i = 0; i < 32; i += 1) {
            auto yp = mask & (1 << i);
            auto ya = ((y >> right_shift_amount) & yp);
            y = y ^ ya;
        }

        return y;
    }

    uint32_t revertLeftShift(uint32_t number, uint32_t left_shift_amount, uint32_t mask) {
        //return number ^ ((number << left_shift_amount) & mask);

        auto y = number;

        for (int i = 0; i < 32; i += 1) {
            auto yp = mask & (1 << i);
            auto ya = ((y << left_shift_amount)  & yp);
            y = y ^ ya;
        }

        return y;
    }
};

int main() {
    const int N = 2; //624;
    const int M = 1024;
    const int P = 10;
    std::random_device rd;

    auto seed = rd();
    std::cout << "seed: " << seed << std::endl;

    mt19937 mt1(seed);
    mt19937_cloner mt2;
    for (int i = 0; i < N; i += 1) {
        auto r = mt1.extract_number();
        mt2.set_state(i, r);
    }
    mt2.set_index(N);

    return 0;

    for (int i = 0; i < M; i += 1) {
        auto a = mt1.extract_number();
        auto b = mt2.extract_number();

        if (i <= P || a != b) {
            std::cout << "(" << i << ") a: " << a << std::endl;
            std::cout << "(" << i << ") b: " << b << std::endl;
        }

        if (a != b) {
            std::cout << "ERROR: a != b" << std::endl;
            return 1;
            break;
        }
    }

    std::cout << "SUCCESS!" << std::endl;
    return 0;
}

