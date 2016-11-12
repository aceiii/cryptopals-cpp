#ifndef __RANDOM_INT_HPP__
#define __RANDOM_INT_HPP__

#include <random>

template <typename T>
class random_int
{
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<T> dist;

public:
    random_int(T lower, T upper):
        rd(),gen(rd()),
        dist(lower, upper)
    {
    }

    T next() {
        return dist(gen);
    }

    std::vector<T> next_n(size_t len) {
        std::vector<T> result(len);
        std::generate(begin(result), end(result), std::bind(&random_int::next, this));

        return result;
    }
};

#endif//__RANDOM_INT_HPP__
