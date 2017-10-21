#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <ctime>

#include "mt19937.hpp"

using namespace std::chrono_literals;

namespace {
    std::chrono::seconds random_sleep_minimum = 5s;// 40s;
    std::chrono::seconds random_sleep_maximum = 40s;// 1000s;

    std::mt19937 gen((std::random_device())());
}

std::chrono::seconds random_sleep_time() {
    std::uniform_int_distribution<int> dist(random_sleep_minimum.count(), random_sleep_maximum.count());
    int t = dist(gen);
    return std::chrono::seconds(t);
}

void random_sleep() {
    auto t = random_sleep_time();
    std::cout << "sleeping for " << t.count() << "s" << std::endl;
    std::this_thread::sleep_for(t);
}

int current_timestamp() {
    return (int)std::time(nullptr);
}

int main() {
    random_sleep();
    auto ct = current_timestamp();
    std::cout << "seed: " << ct << std::endl;
    mt19937 my_random(ct);
    random_sleep();

    uint32_t random_first_value = my_random.extract_number();
    std::cout << "random first value: " << random_first_value << std::endl;

    auto now = current_timestamp();
    mt19937 test_random;
    for (int i = now - (random_sleep_maximum.count()); i < now; i += 1) {
        auto test_seed = i;

        test_random.seed_mt(test_seed);
        uint32_t test_value = test_random.extract_number();

        if (test_value == random_first_value) {
            auto diff = now - test_seed;
            std::cout << "Found seed: " << test_seed << std::endl;
            std::cout << "Seed generated " << diff << "s ago" << std::endl;
        }
    }

    return 0;
}

