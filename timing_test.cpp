#include <iostream>
#include "bitvector.cpp"
#include <vector>
#include <random>
#include <chrono>
#include <functional>
#include <bitset>

static std::random_device rd;
static std::default_random_engine gen(rd());
static std::uniform_int_distribution<int> dis (0, 1);

constexpr int size = 10000000;

using t = std::chrono::high_resolution_clock;

long measure_time (const std::function<size_t ()> &f) {
    const auto s = t::now();
    const auto r = f();
    const auto e = t::now();
    std::cout << "output: " << r << '\n';
    return std::chrono::duration_cast<std::chrono::milliseconds>(e-s).count();
}

int main (void) {
    Bitvector b (size);
    std::vector<bool> v (size);
    std::bitset<size> s;
    
    for (int i = 0; i < size; i++) {
        b.push_back(dis(gen));
        v.push_back(dis(gen));
        s[i] = dis(gen);
    }
    b.print();
    
    const auto t = measure_time( [&](){
        return b.rank(size);
    } );
    const auto y = measure_time( [&](){
        return std::count(v.begin(), v.end(), true);
    } );
    const auto bsc = measure_time( [&](){
        return s.count();
    } );
    std::cout << "custom bitvector: " << t << "ms\n"
            << "vector<bool>: " << y << "ms\n"
            << "bitset: " << bsc << "ms\n";
}
